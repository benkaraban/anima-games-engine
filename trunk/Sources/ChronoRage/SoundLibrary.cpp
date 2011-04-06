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
#include "SoundLibrary.h"

#include <Modes/IGameState.h>

#include <Core/Math/Random.h>

namespace ChronoRage
{

const float MUSIC_DEFAULT_GAIN = 0.4f;

SoundLibrary::SoundLibrary(AppContext & appContext)
:   _appContext(appContext)
{
    for(int32 ii=0; ii<ESoundCode_COUNT; ++ii)
    {
        soundCoolDown[ii] = 0.0f;
    }

    defaultSoundGain[SOUND_BUTTON_BLIP] = 0.05f;
    defaultSoundGain[SOUND_PLAYER_SHOT] = 0.3f;
    defaultSoundGain[SOUND_PLAYER_SHOT_IMPACT] = 0.2f;
    defaultSoundGain[SOUND_PLAYER_SHOT_LASER] = 0.2f;
    defaultSoundGain[SOUND_PLAYER_SHOT_LASER_LOADING] = 0.3f;
    defaultSoundGain[SOUND_PLAYER_TIME_WARP] = 0.8f;
    defaultSoundGain[SOUND_PLAYER_TIME_WARP_INCREASE] = 0.2f;
    defaultSoundGain[SOUND_PLAYER_POWER_UP] = 0.3f;
    defaultSoundGain[SOUND_PLAYER_POWER_DOWN] = 1.0f;
    defaultSoundGain[SOUND_PLAYER_DESTRUCTION] = 1.0f;
    defaultSoundGain[SOUND_FOE_DESTRUCTION] = 0.2f;
    defaultSoundGain[SOUND_FOE_SHOT_BULLET] = 0.35f;
    defaultSoundGain[SOUND_FOE_SHOT_HORNET] = 0.5f;
    defaultSoundGain[SOUND_FOE_SHOT_LASER] = 0.3f;
    defaultSoundGain[SOUND_FOE_SHOT_LASER_LOADING] = 0.5f;
    defaultSoundGain[SOUND_CHRONO_BLIP] = 0.6f;
    defaultSoundGain[SOUND_ACHIEVEMENT] = 1.0f;

    
    soundDefaultCoolDown[SOUND_BUTTON_BLIP] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_SHOT] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_SHOT_IMPACT] = 0.10f;
    soundDefaultCoolDown[SOUND_PLAYER_SHOT_LASER] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_SHOT_LASER_LOADING] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_TIME_WARP] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_TIME_WARP_INCREASE] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_POWER_UP] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_POWER_DOWN] = 0.07f;
    soundDefaultCoolDown[SOUND_PLAYER_DESTRUCTION] = 0.07f;
    soundDefaultCoolDown[SOUND_FOE_DESTRUCTION] = 0.15f;
    soundDefaultCoolDown[SOUND_FOE_SHOT_BULLET] = 0.07f;
    soundDefaultCoolDown[SOUND_FOE_SHOT_HORNET] = 0.07f;
    soundDefaultCoolDown[SOUND_FOE_SHOT_LASER] = 0.07f;
    soundDefaultCoolDown[SOUND_FOE_SHOT_LASER_LOADING] = 0.07f;
    soundDefaultCoolDown[SOUND_CHRONO_BLIP] = 0.07f;
    soundDefaultCoolDown[SOUND_ACHIEVEMENT] = 0.5;

}

SoundLibrary::~SoundLibrary()
{}

void SoundLibrary::update(double elapsed)
{
    for(int32 ii=0; ii<ESoundCode_COUNT; ++ii)
    {
        soundCoolDown[ii] = std::max(0.0f, soundCoolDown[ii] - float(elapsed));
    }
}

