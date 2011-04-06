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
#include <GUI/CheckBox.h>

namespace GUI
{

//-------------------------------------------------------------------------
CheckBox::CheckBox(const Ptr<ResourcePool> & pResPool,
               const Ptr<Renderer::IHUD> & pHUD,
               const Ptr<Window::InputManager> & pInput) :
    _pInput(pInput),
    _pPicture(new Picture(pResPool, pHUD)),
    _pLabel(new Label(pResPool, pHUD)),
    _curRenderState(CBS_DEFAULT),
    _curState(CBIS_DEFAULT)

{
    addChild(_pPicture);
    addChild(_pLabel);

    _pLabel->setDeltaPriority(200);

    updateRenderState();
}
//-------------------------------------------------------------------------
CheckBoxStyle::CheckBoxStyle() :
    fontName(L"Arial"),
    fontSize(12),
    bold(false),
    italic(false),
    shadow(true),
    padding(0.0f)
{
    state[CBS_DISABLED].picColor = Core::Vector4f(0.5f, 0.5f, 0.5f, 1.0f);
    state[CBS_DISABLED].textColor = Core::Vector4f(0.9f, 0.9f, 0.9f, 1.0f);

    state[CBS_PUSHED].posDelta = Core::Vector2f(1.0f, 1.0f);
}
//-------------------------------------------------------------------------
CheckBoxState::CheckBoxState() :
    picColor(1.0f, 1.0f, 1.0f, 1.0f),
    textColor(1.0f, 1.0f, 1.0f, 1.0f),
    posDelta(0.0f, 0.0f)
{
}
//-------------------------------------------------------------------------
void CheckBox::setText(const String & text)
{
    _style.text = text;
    updateRenderState();
}
//-------------------------------------------------------------------------
const String & CheckBox::getText()
{
    return _style.text;
}
//-------------------------------------------------------------------------
void CheckBox::setStyle(const CheckBoxStyle & style)
{
    _style = style;
    updateRenderState();

    Core::Vector2f size = _pLabel->getSize();
    size.y = _pPicture->getSize().y;
    _pLabel->setSize(size);
    _pLabel->fitText(true, false);
    pack();
}
//-------------------------------------------------------------------------
const CheckBoxStyle & CheckBox::getStyle() const
{
    return _style;
}
//-------------------------------------------------------------------------
void CheckBox::updateRenderState()
{
    const CheckBoxState & state = _style.state[int32(_curRenderState)];
    
    _pPicture->setPicture(_isChecked ? _style.picChecked : _style.picUnchecked);
    _pPicture->setOwnColor(state.picColor);
    _pPicture->setPosition(state.posDelta);
    _pPicture->setVisible(true);

    Core::Vector2f picSize = _pPicture->getSize();
    picSize.x += _style.padding;

    Core::Vector2f textSize(_size.x - picSize.x, std::max(picSize.y, _size.y));

    LabelStyle ls;
    ls.fontName   = _style.fontName;
    ls.fontSize   = _style.fontSize;
    ls.bold       = _style.bold;
    ls.italic     = _style.italic;
    ls.shadow     = _style.shadow;       
    ls.color      = state.textColor;
    ls.hLayout    = HL_LEFT;
    ls.vLayout    = VL_CENTER;

    _pLabel->setDictionary(_pDico);
    _pLabel->setText(_style.text);
    _pLabel->setStyle(ls);
    _pLabel->setVisible(true);
    _pLabel->setPosition(state.posDelta + Core::Vector2f(picSize.x, 0.0f));
}
//-------------------------------------------------------------------------
void CheckBox::setChecked(bool checked)
{
    _isChecked = checked;

    updateRenderState();
}
//-------------------------------------------------------------------------
void CheckBox::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
        return;

    Core::Vector2f lm = _pInput->getMousePos() - _screenPos;
    bool mouseOver = (lm.x >= 0.0f) && (lm.y >= 0.0f) && (lm.x < _size.x) && (lm.y < _size.y);
    bool lmbDown = _pInput->isLMBDown();

    if(_pKeyNav != NULL && _pKeyNav->ignoreMouse)
    {
        mouseOver = (_pKeyNav->pKeyFocus == this);
        lmbDown = (_pKeyNav->pCtrl->enter());
    }

    if(!isEnabled())
        _curState = CBIS_DISABLED;

    switch(_curState)
    {
    case CBIS_DISABLED:
        if(isEnabled())
        {
            if(mouseOver)
            {
                if(lmbDown)
                {
                    setRenderState(CBS_PUSHED);
                    _curState = CBIS_PUSHED_OVER;
                }
                else
                {
                    setRenderState(CBS_MOUSE_OVER);
                    _curState = CBIS_MOUSE_OVER;
                }
            }
            else
            {
                setRenderState(CBS_DEFAULT);
                _curState = CBIS_DEFAULT;
            }
        }
        else
        {
            setRenderState(CBS_DISABLED);
        }
        break;

    case CBIS_DEFAULT:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                setRenderState(CBS_MOUSE_OVER);
                _curState = CBIS_MOUSE_OVER;
            }
        }
        else
        {
            setRenderState(CBS_DEFAULT);
            _curState = CBIS_DEFAULT;
        }
        break;

    case CBIS_MOUSE_OVER:
        if(mouseOver)
        {
            if(lmbDown)
            {
                setRenderState(CBS_PUSHED);
                _curState = CBIS_PUSHED_OVER;
            }
        }
        else
        {
            setRenderState(CBS_DEFAULT);
            _curState = CBIS_DEFAULT;
        }
        break;

    case CBIS_PUSHED_OVER:
        if(mouseOver)
        {
            if(!lmbDown)
            {
                _isChecked = !_isChecked;
                internalNotify(EV_COMMAND);
                setRenderState(CBS_MOUSE_OVER);
                _curState = CBIS_MOUSE_OVER;
            }
        }
        else
        {
            _curState = CBIS_PUSHED_NOT_OVER;
        }
        break;

    case CBIS_PUSHED_NOT_OVER:
        if(lmbDown)
        {
            if(mouseOver)
            {
                _curState = CBIS_PUSHED_OVER;
                setRenderState(CBS_PUSHED);
            }
            else
            {
                setRenderState(CBS_MOUSE_OVER);
            }
        }
        else
        {
            _curState = CBIS_DEFAULT;
            setRenderState(CBS_DEFAULT);
        }
    }

    if(_pKeyNav != NULL && isActive())
    {
        if(_curState == CBIS_MOUSE_OVER)
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
void CheckBox::setRenderState(ECheckBoxRenderState rs)
{
    if(rs != _curRenderState)
    {
        _curRenderState = rs;
        updateRenderState();
    }
}
//-------------------------------------------------------------------------
void CheckBox::onVisibilityChanged()
{
}
//-------------------------------------------------------------------------
void CheckBox::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void CheckBox::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void CheckBox::onPositionChanged()
{
}
//-------------------------------------------------------------------------
void CheckBox::onPriorityChanged(int32 p)
{
}
//-------------------------------------------------------------------------
void CheckBox::onSizeChanged()
{
    updateRenderState();
}
//-------------------------------------------------------------------------
void CheckBox::onColorChanged()
{
    updateRenderState();
}
//-------------------------------------------------------------------------
void CheckBox::onKill()
{
}
//-------------------------------------------------------------------------
void CheckBox::internalNotify(EEvent ev)
{
    notify(ev);
}
//-------------------------------------------------------------------------
}
