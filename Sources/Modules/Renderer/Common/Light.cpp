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
#include <Renderer/Common/Light.h>

namespace Renderer
{
//------------------------------------------------------------------------------
static float toLuminosity(const Core::Vector4f & color)
{
    return 0.3f * color.r + 0.59f * color.g + 0.11f * color.b;
}
//------------------------------------------------------------------------------
PointLightInfluence::PointLightInfluence() :
    coeff(0.0), pLight(0)
{
}
//------------------------------------------------------------------------------
PointLightInfluence::PointLightInfluence(float coeff, PointLight * pLight) :
    coeff(coeff), pLight(pLight)
{
}
//------------------------------------------------------------------------------
PointLight::PointLight() :
    _ambient(0.0, 0.0, 0.0, 0.0),
    _diffuse(1.0, 1.0, 1.0, 1.0),
    _specular(1.0, 1.0, 1.0, 1.0),
    _position(0.0, 0.0, 0.0),
    _radius(1.0),
    _invRadius(1.0),
    _ambientLum(0.0),
    _diffuseLum(1.0),
    _specularLum(1.0),
    _shadowCast(false),
    _isVisible(true),
    _isAlive(true)
{
    updateLuminosity();
}
//------------------------------------------------------------------------------
void PointLight::setAmbientColor(const Core::Vector4f & color)
{
    _ambient = color;
    _ambientLum = toLuminosity(color);
    updateLuminosity();
}
//------------------------------------------------------------------------------
void PointLight::setDiffuseColor(const Core::Vector4f & color)
{
    _diffuse = color;
    _diffuseLum = toLuminosity(color);
    updateLuminosity();
}
//------------------------------------------------------------------------------
void PointLight::setSpecularColor(const Core::Vector4f & color)
{
    _specular = color;
    _specularLum = toLuminosity(color);
    updateLuminosity();
}
//------------------------------------------------------------------------------
void PointLight::updateLuminosity()
{
    _luminosity = 0.5f * (_ambientLum + 0.75f * _diffuseLum + 0.25f * _specularLum);
}
//------------------------------------------------------------------------------
void PointLight::setRadius(float radius)
{
    _radius = radius;
    if(_radius == 0.0)
        _invRadius = 0.0;
    else
        _invRadius = 1.0f / radius;
}
//------------------------------------------------------------------------------
PointLightInfluence PointLight::getInfluence(const Core::AABoxf & box)
{
    float coeff = 0.0f;

    if(_isVisible)
    {
        Core::Vector3f center(box.getCenter());
        Core::Vector3f size(box.getSize());
        float boxSize = 0.5f * std::max(size.x, std::max(size.y, size.z));
        float centerDist = (_position - center).getLengthSafe();
        float approxDist = std::max(0.0f, centerDist - boxSize);
        coeff =  _luminosity * std::max(0.0f, 1.0f - (approxDist * _invRadius));
    }

    return PointLightInfluence(coeff, this);
}
//------------------------------------------------------------------------------
}
