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
#include "InGameMusic.h"

InGameMusic::InGameMusic(const Ptr<Universe::World> & pWorld,
        const String& fileNameMusicA, const String& fileNameMusicB, const String& fileNameMusicC, const String& fileNameMusicOutro) :
    _state(IN_GAME_MUSIC_BEGIN),
    _transitionTime(15.f)
{
    _pNodeSoundMusic = pWorld->createSoundSource();
    _pNodeSoundOutro = pWorld->createSoundSource();

    _pNodeSoundMusic->setPause(true);
    _pNodeSoundOutro->setPause(true);

    _pNodeSoundMusic->setStaticSound(true);
    _pNodeSoundOutro->setStaticSound(true);

    _pNodeSoundMusic->enqueue(fileNameMusicA, false);
    _pNodeSoundMusic->enqueue(fileNameMusicB, true);
    _pNodeSoundMusic->enqueue(fileNameMusicC, true);
    _pNodeSoundOutro->enqueue(fileNameMusicOutro, true);

    _pNodeSoundMusic->setWorldPosition(Core::Vector3f::ZERO);
    _pNodeSoundOutro->setWorldPosition(Core::Vector3f::ZERO);
}


void InGameMusic::start()
{
    _state = IN_GAME_MUSIC_BEGIN;
    _pNodeSoundMusic->setPause(false);
}

void InGameMusic::engageFinalState()
{
    if (_state<IN_GAME_MUSIC_FINAL)
    {
        _pNodeSoundMusic->stopCurrentLoop();
    }
}

void InGameMusic::engageOutro()
{
    if (_state != IN_GAME_MUSIC_OUTRO)
    {
        _pNodeSoundOutro->fadeIn(_transitionTime);
        _pNodeSoundMusic->fadeOut(_transitionTime);
        _pNodeSoundOutro->setPause(false);
        _state = IN_GAME_MUSIC_OUTRO;
    }
}

void InGameMusic::setGain(float gain)
{
    _pNodeSoundOutro->setGain(gain);
    _pNodeSoundMusic->setGain(gain);
}