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
#ifndef UNIVERSE_PARTAFFECTORTURNAROUND_H_
#define UNIVERSE_PARTAFFECTORTURNAROUND_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{
class LM_API_UNI PartEffectTurn : public Core::Object, public IPartEffect
{
public:
    PartEffectTurn();

    void setAxis(const Core::Vector3f & axis) { _axis = axis; }
    void setRadialSpeed(float r) { _radialSpeed = r; }
    void setRandSpeed(float r) { _randSpeed = r; }
    void setAroundCenter(bool flag) { _aroundCenter = flag; }

    const Core::Vector3f & getAxis() const { return _axis; }
    float getRadialSpeed() const { return _radialSpeed; }
    float getRandSpeed() const { return _randSpeed; }
    bool isAroundCenter() const { return _aroundCenter; }

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    inline virtual EEffect getType() const { return Universe::EFFECT_TURN; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Core::Vector3f  _axis;
    float           _radialSpeed;
    float           _randSpeed;
    bool            _aroundCenter;
};

}

#endif /*PARTICLE_PARTAFFECTORFADE_H_*/