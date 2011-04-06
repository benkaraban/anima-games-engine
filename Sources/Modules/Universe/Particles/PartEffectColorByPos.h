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
#ifndef UNIVERSE_PARTAFFECTORCOLORBYPOS_H_
#define UNIVERSE_PARTAFFECTORCOLORBYPOS_H_

#include <Universe/Particles/IPartEffect.h>

namespace Universe
{
LM_ENUM_3(EColorAxis, COLOR_BY_X, COLOR_BY_Y, COLOR_BY_Z);

class LM_API_UNI PartEffectColorByPos : public Core::Object, public IPartEffect
{
public:
    PartEffectColorByPos();

    void setRandomFactor(float randFactor) { _randFactor = randFactor; };
    void setColorAxis(EColorAxis axis) { _axis = axis; };
    void setColor0(const Core::Vector4f & c) { _color0 = c; };
    void setColor1(const Core::Vector4f & c) { _color1 = c; };
    void setRangePos(float minPos, float maxPos);

    float getRandomFactor() const { return _randFactor; };
    EColorAxis getColorAxis() const { return _axis; };
    const Core::Vector4f & getColor0() const { return _color0; };
    const Core::Vector4f & getColor1() const { return _color1; };
    void getRangePos(float & minPos, float & maxPos) const;

    virtual void updateParticles(Core::List<Particle> & particles, double elapsedTime);

    virtual EEffect getType() const { return Universe::EFFECT_COLOR_BY_POS; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    EColorAxis      _axis;
    Core::Vector4f  _color0;
    Core::Vector4f  _color1;
    float           _minPos;
    float           _maxPos;
    float           _randFactor;
};
}

#endif /*PARTICLE_PARTAFFECTORColorByPos_H_*/
