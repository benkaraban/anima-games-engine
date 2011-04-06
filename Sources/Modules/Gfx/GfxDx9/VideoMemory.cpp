/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * Jérémie Comarmond, Didier Colin.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * - Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#define INITGUID
#include <windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <Gfx/GfxDx9/VideoMemory.h>
#include <Core/Logger.h>

// Code provenant de la SDK DirectX
// http://msdn.microsoft.com/en-us/library/ee419018%28VS.85%29.aspx

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

namespace Gfx
{
typedef HRESULT ( WINAPI* LPCREATEDXGIFACTORY )( REFIID, void** );

bool getVideoMemoryViaDXGI(HMONITOR hMonitor, int64 & dedicatedVideoMem, int64 & dedicatedSystemMem, int64 & sharedSystemMem)
{
    bool result = false;
    dedicatedVideoMem = 0;
    dedicatedSystemMem = 0;
    sharedSystemMem = 0;

    HINSTANCE hDXGI = LoadLibraryW(L"dxgi.dll");
    if(hDXGI)
    {
        LPCREATEDXGIFACTORY pCreateDXGIFactory = NULL;
        IDXGIFactory* pDXGIFactory = NULL;

        pCreateDXGIFactory = (LPCREATEDXGIFACTORY)GetProcAddress(hDXGI, "CreateDXGIFactory");
        pCreateDXGIFactory(__uuidof(IDXGIFactory), (LPVOID*)&pDXGIFactory);

        for(int32 index = 0; ; ++index)
        {
            bool bFoundMatchingAdapter = false;
            IDXGIAdapter * pAdapter = NULL;
            HRESULT hr = pDXGIFactory->EnumAdapters(index, &pAdapter);
            if(FAILED(hr)) // DXGIERR_NOT_FOUND is expected when the end of the list is hit
                break;

            for(int iOutput = 0; ; ++iOutput)
            {
                IDXGIOutput* pOutput = NULL;
                hr = pAdapter->EnumOutputs(iOutput, &pOutput);
                if(FAILED(hr)) // DXGIERR_NOT_FOUND is expected when the end of the list is hit
                    break;

                DXGI_OUTPUT_DESC outputDesc;
                ZeroMemory(&outputDesc, sizeof(DXGI_OUTPUT_DESC));
                if(SUCCEEDED(pOutput->GetDesc(&outputDesc)))
                {
                    if(hMonitor == outputDesc.Monitor)
                        bFoundMatchingAdapter = true;

                }
                SAFE_RELEASE(pOutput);
            }

            if(bFoundMatchingAdapter)
            {
                DXGI_ADAPTER_DESC desc;
                ZeroMemory(&desc, sizeof(DXGI_ADAPTER_DESC));
                if(SUCCEEDED(pAdapter->GetDesc(&desc)))
                {
                    result = true;
                    dedicatedVideoMem = desc.DedicatedVideoMemory;
                    dedicatedSystemMem = desc.DedicatedSystemMemory;
                    sharedSystemMem = desc.SharedSystemMemory;
                }
                break;
            }
        }

        FreeLibrary( hDXGI );
    }

    return result;
}

HRESULT GetDeviceIDFromHMonitor( HMONITOR hm, const char * strDriverName, WCHAR* strDeviceID, int cchDeviceID )
{
    LONG iDevice = 0;
    DISPLAY_DEVICEA dispdev;

    ZeroMemory( &dispdev, sizeof( dispdev ) );
    dispdev.cb = sizeof( dispdev );

    while( EnumDisplayDevicesA( NULL, iDevice, ( DISPLAY_DEVICEA* )&dispdev, 0 ) )
    {
        // Skip devices that are monitors that echo another display
        if( dispdev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER )
        {
            iDevice++;
            continue;
        }

        // Skip devices that aren't attached since they cause problems
        if( ( dispdev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0 )
        {
            iDevice++;
            continue;
        }

        if( _stricmp( strDriverName, dispdev.DeviceName ) == 0 )
        {
            MultiByteToWideChar( CP_ACP, 0, dispdev.DeviceID, -1, strDeviceID, cchDeviceID );
            return S_OK;
        }

        iDevice++;
    }

    return E_FAIL;
}


typedef BOOL ( WINAPI* PfnCoSetProxyBlanket )( IUnknown* pProxy, DWORD dwAuthnSvc, DWORD dwAuthzSvc,
                                               OLECHAR* pServerPrincName, DWORD dwAuthnLevel, DWORD dwImpLevel,
                                               RPC_AUTH_IDENTITY_HANDLE pAuthInfo, DWORD dwCapabilities );

bool getVideoMemoryViaWMI(HMONITOR hMonitor, const char * deviceID, int64 & videoMem)
{
    HRESULT hr;
    bool result = false;
    HRESULT hrCoInitialize = S_OK;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemServices* pIWbemServices = NULL;
    BSTR pNamespace = NULL;
    wchar_t strInputDeviceID[1024];
    GetDeviceIDFromHMonitor(hMonitor, deviceID, strInputDeviceID, 1024);

    videoMem = 0;
    hrCoInitialize = CoInitialize( 0 );

    hr = CoCreateInstance( CLSID_WbemLocator,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IWbemLocator,
                           ( LPVOID* )&pIWbemLocator );

    if( SUCCEEDED( hr ) && pIWbemLocator )
    {
        // Using the locator, connect to WMI in the given namespace.
        pNamespace = SysAllocString( L"\\\\.\\root\\cimv2" );

        hr = pIWbemLocator->ConnectServer( pNamespace, NULL, NULL, 0L,
                                           0L, NULL, NULL, &pIWbemServices );
        if( SUCCEEDED( hr ) && pIWbemServices != NULL )
        {
            HINSTANCE hinstOle32 = NULL;

            hinstOle32 = LoadLibraryW( L"ole32.dll" );
            if( hinstOle32 )
            {
                PfnCoSetProxyBlanket pfnCoSetProxyBlanket = NULL;

                pfnCoSetProxyBlanket = ( PfnCoSetProxyBlanket )GetProcAddress( hinstOle32, "CoSetProxyBlanket" );
                if( pfnCoSetProxyBlanket != NULL )
                {
                    // Switch security level to IMPERSONATE. 
                    pfnCoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                                          RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );
                }

                FreeLibrary( hinstOle32 );
            }

            IEnumWbemClassObject* pEnumVideoControllers = NULL;
            BSTR pClassName = NULL;

            pClassName = SysAllocString( L"Win32_VideoController" );

            hr = pIWbemServices->CreateInstanceEnum( pClassName, 0,
                                                     NULL, &pEnumVideoControllers );

            if( SUCCEEDED( hr ) && pEnumVideoControllers )
            {
                IWbemClassObject* pVideoControllers[10] = {0};
                DWORD uReturned = 0;
                BSTR pPropName = NULL;

                // Get the first one in the list
                pEnumVideoControllers->Reset();
                hr = pEnumVideoControllers->Next( 5000,             // timeout in 5 seconds
                                                  10,                  // return the first 10
                                                  pVideoControllers,
                                                  &uReturned );

                VARIANT var;
                if( SUCCEEDED( hr ) )
                {
                    bool bFound = false;
                    for( UINT iController = 0; iController < uReturned; iController++ )
                    {
                        pPropName = SysAllocString( L"PNPDeviceID" );
                        hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );

                        if( SUCCEEDED( hr ) )
                        {
                            if( wcsstr( var.bstrVal, strInputDeviceID ) != 0 )
                                bFound = true;
                        }
                        VariantClear( &var );
                        if( pPropName ) SysFreeString( pPropName );

                        if( bFound )
                        {
                            pPropName = SysAllocString( L"AdapterRAM" );
                            hr = pVideoControllers[iController]->Get( pPropName, 0L, &var, NULL, NULL );

                            if( SUCCEEDED( hr ) )
                            {
                                result = true;
                                videoMem = var.ulVal;
                            }
                            VariantClear( &var );
                            if( pPropName ) SysFreeString( pPropName );
                            break;
                        }
                        SAFE_RELEASE( pVideoControllers[iController] );
                    }
                }
            }

            if( pClassName )
                SysFreeString( pClassName );
            SAFE_RELEASE( pEnumVideoControllers );
        }

        if( pNamespace )
            SysFreeString( pNamespace );
        SAFE_RELEASE( pIWbemServices );
    }

    SAFE_RELEASE( pIWbemLocator );

    if( SUCCEEDED( hrCoInitialize ) )
        CoUninitialize();

    return result;
}

}
