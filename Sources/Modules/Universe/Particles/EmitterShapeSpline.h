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
#ifndef UNIVERSE_EMITTERSHAPESPLINE_H_
#define UNIVERSE_EMITTERSHAPESPLINE_H_

#include <Core/Standard.h>
#include <Core/Math/Splines/Trajectory.h>
#include <Universe/Particles/IEmitterShape.h>
#include <Universe/RessourcePool.h>

namespace Universe
{
LM_ENUM_4(ESplineShapeMode,
          SPLINE_DONT_MOVE,
          SPLINE_MOVE_BORDER_WRAP,
          SPLINE_MOVE_BORDER_MIRROR,
          SPLINE_MOVE_BORDER_KILL);


/**
 * Emetteur de particules dans une sphère.
 */
class LM_API_UNI EmitterShapeSpline : public Core::Object, public IEmitterShape
{
public:
    EmitterShapeSpline(const Ptr<RessourcePool> & pResPool);

    const String & getSplineName() const { return _splineName; }

    void setSpline(const String & name);

    void setMode(ESplineShapeMode mode) { _mode = mode; }
    ESplineShapeMode getMode() const { return _mode; }

    virtual void createParticle(Particle & part);

    virtual void updateParticles(Core::List<Particle> & particles, double elapsedTime);

    void setUSpeed(float uMin, float uMax);
    void setUStart(float uMin, float uMax);

    void getUSpeed(float & uMin, float & uMax) const;
    void getUStart(float & uMin, float & uMax) const;

    inline virtual EShape getType() const { return Universe::SHAPE_SPLINE; }
    virtual void exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const;
    virtual void importXML(const Core::Ptr<Core::XMLNode> & pXMLNode);

protected:
    Ptr<RessourcePool>     _pResPool;
    Ptr<Core::Trajectory>  _pSpline;
    ESplineShapeMode       _mode;
    String                 _splineName;

    float                  _uStartMin;
    float                  _uStartMax;

    float                  _uSpeedMin;
    float                  _uSpeedMax;
};
}

#endif /*PARTICLE_EMITTERSHAPESPHERE_H_*/
