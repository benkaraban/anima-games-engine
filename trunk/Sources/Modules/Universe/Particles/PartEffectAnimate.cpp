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
#include <Universe/Particles/PartEffectAnimate.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectAnimate::PartEffectAnimate(float key0,
                                     float key1,
                                     float duration,
                                     bool loop,
                                     bool killPartsAtEnd) :
    _key0(key0),
    _key1(key1),
    _duration(duration),
    _loop(loop),
    _killParts(killPartsAtEnd)
{
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::setKey0(float key0)
{
    _key0 = key0;
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::setKey1(float key1)
{
    _key1 = key1;
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::setDuration(float duration)
{
    _duration = duration;
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::setLoop(bool flag)
{
    _loop = flag;
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::updateParticles(Core::List<Particle> & particles,
                                        double elapsedTime)
{
    int32 ii = 0;

    while(ii < particles.size())
    {
        float lifeTime = (float)(particles[ii].lifeTime);

        if(lifeTime >= _duration && _killParts)
        {
            particles.qerase(ii);
        }
        else
        {
            float alpha = (_duration > 0.0f) ? (lifeTime / _duration) : 1.0f;

            if(_loop)
                alpha = float(fmod(alpha, 1.0f));
            else
                alpha = std::min(alpha, 1.0f);

            particles[ii].timeKey = _key0 + (_key1 - _key0) * alpha;
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"key0", _key0);
    pXMLNode->setValue(L"key1", _key1);
    pXMLNode->setValue(L"duration", _duration);
    pXMLNode->setValue(L"loop", _loop);
    pXMLNode->setValue(L"kill-parts", _killParts);
}
//-----------------------------------------------------------------------------
void PartEffectAnimate::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    success &= pXMLNode->getValue(L"key0", _key0);
    success &= pXMLNode->getValue(L"key1", _key1);
    success &= pXMLNode->getValue(L"duration", _duration);
    success &= pXMLNode->getValue(L"loop", _loop);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectAnimate values.");
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}
