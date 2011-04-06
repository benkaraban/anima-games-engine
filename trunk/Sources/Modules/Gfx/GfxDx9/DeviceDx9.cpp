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

#include "DeviceDx9.h"
#include <Gfx/GfxDx9/D3DTypeConvert.h>
#include <Gfx/GfxDx9/RenderTargetDx9.h>
#include <Gfx/GfxDx9/DepthStencilDx9.h>
#include <Gfx/GfxDx9/TextureDx9.h>
#include <Gfx/GfxDx9/FontDX9.h>

namespace Gfx
{

DeviceDx9::DeviceDx9(LPDIRECT3DDEVICE9 pDevice, const D3DCAPS9 & capsD3D, const GraphicCapabilities & caps, int32 frameBufferMem)
: _pDevice(pDevice),
  _capsD3D(capsD3D),
  _caps(caps),
  _separateAlphaBlendEnabled(true),
  _deviseIsLost(false),
  _renderTargetsCount(1),
  _fixedFuncInit(false)
{
    setDefaultState();
    _fixed.fogEnabled = false;
    _fixed.fogColor = Core::Vector4f::ZERO;

    _totalVideoMem = getAvailableVideoMem() + int64(frameBufferMem);
}


DeviceDx9::~DeviceDx9()
{
    LM_D3D_RELEASE(_pDevice);
}

IVertexFormatPtr DeviceDx9::createVertexFormat(const VertexFormatDesc & desc, const IVertexShaderPtr & pShader)
{
    Core::Map<VertexFormatDesc, IVertexFormatPtr>::const_iterator iVtx = _vtxFormats.find(desc);
    
    if(iVtx != _vtxFormats.end())
        return iVtx->second;
    else
    {
        IVertexFormatPtr pResult(new VertexFormatDx9(desc, pShader, _pDevice));
        _vtxFormats[desc] = pResult;
        return pResult;
    }
}

IVertexBufferPtr DeviceDx9::createVertexBuffer(const VertexBufferDesc& desc, const SubresourceData* pInitData)
{
    return Ptr<VertexBufferDx9>(new VertexBufferDx9 (desc, _pDevice));
}

IIndexBufferPtr DeviceDx9::createIndexBuffer(const IndexBufferDesc& desc, const SubresourceData* pInitData)
{
    return Ptr<IndexBufferDx9>(new IndexBufferDx9 (desc, _pDevice));
}

void DeviceDx9::setVertexBuffer(int32 stream, const IVertexBufferPtr & pBuffer, int32 stride, int32 offset)
{
    VertexBufferDx9Ptr pBufferD3D = LM_DEBUG_PTR_CAST<VertexBufferDx9>(pBuffer);
    HRESULT hresult = _pDevice->SetStreamSource(stream, pBufferD3D->getD3DVertexBuffer(), offset, stride);
    LM_HANDLE_D3D_ERROR(hresult, L"SetStreamSource");
}

void DeviceDx9::setVertexBuffers(int32 nbBuffers, IVertexBufferPtr* tabVB, int32* tabStrides, int32* tabOffset)
{
    for (int32 ii=0; ii<nbBuffers; ii++)
    {
        VertexBufferDx9Ptr ptr = LM_DEBUG_PTR_CAST<VertexBufferDx9> (tabVB[ii]);
        HRESULT hresult = _pDevice->SetStreamSource(ii, ptr->getD3DVertexBuffer(), tabOffset[ii], tabStrides[ii]);
        LM_HANDLE_D3D_ERROR(hresult, L"SetStreamSourceBatch");
    }
}

void DeviceDx9::setIndexBuffer(const IIndexBufferPtr & pBuffer)
{
    IndexBufferDx9Ptr ptr = LM_DEBUG_PTR_CAST<IndexBufferDx9>(pBuffer);
    HRESULT hresult = _pDevice->SetIndices(ptr->getD3DIndexBuffer());
    LM_HANDLE_D3D_ERROR(hresult, L"SetIndices");
}

void DeviceDx9::drawIndexed(EPrimitiveType primType, int32 usedVertices, int32 startIndex, int32 primCount)
{
    _stats.triCount += primCount;
    HRESULT hresult = _pDevice->DrawIndexedPrimitive(D3DTypeConvert(primType), 0, 0, usedVertices, startIndex, primCount);
    LM_HANDLE_D3D_ERROR(hresult, L"DrawIndexedPrimitive");
}

void DeviceDx9::drawIndexed(EPrimitiveType primType, int32 baseVertexIndex, int32 MinIndex, int32 usedVertices, int32 startIndex, int32 primCount)
{
    _stats.triCount += primCount;
    HRESULT hresult = _pDevice->DrawIndexedPrimitive(D3DTypeConvert(primType), baseVertexIndex, MinIndex, usedVertices, startIndex, primCount);
    LM_HANDLE_D3D_ERROR(hresult, L"DrawIndexedPrimitive");
}

void DeviceDx9::beginDraw()
{
    HRESULT hresult = _pDevice->BeginScene();
    LM_HANDLE_D3D_ERROR(hresult, L"BeginScene");
}
void DeviceDx9::endDraw()
{
    HRESULT hresult = _pDevice->EndScene();
    LM_HANDLE_D3D_ERROR(hresult, L"EndScene");
}

void DeviceDx9::setPixelShader(const IPixelShaderPtr & pShader)
{
    if(_pCurPixelShader != pShader)
    {
        for(int32 ii=0; ii < MAX_SAMPLERS; ii++)
        {
            if(_samplerUsed[ii])
            {
                HRESULT hresult = _pDevice->SetTexture(ii, NULL);
                LM_HANDLE_D3D_ERROR(hresult, L"SetTextureSampler NULL");
                _samplerUsed[ii] = false;
            }
        }

        _stats.psSwitchCount ++;

        if(pShader == null)
        {
            HRESULT hresult = _pDevice->SetPixelShader(NULL);
            LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShader NULL");
        }
        else
        {
            HLSLPixelShaderDx9Ptr pShaderDx9 = LM_DEBUG_PTR_CAST<HLSLPixelShaderDx9> (pShader);
            HRESULT hresult = _pDevice->SetPixelShader(pShaderDx9->getShader());
            LM_HANDLE_D3D_ERROR(hresult, L"SetPixelShader");
        }

        _pCurPixelShader = pShader;
    }
}
void DeviceDx9::setVertexShader(const IVertexShaderPtr & pShader)
{
    if(_pCurVertexShader != pShader)
    {
        _stats.vsSwitchCount ++;

        if(pShader == null)
        {
            HRESULT hresult = _pDevice->SetVertexShader(NULL);
            LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShader NULL");
        }
        else
        {
            HLSLVertexShaderDx9Ptr pShaderDx9 = LM_DEBUG_PTR_CAST<HLSLVertexShaderDx9> (pShader);
            HRESULT hresult = _pDevice->SetVertexShader(pShaderDx9->getShader());
            LM_HANDLE_D3D_ERROR(hresult, L"SetVertexShader");
        }

        _pCurVertexShader = pShader;
    }
}

IRSRasterizerPtr DeviceDx9::createState(const RSRasterizerDesc& desc)
{
    return RSRasterizerDx9Ptr(new RSRasterizerDx9(desc));
}

IRSBlendPtr DeviceDx9::createState(const RSBlendDesc& desc)
{
    return RSBlendDx9Ptr(new RSBlendDx9(desc));
}

IRSDepthStencilPtr DeviceDx9::createState(const RSDepthStencilDesc& desc)
{
    return RSDepthStencilDx9Ptr(new RSDepthStencilDx9(desc));
}

IRSSamplerPtr DeviceDx9::createState(const RSSamplerDesc& desc)
{
    return RSSamplerDx9Ptr(new RSSamplerDx9(desc));
}

void DeviceDx9::setVertexFormat(const IVertexFormatPtr& pVertexFormat)
{
    if(_pCurVertexFormat != pVertexFormat)
    {
        _stats.setVertexDeclCount ++;
        VertexFormatDx9Ptr ptr = LM_DEBUG_PTR_CAST<VertexFormatDx9>(pVertexFormat);
        HRESULT hresult = _pDevice->SetVertexDeclaration(ptr->_pD3DVertexDeclaration);
        LM_HANDLE_D3D_ERROR(hresult, L"SetVertexDeclaration");
        _pCurVertexFormat = pVertexFormat;
    }
}

void DeviceDx9::setState(const IRSRasterizerPtr& pNewState)
{
    HRESULT hresult = D3D_OK;
    const RSRasterizerDesc & oldState = _currentState.pRasterizer->getDesc();
    const RSRasterizerDesc & newState = pNewState->getDesc();

    //Cull Mode
    D3DCULL oldCull = D3DTypeConvert(oldState.cullMode, oldState.frontFaceIsCounterClockwise);
    D3DCULL newCull = D3DTypeConvert(newState.cullMode, newState.frontFaceIsCounterClockwise);

    if (oldCull != newCull)
    {
        hresult = _pDevice->SetRenderState(D3DRS_CULLMODE, newCull);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_CULLMODE");
    }

    //Fill Mode
    if (oldState.fillMode != newState.fillMode)
    {
        hresult = _pDevice->SetRenderState(D3DRS_FILLMODE, D3DTypeConvert(newState.fillMode));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_FILLMODE");
    }

    if (oldState.depthBias != newState.depthBias)
    {
        hresult = _pDevice->SetRenderState(D3DRS_DEPTHBIAS, F2DW(float(newState.depthBias)));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_DEPTHBIAS");
    }

    if (oldState.depthBiasScale != newState.depthBiasScale)
    {
        hresult = _pDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS, F2DW(newState.depthBiasScale));
        LM_HANDLE_D3D_ERROR(hresult, L"D3DRS_SLOPESCALEDEPTHBIAS D3DRS_DEPTHBIAS");
    }

