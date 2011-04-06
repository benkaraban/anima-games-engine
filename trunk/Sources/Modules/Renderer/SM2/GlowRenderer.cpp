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
#include <Renderer/SM2/GlowRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
GlowRenderer::GlowRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pScreenQuad(pScreenQuad)
{
}
//-----------------------------------------------------------------------------
bool GlowRenderer::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pPShaderBloom = _pShaderLib->getPShader(L"bloom.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idThresholdBloom = _pPShaderBloom->getConstantTable()->getConstantIndex(L"BloomThreshold");
        _idFactorBloom = _pPShaderBloom->getConstantTable()->getConstantIndex(L"BloomFactor");
        _idSamplerBloom = _pPShaderBloom->getConstantTable()->getConstantIndex(L"SamplerTexture");

        _pPShaderGlow = _pShaderLib->getPShader(L"glow.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSamplerGlow1 = _pPShaderGlow->getConstantTable()->getConstantIndex(L"SamplerTexture1");
        _idSamplerGlow2 = _pPShaderGlow->getConstantTable()->getConstantIndex(L"SamplerTexture2");

        _pPShaderScale = _pShaderLib->getPShader(L"passthru-tex.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSamplerScale = _pPShaderScale->getConstantTable()->getConstantIndex(L"SamplerTexture");

        _pPShaderBlurH = _pShaderLib->getPShader(L"blur-h.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSamplerBlurH = _pPShaderBlurH->getConstantTable()->getConstantIndex(L"SamplerImage");
        _idScaleH = _pPShaderBlurH->getConstantTable()->getConstantIndex(L"Scale");

        _pPShaderBlurV = _pShaderLib->getPShader(L"blur-v.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSamplerBlurV = _pPShaderBlurV->getConstantTable()->getConstantIndex(L"SamplerImage");
        _idScaleV = _pPShaderBlurV->getConstantTable()->getConstantIndex(L"Scale");

        // States

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;
        Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

        sampler.isSRGB = true;
        blend.sRGBWriteEnabled = true;

        _stateBlurH = _pDevice->createState(raster, depth, blend);
        _stateBlurV = _pDevice->createState(raster, depth, blend);
        _stateScale = _pDevice->createState(raster, depth, blend);
        _stateGlow = _pDevice->createState(raster, depth, blend);

        blend = Gfx::RSBlendDesc(Gfx::BM_ONE, Gfx::BO_ADD, Gfx::BM_ONE);
        blend.sRGBWriteEnabled = true;
        _stateBloom = _pDevice->createState(raster, depth, blend);

        Ptr<Gfx::IRSSampler> pSamplerState(_pDevice->createState(sampler));

        _stateBlurH.ptSampler[_pPShaderBlurH->getConstantTable()->getSamplerId(_idSamplerBlurH)] = pSamplerState;
        _stateBlurV.ptSampler[_pPShaderBlurV->getConstantTable()->getSamplerId(_idSamplerBlurV)] = pSamplerState;
        _stateGlow.ptSampler[_pPShaderGlow->getConstantTable()->getSamplerId(_idSamplerGlow1)] = pSamplerState;
        _stateGlow.ptSampler[_pPShaderGlow->getConstantTable()->getSamplerId(_idSamplerGlow2)] = pSamplerState;
        _stateScale.ptSampler[_pPShaderScale->getConstantTable()->getSamplerId(_idSamplerScale)] = pSamplerState;
        _stateBloom.ptSampler[_pPShaderBloom->getConstantTable()->getSamplerId(_idSamplerBloom)] = pSamplerState;
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing GlowRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void GlowRenderer::startGlowPass(const RenderContext & context)
{
    _light = context.getLight();
    _pRenderTarget = context.getRenderTarget();
    _pRenderTarget->updateRefractionBuffer(true);
    _pRenderTarget->bind(RT_GLOW0_BUFFER);
    _pDevice->clearCurrentRenderTargetView(Core::Vector4f(0.0, 0.0, 0.0, 0.0), 1.0f, 0, Gfx::CLEAR_COLOR_BUFFER);
}
//-----------------------------------------------------------------------------
void GlowRenderer::endGlowPass()
{
    // Bloom

    int32 width = _pRenderTarget->getViewportWidth(RT_GLOW0_BUFFER);
    int32 height = _pRenderTarget->getViewportHeight(RT_GLOW0_BUFFER);

    _pDevice->setState(_stateBloom);
    _pDevice->setPixelShader(_pPShaderBloom);

    _pPShaderBloom->getConstantTable()->setConstant(_idThresholdBloom, _light.getBloomThreshold());
    _pPShaderBloom->getConstantTable()->setConstant(_idFactorBloom, _light.getBloomFactor());
    _pPShaderBloom->getConstantTable()->setSampler2D(_idSamplerBloom, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));

    _pScreenQuad->send(width, height);

    // Scale x4

    int32 glowHalfWidth = _pRenderTarget->getViewportWidth(RT_GLOW_HALF_BUFFER);
    int32 glowHalfHeight = _pRenderTarget->getViewportHeight(RT_GLOW_HALF_BUFFER);

    _pDevice->setState(_stateScale);
    _pRenderTarget->bind(RT_GLOW_HALF_BUFFER);
    _pDevice->setPixelShader(_pPShaderScale);
    _pPShaderScale->getConstantTable()->setSampler2D(_idSamplerScale, _pRenderTarget->getShaderTextureView(RT_GLOW0_BUFFER));
    _pScreenQuad->send(glowHalfWidth, glowHalfHeight);

    // Scale x16

    int32 glow1Width = _pRenderTarget->getViewportWidth(RT_GLOW1_BUFFER);
    int32 glow1Height = _pRenderTarget->getViewportHeight(RT_GLOW1_BUFFER);

    _pDevice->setState(_stateScale);
    _pRenderTarget->bind(RT_GLOW1_BUFFER);
    _pDevice->setPixelShader(_pPShaderScale);
    _pPShaderScale->getConstantTable()->setSampler2D(_idSamplerScale, _pRenderTarget->getShaderTextureView(RT_GLOW_HALF_BUFFER));
    _pScreenQuad->send(glow1Width, glow1Height);

    // Blur

    int32 glow2Width = _pRenderTarget->getViewportWidth(RT_GLOW2_BUFFER);
    int32 glow2Height = _pRenderTarget->getViewportHeight(RT_GLOW2_BUFFER);

    float scaleH = 1.0f / std::max(1, glow2Width);
    float scaleV = 1.0f / std::max(1, glow1Height);

    _pDevice->setState(_stateBlurH);
    _pRenderTarget->bind(RT_GLOW2_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurH);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurH, _pRenderTarget->getShaderTextureView(RT_GLOW_HALF_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleH, scaleH);
    _pScreenQuad->send(glow2Width, glow2Height);

    _pDevice->setState(_stateBlurV);
    _pRenderTarget->bind(RT_GLOW1_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurV);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurV, _pRenderTarget->getShaderTextureView(RT_GLOW2_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleV, scaleV);
    _pScreenQuad->send(glow1Width, glow1Height);

    _pDevice->setState(_stateBlurH);
    _pRenderTarget->bind(RT_GLOW2_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurH);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurH, _pRenderTarget->getShaderTextureView(RT_GLOW1_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleH, scaleH);
    _pScreenQuad->send(glow2Width, glow2Height);

    _pDevice->setState(_stateBlurV);
    _pRenderTarget->bind(RT_GLOW1_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurV);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurV, _pRenderTarget->getShaderTextureView(RT_GLOW2_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleV, scaleV);
    _pScreenQuad->send(glow1Width, glow1Height);

    _pDevice->setState(_stateBlurH);
    _pRenderTarget->bind(RT_GLOW2_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurH);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurH, _pRenderTarget->getShaderTextureView(RT_GLOW1_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleH, scaleH);
    _pScreenQuad->send(glow2Width, glow2Height);

    _pDevice->setState(_stateBlurV);
    _pRenderTarget->bind(RT_GLOW1_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurV);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurV, _pRenderTarget->getShaderTextureView(RT_GLOW2_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleV, scaleV);
    _pScreenQuad->send(glow1Width, glow1Height);

    _pDevice->setState(_stateBlurH);
    _pRenderTarget->bind(RT_GLOW2_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurH);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurH, _pRenderTarget->getShaderTextureView(RT_GLOW1_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleH, scaleH);
    _pScreenQuad->send(glow2Width, glow2Height);

    _pDevice->setState(_stateBlurV);
    _pRenderTarget->bind(RT_GLOW1_BUFFER);
    _pDevice->setPixelShader(_pPShaderBlurV);
    _pPShaderBlurH->getConstantTable()->setSampler2D(_idSamplerBlurV, _pRenderTarget->getShaderTextureView(RT_GLOW2_BUFFER));
    _pPShaderBlurH->getConstantTable()->setConstant(_idScaleV, scaleV);
    _pScreenQuad->send(glow1Width, glow1Height);

    // Add glow

    int32 finalWidth = _pRenderTarget->getViewportWidth(RT_LIGHT_BUFFER);
    int32 finalHeight = _pRenderTarget->getViewportHeight(RT_LIGHT_BUFFER);

    _pRenderTarget->bind(RT_LIGHT_BUFFER);

    _pDevice->setState(_stateGlow);
    _pDevice->setPixelShader(_pPShaderGlow);
    _pPShaderGlow->getConstantTable()->setSampler2D(_idSamplerGlow1, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
    _pPShaderGlow->getConstantTable()->setSampler2D(_idSamplerGlow2, _pRenderTarget->getShaderTextureView(RT_GLOW1_BUFFER));
    _pScreenQuad->send(finalWidth, finalHeight);
}
//-----------------------------------------------------------------------------
}
