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
#include <GUI/BitmapLabel.h>

namespace GUI
{
//-------------------------------------------------------------------------
BitmapLabel::BitmapLabel(const Ptr<ResourcePool> & pResPool,
             const Ptr<Renderer::IHUD> & pHUD) :
    _pResPool(pResPool),
    _pHUD(pHUD),
    _hLayout(HL_LEFT),
    _vLayout(VL_UP),
    _zoomFactor(1.0f),
    _textSize(0.0f, 0.0f)
{
}
//-------------------------------------------------------------------------
void BitmapLabel::setText(const String & text)
{
    _text = text;
    updateText();
}
//-------------------------------------------------------------------------
void BitmapLabel::setFont(const String & font)
{
    _fontName = font;
    _pFont = _pResPool->getBitmapFont(font);
    updateText();
}
//-------------------------------------------------------------------------
void BitmapLabel::setZoomFactor(float zoom)
{
    _zoomFactor = zoom;
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::updateText()
{
    if(_pFont == null)
        return;

    int32 newSize = _text.size();
    int32 oldSize = _chars.size();

    for(int32 ii=newSize; ii < oldSize; ii++)
        _chars[ii]->kill();

    _chars.resize(newSize);

    for(int32 ii=0; ii < std::min(newSize, oldSize); ii++)
        _chars[ii]->setPicture(*_pFont->get(_text[ii]));

    for(int32 ii=oldSize; ii < newSize; ii++)
        _chars[ii] = _pHUD->createRectangle(*_pFont->get(_text[ii]));

    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::updateDisplay()
{
    if(_pFont == null)
        return;

    const BitmapFontInfos & finfos = _pFont->getInfos();
    const Core::Vector4f & color = getColor();

    Core::Vector2f position;
    Core::Vector2f charSize(finfos.charWidth, finfos.charHeight);
    Core::Vector2f textSize(float(std::max(_text.size() - 1, 0)) * finfos.charSpacing + finfos.charWidth, finfos.charHeight);

    charSize = _zoomFactor * charSize;
    textSize = _zoomFactor * textSize;

    switch(_hLayout)
    {
    case HL_LEFT:
        position.x = _screenPos.x;
        break;
    case HL_RIGHT:
        position.x = _screenPos.x + getSize().x - textSize.x;
        break;
    case HL_CENTER:
        position.x = _screenPos.x + 0.5f * (getSize().x - textSize.x);
        break;
    }

    switch(_vLayout)
    {
    case VL_UP:
        position.y = _screenPos.y;
        break;
    case VL_DOWN:
        position.y = _screenPos.y + getSize().y - textSize.y;
        break;
    case VL_CENTER:
        position.y = _screenPos.y + 0.5f * (getSize().y - textSize.y);
        break;
    }

    for(int32 ii=0; ii < _chars.size(); ii++)
    {
        _chars[ii]->setPriority(getPriority());
        _chars[ii]->setPosition(position);
        _chars[ii]->setSize(charSize);
        _chars[ii]->setColor(color);
        _chars[ii]->setVisible(isVisible());

        position.x += finfos.charSpacing;
    }

    _textSize = textSize;
}
//-------------------------------------------------------------------------
void BitmapLabel::onVisibilityChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void BitmapLabel::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void BitmapLabel::onPositionChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::onPriorityChanged(int32 p)
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::setHLayout(EHoriLayout layout)
{
    _hLayout = layout;
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::setVLayout(EVertLayout layout)
{
    _vLayout = layout;
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::onSizeChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::onColorChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void BitmapLabel::onKill()
{
    for(int32 ii=0; ii < _chars.size(); ii++)
        _chars[ii]->kill();

    _chars.clear();
}
//-------------------------------------------------------------------------
}

