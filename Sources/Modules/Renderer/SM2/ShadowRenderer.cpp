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
#include <Renderer/Common/Texture.h>
#include <Renderer/SM2/ShadowRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
ShadowRenderer::ShadowRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                               const Ptr<ScreenQuad> & pScreenQuad, const Ptr<GPUResourceLib> & pResLib,
                               EShadowLevel shadowLevel) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pScreenQuad(pScreenQuad),
    _pResLib(pResLib),
    _dynamicShadowLevel(shadowLevel),
    _staticShadowMapSize(8192)
{
    switch(_dynamicShadowLevel)
    {
    case SHADOW_NONE:
    case SHADOW_LOW:
        _dynamicShadowMapSize = 512;
        _dynamicBlurPassCount = 0;
        break;
    case SHADOW_MEDIUM:
        _dynamicShadowMapSize = 512;
        _dynamicBlurPassCount = 1;
        break;
    case SHADOW_HIGH:
        _dynamicShadowMapSize = 1024;
        _dynamicBlurPassCount = 1;
        break;
    }

    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
ShadowRenderer::~ShadowRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool ShadowRenderer::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders

        Core::List<Gfx::ShaderMacro> macros;

        macros.push_back(Gfx::ShaderMacro(L"GAUSSIAN_FLAG", L"1"));

        if(_dynamicShadowMapSize >= 1024)
            _pPShaderBlur = _pShaderLib->getPShader(L"blur-5.psh", Gfx::PS_V2_0, L"ps_main", macros);
        else
            _pPShaderBlur = _pShaderLib->getPShader(L"blur-3.psh", Gfx::PS_V2_0, L"ps_main", macros);

        _idSamplerBlur = _pPShaderBlur->getConstantTable()->getConstantIndex(L"SamplerImage");
        _idDirectionScale = _pPShaderBlur->getConstantTable()->getConstantIndex(L"DirectionScale");

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;
        Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_POINT, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

        sampler.isSRGB = false;
        blend.sRGBWriteEnabled = false;

        _stateBlur = _pDevice->createState(raster, depth, blend);

        Ptr<Gfx::IRSSampler> pSamplerState(_pDevice->createState(sampler));

        _stateBlur.ptSampler[_pPShaderBlur->getConstantTable()->getSamplerId(_idSamplerBlur)] = pSamplerState;
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing ShadowRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void ShadowRenderer::onDeviceLost()
{
    _pDynamicCurView = null;
    _pDynamicCurTexture = null;

    _pDynamicShadowView0 = null;
    _pDynamicShadowView1 = null;
    _pDynamicShadowView2 = null;

    _pDynamicShadowTexture0 = null;
    _pDynamicShadowTexture1 = null;
    _pDynamicShadowTexture2 = null;

    _pDynamicShadowDepthView = null;
    _pDynamicShadowDepthTex = null;
}
//-----------------------------------------------------------------------------
void ShadowRenderer::onDeviceReset()
{
}
//-----------------------------------------------------------------------------
void ShadowRenderer::setContext(const RenderContext & context)
{
    if(Core::isEquivalent(context.getDynamicShadowSplitSize1(), 0.0f))
        _dynamicShadowWidthMul1 = 0.0f;
    else
        _dynamicShadowWidthMul1 = context.getDynamicShadowSplitSize0() / context.getDynamicShadowSplitSize1();
}
//-----------------------------------------------------------------------------
void ShadowRenderer::startDynamicShadowMapRendering(int32 split)
{
    if(_pDynamicShadowTexture0 == null)
    {
        Gfx::Texture2DDesc texDesc;
        texDesc.width = _dynamicShadowMapSize;
        texDesc.height = _dynamicShadowMapSize;
        texDesc.mipLevels = 1;
        texDesc.format = Gfx::TF_A8R8G8B8;
        texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
        texDesc.bind = Gfx::TB_RENDER_TARGET;
        texDesc.autoGenMipmap = false;

        _pDynamicShadowTexture0 = _pDevice->createTexture2D(texDesc);
        _pDynamicShadowTexture1 = _pDevice->createTexture2D(texDesc);
        _pDynamicShadowTexture2 = _pDevice->createTexture2D(texDesc);

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        _pDynamicShadowView0 = _pDevice->createRenderTargetView(_pDynamicShadowTexture0, rtDesc);
        _pDynamicShadowView1 = _pDevice->createRenderTargetView(_pDynamicShadowTexture1, rtDesc);
        _pDynamicShadowView2 = _pDevice->createRenderTargetView(_pDynamicShadowTexture2, rtDesc);

        texDesc.format = Gfx::TF_D24S8;
        texDesc.bind = Gfx::TB_DEPTH_STENCIL_SURFACE;

        _pDynamicShadowDepthTex = _pDevice->createTexture2D(texDesc);

        Gfx::DepthStencilViewDesc dsDesc;
        dsDesc.texture2D.mipSlice = 0;

        _pDynamicShadowDepthView = _pDevice->createDepthStencilView(_pDynamicShadowDepthTex, dsDesc);
    }

    _dynamicCurSplit = split;

    switch(split)
    {
    case 0: 
        _pDynamicCurTexture = _pDynamicShadowTexture0;
        _pDynamicCurView = _pDynamicShadowView0;
        _pDevice->setRenderTarget(_pDynamicShadowView0, _pDynamicShadowDepthView); 
        break;
    case 1: 
        _pDynamicCurTexture = _pDynamicShadowTexture1;
        _pDynamicCurView = _pDynamicShadowView1;
        _pDevice->setRenderTarget(_pDynamicShadowView1, _pDynamicShadowDepthView); 
        break;
    default:
        LM_ASSERT(false);
        break;
    }
}
//-----------------------------------------------------------------------------
void ShadowRenderer::endDynamicShadowMapRendering()
{
    float scale = 1.00f;
/*
    switch(_curSplit)
    {
    case 0: break;
    case 1: scale = _shadowWidthMul1; break;
    default:
        LM_ASSERT(false);
        break;
    }*/

    float scaleH = scale / std::max(1, _dynamicShadowMapSize);
    float scaleV = scale / std::max(1, _dynamicShadowMapSize);

    _pDevice->setState(_stateBlur);
    _pDevice->setPixelShader(_pPShaderBlur);

    for(int32 iPass=0; iPass < _dynamicBlurPassCount; iPass++)
    {
        _pDevice->setRenderTarget(_pDynamicShadowView2, _pDynamicShadowDepthView);
        _pPShaderBlur->getConstantTable()->setSampler2D(_idSamplerBlur, _pDynamicCurTexture->getShaderResourceView());
        _pPShaderBlur->getConstantTable()->setConstant(_idDirectionScale, Core::Vector2f(scaleH, 0.0f));
        _pScreenQuad->send(_dynamicShadowMapSize, _dynamicShadowMapSize);

        _pDevice->setRenderTarget(_pDynamicCurView, _pDynamicShadowDepthView);
        _pPShaderBlur->getConstantTable()->setSampler2D(_idSamplerBlur, _pDynamicShadowTexture2->getShaderResourceView());
        _pPShaderBlur->getConstantTable()->setConstant(_idDirectionScale, Core::Vector2f(0.0f, scaleV));
        _pScreenQuad->send(_dynamicShadowMapSize, _dynamicShadowMapSize);
    }
}
//-----------------------------------------------------------------------------
Ptr<Gfx::ITexture2D> ShadowRenderer::getDynamicShadowMap(int32 split) const
{
    switch(split)
    {
    case 0: return _pDynamicShadowTexture0; break;
    case 1: return _pDynamicShadowTexture1; break;
    default:
        LM_ASSERT(false);
        break;
    }
    
    return _pDynamicShadowTexture0; 
}
//-----------------------------------------------------------------------------
void ShadowRenderer::startStaticShadowMapRendering()
{
    if(_pStaticShadowTexture0 == null)
    {
        Gfx::Texture2DDesc texDesc;
        texDesc.width = _staticShadowMapSize;
        texDesc.height = _staticShadowMapSize;
        texDesc.mipLevels = 1;
        texDesc.format = Gfx::TF_R32F;
        texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
        texDesc.bind = Gfx::TB_RENDER_TARGET;
        texDesc.autoGenMipmap = false;

        _pStaticShadowTexture0 = _pDevice->createTexture2D(texDesc);

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        _pStaticShadowView0 = _pDevice->createRenderTargetView(_pStaticShadowTexture0, rtDesc);

        texDesc.format = Gfx::TF_D24S8;
        texDesc.bind = Gfx::TB_DEPTH_STENCIL_SURFACE;

        _pStaticShadowDepthTex = _pDevice->createTexture2D(texDesc);

        Gfx::DepthStencilViewDesc dsDesc;
        dsDesc.texture2D.mipSlice = 0;

        _pStaticShadowDepthView = _pDevice->createDepthStencilView(_pStaticShadowDepthTex, dsDesc);
    }

    _pStaticCurView = _pStaticShadowView0;
    _pDevice->setRenderTarget(_pStaticCurView, _pStaticShadowDepthView); 
}
//-----------------------------------------------------------------------------
void ShadowRenderer::endStaticShadowMapRendering()
{
}
//-----------------------------------------------------------------------------
Ptr<Gfx::ITexture2D> ShadowRenderer::getStaticShadowMap() const
{
    return _pStaticShadowTexture0;
}
//-----------------------------------------------------------------------------
}
