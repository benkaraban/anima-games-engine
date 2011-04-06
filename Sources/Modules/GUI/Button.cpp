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
#include <GUI/Button.h>

namespace GUI
{

//-------------------------------------------------------------------------
Button::Button(const Ptr<ResourcePool> & pResPool,
               const Ptr<Renderer::IHUD> & pHUD,
               const Ptr<Window::InputManager> & pInput) :
    _pInput(pInput),
    _pPicture(new Picture(pResPool, pHUD)),
    _pLabel(new Label(pResPool, pHUD)),
    _curRenderState(BS_DEFAULT),
    _curState(BIS_DEFAULT),
    _hotKey1(-1),
    _hotKey2(-1)
{
    addChild(_pPicture);
    addChild(_pLabel);

    _pLabel->setDeltaPriority(200);

    updateRenderState();
}
//-------------------------------------------------------------------------
ButtonStyle::ButtonStyle() :
    fontName(L"Arial"),
    fontSize(12),
    bold(false),
    italic(false),
    shadow(true)
{
    state[BS_DISABLED].picColor = Core::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
    state[BS_DISABLED].textColor = Core::Vector4f(0.9f, 0.9f, 0.9f, 1.0f);

    state[BS_PUSHED].posDelta = Core::Vector2f(2.0f, 2.0f);
}
//-------------------------------------------------------------------------
ButtonState::ButtonState() :
    picColor(1.0f, 1.0f, 1.0f, 1.0f),
    posDelta(0.0f, 0.0f),
    textColor(1.0f, 1.0f, 1.0f, 1.0f)
{
}
//-------------------------------------------------------------------------
void Button::setText(const String & text)
{
    _style.text = text;
    updateRenderState();
}
//-------------------------------------------------------------------------
const String & Button::getText()
{
    return _style.text;
}
//-------------------------------------------------------------------------
void Button::setStyle(const ButtonStyle & style)
{
    _style = style;
    updateRenderState();
}
//-------------------------------------------------------------------------
const ButtonStyle & Button::getStyle() const
{
    return _style;
}
//-------------------------------------------------------------------------
void Button::updateRenderState()
{
    const ButtonState & state = _style.state[int32(_curRenderState)];
    
    _pPicture->setPicture(state.picName);
    _pPicture->setOwnColor(state.picColor);
    _pPicture->setPosition(state.posDelta);
    _pPicture->setVisible(true);

    if(state.picName == String::EMPTY)
    {
        _pPicture->setSize(_size);
    }
    else
    {
        _size = _pPicture->getSize();
    }

    LabelStyle ls;
    ls.fontName   = _style.fontName;
    ls.fontSize   = _style.fontSize;
    ls.bold       = _style.bold;
    ls.italic     = _style.italic;
    ls.shadow     = _style.shadow;       
    ls.color      = state.textColor;
    ls.hLayout    = HL_CENTER;
    ls.vLayout    = VL_CENTER;

    _pLabel->setDictionary(_pDico);
    _pLabel->setText(_style.text);
    _pLabel->setStyle(ls);
    _pLabel->setVisible(true);
    _pLabel->setPosition(state.posDelta);
    _pLabel->setSize(_pPicture->getSize());
}
//-------------------------------------------------------------------------
void Button::setHotKey(int32 key1)
{
    _hotKey1 = key1;
    _hotKey2 = -1;
}
//-------------------------------------------------------------------------
void Button::setHotKey(int32 key1, int32 key2)
{
    _hotKey1 = key1;
    _hotKey2 = key2;
}
//-------------------------------------------------------------------------
void Button::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
        return;

    Core::Vector2f lm = _pInput->getMousePos() - _screenPos;
    bool mouseOver = (lm.x >= 0.0f) && (lm.y >= 0.0f) && (lm.x < _size.x) && (lm.y < _size.y);
    bool lmbDown = _pInput->isLMBDown();
    bool hotKey = false;
    bool hotNew = false;

    if(_pKeyNav != NULL && _pKeyNav->ignoreMouse)
    {
        mouseOver = (_pKeyNav->pKeyFocus == this);
        lmbDown = (_pKeyNav->pCtrl->enter());
    }

    if(_hotKey1 != -1 && _hotKey2 != -1)
    {
        hotKey = _pInput->isKeyDown(_hotKey1) && _pInput->isKeyDown(_hotKey2);
        hotNew = (_pInput->isKeyDown(_hotKey1) && _pInput->isKeyTyped(_hotKey2)) ||
                 (_pInput->isKeyTyped(_hotKey1) && _pInput->isKeyDown(_hotKey2));
    }
    else if(_hotKey1 != -1)
    {
        hotKey = _pInput->isKeyDown(_hotKey1);
        hotNew = _pInput->isKeyTyped(_hotKey1);
    }

    if(!isEnabled() && (_curState != BIS_DISABLED && _curState != BIS_DISABLED_MOUSE_OVER) )
        _curState = BIS_DISABLED;

    if(_curState != BIS_PUSHED_BY_HOTKEY && hotNew && isEnabled())
    {
        _curState = BIS_PUSHED_BY_HOTKEY;
        setRenderState(BS_PUSHED);
    }

