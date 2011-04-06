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
#include <Renderer/SM2/HUDRenderer.h>
#include <Renderer/SM2/HUDSM2.h>
#include <Renderer/Common/Font.h>
#include <Renderer/Common/Rectangle.h>

#include <algorithm>


namespace Renderer
{
//-----------------------------------------------------------------------------
LM_ENUM_2(EElemType, ELEM_TEXT, ELEM_RECT);
//-----------------------------------------------------------------------------
struct HUDElement
{
    int32       priority;
    EElemType   type;
    union
    {
        Text *      pText;
        Rectangle * pRect;
    };

    bool operator < (const HUDElement & elem) const
    {
        return priority < elem.priority;
    };
};
//-----------------------------------------------------------------------------
HUDRenderer::HUDRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<ScreenQuad> & pScreenQuad, const Ptr<TextureMap> & pDefaultTexture) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pScreenQuad(pScreenQuad),
    _pDefaultTexture(pDefaultTexture)
{
}
//-----------------------------------------------------------------------------
bool HUDRenderer::initialise()
{
    bool result = true;

    try
    {
        initialise(0, _params);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing HUDRenderer : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void HUDRenderer::initialise(int32 flags, ShaderParams & params)
{
    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(false, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blend(Gfx::BM_ONE, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

    sampler.isSRGB = false;
    blend.sRGBWriteEnabled = false;

    params.state = _pDevice->createState(raster, depth, blend);
    params.state.ptSampler[0] = _pDevice->createState(sampler);
}
//-----------------------------------------------------------------------------
void HUDRenderer::bind(const ShaderParams & params)
{
    _pDevice->setState(params.state);
    _pDevice->setPixelShader(null);
}
//-----------------------------------------------------------------------------
void HUDRenderer::render(const Ptr<IHUD> & pHUD)
{
    Ptr<HUDSM2> pHUD2 = LM_DEBUG_PTR_CAST<HUDSM2>(pHUD);
    
    Core::List<Text *> texts;
    Core::List<Rectangle *> rects;

    pHUD2->cleanUp();
    pHUD2->getTexts(texts);
    pHUD2->getRects(rects);

    Core::List<HUDElement> elems;
    HUDElement elem;

    for(int32 ii=0; ii < texts.size(); ii++)
    {
        const Text & text = *texts[ii];

        if(text.isVisible() && text.getColor().a > 0.0f)
        {
            elem.type = ELEM_TEXT;
            elem.priority = texts[ii]->getPriority();
            elem.pText = texts[ii];

            elems.push_back(elem);
        }
    }

    for(int32 ii=0; ii < rects.size(); ii++)
    {
        const Rectangle & rect = *rects[ii];

        if(rect.isVisible() && rect.getColor().a > 0.0f)
        {
            elem.type = ELEM_RECT;
            elem.priority = rect.getPriority();
            elem.pRect = rects[ii];

            elems.push_back(elem);
        }
    }

    std::sort(elems.begin(), elems.end());

    _pDevice->beginDraw();

    for(int32 ii=0; ii < elems.size(); ii++)
    {
        switch(elems[ii].type)
        {
        case ELEM_TEXT:
        {
            const Text & text = *elems[ii].pText;
            Ptr<IFont> pFont = text.getFont();
            Ptr<Font> pFontSM2 = LM_DEBUG_PTR_CAST<Font>(pFont);
            Ptr<Gfx::IFont> pInternalFont = pFontSM2->getInternalFont(text.isBold(), text.isItalic());

            Core::Vector2f position(text.getPosition());
            Core::Vector2f clipRect(text.getClipRect());
            
            if(clipRect != Core::Vector2f::ZERO)
                pInternalFont->drawClippedText(text.getText(), int32(position.x), int32(position.y), int32(clipRect.x), int32(clipRect.y), text.getColor());
            else
                pInternalFont->drawText(text.getText(), int32(position.x), int32(position.y), text.getColor());
            break;
        }
        case ELEM_RECT:
        {
            bind(_params);

            const Rectangle & rect = *elems[ii].pRect;
            const Core::Vector2f & pos1 = rect.getPosition();
            const PictureInfos & pic = rect.getPicture();
            Core::Vector2f pos2 = pos1 + rect.getSize();

            Gfx::RSFixedFunc ff;

            if(pic.pTexture != null)
                ff.pTex0 = LM_DEBUG_PTR_CAST<TextureMap>(pic.pTexture)->getResource();
            else
                ff.pTex0 = _pDefaultTexture->getResource();

            ff.fogEnabled = false;
            _pDevice->setFixedFuncPipe(ff);

            _pScreenQuad->send(int32(pos1.x), int32(pos1.y), int32(pos2.x), int32(pos2.y),
                pic.u1, pic.v1, pic.u2, pic.v2, rect.getColor());
            break;
        }
        }
    }

    _pDevice->endDraw();
}
//-----------------------------------------------------------------------------
}