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
#include "SoundSourceOpenAL.h"

#include <Core/Buffer.h>
#include <Core/Logger.h>

namespace AudioOpenAL
{
//-----------------------------------------------------------------------------
SoundSourceOpenAL::SoundSourceOpenAL()
:   _sourceCreated(false),
    _bufferGenerated(false),
    _isStatic(false)
{
    initialize();
}
//-----------------------------------------------------------------------------
SoundSourceOpenAL::~SoundSourceOpenAL()
{
    shutdown();
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::initialize()
{
    ALenum error = AL_NO_ERROR; 
    alGenSources(1, &_source);

    error = alGetError();
    if(error == AL_NO_ERROR) 
    {
        _sourceCreated = true;
        alGenBuffers(OPENAL_NUMBUFFERS, _buffers);
        error = alGetError();
        if(error == AL_NO_ERROR)
        {
            for(int32 iBuffer=0; iBuffer < OPENAL_NUMBUFFERS; ++iBuffer)
                _unusedBuffers.push(_buffers[iBuffer]);
            _bufferGenerated = true;
        }
    }

    if(error)
    {
        WAR << L"An error occured while trying to create an OpenAL sound source : ( "<<toString(error)<< L" ).";
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::shutdown()
{
    if(successfullyCreated())
    {
        //Stop the Source and clear the Queue
	    alSourceStop(_source);
	    alSourcei(_source, AL_BUFFER, 0);
    }
    if(_bufferGenerated)
    {
        //delete buffers
        alDeleteBuffers(OPENAL_NUMBUFFERS, _buffers);
    }
    if(_sourceCreated)
    {
        //delete Source
        alDeleteSources(1, &_source);
    }

    _tracks.clear();
    _unusedBuffers.clear();
    _sourceCreated = false;
    _bufferGenerated = false;
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setPosition(const Core::Vector3f & position)
{
    if (!_isStatic)
    {
        alSource3f(_source, AL_POSITION, position.x, position.y, position.z);
        
        ALenum error = alGetError();
        if(error != AL_NO_ERROR) 
        { 
            Core::String message;
            message << L"An error occured while trying change the source position ( "<<toString(error)<< L" ). Position : " << Core::toString(position);
            throw Core::Exception(message);
        }
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setDirection(const Core::Vector3f & direction)
{
    alSource3f(_source, AL_DIRECTION, direction.x, direction.y, direction.z);
    
    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the source direction ( "<<toString(error)<< L" ). Direction : " << Core::toString(direction);
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setVelocity(const Core::Vector3f & velocity)
{
    alSource3f(_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change source velocity ( "<<toString(error)<< L" ). Velocity : " << Core::toString(velocity);
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setGain(float gain)
{
    alSourcef(_source, AL_GAIN, gain);

    ALenum error = alGetError();
    if(error != AL_NO_ERROR) 
    { 
        Core::String message;
        message << L"An error occured while trying change the source gain ( "<<toString(error)<< L" ). Gain : " << gain;
        throw Core::Exception(message);
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::play()
{
    if (_tracks.size() == 0)    return;
    //If the source is not playing and the tracks are empty there is nothing to play
    if(playbackCompleted())
        return;

    ALint   state;
    alGetSourcei(_source, AL_SOURCE_STATE, &state);

    if(state == AL_INITIAL)
    {
        beginNextTrack();
        //Play source
        alSourcePlay(_source);
    }
    else
    {
        setNextData();

        // If the Source was stopped and the playback is not complete then it means
        // the Source was starved of audio data, and needs to be restarted.
        if(state == AL_STOPPED)
            alSourcePlay(_source);
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::beginNextTrack()
{
    Ptr<Track> pTrack = _tracks.front();
    pTrack->pPlayable->rewind();
    setNextData();
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setNextData()
{
    //Get number of buffer processed
    int32 nbBufferProcessed = 0;
    alGetSourcei(_source, AL_BUFFERS_PROCESSED, &nbBufferProcessed);

    for(int32 iBuffer = 0; iBuffer < nbBufferProcessed; ++iBuffer)
    {
        ALuint bufferId = 0;
        //Remove the Buffer from the Queue.
        alSourceUnqueueBuffers(_source, 1, &bufferId);
        _unusedBuffers.push(bufferId);
    }

    if(_tracks.size() <= 0)
        return;

    Ptr<Audio::Playable> pPlayable = _tracks.front()->pPlayable;

    bool readAgain = true;

    while(_unusedBuffers.size() > 0 && readAgain)
    {
        uint32 frequency    = pPlayable->getFrequency();
        uint32 nbChannels   = pPlayable->getNbChannels();
        uint32 format = getPlayableFormat(pPlayable);

        uint32 bufferSize   = pPlayable->getBufferSizeFor250ms();
        Core::Buffer buffer(bufferSize);

        int32 readSize = pPlayable->getData(buffer);
        if(readSize > 0)
        {
            ALuint bufferId = _unusedBuffers.front();
            _unusedBuffers.pop();

            //Enqueue alBuffers
            alBufferData(bufferId, format, buffer.getData(), readSize, frequency);
            alSourceQueueBuffers(_source, 1, &bufferId);
        }
        else//Nothing else to read
        {
            if(_tracks.front()->loop)//If loop we rewind the current track
            {
                _tracks.front()->pPlayable->rewind();
            }
            else//else switching to the next track
            {
                _tracks.pop();
                if(_tracks.size() > 0)
                {
                    pPlayable = _tracks.front()->pPlayable;
                    pPlayable->rewind();
                }
                else
                    readAgain = false;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::enqueue(const Ptr<Audio::Playable> & pPlayable, bool loop)
{
    Ptr<Track> pTrack(new Track());
    pTrack->pPlayable = pPlayable;
    pTrack->loop = loop;
    _tracks.push(pTrack);
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::clear()
{
    shutdown();
    initialize();
}
//-----------------------------------------------------------------------------
bool SoundSourceOpenAL::playbackCompleted()
{
    ALint   state;
    alGetSourcei(_source, AL_SOURCE_STATE, &state);
    
    return (state == AL_STOPPED && _tracks.size() == 0);
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::stopCurrentLoop()
{
    if(_tracks.size() > 0)
    {
        _tracks.front()->loop = false;
    }
}
//-----------------------------------------------------------------------------
void SoundSourceOpenAL::setStaticSound(bool isStatic)
{
    _isStatic = isStatic;

    if (_isStatic==true)
    {
        alSource3f(_source, AL_POSITION, 0, 0, 0);
        
        ALenum error = alGetError();
        if(error != AL_NO_ERROR) 
        { 
            Core::String message;
            message << L"An error occured while trying make static sound source (AL_POSITION)";
            throw Core::Exception(message);
        }

        alSourcei(_source, AL_SOURCE_RELATIVE, AL_TRUE);
        error = alGetError();
        if(error != AL_NO_ERROR) 
        { 
            Core::String message;
            message << L"An error occured while trying make static sound source (AL_SOURCE_RELATIVE)";
            throw Core::Exception(message);
        }
    }
    else
    {
        alSourcei(_source, AL_SOURCE_RELATIVE, AL_FALSE);
        ALenum error = alGetError();
        if(error != AL_NO_ERROR) 
        { 
            Core::String message;
            message << L"An error occured while trying make not static sound source (AL_SOURCE_RELATIVE)";
            throw Core::Exception(message);
        }
    }
}
//-----------------------------------------------------------------------------
bool SoundSourceOpenAL::isStaticSound() const
{
    return _isStatic;
}
//-----------------------------------------------------------------------------
}//namespace AudioOpenAL