Ptr<Universe::NodeSoundSource> SoundLibrary::playSound(ESoundCode soundCode, bool loop)
{
    Ptr<Universe::NodeSoundSource> pSoundSource = null;
    if( _appContext.pSoundWorld != null 
        && _appContext.pGameSettings->getSoundVolume() >= 0.01f
        && soundCoolDown[soundCode] <= 0.0f)
    {
        String soundName = getSound(soundCode);
        //Attention on peut avoir envie qu'un soundCode ne fasse pas de son.
        if(soundName != String::EMPTY)
        {
            pSoundSource = _appContext.pSoundWorld->createSoundSource();
            if(pSoundSource != null)
            {
                pSoundSource->enqueue(soundName, loop);
                pSoundSource->killAtEnd(true);
                float gain = defaultSoundGain[soundCode] * _appContext.pGameSettings->getSoundVolume();
                pSoundSource->setGain(gain);
                soundCoolDown[soundCode] = soundDefaultCoolDown[soundCode];
            }
        }
    }

    return pSoundSource;
}

void SoundLibrary::playMenuMusic()
{
    if(_appContext.pGameMusic != null)
    {
        _appContext.pGameMusic->killAtEnd(true);
        _appContext.pGameMusic->fadeOut(1.0f);
    }

    if(_appContext.pMenuMusic != null && _appContext.pMenuMusic->isAlive())
    {
        _appContext.pMenuMusic->kill();
        _appContext.pMenuMusic = null;
    }

    _appContext.pMenuMusic = _appContext.pSoundWorld->createSoundSource();
    String musicName = getMenuMusic();
    if(_appContext.pMenuMusic != null && musicName != String::EMPTY)
    {
        _appContext.pMenuMusic->fadeIn(3.0f);
        _appContext.pMenuMusic->enqueue(musicName, true);
        _appContext.pMenuMusic->setGain(MUSIC_DEFAULT_GAIN * _appContext.pGameSettings->getMusicVolume());
    }
}

void SoundLibrary::playLevelMusic(int32 level)
{
    if(_appContext.pGameSettings->getMusicVolume() >= 0.01f)
    {
        if( _appContext.pGameMusic != null && _appContext.pGameMusic->isAlive())
        {
            _appContext.pGameMusic->kill();
            _appContext.pGameMusic = null;
        }

        _appContext.pGameMusic = _appContext.pSoundWorld->createSoundSource();
        String musicName = getLevelMusic(level-1);
        if(_appContext.pGameMusic != null && musicName != String::EMPTY)
        {
            _appContext.pGameMusic->enqueue(musicName, true);
            _appContext.pGameMusic->setGain(MUSIC_DEFAULT_GAIN * _appContext.pGameSettings->getMusicVolume());
        }
    }
}

void SoundLibrary::playSurvivalMusic()
{
    if(_appContext.pGameSettings->getMusicVolume() >= 0.01f)
    {
        if( _appContext.pGameMusic != null && _appContext.pGameMusic->isAlive())
        {
            _appContext.pGameMusic->kill();
            _appContext.pGameMusic = null;
        }

        _appContext.pGameMusic = _appContext.pSoundWorld->createSoundSource();
        String musicName = getSurvivalMusic();
        if(_appContext.pGameMusic != null && musicName != String::EMPTY)
        {
            _appContext.pGameMusic->enqueue(musicName);
            _appContext.pGameMusic->setGain(MUSIC_DEFAULT_GAIN * _appContext.pGameSettings->getMusicVolume());
        }
    }
}

void SoundLibrary::playDemoMusic()
{
    if(_appContext.pGameSettings->getMusicVolume() >= 0.01f)
    {
        if( _appContext.pGameMusic != null && _appContext.pGameMusic->isAlive())
        {
            _appContext.pGameMusic->kill();
            _appContext.pGameMusic = null;
        }

        _appContext.pGameMusic = _appContext.pSoundWorld->createSoundSource();
        String musicName = L"/ChronoRageData/Musics/Demo/02_afterburner_60_loop.ogg";
        if(_appContext.pGameMusic != null)
        {
            _appContext.pGameMusic->enqueue(musicName, true);
            _appContext.pGameMusic->setGain(MUSIC_DEFAULT_GAIN * _appContext.pGameSettings->getMusicVolume());
        }
    }
}


float SoundLibrary::getMusicGain()
{
    return MUSIC_DEFAULT_GAIN;
}

