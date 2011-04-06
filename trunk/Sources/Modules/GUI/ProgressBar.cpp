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
#include <GUI/ProgressBar.h>
#include <Core/Math/Interpolate.h>

namespace GUI
{
//-------------------------------------------------------------------------
ProgressBar::ProgressBar(const Ptr<ResourcePool> & pResPool,
                         const Ptr<Renderer::IHUD> & pHUD) :
    _pResPool(pResPool),
    _borderColor(1.0f, 1.0f, 1.0f, 1.0f),
    _barInteriorPosition(0.0f, 0.0f),
    _orient(PROGRESSBAR_LEFT_TO_RIGHT),
    _progress(1.0f)
{
    _pRectInterior = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pRectBorder = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pRectInterior->setVisible(false);
    _pRectBorder->setVisible(false);
}
//-------------------------------------------------------------------------
void ProgressBar::onVisibilityChanged()
{
    _pRectInterior->setVisible(isVisible());
    _pRectBorder->setVisible(isVisible());
}
//-------------------------------------------------------------------------
void ProgressBar::setBarInterior(const String & pic)
{
    _picInterior = _pResPool->getPicture(pic);
    _barInterior = pic;
    internalUpdateBar();
}
//-------------------------------------------------------------------------
void ProgressBar::setBorderColor(const Core::Vector4f & color)
{
    _borderColor = color;
    onColorChanged();
}
//-------------------------------------------------------------------------
void ProgressBar::setBarBorder(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRectBorder->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRectBorder->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _barBorder = pic;
}
//-------------------------------------------------------------------------
void ProgressBar::setProgress(float p)
{
    _progress = Core::clamp(0.0f, 1.0f, p);
    internalUpdateBar();
}
//-------------------------------------------------------------------------
void ProgressBar::setOrient(EProgressBarOrient orient)
{
    _orient = orient;
    internalUpdateBar();
}
//-------------------------------------------------------------------------
void ProgressBar::internalUpdateBar()
{
    Core::Vector2f deltaPos(0.0f, 0.0f);
    Renderer::PictureInfos p = _picInterior;
    float oriWidth = float(p.width);
    float width = p.width * _progress;
    p.width = int32(width);

    switch(_orient)
    {
    case PROGRESSBAR_LEFT_TO_RIGHT:
        p.u2 = Core::lerp(p.u1, p.u2, _progress);
        break;
    case PROGRESSBAR_RIGHT_TO_LEFT:
        deltaPos.x = oriWidth - width;
        p.u1 = Core::lerp(p.u2, p.u1, _progress);
        break;
    }

    _pRectInterior->setPicture(p);
    _pRectInterior->setSize(Core::Vector2f(width, float(p.height)));
    _pRectInterior->setPosition(_screenPos + deltaPos + _barInteriorPosition);
}
//-------------------------------------------------------------------------
void ProgressBar::pack()
{
    const Renderer::PictureInfos & p1 = _pRectInterior->getPicture();
    const Renderer::PictureInfos & p2 = _pRectBorder->getPicture();

    setSize(Core::Vector2f(float(std::max(p1.width, p2.width)), float(std::max(p1.height, p2.height))));
}
//-------------------------------------------------------------------------
const String & ProgressBar::getBarInterior() const
{
    return _barInterior;
}
//-------------------------------------------------------------------------
void ProgressBar::setBarInteriorPosition(const Core::Vector2f & pos)
{
    _barInteriorPosition = pos;
    internalUpdateBar();
}
//-------------------------------------------------------------------------
const String & ProgressBar::getBarBorder() const
{
    return _barBorder;
}
//-------------------------------------------------------------------------
void ProgressBar::onColorChanged()
{
    _pRectBorder->setColor(Core::mul(_borderColor, _parentColor));
    _pRectInterior->setColor(getColor());
}
//-------------------------------------------------------------------------
void ProgressBar::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void ProgressBar::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void ProgressBar::onPositionChanged()
{
    internalUpdateBar();
    _pRectBorder->setPosition(_screenPos);
}
//-------------------------------------------------------------------------
void ProgressBar::onPriorityChanged(int32 p)
{
    _pRectInterior->setPriority(p + 1);
    _pRectBorder->setPriority(p);
}
//-------------------------------------------------------------------------
void ProgressBar::onSizeChanged()
{
    _pRectBorder->setSize(getSize());
}
//-------------------------------------------------------------------------
void ProgressBar::onKill()
{
    _pRectInterior->kill();
    _pRectBorder->kill();
}
//-------------------------------------------------------------------------
}

