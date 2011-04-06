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
#ifndef UNIVERSE_PARTEFFECTATTRACTOR_H_
#define UNIVERSE_PARTEFFECTATTRACTOR_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{

class LM_API_UNI PartEffectAttractor : public Core::Object, public IPartEffect
{
public:
    PartEffectAttractor(const Core::Vector3f & position, float intensity);

    void setPosition(const Core::Vector3f & position);
    void setIntensity(float intensity);
    void setMagnet(bool magnet);
    void setMagnetRadius(float magnetRadius);

    inline const Core::Vector3f & getPosition() const { return _position; }
    inline float getIntensity() const { return _intensity; }
    inline bool getMagnet() const { return _magnet; }
    inline float getMagnetRadius() const { return _magnetRadius; }

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_ATTRACTOR; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Core::Vector3f   _position;
    float            _intensity;
    bool             _magnet;
    float            _magnetRadius;
};

}//namespace Universe

#endif /*UNIVERSE_PARTEFFECTATTRACTOR_H_*/
