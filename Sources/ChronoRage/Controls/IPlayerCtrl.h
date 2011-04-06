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
#ifndef CHRONORAGE_IPLAYERCTRL_H_
#define CHRONORAGE_IPLAYERCTRL_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

namespace ChronoRage
{
class IPlayerCtrl
{
public:
    virtual ~IPlayerCtrl() {};

    virtual void setViewDimensions(int32 width, int32 height) = 0;
    virtual void setPlayerPos(const Core::Vector3f & pos) = 0;

    virtual void shakeDie() = 0;

    virtual bool showCursor() const = 0;

    virtual void setDefaultRadialSpeed() = 0;
    virtual void setRadialSpeed(float radialSpeed){ _radialSpeed = radialSpeed; }
    virtual float getRadialSpeed() const { return _radialSpeed; }

    virtual Core::Vector3f getMoveDir() const = 0;
    virtual Core::Vector3f getShootDir() const = 0;
    virtual void setShootDir(const Core::Vector3f & dir) = 0;

    virtual bool fire() const = 0;
    virtual bool timeWarp() const = 0;

    virtual void update(double elapsed) = 0;

protected:
    float   _radialSpeed;
};
}

#endif