    _currentState.pRasterizer = pNewState;
}
void DeviceDx9::setState(const IRSBlendPtr& pNewState)
{
    HRESULT hresult = D3D_OK;
    const RSBlendDesc & oldState = _currentState.pBlend->getDesc();
    const RSBlendDesc & newState = pNewState->getDesc();

    if(oldState.colorWriteEnabled != newState.colorWriteEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, newState.colorWriteEnabled ? 0xFFFFFFFF : 0x00000000);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_COLORWRITEENABLE");
    }

    // Enabled
    if (oldState.enabled != newState.enabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, newState.enabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ALPHABLENDENABLE");
    }

    //source blend
    if (oldState.src != newState.src)
    {
        hresult = _pDevice->SetRenderState(D3DRS_SRCBLEND, D3DTypeConvert(newState.src));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SRCBLEND");
    }

    //dest blend
    if (oldState.dst != newState.dst)
    {
        hresult = _pDevice->SetRenderState(D3DRS_DESTBLEND, D3DTypeConvert(newState.dst));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_DESTBLEND");
    }

    //Op Blend
    if (oldState.op != newState.op)
    {
        hresult = _pDevice->SetRenderState(D3DRS_BLENDOP, D3DTypeConvert(newState.op));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_BLENDOP");
    }

    //alpha
    if (_capsD3D.PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND)
    {
        bool separateAlpha = (newState.srcAlpha != newState.src) || (newState.dstAlpha != newState.dst) || (newState.opAlpha != newState.op);

        if(separateAlpha != _separateAlphaBlendEnabled)
        {
            hresult = _pDevice->SetRenderState(D3DRS_SEPARATEALPHABLENDENABLE, separateAlpha);
            LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SEPARATEALPHABLENDENABLE");
        }

        //src alpha
        if (oldState.srcAlpha != newState.srcAlpha)
        {
            hresult = _pDevice->SetRenderState(D3DRS_SRCBLENDALPHA, D3DTypeConvert(newState.srcAlpha));
            LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SRCBLENDALPHA");
        }

        //dst alpha
        if (oldState.dstAlpha != newState.dstAlpha)
        {
            hresult = _pDevice->SetRenderState(D3DRS_DESTBLENDALPHA, D3DTypeConvert(newState.dstAlpha));
            LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_DESTBLENDALPHA");
        }

        //Op apha
        if (oldState.opAlpha != newState.opAlpha)
        {
            hresult = _pDevice->SetRenderState(D3DRS_BLENDOPALPHA, D3DTypeConvert(newState.opAlpha));
            LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_BLENDOPALPHA");
        }
        _separateAlphaBlendEnabled = separateAlpha;
    }

    if(_caps.supportAlphaCoverage)
    {
        if(oldState.alphaCoverageEnabled != newState.alphaCoverageEnabled)
        {
            switch(_caps.vendor) // http://www.gamedev.net/community/forums/topic.asp?topic_id=485113
            {
            case VENDOR_ATI: // http://developer.amd.com/gpu_assets/Advanced%20DX9%20Capabilities%20for%20ATI%20Radeon%20Cards_v2.pdf
                if(newState.alphaCoverageEnabled)
                {
                    hresult = _pDevice->SetRenderState(D3DRS_POINTSIZE, MAKEFOURCC('A','2','M','1'));
                    LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState ALPHA COVERAGE ON (ATI hack)");
                }
                else
                {
                    hresult = _pDevice->SetRenderState(D3DRS_POINTSIZE, MAKEFOURCC('A','2','M','0'));
                    LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState ALPHA COVERAGE OFF (ATI hack)");
                }
                break;

            case VENDOR_NVIDIA: // http://http.download.nvidia.com/developer/SDK/Individual_Samples/DEMOS/Direct3D9/src/AntiAliasingWithTransparency/docs/AntiAliasingWithTransparency.pdf
                if(newState.alphaCoverageEnabled)
                {
                    hresult = _pDevice->SetRenderState(D3DRS_ADAPTIVETESS_Y, MAKEFOURCC('A','T','O','C'));
                    LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState ALPHA COVERAGE ON (NVIDIA hack)");
                }
                else
                {
                    hresult = _pDevice->SetRenderState(D3DRS_ADAPTIVETESS_Y, D3DFMT_UNKNOWN);
                    LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState ALPHA COVERAGE OFF (NVIDIA hack)");
                }
                break;

            default:
                LM_ASSERT(false);
                break;
            }
        }
    }

    if(oldState.alphaTestEnabled != newState.alphaTestEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, newState.alphaTestEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ALPHATESTENABLE TRUE");
    }

    if(oldState.alphaTestFunc != newState.alphaTestFunc)
    {
        hresult =  _pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DTypeConvert(newState.alphaTestFunc));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ALPHAFUNC");
    }

    if(oldState.alphaTestRef != newState.alphaTestRef)
    {
        hresult =  _pDevice->SetRenderState(D3DRS_ALPHAREF, (DWORD)newState.alphaTestRef);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ALPHAREF");
    }

    if(oldState.sRGBWriteEnabled != newState.sRGBWriteEnabled)
    {
        hresult =  _pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, (DWORD)newState.sRGBWriteEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SRGBWRITEENABLE");
    }

    if(_fixed.fogEnabled) // Actif en PS 2.0 mais pas en PS 3.0
    {
        hresult = _pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : FogDisable");
        _fixed.fogEnabled = false;
    }

    _currentState.pBlend = pNewState;
}
void DeviceDx9::setState(const IRSDepthStencilPtr& pNewState)
{
    HRESULT hresult = D3D_OK;
    const RSDepthStencilDesc & oldState = _currentState.pDepthStencil->getDesc();
    const RSDepthStencilDesc & newState = pNewState->getDesc();

    //depth
    //z enable
    if (oldState.isDepthTestEnabled != newState.isDepthTestEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_ZENABLE, newState.isDepthTestEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ZENABLE");
    }

    //z read only ?
    if (oldState.isDepthWriteEnabled != newState.isDepthWriteEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_ZWRITEENABLE, newState.isDepthWriteEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ZWRITEENABLE");
    }

    //z fonction
    if (oldState.depthFunction != newState.depthFunction)
    {
        hresult = _pDevice->SetRenderState(D3DRS_ZFUNC, D3DTypeConvert(newState.depthFunction));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_ZFUNC");
    }


    //stencil
    //stencil enable
    if (oldState.isStencilEnabled != newState.isStencilEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILENABLE, newState.isStencilEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILENABLE");
    }

    //masque de lecture
    if (oldState.stencilReadMask != newState.stencilReadMask)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILMASK, newState.stencilReadMask);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILMASK");
    }

    //masque d'ecriture
    if (oldState.stencilWriteMask != newState.stencilWriteMask)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILWRITEMASK, newState.stencilWriteMask);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILWRITEMASK");
    }

    //echec du test stencil
    if (oldState.opStencil.opStencilNOK != newState.opStencil.opStencilNOK)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DTypeConvert(newState.opStencil.opStencilNOK));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILFAIL");
    }

    //test stencil ok et test depth echec
    if (oldState.opStencil.opStencilOKDepthNOK != newState.opStencil.opStencilOKDepthNOK)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DTypeConvert(newState.opStencil.opStencilOKDepthNOK));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILZFAIL");
    }

    //test stencil et depth ok
    if (oldState.opStencil.opStencilOKDepthOK != newState.opStencil.opStencilOKDepthOK)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILPASS, D3DTypeConvert(newState.opStencil.opStencilOKDepthOK));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILPASS");
    }

    //comment la comparaison s'effectue sur le stencil
    if (oldState.opStencil.function != newState.opStencil.function)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DTypeConvert(newState.opStencil.function));
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILFUNC");
    }

    if (oldState.stencilRef != newState.stencilRef)
    {
        hresult = _pDevice->SetRenderState(D3DRS_STENCILREF, newState.stencilRef);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_STENCILREF");
    }

    _currentState.pDepthStencil = pNewState;
}

