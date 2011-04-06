/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include <GUI/TextEdit.h>

namespace GUI
{

//-------------------------------------------------------------------------
TextEdit::TextEdit(const Ptr<ResourcePool> & pResPool,
                   const Ptr<Renderer::IHUD> & pHUD,
                   const Ptr<Window::InputManager> & pInput,
                   Focus * pFocus) :
    Label(pResPool, pHUD),
    _pInput(pInput),
    _state(TEIS_DEFAULT),
    _maxLength(100),
    _isPassword(false),
    _cursorTime(0.0f),
    _pFocus(pFocus),
    _iVisibleStart(0),
    _iVisibleEnd(0),
    _selectionActive(false),
    _selectionByWord(false)
{
    _pCursor = pHUD->createText(_pResPool->getFont(L"Arial", 12), L"|");
    _pSelection = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pSelection->setColor(Core::Vector4f(0.5f, 0.0f, 0.0f, 0.5f));

    _pCursor->setVisible(false);
    _pSelection->setVisible(false);

    _returnTyped = false;
}
//-------------------------------------------------------------------------
void TextEdit::setPassword(bool pass)
{
    _isPassword = pass;
    clampVisibleText();
}
//-------------------------------------------------------------------------
void TextEdit::setMaxLength(int32 maxLength)
{
    _maxLength = maxLength;

    if(_maxLength < _editText.size())
        setText(_editText.slice(0, _maxLength - 1));
}
//-------------------------------------------------------------------------
void TextEdit::setText(const String & text)
{
    _editText = text;
    _iVisibleStart = 0;
    _iVisibleEnd = _editText.size() - 1;
    clampVisibleText();
}
//-------------------------------------------------------------------------
void TextEdit::loseFocus()
{
    _selectionActive = false;
    _pSelection->setVisible(false);
}
//-------------------------------------------------------------------------
void TextEdit::focus()
{
    _pFocus->setOwner(this);
}
//-------------------------------------------------------------------------
void TextEdit::gainFocus()
{
    _selectionByWord = false;
    _cursorTime = 0.0f;

    if(_pInput->isLMBDown())
    {
        _cursorPos = mouseIndex();
        _selectionCursor = _cursorPos;

        _iSelStart = _cursorPos;
        _iSelEnd = _cursorPos;
        _selectionActive = false;
    }
    else
    {
        _selectionCursor = 0;
        _cursorPos = _editText.size();

        _iSelStart = std::min(_cursorPos, _selectionCursor);
        _iSelEnd = std::max(_cursorPos, _selectionCursor);
        _selectionActive = (_iSelStart != _iSelEnd);
    }
}
//-------------------------------------------------------------------------
void TextEdit::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
    {
        _pSelection->setVisible(false);
        _pCursor->setVisible(false);
        return;
    }

    _preferForwardVisible = true;
    Core::Vector2f lm = _pInput->getMousePos() - _screenPos;
    bool mouseOver = (lm.x >= 0.0f) && (lm.y >= 0.0f) && (lm.x < _size.x) && (lm.y < _size.y);
    bool lmbDown = _pInput->isLMBDown();

    ETextEditInternalState state = _state;

    switch(_state)
    {
    case TEIS_DEFAULT:
    {
        if(mouseOver && !lmbDown)
            _state = TEIS_MOUSE_OVER;
        break;
    }
    case TEIS_MOUSE_OVER:
    {
        if(mouseOver)
        {
            if(_pInput->isLMBDoubleClicked())
            {
                _state = TEIS_MOUSE_OVER_DOWN;
                focus();
                _cursorPos = mouseIndex();
                _selectionCursor = _cursorPos;
                _selectionByWord = true;

                _iSelStart = prevWord(std::min(_cursorPos, _selectionCursor));
                _iSelEnd = nextWord(std::max(_cursorPos, _selectionCursor));

                _selectionActive = (_iSelStart != _iSelEnd);
            }
            else if(lmbDown)
            {
                _state = TEIS_MOUSE_OVER_DOWN;
                focus();
                _cursorPos = mouseIndex();
                _selectionCursor = _cursorPos;
                _selectionByWord = false;
            }
        }
        else
        {
            _state = TEIS_DEFAULT;
        }
        break;
    }
    case TEIS_MOUSE_OVER_DOWN:
    {
        if(lmbDown)
        {
            _cursorPos = mouseIndex();
            _iSelStart = std::min(_cursorPos, _selectionCursor);
            _iSelEnd = std::max(_cursorPos, _selectionCursor);

            if(_selectionByWord)
            {
                _iSelStart = prevWord(_iSelStart);
                _iSelEnd = nextWord(_iSelEnd);
            }

            _selectionActive = (_iSelStart != _iSelEnd);
        }
        else
        {
            _state = TEIS_DEFAULT;
        }
        break;
    }
    };

