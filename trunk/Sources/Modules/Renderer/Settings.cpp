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
#include <Renderer/Settings.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
FogSettings::FogSettings() :
    color(0.0, 0.0, 0.0, 0.0),
    start(0.0),
    end(1000.0),
    skyFogOpacity(0.0)
{
}
//-----------------------------------------------------------------------------
FogSettings::FogSettings(const Core::Vector4f & color, float start, float end, float skyFogOpacity) :
    color(color),
    start(start),
    end(end),
    skyFogOpacity(skyFogOpacity)
{
}
//-----------------------------------------------------------------------------
WindSettings::WindSettings() :
    direction(1.0, 0.0, 0.0),
    waveLength(1.0),
    period(1.0),
    phase(0.0),
    intensity(0.0)
{
}
//-----------------------------------------------------------------------------
MainLightSettings::MainLightSettings() :
    globalFade(1.0f),
    sunAngleToZenith(0.25f * f_PI_DIV_2),
    sunAngleToNorth(0.125f * f_PI),
    ambient(0.05f, 0.05f, 0.05f, 0.05f),
    diffuse0(0.8f, 0.8f, 0.8f, 1.0f),
    diffuse1(0.0f, 0.0f, 0.0f, 1.0f),
    diffuse2(0.2f, 0.2f, 0.4f, 1.0f),
    specular(0.8f, 0.8f, 0.8f, 1.0f),
    mulIntensity(1.0f),
    staticShadowBias(0.001f),
    staticShadowOpacity(0.6f),
    staticShadowDiffusion(0.05f),
    dynamicShadowOpacity(0.6f),
    dynamicShadowMiddle(12.0f),
    dynamicShadowMiddleBlend(2.0f),
    dynamicShadowEnd(50.0f),
    dynamicShadowEndBlend(10.0f),
    bloomThreshold(0.0f),
    bloomFactor(0.0f),
    occlusionContrast(0.0f),
    occlusionLuminosity(0.0f)
{
}
//-----------------------------------------------------------------------------
Core::Vector3f MainLightSettings::getLightDirection() const
{
    float teta = f_PI_DIV_2 + sunAngleToNorth;
    float phi = sunAngleToZenith;

    float cosPhi, sinPhi;
    float cosTeta, sinTeta;

    Core::L_sincos(phi, &sinPhi, &cosPhi);
    Core::L_sincos(teta, &sinTeta, &cosTeta);

    float y = cosPhi;
    float x = sinPhi * cosTeta;
    float z = sinPhi * sinTeta;

    return Core::Vector3f(-x, -y, -z);
}
//-----------------------------------------------------------------------------
bool MainLightSettings::operator == (const MainLightSettings & s) const
{
    return (globalFade == s.globalFade &&
            sunAngleToZenith == s.sunAngleToZenith &&
            sunAngleToNorth == s.sunAngleToNorth &&
            ambient == s.ambient &&
            diffuse0 == s.diffuse0 &&
            diffuse1 == s.diffuse1 &&
            diffuse2 == s.diffuse2 &&
            specular == s.specular && 
            mulIntensity == s.mulIntensity &&
            staticShadowBias == s.staticShadowBias &&
            staticShadowOpacity == s.staticShadowOpacity &&
            staticShadowDiffusion == s.staticShadowDiffusion &&
            dynamicShadowOpacity == s.dynamicShadowOpacity &&
            dynamicShadowMiddle == s.dynamicShadowMiddle && 
            dynamicShadowMiddleBlend == s.dynamicShadowMiddleBlend &&
            dynamicShadowEnd == s.dynamicShadowEnd &&
            dynamicShadowEndBlend == s.dynamicShadowEndBlend &&
            bloomThreshold == s.bloomThreshold &&
            bloomFactor == s.bloomFactor &&
            occlusionContrast == s.occlusionContrast &&
            occlusionLuminosity == s.occlusionLuminosity);
}
//-----------------------------------------------------------------------------
bool MainLightSettings::operator != (const MainLightSettings & s) const
{
    return !(*this == s);
}
//-----------------------------------------------------------------------------
RendererSettings::RendererSettings()
{
    fullscreen = false;
    vsync = false;
    width = 1;
    height = 1;
    setGlobalLevel(GLOBAL_MEDIUM);
}
//-----------------------------------------------------------------------------
RendererSettings::RendererSettings(EGlobalLevel level)
{
    fullscreen = false;
    vsync = false;
    width = 1;
    height = 1;
    setGlobalLevel(level);
}
//-----------------------------------------------------------------------------
bool RendererSettings::operator == (const RendererSettings & rs) const
{
    return (shaderLevel == rs.shaderLevel &&
            shadowLevel == rs.shadowLevel &&
            textureLevel == rs.textureLevel &&
            reflecLevel == rs.reflecLevel &&
            refracLevel == rs.refracLevel &&
            filterLevel == rs.filterLevel &&
            msaaLevel == rs.msaaLevel &&
            glowOn == rs.glowOn &&
            postFXOn == rs.postFXOn &&
            fullscreen == rs.fullscreen &&
            width == rs.width &&
            height == rs.height &&
            vsync == rs.vsync);
}
//-----------------------------------------------------------------------------
bool RendererSettings::operator != (const RendererSettings & rs) const
{
    return !(*this == rs);
}
//-----------------------------------------------------------------------------
DebugSettings::DebugSettings() :
    renderMode(RDR_NORMAL),
    polyMode(POLY_SOLID)
{
}
//-----------------------------------------------------------------------------
void RendererSettings::setGlobalLevel(EGlobalLevel level)
{
    switch(level)
    {
    case GLOBAL_VERY_LOW:
        shaderLevel  = SHADER_NONE;
        shadowLevel  = SHADOW_NONE;
        textureLevel = TEXTURE_LOW;
        reflecLevel  = REFLEC_NONE;
        refracLevel  = REFRAC_NONE;
        filterLevel  = FILTER_BILINEAR;
        msaaLevel    = MSAA_NONE;
        glowOn       = false;
        postFXOn     = false;
        break;
    case GLOBAL_LOW:
        shaderLevel  = SHADER_LOW;
        shadowLevel  = SHADOW_NONE;
        textureLevel = TEXTURE_LOW;
        reflecLevel  = REFLEC_LOW;
        refracLevel  = REFRAC_LOW;
        filterLevel  = FILTER_TRILINEAR;
        msaaLevel    = MSAA_NONE;
        glowOn       = true;
        postFXOn     = true;
        break;
    case GLOBAL_MEDIUM:
        shaderLevel  = SHADER_MEDIUM;
        shadowLevel  = SHADOW_LOW;
        textureLevel = TEXTURE_MEDIUM;
        reflecLevel  = REFLEC_MEDIUM;
        filterLevel  = FILTER_ANISO_2X;
        refracLevel  = REFRAC_MEDIUM;
        msaaLevel    = MSAA_NONE;
        glowOn       = true;
        postFXOn     = true;
        break;
    case GLOBAL_HIGH:
        shaderLevel  = SHADER_HIGH;
        shadowLevel  = SHADOW_MEDIUM;
        textureLevel = TEXTURE_HIGH;
        reflecLevel  = REFLEC_HIGH;
        refracLevel  = REFRAC_MEDIUM;
        filterLevel  = FILTER_ANISO_4X;
        msaaLevel    = MSAA_2X;
        glowOn       = true;
        postFXOn     = true;
        break;
    case GLOBAL_VERY_HIGH:
        shaderLevel  = SHADER_HIGH;
        shadowLevel  = SHADOW_HIGH;
        textureLevel = TEXTURE_HIGH;
        reflecLevel  = REFLEC_HIGH;
        refracLevel  = REFRAC_HIGH;
        filterLevel  = FILTER_ANISO_8X;
        msaaLevel    = MSAA_4X;
        glowOn       = true;
        postFXOn     = true;
        break;
    };
}
//-----------------------------------------------------------------------------
SkysphereSettings::SkysphereSettings() : 
    horizon(0.0), 
    color(1.0, 1.0, 1.0, 1.0), 
    glow(0.0, 0.0, 0.0, 0.0), 
    yaw(0.0),
    isProcedural(false),
    model(LIGHTING_MODEL_CLEAR_SKY),
    sunColor(1.0f, 1.0f, 1.0f, 1.0f),
    skyColor(0.25f, 0.25f, 0.5f, 1.0f),
    intensity(1.0f),
    skyRoof(1000.0f),
    sphericity(1.0f),
    isAtInfinity(true)
{};
//-----------------------------------------------------------------------------
bool SkysphereSettings::operator != (const SkysphereSettings & s) const
{
    return !(*this == s);
}
//-----------------------------------------------------------------------------
bool SkysphereSettings::operator == (const SkysphereSettings & s) const
{
    return (pSkyBox == s.pSkyBox &&
            pTexture == s.pTexture &&
            horizon == s.horizon && 
            color == s.color &&
            glow == s.glow &&
            yaw == s.yaw &&
            isProcedural == s.isProcedural &&
            model == s.model &&
            sunColor == s.sunColor &&
            skyColor == s.skyColor &&
            intensity == s.intensity &&
            skyRoof == s.skyRoof &&
            sphericity == s.sphericity &&
            isAtInfinity == s.isAtInfinity);
}
//-----------------------------------------------------------------------------
SkysphereSettings::SkysphereSettings(const Ptr<ITextureMap> & pTexture,
                                     float horizon) :
    pTexture(pTexture), 
    horizon(horizon), 
    color(1.0, 1.0, 1.0, 1.0), 
    glow(0.0, 0.0, 0.0, 0.0), 
    yaw(0.0), 
    isProcedural(false),
    model(LIGHTING_MODEL_CLEAR_SKY),
    sunColor(1.0f, 1.0f, 1.0f, 1.0f),
    skyColor(0.25f, 0.25f, 0.5f, 1.0f),
    intensity(1.0f),
    skyRoof(200.0f),
    sphericity(1.0f),
    isAtInfinity(true)
{};
//-----------------------------------------------------------------------------
ConfigProfile::ConfigProfile() : textureVRAMRatio(0.5f), msaaLevel(MSAA_NONE), minWidth(0), minHeight(0)
{};
//-----------------------------------------------------------------------------
PostFX::PostFX() :
    blurDir1(0.0f, 0.0f),
    blurDir2(0.0f, 0.0f),
    radialBlurCenter(0.5f, 0.5f),
    radialBlurFactor(0.0f),
    fadeColor(Core::Vector4f::ZERO),
    saturation(1.0f)
{
}
//-----------------------------------------------------------------------------
}
