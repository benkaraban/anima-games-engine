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
#include <Core/Math/Interpolate.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/SM2/PostFXRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
PostFXRenderer::PostFXRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad,
                               const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pScreenQuad(pScreenQuad),
    _settings(settings)
{
}
//-----------------------------------------------------------------------------
bool PostFXRenderer::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pPShaderRadialBlur = _pShaderLib->getPShader(L"radialblur-9.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idCenterRadialBlur = _pPShaderRadialBlur->getConstantTable()->getConstantIndex(L"Center");
        _idScaleRadialBlur = _pPShaderRadialBlur->getConstantTable()->getConstantIndex(L"Scale");
        _idSamplerRadialBlur = _pPShaderRadialBlur->getConstantTable()->getConstantIndex(L"SamplerImage");

        _pPShaderScale = _pShaderLib->getPShader(L"passthru-tex.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSamplerScale = _pPShaderScale->getConstantTable()->getConstantIndex(L"SamplerTexture");

        _pPShaderAccum = _pShaderLib->getPShader(L"accum.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSourceFactorAccum = _pPShaderAccum->getConstantTable()->getConstantIndex(L"SourceFactor");
        _idSamplerAccum = _pPShaderAccum->getConstantTable()->getConstantIndex(L"SamplerTexture");

        _pPShaderPostFX = _pShaderLib->getPShader(L"postfx.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idBlurFactorPostFX = _pPShaderPostFX->getConstantTable()->getConstantIndex(L"BlurFactor");
        _idSaturationPostFX = _pPShaderPostFX->getConstantTable()->getConstantIndex(L"Saturation");
        _idFadePostFX = _pPShaderPostFX->getConstantTable()->getConstantIndex(L"Fade");
        _idSamplerTexturePostFX = _pPShaderPostFX->getConstantTable()->getConstantIndex(L"SamplerTexture");
        _idSamplerBlurPostFX = _pPShaderPostFX->getConstantTable()->getConstantIndex(L"SamplerBlur");

        macros.push_back(Gfx::ShaderMacro(L"GAUSSIAN_FLAG", L"1"));

        _pPShaderBlur = _pShaderLib->getPShader(L"blur-9.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idDirectionScaleBlur = _pPShaderBlur->getConstantTable()->getConstantIndex(L"DirectionScale");
        _idSamplerBlur = _pPShaderBlur->getConstantTable()->getConstantIndex(L"SamplerImage");


        // States

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;
        Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

        sampler.isSRGB = true;
        blend.sRGBWriteEnabled = true;

        _stateBlur = _pDevice->createState(raster, depth, blend);
        _stateRadialBlur = _pDevice->createState(raster, depth, blend);
        _stateScale = _pDevice->createState(raster, depth, blend);
        _statePostFX = _pDevice->createState(raster, depth, blend);

        Gfx::RSBlendDesc        blendAccum(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
        blendAccum.sRGBWriteEnabled = true;
        _stateAccum = _pDevice->createState(raster, depth, blendAccum);

        Ptr<Gfx::IRSSampler> pSamplerState(_pDevice->createState(sampler));

        _stateBlur.ptSampler[_pPShaderBlur->getConstantTable()->getSamplerId(_idSamplerBlur)] = pSamplerState;
        _stateRadialBlur.ptSampler[_pPShaderRadialBlur->getConstantTable()->getSamplerId(_idSamplerRadialBlur)] = pSamplerState;
        _stateScale.ptSampler[_pPShaderScale->getConstantTable()->getSamplerId(_idSamplerScale)] = pSamplerState;
        _stateAccum.ptSampler[_pPShaderScale->getConstantTable()->getSamplerId(_idSamplerAccum)] = pSamplerState;
        
        _statePostFX.ptSampler[_pPShaderPostFX->getConstantTable()->getSamplerId(_idSamplerTexturePostFX)] = pSamplerState;
        _statePostFX.ptSampler[_pPShaderPostFX->getConstantTable()->getSamplerId(_idSamplerBlurPostFX)] = pSamplerState;
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing PostFXRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void PostFXRenderer::postFX(const PostFX & fx, const RenderContext & context)
{
    _pRenderTarget = context.getRenderTarget();   

    float blurFactor = std::min(1.0f, std::max(fx.radialBlurFactor, std::max(fx.blurDir1.getLengthSafe(), fx.blurDir2.getLengthSafe())));
    int w = _pRenderTarget->getViewportWidth(RT_LIGHT_BUFFER);
    int h = _pRenderTarget->getViewportHeight(RT_LIGHT_BUFFER);

    bool dirBlurOn = _settings.postFXOn && (fx.blurDir1 != Core::Vector2f::ZERO || fx.blurDir2 != Core::Vector2f::ZERO);
    bool radialBlurOn = _settings.postFXOn &&  (fx.radialBlurFactor > 0.0f);
    bool blurOn = dirBlurOn || radialBlurOn;
    bool doPostFX = blurFactor > 0.0f || fx.saturation < 1.0f || fx.fadeColor.a > 0.0f;

    if(!_settings.postFXOn)
        blurFactor = 0.0f;

    if(doPostFX)
        _pRenderTarget->updateRefractionBuffer(true);

    if(blurOn)
    {
        // Scale x4

        int32 glowHalfWidth = _pRenderTarget->getViewportWidth(RT_GLOW_HALF_BUFFER);
        int32 glowHalfHeight = _pRenderTarget->getViewportHeight(RT_GLOW_HALF_BUFFER);

        _pDevice->setState(_stateScale);
        _pRenderTarget->bind(RT_GLOW_HALF_BUFFER);
        _pDevice->setPixelShader(_pPShaderScale);
        _pPShaderScale->getConstantTable()->setSampler2D(_idSamplerScale, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
        _pScreenQuad->send(glowHalfWidth, glowHalfHeight);

        // Scale x16

        int32 glow1Width = _pRenderTarget->getViewportWidth(RT_GLOW1_BUFFER);
        int32 glow1Height = _pRenderTarget->getViewportHeight(RT_GLOW1_BUFFER);

        _pDevice->setState(_stateScale);
        _pRenderTarget->bind(RT_GLOW1_BUFFER);
        _pDevice->setPixelShader(_pPShaderScale);
        _pPShaderScale->getConstantTable()->setSampler2D(_idSamplerScale, _pRenderTarget->getShaderTextureView(RT_GLOW_HALF_BUFFER));
        _pScreenQuad->send(glow1Width, glow1Height);

        if(dirBlurOn)
        {
            for(int32 ii=0; ii < 1; ii++)
            {
                blur(fx.blurDir1, RT_GLOW1_BUFFER, RT_GLOW2_BUFFER);
                blur(fx.blurDir2, RT_GLOW2_BUFFER, RT_GLOW1_BUFFER);
            }
        }

        if(radialBlurOn)
        {
            for(int32 ii=0; ii < 1; ii++)
            {
                radialBlur(fx.radialBlurCenter, fx.radialBlurFactor, RT_GLOW1_BUFFER, RT_GLOW2_BUFFER);
                radialBlur(fx.radialBlurCenter, fx.radialBlurFactor, RT_GLOW2_BUFFER, RT_GLOW1_BUFFER);
            }
        }
    }

    // Final (presque)

    if(doPostFX)
    {
        _pRenderTarget->bind(RT_LIGHT_BUFFER);

        _pDevice->setState(_statePostFX);
        _pDevice->setPixelShader(_pPShaderPostFX);
        _pPShaderPostFX->getConstantTable()->setConstant(_idBlurFactorPostFX, blurFactor);
        _pPShaderPostFX->getConstantTable()->setConstant(_idSaturationPostFX, fx.saturation);
        _pPShaderPostFX->getConstantTable()->setConstant(_idFadePostFX, fx.fadeColor);
        _pPShaderPostFX->getConstantTable()->setSampler2D(_idSamplerTexturePostFX, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
        _pPShaderPostFX->getConstantTable()->setSampler2D(_idSamplerBlurPostFX, _pRenderTarget->getShaderTextureView(RT_GLOW1_BUFFER));
        _pScreenQuad->send(w, h);
    }
}
//-----------------------------------------------------------------------------
void PostFXRenderer::blur(const Core::Vector2f & dir, ERenderTargetBuffer src, ERenderTargetBuffer dst)
{
    int w = _pRenderTarget->getViewportWidth(dst);
    int h = _pRenderTarget->getViewportHeight(dst);

    Core::Vector2f pixDir(dir);
    pixDir.x /= float(w);
    pixDir.y /= float(h);

    _pDevice->setState(_stateBlur);
    _pRenderTarget->bind(dst);
    _pDevice->setPixelShader(_pPShaderBlur);
    _pPShaderBlur->getConstantTable()->setSampler2D(_idSamplerBlur, _pRenderTarget->getShaderTextureView(src));
    _pPShaderBlur->getConstantTable()->setConstant(_idDirectionScaleBlur, pixDir);
    _pScreenQuad->send(w, h);
}
//-----------------------------------------------------------------------------
void PostFXRenderer::radialBlur(const Core::Vector2f & center, float scale, ERenderTargetBuffer src, ERenderTargetBuffer dst)
{
    int w = _pRenderTarget->getViewportWidth(dst);
    int h = _pRenderTarget->getViewportHeight(dst);

    scale *= 5.0f;

    Core::Vector2f pixDir(scale, scale);
    pixDir.x /= float(w);
    pixDir.y /= float(h);

    _pDevice->setState(_stateRadialBlur);
    _pRenderTarget->bind(dst);
    _pDevice->setPixelShader(_pPShaderRadialBlur);
    _pPShaderRadialBlur->getConstantTable()->setSampler2D(_idSamplerRadialBlur, _pRenderTarget->getShaderTextureView(src));
    _pPShaderRadialBlur->getConstantTable()->setConstant(_idCenterRadialBlur, center);
    _pPShaderRadialBlur->getConstantTable()->setConstant(_idScaleRadialBlur, pixDir);
    _pScreenQuad->send(w, h);
}
//-----------------------------------------------------------------------------
}
