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
#include "OpenALGlobals.h"

#include <Audio/Wave.h>

namespace AudioOpenAL
{

Core::String toString(ALenum error)
{
    Core::String errorString;
    errorString << L"Error unknown : (" << int32(error) << L").";

    switch(error)
    {
    case AL_NO_ERROR:
        errorString = L"No error.";
        break;
    case AL_INVALID_NAME:
        errorString = L"A bad name (ID) was passed to an OpenAL function .";
        break;
    case AL_INVALID_ENUM:
        errorString = L"An invalid enum value was passed to an OpenAL function.";
        break;
    case AL_INVALID_VALUE:
        errorString = L"An invalid value was passed to an OpenAL function.";
        break;
    case AL_INVALID_OPERATION:
        errorString = L"The requested operation is not valid.";
        break;
    case AL_OUT_OF_MEMORY:
        errorString = L"The requested operation resulted in OpenAL running out of memory.";
        break;
    }

    return errorString;
}

uint32 getWaveFormat(const Ptr<Audio::Playable> & pPlayable)
{
    uint32 format = 0;
    Ptr<Audio::Wave> pWave = LM_DEBUG_PTR_CAST<Audio::Wave>(pPlayable);
    
    if (pWave->getNbChannels() == 1)
	{
		switch (pWave->getBitsPerSample())
		{
		case 4:
			format = alGetEnumValue("AL_FORMAT_MONO_IMA4");
			break;
		case 8:
			format = alGetEnumValue("AL_FORMAT_MONO8");
			break;
		case 16:
			format = alGetEnumValue("AL_FORMAT_MONO16");
			break;
		}
	}
	else if (pWave->getNbChannels() == 2)
	{
		switch (pWave->getBitsPerSample())
		{
		case 4:
			format = alGetEnumValue("AL_FORMAT_STEREO_IMA4");
			break;
		case 8:
			format = alGetEnumValue("AL_FORMAT_STEREO8");
			break;
		case 16:
			format = alGetEnumValue("AL_FORMAT_STEREO16");
			break;
		}
	}
	else if ((pWave->getNbChannels() == 4) && (pWave->getBitsPerSample() == 16))
		format = alGetEnumValue("AL_FORMAT_QUAD16");

    return format;
}

uint32 getOggVorbisFormat(const Ptr<Audio::Playable> & pPlayable)
{
    uint32 format = 0;
    if (pPlayable->getNbChannels() == 1)
    {
        format = alGetEnumValue("AL_FORMAT_MONO16");
    }
    else if (pPlayable->getNbChannels() == 2)
    {
        format = alGetEnumValue("AL_FORMAT_STEREO16");
    }
    else if (pPlayable->getNbChannels() == 4)
    {
        format = alGetEnumValue("AL_FORMAT_QUAD16");
    }
    else if (pPlayable->getNbChannels() == 6)
    {
        format = alGetEnumValue("AL_FORMAT_51CHN16");
    }
    return format;
}

uint32 getPlayableFormat(const Ptr<Audio::Playable> & pPlayable)
{
    uint32 format = 0;
    if(pPlayable->getType() == Audio::WAV_TYPE)
        format = getWaveFormat(pPlayable);
    else if(pPlayable->getType() == Audio::OGGVORBIS_TYPE)
    {
        format = getOggVorbisFormat(pPlayable);
    }
    else
    {
        Core::String message;
        message << L"Unable to determine format for Playable. Unknown type : "<< pPlayable->getType();
        throw Core::Exception(message);
    }

    return format;
}

}//namespace AudioOpenAL
