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
#include <Core/Math/MathTools.h>
#include <Core/Math/Random.h>
#include <Universe/Particles/EmitterShapeCone.h>

namespace Universe
{
//-----------------------------------------------------------------------------
EmitterShapeCone::EmitterShapeCone() :
    _minRadius(0.0),
    _maxRadius(0.0),
    _radiusRange(0.0),
    _minAngle(0.0),
    _maxAngle(0.0),
    _minU(1.0),
    _maxU(1.0),
    _uRange(0.0),
    _minSpeed(0.0),
    _maxSpeed(0.0),
    _speedRange(0.0),
    _tangentSpeed(false)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::createParticle(Particle & part)
{
    // http://mathworld.wolfram.com/SpherePointPicking.html
    float u = _minU + Core::frand() * _uRange;
    float teta = f_PI_MUL_2 * Core::frand();

    float mu = Core::L_sqrt(1 - u*u);
    float cosTeta, sinTeta;
    Core::L_sincos(teta, &cosTeta, &sinTeta);

    Core::Vector3f  unitv(mu * cosTeta, u, mu * sinTeta);

    float speed = _minSpeed + Core::frand() * _speedRange;
    float radius = _minRadius + Core::frand() * _radiusRange;

    part.position = radius * unitv;

    if(_tangentSpeed)
    {
        unitv = Core::Vector3f(unitv.z, 0.0f, -unitv.x);
        unitv.normalizeSafe();
    }

    part.speed = speed * unitv;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::setRadius(float minRadius, float maxRadius)
{
    _minRadius = minRadius;
    _maxRadius = maxRadius;
    _radiusRange = maxRadius - minRadius;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::setAngle(float minAngle, float maxAngle)
{
    LM_ASSERT(0 <= minAngle);
    LM_ASSERT(0 <= maxAngle);
    LM_ASSERT(minAngle <= (f_PI + 0.001));
    LM_ASSERT(maxAngle <= (f_PI + 0.001));

    _minAngle = minAngle;
    _maxAngle = maxAngle;
    _maxU = Core::L_cos(minAngle);
    _minU = Core::L_cos(maxAngle);
    _uRange = _maxU - _minU;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::setSpeed(float minSpeed, float maxSpeed)
{
    _minSpeed = minSpeed;
    _maxSpeed = maxSpeed;
    _speedRange = maxSpeed - minSpeed;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::getRadius(float & minRadius, float & maxRadius) const
{
    minRadius = _minRadius;
    maxRadius = _maxRadius;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::getAngle(float & minAngle, float & maxAngle) const
{
    minAngle = _minAngle;
    maxAngle = _maxAngle;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::getSpeed(float & minSpeed, float & maxSpeed) const
{
    minSpeed = _minSpeed;
    maxSpeed = _maxSpeed;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::setTangentialSpeed(bool t)
{
    _tangentSpeed = t;
}
//-----------------------------------------------------------------------------
bool EmitterShapeCone::getTangentialSpeed() const
{
    return _tangentSpeed;
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"shape.min-radius", _minRadius);
    pXMLNode->setValue(L"shape.max-radius", _maxRadius);

    pXMLNode->setValue(L"shape.min-angle", _minAngle);
    pXMLNode->setValue(L"shape.max-angle", _maxAngle);

    pXMLNode->setValue(L"shape.min-speed", _minSpeed);
    pXMLNode->setValue(L"shape.max-speed", _maxSpeed);

    pXMLNode->setValue(L"shape.tangential-speed", _tangentSpeed);
}
//-----------------------------------------------------------------------------
void EmitterShapeCone::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    float minRadius, maxRadius;
    float minAngle, maxAngle;
    float minSpeed, maxSpeed;

    success &= pXMLNode->getValue(L"shape.min-radius", minRadius);
    success &= pXMLNode->getValue(L"shape.max-radius", maxRadius);

    success &= pXMLNode->getValue(L"shape.min-angle", minAngle);
    success &= pXMLNode->getValue(L"shape.max-angle", maxAngle);

    success &= pXMLNode->getValue(L"shape.min-speed", minSpeed);
    success &= pXMLNode->getValue(L"shape.max-speed", maxSpeed);

    if(!pXMLNode->getValue(L"shape.tangential-speed", _tangentSpeed))
        _tangentSpeed = false;

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML EmitterShapeCone values.");

    // Les set* ne font pas que des affectations
    setRadius(minRadius, maxRadius);
    setAngle(minAngle, maxAngle);
    setSpeed(minSpeed, maxSpeed);
}
//-----------------------------------------------------------------------------
}