void DeviceDx9::internalSetFilter(int32 sampler, D3DTEXTUREFILTERTYPE min, D3DTEXTUREFILTERTYPE mag, D3DTEXTUREFILTERTYPE mip)
{
    if(mag == D3DTEXF_ANISOTROPIC)
        mag = D3DTEXF_LINEAR;

    if(_minFilter[sampler] != min)
    {
        HRESULT hresult = _pDevice->SetSamplerState(sampler, D3DSAMP_MINFILTER, min);
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_MINFILTER");
        _minFilter[sampler] = min;
    }

    if(_magFilter[sampler] != mag)
    {
        HRESULT hresult = _pDevice->SetSamplerState(sampler, D3DSAMP_MAGFILTER, mag);
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_MAGFILTER");
        _magFilter[sampler] = mag;
    }

    if(_mipFilter[sampler] != mip)
    {
        HRESULT hresult = _pDevice->SetSamplerState(sampler, D3DSAMP_MIPFILTER, mip);
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_MIPFILTER");
        _mipFilter[sampler] = mip;
    }
}

void DeviceDx9::setState(int32 id, const IRSSamplerPtr& pNewState)
{
    LM_ASSERT(id < MAX_RS_SAMPLER);

    HRESULT hresult;
    const RSSamplerDesc & oldState = _currentState.ptSampler[id]->getDesc();
    const RSSamplerDesc & newState = pNewState->getDesc();

    //filter
    if (oldState.filter != newState.filter)
    {
        switch(newState.filter)
        {
        case FILTER_MIN_MAG_MIP_POINT: internalSetFilter(id, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_POINT); break;
        case FILTER_MIN_MAG_POINT_MIP_LINEAR: internalSetFilter(id, D3DTEXF_POINT, D3DTEXF_POINT, D3DTEXF_LINEAR); break;
        case FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT: internalSetFilter(id, D3DTEXF_POINT, D3DTEXF_LINEAR, D3DTEXF_POINT); break;
        case FILTER_MIN_POINT_MAG_MIP_LINEAR: internalSetFilter(id, D3DTEXF_POINT, D3DTEXF_LINEAR, D3DTEXF_LINEAR); break;
        case FILTER_MIN_LINEAR_MAG_MIP_POINT: internalSetFilter(id, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_POINT); break;
        case FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR: internalSetFilter(id, D3DTEXF_LINEAR, D3DTEXF_POINT, D3DTEXF_LINEAR); break;
        case FILTER_MIN_MAG_LINEAR_MIP_POINT: internalSetFilter(id, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_POINT); break;
        case FILTER_MIN_MAG_MIP_LINEAR: internalSetFilter(id, D3DTEXF_LINEAR, D3DTEXF_LINEAR, D3DTEXF_LINEAR); break;
        case FILTER_ANISOTROPIC:
            internalSetFilter(id, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

            if(_maxAniso[id] != newState.maxAnisotropy)
            {
                HRESULT hresult = _pDevice->SetSamplerState(id, D3DSAMP_MAXANISOTROPY, newState.maxAnisotropy);
                LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_MAXANISOTROPY");
                _maxAniso[id] = newState.maxAnisotropy;
            }
            break;
        }
    }

    //adressing u
    if (oldState.addrModeU != newState.addrModeU)
    {
        hresult = _pDevice->SetSamplerState(id, D3DSAMP_ADDRESSU, D3DTypeConvert(newState.addrModeU, _capsD3D));
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_ADDRESSU");
    }

    //adressing v
    if (oldState.addrModeV != newState.addrModeV)
    {
        hresult = _pDevice->SetSamplerState(id, D3DSAMP_ADDRESSV, D3DTypeConvert(newState.addrModeV, _capsD3D));
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_ADDRESSV");
    }

    //adressing w
    if (oldState.addrModeW != newState.addrModeW)
    {
        hresult = _pDevice->SetSamplerState(id, D3DSAMP_ADDRESSW, D3DTypeConvert(newState.addrModeW, _capsD3D));
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_ADDRESSW");
    }

    //border color
    if (oldState.borderColor != newState.borderColor)
    {
        hresult = _pDevice->SetSamplerState(id, D3DSAMP_BORDERCOLOR, D3DCOLOR_RGBA(
                (int)(newState.borderColor.r*255),
                (int)(newState.borderColor.g*255),
                (int)(newState.borderColor.b*255),
                (int)(newState.borderColor.a*255)));
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_BORDERCOLOR");
    }

    if(oldState.isSRGB != newState.isSRGB)
    {
        hresult = _pDevice->SetSamplerState(id, D3DSAMP_SRGBTEXTURE, (DWORD)newState.isSRGB);
        LM_HANDLE_D3D_ERROR(hresult, L"SetSamplerState D3DSAMP_SRGBTEXTURE");
    }

    _currentState.ptSampler[id] = pNewState;
}

void DeviceDx9::tagTextureUnitAsUsed(int32 id)
{
    LM_ASSERT(id >= 0 && id < MAX_SAMPLERS);
    _samplerUsed[id] = true;
}

void DeviceDx9::compileShader(const String & code, const String8 & str8Version, const String & entryPoint,
                              LPD3DXCONSTANTTABLE & pConstTableD3D, LPD3DXBUFFER & pShaderData,
                              const Core::List<ShaderMacro> & macros, const Ptr<IShaderIncluder> & pIncluder)
{
    LPD3DXBUFFER pErrorBuffer = NULL;
    DWORD compileFlags = 0;

    String8 str8Code(code);
    String8 str8EntryPoint(entryPoint);

    Core::List<String8>   names;
    Core::List<String8>   values;
    Core::List<D3DXMACRO> macrosD3D;

	//TODO GCC->VC2008 foreach
	Core::List<ShaderMacro>::const_iterator iMacro = macros.begin();
	Core::List<ShaderMacro>::const_iterator iMacroEnd = macros.end();
    for(; iMacro!=iMacroEnd; iMacro++)
    {
        names.push_back(String8(iMacro->name));
        values.push_back(String8(iMacro->value));
    }

    for(int32 ii=0; ii < names.size(); ii++)
    {
        D3DXMACRO macroD3D;
        macroD3D.Name = names[ii].c_str();
        macroD3D.Definition = values[ii].c_str();
        macrosD3D.push_back(macroD3D);
    }

    D3DXMACRO nullMacro = {NULL, NULL};
    macrosD3D.push_back(nullMacro);

    D3DXIncluder includer(pIncluder);

    HRESULT hresult = D3DXCompileShader(
            str8Code.c_str(),
            str8Code.length(),
			&macrosD3D[0],
            &includer,
            str8EntryPoint.c_str(),
            str8Version.c_str(),
            compileFlags,
            &pShaderData,
            &pErrorBuffer,
            &pConstTableD3D);

    if(pErrorBuffer != NULL)
    {
        if(hresult == D3D_OK) WAR << L"Warning compiling " << String(str8Version) << L" shader : \n";
        else                  ERR << L"Error compiling " << String(str8Version) << L" shader : \n";

        const char * pBegin = reinterpret_cast<char*>(pErrorBuffer->GetBufferPointer());
        int32   size = pErrorBuffer->GetBufferSize() - 1; // -1 car GetBufferSize() inclut le '\0' final

        String8 errorMessage;
        errorMessage.resize(size);
        memcpy(const_cast<char*>(errorMessage.data()), pBegin, size);
        LM_D3D_RELEASE(pErrorBuffer);

        if(hresult == D3D_OK)
        {
            WAR << errorMessage; // errorMessage inclut un '\n'
        }
        else
        {
            ERR << errorMessage; // errorMessage inclut un '\n'
            throw GfxException(L"Error in shader source code : \n" + String(errorMessage));
        }
    }
    else if(hresult != D3D_OK)
    {
        ERR << L"Error compiling " << String(str8Version) << L" shader : \n";
        throw GfxException(L"Error compiling shader.");
    }

    LM_ASSERT(pConstTableD3D != NULL);
    LM_ASSERT(pShaderData != NULL);
}

IPixelShaderBinObjectPtr DeviceDx9::compilePShader(const Core::String& code, EPixelShaderVersion version, const Core::String& entryPoint,
                                                   const Core::List<ShaderMacro> & macros, const Ptr<IShaderIncluder> & pIncluder)
{
    LPD3DXCONSTANTTABLE pConstTableD3D = NULL;
    LPD3DXBUFFER        pShaderData    = NULL;

    String8 strVersion;

    switch(version)
    {
    case PS_V0_0: throw Core::Exception(L"Trying to compile a V0.0 pixel shader."); break;
    case PS_V1_1: strVersion = "ps_1_1"; break;
    case PS_V1_2: strVersion = "ps_1_2"; break;
    case PS_V1_3: strVersion = "ps_1_3"; break;
    case PS_V1_4: strVersion = "ps_1_4"; break;
    case PS_V2_0: strVersion = "ps_2_0"; break;
    case PS_V2_a: strVersion = "ps_2_a"; break;
    case PS_V3_0: strVersion = "ps_3_0"; break;
    }

    compileShader(code, strVersion, entryPoint, pConstTableD3D, pShaderData, macros, pIncluder);

    ShaderConstantTablePtr pConstTable(new ShaderConstantTableDx9(_pDevice, this, pConstTableD3D, PIXEL_SHADER, _stats));
    LM_D3D_RELEASE(pConstTableD3D);

    IPixelShaderBinObjectPtr pResult(new HLSLPixelShaderBinObjectDx9(pConstTable, pShaderData));
    LM_D3D_RELEASE(pShaderData);

    return pResult;
}

IVertexShaderBinObjectPtr DeviceDx9::compileVShader(const Core::String& code, EVertexShaderVersion version, const Core::String& entryPoint,
                                                    const Core::List<ShaderMacro> & macros, const Ptr<IShaderIncluder> & pIncluder)
{
    LPD3DXCONSTANTTABLE pConstTableD3D = NULL;
    LPD3DXBUFFER        pShaderData    = NULL;

    String8 strVersion;

    switch(version)
    {
    case VS_V0_0: throw Core::Exception(L"Trying to compile a V0.0 vertex shader."); break;
    case VS_V1_1: strVersion = "vs_1_1"; break;
    case VS_V2_0: strVersion = "vs_2_0"; break;
    case VS_V3_0: strVersion = "vs_3_0"; break;
    }

    compileShader(code, strVersion, entryPoint, pConstTableD3D, pShaderData, macros, pIncluder);

    ShaderConstantTablePtr pConstTable(new ShaderConstantTableDx9(_pDevice, this, pConstTableD3D, VERTEX_SHADER, _stats));
    LM_D3D_RELEASE(pConstTableD3D);

    IVertexShaderBinObjectPtr pResult(new HLSLVertexShaderBinObjectDx9(pConstTable, pShaderData));
    LM_D3D_RELEASE(pShaderData);

    return pResult;
}

IVertexShaderBinObjectPtr DeviceDx9::readPrecompiledVShader(Core::InputStream & inputStream)
{
    IVertexShaderBinObjectPtr pResult(new HLSLVertexShaderBinObjectDx9(_pDevice, this, _stats));
    pResult->read(inputStream);
    return pResult;
}

IPixelShaderBinObjectPtr DeviceDx9::readPrecompiledPShader(Core::InputStream & inputStream)
{
    IPixelShaderBinObjectPtr pResult(new HLSLPixelShaderBinObjectDx9(_pDevice, this, _stats));
    pResult->read(inputStream);
    return pResult;
}

void DeviceDx9::writePrecompiledVShader(Core::OutputStream & outputStream, const IVertexShaderBinObjectPtr & pBinObject)
{
    Ptr<HLSLVertexShaderBinObjectDx9> pBinObjectDX9(LM_DEBUG_PTR_CAST<HLSLVertexShaderBinObjectDx9>(pBinObject));
    pBinObjectDX9->write(outputStream);
}

void DeviceDx9::writePrecompiledPShader(Core::OutputStream & outputStream, const IPixelShaderBinObjectPtr & pBinObject)
{
    Ptr<HLSLPixelShaderBinObjectDx9> pBinObjectDX9(LM_DEBUG_PTR_CAST<HLSLPixelShaderBinObjectDx9>(pBinObject));
    pBinObjectDX9->write(outputStream);
}


IVertexShaderPtr DeviceDx9::createVertexShader(const IVertexShaderBinObjectPtr & pShaderBinObject)
{
    Ptr<HLSLVertexShaderBinObjectDx9> pShaderBinObjectDX9(LM_DEBUG_PTR_CAST<HLSLVertexShaderBinObjectDx9>(pShaderBinObject));
    LPDIRECT3DVERTEXSHADER9 pShader = NULL;

    HRESULT hresult = _pDevice->CreateVertexShader(pShaderBinObjectDX9->getCodeData(), &pShader);
    LM_HANDLE_D3D_ERROR(hresult, L"CreateVertexShader");

    return IVertexShaderPtr(new HLSLVertexShaderDx9(pShaderBinObjectDX9->getConstantTable(), pShader));
}

IPixelShaderPtr DeviceDx9::createPixelShader(const IPixelShaderBinObjectPtr & pShaderBinObject)
{
    Ptr<HLSLPixelShaderBinObjectDx9> pShaderBinObjectDX9(LM_DEBUG_PTR_CAST<HLSLPixelShaderBinObjectDx9>(pShaderBinObject));
    LPDIRECT3DPIXELSHADER9 pShader = NULL;

    HRESULT hresult = _pDevice->CreatePixelShader(pShaderBinObjectDX9->getCodeData(), &pShader);
    LM_HANDLE_D3D_ERROR(hresult, L"CreatePixelShader");

    return IPixelShaderPtr(new HLSLPixelShaderDx9(pShaderBinObjectDX9->getConstantTable(), pShader));
}

IRenderTargetViewPtr DeviceDx9::createRenderTargetView(ISwapChainPtr pSwapChain, const RenderTargetViewDesc& desc)
{
    SwapChainDx9Ptr pSwapChainDx9 = LM_DEBUG_PTR_CAST<SwapChainDx9> (pSwapChain);
    return IRenderTargetViewPtr(new RenderTargetViewDx9(desc, pSwapChainDx9->_D3DswapChain));
}

IRenderTargetViewPtr DeviceDx9::createRenderTargetView(ITexture2DPtr pTex2D, const RenderTargetViewDesc& desc)
{
    Texture2DDx9Ptr pTex2DDx9 = LM_DEBUG_PTR_CAST<Texture2DDx9> (pTex2D);
    return IRenderTargetViewPtr(new RenderTargetViewDx9(desc, pTex2DDx9->_pD3DTexture));
}

IRenderTargetViewPtr DeviceDx9::createRenderTargetView(ITextureCubePtr texCube, const RenderTargetViewDesc& desc)
{
    TextureCubeDx9Ptr pTexCubeDx9 = LM_DEBUG_PTR_CAST<TextureCubeDx9> (texCube);
    return IRenderTargetViewPtr(new RenderTargetViewDx9(desc, pTexCubeDx9->_pD3DTextureCube));
}

IRenderTargetViewPtr DeviceDx9::createRenderTargetView(const RenderTargetViewDesc& desc)
{
    const RTVSubresourceSurface & surface = desc.surface;
    IDirect3DSurface9 * pSurface = NULL;

    HRESULT hresult = _pDevice->CreateRenderTarget(surface.width, surface.height, D3DTypeConvert(surface.format),
                                                   D3DTypeConvert(surface.multiSampleType), 0, FALSE, &pSurface, NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"CreateRenderTarget");

    IRenderTargetViewPtr pResult(new RenderTargetViewDx9(desc, pSurface));

    // RenderTargetViewDx9 incrémente le compteur de références, il faut donc le décrémenter
    LM_D3D_RELEASE(pSurface);
    return pResult;
}

ISwapChainPtr DeviceDx9::createSwapChain(const SwapChainDesc& desc)
{    
    LPDIRECT3DSWAPCHAIN9 pD3DswapChain;    
    D3DPRESENT_PARAMETERS presentParam;    
    D3DTypeConvert(presentParam, desc);    
    HRESULT hresult = _pDevice->CreateAdditionalSwapChain(&presentParam, &pD3DswapChain);
    LM_HANDLE_D3D_ERROR(hresult, L"createSwapChain : CreateAdditionalSwapChain");

    return Core::Ptr<Gfx::SwapChainDx9>(new SwapChainDx9 (desc, pD3DswapChain, _pDevice));
}

IDepthStencilViewPtr DeviceDx9::createDepthStencilView(ITexture2DPtr pTex2D, const DepthStencilViewDesc& desc)
{
    const Texture2DDesc texDesc = pTex2D->getDesc();
    IDepthStencilViewPtr ret;

    if (texDesc.bind==TB_DEPTH_STENCIL_SURFACE)
    {
        SurfaceDx9Ptr pSurDx9 = LM_DEBUG_PTR_CAST<SurfaceDx9> (pTex2D);
        ret = DepthStencilViewDx9Ptr (new DepthStencilViewDx9 (desc, pSurDx9->_pD3DSurface));
    }
    else if(texDesc.bind==TB_DEPTH_STENCIL_TEXTURE)
    {
        Texture2DDx9Ptr pTexDx9 = LM_DEBUG_PTR_CAST<Texture2DDx9> (pTex2D);
        ret = DepthStencilViewDx9Ptr (new DepthStencilViewDx9 (desc, pTexDx9->_pD3DTexture));
    }

    return ret;
}

void DeviceDx9::setRenderTarget(IRenderTargetViewPtr rtv, IDepthStencilViewPtr dsv)
{
    HRESULT hresult;

    RenderTargetViewDx9Ptr rtvDx9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9> (rtv);

    if(rtv != null)
    {
        hresult = _pDevice->SetRenderTarget(0, rtvDx9->_D3Dcanvas);        
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetRenderTarget");    
    }    
    else    
    {        
        hresult = _pDevice->SetRenderTarget(0, NULL);        
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetRenderTarget NULL");    
    }
    if(dsv != null)
    {
        DepthStencilViewDx9Ptr dsvDx9 = LM_DEBUG_PTR_CAST<DepthStencilViewDx9> (dsv);
        hresult = _pDevice->SetDepthStencilSurface(dsvDx9->_D3Dcanvas);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetDepthStencilSurface");
    }
    else
    {
        hresult = _pDevice->SetDepthStencilSurface(NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetDepthStencilSurface NULL");
    }

    for(int32 iTarget=1; iTarget < _renderTargetsCount; iTarget++)
    {
        hresult = _pDevice->SetRenderTarget(iTarget, NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetRenderTarget NULL");
    }

    _renderTargetsCount = 1;
}

void DeviceDx9::setRenderTargets(const int32 nbRtv, IRenderTargetViewPtr* tabRtv, IDepthStencilViewPtr dsv)
{
    HRESULT hresult;

    if(dsv != null)
    {
        DepthStencilViewDx9Ptr dsvDx9 = LM_DEBUG_PTR_CAST<DepthStencilViewDx9> (dsv);
        hresult = _pDevice->SetDepthStencilSurface(dsvDx9->_D3Dcanvas);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetDepthStencilSurface");
    }
    else
    {
        hresult = _pDevice->SetDepthStencilSurface(NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetDepthStencilSurface NULL");
    }

    for (int32 iRtv=0; iRtv<nbRtv; iRtv++)
    {
        RenderTargetViewDx9Ptr rtvDx9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9> (tabRtv[iRtv]);
        hresult = _pDevice->SetRenderTarget(iRtv, rtvDx9->_D3Dcanvas);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTargets -> SetRenderTarget");
    }

    for(int32 iTarget=nbRtv; iTarget < _renderTargetsCount; iTarget++)
    {
        hresult = _pDevice->SetRenderTarget(iTarget, NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"setRenderTarget -> SetRenderTarget NULL");
    }

    _renderTargetsCount = nbRtv;
}


void DeviceDx9::getRenderTarget(IRenderTargetViewPtr& rtv, IDepthStencilViewPtr& dsv)
{
    HRESULT hresult;
    LPDIRECT3DSURFACE9 D3Dsurface;
    DepthStencilViewDesc dsvDesc;
    RenderTargetViewDesc rtvDesc;

    dsvDesc.texture2D.mipSlice = 0;
    rtvDesc.texture2D.mipSlice = 0;

    RenderTargetViewDx9Ptr rtvDx9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9> (rtv);
    DepthStencilViewDx9Ptr dsvDx9 = LM_DEBUG_PTR_CAST<DepthStencilViewDx9> (dsv);

    hresult = _pDevice->GetRenderTarget(0, &D3Dsurface);
    LM_HANDLE_D3D_ERROR(hresult, L"getRenderTarget -> GetRenderTarget");
    rtv = IRenderTargetViewPtr(new RenderTargetViewDx9 (rtvDesc, D3Dsurface));
    LM_D3D_RELEASE(D3Dsurface);

    hresult = _pDevice->GetDepthStencilSurface(&D3Dsurface);
    LM_HANDLE_D3D_ERROR(hresult, L"getRenderTarget -> GetDepthStencilSurface");
    dsv = IDepthStencilViewPtr(new DepthStencilViewDx9 (dsvDesc, D3Dsurface));
    LM_D3D_RELEASE(D3Dsurface);
}

void DeviceDx9::clearCurrentRenderTargetView (const RGBA& color, float depth, int32 stencil, int32 flags)
{
    int32 clearFlags = 0;

    if(flags & CLEAR_COLOR_BUFFER) clearFlags |= D3DCLEAR_TARGET;
    if(flags & CLEAR_DEPTH_BUFFER) clearFlags |= D3DCLEAR_ZBUFFER;
    if(flags & CLEAR_STENCIL_BUFFER) clearFlags |= D3DCLEAR_STENCIL;

    HRESULT hresult;
    bool mustDisableSRGB = _currentState.pBlend->getDesc().sRGBWriteEnabled;

    if(mustDisableSRGB)
    {
        hresult =  _pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, FALSE);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SRGBWRITEENABLE before clear");
    }

    D3DCOLOR D3Dcolor = D3DCOLOR_RGBA((int)(255.0 * color.r), (int)(255.0 * color.g), (int)(255.0 * color.b), (int)(255.0 * color.a));
    hresult = _pDevice->Clear(0, NULL, clearFlags, D3Dcolor, depth, stencil);
    LM_HANDLE_D3D_ERROR(hresult, L"Clear");

    if(mustDisableSRGB)
    {
        hresult =  _pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, TRUE);
        LM_HANDLE_D3D_ERROR(hresult, L"SetRenderState D3DRS_SRGBWRITEENABLE after clear");
    }
}

void DeviceDx9::getCaps(GraphicCapabilities & caps)
{
    caps = _caps;
}

void DeviceDx9::setViewport(const Viewport& vp)
{
    D3DVIEWPORT9 D3Dvp = D3DTypeConvert(vp);
    HRESULT hresult = _pDevice->SetViewport(&D3Dvp);
    LM_HANDLE_D3D_ERROR(hresult, L"SetViewport");
}


ITexture1DPtr DeviceDx9::createTexture1D(const Texture1DDesc& desc, const SubresourceData* pInitData)
{
    return Ptr<ITexture1D>(new Texture1DDx9(desc, _pDevice));
}

ITexture2DPtr DeviceDx9::createTexture2D(const Texture2DDesc& desc, const SubresourceData* pInitDataL)
{
    Ptr<ITexture2D> pTex2D;

    switch (desc.bind)
    {
        case TB_DEPTH_STENCIL_SURFACE:
            pTex2D = (ITexture2DPtr) (new SurfaceDx9 (desc, _pDevice));
        break;

        case TB_RENDER_TARGET:
        case TB_DEPTH_STENCIL_TEXTURE:
        case TB_BUFFER_DEFAULT:
            pTex2D = (ITexture2DPtr) (new Texture2DDx9 (desc, _pDevice));
        break;
    }

    return pTex2D;
}

ITexture3DPtr DeviceDx9::createTexture3D(const Texture3DDesc& desc, const SubresourceData* pInitData)
{
    return Ptr<ITexture3D>(new Texture3DDx9(desc, _pDevice));
}

ITextureCubePtr DeviceDx9::createTextureCube(const TextureCubeDesc& desc, const SubresourceData* pInitData [6])
{
    return ITextureCubePtr(new TextureCubeDx9(desc, _pDevice));
}

void DeviceDx9::PSSetShaderResources(const int32 startSlot, const int32& nbViews, IShaderResourceViewPtr* pShaderResourceViews)
{
    HRESULT hresult;

    for (int ii=0; ii<nbViews; ii++)
    {
        hresult = _pDevice->SetTexture(ii+startSlot, (*(ShaderResourceViewDx9Ptr*)pShaderResourceViews)->_pD3DBaseTexture);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : PSSetShaderResources");
    }
}

void DeviceDx9::VSSetShaderResources(const int32 startSlot, const int32& nbViews, IShaderResourceViewPtr* pShaderResourceViews)
{
    HRESULT hresult;

    for (int ii=0; ii<nbViews; ii++)
    {
        //D3DVERTEXTEXTURESAMPLER0
        hresult = _pDevice->SetTexture(ii+startSlot+(D3DDMAPSAMPLER+1), (*(ShaderResourceViewDx9Ptr*)pShaderResourceViews)->_pD3DBaseTexture);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : VSSetShaderResources");
    }
}

void DeviceDx9::setFixedFuncPipe(const RSFixedFunc & fixed)
{
    HRESULT hresult = S_OK;

    hresult = _pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&fixed.world.getTransposed());
    LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTransform D3DTS_WORLD");

    hresult = _pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&fixed.view.getTransposed());
    LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTransform D3DTS_VIEW");

    hresult = _pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&fixed.proj.getTransposed());
    LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTransform D3DTS_PROJECTION");

    if(fixed.pTex0 != null)
    {
        Ptr<ShaderResourceViewDx9> pRes = LM_DEBUG_PTR_CAST<ShaderResourceViewDx9>(fixed.pTex0);
        hresult = _pDevice->SetTexture(0, pRes->_pD3DBaseTexture);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTexture0");
    }
    else
    {
        hresult = _pDevice->SetTexture(0, NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTexture0");
    }

    if(fixed.pTex1 != null)
    {
        Ptr<ShaderResourceViewDx9> pRes = LM_DEBUG_PTR_CAST<ShaderResourceViewDx9>(fixed.pTex1);
        hresult = _pDevice->SetTexture(1, pRes->_pD3DBaseTexture);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTexture1");
    }
    else
    {
        hresult = _pDevice->SetTexture(1, NULL);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTexture1");
    }

    if(!_fixedFuncInit)
    {
        hresult = _pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTextureStageState Ao");

        hresult = _pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTextureStageState A1");

        hresult = _pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : SetTextureStageState A2");
        _fixedFuncInit = true;
    }

    if(fixed.fogEnabled != _fixed.fogEnabled)
    {
        hresult = _pDevice->SetRenderState(D3DRS_FOGENABLE, fixed.fogEnabled);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : FogEnable");
    }

    if(fixed.fogColor.getXYZ() != _fixed.fogColor.getXYZ())
    {
        D3DCOLOR color = D3DCOLOR_RGBA((int)(255.0 * fixed.fogColor.r), (int)(255.0 * fixed.fogColor.g), (int)(255.0 * fixed.fogColor.b), 0);
        hresult = _pDevice->SetRenderState(D3DRS_FOGCOLOR, color);
        LM_HANDLE_D3D_ERROR(hresult, L"DeviceDx9 : FogColor");
    }

    _fixed.fogEnabled = fixed.fogEnabled;
    _fixed.fogColor = fixed.fogColor;
}

