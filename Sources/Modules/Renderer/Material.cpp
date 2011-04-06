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
#include <Renderer/Material.h>

namespace Renderer
{

Material::Material() :
    matType(Assets::MAT_STANDARD),
    pDiffuseMap(null),
    pSpecularMap(null),
    pNormalMap(null),
    pLightMap(null),
    pOcclusionMap(null),
    emissive(0.0, 0.0, 0.0, 0.0),
    ambient(1.0, 1.0, 1.0, 1.0),
    diffuse(1.0, 1.0, 1.0, 1.0),
    specular(1.0, 1.0, 1.0, 1.0),
    shininess(64.0),
    glowColor(0.0, 0.0, 0.0, 0.0),
    glowExtent(0.0, 0.0, 0.0),
    reflexions(0.0, 0.0, 0.0, 0.0),
    refracColor(0.0, 0.0, 0.0, 0.0),
    refracScale(1.0f),
    refracIndex(0.7f),
    pCustomMap0(null),
    pCustomMap1(null),
    pCustomMap2(null),
    customMode(Assets::CM_ADD),
    customColor(1.0f),
    colorFactor(1.0f),
    glowFactor(0.0f),
    opColor01(Assets::OP_MUL),
    opColor12(Assets::OP_MUL),
    duv0(0.0f, 0.0f),
    duv1(0.0f, 0.0f),
    duv2(0.0f, 0.0f),
    transDiffuseTex(false),
    clampTextureBorder(false),
    internalTextureBorder(false),
    dynamicShadowOpacity(1.0f),
    dynamicShadowCaster(true),
    dynamicShadowReceiver(false),
    staticLighting(false),
    invertedCulling(false),
    zBias(0.0)
{}

Material::Material(const Assets::Material & mat) :
    matType(mat.type),
    pDiffuseMap(null),
    pSpecularMap(null),
    pNormalMap(null),
    pLightMap(null),
    pOcclusionMap(null),
    emissive(mat.emissive),
    ambient(mat.ambient),
    diffuse(mat.diffuse),
    specular(mat.specular),
    shininess(mat.shininess),
    glowColor(mat.glowColor),
    glowExtent(mat.glowExtent),
    reflexions(mat.reflexions),
    refracColor(mat.refracColor),
    refracScale(mat.refracScale),
    refracIndex(mat.refracIndex),
    pCustomMap0(null),
    pCustomMap1(null),
    pCustomMap2(null),
    customMode(mat.customMode),
    customColor(mat.customColor),
    colorFactor(mat.colorFactor),
    glowFactor(mat.glowFactor),
    opColor01(mat.opColor01),
    opColor12(mat.opColor12),
    duv0(mat.duv0),
    duv1(mat.duv1),
    duv2(mat.duv2),
    transDiffuseTex( (mat.flags & Assets::MAT_FLAG_TRANSLUCID_TEXTURE) != 0),
    clampTextureBorder( (mat.flags & Assets::MAT_FLAG_CLAMPED_TEXTURE_BORDER) != 0),
    internalTextureBorder(false),
    dynamicShadowOpacity(1.0f),
    dynamicShadowCaster(true),
    dynamicShadowReceiver(false),
    staticLighting(false),
    invertedCulling(false),
    zBias(mat.zBias)
{
    dynamicShadowCaster   = (mat.flags & Assets::MAT_FLAG_DYNAMIC_SHADOW_CASTER) != 0;
    dynamicShadowReceiver = (mat.flags & Assets::MAT_FLAG_DYNAMIC_SHADOW_RECEIVER) != 0;
}

Material::Material(const Renderer::Material & mat) :
    matType(mat.matType),
    pDiffuseMap(mat.pDiffuseMap),
    pSpecularMap(mat.pSpecularMap),
    pNormalMap(mat.pNormalMap),
    pLightMap(mat.pLightMap),
    pOcclusionMap(mat.pOcclusionMap),
    emissive(mat.emissive),
    ambient(mat.ambient),
    diffuse(mat.diffuse),
    specular(mat.specular),
    shininess(mat.shininess),
    glowColor(mat.glowColor),
    glowExtent(mat.glowExtent),
    reflexions(mat.reflexions),
    refracColor(mat.refracColor),
    refracScale(mat.refracScale),
    refracIndex(mat.refracIndex),
    pCustomMap0(mat.pCustomMap0),
    pCustomMap1(mat.pCustomMap1),
    pCustomMap2(mat.pCustomMap2),
    customMode(mat.customMode),
    customColor(mat.customColor),
    colorFactor(mat.colorFactor),
    glowFactor(mat.glowFactor),
    opColor01(mat.opColor01),
    opColor12(mat.opColor12),
    duv0(mat.duv0),
    duv1(mat.duv1),
    duv2(mat.duv2),
    transDiffuseTex(mat.transDiffuseTex),
    clampTextureBorder(mat.clampTextureBorder),
    internalTextureBorder(mat.internalTextureBorder),
    dynamicShadowOpacity(1.0f),
    dynamicShadowCaster(mat.dynamicShadowCaster),
    dynamicShadowReceiver(mat.dynamicShadowReceiver),
    staticLighting(mat.staticLighting),
    invertedCulling(mat.invertedCulling),
    zBias(mat.zBias)
{
}

}
