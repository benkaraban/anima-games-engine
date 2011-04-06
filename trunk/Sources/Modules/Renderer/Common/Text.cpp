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
#include <Renderer/Common/Text.h>
#include <Renderer/Common/Font.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
Text::Text(const Ptr<IFont> & pFont, const String & text, int32 priority) :
    _pFont(pFont),
    _text(text),
    _color(Core::Vector4f::ONE),
    _position(0.0f, 0.0f),
    _clipRect(0.0f, 0.0f),
    _priority(priority),
    _isBold(false),
    _isItalic(false),
    _isVisible(true),
    _isAlive(true)
{
}
//-----------------------------------------------------------------------------
void Text::setPosition(const Core::Vector2f & position)
{
    _position = position;
}
//-----------------------------------------------------------------------------
const Core::Vector2f & Text::getPosition() const
{
    return _position;
}
//-----------------------------------------------------------------------------
void Text::setClipRect(const Core::Vector2f & clip)
{
    _clipRect = clip;
}
//-----------------------------------------------------------------------------
const Core::Vector2f & Text::getClipRect() const
{
    return _clipRect;
}
//-----------------------------------------------------------------------------
void Text::setPriority(int32 p)
{
    _priority = p;
}
//-----------------------------------------------------------------------------
int32 Text::getPriority() const
{
    return _priority;
}
//-----------------------------------------------------------------------------
void Text::setText(const String & text)
{
    _text = text;
}
//-----------------------------------------------------------------------------
const String & Text::getText() const
{
    return _text;
}
//-----------------------------------------------------------------------------
void Text::setColor(const Core::Vector4f & color)
{
    _color = color;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & Text::getColor() const
{
    return _color;
}
//-----------------------------------------------------------------------------
void Text::setFont(const Ptr<IFont> & pFont)
{
    _pFont = pFont;
}
//-----------------------------------------------------------------------------
Ptr<IFont> Text::getFont() const
{
    return _pFont;
}
//-----------------------------------------------------------------------------
void Text::setBold(bool flag)
{
    _isBold = flag;
}
//-----------------------------------------------------------------------------
bool Text::isBold() const
{
    return _isBold;
}
//-----------------------------------------------------------------------------
void Text::setItalic(bool flag)
{
    _isItalic = flag;
}
//-----------------------------------------------------------------------------
bool Text::isItalic() const
{
    return _isItalic;
}
//-----------------------------------------------------------------------------
void Text::setVisible(bool flag)
{
    _isVisible = flag;
}
//-----------------------------------------------------------------------------
bool Text::isVisible() const
{
    return _isVisible;
}
//-----------------------------------------------------------------------------
void Text::kill()
{
    _isAlive = false;
}
//-----------------------------------------------------------------------------
bool Text::isAlive() const
{
    return _isAlive;
}
//-----------------------------------------------------------------------------
void Text::getTextSize(Core::Vector2f & size)
{
    Ptr<Gfx::IFont> pFont = LM_DEBUG_PTR_CAST<Font>(_pFont)->getInternalFont(_isBold, _isItalic);
    int32 w = 1;
    int32 h = 1;
    if(pFont != null)
        pFont->getTextSize(_text, w, h);
    size.x = float(w);
    size.y = float(h);
}
//-----------------------------------------------------------------------------
}
