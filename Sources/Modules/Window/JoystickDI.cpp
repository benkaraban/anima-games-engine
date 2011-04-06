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
#include <Core/Logger.h>
#include <Core/Math/MathTools.h>
#include <Window/JoystickDI.h>
#include <dxerr.h>

namespace Window
{
struct EnumJoyContext
{
    void                         * hwnd;
    IDirectInput8                * pDI;
    Core::List<Ptr<IJoystick> >  * pJoysticks;
};

struct XInputDeviceNode
{
    DWORD               dwVidPid;
    XInputDeviceNode *  pNext;
};

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if((x) != NULL) { x->Release(); x = NULL; }
#endif

XInputDeviceNode * spXInputDeviceList = NULL;

//-----------------------------------------------------------------------------
HRESULT SetupForIsXInputDevice()
{
    IWbemServices* pIWbemServices = NULL;
    IEnumWbemClassObject* pEnumDevices = NULL;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemClassObject* pDevices[20] = {0};
    BSTR bstrDeviceID = NULL;
    BSTR bstrClassName = NULL;
    BSTR bstrNamespace = NULL;
    DWORD uReturned = 0;
    bool bCleanupCOM = false;
    UINT iDevice = 0;
    VARIANT var;
    HRESULT hr;

    // CoInit if needed
    hr = CoInitialize( NULL );
    bCleanupCOM = SUCCEEDED( hr );

    // Create WMI
    hr = CoCreateInstance( __uuidof( WbemLocator ),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof( IWbemLocator ),
                           ( LPVOID* )&pIWbemLocator );
    if( FAILED( hr ) || pIWbemLocator == NULL )
        goto LCleanup;

    // Create BSTRs for WMI
    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" ); if( bstrNamespace == NULL ) goto LCleanup;
    bstrDeviceID = SysAllocString( L"DeviceID" );           if( bstrDeviceID == NULL )  goto LCleanup;
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );    if( bstrClassName == NULL ) goto LCleanup;

    // Connect to WMI 
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED( hr ) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );

    // Get list of Win32_PNPEntity devices
    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
    if( FAILED( hr ) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for(; ; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED( hr ) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice = 0; iDevice < uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it�s an XInput device
                // Unfortunately this information can not be found by just using DirectInput 
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf_s( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf_s( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );

                    // Add the VID/PID to a linked list
                    XInputDeviceNode * pNewNode = new XInputDeviceNode;
                    if( pNewNode )
                    {
                        pNewNode->dwVidPid = dwVidPid;
                        pNewNode->pNext = spXInputDeviceList;
                        spXInputDeviceList = pNewNode;
                    }
                }
            }
            SAFE_RELEASE( pDevices[iDevice] );
        }
    }

