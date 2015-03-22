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
#ifndef GUI_BITMAP_FONT_H_
#define GUI_BITMAP_FONT_H_

#include <Core/Map.h>
#include <Core/XML/XMLNode.h>

#include <Renderer/ITexture.h>
#include <Renderer/IPictureBank.h>

namespace GUI
{
struct LM_API_GUI BitmapFontInfos : public Core::Object
{
    String      textureName;
    
    float       charWidth;
    float       charHeight;
    float       charSpacing;

    int32       uRes;
    int32       vRes;

    String      characters;

    void importXML(const Ptr<Core::XMLNode> & pNode);
    void exportXML(const Ptr<Core::XMLNode> & pNode) const;
};

class LM_API_GUI BitmapFont : public Core::Object
{
public:
    BitmapFont(const Ptr<Renderer::ITextureMap> & pTexture,
               const BitmapFontInfos & infos);


    const BitmapFontInfos & getInfos() const { return _infos; }

    const Renderer::PictureInfos * get(wchar_t c) const;
    const Renderer::PictureInfos * operator [] (wchar_t c) const;


protected:
    BitmapFontInfos                  _infos;
    Core::Map<wchar_t, Renderer::PictureInfos> _pics;
};
}

#endif
