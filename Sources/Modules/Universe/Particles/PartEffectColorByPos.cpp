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
#include <Core/Math/Interpolate.h>
#include <Universe/Particles/PartEffectColorByPos.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectColorByPos::PartEffectColorByPos() :
    _axis(COLOR_BY_X),
    _color0(Core::Vector4f::ONE),
    _color1(Core::Vector4f::ONE),
    _minPos(-1.0f),
    _maxPos(1.0f)
{
}
//-----------------------------------------------------------------------------
void PartEffectColorByPos::setRangePos(float minPos, float maxPos)
{
    _minPos = minPos;
    _maxPos = maxPos;
}
//-----------------------------------------------------------------------------
void PartEffectColorByPos::getRangePos(float & minPos, float & maxPos) const
{
    minPos = _minPos;
    maxPos = _maxPos;
}
//-----------------------------------------------------------------------------
void PartEffectColorByPos::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
    for(Core::List<Particle>::iterator iPart = particles.begin(); iPart != particles.end(); ++iPart)
    {
        float p = 0.0f;

        switch(_axis)
        {
        case COLOR_BY_X: p = iPart->position.x; break;
        case COLOR_BY_Y: p = iPart->position.y; break;
        case COLOR_BY_Z: p = iPart->position.z; break;
        }

        float alpha = Core::clamp(0.0f, 1.0f, (p - _minPos) / (_maxPos - _minPos));
        alpha = Core::lerp(alpha, iPart->randU, _randFactor);
        iPart->color.r = Core::lerp(_color0.r, _color1.r, alpha);
        iPart->color.g = Core::lerp(_color0.g, _color1.g, alpha);
        iPart->color.b = Core::lerp(_color0.b, _color1.b, alpha);
    }
}
//-----------------------------------------------------------------------------
void PartEffectColorByPos::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"axis", int32(_axis));

    pXMLNode->setValue(L"color0", Core::toXMLString(_color0));
    pXMLNode->setValue(L"color1", Core::toXMLString(_color1));

    pXMLNode->setValue(L"min-pos", _minPos);
    pXMLNode->setValue(L"max-pos", _maxPos);

    pXMLNode->setValue(L"rand-factor", _randFactor);
}
//-----------------------------------------------------------------------------
void PartEffectColorByPos::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    int32 iAxis = 0;
    pXMLNode->getValue(L"axis", iAxis);
    _axis = EColorAxis(iAxis);

    Core::getXMLValue(*pXMLNode, L"color0", _color0);
    Core::getXMLValue(*pXMLNode, L"color1", _color1);

    pXMLNode->getValue(L"min-pos", _minPos);
    pXMLNode->getValue(L"max-pos", _maxPos);

    pXMLNode->getValue(L"rand-factor", _randFactor);
}
//-----------------------------------------------------------------------------
}
