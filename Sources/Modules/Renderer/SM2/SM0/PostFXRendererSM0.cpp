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
#include <Renderer/SM2/SM0/PostFXRendererSM0.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
PostFXRendererSM0::PostFXRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ScreenQuad> & pScreenQuad) :
    _pDevice(pDevice),
    _pScreenQuad(pScreenQuad)
{
}
//-----------------------------------------------------------------------------
bool PostFXRendererSM0::initialise()
{
    bool result = true;

    try
    {
       // States

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);

        blend.sRGBWriteEnabled = true;

        _stateFade = _pDevice->createState(raster, depth, blend);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing PostFXRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void PostFXRendererSM0::postFX(const PostFX & fx, const RenderContext & context)
{
    if(fx.fadeColor.a > 0.0f)
    {
        _pDevice->setState(_stateFade);
        _pDevice->setPixelShader(null);

        Gfx::RSFixedFunc ff;
        ff.fogEnabled = false;
        _pDevice->setFixedFuncPipe(ff);

        _pScreenQuad->send(context.getViewportWidth(), context.getViewportHeight(), fx.fadeColor);
    }
}
//-----------------------------------------------------------------------------
}
