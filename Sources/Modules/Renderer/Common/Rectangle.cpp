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
#include <Renderer/Common/Rectangle.h>

namespace Renderer
{

// ---------------------------------------
//-----------------------------------------------------------------------------
Rectangle::Rectangle(const PictureInfos & pic) :
    _pic(pic),
    _color(Core::Vector4f::ONE),
    _position(0.0f, 0.0f),
    _size(100.0f, 100.0f),
    _priority(0),
    _isVisible(true),
    _isAlive(true)
{
}
//-----------------------------------------------------------------------------
void Rectangle::setPosition(const Core::Vector2f & position)
{
    _position = position;
}
//-----------------------------------------------------------------------------
const Core::Vector2f & Rectangle::getPosition() const
{
    return _position;
}
//-----------------------------------------------------------------------------
void Rectangle::setSize(const Core::Vector2f & size)
{
    _size = size;
}
//-----------------------------------------------------------------------------
const Core::Vector2f & Rectangle::getSize() const
{
    return _size;
}
//-----------------------------------------------------------------------------
void Rectangle::setPriority(int32 p)
{
    _priority = p;
}
//-----------------------------------------------------------------------------
int32 Rectangle::getPriority() const
{
    return _priority;
}
//-----------------------------------------------------------------------------
void Rectangle::setPicture(const PictureInfos & pic)
{
    _pic = pic;
}
//-----------------------------------------------------------------------------
const PictureInfos & Rectangle::getPicture() const
{
    return _pic;
}
//-----------------------------------------------------------------------------
void Rectangle::setColor(const Core::Vector4f & color)
{
    _color = color;
}
//-----------------------------------------------------------------------------
const Core::Vector4f & Rectangle::getColor() const
{
    return _color;
}
//-----------------------------------------------------------------------------
void Rectangle::setVisible(bool flag)
{
    _isVisible = flag;
}
//-----------------------------------------------------------------------------
bool Rectangle::isVisible() const
{
    return _isVisible;
}
//-----------------------------------------------------------------------------
void Rectangle::kill()
{
    _isAlive = false;
}
//-----------------------------------------------------------------------------
bool Rectangle::isAlive() const
{
    return _isAlive;
}
//-----------------------------------------------------------------------------


}