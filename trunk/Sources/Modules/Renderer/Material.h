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
#ifndef RENDERER_MATERIAL_H_
#define RENDERER_MATERIAL_H_

#include <Core/Math/Vector.h>
#include <Assets/Material.h>
#include <Renderer/ITexture.h>

namespace Renderer
{
struct LM_API_RDR Material
{
    Material();
    Material(const Assets::Material & mat);
    Material(const Renderer::Material & mat);

    Assets::EMaterialType   matType;

    // Standard material ------------------------------------------------------

    Ptr<ITextureMap>   pDiffuseMap;
    Ptr<ITextureMap>   pSpecularMap;
    Ptr<ITextureMap>   pNormalMap;
    Ptr<ITextureMap>   pLightMap;
    Ptr<ITextureMap>   pOcclusionMap;

    Core::Vector4f     emissive;
    Core::Vector4f     ambient;
    Core::Vector4f     diffuse;

    Core::Vector4f     specular;
    float              shininess;

    Core::Vector4f     glowColor;
    Core::Vector3f     glowExtent;

    Core::Vector4f     reflexions;
    Core::Vector4f     refracColor;
    float              refracScale;
    float              refracIndex;

    // Custom material --------------------------------------------------------

    Ptr<ITextureMap>        pCustomMap0;
    Ptr<ITextureMap>        pCustomMap1;
    Ptr<ITextureMap>        pCustomMap2;

    Assets::ECustomMode     customMode;
    Core::Vector4f          customColor;
    float                   colorFactor;
    float                   glowFactor;
    Assets::EOperationType  opColor01;
    Assets::EOperationType  opColor12;
    Core::Vector2f          duv0;
    Core::Vector2f          duv1;
    Core::Vector2f          duv2;


    // Flags ------------------------------------------------------------------

    // Si vrai -> normales dans l'espace objet
    // Si faux -> normales dans l'espace tangentiel
    bool               transDiffuseTex;
    bool               clampTextureBorder; // Si false => wrap
    bool               internalTextureBorder;

    float              dynamicShadowOpacity;
    bool               dynamicShadowCaster;
    bool               dynamicShadowReceiver;

    bool               staticLighting;

    bool               invertedCulling;

    float              zBias;
};
}

#endif /* MATERIAL_H_ */
