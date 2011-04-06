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
#ifndef RENDERER_DEBUGRENDERER_H_
#define RENDERER_DEBUGRENDERER_H_

#include <Gfx/IDevice.h>
#include <Renderer/SM2/RenderContext.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/ScreenQuad.h>

namespace Renderer
{
class IDebugRenderer
{
public:
    virtual ~IDebugRenderer() {};
    virtual bool initialise() = 0;

    virtual void renderTexture(const RenderContext & context, const Ptr<Gfx::ITexture2D> & pTexture, int32 x, int32 y, int32 w, int32 h) = 0;
};

class DebugRendererDummy : public IDebugRenderer, public Core::Object
{
public:
    virtual bool initialise() { return true; };

    virtual void renderTexture(const RenderContext & context, const Ptr<Gfx::ITexture2D> & pTexture, int32 x, int32 y, int32 w, int32 h)
    {};
};

class DebugRenderer : public IDebugRenderer, public Core::Object
{
public:
    DebugRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad);
    virtual bool initialise();

    virtual void renderTexture(const RenderContext & context, const Ptr<Gfx::ITexture2D> & pTexture, int32 x, int32 y, int32 w, int32 h);

protected:
    Ptr<Gfx::IDevice>            _pDevice;
    Ptr<ShaderLib>               _pShaderLib;
    Ptr<ScreenQuad>              _pScreenQuad;

    Gfx::GlobalRenderState       _state;

    Ptr<Gfx::IPixelShader>       _pPShader;
    int32                        _idSampler;
};

}

#endif /* GLOWRENDERER_H_ */
