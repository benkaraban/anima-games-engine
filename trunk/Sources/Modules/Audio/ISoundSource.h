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
#ifndef AUDIO_ISOUNDSOURCE_H_
#define AUDIO_ISOUNDSOURCE_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

#include <Audio/Playable.h>

namespace Audio
{

class LM_API_AUD ISoundSource
{
public:
    virtual ~ISoundSource();

    virtual void setPosition(const Core::Vector3f & position) = 0;
    virtual void setDirection(const Core::Vector3f & direction) = 0;
    virtual void setVelocity(const Core::Vector3f & velocity) = 0;

    virtual void setGain(float gain) = 0;

    virtual void play() = 0;
    virtual void enqueue(const Ptr<Playable> & pPlayable, bool loop = false) = 0;
    virtual bool playbackCompleted() = 0;
    virtual void stopCurrentLoop() = 0;
    virtual void clear() = 0;

    virtual void setStaticSound(bool isStatic) = 0;
    virtual bool isStaticSound() const = 0;

protected:
    ISoundSource();
};

}//namespace Audio

#endif/*AUDIO_ISOUNDSOURCE_H_*/