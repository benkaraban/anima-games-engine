/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
//TODO GCC-> VC2008
//deplacement de la ligne en debut de fichier
//#include "DriverDx9.h"
#include <Gfx/GfxDx9/D3DTypeConvert.h>
#include <Gfx/GfxDx9/VideoMemory.h>
#include "DriverDx9.h"
//fin TODO

#include <Core/Set.h>
#include <Core/Logger.h>

//TODO GCC-> VC2008
//deplacement de la ligne en debut de fichier
//#include <Gfx/GfxDx9/D3DTypeConvert.h>
//#include "DriverDx9.h"
//fin TODO

namespace Gfx
{
//-----------------------------------------------------------------------------
AdapterInformation D3D2GfxTypeConvert(const D3DADAPTER_IDENTIFIER9& D3Didentifier)
{
    //Driver
    String name(String8(D3Didentifier.Description));

    //DriverVersion
    int32 product = HIWORD(D3Didentifier.DriverVersion.HighPart);
    int32 version = LOWORD(D3Didentifier.DriverVersion.HighPart);
    int32 subVersion = HIWORD(D3Didentifier.DriverVersion.LowPart);
    int32 build = LOWORD(D3Didentifier.DriverVersion.LowPart);

    //Description
    String information;
    information << L"DeviceName           : " << String8(D3Didentifier.DeviceName) << L"\n";
    information << L"DriverName           : " << String8(D3Didentifier.Driver) << L"\n";
    information << L"DriverVersion        : " << Core::toString(product) << L"." << Core::toString(version) << L"."
    << Core::toString(subVersion) << L"." << Core::toString(build) << L"\n";
    information << L"VendorId             : 0x" << Core::toStringHex(int32(D3Didentifier.VendorId)) << L"\n";
    information << L"DeviceId             : 0x" << Core::toStringHex(int32(D3Didentifier.DeviceId)) << L"\n";
    information << L"SubSysId             : 0x" << Core::toStringHex(int32(D3Didentifier.SubSysId)) << L"\n";
    information << L"Revision             : " << Core::toString(int32(D3Didentifier.Revision)) << L"\n";

    return AdapterInformation (name, information);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
DriverDx9::DriverDx9()
{
    _pD3D = Direct3DCreate9(D3D_SDK_VERSION);

    if (!_pD3D)
    {
        ERR << L"Unable to create Direct3D9 object.\n";
        throw GfxException(L"Unable to create Direct3D9 object.");
    }
    else
    {
        initAdapters();
        initDisplayModeList();

        INF << L"---------------------------------------------------------\n";
        INF << L"Direct3D9 initialised (" << Core::toString(getAdapterCount()) << L" device(s) found)\n";
        for(int32 ii=0; ii < getAdapterCount(); ii++)
        {
            INF << L"---------------------------------------------------------\n";
            INF << L"Adapter              : " << _infos[ii].getName() << L"\n";
            INF << L"Monitor              : 0x" << Core::toStringHex(int32(_pD3D->GetAdapterMonitor(ii)), 8) << L"\n";

            if(_caps[ii].detailedVRAMInfosAvailable)
            {
                INF << L"Dedicated video RAM  : " << Core::toStringByteSize(_caps[ii].dedicatedVideoMem) << L"\n";
                INF << L"Dedicated system RAM : " << Core::toStringByteSize(_caps[ii].dedicatedSystemMem) << L"\n";
                INF << L"Shared system RAM    : " << Core::toStringByteSize(_caps[ii].sharedSystemMem) << L"\n";
            }
            else if(_caps[ii].globalVRAMInfosAvailable)
            {
                INF << L"Dedicated RAM        : " << Core::toStringByteSize(_caps[ii].globalVideoMem) << L"\n";
            }
            else
            {
                INF << L"Dedicated RAM        : unknown\n";
            }

            INF << _infos[ii].getInformation();
            INF << L"- CAPS --------------------------------------------------\n";
            INF << L"Detected vendor      : " << Gfx::toString(_caps[ii].vendor) << L"\n";
            INF << L"Vertex shader vers.  : " << Gfx::toString(_caps[ii].vsVersion) << L"\n";
            INF << L"Pixel shader vers.   : " << Gfx::toString(_caps[ii].psVersion) << L"\n";
            INF << L"Max VS constants     : " << Core::toString(_caps[ii].maxVSConstCount) << L"\n";
            INF << L"Max PS instructions  : " << Core::toString(_caps[ii].maxPS2InstCount) << L"\n";
            INF << L"---------------------------------------------------------\n";
            INF << L"Max MSAA level       : " << Gfx::toString(_caps[ii].maxMSAA) << "\n";
            INF << L"Max MSAA quality     : " << Core::toString(_caps[ii].maxMSAAQuality) << "\n";
            INF << L"Max Anisotropy       : " << Core::toString(_caps[ii].maxAnisotropy) << "x\n";
            INF << L"Alpha coverage       : " << Core::strYesNo(_caps[ii].supportAlphaCoverage) << "\n";
            INF << L"---------------------------------------------------------\n";
            INF << L"Separate Alpha Blend : " << Core::strYesNo(_caps[ii].supportSeparateAlphaBlend) << "\n";
            INF << L"Blending ops         : " << Core::strYesNo(_caps[ii].supportBlendingOps) << "\n";
            INF << L"Float16 Textures     : " << Core::strYesNo(_caps[ii].supportFLOAT16Tex) << "\n";
            INF << L"Float16 Blending     : " << Core::strYesNo(_caps[ii].supportFLOAT16Blend) << "\n";
            INF << L"---------------------------------------------------------\n";
            INF << L"Support UBYTE4       : " << Core::strYesNo(_caps[ii].supportUBYTE4) << "\n";
            INF << L"Support UBYTE4N      : " << Core::strYesNo(_caps[ii].supportUBYTE4N) << "\n";
            INF << L"Support SHORT2N      : " << Core::strYesNo(_caps[ii].supportSHORT2N) << "\n";
            INF << L"Support SHORT4N      : " << Core::strYesNo(_caps[ii].supportSHORT4N) << "\n";
            INF << L"Support DEC3N        : " << Core::strYesNo(_caps[ii].supportDEC3N) << "\n";
            INF << L"Support FLOAT16_2    : " << Core::strYesNo(_caps[ii].supportFLOAT16_2) << "\n";
            INF << L"Support FLOAT16_4    : " << Core::strYesNo(_caps[ii].supportFLOAT16_4) << "\n";
            INF << L"---------------------------------------------------------\n";
        }
    }
}
//-----------------------------------------------------------------------------
DriverDx9::~DriverDx9()
{
    LM_ASSERT(_pD3D != NULL);
    LM_D3D_RELEASE(_pD3D);
}
//-----------------------------------------------------------------------------
bool DriverDx9::checkMSAA(int32 id, D3DMULTISAMPLE_TYPE t, int32 & maxQuality)
{
    DWORD q1 = 0;
    DWORD q2 = 0;
    DWORD q3 = 0;
    DWORD q4 = 0;

    HRESULT hr;
    bool ok1 = (hr = _pD3D->CheckDeviceMultiSampleType(id, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,  FALSE, t, &q1)) == D3D_OK;
    bool ok2 = (hr = _pD3D->CheckDeviceMultiSampleType(id, D3DDEVTYPE_HAL, D3DFMT_D24S8,     FALSE, t, &q2)) == D3D_OK;
    bool ok3 = (hr = _pD3D->CheckDeviceMultiSampleType(id, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,  TRUE,  t, &q3)) == D3D_OK;
    bool ok4 = (hr = _pD3D->CheckDeviceMultiSampleType(id, D3DDEVTYPE_HAL, D3DFMT_D24S8,     TRUE,  t, &q4)) == D3D_OK;
    bool ok = (ok1 && ok2 && ok3 && ok4);

    //LM_ASSERT(q1 == q2 && q2 == q3 && q3 == q4);
    maxQuality = int32(std::min(q1, std::min(q2, std::min(q3, q4))));

    return ok;
}
//-----------------------------------------------------------------------------
void DriverDx9::initAdapters()
{
    LM_ASSERT(_pD3D != NULL);
    D3DADAPTER_IDENTIFIER9 D3Didentifier;

    _caps.clear();
    _capsD3D.clear();
    _infos.clear();

	//TODO GCC-> VC2008 int32 ii -> uint32 ii
    for (int32 ii=0; ii < int32(_pD3D->GetAdapterCount()); ii++)
    {
        GraphicCapabilities caps;
        HRESULT hresult = _pD3D->GetAdapterIdentifier(ii, 0, &D3Didentifier);
        LM_HANDLE_D3D_ERROR(hresult, L"GetAdapterIdentifier");
        _infos.push_back(D3D2GfxTypeConvert(D3Didentifier));

        // VRAM
        HMONITOR hMonitor = _pD3D->GetAdapterMonitor(ii);
        caps.detailedVRAMInfosAvailable = getVideoMemoryViaDXGI(hMonitor, caps.dedicatedVideoMem, caps.dedicatedSystemMem, caps.sharedSystemMem);
        caps.globalVRAMInfosAvailable = getVideoMemoryViaWMI(hMonitor, D3Didentifier.DeviceName, caps.globalVideoMem);

        if(caps.detailedVRAMInfosAvailable)
        {
            if(caps.dedicatedVideoMem > 0)
                caps.videoMem = caps.dedicatedVideoMem;
            else if(caps.dedicatedSystemMem > 0)
                caps.videoMem = caps.dedicatedSystemMem / 2;
            else
                caps.videoMem = caps.sharedSystemMem / 4;
        }
        else if(caps.globalVRAMInfosAvailable)
            caps.videoMem = caps.globalVideoMem;
        else
            caps.videoMem = 0;

        // Caps

        D3DCAPS9 capsD3D;
        HRESULT result = _pD3D->GetDeviceCaps(ii, D3DDEVTYPE_HAL, &capsD3D);
        LM_HANDLE_D3D_ERROR(result, L"GetDeviceCaps");

        switch(D3Didentifier.VendorId)
        {
        case 0x1002: caps.vendor = VENDOR_ATI; break;
        case 0x10DE: caps.vendor = VENDOR_NVIDIA; break;
        case 0x8086: caps.vendor = VENDOR_INTEL; break;
        default:     caps.vendor = VENDOR_UNKOWN; break;
        }

        int maxq2 = 0;
        int maxq4 = 0;
        int maxq8 = 0;
        bool m2 = checkMSAA(ii, D3DMULTISAMPLE_2_SAMPLES, maxq2);
        bool m4 = checkMSAA(ii, D3DMULTISAMPLE_4_SAMPLES, maxq4);
        bool m8 = checkMSAA(ii, D3DMULTISAMPLE_8_SAMPLES, maxq8);

        //LM_ASSERT(maxq2 == maxq4);

        if(m2 && m4 && m8)
            caps.maxMSAA = MSAA_8X;
        else if(m2 && m4)
            caps.maxMSAA = MSAA_4X;
        else if(m2)
            caps.maxMSAA = MSAA_2X;
        else
            caps.maxMSAA = MSAA_NONE;

        caps.maxMSAAQuality = std::min(maxq2, maxq4);
        caps.maxAnisotropy = capsD3D.MaxAnisotropy;

        if(_pD3D->CheckDeviceFormat(ii, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK)
        {
            caps.supportFLOAT16Tex = true;
            caps.supportFLOAT16Blend = (_pD3D->CheckDeviceFormat(ii, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8,
                                                                 D3DUSAGE_RENDERTARGET | D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                                                 D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F) == D3D_OK);
        }
        else
        {
            caps.supportFLOAT16Tex = false;
            caps.supportFLOAT16Blend = false;
        }

        caps.supportBlendingOps = (capsD3D.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP) != 0;
        caps.supportSeparateAlphaBlend = (capsD3D.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND) != 0;

        caps.supportUBYTE4 = ((capsD3D.DeclTypes & D3DDTCAPS_UBYTE4) != 0);
        caps.supportUBYTE4N = ((capsD3D.DeclTypes & D3DDTCAPS_UBYTE4N) != 0);
        caps.supportSHORT2N = ((capsD3D.DeclTypes & D3DDTCAPS_SHORT2N) != 0);
        caps.supportSHORT4N = ((capsD3D.DeclTypes & D3DDTCAPS_SHORT4N) != 0);
        caps.supportDEC3N = ((capsD3D.DeclTypes & D3DDTCAPS_DEC3N) != 0);
        caps.supportFLOAT16_2 = ((capsD3D.DeclTypes & D3DDTCAPS_FLOAT16_2) != 0);
        caps.supportFLOAT16_4 = ((capsD3D.DeclTypes & D3DDTCAPS_FLOAT16_4) != 0);

        // Vertex shader version

        int32 majorVS = D3DSHADER_VERSION_MAJOR(capsD3D.VertexShaderVersion);
        int32 minorVS = D3DSHADER_VERSION_MINOR(capsD3D.VertexShaderVersion);

        if(majorVS >= 3)
            caps.vsVersion = VS_V3_0;
        else if(majorVS == 2)
            caps.vsVersion = VS_V2_0;
        else if(majorVS == 1 && minorVS >= 1)
            caps.vsVersion = VS_V1_1;
        else
            caps.vsVersion = VS_V0_0;

        caps.maxVSConstCount = capsD3D.MaxVertexShaderConst;
        caps.maxPS2InstCount = capsD3D.PS20Caps.NumInstructionSlots;


        // Pixel shader version

        int32 majorPS = D3DSHADER_VERSION_MAJOR(capsD3D.PixelShaderVersion);
        int32 minorPS = D3DSHADER_VERSION_MINOR(capsD3D.PixelShaderVersion);

        if(majorPS >= 3)
            caps.psVersion = PS_V3_0;
        else if(majorPS == 2)
            if(capsD3D.PS20Caps.NumInstructionSlots >= 512 &&
                capsD3D.PS20Caps.NumTemps >= 22 &&
                ((capsD3D.PS20Caps.Caps & D3DPS20CAPS_ARBITRARYSWIZZLE) != 0) &&
                ((capsD3D.PS20Caps.Caps & D3DPS20CAPS_GRADIENTINSTRUCTIONS) != 0) &&
                ((capsD3D.PS20Caps.Caps & D3DPS20CAPS_PREDICATION) != 0) &&
                ((capsD3D.PS20Caps.Caps & D3DPS20CAPS_NODEPENDENTREADLIMIT) != 0) &&
                ((capsD3D.PS20Caps.Caps & D3DPS20CAPS_NOTEXINSTRUCTIONLIMIT) != 0))
                caps.psVersion = PS_V2_a;
            else
                caps.psVersion = PS_V2_0;
        else if(majorPS == 1 && minorPS >= 1)
            if(minorPS >= 4)
                caps.psVersion = PS_V1_4;
            else if(minorPS == 3)
                caps.psVersion = PS_V1_3;
            else if(minorPS == 2)
                caps.psVersion = PS_V1_2;
            else
                caps.psVersion = PS_V1_1;
        else
            caps.psVersion = PS_V0_0;

        // Tests vendor-specific alpha coverage
        switch(caps.vendor)
        {
        case VENDOR_NVIDIA:
            caps.supportAlphaCoverage = (_pD3D->CheckDeviceFormat(ii, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, 0, D3DRTYPE_SURFACE, 
                                                                  (D3DFORMAT)MAKEFOURCC('A', 'T', 'O', 'C')) == S_OK);
            break;
        case VENDOR_ATI:
            caps.supportAlphaCoverage = (caps.psVersion >= PS_V3_0);
            break;
        case VENDOR_INTEL:
            // Certaines Intel sont sensées supporter le même hack que ATI mais on risque surtout de les faire planter, à
            // modifier un jour si on est sûrs de nous.
        case VENDOR_UNKOWN:
            caps.supportAlphaCoverage = false;
            break;
        }

        _capsD3D.push_back(capsD3D);
        _caps.push_back(caps);
    }
}

//-----------------------------------------------------------------------------
void DriverDx9::initDisplayModeList()
{
    int32 adapterCount = _pD3D->GetAdapterCount();
    _displayModes.clear();
    _displayModes.resize(adapterCount);

    for(int32 iAdapter=0; iAdapter < adapterCount; iAdapter++)
    {
        for(int32 iFormat=0; iFormat < EDisplayPixelFormat_COUNT; iFormat++)
        {
            D3DFORMAT format = D3DTypeConvert(EDisplayPixelFormat(iFormat));
			//TODO GCC->VC2008 int32 iMode->uint32 iMode
            for(int32 iMode=0; iMode < int32(_pD3D->GetAdapterModeCount(iAdapter, format)); iMode++)
            {
                D3DDISPLAYMODE mode;
                HRESULT hresult = _pD3D->EnumAdapterModes(iAdapter, format, iMode, &mode);
                LM_HANDLE_D3D_ERROR(hresult, L"GetAdapterDisplayMode");
                _displayModes[iAdapter].push_back(DisplayMode(mode.Width, mode.Height, mode.RefreshRate, EDisplayPixelFormat(iFormat)));
            }
        }
    }
}
//-----------------------------------------------------------------------------
void DriverDx9::createDevice(
        int32 adapterId,
        EVertexProcessing vertexProcess,
        const SwapChainDesc& scDesc,
        ISwapChainPtr& pSC,
        IDevicePtr& pDevice,
        int32 flags)
{
    INF << L"Creating D3D9 device adapter "
    << Core::toString(adapterId) << L" "
    << Gfx::toString(vertexProcess);

    int32 vProcess;
    LPDIRECT3DDEVICE9 pDeviceD3D = NULL;
    D3DPRESENT_PARAMETERS presentParam;
    D3DTypeConvert(presentParam, scDesc);

    switch(vertexProcess)
    {
        case HARDWARE_PROCESSING: vProcess = D3DCREATE_HARDWARE_VERTEXPROCESSING; break;
        case SOFTWARE_PROCESSING: vProcess = D3DCREATE_SOFTWARE_VERTEXPROCESSING; break;
    }

    if(flags & MULTITHREADED_DEVICE_FLAG)
        vProcess |= D3DCREATE_MULTITHREADED;

    HRESULT hresult = _pD3D->CreateDevice(
            adapterId,
            D3DDEVTYPE_HAL,
            static_cast<HWND>(scDesc.handleWindow),
            vProcess,
            &presentParam,
            &pDeviceD3D);
    LM_HANDLE_D3D_ERROR(hresult, L"createDevice -> CreateDevice");

    if(pDeviceD3D == NULL)
    {
        ERR << L"Unable to create device for adapter " << Core::toString(adapterId) <<"\n";
        throw GfxException(L"Device creation error.");
    }

    LPDIRECT3DSWAPCHAIN9 pD3DswapChain;
    hresult = pDeviceD3D->GetSwapChain(0, &pD3DswapChain);
    LM_HANDLE_D3D_ERROR(hresult, L"createDevice -> GetSwapChain");

    // Le back buffer est en MSAA mais pas le front
    int32 frontMem = scDesc.width * scDesc.height * 4;
    int32 frameBufferMem = frontMem + frontMem * (1 << int32(scDesc.multiSamplingType));

    pDevice = Core::Ptr<Gfx::DeviceDx9> (new DeviceDx9(pDeviceD3D, _capsD3D[adapterId], _caps[adapterId], frameBufferMem));
    pSC = Core::Ptr<Gfx::SwapChainDx9>(new SwapChainDx9 (scDesc, pD3DswapChain, pDeviceD3D));
}
//-----------------------------------------------------------------------------
DisplayMode DriverDx9::getAdapterDisplayMode(int32 adapterId)
{
    D3DDISPLAYMODE mode;

    HRESULT hresult = _pD3D->GetAdapterDisplayMode(adapterId, &mode);
    LM_HANDLE_D3D_ERROR(hresult, L"GetAdapterDisplayMode");

    EDisplayPixelFormat format;

    switch(mode.Format)
    {
    case D3DFMT_A2B10G10R10: format = DPF_A2R10G10B10; break;
    case D3DFMT_A8B8G8R8:    format = DPF_A8R8G8B8; break;
    case D3DFMT_X8R8G8B8:    format = DPF_X8R8G8B8; break;
    case D3DFMT_A1R5G5B5:    format = DPF_A1R5G5B5; break;
    case D3DFMT_X1R5G5B5:    format = DPF_X1R5G5B5; break;
    case D3DFMT_R5G6B5:      format = DPF_R5G6B5; break;
    default:
        throw(Core::Exception(L"getAdapterDisplayMode => unkown format " + Core::toString(int32(mode.Format))));
    }

    return DisplayMode(mode.Width, mode.Height, mode.RefreshRate, format);
}
//-----------------------------------------------------------------------------
const Core::List<DisplayMode> & DriverDx9::getDisplayFormats(int32 adapterId) const
{
    return _displayModes[adapterId];
}
//-----------------------------------------------------------------------------
} // namespace Gfx
