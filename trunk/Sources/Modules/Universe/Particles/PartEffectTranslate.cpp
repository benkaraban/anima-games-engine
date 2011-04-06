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
#include <Universe/Particles/PartEffectTranslate.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectTranslate::PartEffectTranslate() :
    _constSpeed(Core::ZERO),
    _randSpeed(Core::ZERO)
{
}
//-----------------------------------------------------------------------------
void PartEffectTranslate::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
    for(Core::List<Particle>::iterator iPart = particles.begin(); iPart != particles.end(); ++iPart)
    {
        iPart->position.x += float(elapsedTime) * (_constSpeed.x + iPart->randU * _randSpeed.x);
        iPart->position.y += float(elapsedTime) * (_constSpeed.y + iPart->randV * _randSpeed.y);
        iPart->position.z += float(elapsedTime) * (_constSpeed.z + iPart->randW * _randSpeed.z);
    }
}
//-----------------------------------------------------------------------------
void PartEffectTranslate::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"const-speed", Core::toXMLString(_constSpeed));
    pXMLNode->setValue(L"rand-speed", Core::toXMLString(_randSpeed));
}
//-----------------------------------------------------------------------------
void PartEffectTranslate::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    Core::getXMLValue(*pXMLNode, L"const-speed", _constSpeed);
    Core::getXMLValue(*pXMLNode, L"rand-speed", _randSpeed);
}
//-----------------------------------------------------------------------------
}