    if(_state != state)
        INF << GUI::toString(state) << L" -> " << GUI::toString(_state) << L" LMB " << Core::toString(lmbDown) << L"\n";

    if(_pFocus->hasFocus(this))
    {
        int32 oldCursorPos = _cursorPos;
        int32 oldSize = _editText.size();

        if(_pInput->isKeyDown(VK_SHIFT))
        {
            if(!_selectionActive)
            {
                _selectionActive = true;
                _selectionCursor = _cursorPos;
            }

            if(_pInput->isKeyTyped(VK_LEFT))
                if(_pInput->isKeyDown(VK_CONTROL))
                    _cursorPos = prevWord(_cursorPos);
                else
                    _cursorPos = std::max(0, _cursorPos - 1);
            else if(_pInput->isKeyTyped(VK_RIGHT))
                if(_pInput->isKeyDown(VK_CONTROL))
                    _cursorPos = nextWord(_cursorPos);
                else
                    _cursorPos = std::min(_editText.size(), _cursorPos + 1);
            else if(_pInput->isKeyTyped(VK_HOME))
                _cursorPos = 0;
            else if(_pInput->isKeyTyped(VK_END))
                _cursorPos = _editText.size();

            _iSelStart = std::min(_cursorPos, _selectionCursor);
            _iSelEnd = std::max(_cursorPos, _selectionCursor);
        }
        else
        {
            if(_pInput->isKeyTyped(VK_LEFT))
            {
                if(_pInput->isKeyDown(VK_CONTROL))
                    _cursorPos = prevWord(_cursorPos);
                else
                    _cursorPos = std::max(0, _cursorPos - 1);
                _selectionActive = false;
            }
            else if(_pInput->isKeyTyped(VK_RIGHT))
            {
                if(_pInput->isKeyDown(VK_CONTROL))
                    _cursorPos = nextWord(_cursorPos);
                else
                    _cursorPos = std::min(_editText.size(), _cursorPos + 1);
                _selectionActive = false;
            }
            else if(_pInput->isKeyTyped(VK_HOME))
            {
                _cursorPos = 0;
                _selectionActive = false;
            }
            else if(_pInput->isKeyTyped(VK_END))
            {
                _cursorPos = _editText.size();
                _selectionActive = false;
            }
        }

        if(_pInput->isKeyTyped(VK_DELETE))
        {
            removeChar(false);
        }
        else if(_pInput->isKeyTyped(VK_BACK))
        {
            removeChar(true);
        }
        else if(_pInput->isKeyDown(VK_CONTROL) && _pInput->isKeyTyped('V')) // TODO : get WM_PASTE
        {
            removeSelection();
            insert(Core::System::getClipboardContent());
        }
        else if(_pInput->isKeyDown(VK_CONTROL) && _pInput->isKeyTyped('C') && _selectionActive) // TODO : get WM_COPY
        {
            Core::System::setClipboardContent(_editText.slice(_iSelStart, _iSelEnd - 1));
        }
        else if(_pInput->isKeyDown(VK_CONTROL) && _pInput->isKeyTyped('X') && _selectionActive) // TODO : get WM_CUT
        {
            Core::System::setClipboardContent(_editText.slice(_iSelStart, _iSelEnd - 1));
            removeSelection();
        }
        else if(_pInput->getKeyboardBuffer().size() > 0)
        {
            removeSelection();
            insert(_pInput->getKeyboardBuffer());
        }

        clampVisibleText();

        // Display cursor

        Core::Vector2f cursorSize(0.0f, 0.0f);

        _cursorTime = fmod(_cursorTime + float(elapsed), 1.0f);

        if(oldCursorPos != _cursorPos || oldSize != _editText.size())
            _cursorTime = 0.0f;

        _pCursor->setColor(Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f));
        _pCursor->setVisible(_cursorTime < 0.5f);
        _pCursor->setBold(true);
        _pCursor->setFont(_pText1->getFont());
        _pCursor->getTextSize(cursorSize);