Ptr<IFont> DeviceDx9::createFont(const FontDesc & desc)
{
    return Ptr<IFont>(new FontDX9(_pDevice, desc));
}

void DeviceDx9::copyRenderTargets(const Ptr<IRenderTargetView> & pSrc, const Ptr<IRenderTargetView> & pDst)
{
    RenderTargetViewDx9Ptr pSrcDX9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9>(pSrc);
    RenderTargetViewDx9Ptr pDstDX9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9>(pDst);

    HRESULT hresult = _pDevice->StretchRect(pSrcDX9->_D3Dcanvas, NULL, pDstDX9->_D3Dcanvas, NULL, D3DTEXF_NONE);
    LM_HANDLE_D3D_ERROR(hresult, L"copyRenderTargets : StretchRect");
}

void DeviceDx9::updateTexture(const Ptr<ITexture1D> & pSrc, const Ptr<ITexture1D> & pDst)
{
    Ptr<Texture1DDx9> pSrcDX9 = LM_DEBUG_PTR_CAST<Texture1DDx9>(pSrc);
    Ptr<Texture1DDx9> pDstDX9 = LM_DEBUG_PTR_CAST<Texture1DDx9>(pDst);

    HRESULT hresult = pSrcDX9->_pD3DTexture->AddDirtyRect(NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : AddDirtyRect");

    hresult = _pDevice->UpdateTexture(pSrcDX9->_pD3DTexture, pDstDX9->_pD3DTexture);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : UpdateTexture");
}

void DeviceDx9::updateTexture(const Ptr<ITexture2D> & pSrc, const Ptr<ITexture2D> & pDst)
{
    Ptr<Texture2DDx9> pSrcDX9 = LM_DEBUG_PTR_CAST<Texture2DDx9>(pSrc);
    Ptr<Texture2DDx9> pDstDX9 = LM_DEBUG_PTR_CAST<Texture2DDx9>(pDst);

    HRESULT hresult = pSrcDX9->_pD3DTexture->AddDirtyRect(NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : AddDirtyRect");

    hresult = _pDevice->UpdateTexture(pSrcDX9->_pD3DTexture, pDstDX9->_pD3DTexture);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : UpdateTexture");
}

void DeviceDx9::updateTexture(const Ptr<ITexture3D> & pSrc, const Ptr<ITexture3D> & pDst)
{
    Ptr<Texture3DDx9> pSrcDX9 = LM_DEBUG_PTR_CAST<Texture3DDx9>(pSrc);
    Ptr<Texture3DDx9> pDstDX9 = LM_DEBUG_PTR_CAST<Texture3DDx9>(pDst);

    HRESULT hresult = pSrcDX9->_pD3DVolumeTexture->AddDirtyBox(NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : AddDirtyBox");

    hresult = _pDevice->UpdateTexture(pSrcDX9->_pD3DVolumeTexture, pDstDX9->_pD3DVolumeTexture);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : UpdateTexture");
}

void DeviceDx9::updateTexture(const Ptr<ITextureCube> & pSrc, const Ptr<ITextureCube> & pDst)
{
    Ptr<TextureCubeDx9> pSrcDX9 = LM_DEBUG_PTR_CAST<TextureCubeDx9>(pSrc);
    Ptr<TextureCubeDx9> pDstDX9 = LM_DEBUG_PTR_CAST<TextureCubeDx9>(pDst);

    HRESULT hresult;
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_POSITIVE_X, NULL);
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_NEGATIVE_X, NULL);
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_POSITIVE_Y, NULL);
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_NEGATIVE_Y, NULL);
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_POSITIVE_Z, NULL);
    hresult = pSrcDX9->_pD3DTextureCube->AddDirtyRect(D3DCUBEMAP_FACE_NEGATIVE_Z, NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : AddDirtyRect");

    hresult = _pDevice->UpdateTexture(pSrcDX9->_pD3DTextureCube, pDstDX9->_pD3DTextureCube);
    LM_HANDLE_D3D_ERROR(hresult, L"updateTexture : UpdateTexture");
}

