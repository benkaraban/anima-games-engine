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
#include "ListenerOpenAL.h"

#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#include <Audio/AudioOpenAL/OpenALGlobals.h>

#include <Core/Exception.h>

namespace AudioOpenAL
{
//-----------------------------------------------------------------------------
ListenerOpenAL::ListenerOpenAL()
{
}
//-----------------------------------------------------------------------------
ListenerOpenAL::~ListenerOpenAL()
{
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::getPosition(Core::Vector3f & position)
{
    alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z);
    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying GET the Listener position ( "<<toString(error)<< L" ).";
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::getDirection(Core::Vector3f & direction)
{
    float listenerOri[6];
    alGetListenerfv(AL_ORIENTATION, listenerOri);

    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying GET the Listener direction ( "<<toString(error)<< L" ).";
        throw Core::Exception(message);
    }

    direction.x = listenerOri[0];
    direction.y = listenerOri[1];
    direction.z = listenerOri[2];
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::setPosition(const Core::Vector3f & position)
{
    float listenerPos[] = {position.x, position.y, position.z};

    alListenerfv(AL_POSITION,listenerPos);
    
    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the Listener position ( "<<toString(error)<< L" ). Position : " << Core::toString(position);
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::setDirection(const Core::Vector3f & direction)
{
    float listenerOri[] = {direction.x, direction.y, direction.z, 0.0f, 1.0f, 0.0f};

    alListenerfv(AL_ORIENTATION,listenerOri);
    
    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the Listener direction ( "<<toString(error)<< L" ). Direction : " << Core::toString(direction);
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::setVelocity(const Core::Vector3f & velocity)
{
    float listenerVel[] = {velocity.x, velocity.y, velocity.z};
    
    alListenerfv(AL_VELOCITY,listenerVel); 
    
    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the Listener position ( "<<toString(error)<< L" ). Velocity : " << Core::toString(velocity);
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void ListenerOpenAL::setGain(float gain)
{
    alListenerf(AL_GAIN, gain);

    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the master gain ( "<<toString(error)<< L" ). Gain : " << gain;
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
}//namespace AudioOpenAL