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
#ifndef AUDIO_OGG_VORBIS_H_
#define AUDIO_OGG_VORBIS_H_

#include <Core/IO/Streams.h>
#include <Core/Buffer.h>

#include <Audio/Playable.h>

#include <Vorbis/vorbisfile.h>

namespace Audio
{

class LM_API_AUD OggVorbis : public Core::Object, public Audio::Playable
{
public:
    OggVorbis();
    virtual ~OggVorbis();

    //Serializable
    virtual void read(Core::InputStream & input);
    virtual void write(Core::OutputStream & output) const;

    //Playable
    inline virtual EPlayableType getType() const { return Audio::OGGVORBIS_TYPE; }
    virtual uint32 getFrequency() const;
    virtual uint32 getNbChannels() const;
    virtual uint32 getBufferSizeFor250ms() const;

    virtual void rewind();
    virtual int32 getData(Core::Buffer & buffer);

    //Ogg Vorbis
    int32 oggRead(void * ptr, int32 size);
    int32 oggSeek(long offset, int32 origin);
    int32 oggTell() const;

private:
    void swap(int16 & s1, int16 & s2);

private:
    Core::Buffer    _oggVorbisData;
    int32           _offset;

    ov_callbacks	_callbacks;
	OggVorbis_File	_oggVorbisFile;

    uint32          _frequency;
    uint32          _nbChannels;

};

}//namespace Audio

#endif/*AUDIO_OGG_VORBIS_H_*/