    switch(_curState)
    {
    case BIS_DISABLED:
        if(isEnabled())
        {
            if(mouseOver)
            {
                if(lmbDown)
                {
                    setRenderState(BS_PUSHED);
                    _curState = BIS_PUSHED_OVER;
                    internalNotify(EV_OVER_BEGIN);
                }
                else
                {
                    setRenderState(BS_MOUSE_OVER);
                    _curState = BIS_MOUSE_OVER;
                    internalNotify(EV_OVER_BEGIN);
                }
            }
            else
            {
                setRenderState(BS_DEFAULT);
                _curState = BIS_DEFAULT;
            }
        }
        else
        {
            if(mouseOver)
            {
                setRenderState(BS_DISABLED_MOUSE_OVER);
                _curState = BIS_DISABLED_MOUSE_OVER;
                internalNotify(EV_OVER_BEGIN);
            }
            else
                setRenderState(BS_DISABLED);
        }
        break;

    case BIS_DISABLED_MOUSE_OVER:
        if(isEnabled())
        {
            if(mouseOver)
            {
                if(lmbDown)
                {
                    setRenderState(BS_PUSHED);
                    _curState = BIS_PUSHED_OVER;
                    internalNotify(EV_OVER_BEGIN);
                }
                else
                {
                    setRenderState(BS_MOUSE_OVER);
                    _curState = BIS_MOUSE_OVER;
                    internalNotify(EV_OVER_BEGIN);
                }
            }
            else
            {
                setRenderState(BS_DEFAULT);
                _curState = BIS_DEFAULT;
            }
        }
        else
        {
            if(!mouseOver)
            {
                setRenderState(BS_DISABLED);
                _curState = BIS_DISABLED;
                internalNotify(EV_OVER_END);
            }
        }
        break;

    case BIS_DEFAULT:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                setRenderState(BS_MOUSE_OVER);
                _curState = BIS_MOUSE_OVER;
                internalNotify(EV_OVER_BEGIN);
            }
        }
        else
        {
            setRenderState(BS_DEFAULT);
            _curState = BIS_DEFAULT;
        }
        break;

    case BIS_MOUSE_OVER:
        if(mouseOver)
        {
            if(lmbDown)
            {
                setRenderState(BS_PUSHED);
                _curState = BIS_PUSHED_OVER;
            }
        }
        else
        {
            setRenderState(BS_DEFAULT);
            _curState = BIS_DEFAULT;
            internalNotify(EV_OVER_END);
        }
        break;

    case BIS_PUSHED_OVER:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                internalNotify(EV_COMMAND);
                setRenderState(BS_MOUSE_OVER);
                _curState = BIS_MOUSE_OVER;
            }
        }
        else
        {
            _curState = BIS_PUSHED_NOT_OVER;
        }
        break;

    case BIS_PUSHED_NOT_OVER:
        if(lmbDown)
        {
            if(mouseOver)
            {
                _curState = BIS_PUSHED_OVER;
                setRenderState(BS_PUSHED);
            }
            else
            {
                setRenderState(BS_MOUSE_OVER);
            }
        }
        else
        {
            _curState = BIS_DEFAULT;
            setRenderState(BS_DEFAULT);
            internalNotify(EV_OVER_END);
        }
        break;

    case BIS_PUSHED_BY_HOTKEY:
        if(!hotKey)
        {
            internalNotify(EV_COMMAND);

            if(mouseOver)
            {
                _curState = BIS_MOUSE_OVER;
                setRenderState(BS_MOUSE_OVER);
            }
            else
            {
                _curState = BIS_DEFAULT;
                setRenderState(BS_DEFAULT);
            }
        }
        break;
    }

    if(_pKeyNav != NULL && isActive())
    {
        if(_curState == BIS_MOUSE_OVER)
        {
            _pKeyNav->pKeyFocus = this;
            updateKeyNavigation();
        }
        else
        {
            if(_pKeyNav->pKeyFocus == this)
            {
                if(_pKeyNav->pCtrl->left() || _pKeyNav->pCtrl->right() || _pKeyNav->pCtrl->up() || _pKeyNav->pCtrl->down())
                {
                    _pKeyNav->ignoreMouse = true;
                }
            }
        }
    }
}
//-------------------------------------------------------------------------
void Button::setRenderState(EButtonRenderState rs)
{
    if(rs != _curRenderState)
    {
        _curRenderState = rs;
        updateRenderState();
    }
}
//-------------------------------------------------------------------------
void Button::onVisibilityChanged()
{
}
//-------------------------------------------------------------------------
void Button::onEnableChanged()
{
    _curState = BIS_DISABLED;
    _curRenderState = BS_DISABLED;
    updateRenderState();
}
//-------------------------------------------------------------------------
void Button::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void Button::onPositionChanged()
{
}
//-------------------------------------------------------------------------
void Button::onPriorityChanged(int32 p)
{
}
//-------------------------------------------------------------------------
void Button::onSizeChanged()
{
    updateRenderState();
}
//-------------------------------------------------------------------------
void Button::onColorChanged()
{
    updateRenderState();
}
//-------------------------------------------------------------------------
void Button::onKill()
{
}
//-------------------------------------------------------------------------
void Button::internalNotify(EEvent ev)
{
    notify(ev);
}
//-------------------------------------------------------------------------
}
