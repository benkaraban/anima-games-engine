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
#include <Universe/Particles/PartEffectAttractor.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectAttractor::PartEffectAttractor(const Core::Vector3f & position, float intensity)
:   _position(position),
    _intensity(intensity),
    _magnet(false),
    _magnetRadius(0.1f)
{
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::setPosition(const Core::Vector3f & position)
{
    _position = position;
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::setIntensity(float intensity)
{
    _intensity = intensity;
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::setMagnet(bool magnet)
{
    _magnet = magnet;
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::setMagnetRadius(float magnetRadius)
{
    _magnetRadius = magnetRadius;
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::updateParticles(  Core::List<Particle> & particles,
                                             double elapsedTime)
{
    for(Core::List<Particle>::iterator iPart = particles.begin(); iPart != particles.end(); ++iPart)
    {
        if(!_magnet || !iPart->position.isEquivalent(_position, _magnetRadius))
        {
            Core::Vector3f deltaPos = iPart->position - _position;

            if(!deltaPos.isZero())
            {
                iPart->speed += (float)(elapsedTime) * _intensity * deltaPos.getNormalizedSafe();
            }
        }
        else
            iPart->speed = Core::Vector3f(Core::ZERO);
    }
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"position", Core::toXMLString(_position));
    pXMLNode->setValue(L"intensity", _intensity);
}
//-----------------------------------------------------------------------------
void PartEffectAttractor::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    String positionString;
    success &= pXMLNode->getValue(L"position", positionString);
    Core::Vector3f position;
    Core::fromXMLString(positionString, position);
    float intensity;
    success &= pXMLNode->getValue(L"intensity", intensity);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectAttractor values.");

    setPosition(position);
    setIntensity(intensity);
}
//-----------------------------------------------------------------------------
}//namespace Universe
