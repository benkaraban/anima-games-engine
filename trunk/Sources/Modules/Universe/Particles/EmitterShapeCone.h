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
#ifndef UNIVERSE_EMITTERSHAPESPHERE_H_
#define UNIVERSE_EMITTERSHAPESPHERE_H_

#include <Core/Standard.h>
#include <Universe/Particles/IEmitterShape.h>

namespace Universe
{
/**
 * Emetteur de particules dans une sphère.
 */
class LM_API_UNI EmitterShapeCone : public Core::Object, public IEmitterShape
{
public:
    /**
     * Construit un émetteur sur une sphere de centre \a position et de rayon
     * \a radius. Si \a border est à true alors les particules sont émises sur le
     * bord de la sphère. Dans le cas contraire elles sont émises à l'intérieur
     * de la sphère.
     */
    EmitterShapeCone();
    virtual void createParticle(Particle & part);

    virtual void updateParticles(Core::List<Particle> & particles,
                                 double elapsedTime);

    void setRadius(float minRadius, float maxRadius);

    /**
     * 0 <= minAngle <= PI
     * 0 <= maxAngle <= PI
     */
    void setAngle(float minAngle, float maxAngle);
    void setSpeed(float minSpeed, float maxSpeed);
    void setTangentialSpeed(bool t);

    void getRadius(float & minRadius, float & maxRadius) const;
    void getAngle(float & minAngle, float & maxAngle) const;
    void getSpeed(float & minSpeed, float & maxSpeed) const;
    bool getTangentialSpeed() const;

    inline virtual EShape getType() const { return Universe::SHAPE_CONE; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    float    _minRadius;
    float    _maxRadius;
    float    _radiusRange;

    float    _minAngle;
    float    _maxAngle;
    float    _minU;
    float    _maxU;
    float    _uRange;

    float    _minSpeed;
    float    _maxSpeed;
    float    _speedRange;

    bool     _tangentSpeed;
};
}

#endif /*PARTICLE_EMITTERSHAPESPHERE_H_*/
