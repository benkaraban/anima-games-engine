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
#include <GUI/Widget.h>

#include <Core/Logger.h>

namespace GUI
{
int32 Widget::sUID = -1;
//-------------------------------------------------------------------------
Focus::Focus() : _pOwner(NULL)
{
}
//-------------------------------------------------------------------------
void Focus::setOwner(Widget * pOwner)
{
    if(_pOwner != NULL)
        _pOwner->loseFocus();

    _pOwner = pOwner;

    if(_pOwner != NULL)
        _pOwner->gainFocus();
}
//-------------------------------------------------------------------------
Widget::Widget() :
    _pKeyNav(NULL),
    _pPrevWidget(NULL),
    _pNextWidget(NULL),
    _pKeyWidgetL(NULL),
    _pKeyWidgetR(NULL),
    _pKeyWidgetU(NULL),
    _pKeyWidgetD(NULL),
    _id(sUID),
    _parentPriority(0),
    _deltaPriority(100),
    _parentPos(0.0f, 0.0f),
    _localPos(0.0f, 0.0f),
    _screenPos(0.0f, 0.0f),
    _size(0.0f, 0.0f),
    _clipRect(0.0f, 0.0f),
    _parentColor(Core::Vector4f::ONE),
    _ownColor(Core::Vector4f::ONE),
    _color(Core::Vector4f::ONE),
    _colorChangeDuration(0.0f),
    _colorChangeElapsed(0.0f),
    _color1(Core::Vector4f::ONE),
    _color2(Core::Vector4f::ONE),
    _isAlive(true),
    _isVisible(true),
    _isEnabled(true),
    _isActive(true),
    _isParentVisible(false),
    _isParentEnabled(true),
    _isParentActive(false)
{
    sUID --;
}
//-------------------------------------------------------------------------
int32 Widget::getId() const
{
    return _id;
}
//-------------------------------------------------------------------------
void Widget::setId(int32 id)
{
    _id = id;
}
//-------------------------------------------------------------------------
void Widget::setDictionary(const Ptr<Core::Dictionary> & pDico)
{
    _pDico = pDico;
}
//-------------------------------------------------------------------------
void Widget::setParentVisible(bool visible)
{
    bool old = isVisible();
    _isParentVisible = visible;

    if(isVisible() != old)
    {
        onVisibilityChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentVisible(isVisible());
    }
}
//-------------------------------------------------------------------------
void Widget::setVisible(bool visible)
{
    bool old = isVisible();
    _isVisible = visible;

    if(isVisible() != old)
    {
        onVisibilityChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentVisible(isVisible());
    }
}
//-------------------------------------------------------------------------
bool Widget::isVisible() const
{
    return _isVisible && _isParentVisible;
}
//-------------------------------------------------------------------------
void Widget::setParentEnabled(bool enabled)
{
    bool old = isVisible();
    _isParentEnabled = enabled;

    if(isEnabled() != old)
    {
        onEnableChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentEnabled(isEnabled());
    }
}
//-------------------------------------------------------------------------
void Widget::setEnabled(bool enabled)
{
    bool old = isEnabled();
    _isEnabled = enabled;

    if(isEnabled() != old)
    {
        onEnableChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentEnabled(isEnabled());
    }
}
//-------------------------------------------------------------------------
bool Widget::isEnabled() const
{
    return _isEnabled && _isParentEnabled;
}
//-------------------------------------------------------------------------
void Widget::kill()
{
    _isAlive = false;
    onKill();

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->kill();

    _children.clear();
    _evHandlers.clear();
}
//-------------------------------------------------------------------------
bool Widget::isAlive() const
{
    return _isAlive;
}
//-------------------------------------------------------------------------
void Widget::setParentPosition(const Core::Vector2f & pos)
{
    _parentPos = pos;
    _screenPos = _parentPos + _localPos;

    onPositionChanged();

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setParentPosition(_screenPos);
}
//-------------------------------------------------------------------------
void Widget::setPosition(const Core::Vector2f & pos)
{
    _localPos = pos;
    _screenPos = _parentPos + _localPos;

    onPositionChanged();

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setParentPosition(_screenPos);
}
//-------------------------------------------------------------------------
const Core::Vector2f & Widget::getPosition() const
{
    return _localPos;
}
//-------------------------------------------------------------------------
void Widget::setSize(const Core::Vector2f & size)
{
    _size = size;
    onSizeChanged();
}
//-------------------------------------------------------------------------
const Core::Vector2f & Widget::getSize() const
{
    return _size;
}
//-------------------------------------------------------------------------
void Widget::setClipRect(const Core::Vector2f & size)
{
    _clipRect = size;
    onClipRectChanged();
}
//-------------------------------------------------------------------------
const Core::Vector2f & Widget::getClipRect() const
{
    return _clipRect;
}
//-------------------------------------------------------------------------
void Widget::addEventHandler(IEventHandler * pEvHandler)
{
    _evHandlers.push_back(pEvHandler);
}
//-------------------------------------------------------------------------
void Widget::removeEventHandler(IEventHandler * pEvHandler)
{
    Core::List<IEventHandler * >::iterator iEv = std::find(_evHandlers.begin(), _evHandlers.end(), pEvHandler);

    if(iEv != _evHandlers.end())
        _evHandlers.erase(iEv);
}
//-------------------------------------------------------------------------
void Widget::setParentPriority(int32 p)
{
    _parentPriority = p;
    int32 newPriority = _parentPriority + _deltaPriority;
    onPriorityChanged(newPriority);

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setParentPriority(newPriority);
}
//-------------------------------------------------------------------------
int32 Widget::getPriority() const
{
    return _parentPriority + _deltaPriority;
}
//-------------------------------------------------------------------------
int32 Widget::getDeltaPriority() const
{
    return _deltaPriority;
}
//-------------------------------------------------------------------------
void Widget::setDeltaPriority(int32 p)
{
    _deltaPriority = p;
    int32 newPriority = _parentPriority + _deltaPriority;
    onPriorityChanged(newPriority);

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setParentPriority(newPriority);
}
//-------------------------------------------------------------------------
void Widget::setParentActive(bool active)
{
    bool old = isActive();
    _isParentActive = active;

    if(isActive() != old)
    {
        onActiveChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentActive(isActive());
    }
}
//-------------------------------------------------------------------------
void Widget::setOwnColor(const Core::Vector4f & color, float changeTime)
{
    if(changeTime == 0.0f)
    {
        _colorChangeDuration = 0.0f;
        _colorChangeElapsed = 0.0f;
        _ownColor = color;
        updateColor();
    }
    else
    {
        _colorChangeDuration = changeTime;
        _colorChangeElapsed = 0.0f;
        _color1 = _ownColor;
        _color2 = color;
    }
}
//-------------------------------------------------------------------------
const Core::Vector4f & Widget::getColor() const
{
    return _color;
}
//-------------------------------------------------------------------------
void Widget::setParentColor(const Core::Vector4f & color)
{
    _parentColor = color;
    updateColor();
}
//-------------------------------------------------------------------------
void Widget::updateColor()
{
    _color = Core::mul(_parentColor, _ownColor);

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->setParentColor(_color);

    onColorChanged();
}
//-------------------------------------------------------------------------
void Widget::onColorChanged()
{
}
//-------------------------------------------------------------------------
void Widget::setActive(bool active)
{
    bool old = isActive();
    _isActive = active;

    if(isActive() != old)
    {
        onActiveChanged();

        for(int32 ii=0; ii < _children.size(); ii++)
            _children[ii]->setParentActive(isActive());
    }
}
//-------------------------------------------------------------------------
bool Widget::isActive() const
{
    return _isActive && _isParentActive;
}
//-------------------------------------------------------------------------
void Widget::addChild(const Ptr<Widget> & pWidget)
{
    pWidget->setParentActive(isActive());
    pWidget->setParentEnabled(isEnabled());
    pWidget->setParentVisible(isVisible());
    pWidget->setParentPosition(_screenPos);
    pWidget->setParentPriority(_parentPriority + _deltaPriority);
    pWidget->setParentColor(_color);
    _children.push_back(pWidget);
}
//-------------------------------------------------------------------------
void Widget::removeChild(const Ptr<Widget> & pWidget)
{
    Core::List<Ptr<Widget> >::iterator iChild = std::find(_children.begin(), _children.end(), pWidget);

    if(iChild != _children.end())
        _children.erase(iChild);
}
//-------------------------------------------------------------------------
void Widget::pack()
{
    Core::Vector2f s(0.0f, 0.0f);

    for(int32 ii=0; ii < _children.size(); ii++)
    {
        const Core::Vector2f & sc = _children[ii]->getPosition() + _children[ii]->getSize();
        s.x = std::max(s.x, sc.x);
        s.y = std::max(s.y, sc.y);
    }

    setSize(s);
}
//-------------------------------------------------------------------------
void Widget::updateKeyNavigation()
{
    if(_pKeyNav != NULL)
    {
        if(_pKeyNav->pKeyFocus == this && !_pKeyNav->focusJustChanged)
        {
            if(_pKeyNav->pCtrl->left() && _pKeyWidgetL != NULL)
            {
                _pKeyNav->pKeyFocus = _pKeyWidgetL;
                
                while(_pKeyNav->pKeyFocus != NULL && !_pKeyNav->pKeyFocus->isEnabled())
                    _pKeyNav->pKeyFocus = _pKeyNav->pKeyFocus->_pKeyWidgetL;

                if(_pKeyNav->pKeyFocus == NULL)
                {
                    _pKeyNav->pKeyFocus = this;
                }
                else
                {
                    _pKeyNav->ignoreMouse = true;
                    _pKeyNav->focusJustChanged = true;
                }
            }
            else if(_pKeyNav->pCtrl->right() && _pKeyWidgetR != NULL)
            {
                _pKeyNav->pKeyFocus = _pKeyWidgetR;
                while(_pKeyNav->pKeyFocus != NULL && !_pKeyNav->pKeyFocus->isEnabled())
                    _pKeyNav->pKeyFocus = _pKeyNav->pKeyFocus->_pKeyWidgetR;

                if(_pKeyNav->pKeyFocus == NULL)
                {
                    _pKeyNav->pKeyFocus = this;
                }
                else
                {
                    _pKeyNav->ignoreMouse = true;
                    _pKeyNav->focusJustChanged = true;
                }
            }
            else if(_pKeyNav->pCtrl->up() && _pKeyWidgetU != NULL)
            {
                _pKeyNav->pKeyFocus = _pKeyWidgetU;

                while(_pKeyNav->pKeyFocus != NULL && !_pKeyNav->pKeyFocus->isEnabled())
                    _pKeyNav->pKeyFocus = _pKeyNav->pKeyFocus->_pKeyWidgetU;

                if(_pKeyNav->pKeyFocus == NULL)
                {
                    _pKeyNav->pKeyFocus = this;
                }
                else
                {
                    _pKeyNav->ignoreMouse = true;
                    _pKeyNav->focusJustChanged = true;
                }
            }
            else if(_pKeyNav->pCtrl->down() && _pKeyWidgetD != NULL)
            {
                _pKeyNav->pKeyFocus = _pKeyWidgetD;

                while(_pKeyNav->pKeyFocus != NULL && !_pKeyNav->pKeyFocus->isEnabled())
                    _pKeyNav->pKeyFocus = _pKeyNav->pKeyFocus->_pKeyWidgetD;

                if(_pKeyNav->pKeyFocus == NULL)
                {
                    _pKeyNav->pKeyFocus = this;
                }
                else
                {
                    _pKeyNav->ignoreMouse = true;
                    _pKeyNav->focusJustChanged = true;
                }
            }
        }
    }
}
//-------------------------------------------------------------------------
void Widget::update(double elapsed)
{
    if(_colorChangeDuration > 0.0f)
    {
        _colorChangeElapsed += float(elapsed);

        if(_colorChangeElapsed >= _colorChangeDuration)
        {
            _ownColor = _color2;
            _colorChangeDuration = 0.0f;
            _colorChangeElapsed = 0.0f;
        }
        else
        {
            _ownColor.setLerp(_color1, _color2, _colorChangeElapsed / _colorChangeDuration);
        }

        updateColor();
    }

    for(int32 ii=0; ii < _children.size(); ii++)
        _children[ii]->update(elapsed);
}
//-------------------------------------------------------------------------
void Widget::notify(EEvent ev)
{
    for(int32 ii=0; ii < _evHandlers.size(); ii++)
        _evHandlers[ii]->onEvent(ev, this);
}
//-------------------------------------------------------------------------
void Widget::onVisibilityChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onEnableChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onActiveChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onPositionChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onPriorityChanged(int32 p)
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onClipRectChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onSizeChanged()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
void Widget::onKill()
{
    // Nothing to do
}
//-------------------------------------------------------------------------
}

