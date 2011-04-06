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
#include <Core/XML/XMLMath.h>
#include <Universe/Particles/PartEffectFade.h>

namespace Universe
{
//-----------------------------------------------------------------------------
PartEffectFade::PartEffectFade(const Core::Vector4f & sourceColor,
                               const Core::Vector4f & targetColor,
                               float duration,
                               bool killParts) :
    _sourceColor(sourceColor),
    _targetColor(targetColor),
    _duration(duration),
    _killParts(killParts),
    _onlyAlpha(false)
{
}
//-----------------------------------------------------------------------------
void PartEffectFade::setSourceColor(const Core::Vector4f & sourceColor)
{
    _sourceColor = sourceColor;
}
//-----------------------------------------------------------------------------
void PartEffectFade::setTargetColor(const Core::Vector4f & targetColor)
{
    _targetColor = targetColor;
}
//-----------------------------------------------------------------------------
void PartEffectFade::setDuration(float duration)
{
    _duration = duration;
}
//-----------------------------------------------------------------------------
void PartEffectFade::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade::onlyAlpha(bool flag)
{
    _onlyAlpha = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade::updateParticles(Core::List<Particle> & particles,
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
                if(_onlyAlpha)
                    particles[ii].color.a = _targetColor.a;
                else
                    particles[ii].color = _targetColor;
                ii ++;
            }
        }
        else
        {
            float alpha = (_duration > 0.0f) ? (lifeTime / _duration) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_sourceColor.a, _targetColor.a, alpha);
            else
                particles[ii].color.setLerp(_sourceColor, _targetColor, alpha);
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectFade::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"source-color", Core::toXMLString(_sourceColor));
    pXMLNode->setValue(L"target-color", Core::toXMLString(_targetColor));
    pXMLNode->setValue(L"duration", _duration);
    pXMLNode->setValue(L"kill-parts", _killParts);
    pXMLNode->setValue(L"only-alpha", _onlyAlpha);
}
//-----------------------------------------------------------------------------
void PartEffectFade::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    String sourceColorString;
    String targetColorString;
    success &= pXMLNode->getValue(L"source-color", sourceColorString);
    success &= pXMLNode->getValue(L"target-color", targetColorString);
    Core::fromXMLString(sourceColorString, _sourceColor);
    Core::fromXMLString(targetColorString, _targetColor);

    success &= pXMLNode->getValue(L"duration", _duration);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if(!pXMLNode->getValue(L"only-alpha", _onlyAlpha))
        _onlyAlpha = false;

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectFade values.");
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PartEffectFade2::PartEffectFade2(
        const Core::Vector4f & color0,
        const Core::Vector4f & color1,
        const Core::Vector4f & color2,
        float time1,
        float time2,
        bool killPartsAtEnd,
        bool loop) :
    _color0(color0),
    _color1(color1),
    _color2(color2),
    _time1(time1),
    _time2(time2),
    _killParts(killPartsAtEnd),
    _loop(loop),
    _onlyAlpha(false)
{}
//-----------------------------------------------------------------------------
void PartEffectFade2::setColor0(const Core::Vector4f & color0)
{
    _color0 = color0;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::setColor1(const Core::Vector4f & color1)
{
    _color1 = color1;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::setColor2(const Core::Vector4f & color2)
{
    _color2 = color2;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::setTime1(float time1)
{
    _time1 = time1;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::setTime2(float time2)
{
    _time2 = time2;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::loop(bool flag)
{
    _loop = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::onlyAlpha(bool flag)
{
    _onlyAlpha = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade2::updateParticles(Core::List<Particle> & particles,
                             double elapsedTime)
{
    int32 ii = 0;

    while(ii < particles.size())
    {
        float lifeTime = 0.0f;

        if(_loop && (float)(particles[ii].lifeTime) >= _time2)
        {
            lifeTime = 0.0f;
            particles[ii].lifeTime = 0.0f;
        }
        else
            lifeTime = (float)(particles[ii].lifeTime);

        if(lifeTime >= _time2)
        {
            if(_killParts)
            {
                particles.qerase(ii);
            }
            else
            {
                if(_onlyAlpha)
                    particles[ii].color.a = _color2.a;
                else
                    particles[ii].color = _color2;
                ii ++;
            }
        }
        else if (lifeTime >= _time1)
        {
            float duration = _time2 - _time1;
            float alpha = (duration > 0.0f) ? ((lifeTime - _time1) / duration) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_color1.a, _color2.a, alpha);
            else
                particles[ii].color.setLerp(_color1, _color2, alpha);
            ii ++;
        }
        else
        {
            float alpha = (_time1 > 0.0f) ? (lifeTime / _time1) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_color0.a, _color1.a, alpha);
            else
                particles[ii].color.setLerp(_color0, _color1, alpha);
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectFade2::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"color0", Core::toXMLString(_color0));
    pXMLNode->setValue(L"color1", Core::toXMLString(_color1));
    pXMLNode->setValue(L"color2", Core::toXMLString(_color2));
    pXMLNode->setValue(L"time1", _time1);
    pXMLNode->setValue(L"time2", _time2);
    pXMLNode->setValue(L"kill-parts", _killParts);
    pXMLNode->setValue(L"loop", _loop);
    pXMLNode->setValue(L"only-alpha", _onlyAlpha);
}
//-----------------------------------------------------------------------------
void PartEffectFade2::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    String color0;
    String color1;
    String color2;
    success &= pXMLNode->getValue(L"color0", color0);
    success &= pXMLNode->getValue(L"color1", color1);
    success &= pXMLNode->getValue(L"color2", color2);
    Core::fromXMLString(color0, _color0);
    Core::fromXMLString(color1, _color1);
    Core::fromXMLString(color2, _color2);

    success &= pXMLNode->getValue(L"time1", _time1);
    success &= pXMLNode->getValue(L"time2", _time2);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if( !pXMLNode->getValue(L"loop", _loop) )
        _loop = false;

    if(!pXMLNode->getValue(L"only-alpha", _onlyAlpha))
        _onlyAlpha = false;

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectFade2 values.");
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PartEffectFade3::PartEffectFade3(
        const Core::Vector4f & color0,
        const Core::Vector4f & color1,
        const Core::Vector4f & color2,
        const Core::Vector4f & color3,
        float time1,
        float time2,
        float time3,
        bool killPartsAtEnd,
        bool loop) :
    _color0(color0),
    _color1(color1),
    _color2(color2),
    _color3(color3),
    _time1(time1),
    _time2(time2),
    _time3(time3),
    _killParts(killPartsAtEnd),
    _loop(loop),
    _onlyAlpha(false)
{}
//-----------------------------------------------------------------------------
void PartEffectFade3::setColor0(const Core::Vector4f & color0)
{
    _color0 = color0;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setColor1(const Core::Vector4f & color1)
{
    _color1 = color1;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setColor2(const Core::Vector4f & color2)
{
    _color2 = color2;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setColor3(const Core::Vector4f & color3)
{
    _color3 = color3;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setTime1(float time1)
{
    _time1 = time1;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setTime2(float time2)
{
    _time2 = time2;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::setTime3(float time3)
{
    _time3 = time3;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::killPartsAtEnd(bool flag)
{
    _killParts = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::loop(bool flag)
{
    _loop = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::onlyAlpha(bool flag)
{
    _onlyAlpha = flag;
}
//-----------------------------------------------------------------------------
void PartEffectFade3::updateParticles(Core::List<Particle> & particles,
                             double elapsedTime)
{
    int32 ii = 0;

    while(ii < particles.size())
    {
        float lifeTime = 0.0f;

        if(_loop && (float)(particles[ii].lifeTime) >= _time3)
        {
            lifeTime = 0.0f;
            particles[ii].lifeTime = 0.0f;
        }
        else
            lifeTime = (float)(particles[ii].lifeTime);

        if(lifeTime >= _time3)
        {
            if(_killParts)
            {
                particles.qerase(ii);
            }
            else
            {
                if(_onlyAlpha)
                    particles[ii].color.a = _color3.a;
                else
                    particles[ii].color = _color3;
                ii ++;
            }
        }
        else if (lifeTime >= _time2)
        {
            float duration = _time3 - _time2;
            float alpha = (duration > 0.0f) ? ((lifeTime - _time2) / duration) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_color2.a, _color3.a, alpha);
            else
                particles[ii].color.setLerp(_color2, _color3, alpha);
            ii ++;
        }
        else if (lifeTime >= _time1)
        {
            float duration = _time2 - _time1;
            float alpha = (duration > 0.0f) ? ((lifeTime - _time1) / duration) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_color1.a, _color2.a, alpha);
            else
                particles[ii].color.setLerp(_color1, _color2, alpha);
            ii ++;
        }
        else
        {
            float alpha = (_time1 > 0.0f) ? (lifeTime / _time1) : 1.0f;
            if(_onlyAlpha)
                particles[ii].color.a = Core::lerp(_color0.a, _color1.a, alpha);
            else
                particles[ii].color.setLerp(_color0, _color1, alpha);
            ii ++;
        }
    }
}
//-----------------------------------------------------------------------------
void PartEffectFade3::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setValue(L"color0", Core::toXMLString(_color0));
    pXMLNode->setValue(L"color1", Core::toXMLString(_color1));
    pXMLNode->setValue(L"color2", Core::toXMLString(_color2));
    pXMLNode->setValue(L"color3", Core::toXMLString(_color3));
    pXMLNode->setValue(L"time1", _time1);
    pXMLNode->setValue(L"time2", _time2);
    pXMLNode->setValue(L"time3", _time3);
    pXMLNode->setValue(L"kill-parts", _killParts);
    pXMLNode->setValue(L"loop", _loop);
    pXMLNode->setValue(L"only-alpha", _onlyAlpha);
}
//-----------------------------------------------------------------------------
void PartEffectFade3::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    String color0;
    String color1;
    String color2;
    String color3;
    success &= pXMLNode->getValue(L"color0", color0);
    success &= pXMLNode->getValue(L"color1", color1);
    success &= pXMLNode->getValue(L"color2", color2);
    success &= pXMLNode->getValue(L"color3", color3);
    Core::fromXMLString(color0, _color0);
    Core::fromXMLString(color1, _color1);
    Core::fromXMLString(color2, _color2);
    Core::fromXMLString(color3, _color3);

    success &= pXMLNode->getValue(L"time1", _time1);
    success &= pXMLNode->getValue(L"time2", _time2);
    success &= pXMLNode->getValue(L"time3", _time3);
    success &= pXMLNode->getValue(L"kill-parts", _killParts);

    if(!pXMLNode->getValue(L"loop", _loop))
        _loop = false;

    if(!pXMLNode->getValue(L"only-alpha", _onlyAlpha))
        _onlyAlpha = false;

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML PartEffectFade3 values.");
}
//-----------------------------------------------------------------------------
}