LCleanup:
    if( bstrNamespace )
        SysFreeString( bstrNamespace );
    if( bstrDeviceID )
        SysFreeString( bstrDeviceID );
    if( bstrClassName )
        SysFreeString( bstrClassName );
    for( iDevice = 0; iDevice < 20; iDevice++ )
    SAFE_RELEASE( pDevices[iDevice] );
    SAFE_RELEASE( pEnumDevices );
    SAFE_RELEASE( pIWbemLocator );
    SAFE_RELEASE( pIWbemServices );

    return hr;
}
//-----------------------------------------------------------------------------
bool IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
    // Check each xinput device to see if this device's vid/pid matches
    XInputDeviceNode * pNode = spXInputDeviceList;
    while(pNode)
    {
        if( pNode->dwVidPid == pGuidProductFromDirectInput->Data1 )
            return true;
        pNode = pNode->pNext;
    }

    return false;
}
//-----------------------------------------------------------------------------
void CleanupForIsXInputDevice()
{
    XInputDeviceNode * pNode = spXInputDeviceList;
    while(pNode != NULL)
    {
        XInputDeviceNode * pDelete = pNode;
        pNode = pNode->pNext;
        delete pDelete;
    }
}
//-----------------------------------------------------------------------------
BOOL CALLBACK enumEffectsCallback(const DIEFFECTINFO * pEI, void * pContext)
{
    return DIENUM_CONTINUE;
}
//-----------------------------------------------------------------------------
BOOL CALLBACK enumJoysticksCallback(const DIDEVICEINSTANCE * pInst, void * pContext)
{
    EnumJoyContext &        context = *((EnumJoyContext*)pContext);
    IDirectInputDevice8 *   pDevice = NULL;
    String                  name(String8(pInst->tszInstanceName));

    if(IsXInputDevice(&pInst->guidProduct))
    {
        INF << L"Detected DirectInput device seems to be a XInput device, we ignore it\n";
        return DIENUM_CONTINUE;
    }

    HRESULT hr = context.pDI->CreateDevice(pInst->guidInstance, &pDevice, NULL);

    if(FAILED(hr))
        ERR << L"Error creating DirectInput device for joystick " << name << L" : " << DXGetErrorString(hr) << L"\n";
    else
    {
        hr = pDevice->SetDataFormat(&c_dfDIJoystick2);

        if(FAILED(hr))
        {
            ERR << L"Error setting data format for DirectInput joystick " << name << L" : " << DXGetErrorString(hr) << L"\n";
            return DIENUM_CONTINUE;
        }
    
        hr = pDevice->SetCooperativeLevel((HWND)context.hwnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);

        if(FAILED(hr))
        {
            ERR << L"Error setting cooperative level for DirectInput joystick " << name << L" : " << DXGetErrorString(hr) << L"\n";
            return DIENUM_CONTINUE;
        }

        hr = pDevice->EnumEffects(enumEffectsCallback, NULL, DIEFT_ALL );

        if(FAILED(hr))
        {
            ERR << L"Error ernumerating effects for DirectInput joystick " << name << L" : " << DXGetErrorString(hr) << L"\n";
            return DIENUM_CONTINUE;
        }

        INF << L"Detected joystick " << name << L"\n";
        context.pJoysticks->push_back(Ptr<JoystickDI>(new JoystickDI(pDevice, name)));
    }

    return DIENUM_CONTINUE;
}
//-----------------------------------------------------------------------------
void JoystickDI::enumerateJoysticks(Core::List<Ptr<IJoystick> > & joysticks, void * hwnd)
{
    SetupForIsXInputDevice();

    EnumJoyContext context;
    context.hwnd       = hwnd;
    context.pDI        = NULL;
    context.pJoysticks = &joysticks;

    HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&context.pDI, NULL);

    if(FAILED(hr))
    {
        ERR << L"Error initialising DirectInput : " << DXGetErrorString(hr) << L"\n";
        return;
    }

    hr = context.pDI->EnumDevices(DI8DEVCLASS_GAMECTRL, enumJoysticksCallback, &context, DIEDFL_ATTACHEDONLY);

    if(FAILED(hr))
        ERR << L"Error enumerating DirectInput joysticks : " << DXGetErrorString(hr) << L"\n";

    context.pDI->Release();

    CleanupForIsXInputDevice();
}
//-----------------------------------------------------------------------------
JoystickDI::JoystickDI(IDirectInputDevice8 * pDevice, const String & name) :
    _pDevice(pDevice),
    _name(name),
    _pShakeLoop(NULL),
    _iShake(0)
{
    _buttonDown.resize(128, false);
    _buttonTyped.resize(128, false);


    // Force feedback

    DWORD rgdwAxes[2]    = { DIJOFS_X, DIJOFS_Y };
    LONG rglDirection[2] = { 10000, 10000 };
    DICONSTANTFORCE cf   = { 10000 };
    DIRAMPFORCE rf       = { 10000, 10000 };

    DIEFFECT effect;
    ZeroMemory(&effect, sizeof(effect));
    effect.dwSize = sizeof(DIEFFECT);
    effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
    effect.dwSamplePeriod = 0;
    effect.dwGain = DI_FFNOMINALMAX;
    effect.dwTriggerButton = DIEB_NOTRIGGER;
    effect.dwTriggerRepeatInterval = 0;
    effect.cAxes = 2;
    effect.rgdwAxes = rgdwAxes;
    effect.rglDirection = rglDirection;
    effect.lpEnvelope = 0;
    effect.dwStartDelay = 0;
    
    effect.dwDuration = INFINITE;
    effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    effect.lpvTypeSpecificParams = &cf;
    HRESULT hr = _pDevice->CreateEffect(GUID_ConstantForce, &effect, &_pShakeLoop, NULL);


    effect.dwDuration = 500000;
    effect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
    effect.lpvTypeSpecificParams = &rf;

    while(!FAILED(hr) && _shakes.size() < 5)
    {
        IDirectInputEffect * pEffect = NULL;
        _pDevice->CreateEffect(GUID_RampForce, &effect, &pEffect, NULL);

        if(!FAILED(hr) && pEffect != NULL)
            _shakes.push_back(pEffect);
    }

    update();
}
//-----------------------------------------------------------------------------
JoystickDI::~JoystickDI()
{
    for(int32 ii=0; ii < _shakes.size(); ii++)
        if(_shakes[ii] != NULL)
            _shakes[ii]->Release();

    if(_pShakeLoop != NULL)
        _pShakeLoop->Release();

    if(_pDevice != NULL)
        _pDevice->Release();
}
//-----------------------------------------------------------------------------
float JoystickDI::xAxisL() const
{
    return _xAxis;
}
//-----------------------------------------------------------------------------
float JoystickDI::yAxisL() const
{
    return _yAxis;
}
//-----------------------------------------------------------------------------
float JoystickDI::xAxisR() const
{
    return _zAxis;
}
//-----------------------------------------------------------------------------
float JoystickDI::yAxisR() const
{
    return _zRoll;
}
//-----------------------------------------------------------------------------
bool JoystickDI::isButtonTyped(int32 n) const
{
    return n < _buttonTyped.size() ? _buttonTyped[n] : false;
}
//-----------------------------------------------------------------------------
bool JoystickDI::isButtonDown(int32 n) const
{
    return n < _buttonDown.size() ? _buttonDown[n] : false;
}
//-----------------------------------------------------------------------------
static float toFloatInput(int32 x)
{
    static const int32 THRESHOLD = 1000;

    x -= 0x00008000;

    if(x > 0)
        return float(std::max(0, x - THRESHOLD)) / float(0x00007FFF - THRESHOLD);
    else
        return float(std::min(0, x + THRESHOLD)) / float(0x00008000 - THRESHOLD);
}
//-----------------------------------------------------------------------------
void JoystickDI::update()
{
    DIJOYSTATE2 js;
    HRESULT hr = _pDevice->Poll();

    if(FAILED(hr))
        hr = _pDevice->Acquire();

    if(!FAILED(hr))
        hr = _pDevice->GetDeviceState(sizeof(js), &js);

    if(FAILED(hr))
    {
        _xAxis = 0.0f;
        _yAxis = 0.0f;
        _zAxis = 0.0f;
        _zRoll = 0.0f;

        for(int32 ii=0; ii < _buttonDown.size(); ii++)
        {
            _buttonDown[ii] = false;
            _buttonTyped[ii] = false;
        }
    }
    else
    {
        _xAxis = toFloatInput(js.lX);
        _yAxis = toFloatInput(js.lY);
        _zAxis = toFloatInput(js.lZ);
        _zRoll = toFloatInput(js.lRz);

        for(int32 ii=0; ii < _buttonDown.size(); ii++)
        {
            if((js.rgbButtons[ii] & 0x80) != 0)
            {
                _buttonTyped[ii] = !_buttonDown[ii];
                _buttonDown[ii] = true;
            }
            else
            {
                _buttonDown[ii] = false;
                _buttonTyped[ii] = false;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void JoystickDI::startVibrations(float intensity)
{
    if(_pShakeLoop == NULL)
        return;

    DICONSTANTFORCE cf   = { int32(float(intensity) * 10000.0f) };

    DIEFFECT effect;
    ZeroMemory(&effect, sizeof(effect));
    effect.dwSize = sizeof(DIEFFECT);
    effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
    effect.lpvTypeSpecificParams = &cf;

    HRESULT hr = _pShakeLoop->SetParameters(&effect, DIEP_TYPESPECIFICPARAMS);

    if(!FAILED(hr))
        _pShakeLoop->Start(1, 0);
}
//-----------------------------------------------------------------------------
void JoystickDI::stopVibrations()
{
    if(_pShakeLoop != NULL)
        _pShakeLoop->Stop();
}
//-----------------------------------------------------------------------------
void JoystickDI::shake(float intensity, float duration)
{
    shake(intensity, intensity, duration);
}
//-----------------------------------------------------------------------------
void JoystickDI::shake(float intensity1, float intensity2, float duration)
{
    if(_shakes.size() == 0)
        return;

    intensity1 = Core::clamp(0.0f, 1.0f, intensity1);
    intensity2 = Core::clamp(0.0f, 1.0f, intensity2);

    DIRAMPFORCE rf       = { int32(float(intensity1) * 10000.0f), int32(float(intensity2) * 10000.0f) };

    DIEFFECT effect;
    ZeroMemory(&effect, sizeof(effect));
    effect.dwSize = sizeof(DIEFFECT);
    effect.dwDuration = int32(duration * 1000000.0f);
    effect.cbTypeSpecificParams = sizeof(DIRAMPFORCE);
    effect.lpvTypeSpecificParams = &rf;

    HRESULT hr =_shakes[_iShake]->SetParameters(&effect, DIEP_DURATION | DIEP_TYPESPECIFICPARAMS);

    if(!FAILED(hr))
    {
        _shakes[_iShake]->Start(1, 0);
        _iShake = (_iShake + 1) % _shakes.size();
    }
}
//-----------------------------------------------------------------------------
const String & JoystickDI::getName() const
{
    return _name;
}
//-----------------------------------------------------------------------------
String JoystickDI::getButtonName(int32 index) const
{
    return Core::toString(index + 1);
}
//-----------------------------------------------------------------------------
int32 JoystickDI::getButtonTyped() const
{
    for(int32 ii=0; ii < _buttonTyped.size(); ii++)
        if(_buttonTyped[ii])
            return ii;
    return -1;
}
//-----------------------------------------------------------------------------
}