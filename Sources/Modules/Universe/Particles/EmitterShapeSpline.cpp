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
#include <Universe/Particles/EmitterShapeSpline.h>

namespace Universe
{
//-----------------------------------------------------------------------------
EmitterShapeSpline::EmitterShapeSpline(const Ptr<RessourcePool> & pResPool) :
    _pResPool(pResPool),
    _mode(SPLINE_DONT_MOVE),
    _uStartMin(0.0f),
    _uStartMax(1.0f),
    _uSpeedMin(0.0f),
    _uSpeedMax(0.0f)
{
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::setSpline(const String & name)
{
    _splineName = name;

    if(name != String::EMPTY)
        _pSpline = _pResPool->getSpline(name);
    else
        _pSpline = null;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::createParticle(Particle & part)
{
    part.u        = _uStartMin + Core::frand() * (_uStartMax - _uStartMin);
    part.uSpeed   = _uSpeedMin + Core::frand() * (_uSpeedMax - _uSpeedMin);
    part.speed    = Core::Vector3f(0.0f);

    if(_pSpline == null)
        part.position = Core::Vector3f(0.0f);
    else
        part.position = _pSpline->getPosition(part.u * _pSpline->getDuration());
}
//-----------------------------------------------------------------------------
float qfmod(float x, float m)
{
    if(x > m)
    {
        x -= m;
        if(x > m)
            x = fmod(x, m);
    }
    else if(x < 0.0f)
    {
        x += m;
        if(x < 0.0f)
            x = m - fmod(-x, m);
    }

    return x;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::updateParticles(Core::List<Particle> & particles, double elapsedTime)
{
    if(_pSpline == null)
        return;

    switch(_mode)
    {
    case SPLINE_DONT_MOVE:
        // Nothing to do
        break;

    case SPLINE_MOVE_BORDER_WRAP:
        {
            for(int32 ii=0; ii < particles.size(); ii++)
            {
                Particle & part = particles[ii];
                part.u = qfmod(float(part.u + part.uSpeed * elapsedTime), 1.0f);
                part.position = _pSpline->getPosition(part.u * _pSpline->getDuration());
            }
            break;
        }

    case SPLINE_MOVE_BORDER_MIRROR:
        {
            for(int32 ii=0; ii < particles.size(); ii++)
            {
                Particle & part = particles[ii];
                part.u = float(part.u + part.uSpeed * elapsedTime);

                if(part.u > 1.0f)
                {
                    part.u = qfmod(2.0f - part.u, 1.0f);
                    part.uSpeed = -part.uSpeed;
                }
                else if(part.u < 0.0f)
                {
                    part.u = qfmod(-part.u, 1.0f);
                    part.uSpeed = -part.uSpeed;
                }


                part.position = _pSpline->getPosition(part.u * _pSpline->getDuration());
            }
            break;
        }

    case SPLINE_MOVE_BORDER_KILL:
        {
            int32 ii = 0;

            while(ii < particles.size())
            {
                Particle & part = particles[ii];
                part.u = float(part.u + part.uSpeed * elapsedTime);

                if(part.u > 1.0f || part.u < 0.0f)
                {
                    particles.qerase(ii);
                }
                else
                {
                    part.position = _pSpline->getPosition(part.u * _pSpline->getDuration());
                    ii++;
                }
            }
            break;
        }
    }
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::setUSpeed(float uMin, float uMax)
{
    _uSpeedMin = uMin;
    _uSpeedMax = uMax;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::setUStart(float uMin, float uMax)
{
    _uStartMin = uMin;
    _uStartMax = uMax;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::getUSpeed(float & uMin, float & uMax) const
{
    uMin = _uSpeedMin;
    uMax = _uSpeedMax;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::getUStart(float & uMin, float & uMax) const
{
    uMin = _uStartMin;
    uMax = _uStartMax;
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"shape.spline-name", _splineName);
    pXMLNode->setValue(L"shape.mode", Universe::toString(_mode));

    pXMLNode->setValue(L"shape.min-ustart", _uStartMin);
    pXMLNode->setValue(L"shape.max-ustart", _uStartMax);

    pXMLNode->setValue(L"shape.min-uspeed", _uSpeedMin);
    pXMLNode->setValue(L"shape.max-uspeed", _uSpeedMax);
}
//-----------------------------------------------------------------------------
void EmitterShapeSpline::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    String strMode, splineName;

    pXMLNode->getValue(L"shape.spline-name", splineName);
    pXMLNode->getValue(L"shape.mode", strMode);

    setSpline(splineName);

    if(strMode == Universe::toString(SPLINE_MOVE_BORDER_WRAP))
        _mode = SPLINE_MOVE_BORDER_WRAP;
    else if(strMode == Universe::toString(SPLINE_MOVE_BORDER_MIRROR))
        _mode = SPLINE_MOVE_BORDER_MIRROR;
    else if(strMode == Universe::toString(SPLINE_MOVE_BORDER_KILL))
        _mode = SPLINE_MOVE_BORDER_KILL;
    else
        _mode = SPLINE_DONT_MOVE;

    pXMLNode->getValue(L"shape.min-ustart", _uStartMin);
    pXMLNode->getValue(L"shape.max-ustart", _uStartMax);

    pXMLNode->getValue(L"shape.min-uspeed", _uSpeedMin);
    pXMLNode->getValue(L"shape.max-uspeed", _uSpeedMax);
}
//-----------------------------------------------------------------------------
}
