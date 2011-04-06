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
#ifndef UNIVERSE_NODESOUND_H_
#define UNIVERSE_NODESOUND_H_

#include <Universe/Node.h>
#include <Universe/RessourcePool.h>
#include <Audio/ISoundSource.h>

namespace Universe
{

LM_ENUM_3(ESoundFade,   SOUND_NO_FADE,
                        SOUND_FADE_IN,
                        SOUND_FADE_OUT);

class LM_API_UNI NodeSoundSource : public Node
{
public:
    struct SoundSourceInfo
    {
        SoundSourceInfo() : name(L""), loop(false) {}
        String  name;
        bool    loop;
    };

    NodeSoundSource(const Ptr<Renderer::IAABox> & pBox,
                    const Ptr<RessourcePool> & pRessourcesPool,
                    const Ptr<Audio::ISoundSource> & pSoundSource,
                    World * pWorld);

    virtual ~NodeSoundSource();

    virtual ENodeType getNodeType() const { return NODE_SOUND; };

    void enqueue(const String & soundName, bool loop = false);
    void eraseAllSounds();
    void setGain(float gain);
    float getGain() {return _gain;}
    void stopCurrentLoop();

    void killAtEnd(bool killAtEnd){ _killAtEnd = killAtEnd; }
    bool killAtEnd() const { return _killAtEnd; }

    bool playbackCompleted();

    virtual void update(double elapsed);

    void fadeIn(float duration);
    void fadeOut(float duration);

    void setStaticSound(bool isStatic);
    bool isStaticSound() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    const Core::List<SoundSourceInfo>& getSoundsSourceInfos() {return _soundsSrcInfos;}
    void setPause(bool  paused) {_paused = paused;}
    bool getPaused() {return _paused;}

protected:
    virtual void onWorldMatrixUpdate();
    virtual void internalPrepareForPacking();

private:
    Ptr<RessourcePool>          _pRessourcesPool;
    Ptr<Audio::ISoundSource>    _pSoundSource;
    float                       _lastPlay;
    bool                        _killAtEnd;
    float                       _gain;

    ESoundFade                  _fade;
    double                      _duration;
    double                      _elapsed;
    Core::List<SoundSourceInfo> _soundsSrcInfos;
    bool                        _paused;
};

}//namespace Universe

#endif/*UNIVERSE_NODESOUND_H_*/