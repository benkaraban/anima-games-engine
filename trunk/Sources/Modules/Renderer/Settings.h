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
#ifndef RENDERER_SETTINGS_H_
#define RENDERER_SETTINGS_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>
#include <Renderer/ITexture.h>

namespace Renderer
{
LM_ENUM_3(ELightingModel,
          LIGHTING_MODEL_FOGGY,
          LIGHTING_MODEL_CLOUDY,
          LIGHTING_MODEL_CLEAR_SKY);

struct LM_API_RDR SkysphereSettings
{
    Ptr<ICubeTextureMap> pSkyBox;

    Ptr<ITextureMap> pTexture;
    float            horizon;
    Core::Vector4f   color;
    Core::Vector4f   glow;
    float            yaw;

    // Pour procedural
    bool             isProcedural;
    ELightingModel   model;
    Core::Vector4f   sunColor;
    Core::Vector4f   skyColor;
    float            intensity;

    float            skyRoof;
    float            sphericity;
    bool             isAtInfinity;

    SkysphereSettings();
    SkysphereSettings(const Ptr<ITextureMap> & pTexture, float horizon);

    bool operator == (const SkysphereSettings & s) const;
    bool operator != (const SkysphereSettings & s) const;
};

struct LM_API_RDR FogSettings
{
    FogSettings();
    FogSettings(const Core::Vector4f & color, float start, float end, float skyFogOpacity);

    Core::Vector4f  color;
    float           start;
    float           end;
    float           skyFogOpacity;
};

const int32 WIND_COUNT = 4;

struct LM_API_RDR WindSettings
{
    Core::Vector3f direction;
    float          waveLength;  // <=> période spatiale (sur quelle distance l'onde se répète)
    float          period;      // Période temporelle (sur quelle durée l'onde se répète)
    float          phase;       // Valeur -PI <= phase <= PI qui sert de seed. Permet de différencier des vents similaires
    float          intensity;

    WindSettings();
};

struct LM_API_RDR MainLightSettings
{
    // Global fade
    float           globalFade;

    // Lighting
    float           sunAngleToZenith;
    float           sunAngleToNorth;
    Core::Vector4f  ambient;
    Core::Vector4f  diffuse0;
    Core::Vector4f  diffuse1;
    Core::Vector4f  diffuse2;
    Core::Vector4f  specular;
    float           mulIntensity;

    // Static shadows
    float           staticShadowBias;
    float           staticShadowOpacity;
    float           staticShadowDiffusion;

    // Dynamic shadows
    float           dynamicShadowOpacity;
    float           dynamicShadowMiddle;
    float           dynamicShadowMiddleBlend;
    float           dynamicShadowEnd;
    float           dynamicShadowEndBlend;

    // Bloom
    float           bloomThreshold;
    float           bloomFactor;

    // Occlusion
    float           occlusionContrast;
    float           occlusionLuminosity;

    MainLightSettings();
    Core::Vector3f getLightDirection() const;

    bool operator == (const MainLightSettings & s) const;
    bool operator != (const MainLightSettings & s) const;
};

LM_ENUM_9(ERenderMode, 
          RDR_NORMAL,
          RDR_HQ_PREVIEW,
          RDR_DEBUG_SHADOW_MAP,
          RDR_DEBUG_UV,
          RDR_DEBUG_NORMAL,
          RDR_DEBUG_T,
          RDR_DEBUG_B,
          RDR_DEBUG_N,
          RDR_DEBUG_MIP);

LM_ENUM_3(EPolygonMode,
          POLY_SOLID,
          POLY_SOLID_WIREFRAME,
          POLY_WIREFRAME);

LM_ENUM_4(EShaderLevel,
          SHADER_NONE,
          SHADER_LOW,
          SHADER_MEDIUM,
          SHADER_HIGH);

LM_ENUM_4(EShadowLevel,
          SHADOW_NONE,
          SHADOW_LOW,
          SHADOW_MEDIUM,
          SHADOW_HIGH);

LM_ENUM_3(ETextureLevel,
          TEXTURE_LOW,
          TEXTURE_MEDIUM,
          TEXTURE_HIGH);

LM_ENUM_4(EReflecLevel,
          REFLEC_NONE,
          REFLEC_LOW,
          REFLEC_MEDIUM,
          REFLEC_HIGH);

LM_ENUM_4(ERefracLevel,
          REFRAC_NONE,
          REFRAC_LOW,
          REFRAC_MEDIUM,
          REFRAC_HIGH);

LM_ENUM_5(EGlobalLevel,
          GLOBAL_VERY_LOW,
          GLOBAL_LOW,
          GLOBAL_MEDIUM,
          GLOBAL_HIGH,
          GLOBAL_VERY_HIGH);

LM_ENUM_5(EFilterLevel,
          FILTER_BILINEAR,
          FILTER_TRILINEAR,
          FILTER_ANISO_2X,
          FILTER_ANISO_4X,
          FILTER_ANISO_8X);


LM_ENUM_4(EMSAALevel,
          MSAA_NONE,
          MSAA_2X,
          MSAA_4X,
          MSAA_8X);

struct LM_API_RDR Resolution
{
    Resolution(int32 w, int32 h) : width(w), height(h)
    {}

    int32   width;
    int32   height;

    bool operator == (const Resolution & r) const
    {
        return width == r.width && height == r.height;
    }

    bool operator < (const Resolution & r) const
    {
        if(width != r.width)
            return width < r.width;
        else
            return height < r.height;
    }
};

struct LM_API_RDR RendererSettings
{
    void setGlobalLevel(EGlobalLevel level);

    // Details -------------------------

    EShaderLevel    shaderLevel;
    EShadowLevel    shadowLevel;
    ETextureLevel   textureLevel;
    EReflecLevel    reflecLevel;
    ERefracLevel    refracLevel;
    EFilterLevel    filterLevel;
    EMSAALevel      msaaLevel;
    bool            postFXOn;
    bool            glowOn;

    bool            fullscreen;
    int             width;
    int             height;
    bool            vsync;

    RendererSettings();
    RendererSettings(EGlobalLevel level);

    bool operator == (const RendererSettings & rs) const;
    bool operator != (const RendererSettings & rs) const;
};

struct LM_API_RDR ConfigProfile
{
    ConfigProfile();

    float           textureVRAMRatio;
    EMSAALevel      msaaLevel;
    int32           minWidth;
    int32           minHeight;
};

struct LM_API_RDR DebugSettings
{
    ERenderMode   renderMode;
    EPolygonMode  polyMode;

    DebugSettings();
};

struct LM_API_RDR PostFX
{
    PostFX();

    Core::Vector2f  blurDir1;
    Core::Vector2f  blurDir2;

    Core::Vector2f  radialBlurCenter;
    float           radialBlurFactor;

    Core::Vector4f  fadeColor;
    float           saturation;
};

}

#endif /* SETTINGS_H_ */
