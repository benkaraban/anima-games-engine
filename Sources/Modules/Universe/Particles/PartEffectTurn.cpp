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
#include <Core/Logger.h>
#include <Core/Math/Interpolate.h>
#include <Core/Math/Matrix.h>
#include <Core/XML/XMLMath.h>
#include <Universe/Particles/PartEffectTurn.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectTurn::PartEffectTurn() :
    _axis(0.0f, 1.0f, 0.0f),
    _radialSpeed(0.0f),
    _randSpeed(0.0f),
    _aroundCenter(false)
{
}
//-----------------------------------------------------------------------------
void PartEffectTurn::updateParticles(Core::List<Particle> & particles,
                                           double elapsedTime)
{
    int32 ii = 0;
   
    if(_aroundCenter)
    {
        for(int32 ii=0; ii < particles.size(); ii++)
        {
            Core::Vector3f unitv(particles[ii].position.getNormalizedFast());
            Core::Vector3f v1(Core::cross(unitv, Core::Vector3f(0.0f, -1.0f, 0.0f)).getNormalizedSafe());
            Core::Vector3f v2(Core::cross(unitv, Core::Vector3f(0.0f, 0.0f, -1.0f)).getNormalizedSafe());
            Core::Vector3f axis = (v1 + v2).getNormalizedFast();

            Core::Matrix4f rotMatrix2(Core::ROTATION, float(elapsedTime * (_radialSpeed + particles[ii].randU * _randSpeed)), axis);
            particles[ii].position = rotMatrix2.apply(particles[ii].position);
        }
    }
    else
    {
        for(int32 ii=0; ii < particles.size(); ii++)
        {
            Core::Matrix4f rotMatrix(Core::ROTATION, float(elapsedTime * (_radialSpeed + particles[ii].randU * _randSpeed)), _axis.getNormalizedSafe());
            particles[ii].position = rotMatrix.apply(particles[ii].position);
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectTurn::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"axis", Core::toXMLString(_axis));
    pXMLNode->setValue(L"radial-speed", _radialSpeed);
    pXMLNode->setValue(L"rand-speed", _randSpeed);
    pXMLNode->setValue(L"around-center", _aroundCenter);
}
//-----------------------------------------------------------------------------
void PartEffectTurn::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    Core::getXMLValue(*pXMLNode, L"axis", _axis);
    pXMLNode->getValue(L"radial-speed", _radialSpeed);
    pXMLNode->getValue(L"rand-speed", _randSpeed);
    pXMLNode->getValue(L"around-center", _aroundCenter);
}
//-----------------------------------------------------------------------------
}
