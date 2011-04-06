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
#include <GUI/BitmapFont.h>

namespace GUI
{
//-------------------------------------------------------------------------
void BitmapFontInfos::importXML(const Ptr<Core::XMLNode> & pNode)
{
    bool ok = true;
    ok &= pNode->getValue(L"texture-name", textureName);

    ok &= pNode->getValue(L"char-width", charWidth);
    ok &= pNode->getValue(L"char-height", charHeight);
    ok &= pNode->getValue(L"char-spacing", charSpacing);

    ok &= pNode->getValue(L"u-res", uRes);
    ok &= pNode->getValue(L"v-res", vRes);

    ok &= pNode->getValue(L"characters", characters);
}
//-------------------------------------------------------------------------
void BitmapFontInfos::exportXML(const Ptr<Core::XMLNode> & pNode) const
{
    pNode->setValue(L"texture-name", textureName);

    pNode->setValue(L"char-width", charWidth);
    pNode->setValue(L"char-height", charHeight);
    pNode->setValue(L"char-spacing", charSpacing);

    pNode->setValue(L"u-res", uRes);
    pNode->setValue(L"v-res", vRes);

    pNode->setValue(L"characters", characters);
}
//-------------------------------------------------------------------------
BitmapFont::BitmapFont(const Ptr<Renderer::ITextureMap> & pTexture,
                       const BitmapFontInfos & infos) :
    _infos(infos)
{
    for(int32 ii=0; ii < infos.characters.size(); ii++)
    {
        int32 iu = ii % infos.uRes;
        int32 iv = ii / infos.uRes;

        Renderer::PictureInfos & pic = _pics[infos.characters[ii]];
        pic.pTexture    = pTexture;
        pic.width       = int32(infos.charWidth);
        pic.height      = int32(infos.charHeight);
        pic.u1          = float(iu + 0) / float(infos.uRes);
        pic.v1          = float(iv + 0) / float(infos.vRes);
        pic.u2          = float(iu + 1) / float(infos.uRes);
        pic.v2          = float(iv + 1) / float(infos.vRes);
    }
}
//-------------------------------------------------------------------------
const Renderer::PictureInfos * BitmapFont::get(wchar_t c) const
{
    Core::Map<wchar_t, Renderer::PictureInfos>::const_iterator iPic = _pics.find(c);

    if(iPic == _pics.end())
        return NULL;
    else
        return &iPic->second;
}
//-------------------------------------------------------------------------
const Renderer::PictureInfos * BitmapFont::operator [] (wchar_t c) const
{
    return get(c);
}
//-------------------------------------------------------------------------
}

