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
#ifndef RENDERER_IRECTANGLE_H_
#define RENDERER_IRECTANGLE_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

#include <Renderer/IPictureBank.h>

namespace Renderer
{
class LM_API_RDR IRectangle
{
public:
    virtual ~IRectangle() {};

    virtual void setPosition(const Core::Vector2f & position) = 0;
    virtual const Core::Vector2f & getPosition() const = 0;

    virtual void setSize(const Core::Vector2f & size) = 0;
    virtual const Core::Vector2f & getSize() const = 0;

    virtual void setPicture(const PictureInfos & pic) = 0;
    virtual const PictureInfos & getPicture() const = 0;

    virtual void setPriority(int32 p) = 0;
    virtual int32 getPriority() const = 0;

    virtual void setColor(const Core::Vector4f & color) = 0;
    virtual const Core::Vector4f & getColor() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;

};
}

#endif
