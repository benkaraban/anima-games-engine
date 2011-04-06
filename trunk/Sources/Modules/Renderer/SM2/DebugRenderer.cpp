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
#include <Renderer/SM2/DebugRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
DebugRenderer::DebugRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pScreenQuad(pScreenQuad)
{
}
//-----------------------------------------------------------------------------
bool DebugRenderer::initialise()
{
    bool result = true;

    try
    {
        // Pixel shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pPShader = _pShaderLib->getPShader(L"passthru-tex-z.psh", Gfx::PS_V2_0, L"ps_main", macros);
        _idSampler = _pPShader->getConstantTable()->getConstantIndex(L"SamplerTexture");

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;
        Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_POINT, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

        sampler.isSRGB = false;
        blend.sRGBWriteEnabled = false;

        _state = _pDevice->createState(raster, depth, blend);

        Ptr<Gfx::IRSSampler> pSamplerState(_pDevice->createState(sampler));
        _state.ptSampler[_pPShader->getConstantTable()->getSamplerId(_idSampler)] = pSamplerState;
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing DebugRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void DebugRenderer::renderTexture(const RenderContext & context, const Ptr<Gfx::ITexture2D> & pTexture, int32 x, int32 y, int32 w, int32 h)
{
    _pDevice->setState(_state);
    _pDevice->setPixelShader(_pPShader);
    _pPShader->getConstantTable()->setSampler2D(_idSampler, pTexture->getShaderResourceView());
    _pScreenQuad->send(x, y, x + w - 1, y + h - 1);
}
//-----------------------------------------------------------------------------
}
