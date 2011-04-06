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
#include <GUI/Picture.h>

namespace GUI
{
//-------------------------------------------------------------------------
Picture::Picture(const Ptr<ResourcePool> & pResPool,
                 const Ptr<Renderer::IHUD> & pHUD) :
    _pResPool(pResPool),
    _picColor(Core::Vector4f::ONE)
{
    _pRect = pHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pRect->setVisible(false);
}
//-------------------------------------------------------------------------
void Picture::onVisibilityChanged()
{
    _pRect->setVisible(isVisible());
}
//-------------------------------------------------------------------------
void Picture::setPicture(const String & pic)
{
    if(pic == String::EMPTY)
    {
        _pRect->setPicture(Renderer::PictureInfos::PLAIN_COLOR);
    }
    else
    {
        Renderer::PictureInfos p = _pResPool->getPicture(pic);
        _pRect->setPicture(p);
        setSize(Core::Vector2f(float(p.width), float(p.height)));
    }

    _picName = pic;
}
//-------------------------------------------------------------------------
void Picture::pack()
{
    const Renderer::PictureInfos & p = _pRect->getPicture();
    setSize(Core::Vector2f(float(p.width), float(p.height)));
}
//-------------------------------------------------------------------------
const String & Picture::getPicture() const
{
    return _picName;
}
//-------------------------------------------------------------------------
void Picture::onColorChanged()
{
    _pRect->setColor(Core::mul(getColor(), _picColor));
}
//-------------------------------------------------------------------------
void Picture::onEnableChanged()
{
}
//-------------------------------------------------------------------------
void Picture::onActiveChanged()
{
}
//-------------------------------------------------------------------------
void Picture::onPositionChanged()
{
    _pRect->setPosition(_screenPos);
}
//-------------------------------------------------------------------------
void Picture::onPriorityChanged(int32 p)
{
    _pRect->setPriority(p);
}
//-------------------------------------------------------------------------
void Picture::onSizeChanged()
{
    _pRect->setSize(getSize());
}
//-------------------------------------------------------------------------
void Picture::onKill()
{
    _pRect->kill();
}
//-------------------------------------------------------------------------
}

