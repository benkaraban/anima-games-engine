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
#ifndef CHRONORAGE_SOUNDLIBRARY_H_
#define CHRONORAGE_SOUNDLIBRARY_H_

#include <Core/Standard.h>
#include <Universe/NodeSoundSource.h>

namespace ChronoRage
{

struct AppContext;

LM_ENUM_17(ESoundCode,  SOUND_BUTTON_BLIP,
                        SOUND_PLAYER_SHOT,
                        SOUND_PLAYER_SHOT_IMPACT,
                        SOUND_PLAYER_SHOT_LASER,
                        SOUND_PLAYER_SHOT_LASER_LOADING,
                        SOUND_PLAYER_TIME_WARP,
                        SOUND_PLAYER_TIME_WARP_INCREASE,
                        SOUND_PLAYER_POWER_UP,
                        SOUND_PLAYER_POWER_DOWN,
                        SOUND_PLAYER_DESTRUCTION,
                        SOUND_FOE_DESTRUCTION,
                        SOUND_FOE_SHOT_BULLET,
                        SOUND_FOE_SHOT_HORNET,
                        SOUND_FOE_SHOT_LASER,
                        SOUND_FOE_SHOT_LASER_LOADING,
                        SOUND_CHRONO_BLIP,
                        SOUND_ACHIEVEMENT);

class LM_API_CHR SoundLibrary : public Core::Object
{
public:
    SoundLibrary(AppContext & appContext);
    virtual ~SoundLibrary();

    void update(double elapsed);

    Ptr<Universe::NodeSoundSource> playSound(ESoundCode soundCode, bool loop);
    void playMenuMusic();
    void playLevelMusic(int32 level);
    void playSurvivalMusic();
    void playDemoMusic();

    float getMusicGain();

    Core::String getSound(ESoundCode soundCode);
    float getDefaultGain(ESoundCode soundCode);
protected:
    Core::String getMenuMusic();
    Core::String getLevelMusic(int32 level);
    Core::String getSurvivalMusic();

protected:
    AppContext & _appContext;
    float soundDefaultCoolDown[ESoundCode_COUNT];
    float soundCoolDown[ESoundCode_COUNT];
    float defaultSoundGain[ESoundCode_COUNT];
};

}//namespace ChronoRage

#endif/*CHRONORAGE_SOUNDLIBRARY_H_*/