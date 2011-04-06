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

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>

#include <Audio/AudioGlobals.h>
#include <Audio/Wave.h>
#include <Audio/OggVorbis.h>
#include <Audio/AudioOpenAL/DriverOpenAL.h>

void main()
{
    try
    {
        Audio::initVorbisFileDll();

        Core::String workDir(LM_WORKING_DIRECTORY);
        SetCurrentDirectory(Core::String8(workDir).c_str());

        Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
        if(!pVFS->linkDirectory(L"."))
        {
            ERR << L"Unable to initialise VFS\n";
            return;
        }

        AudioOpenAL::DriverOpenAL driver;
        Ptr<Audio::IDevice> pIDevice = driver.createDevice(0);

        /*Ptr<Audio::Wave> pWave(new Audio::Wave());
        Ptr<Core::InputStream> pInputStream = pVFS->readFile(L"/Data/TUData/Sound/Footsteps.wav");
        pWave->read(*pInputStream);

        Ptr<Audio::OggVorbis> pOggVorbis(new Audio::OggVorbis());
        Ptr<Core::InputStream> pInputStream2 = pVFS->readFile(L"/Data/TUData/Sound/stereo.ogg");
        pOggVorbis->read(*pInputStream2);*/

        Ptr<Audio::OggVorbis> pOggVorbis1(new Audio::OggVorbis());
        Ptr<Audio::Playable> pOggVorbis2(new Audio::OggVorbis());
        Ptr<Core::InputStream> pInputStream1 = pVFS->readFile(L"/Data/TUData/Sound/MainTitleLoop1.ogg");
        Ptr<Core::InputStream> pInputStream2 = pVFS->readFile(L"/Data/TUData/Sound/player-shot.ogg");
        pOggVorbis1->read(*pInputStream1);
        pOggVorbis2->read(*pInputStream2);
        
        
        Ptr<Audio::IListener> pIListener = pIDevice->createListener();
        pIListener->setGain(1);

        Ptr<Audio::ISoundSource> pISoundSource = pIDevice->createSoundSource();
        pISoundSource->setGain(1.0f);
        //pISoundSource->enqueue(pOggVorbis1, true);
        pISoundSource->enqueue(pOggVorbis2, true);

        while(!pISoundSource->playbackCompleted())
        {
            pISoundSource->play();
            Sleep(20);
        }
        /*Ptr<Audio::ISoundSource> pISoundSource = pIDevice->createSoundSource();
        pISoundSource->setGain(1.0f);
        pISoundSource->enqueue(pWave, true);
                
        Ptr<Audio::ISoundSource> pISoundSource2 = pIDevice->createSoundSource();
        pISoundSource2->setGain(0.05f);
        pISoundSource2->enqueue(pOggVorbis);

        Core::Vector3f direction;
        pIListener->getDirection(direction);
        direction.z = -1.0;
        pIListener->setDirection(direction);

        while(!pISoundSource->playbackCompleted())
        {
            Sleep(20);
            static float alpha = 0;
            alpha += 0.01f;
            pISoundSource->setPosition(Core::Vector3f(10.0f * cos(alpha), 0.0f, 10.0f * sin(alpha)));

            if(!pISoundSource->playbackCompleted())
                pISoundSource->play();

            if(alpha > 6)
                if(!pISoundSource2->playbackCompleted())
                    pISoundSource2->play();
                else
                    pISoundSource->stopCurrentLoop();
        }*/
        Audio::shutdownVorbisFileDll();
    }
    catch(Core::Exception & e)
    {
        ERR << e;
    }
}


