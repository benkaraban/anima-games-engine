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
#ifndef AUDIOOPENAL_SOUNDSOURCEOPENAL_H_
#define AUDIOOPENAL_SOUNDSOURCEOPENAL_H_

#include <Audio/ISoundSource.h>
#include <Audio/AudioOpenAL/OpenALGlobals.h>

#include <Core/Queue.h>

#include <OpenAL/al.h>

namespace AudioOpenAL
{

struct Track
{
    Ptr<Audio::Playable> pPlayable;
    bool loop;
};

class LM_API_OAL SoundSourceOpenAL : public Audio::ISoundSource
{
public:
    SoundSourceOpenAL();
    virtual ~SoundSourceOpenAL();

    bool successfullyCreated() const { return (_sourceCreated && _bufferGenerated); }

    virtual void setPosition(const Core::Vector3f & position);
    virtual void setDirection(const Core::Vector3f & direction);
    virtual void setVelocity(const Core::Vector3f & velocity);

    virtual void setGain(float gain);

    virtual void play();
    virtual void enqueue(const Ptr<Audio::Playable> & pPlayable, bool loop);
    virtual bool playbackCompleted();
    virtual void stopCurrentLoop();
    virtual void clear();

    virtual void setStaticSound(bool isStatic);
    virtual bool isStaticSound() const;

protected:
    void beginNextTrack();
    void setNextData();

private:
    void initialize();
    void shutdown();

protected:
    bool    _sourceCreated;
    bool    _bufferGenerated;
    bool    _isStatic;
    ALuint  _source;
    ALuint	_buffers[OPENAL_NUMBUFFERS];

    Core::Queue<ALuint>  _unusedBuffers;
    Core::Queue<Ptr<Track> > _tracks;
};

}//namespace AudioOpenAL

#endif/*AUDIOOPENAL_SOUNDSOURCEOPENAL_H_*/