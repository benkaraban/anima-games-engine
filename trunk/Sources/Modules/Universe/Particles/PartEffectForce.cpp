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

#include <Core/XML/XMLMath.h>
#include <Universe/Particles/PartEffectForce.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectForce::PartEffectForce(const Core::Vector3f & accelVec) :
    _accelVec(accelVec),
    _direction(Core::ZERO),
    _acceleration(accelVec.getLengthFast())
{
    if(_acceleration > 0.0f)
        _direction = (1.0f / _acceleration) * _accelVec;
}
//-----------------------------------------------------------------------------
PartEffectForce::PartEffectForce(const Core::Vector3f & direction, float acceleration) :
    _accelVec(acceleration * direction),
    _direction(direction),
    _acceleration(acceleration)
{
}
//-----------------------------------------------------------------------------
void PartEffectForce::setAccelVec(const Core::Vector3f & accelVec)
{
    _accelVec = accelVec;
    _acceleration = accelVec.getLengthFast();
    if(_acceleration > 0.0f)
        _direction = (1.0f / _acceleration) * _accelVec;
    else
        _direction.setZero();
}
//-----------------------------------------------------------------------------
void PartEffectForce::setDirection(const Core::Vector3f & direction)
{
    _direction = direction;
    _accelVec = _acceleration * direction;
}
//-----------------------------------------------------------------------------
void PartEffectForce::setAcceleration(float acceleration)
{
    _acceleration = acceleration;
    _accelVec = acceleration * _direction;
}
//-----------------------------------------------------------------------------
void PartEffectForce::updateParticles(Core::List<Particle> & particles,
                             double elapsedTime)
{
    Core::Vector3f deltaSpeed((float)(elapsedTime) * _accelVec);

    for(Core::List<Particle>::iterator iPart = particles.begin(); iPart != particles.end(); ++iPart)
    {
        iPart->speed += deltaSpeed;
    }
}
//-----------------------------------------------------------------------------
void PartEffectForce::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"direction", Core::toXMLString(_direction));
    pXMLNode->setValue(L"acceleration", _acceleration);
}
//-----------------------------------------------------------------------------
void PartEffectForce::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    String directionString;
    success &= pXMLNode->getValue(L"direction", directionString);
    Core::Vector3f direction;
    Core::fromXMLString(directionString, direction);
    float acceleration;
    success &= pXMLNode->getValue(L"acceleration", acceleration);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectForce values.");

    setDirection(direction);
    setAcceleration(acceleration);
}
//-----------------------------------------------------------------------------
}
