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
#ifndef AUDIOSTUB_LISTENERSTUB_H_
#define AUDIOSTUB_LISTENERSTUB_H_

#include <Audio/IListener.h>

namespace Audio
{

class LM_API_ADS ListenerStub : public Audio::IListener
{
public:
    ListenerStub()                                              {}
    virtual ~ListenerStub()                                     {}

    virtual void setPosition(const Core::Vector3f & position)   {_position=position;}
    virtual void setDirection(const Core::Vector3f & direction) {_direction=direction;}
    virtual void setVelocity(const Core::Vector3f & velocity)   {_velocity=velocity;}

    virtual void getPosition(Core::Vector3f & position)         {position = _position;}
    virtual void getDirection(Core::Vector3f & direction)       {direction = _direction;}

    virtual void setGain(float gain){}

private:
    Core::Vector3f _position;
    Core::Vector3f _direction;
    Core::Vector3f _velocity;
};

}

#endif/*AUDIOSTUB_LISTENERSTUB_H_*/