Ptr<Core::Bitmap> DeviceDx9::toBitmap(const IRenderTargetViewPtr& pRtv)
{
    RenderTargetViewDx9Ptr rtvDx9 = LM_DEBUG_PTR_CAST<RenderTargetViewDx9> (pRtv);
    D3DSURFACE_DESC surfaceDesc;
    HRESULT hresult;
    LPDIRECT3DSURFACE9 pD3Dsurface;

    hresult = rtvDx9->_D3Dcanvas->GetDesc(&surfaceDesc);
    LM_HANDLE_D3D_ERROR(hresult, L"RenderTargetView_to_MemoryImage : GetDesc");

    hresult = _pDevice->CreateOffscreenPlainSurface(surfaceDesc.Width, surfaceDesc.Height, surfaceDesc.Format, D3DPOOL_SYSTEMMEM, &pD3Dsurface, NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"RenderTargetView_to_MemoryImage : CreateOffscreenPlainSurface");

    _pDevice->GetRenderTargetData(rtvDx9->_D3Dcanvas, pD3Dsurface);

    D3DLOCKED_RECT lockedRect;
    pD3Dsurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);

    Core::EBitmapFormat format = D3DTypeConvert(surfaceDesc.Format);
    int32 byteCount = lockedRect.Pitch * surfaceDesc.Height;
    byte * pData = new byte[byteCount];

    LM_ASSERT((surfaceDesc.Width * surfaceDesc.Height * Core::getBytesPerPixel(format)) == byteCount);
    Ptr<Core::Bitmap> pResult(new Core::Bitmap(pData, surfaceDesc.Width, surfaceDesc.Height, format, lockedRect.Pitch, true));

    memcpy(pData, lockedRect.pBits, byteCount);
    pD3Dsurface->UnlockRect();

    LM_D3D_RELEASE(pD3Dsurface);

    return pResult;
}

