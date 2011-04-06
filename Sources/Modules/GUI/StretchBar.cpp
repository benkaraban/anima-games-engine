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
#include <GUI/StretchBar.h>

namespace GUI
{
//-------------------------------------------------------------------------
StretchBar::StretchBar(const Ptr<ResourcePool> & pResPool,
                       const Ptr<Renderer::IHUD> & pHUD,
                       const Ptr<Window::InputManager> & pInput) :
    _pResPool(pResPool),
    _pInput(pInput),
    _standardSize(30.0f, 30.0f),
    _highlightSize(60.0f, 60.0f)
{
}
//-------------------------------------------------------------------------
void StretchBar::setStandardSize(const Core::Vector2f & size)
{
    _standardSize = size;
    _size.x = (_children.size() + 1.0f) * _standardSize.x;
    _size.y = _standardSize.y;
    updateDisplay();
}
//-------------------------------------------------------------------------
void StretchBar::setHighlightSize(const Core::Vector2f & size)
{
    _highlightSize = size;
    updateDisplay();
}
//-------------------------------------------------------------------------
void StretchBar::updateDisplay()
{
}
//-------------------------------------------------------------------------
void StretchBar::onColorChanged()
{
}
//-------------------------------------------------------------------------
void StretchBar::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void StretchBar::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void StretchBar::onPositionChanged()
{
}
//-------------------------------------------------------------------------
void StretchBar::onPriorityChanged(int32 p)
{
}
//-------------------------------------------------------------------------
void StretchBar::onSizeChanged()
{
}
//-------------------------------------------------------------------------
void StretchBar::onKill()
{
}
//-------------------------------------------------------------------------
void StretchBar::update(double elapsed)
{
    Widget::update(elapsed);

    _size.x = (_children.size() + 1.0f) * _standardSize.x;
    _size.y = _standardSize.y;

    if(!isActive())
        return;

    int32 childCount = _children.size();

    if(_slots.size() != childCount)
    {
        _slots.resize(childCount);
        Core::Vector2f position = Core::Vector2f(0.0f, 0.0f);

        for(int32 ii=0; ii < childCount; ii++)
        {
            position.x += _standardSize.x;
            _slots[ii].defaultPos = position;
            _slots[ii].centerPos = position;
            _slots[ii].oldSize = _standardSize;
        }
    }

    Core::Vector2f mouse = _pInput->getMousePos() - _screenPos;

    for(int32 ii=0; ii < childCount; ii++)
       _slots[ii].mouseDist = (mouse - _slots[ii].centerPos).getLengthSafe();

    float totalWidth = 0.0f;

    for(int32 ii=0; ii < childCount; ii++)
    {
        float a = std::min(1.0f, _slots[ii].mouseDist / (2.0f * _standardSize.getLengthSafe()));
        _slots[ii].newSize.setLerp(_highlightSize, _standardSize, a);
        totalWidth += _slots[ii].newSize.x;
    }

    for(int32 ii=0; ii < childCount; ii++)
        _slots[ii].force = Core::Vector2f::ZERO;

    for(int32 ii=0; ii < childCount; ii++)
    {
        float forceIntensity = 0.5f * (_slots[ii].newSize.x - _standardSize.x);

        for(int32 jj=(ii + 1); jj < childCount; jj++)
            _slots[jj].force.x += forceIntensity;

        for(int32 jj=(ii - 1); jj >= 0; jj--)
            _slots[jj].force.x -= forceIntensity;
    }

    for(int32 ii=0; ii < childCount; ii++)
    {
        _slots[ii].centerPos = _slots[ii].defaultPos + Core::Vector2f(_slots[ii].force);
    }

    for(int32 ii=0; ii < childCount; ii++)
    {
        _children[ii]->setPosition(_slots[ii].centerPos - 0.5f * _slots[ii].newSize);
        _children[ii]->setSize(_slots[ii].newSize);
        _children[ii]->setOwnColor(Core::Vector4f::ONE);
    }
}
//-------------------------------------------------------------------------
}

