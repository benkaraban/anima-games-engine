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
#ifndef AUDIOSTUB_SOUNDSOURCESTUB_H_
#define AUDIOSTUB_SOUNDSOURCESTUB_H_

#include <Audio/ISoundSource.h>


namespace Audio
{

class LM_API_ADS SoundSourceStub : public Audio::ISoundSource
{
public:
    SoundSourceStub() : isStatic(false) {}
    virtual ~SoundSourceStub() {}

    virtual void setPosition(const Core::Vector3f & position)   {}
    virtual void setDirection(const Core::Vector3f & direction) {}
    virtual void setVelocity(const Core::Vector3f & velocity)   {}

    virtual void setGain(float gain)  {}

    virtual void play()  {}
    virtual void enqueue(const Ptr<Audio::Playable> & pPlayable, bool loop = false)  {}
    virtual bool playbackCompleted()  {return true;}
    virtual void stopCurrentLoop()   {}
    virtual void clear()  {}

    virtual void setStaticSound(bool isStatic) {isStatic= isStatic;}
    virtual bool isStaticSound() const {return isStatic;}

private:
    bool isStatic;
};

}

#endif/*AUDIOSTUB_SOUNDSOURCESTUB_H_*/