Ptr<Core::Bitmap> DeviceDx9::toBitmap(const IDepthStencilViewPtr& pDtv)
{
    throw Core::Exception(L"fonction non valide pour directx 9");
    return null;
}


EDeviceState DeviceDx9::getDeviceState()
{
    EDeviceState state = DS_DEVICE_LOST;
    HRESULT hresult = _pDevice->TestCooperativeLevel();
    switch(hresult)
    {
    case D3D_OK:
        state = DS_DEVICE_OK;
        break;
    case D3DERR_DEVICELOST:
        state = DS_DEVICE_LOST;        
        break;    
    case D3DERR_DEVICENOTRESET:        
        state = DS_DEVICE_NOT_RESET;        
        break;    
    case D3DERR_DRIVERINTERNALERROR:        
        state = DS_DEVICE_ERROR;        
        break;    
    default:        
        ERR << L"Unexpected DX9 TestCooperativeLevel() result : " << Core::toStringHex(hresult) << L"\n";        
        state = DS_DEVICE_ERROR;        
        break;    
    }
    return state;
}

ISwapChainPtr DeviceDx9::reset(const SwapChainDesc & desc)
{
#if LM_WATCH_TEX_FOR_RESET
    for(int32 ii=0; ii < Texture2DDx9::sTextures.size(); ii++)
    {
        ERR << L"Following texture was not reset :\n" << Texture2DDx9::sTextures[ii]->_cs << L"\n\n";
    }
#endif

    D3DPRESENT_PARAMETERS presentParam;
    D3DTypeConvert(presentParam, desc);
    HRESULT hresult = _pDevice->Reset(&presentParam);
    LM_HANDLE_D3D_ERROR(hresult, L"Device Reset");

    LPDIRECT3DSWAPCHAIN9 pD3DswapChain;
    hresult = _pDevice->GetSwapChain(0, &pD3DswapChain);
    LM_HANDLE_D3D_ERROR(hresult, L"restoreDevice -> GetSwapChain");
    setDefaultState();

    _fixedFuncInit = false;

    int32 frameBufferMem = desc.width * desc.height * 4 * (1 << int32(desc.multiSamplingType));
    _totalVideoMem = getAvailableVideoMem() + (frameBufferMem >> 10);

    return ISwapChainPtr(new SwapChainDx9(desc, pD3DswapChain, _pDevice));
}

