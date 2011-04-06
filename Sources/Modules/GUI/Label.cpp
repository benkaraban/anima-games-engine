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
#include <GUI/Label.h>

namespace GUI
{
//-------------------------------------------------------------------------
LabelStyle::LabelStyle() :
    fontName(L"Arial"),
    fontSize(12),
    color(1.0f, 1.0f, 1.0f, 1.0f),
    shadowColor(0.0f, 0.0f, 0.0f, 1.0f),
    italic(false),
    bold(false),
    shadow(false),
    hLayout(HL_LEFT),
    vLayout(VL_UP)
{
}
//-------------------------------------------------------------------------
//*************************************************************************
//-------------------------------------------------------------------------
Label::Label(const Ptr<ResourcePool> & pResPool,
             const Ptr<Renderer::IHUD> & pHUD) :
    _pResPool(pResPool),
    _text(),
    _fontName(L"Calibri"),
    _fontSize(12),
    _hLayout(HL_LEFT),
    _vLayout(VL_UP),
    _textColor(1.0f, 1.0f, 1.0f, 1.0f),
    _shadowColor(0.0f, 0.0f, 0.0f, 1.0f),
    _bold(false),
    _italic(false),
    _shadow(true)
{
    _pText1 = pHUD->createText(_pResPool->getFont(_fontName, _fontSize), L"");
    _pText2 = pHUD->createText(_pResPool->getFont(_fontName, _fontSize), L"");

    _pText1->setVisible(false);
    _pText2->setVisible(false);

    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setText(const String & text)
{
    if(_pDico != null)
        _text = (*_pDico)[text];
    else
        _text = text;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setFontName(const String & font)
{
    _fontName = font;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setFontSize(int32 size)
{
    _fontSize = size;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setTextColor(const Core::Vector4f & color)
{
    _textColor = color;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setShadowColor(const Core::Vector4f & color)
{
    _shadowColor = color;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setBold(bool bold)
{
    _bold = bold;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setItalic(bool italic)
{
    _italic = italic;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setShadow(bool shadow)
{
    _shadow = shadow;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setHLayout(EHoriLayout layout)
{
    _hLayout = layout;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setVLayout(EVertLayout layout)
{
    _vLayout = layout;
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::setStyle(const LabelStyle & style)
{
    _fontName   = style.fontName;
    _fontSize   = style.fontSize;
    _textColor  = style.color;
    _shadowColor= style.shadowColor;
    _bold       = style.bold;
    _italic     = style.italic;
    _shadow     = style.shadow;
    _hLayout    = style.hLayout;
    _vLayout    = style.vLayout;
    
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::getStyle(LabelStyle & style)
{
    style.fontName   = _fontName;
    style.fontSize   = _fontSize;
    style.color      = _textColor;
    style.shadowColor= _shadowColor;
    style.bold       = _bold;
    style.italic     = _italic;
    style.shadow     = _shadow;
    style.hLayout    = _hLayout;
    style.vLayout    = _vLayout;
}
//-------------------------------------------------------------------------
void Label::fitText(bool fitX, bool fitY)
{
   Core::Vector2f textSize;
    _pText1->getTextSize(textSize);

    if(!fitX)
        textSize.x = _size.x;

    if(!fitY)
        textSize.y = _size.y;

    setSize(textSize);
}
//-------------------------------------------------------------------------
void Label::updateDisplay()
{
    _pText1->setText(_text);
    _pText2->setText(_text);

    _pText1->setFont(_pResPool->getFont(_fontName, _fontSize));
    _pText2->setFont(_pResPool->getFont(_fontName, _fontSize));

    _pText1->setColor(Core::mul(getColor(), _textColor));
    _pText2->setColor(Core::mul(getColor(), _shadowColor));

    _pText1->setBold(_bold);
    _pText2->setBold(_bold);

    _pText1->setItalic(_italic);
    _pText2->setItalic(_italic);

    _pText1->setVisible(isVisible());
    _pText2->setVisible(isVisible() && _shadow);

    _pText1->setClipRect(getClipRect());
    _pText2->setClipRect(getClipRect());

    Core::Vector2f text1Pos;
    Core::Vector2f textSize;
    _pText1->getTextSize(textSize);

    switch(_hLayout)
    {
    case HL_LEFT:
        text1Pos.x = _screenPos.x;
        break;
    case HL_RIGHT:
        text1Pos.x = _screenPos.x + getSize().x - textSize.x;
        break;
    case HL_CENTER:
        text1Pos.x = _screenPos.x + 0.5f * (getSize().x - textSize.x);
        break;
    }

    switch(_vLayout)
    {
    case VL_UP:
        text1Pos.y = _screenPos.y;
        break;
    case VL_DOWN:
        text1Pos.y = _screenPos.y + getSize().y - textSize.y;
        break;
    case VL_CENTER:
        text1Pos.y = _screenPos.y + 0.5f * (getSize().y - textSize.y);
        break;
    }

    _pText1->setPosition(text1Pos);
    _pText2->setPosition(text1Pos + Core::Vector2f(1.0f, 1.0f));
}
//-------------------------------------------------------------------------
void Label::onVisibilityChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void Label::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void Label::onPositionChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::onPriorityChanged(int32 p)
{
    _pText1->setPriority(p + 1);
    _pText2->setPriority(p + 0);
}
//-------------------------------------------------------------------------
void Label::onSizeChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::onColorChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void Label::onKill()
{
    _pText1->kill();
    _pText2->kill();
}
//-------------------------------------------------------------------------
}

