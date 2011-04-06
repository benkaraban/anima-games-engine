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
#ifndef HOOCLIENT_IN_GAME_MUSIC_H_
#define HOOCLIENT_IN_GAME_MUSIC_H_

#include <Core/Standard.h>
#include <Universe/World.h>
#include <Universe/NodeSoundSource.h>


LM_ENUM_3 (EInGameMusicState,
            IN_GAME_MUSIC_BEGIN,    //A puis boucle sur B
            IN_GAME_MUSIC_FINAL,    //boucle sur C
            IN_GAME_MUSIC_OUTRO);

class InGameMusic : public Core::Object
{
public:
    InGameMusic(const Ptr<Universe::World> & pWorld,
        const String& fileNameMusicA, const String& fileNameMusicB, const String& fileNameMusicC, const String& fileNameMusicOutro);

    void start();
    void engageFinalState();
    void engageOutro();
    

    EInGameMusicState getMusicState() {return _state;}

    void setTransitionTime(float time)  {_transitionTime = time;}
    float getTransitionTime()           {return _transitionTime;}

    void setGain(float gain);

private:
    Ptr<Universe::NodeSoundSource>  _pNodeSoundMusic;
    Ptr<Universe::NodeSoundSource>  _pNodeSoundOutro;

    EInGameMusicState               _state;
    float                           _transitionTime;   //duree de la transition entre la musique et l'outro
};

#endif