Core::String SoundLibrary::getSound(ESoundCode soundCode)
{
    Core::String soundFile;
    switch(soundCode)
    {
    case SOUND_BUTTON_BLIP:
        soundFile = L"/ChronoRageData/Sounds/button-blip.wav";
        break;
    case SOUND_PLAYER_SHOT:
        soundFile = L"/ChronoRageData/Sounds/player-shot.ogg";
        break;
    case SOUND_PLAYER_SHOT_IMPACT:
        soundFile = L"/ChronoRageData/Sounds/player-shot-impact.wav";
        break;
    case SOUND_PLAYER_SHOT_LASER:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-laser.ogg";
        break;
    case SOUND_PLAYER_SHOT_LASER_LOADING:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-laser-loading.wav";
        break;
    case SOUND_PLAYER_TIME_WARP:
        soundFile = L"/ChronoRageData/Sounds/player-time-warp.wav";
        break;
    case SOUND_PLAYER_TIME_WARP_INCREASE:
        soundFile = L"/ChronoRageData/Sounds/player-time-warp-increase.wav";
        break;
    case SOUND_PLAYER_POWER_UP:
        soundFile = L"/ChronoRageData/Sounds/player-power-up.wav";
        break;
    case SOUND_PLAYER_POWER_DOWN:
        soundFile = L"/ChronoRageData/Sounds/player-power-down.ogg";
        break;
    case SOUND_PLAYER_DESTRUCTION: 
        soundFile = L"/ChronoRageData/Sounds/player-destruction.wav";
        break;
    case SOUND_FOE_DESTRUCTION:
        //soundFile = L"/ChronoRageData/Sounds/foe-explosion.wav";
        break;
    case SOUND_FOE_SHOT_BULLET:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-bullet.ogg";
        break;
    case SOUND_FOE_SHOT_HORNET:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-hornet.wav";
        break;
    case SOUND_FOE_SHOT_LASER:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-laser.ogg";
        break;
    case SOUND_FOE_SHOT_LASER_LOADING:
        soundFile = L"/ChronoRageData/Sounds/foe-shot-laser-loading.wav";
        break;
    case SOUND_CHRONO_BLIP:
        soundFile = L"/ChronoRageData/Sounds/chrono-blip.wav";
        break;
    case SOUND_ACHIEVEMENT:
        soundFile = L"/ChronoRageData/Sounds/achievement.wav";
        break;
    }        
             
    return soundFile;
}

Core::String SoundLibrary::getMenuMusic()
{
    return L"/ChronoRageData/Musics/Smoothass345_H001-CruisingAndTripping-Schmidt.ogg";
}

Core::String SoundLibrary::getLevelMusic(int32 level)
{
    int32 musicId = level % 7;
    Core::String musicFile = L"/ChronoRageData/Musics/01_adrenaline_full_loop.ogg";

    if(musicId < 0)
    {
        musicFile = getSurvivalMusic();
    }
    else
    {
        switch(musicId)
        {
        case 0:
            musicFile = L"/ChronoRageData/Musics/01_adrenaline_full_loop.ogg";
            break;
        case 1:
            musicFile = L"/ChronoRageData/Musics/02_afterburner_60_loop.ogg";
            break;
        case 2:
            musicFile = L"/ChronoRageData/Musics/03_hyperdrive_full.ogg";
            break;
        case 3:
            musicFile = L"/ChronoRageData/Musics/04_BrandingPower_BP42-Starflight-Steele.ogg";
            break;
        case 4:
            musicFile = L"/ChronoRageData/Musics/05_DigitalAge_DA04-ElectroCity-Steele.ogg";
            break;
        case 5:
            musicFile = L"/ChronoRageData/Musics/06_downhill_full.ogg";
            break;
        case 6:
            musicFile = L"/ChronoRageData/Musics/07_on_the_move_full.ogg";
            break;
        }
    }

    return musicFile;
}

Core::String SoundLibrary::getSurvivalMusic()
{
    static int32 tuneId = Core::irand(0, 6);

    Core::String musicFile = getLevelMusic(tuneId);
    tuneId = (tuneId + 1)%7;

    return musicFile;
}

float SoundLibrary::getDefaultGain(ESoundCode soundCode)
{
    LM_ASSERT(isValid(soundCode));

    return defaultSoundGain[soundCode];
}

}//namespace ChronoRage