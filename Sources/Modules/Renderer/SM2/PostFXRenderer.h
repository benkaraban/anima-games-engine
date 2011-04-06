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
#ifndef RENDERER_POSTFXRENDERER_H_
#define RENDERER_POSTFXRENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/ScreenQuad.h>

namespace Renderer
{
class IPostFXRenderer
{
public:
    virtual ~IPostFXRenderer() {};
    virtual bool initialise() = 0;
    virtual void postFX(const PostFX & fx, const RenderContext & context) = 0;
};

class PostFXRenderer : public IPostFXRenderer, public Core::Object
{
public:
    PostFXRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad,
                   const RendererSettings & settings);
    virtual bool initialise();
    virtual void postFX(const PostFX & fx, const RenderContext & context);

protected:
    void blur(const Core::Vector2f & dir, ERenderTargetBuffer src, ERenderTargetBuffer dst);
    void radialBlur(const Core::Vector2f & center, float scale, ERenderTargetBuffer src, ERenderTargetBuffer dst);

    Ptr<Gfx::IDevice>          _pDevice;
    Ptr<ShaderLib>             _pShaderLib;
    Ptr<ScreenQuad>            _pScreenQuad;
    RendererSettings           _settings;

    Gfx::GlobalRenderState     _stateBlur;
    Ptr<Gfx::IPixelShader>     _pPShaderBlur;
    int32                      _idDirectionScaleBlur;
    int32                      _idSamplerBlur;

    Gfx::GlobalRenderState     _stateRadialBlur;
    Ptr<Gfx::IPixelShader>     _pPShaderRadialBlur;
    int32                      _idCenterRadialBlur;
    int32                      _idScaleRadialBlur;
    int32                      _idSamplerRadialBlur;

    Gfx::GlobalRenderState     _stateScale;
    Ptr<Gfx::IPixelShader>     _pPShaderScale;
    int32                      _idSamplerScale;

    Gfx::GlobalRenderState     _stateAccum;
    Ptr<Gfx::IPixelShader>     _pPShaderAccum;
    int32                      _idSourceFactorAccum;
    int32                      _idSamplerAccum;

    Gfx::GlobalRenderState     _statePostFX;
    Ptr<Gfx::IPixelShader>     _pPShaderPostFX;
    int32                      _idBlurFactorPostFX;
    int32                      _idSaturationPostFX;
    int32                      _idFadePostFX;
    int32                      _idSamplerTexturePostFX;
    int32                      _idSamplerBlurPostFX;

    IRenderTarget *            _pRenderTarget;
};

}

#endif /* GLOWRENDERER_H_ */