        Core::Vector2f cursorPos(_pText1->getPosition() + Core::Vector2f(-0.5f * cursorSize.x, 0.0f));

        int32 cpos = _cursorPos - _iVisibleStart;

        if(cpos > 0)
        {
            String precedingText(_text.slice(0, cpos - 1));
            Core::Vector2f precedingTextSize(0.0f, 0.0f);
            _pText1->getFont()->getTextSize(precedingTextSize, precedingText, _pText1->isBold(), _pText1->isItalic());
            cursorPos.x += precedingTextSize.x;
        }

        _pCursor->setPosition(cursorPos);

        if(_pInput->isKeyTyped(VK_RETURN))
            _returnTyped = true;

        if(_returnTyped)
        {
            if(!_pInput->isKeyDown(VK_RETURN))
            {
                _returnTyped = false;
                notify(EV_COMMAND);
            }
        }
    }
    else
    {
        clampVisibleText();
        _pSelection->setVisible(false);
        _pCursor->setVisible(false);
    }
}
//-------------------------------------------------------------------------
void TextEdit::removeChar(bool back)
{
    if(_selectionActive)
    {
        removeSelection();
    }
    else
    {
        if(back)
            if(_cursorPos == 0)
                return;
            else
                _cursorPos -= 1;

        _editText.erase(_editText.begin() + _cursorPos);
        _preferForwardVisible = back;
        updateDisplay();
    }
}
//-------------------------------------------------------------------------
void TextEdit::removeSelection()
{
    if(_selectionActive)
    {
        _editText.erase(_editText.begin() + _iSelStart, _editText.begin() + _iSelEnd);
        _selectionActive = false;
        _cursorPos = _iSelStart;
        updateDisplay();
    }
}
//-------------------------------------------------------------------------
void TextEdit::insert(const String & text)
{
    if(!text.empty())
    {
        int32 maxInsert = _maxLength - _editText.size();
        int32 insertLength = std::min(maxInsert, text.size());

        _editText.insert(_editText.begin() + _cursorPos, text.begin(), text.begin() + insertLength);
        _cursorPos += insertLength;
        _iVisibleEnd += insertLength;
        updateDisplay();
    }
}
//-------------------------------------------------------------------------
void TextEdit::clampVisibleText()
{
    if(_isPassword)
    {
        _displayText.clear();
        _displayText.resize(_editText.size(), L'*');
    }
    else
        _displayText = _editText;

    if(_displayText.empty())
    {
        Label::setText(_displayText);
        _pSelection->setVisible(false);
        return;
    }

    Ptr<Renderer::IFont> pFont = _pText1->getFont();

    if(!_pFocus->hasFocus(this))
        _cursorPos = _displayText.size();

    _iVisibleStart = std::min(_iVisibleStart, _cursorPos);
    _iVisibleEnd = std::max(_iVisibleEnd, _cursorPos - 1);

    _iVisibleStart = std::max(0, _iVisibleStart);
    _iVisibleEnd = std::min(_iVisibleEnd, _displayText.size() - 1);

    String         visibleText(_displayText.slice(_iVisibleStart, _iVisibleEnd));
    Core::Vector2f visiblePartSize;

    pFont->getTextSize(visiblePartSize, visibleText, _pText1->isBold(), _pText1->isItalic());

    while(visiblePartSize.x > _size.x)
    {
        if(_preferForwardVisible)
        {
            if(_iVisibleStart == _cursorPos)
                _iVisibleEnd --;
            else
                _iVisibleStart ++;
        }
        else
        {
            if(_iVisibleEnd == _cursorPos)
                _iVisibleStart ++;
            else
                _iVisibleEnd --;
        }

        visibleText = _displayText.slice(_iVisibleStart, _iVisibleEnd);
        pFont->getTextSize(visiblePartSize, visibleText, _pText1->isBold(), _pText1->isItalic());
    }

    if(_selectionActive)
    {
        int32 i1 = std::max(0, std::min(_iSelStart - _iVisibleStart, _text.size()));
        int32 i2 = std::max(0, std::min(_iSelEnd - _iVisibleStart, _text.size()));

        String precText1(_text.slice(0, i1 - 1));
        String precText2(_text.slice(i1, i2 - 1));

        Core::Vector2f size1(0.0f, 0.0f);
        Core::Vector2f size2(0.0f, 0.0f);

        _pText1->getFont()->getTextSize(size1, precText1, _pText1->isBold(), _pText1->isItalic());
        _pText1->getFont()->getTextSize(size2, precText2, _pText1->isBold(), _pText1->isItalic());

        _pSelection->setPosition(_pText1->getPosition() + Core::Vector2f(size1.x, 0.0f));
        _pSelection->setSize(size2);

        _pSelection->setVisible(true);
    }
    else
    {
        _pSelection->setVisible(false);
    }

    Label::setText(visibleText);
}
//-------------------------------------------------------------------------
int32 TextEdit::mouseIndex()
{
    Ptr<Renderer::IFont> pFont = _pText1->getFont();
    Core::Vector2f mpos = _pInput->getMousePos() - _screenPos;

    if(mpos.x < 0.0f)
        return std::max(0, _iVisibleStart - 1);

    Core::Vector2f lastSize(0.0f, 0.0f);

    for(int32 ii=0; ii < _text.size(); ii++)
    {
        String part(_text.slice(0, ii));
        Core::Vector2f size(0.0f, 0.0f);
        _pText1->getFont()->getTextSize(size, part, _pText1->isBold(), _pText1->isItalic());

        if(0.5f * (lastSize.x + size.x) > mpos.x)
            return ii + _iVisibleStart;

        lastSize = size;
    }

    return _displayText.size();
}
//-------------------------------------------------------------------------
int32 TextEdit::prevWord(int32 ii)
{
    int32 iPrev = _editText.find_last_not_of(L" ", std::max(0, ii - 1));
    
    if(iPrev == String::npos)
    {
        return 0;
    }
    else
    {
        iPrev = _editText.find_last_of(L" ", iPrev);
        return (iPrev == String::npos) ? 0 : (iPrev + 1);
    }
}
//-------------------------------------------------------------------------
int32 TextEdit::nextWord(int32 ii)
{
    if(!_editText.empty() && _editText[std::max(std::min(ii, _editText.size() - 1), 0)] == L' ')
    {
        int32 iNext = _editText.find_first_not_of(L" ", std::min(_editText.size() - 1, ii + 1));
        
        if(iNext == String::npos)
            return _editText.size();
        else
            return iNext;
    }
    else
    {
        int32 iNext = _editText.find_first_of(L" ", ii);

        if(iNext == String::npos)
        {
            return _editText.size();
        }
        else
        {
            iNext = _editText.find_first_not_of(L" ", iNext);
            return (iNext == String::npos) ? _editText.size() : iNext;
        }
    }
}
//-------------------------------------------------------------------------
void TextEdit::onVisibilityChanged()
{
    Label::onVisibilityChanged();
    if(!isVisible())
    {
        _pCursor->setVisible(false);
        _pSelection->setVisible(false);
    }
    update(0.0);
}
//-------------------------------------------------------------------------
void TextEdit::onPriorityChanged(int32 p)
{
    Label::onPriorityChanged(p); // Les deux textes sont dÈj‡ +0 et +1
    _pCursor->setPriority(p + 5);
    _pSelection->setPriority(p + 4);
}
//-------------------------------------------------------------------------
}
