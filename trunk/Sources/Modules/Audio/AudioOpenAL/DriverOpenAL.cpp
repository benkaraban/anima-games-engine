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
#include "DriverOpenAL.h"

#include <Core/Logger.h>
#include <Audio/AudioOpenAL/OpenALGlobals.h>

#include <OpenAL/alc.h>

namespace AudioOpenAL
{
//-----------------------------------------------------------------------------
DriverOpenAL::DriverOpenAL()
{
}
//-----------------------------------------------------------------------------
DriverOpenAL::~DriverOpenAL()
{
}
//-----------------------------------------------------------------------------
Ptr<Audio::IDevice> DriverOpenAL::createDevice(int32 adapterId)
{
    Ptr<Audio::IDevice> pIDevice = null;
    if(adapterId == 0)
    {
        ALCcontext * pContext = NULL;
	    ALCdevice * pDevice = NULL;
        INF << L"---------------------------------------------------------";
        pDevice = alcOpenDevice(NULL);
        if (pDevice)
        {
            pContext = alcCreateContext(pDevice, NULL);
            if (pContext)
            {
                INF << L"\"" << alcGetString(pDevice, ALC_DEVICE_SPECIFIER) << L"\"" << L" device opened.";
                alcMakeContextCurrent(pContext);
                pIDevice = Ptr<Audio::IDevice>(new DeviceOpenAL(pDevice, pContext));

                Core::List<Ptr<Audio::ISoundSource> > soundSources;
                bool sourceSuccessfullyCreated = true;
                int32 iSource = 0;
                do
                {
                    Ptr<Audio::ISoundSource> pSoundSource = pIDevice->createSoundSource();
                    if(pSoundSource != null)
                    {
                        soundSources.push_back(pSoundSource);
                        iSource++;
                    }
                    else
                        sourceSuccessfullyCreated = false;

                }while(iSource < OPENAL_MAX_TEST_SOUNDSOURCES && sourceSuccessfullyCreated);

                if(sourceSuccessfullyCreated)
                    INF << L"More than " << OPENAL_MAX_TEST_SOUNDSOURCES << L" sound sources can be created at the same time.";
                else
                    INF << L"A maximum of " << iSource << L" can be created at the same time.";

                soundSources.clear();
            }
            else
            {
                ALenum error = alGetError();

                alcCloseDevice(pDevice);
                ERR << L"Failed to create a context for OpenAL device! " << toString(error);
            }
        }
        else
        {
            ALenum error = alGetError();
            ERR << L"Failed to create OpenAL device! " << toString(error);
        }
        INF << L"---------------------------------------------------------";
    }
    return pIDevice;
}
//-----------------------------------------------------------------------------
}//namespace AudioOpenAL