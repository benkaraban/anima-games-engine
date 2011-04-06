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
#include <Core/XML/XMLMath.h>
#include <Universe/Particles/EmitterShapeBox.h>

namespace Universe
{
//-----------------------------------------------------------------------------
EmitterShapeBox::EmitterShapeBox()
{
    _box.addPoint(Core::Vector3f::ZERO);
}
//-----------------------------------------------------------------------------
void EmitterShapeBox::createParticle(Particle & part)
{
    Core::Vector3f size(_box.getSize());
    part.position.x = _box.min.x + size.x * Core::frand();
    part.position.y = _box.min.y + size.y * Core::frand();
    part.position.z = _box.min.z + size.z * Core::frand();
    part.speed = Core::Vector3f::ZERO;
}
//-----------------------------------------------------------------------------
void EmitterShapeBox::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeBox::setBox(const Core::AABoxf & box)
{ 
    _box = box; 
}
//-----------------------------------------------------------------------------
void EmitterShapeBox::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"shape.min-box", Core::toXMLString(_box.min));
    pXMLNode->setValue(L"shape.max-box", Core::toXMLString(_box.max));
}
//-----------------------------------------------------------------------------
void EmitterShapeBox::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    Core::Vector3f v1;
    Core::Vector3f v2;

    Core::getXMLValue(*pXMLNode, L"shape.min-box", v1);
    Core::getXMLValue(*pXMLNode, L"shape.max-box", v2);

    setBox(Core::AABoxf(v1, v2));
}
//-----------------------------------------------------------------------------
}
