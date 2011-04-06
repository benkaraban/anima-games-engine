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
#include <Universe/Particles/PartEffectResistance.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectResistance::PartEffectResistance(float resistance)
:   _resistance(resistance)
{
}
//-----------------------------------------------------------------------------
void PartEffectResistance::setResistance(float resistance)
{
    _resistance = resistance;
}
//-----------------------------------------------------------------------------
void PartEffectResistance::updateParticles(Core::List<Particle> & particles,
                                             double elapsedTime)
{
    for(Core::List<Particle>::iterator iPart = particles.begin(); iPart != particles.end(); ++iPart)
    {
        float speed1 = iPart->speed.getLengthSafe();

        if(!Core::isEquivalent(0.0f, speed1))
        {
            float speed2 = float(std::max(0.0, speed1 - elapsedTime * _resistance));
            iPart->speed = iPart->speed * (speed2 / speed1);
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectResistance::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"resistance", _resistance);
}
//-----------------------------------------------------------------------------
void PartEffectResistance::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    float resistance;
    success &= pXMLNode->getValue(L"resistance", resistance);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectResistance values.");

    setResistance(resistance);
}
//-----------------------------------------------------------------------------
}//namespace Universe