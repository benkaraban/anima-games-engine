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
#ifndef RENDERER_INTERNALSETTINGS_H_
#define RENDERER_INTERNALSETTINGS_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>
#include <Renderer/Settings.h>

namespace Renderer
{
LM_ENUM_8(ERenderPass, PASS_DEBUG, PASS_LIGHTMAP_GENERATION, PASS_STATIC_SHADOW, PASS_DYNAMIC_SHADOW, PASS_REFLECTION, PASS_LIGHTING, PASS_WIREFRAME, PASS_GLOW);

class InternalFogSettings : public Core::Object
{
public:
    InternalFogSettings();
    InternalFogSettings(const FogSettings & settings, float nearPlane, float farPlane);

    const Core::Vector4f & getColor() const { return _color; };
    float getStart() const { return _start; };
    float getEnd() const { return _end; };
    float getInvRange() const { return _invRange; };
    float getSkyOpacity() const { return _skyOpacity; };

    Core::Vector4f _color;
    float          _start;
    float          _end;
    float          _invRange;
    float          _skyOpacity;
};

class InternalMainLightSettings : public Core::Object
{
public:
    InternalMainLightSettings();
    InternalMainLightSettings(const MainLightSettings & settings);

    const Core::Vector3f & getDirection() const { return _direction; };
    const Core::Vector4f & getAmbient() const { return _ambient; };
    const Core::Vector4f & getDiffuse0() const { return _diffuse0; };
    const Core::Vector4f & getDiffuse1() const { return _diffuse1; };
    const Core::Vector4f & getDiffuse2() const { return _diffuse2; };
    const Core::Vector4f & getSpecular() const { return _specular; };

    float    getGlobalFade() const { return _globalFade; };

    float    getMulIntensity() const { return _mulIntensity; };

    float    getStaticShadowBias() const { return _staticShadowBias; };
    float    getStaticShadowOpacity() const { return _staticShadowOpacity; };
    float    getStaticShadowDiffusion() const { return _staticShadowDiffusion; };

    float    getDynamicShadowOpacity() const { return _dynamicShadowOpacity; };
    float    getDynamicShadowMiddle() const { return _dynamicShadowMiddle; };
    float    getDynamicShadowMiddleBlend() const { return _dynamicShadowMiddleBlend; };
    float    getDynamicShadowEnd() const { return _dynamicShadowEnd; };
    float    getDynamicShadowEndBlend() const { return _dynamicShadowEndBlend; };

    float    getBloomThreshold() const { return _bloomThreshold; };
    float    getBloomFactor() const { return _bloomFactor; };

    float    getOcclusionContrast() const { return _occlusionContrast; };
    float    getOcclusionLuminosity() const { return _occlusionLuminosity; };

protected:
    Core::Vector3f _direction;
    Core::Vector4f _ambient;
    Core::Vector4f _diffuse0;
    Core::Vector4f _diffuse1;
    Core::Vector4f _diffuse2;
    Core::Vector4f _specular;
    float          _globalFade;
    float          _mulIntensity;
    float          _staticShadowBias;
    float          _staticShadowOpacity;
    float          _staticShadowEnd;
    float          _staticShadowEndBlend;
    float          _staticShadowDiffusion;
    float          _dynamicShadowMiddle;
    float          _dynamicShadowMiddleBlend;
    float          _dynamicShadowEnd;
    float          _dynamicShadowEndBlend;
    float          _dynamicShadowOpacity;
    float          _bloomThreshold;
    float          _bloomFactor;
    float          _occlusionContrast;
    float          _occlusionLuminosity;
};

}

#endif /* INTERNALSETTINGS_H_ */
