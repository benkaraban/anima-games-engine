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
#include "NodeSoundSource.h"

namespace Universe
{


const float SOUND_SOURCE_TIMER = 0.2f;

NodeSoundSource::NodeSoundSource(   const Ptr<Renderer::IAABox> & pBox,
                                    const Ptr<RessourcePool> & pRessourcesPool,
                                    const Ptr<Audio::ISoundSource> & pSoundSource,
                                    World * pWorld)
:   Node(pBox, pWorld),
    _pRessourcesPool(pRessourcesPool),
    _pSoundSource(pSoundSource),
    _lastPlay(0.0f),
    _killAtEnd(false),
    _gain(1.0f),
    _fade(SOUND_NO_FADE),
    _duration(0.0),
    _elapsed(0.0),
    _paused(false)
{
}

NodeSoundSource::~NodeSoundSource()
{
}

void NodeSoundSource::enqueue(const String & soundName, bool loop)
{
    Ptr<Audio::Playable> pPlayable = _pRessourcesPool->getTune(soundName);

    _pSoundSource->enqueue(pPlayable, loop);

    SoundSourceInfo info;
        info.name = soundName;
        info.loop = loop;
    _soundsSrcInfos.push_back(info);
}

void NodeSoundSource::eraseAllSounds()
{
    _pSoundSource->clear();
    _soundsSrcInfos.clear();
}

void NodeSoundSource::setGain(float gain)
{
    _gain = gain;
    _pSoundSource->setGain(gain);
}

void NodeSoundSource::stopCurrentLoop()
{
    _pSoundSource->stopCurrentLoop();
}

bool NodeSoundSource::playbackCompleted()
{
    return _pSoundSource->playbackCompleted();
}

void NodeSoundSource::update(double elapsed)
{
    if (_paused == false)
    {
        float a = 1.0f;

        if(_fade != SOUND_NO_FADE)
        {
            _elapsed = std::min(_duration, _elapsed + elapsed);
            if(_fade == SOUND_FADE_IN)
                a = float(_elapsed / _duration);
            else if(_fade == SOUND_FADE_OUT)
                a = 1.0f - float(_elapsed / _duration);

            if(_fade == SOUND_FADE_OUT && a == 0.0f && _killAtEnd)
            {
                kill();
            }
            
            _pSoundSource->setGain(a * _gain);

            if(a >= 1.0f && _fade == SOUND_FADE_IN)
                _fade = SOUND_NO_FADE;
            if(a <= 0.0f && _fade == SOUND_FADE_OUT)
                _fade = SOUND_NO_FADE;
        }

        _lastPlay = std::max(0.0f, _lastPlay - float(elapsed));
        if(!_pSoundSource->playbackCompleted())
        {
            if(_lastPlay <= 0.0f)
            {
                _pSoundSource->play();
                _lastPlay = SOUND_SOURCE_TIMER;
            }
        }
        else if(_killAtEnd)
        {
            kill();
        }
    }
}

void NodeSoundSource::fadeIn(float duration)
{
    _duration = duration;
    _elapsed = 0.0f;
    _fade = SOUND_FADE_IN;
}

void NodeSoundSource::fadeOut(float duration)
{
    _duration = duration;
    _elapsed = 0.0f;
    _fade = SOUND_FADE_OUT;
}

void NodeSoundSource::onWorldMatrixUpdate()
{
    if (!_isStatic)
    {
        Core::Vector3f position = getWorldPosition();
        Core::Vector3f direction = getWorldDirection();

        _pSoundSource->setPosition(position);
        _pSoundSource->setDirection(direction);
    }
}

void NodeSoundSource::setStaticSound(bool isStatic)
{
    _pSoundSource->setStaticSound(isStatic);
}

bool NodeSoundSource::isStaticSound() const
{
    return _pSoundSource->isStaticSound();
}

void NodeSoundSource::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String                      fadeStr;
    String                      name;
    bool                        loop;
    bool                        isStatic;
    Core::List<SoundSourceInfo> sourcesInfo;
    int32                       nbSource;                   

    pNode->getValue(L"last-play",   _lastPlay);
    pNode->getValue(L"kill-at-end", _killAtEnd);
    pNode->getValue(L"gain",        _gain);
    pNode->getValue(L"duration",    _duration);
    pNode->getValue(L"elapsed",     _elapsed);
    pNode->getValue(L"fade",        fadeStr);
    pNode->getValue(L"isStatic",    isStatic);
        setStaticSound(isStatic);

    Core::int32 i = 0;
    while(i < Universe::ESoundFade_COUNT)
    {
        if(fadeStr == Core::toString((Universe::ESoundFade) i))
            _fade = (Universe::ESoundFade) i;

        i++;
    }


    pNode->getValue(L"nb-sources-names", nbSource);
    for (int32 ii=0; ii<nbSource; ++ii)
    {
        pNode->getValue(L"sources-names-" + Core::toString(ii), name);
        pNode->getValue(L"sources-loop-" + Core::toString(ii), loop);

        SoundSourceInfo info;
            info.name = name;
            info.loop = loop;

        sourcesInfo.push_back(info);
    }

    for (int32 ii=0; ii<sourcesInfo.size(); ++ii)
    {
        enqueue(sourcesInfo[ii].name, sourcesInfo[ii].loop);
    }

    restart();
}

void NodeSoundSource::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"last-play",   _lastPlay);
    pNode->setValue(L"kill-at-end", _killAtEnd);
    pNode->setValue(L"gain",        _gain);
    pNode->setValue(L"duration",    _duration);
    pNode->setValue(L"elapsed",     _elapsed);
    pNode->setValue(L"fade", Core::toString(_fade));
    pNode->setValue(L"isStatic", isStaticSound());
    
    pNode->setValue(L"nb-sources-names", _soundsSrcInfos.size());
    for (int32 ii=0; ii<_soundsSrcInfos.size(); ++ii)
    {
        pNode->setValue(L"sources-names-" + Core::toString(ii), _soundsSrcInfos[ii].name);
        pNode->setValue(L"sources-loop-" + Core::toString(ii), _soundsSrcInfos[ii].loop);
    }
}

void NodeSoundSource::internalPrepareForPacking()
{
}

}//namespace Universe
