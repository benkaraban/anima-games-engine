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
#ifndef UNIVERSE_PARTAFFECTANIMATE_H_
#define UNIVERSE_PARTAFFECTANIMATE_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{
class LM_API_UNI PartEffectAnimate : public Core::Object, public IPartEffect
{
public:
    PartEffectAnimate(float key0,
                      float key1,
                      float duration,
                      bool loop,
                      bool killPartsAtEnd);

    void setKey0(float key0);
    void setKey1(float key1);
    void setDuration(float duration);
    void setLoop(bool flag);
    void killPartsAtEnd(bool flag);

    inline float getKey0() const { return _key0; }
    inline float getKey1() const { return _key1; }
    inline float getDuration() const { return _duration; }
    inline bool getLoop() const { return _loop; }
    inline bool killPartsAtEnd() const { return _killParts; }

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_ANIMATE; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    float           _key0;
    float           _key1;
    float           _duration;
    bool            _loop;
    bool            _killParts;
};
}

#endif /*UNIVERSE_PARTAFFECTANIMATE_H_*/
