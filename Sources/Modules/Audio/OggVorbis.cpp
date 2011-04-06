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
#include "OggVorbis.h"

#include <Audio/AudioGlobals.h>

namespace Audio
{

extern LPOVCLEAR           ov_clear;
extern LPOVREAD            ov_read;
extern LPOVPCMTOTAL        ov_pcm_total;
extern LPOVINFO            ov_info;
extern LPOVCOMMENT         ov_comment;
extern LPOVOPENCALLBACKS   ov_open_callbacks;
extern LPOVTIMESEEK         ov_time_seek;


size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void * datasource);
int ov_seek_func(void *datasource, ogg_int64_t offset, int whence);
long ov_tell_func(void *datasource);

OggVorbis::OggVorbis()
:   _oggVorbisData(Core::Buffer(0)),
    _offset(0)
{
    _callbacks.close_func = NULL;
    _callbacks.read_func = ov_read_func;
    _callbacks.seek_func = ov_seek_func;
    _callbacks.tell_func = ov_tell_func;
}
//-----------------------------------------------------------------------------
OggVorbis::~OggVorbis()
{
}
//-----------------------------------------------------------------------------
void OggVorbis::read(Core::InputStream & input)
{
    _oggVorbisData.read(input);

    if(ov_open_callbacks(this, &_oggVorbisFile, NULL, 0, _callbacks) == 0)
    {
        // Get some information about the file (Channels, Format, and Frequency)
		vorbis_info * pVorbisInfo = ov_info(&_oggVorbisFile, -1);
        _frequency = pVorbisInfo->rate;
        _nbChannels = pVorbisInfo->channels;
    }
    else 
    {
        throw Core::Exception(L"An error occured while trying to read an Ogg Vorbis stream.");
    }
}
//-----------------------------------------------------------------------------
void OggVorbis::write(Core::OutputStream & output) const
{
    _oggVorbisData.write(output);
}
//-----------------------------------------------------------------------------
uint32 OggVorbis::getFrequency() const
{
    return _frequency;
}
//-----------------------------------------------------------------------------
uint32 OggVorbis::getNbChannels() const
{
    return _nbChannels;
}
//-----------------------------------------------------------------------------
uint32 OggVorbis::getBufferSizeFor250ms() const
{
    uint32 bufferSize = 0;
    if (_nbChannels == 1)
	{
		// Set BufferSize to 250ms (Frequency * 2 (16bit) divided by 4 (quarter of a second))
		bufferSize = _frequency >> 1;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		bufferSize -= (bufferSize % 2);
	}
	else if (_nbChannels == 2)
	{
		// Set BufferSize to 250ms (Frequency * 4 (16bit stereo) divided by 4 (quarter of a second))
		bufferSize = _frequency;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		bufferSize -= (bufferSize % 4);
	}
	else if (_nbChannels == 4)
	{
		// Set BufferSize to 250ms (Frequency * 8 (16bit 4-channel) divided by 4 (quarter of a second))
		bufferSize = _frequency * 2;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		bufferSize -= (bufferSize % 8);
	}
	else if (_nbChannels == 6)
	{
        // Set BufferSize to 250ms (Frequency * 12 (16bit 6-channel) divided by 4 (quarter of a second))
		bufferSize = _frequency * 3;
		// IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
		bufferSize -= (bufferSize % 12);
	}

    return bufferSize;
}
//-----------------------------------------------------------------------------
void OggVorbis::rewind()
{
    ov_time_seek(&_oggVorbisFile, 0);
}
//-----------------------------------------------------------------------------
int32 OggVorbis::getData(Core::Buffer & buffer)
{
    int32 current_section = 0;
	int32 decodedSize;
	int32 bytesDone = 0;

    bool decodeAgain = true;
	while(decodeAgain)
	{
        decodedSize = ov_read(&_oggVorbisFile, (char *)buffer.getData() + bytesDone, buffer.size() - bytesDone, 0, 2, 1, &current_section);
                
		if (decodedSize > 0)
		{
			bytesDone += decodedSize;

			if (bytesDone >= buffer.size())
				decodeAgain = false;
		}
		else
		{
			decodeAgain = false;
		}
	}

	// Mono, Stereo and 4-Channel files decode into the same channel order as WAVEFORMATEXTENSIBLE,
	// however 6-Channels files need to be re-ordered
	if (_nbChannels == 6)
	{
        int32 iSample;
	    int16 * pSamples;
        pSamples = (int16*)buffer.getData();
		for (iSample = 0; iSample < (buffer.size()>>1); iSample+=6)
		{
			// WAVEFORMATEXTENSIBLE Order : FL, FR, FC, LFE, RL, RR
			// OggVorbis Order            : FL, FC, FR,  RL, RR, LFE
			swap(pSamples[iSample+1], pSamples[iSample+2]);
			swap(pSamples[iSample+3], pSamples[iSample+5]);
			swap(pSamples[iSample+4], pSamples[iSample+5]);
		}
	}

	return bytesDone;
}
//-----------------------------------------------------------------------------
int32 OggVorbis::oggRead(void * ptr, int32 size)
{
    int32 readSize = 0;
    if(_offset < _oggVorbisData.size())
    {
        byte * data = _oggVorbisData.getData();
        data += _offset;

        int32 remainingBytesCount = _oggVorbisData.size() - _offset; 
        readSize = std::min(remainingBytesCount, size);

        memcpy(ptr, data, readSize);
        _offset += readSize;
    }

    return readSize;
}
//-----------------------------------------------------------------------------
int32 OggVorbis::oggSeek(long offset, int32 origin)
{
    int32 oldPosition = _offset;
    switch(origin)
    {
    case SEEK_SET: _offset = offset; break;
    case SEEK_CUR: _offset += offset; break;
    case SEEK_END: _offset = (_oggVorbisData.size() - 1) - offset; break;
    }
    return oldPosition;
}
//-----------------------------------------------------------------------------
int32 OggVorbis::oggTell() const
{
    return _offset;
}
//-----------------------------------------------------------------------------
void OggVorbis::swap(int16 & s1, int16 & s2)
{
	int16 sTemp = s1;
	s1 = s2;
	s2 = sTemp;
}
//-----------------------------------------------------------------------------
//      CALLBACKS OGGVORBIS
//-----------------------------------------------------------------------------
size_t ov_read_func(void *ptr, size_t size, size_t nmemb, void * datasource)
{
    OggVorbis * pOggVorbis = reinterpret_cast<OggVorbis *>(datasource);
    return pOggVorbis->oggRead(ptr, size*nmemb);
}

int ov_seek_func(void *datasource, ogg_int64_t offset, int whence)
{
    OggVorbis * pOggVorbis = reinterpret_cast<OggVorbis *>(datasource);
    return pOggVorbis->oggSeek((int32)offset, whence);
}

long ov_tell_func(void *datasource)
{
    OggVorbis * pOggVorbis = reinterpret_cast<OggVorbis *>(datasource);
    return pOggVorbis->oggTell();
}

}