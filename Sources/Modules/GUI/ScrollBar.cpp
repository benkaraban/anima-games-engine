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
#include <GUI/ScrollBar.h>

namespace GUI
{
//-------------------------------------------------------------------------
VScrollBar::VScrollBar(const Ptr<ResourcePool> & pResPool,
                     const Ptr<Renderer::IHUD> & pHUD,
                     const Ptr<Window::InputManager> & pInput) :
    _pResPool(pResPool),
    _pInput(pInput),
    _minv(0),
    _maxv(0),
    _backColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barHighColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barSize(1),
    _state(SIS_DEFAULT)
{
    _pRectBack = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pRectBar = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);

    _pRectBack->setVisible(false);
    _pRectBar->setVisible(false);
}
//-------------------------------------------------------------------------
void VScrollBar::onVisibilityChanged()
{
    _pRectBack->setVisible(isVisible());
    _pRectBar->setVisible(isVisible());
}
//-------------------------------------------------------------------------
void VScrollBar::setBackgroundPicture(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRectBack->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRectBack->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _backPicName = pic;
}
//-------------------------------------------------------------------------
void VScrollBar::setBarPicture(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRectBar->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRectBar->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _barPicName = pic;
}
//-------------------------------------------------------------------------
void VScrollBar::setRange(int32 minv, int32 maxv, int32 barSize)
{
    _minv = minv;
    _maxv = maxv;
    _barSize = std::max(1, barSize);
    updateDisplay();
}
//-------------------------------------------------------------------------
void VScrollBar::setValue(int32 v)
{
    _value = v;
    updateDisplay();
}
//-------------------------------------------------------------------------
void VScrollBar::setBackgroundColor(const Core::Vector4f & col)
{
    _backColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void VScrollBar::setBarColor(const Core::Vector4f & col)
{
    _barColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void VScrollBar::setBarHighlightColor(const Core::Vector4f & col)
{
    _barHighColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void VScrollBar::pack()
{
    const Renderer::PictureInfos & p = _pRectBack->getPicture();
    setSize(Core::Vector2f(float(p.width), float(p.height)));
}
//-------------------------------------------------------------------------
const String & VScrollBar::getBackgroundPicture() const
{
    return _backPicName;
}
//-------------------------------------------------------------------------
void VScrollBar::updateDisplay()
{
    _value = std::max(_minv, std::min(_maxv - _barSize, _value));
    int32 range = std::max(1, _maxv - _minv);

    float relativeValue = float(_value) / float(range);
    float relativeSize = float(_barSize) / float(range);


    _pRectBack->setSize(_size);
    _pRectBack->setPosition(_screenPos);

    _pRectBar->setSize(Core::Vector2f(_size.x, relativeSize * _size.y));
    _pRectBar->setPosition(_screenPos + Core::Vector2f(0.0f, relativeValue * _size.y));
}
//-------------------------------------------------------------------------
void VScrollBar::onColorChanged()
{
    _pRectBack->setColor(Core::mul(getColor(), _backColor));

    if(_state == SIS_DEFAULT)
        _pRectBar->setColor(Core::mul(getColor(), _barColor));
    else
        _pRectBar->setColor(Core::mul(getColor(), _barHighColor));
}
//-------------------------------------------------------------------------
void VScrollBar::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void VScrollBar::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void VScrollBar::onPositionChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void VScrollBar::onPriorityChanged(int32 p)
{
    _pRectBack->setPriority(p);
    _pRectBar->setPriority(p + 1);
}
//-------------------------------------------------------------------------
void VScrollBar::onSizeChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void VScrollBar::onKill()
{
    _pRectBack->kill();
    _pRectBar->kill();
}
//-------------------------------------------------------------------------
void VScrollBar::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
        return;

    Core::Vector2f lmBar  = _pInput->getMousePos() - _pRectBar->getPosition();
    Core::Vector2f lmBack = _pInput->getMousePos() - _screenPos;
    Core::Vector2f barSize = _pRectBar->getSize();
    bool mouseOverBar  = (lmBar.x >= 0.0f) && (lmBar.y >= 0.0f) && (lmBar.x < barSize.x) && (lmBar.y < barSize.y);
    bool mouseOverBack = (lmBack.x >= 0.0f) && (lmBack.y >= 0.0f) && (lmBack.x < _size.x) && (lmBack.y < _size.y);
    bool lmbDown = _pInput->isLMBDown();

    if(!isEnabled())
        _state = SIS_DEFAULT;

    switch(_state)
    {
    case SIS_DEFAULT:
    {
        if(mouseOverBar)
        {
            if(!lmbDown)
            {
                _state = SIS_MOUSE_OVER_BAR;
            }
        }
        break;
    }
    case SIS_MOUSE_OVER_BAR:
    {
        if(mouseOverBar)
        {
            if(lmbDown)
            {
                _mouseDY = lmBack.y - (_pRectBar->getPosition().y - _screenPos.y);
                _state = SIS_MOUSE_GRIP_BAR;
            }
        }
        else
        {
            _state = SIS_DEFAULT;
        }
        break;
    }
    case SIS_MOUSE_GRIP_BAR:
    {
        if(lmbDown)
        {
            float y = lmBack.y - _mouseDY;
            float relativeValue = y / _size.y;
            setValue(int32(relativeValue * float(_maxv - _minv)));
            notify(EV_SCROLLBAR);
        }
        else
        {
            _state = SIS_DEFAULT;
        }
        break;
    }
    }

    if(!mouseOverBar && mouseOverBack && _pInput->isLMBClicked())
    {
        if(lmBar.y > 0.0f)
        {
            setValue(_value + std::max(1, (_barSize - 2)));
            notify(EV_SCROLLBAR);
        }
        else
        {
            setValue(_value - std::max(1, (_barSize - 2)));
            notify(EV_SCROLLBAR);
        }
    }

    onColorChanged();
}
//-------------------------------------------------------------------------
//*************************************************************************
//-------------------------------------------------------------------------
HScrollBar::HScrollBar(const Ptr<ResourcePool> & pResPool,
                     const Ptr<Renderer::IHUD> & pHUD,
                     const Ptr<Window::InputManager> & pInput) :
    _pResPool(pResPool),
    _pInput(pInput),
    _minv(0),
    _maxv(0),
    _backColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barHighColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barSize(1),
    _state(SIS_DEFAULT)
{
    _pRectBack = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pRectBar = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);

    _pRectBack->setVisible(false);
    _pRectBar->setVisible(false);
}
//-------------------------------------------------------------------------
void HScrollBar::onVisibilityChanged()
{
    _pRectBack->setVisible(isVisible());
    _pRectBar->setVisible(isVisible());
}
//-------------------------------------------------------------------------
void HScrollBar::setBackgroundPicture(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRectBack->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRectBack->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _backPicName = pic;
}
//-------------------------------------------------------------------------
void HScrollBar::setBarPicture(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRectBar->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRectBar->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _barPicName = pic;
}
//-------------------------------------------------------------------------
void HScrollBar::setRange(int32 minv, int32 maxv, int32 barSize)
{
    _minv = minv;
    _maxv = maxv;
    _barSize = std::max(1, barSize);
    updateDisplay();
}
//-------------------------------------------------------------------------
void HScrollBar::setValue(int32 v)
{
    _value = v;
    updateDisplay();
}
//-------------------------------------------------------------------------
void HScrollBar::setValuePercent(float percent)
{
    _value = int32(_minv + percent *(_maxv - _barSize - _minv));
    updateDisplay();
}
//-------------------------------------------------------------------------
float HScrollBar::getValuePercent() const
{
    return float(_value - _minv) / float(_maxv - _barSize - _minv);
}
//-------------------------------------------------------------------------
void HScrollBar::setBackgroundColor(const Core::Vector4f & col)
{
    _backColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void HScrollBar::setBarColor(const Core::Vector4f & col)
{
    _barColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void HScrollBar::setBarHighlightColor(const Core::Vector4f & col)
{
    _barHighColor = col;
    onColorChanged();
}
//-------------------------------------------------------------------------
void HScrollBar::pack()
{
    const Renderer::PictureInfos & p = _pRectBack->getPicture();
    setSize(Core::Vector2f(float(p.width), float(p.height)));
}
//-------------------------------------------------------------------------
const String & HScrollBar::getBackgroundPicture() const
{
    return _backPicName;
}
//-------------------------------------------------------------------------
void HScrollBar::updateDisplay()
{
    _value = std::max(_minv, std::min(_maxv - _barSize, _value));
    int32 range = std::max(1, _maxv - _minv);

    float relativeValue = float(_value) / float(range);
    float relativeSize = float(_barSize) / float(range);


    _pRectBack->setSize(_size);
    _pRectBack->setPosition(_screenPos);

    _pRectBar->setSize(Core::Vector2f(relativeSize * _size.x, _size.y));
    _pRectBar->setPosition(_screenPos + Core::Vector2f(relativeValue * _size.x, 0.0f));
}
//-------------------------------------------------------------------------
void HScrollBar::onColorChanged()
{
    _pRectBack->setColor(Core::mul(getColor(), _backColor));

    if(_state == SIS_DEFAULT)
        _pRectBar->setColor(Core::mul(getColor(), _barColor));
    else
        _pRectBar->setColor(Core::mul(getColor(), _barHighColor));
}
//-------------------------------------------------------------------------
void HScrollBar::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void HScrollBar::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void HScrollBar::onPositionChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void HScrollBar::onPriorityChanged(int32 p)
{
    _pRectBack->setPriority(p);
    _pRectBar->setPriority(p + 1);
}
//-------------------------------------------------------------------------
void HScrollBar::onSizeChanged()
{
    updateDisplay();
}
//-------------------------------------------------------------------------
void HScrollBar::onKill()
{
    _pRectBack->kill();
    _pRectBar->kill();
}
//-------------------------------------------------------------------------
void HScrollBar::update(double elapsed)
{
    Widget::update(elapsed);

    if(!isActive())
        return;

    Core::Vector2f lmBar  = _pInput->getMousePos() - _pRectBar->getPosition();
    Core::Vector2f lmBack = _pInput->getMousePos() - _screenPos;
    Core::Vector2f barSize = _pRectBar->getSize();
    bool mouseOverBar  = (lmBar.x >= 0.0f) && (lmBar.y >= 0.0f) && (lmBar.x < barSize.x) && (lmBar.y < barSize.y);
    bool mouseOverBack = (lmBack.x >= 0.0f) && (lmBack.y >= 0.0f) && (lmBack.x < _size.x) && (lmBack.y < _size.y);
    bool lmbDown = _pInput->isLMBDown();

    if(_pKeyNav != NULL && _pKeyNav->ignoreMouse)
    {
        mouseOverBack = mouseOverBar = (_pKeyNav->pKeyFocus == this);
        lmbDown = false;
    }

    if(!isEnabled())
        _state = SIS_DEFAULT;

    switch(_state)
    {
    case SIS_DEFAULT:
    {
        if(mouseOverBar)
        {
            if(!lmbDown)
            {
                _state = SIS_MOUSE_OVER_BAR;
            }
        }
        break;
    }
    case SIS_MOUSE_OVER_BAR:
    {
        if(mouseOverBar)
        {
            if(lmbDown)
            {
                _mouseDX = lmBack.x - (_pRectBar->getPosition().x - _screenPos.x);
                _state = SIS_MOUSE_GRIP_BAR;
            }
        }
        else
        {
            _state = SIS_DEFAULT;
        }
        break;
    }
    case SIS_MOUSE_GRIP_BAR:
    {
        if(lmbDown)
        {
            float x = lmBack.x - _mouseDX;
            float relativeValue = x / _size.x;
            setValue(int32(relativeValue * float(_maxv - _minv)));
            notify(EV_SCROLLBAR);
        }
        else
        {
            _state = SIS_DEFAULT;
        }
        break;
    }
    }

    if(!mouseOverBar && mouseOverBack && _pInput->isLMBClicked())
    {
        if(lmBar.x > 0.0f)
        {
            setValue(_value + std::max(1, (_barSize - 2)));
            notify(EV_SCROLLBAR);
        }
        else
        {
            setValue(_value - std::max(1, (_barSize - 2)));
            notify(EV_SCROLLBAR);
        }
    }

    if(_pKeyNav != NULL && isActive())
    {
        if(_state == SIS_MOUSE_OVER_BAR)
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

        if(_pKeyNav->ignoreMouse && _pKeyNav->pKeyFocus == this)
        {
            if(_pKeyNav->pCtrl->right())
            {
                setValue(_value + std::max(1, (_barSize - 2)));
                notify(EV_SCROLLBAR);
            }
            else if(_pKeyNav->pCtrl->left())
            {
                setValue(_value - std::max(1, (_barSize - 2)));
                notify(EV_SCROLLBAR);
            }
        }
    }

    onColorChanged();
}
}

