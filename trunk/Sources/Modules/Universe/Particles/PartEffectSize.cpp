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
#include <Core/XML/XMLMath.h>
#include <Universe/Particles/PartEffectSize.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectSize::PartEffectSize(float width0,
                               float height0,
                               float width1,
                               float height1,
                               float duration,
                               bool killPartsAtEnd) 
:   _width0(width0),
    _height0(height0),
    _width1(width1),
    _height1(height1),
    _duration(duration),
    _killParts(killPartsAtEnd)
{
}
//-----------------------------------------------------------------------------
void PartEffectSize::setSize0(float width, float height)
{
    _width0 = width;
    _height0 = height;
}
//-----------------------------------------------------------------------------
void PartEffectSize::setSize1(float width, float height)
{
    _width1 = width;
    _height1 = height;
}
//-----------------------------------------------------------------------------
void PartEffectSize::setDuration(float duration)
{
    _duration = duration;
}
//-----------------------------------------------------------------------------
void PartEffectSize::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectSize::setSquareParticles(bool square)
{
    _squareParticles = square;
}
//-----------------------------------------------------------------------------
void PartEffectSize::updateParticles(Core::List<Particle> & particles,
                             double elapsedTime)
{
    int32 ii = 0;

    while(ii < particles.size())
    {
        float lifeTime = (float)(particles[ii].lifeTime);

        if(lifeTime >= _duration)
        {
            if(_killParts)
            {
                particles.qerase(ii);
            }
            else
            {
                particles[ii].width = _width1;
                if(_squareParticles)
                    particles[ii].height = _width1;
                else
                    particles[ii].height = _height1;
                ii ++;
            }
        }
        else
        {
            float alpha = (_duration > 0.0f) ? (lifeTime / _duration) : 1.0f;
            particles[ii].width = (_width1 - _width0) * alpha + _width0;
            if(_squareParticles)
                particles[ii].height = (_width1 - _width0) * alpha + _width0;
            else
                particles[ii].height = (_height1 - _height0) * alpha + _height0;
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectSize::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"width0", _width0);
    pXMLNode->setValue(L"height0", _height0);
    pXMLNode->setValue(L"width1", _width1);
    pXMLNode->setValue(L"height1", _height1);
    pXMLNode->setValue(L"duration", _duration);
    pXMLNode->setValue(L"square-parts", _squareParticles);
    pXMLNode->setValue(L"kill-parts", _killParts);
}
//-----------------------------------------------------------------------------
void PartEffectSize::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    success &= pXMLNode->getValue(L"width0", _width0);
    success &= pXMLNode->getValue(L"height0", _height0);
    success &= pXMLNode->getValue(L"width1", _width1);
    success &= pXMLNode->getValue(L"height1", _height1);
    
    success &= pXMLNode->getValue(L"duration", _duration);
    success &= pXMLNode->getValue(L"square-parts", _squareParticles);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectSize values.");
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PartEffectSize2::PartEffectSize2(   float width0,
                                    float height0,
                                    float width1,
                                    float height1,
                                    float width2,
                                    float height2,
                                    float time1,
                                    float time2,
                                    bool killPartsAtEnd) 
:   _width0(width0),
    _height0(height0),
    _width1(width1),
    _height1(height1),
    _width2(width2),
    _height2(height2),
    _time1(time1),
    _time2(time2),
    _killParts(killPartsAtEnd)
{}
//-----------------------------------------------------------------------------
void PartEffectSize2::setSize0(float width, float height)
{
    _width0 = width;
    _height0 = height;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::setSize1(float width, float height)
{
    _width1 = width;
    _height1 = height;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::setSize2(float width, float height)
{
    _width2 = width;
    _height2 = height;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::setTime1(float time1)
{
    _time1 = time1;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::setTime2(float time2)
{
    _time2 = time2;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::setSquareParticles(bool square)
{
    _squareParticles = square;
}
//-----------------------------------------------------------------------------
void PartEffectSize2::updateParticles(Core::List<Particle> & particles,
                             double elapsedTime)
{
    int32 ii = 0;

    while(ii < particles.size())
    {
        float lifeTime = (float)(particles[ii].lifeTime);

        if(lifeTime >= _time2)
        {
            if(_killParts)
            {
                particles.qerase(ii);
            }
            else
            {
                particles[ii].width = _width2;
                if(_squareParticles)
                    particles[ii].height = _width2;
                else
                    particles[ii].height = _height2;
                ii ++;
            }
        }
        else if (lifeTime >= _time1)
        {
            float duration = _time2 - _time1;
            float alpha = (duration > 0.0f) ? ((lifeTime - _time1) / duration) : 1.0f;
            
            particles[ii].width = (_width2 - _width1) * alpha + _width1;
            if(_squareParticles)
                particles[ii].height = (_width2 - _width1) * alpha + _width1;
            else
                particles[ii].height = (_height2 - _height1) * alpha + _height1;

            ii ++;
        }
        else
        {
            float alpha = (_time1 > 0.0f) ? (lifeTime / _time1) : 1.0f;
            
            particles[ii].width = (_width1 - _width0) * alpha + _width0;
            if(_squareParticles)
                particles[ii].height = (_width1 - _width0) * alpha + _width0;
            else
                particles[ii].height = (_height1 - _height0) * alpha + _height0;
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectSize2::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"width0", _width0);
    pXMLNode->setValue(L"height0", _height0);
    pXMLNode->setValue(L"width1", _width1);
    pXMLNode->setValue(L"height1", _height1);
    pXMLNode->setValue(L"width2", _width2);
    pXMLNode->setValue(L"height2", _height2);
    pXMLNode->setValue(L"time1", _time1);
    pXMLNode->setValue(L"time2", _time2);
    pXMLNode->setValue(L"square-parts", _squareParticles);
    pXMLNode->setValue(L"kill-parts", _killParts);
}
//-----------------------------------------------------------------------------
void PartEffectSize2::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    success &= pXMLNode->getValue(L"width0", _width0);
    success &= pXMLNode->getValue(L"height0", _height0);
    success &= pXMLNode->getValue(L"width1", _width1);
    success &= pXMLNode->getValue(L"height1", _height1);
    success &= pXMLNode->getValue(L"width2", _width2);
    success &= pXMLNode->getValue(L"height2", _height2);

    success &= pXMLNode->getValue(L"time1", _time1);
    success &= pXMLNode->getValue(L"time2", _time2);
    success &= pXMLNode->getValue(L"square-parts", _squareParticles);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectSize2 values.");
}
//-----------------------------------------------------------------------------
}
