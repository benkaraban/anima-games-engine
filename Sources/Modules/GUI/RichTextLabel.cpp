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
#include <GUI/RichTextLabel.h>
#include <GUI/Picture.h>

namespace GUI
{
//-------------------------------------------------------------------------
//*************************************************************************
//-------------------------------------------------------------------------
RichTextLabel::RichTextLabel(const Ptr<ResourcePool> & pResPool,
                             const Ptr<Renderer::IHUD> & pHUD) :
    _pResPool(pResPool),
    _pHUD(pHUD),
    _fontName(L"Arial"),
    _fontSize(12),
    _hLayout(HL_LEFT),
    _vLayout(VL_UP),
    _shadow(true),
    _autoCenterVContent(false)
{
}
//-------------------------------------------------------------------------
void RichTextLabel::setText(const String & text)
{
    if(_pDico != null)
        _text = (*_pDico)[text];
    else
        _text = text;

    updateText();
}
//-------------------------------------------------------------------------
void RichTextLabel::updateText()
{
    GUI::parseRichText(_tokens, _text);
    generateLines();
    computeMaxFirstLine();
    setFirstLine(0);

    if(_autoCenterVContent)
    {
        float remainingSize = getSize().y - _yLine;

        if(remainingSize > 0.0f)
        {
            float dy = 0.5f * remainingSize;

            for(int32 ii=0; ii < _lines.size(); ii++)
            {
                Core::Vector2f position(_lines[ii]->getPosition());
                position.y += dy;
                _lines[ii]->setPosition(position);
            }
        }
    }
}
//-------------------------------------------------------------------------
void RichTextLabel::computeMaxFirstLine()
{
    if(_lines.empty())
    {
        _maxFirstLine = 0;
        return;
    }

    float size = 0.0f;
    int32 ii = _lines.size() - 1;

    while(ii >= 0 && size < _size.y)
    {
        size += _lines[ii]->getSize().y;
        ii --;
    }

    _maxFirstLine = ii;
}
//-------------------------------------------------------------------------
void RichTextLabel::generateImage(const StyleState & state, const String & name)
{
    Ptr<Picture> pPic(new Picture(_pResPool, _pHUD));
    pPic->setPicture(name);

    Core::Vector2f lineSize(_pCurLine->getSize());
    Core::Vector2f picSize(pPic->getSize());

    if((lineSize.x + picSize.x) > _size.x)
    {
        // Does not it in line
        nextLine(state);
        lineSize = _pCurLine->getSize();
    }

    pPic->setPosition(Core::Vector2f(lineSize.x, 0.0f));

    lineSize.x += picSize.x;
    lineSize.y = std::max(lineSize.y, picSize.y);

    _pCurLine->setSize(lineSize);
    _pCurLine->addChild(pPic);
}
//-------------------------------------------------------------------------
void RichTextLabel::generateText(const StyleState & state, const String & text)
{
    Ptr<Renderer::IFont> pFont = _pResPool->getFont(state.style.fontName, state.style.fontSize);

    int32 iStart = 0;
    int32 iPrevEnd = -1;
    int32 iEnd = text.find_first_of(L" \t\n\r");
    
    while(iEnd != String::npos)
    {
        if(!fitInLine(state, text.slice(iStart, iEnd), pFont))
        {
            if(iPrevEnd >= iStart)
                addToLine(state, text.slice(iStart, iPrevEnd), pFont);
            nextLine(state);
            iStart = iPrevEnd + 1;
            iPrevEnd = iEnd;
        }
        else
        {
            iPrevEnd = iEnd;
            iEnd = text.find_first_of(L" \t\n\r", iEnd + 1);
        }
    }

    String trail(text.slice(iStart));

    if(!fitInLine(state, trail, pFont))
    {
        if(iPrevEnd >= iStart)
            addToLine(state, text.slice(iStart, iPrevEnd), pFont);
        nextLine(state);
        iStart = iPrevEnd + 1;
        trail = text.slice(iStart);
    }

    addToLine(state, trail, pFont);
}
//-------------------------------------------------------------------------
void RichTextLabel::nextLine(const StyleState & state)
{
    if(_pCurLine != null)
    {
        Core::Vector2f lineSize = _pCurLine->getSize();
        Core::Vector2f linePos(0.0f, _yLine);
        
        if(lineSize.y == 0.0f)
        {
            lineSize.y = _lastLineY;
            _pCurLine->setSize(lineSize);
        }

        switch(state.hLayout)
        {
        case HL_LEFT:   linePos.x = 0.0; break;
        case HL_RIGHT:  linePos.x = _size.x - lineSize.x; break;
        case HL_CENTER: linePos.x = 0.5f * (_size.x - lineSize.x); break;
        };


        const Core::List<Ptr<Widget> > & children = _pCurLine->getChildren();

        for(int32 ii=0; ii < children.size(); ii++)
        {
            Core::Vector2f childSize = children[ii]->getSize();
            Core::Vector2f childPos = children[ii]->getPosition();


            switch(state.vLayout)
            {
            case VL_UP:     childPos.y = 0.0; break;
            case VL_DOWN:   childPos.y = (lineSize.y - childSize.y); break;
            case VL_CENTER: childPos.y = 0.5f * (lineSize.y - childSize.y); break;
            };

            children[ii]->setPosition(childPos);
        }

        _pCurLine->setPosition(linePos);

        _lastLineY = lineSize.y;
        _yLine += lineSize.y;
        _baseY.push_back(linePos.y);
        _lines.push_back(_pCurLine);
        addChild(_pCurLine);
    }

    {
        Core::Vector2f linePos(0.0f, _yLine);
        _pCurLine = Ptr<Widget>(new Widget());
        _pCurLine->setSize(Core::Vector2f(0.0f, 0.0f));
        _pCurLine->setPosition(linePos);
    }
}
//-------------------------------------------------------------------------
bool RichTextLabel::fitInLine(const StyleState & state, const String & text, const Ptr<Renderer::IFont> & pFont)
{
    Core::Vector2f lineSize(_pCurLine->getSize());
    Core::Vector2f textSize;

    pFont->getTextSize(textSize, text, state.style.bold, state.style.italic);

    return (lineSize.x + textSize.x) < _size.x;
}
//-------------------------------------------------------------------------
void RichTextLabel::addToLine(const StyleState & state, const String & text, const Ptr<Renderer::IFont> & pFont)
{
    LM_ASSERT(!text.empty());

    Core::Vector2f lineSize = _pCurLine->getSize();
    Core::Vector2f textSize;

    pFont->getTextSize(textSize, text, state.style.bold, state.style.italic);

    Ptr<Label> pLabel(new Label(_pResPool, _pHUD));
    pLabel->setStyle(state.style);
    pLabel->setPosition(Core::Vector2f(lineSize.x, 0.0f));
    pLabel->setSize(textSize);
    pLabel->setText(text);

    lineSize.x += textSize.x;
    lineSize.y = std::max(lineSize.y, textSize.y);

    _pCurLine->setSize(lineSize);
    _pCurLine->addChild(pLabel);
}
//-------------------------------------------------------------------------
void RichTextLabel::generateLines()
{
    StyleState style;
    style.hLayout = _hLayout;
    style.vLayout = _vLayout;
    style.style.fontName = _fontName;
    style.style.fontSize = _fontSize;
    style.style.shadow = _shadow;
    style.style.hLayout = _hLayout;
    style.style.vLayout = _vLayout;
    Core::List<StyleState> styleStack;
    styleStack.push_back(style);

    clearLines();
    nextLine(style);    

    for(int32 iToken=0; iToken < _tokens.size(); iToken++)
    {
        const RichTextToken & token = _tokens[iToken];

        if(token.type == RT_TEXT)
        {
            generateText(styleStack.back(), token.text);
        }
        else if(token.type == RT_TAG_STOP)
        {
            if(styleStack.back().tag != token.tag)
                throw Core::Exception(L"Tag mismatch " + styleStack.back().tag + L"/" + token.tag);
            else if(styleStack.size() <= 1)
                throw Core::Exception(L"Superflous /" + token.tag);
            else
            {
                if(styleStack.back().tagType == RT_TAG_ALIGN)
                    nextLine(styleStack.back());

                styleStack.pop_back();
            }
        }
        else if(token.type == RT_TAG_N)
        {
            nextLine(styleStack.back());
        }
        else if(token.type == RT_TAG_IMG)
        {
            generateImage(styleStack.back(), token.text);
        }
        else
        {
            pushStyle(styleStack, token);
        }
    }

    nextLine(styleStack.back());
}
//-------------------------------------------------------------------------
void RichTextLabel::setFontName(const String & font)
{
    _fontName = font;
}
//-------------------------------------------------------------------------
void RichTextLabel::setFontSize(int32 size)
{
    _fontSize = size;
}
//-------------------------------------------------------------------------
void RichTextLabel::pushStyle(Core::List<StyleState> & stack, const RichTextToken & token)
{
    StyleState ss(stack.back());
    ss.tag = token.tag;
    ss.tagType = token.type;

    switch(token.type)
    {
    case RT_TAG_B:          ss.style.bold = true; break;
    case RT_TAG_I:          ss.style.italic = true; break;
    case RT_TAG_COLOR:      ss.style.color = token.color; break;
    case RT_TAG_FONT:       ss.style.fontName = token.text; break;
    case RT_TAG_SIZE:       ss.style.fontSize = token.value; break;
    case RT_TAG_ALIGN:      ss.hLayout = token.hLayout; break;
    case RT_TAG_VALIGN:     ss.vLayout = token.vLayout; break;
    default:
        LM_ASSERT(false);
        break;
    }

    stack.push_back(ss);
}
//-------------------------------------------------------------------------
void RichTextLabel::setAutoCenterVContent(bool flag)
{ 
    _autoCenterVContent = flag;
    updateText();
}
//-------------------------------------------------------------------------
void RichTextLabel::setFirstLine(int32 line)
{
    if(_lines.empty())
        return;

    _firstLine = std::max(0, std::min(line, getMaxFirstLine()));
    float dy = _baseY[_firstLine];

    for(int32 ii=0; ii < _lines.size(); ii++)
    {
        Core::Vector2f size(_lines[ii]->getSize());
        Core::Vector2f position(_lines[ii]->getPosition());
        
        position.y = _baseY[ii] - dy;
        _lines[ii]->setPosition(position);

        if(position.y < 0.0f)
            _lines[ii]->setVisible(false);
        else if((position.y + size.y) > _size.y)
            _lines[ii]->setVisible(false);
        else
            _lines[ii]->setVisible(true);
    }
}
//-------------------------------------------------------------------------
int32 RichTextLabel::getFirstLine() const
{
    return _firstLine;
}
//-------------------------------------------------------------------------
int32 RichTextLabel::getMaxFirstLine() const
{
    return _maxFirstLine;
}
//-------------------------------------------------------------------------
void RichTextLabel::clearLines()
{
    for(int32 ii=0; ii < _lines.size(); ii++)
    {
        removeChild(_lines[ii]);
        _lines[ii]->kill();
    }

    _pCurLine = null;
    _lines.clear();
    _baseY.clear();
    _yLine = 0.0f;
    _lastLineY = 10.0f;
}
//-------------------------------------------------------------------------
int32 RichTextLabel::getLineCount() const
{
    return _lines.size();
}
//-------------------------------------------------------------------------
int32 RichTextLabel::getVisibleLineCount() const
{
    return getLineCount() - getMaxFirstLine();
}
//-------------------------------------------------------------------------
}

