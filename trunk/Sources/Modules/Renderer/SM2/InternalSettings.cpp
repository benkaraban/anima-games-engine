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
#include <Renderer/SM2/InternalSettings.h>
#include <Core/Logger.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
const float FOG_INV_RANGE_MAX = 1000.0;
//-----------------------------------------------------------------------------
InternalFogSettings::InternalFogSettings() :
    _color(1.0, 1.0, 1.0, 1.0),
    _start(1.0),
    _end(1.0),
    _invRange(FOG_INV_RANGE_MAX),
    _skyOpacity(0.0)
{}
//-----------------------------------------------------------------------------
InternalFogSettings::InternalFogSettings(const FogSettings & settings, float nearPlane, float farPlane) :
    _color(settings.color),
    _start(settings.start),
    _end(settings.end),
    _invRange(FOG_INV_RANGE_MAX),
    _skyOpacity(settings.skyFogOpacity)
{
    float lrange = _end - _start;
    if(lrange != 0.0)
        _invRange = float(1.0f / lrange);
}
//-----------------------------------------------------------------------------
InternalMainLightSettings::InternalMainLightSettings() :
    _direction(0.0f, -1.0f, 0.0f),
    _ambient(0.2f, 0.2f, 0.2f, 0.2f),
    _diffuse0(1.0f, 1.0f, 1.0f, 1.0f),
    _diffuse1(1.0f, 1.0f, 1.0f, 1.0f),
    _diffuse2(1.0f, 1.0f, 1.0f, 1.0f),
    _specular(1.0f, 1.0f, 1.0f, 1.0f),
    _globalFade(1.0f),
    _mulIntensity(1.0f),
    _staticShadowBias(0.001f),
    _staticShadowOpacity(0.6f),
    _staticShadowEnd(100.0f),
    _staticShadowEndBlend(50.0f),
    _staticShadowDiffusion(0.5f),
    _dynamicShadowMiddle(10.f),
    _dynamicShadowMiddleBlend(1.0f),
    _dynamicShadowEnd(50.0f),
    _dynamicShadowEndBlend(10.0f),
    _dynamicShadowOpacity(1.0f),
    _bloomThreshold(0.0f),
    _bloomFactor(0.0f),
    _occlusionContrast(0.0f),
    _occlusionLuminosity(0.0f)
{
}
//-----------------------------------------------------------------------------
InternalMainLightSettings::InternalMainLightSettings(const MainLightSettings & settings) :
    _direction(settings.getLightDirection()),
    _ambient(settings.ambient),
    _diffuse0(settings.diffuse0),
    _diffuse1(settings.diffuse1),
    _diffuse2(settings.diffuse2),
    _specular(settings.specular),
    _globalFade(settings.globalFade),
    _mulIntensity(settings.mulIntensity),
    _staticShadowBias(settings.staticShadowBias),
    _staticShadowOpacity(settings.staticShadowOpacity),
    _staticShadowDiffusion(settings.staticShadowDiffusion),
    _dynamicShadowMiddle(settings.dynamicShadowMiddle),
    _dynamicShadowMiddleBlend(settings.dynamicShadowMiddleBlend),
    _dynamicShadowEnd(settings.dynamicShadowEnd),
    _dynamicShadowEndBlend(settings.dynamicShadowEndBlend),
    _dynamicShadowOpacity(settings.dynamicShadowOpacity),
    _bloomThreshold(settings.bloomThreshold),
    _bloomFactor(settings.bloomFactor),
    _occlusionContrast(settings.occlusionContrast),
    _occlusionLuminosity(settings.occlusionLuminosity)
{
}
//-----------------------------------------------------------------------------
}