void DeviceDx9::setDefaultState()
{
    ///////////////////
    //    RenderState    
    ///////////////////    
    //Note : On initialise les etat actuelle à des valeurs erronnées pour initialiser correctement    
    //les etats par les appel setState    
    RSRasterizerDesc   rasterizerDescBidon(INVALID);    
    RSBlendDesc        blendDescBidon(INVALID);    
    RSSamplerDesc      samplerDescBidon(INVALID);    
    RSDepthStencilDesc depthStencilDescBidon(INVALID);

    _currentState.pRasterizer = RSRasterizerDx9Ptr (new RSRasterizerDx9 (rasterizerDescBidon));
    _currentState.pBlend = RSBlendDx9Ptr (new RSBlendDx9 (blendDescBidon));   
    _currentState.pDepthStencil = RSDepthStencilDx9Ptr (new RSDepthStencilDx9 (depthStencilDescBidon));

    for(int32 ii=0; ii < MAX_RS_SAMPLER; ii++)
        _currentState.ptSampler[ii] = RSSamplerDx9Ptr (new RSSamplerDx9 (samplerDescBidon));

    IRSRasterizerPtr   pDefaultRaterizer(new RSRasterizerDx9 (RSRasterizerDesc()));
    IRSBlendPtr        pDefaultBlend(new RSBlendDx9 (RSBlendDesc()));
    IRSDepthStencilPtr pDefaultDepthStencil(new RSDepthStencilDx9 (RSDepthStencilDesc()));

    setState(pDefaultRaterizer);
    setState(pDefaultBlend);    
    setState(pDefaultDepthStencil);   

    for(int32 ii=0; ii < MAX_RS_SAMPLER; ii++)    
    {
        IRSSamplerPtr pDefaultSampler(new RSSamplerDx9 (RSSamplerDesc()));
        setState(ii, pDefaultSampler);
    }

    for(int32 ii=0; ii < MAX_SAMPLERS; ii++)
    {
        _minFilter[ii] = -1;
        _magFilter[ii] = -1;
        _mipFilter[ii] = -1;        
        _maxAniso[ii] = -1;        
        _samplerUsed[ii] = false;    
    }

    for(int32 ii=0; ii < D3DMAXUSERCLIPPLANES; ii++)
        _clipPlaneEnabled[ii] = false;
}

int64 DeviceDx9::getTotalVideoMem()
{
    return _totalVideoMem;
}

int64 DeviceDx9::getAvailableVideoMem()
{
    return int64(_pDevice->GetAvailableTextureMem());
}

void DeviceDx9::startStats()
{
    _stats.triCount = 0;
    _stats.setVertexDeclCount = 0;
    _stats.setVSConstCount = 0;
    _stats.setPSConstCount = 0;
    _stats.vsSwitchCount = 0;
    _stats.psSwitchCount = 0;
}

void DeviceDx9::endStats()
{
}

const GfxStats & DeviceDx9::getStats() const
{
    return _stats;
}

} // namespace Gfx
