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
#ifndef ABSTRACT_IDECAL_H_
#define ABSTRACT_IDECAL_H_

#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/GeometryComputations.h>
#include <Renderer/ITexture.h>

namespace Renderer
{
struct DecalVertex
{
    Core::Vector3f position; // 12 - 12
    float  texcoords[3];     // 12 - 24
    float  zbias;            //  4 - 28
    byte   color[4];         //  4 - 32
};

LM_STATIC_ASSERT(sizeof(DecalVertex) == 32);

LM_ENUM_2(EDecalNormal, 
          DECAL_NORMAL_Z,
          DECAL_NORMAL_Y);

LM_ENUM_3(EDecalMode,
        DECAL_LERP,
        DECAL_ADD,
        DECAL_REFRAC);

/**
 * Cette classe représente un decal, un rectangle généralement texturé.
 * Ce rectangle est positionnné en trois dimensions et est géré exactement
 * comme n'importe quel objet 3D contrairement aux billboards qui font toujours
 * face à la caméra.
 *
 * Les decals peuvent être utilisés pour afficher des impacts de balles sur un mur,
 * un pentacle qui tourne autour d'un personnage, etc... Les decals peuvent aussi
 * être utilisés pour afficher de simples rectangles dans l'IHM.
 */
class LM_API_RDR IDecal
{
public:
    virtual ~IDecal() {};

    /**
     * Modifie la matrice de transformation du décal.
     */
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;
    virtual void getWorldMatrix(Core::Matrix4f & matrix) const = 0;

    virtual void setSize(float width, float height) = 0;
    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;

    virtual void setMode(EDecalMode mode) = 0;
    virtual EDecalMode getMode() const = 0;

    /**
     * Modifie à la fois la largeur et la hauteur pour un décal carré.
     */
    virtual void setSize(float size) = 0;

    virtual void setDecalNormal(EDecalNormal n) = 0;
    virtual EDecalNormal getDecalNormal() const  = 0;

    /**
     * Pour un decal en mode DECAL_REFRAC, l'alpha représente l'intensité de la réfraction
     */
    virtual void setColor(const Core::Vector4f & color) = 0;
    virtual const Core::Vector4f & getColor() const = 0;

    virtual void setGlow(const Core::Vector4f & color) = 0;
    virtual const Core::Vector4f & getGlow() const = 0;

    virtual void setTexture(const Ptr<ITextureMap> & color) = 0;
    virtual Ptr<ITextureMap> getTexture() const = 0;

    virtual void setZBias(float z) = 0;
    virtual float getZBias() const = 0;

    virtual void setFogFactor(float f) = 0;
    virtual float getFogFactor() const = 0;

    /**
     * Précise que la texture utilisée est un tableau de sprites contenant
     * uCount sprites en largeurs * vCount sprites en hauteur.
     */
    virtual void setSpriteLayout(int32 uCount, int32 vCount) = 0;
    virtual int32 getUSpriteLayout() const = 0;
    virtual int32 getVSpriteLayout() const = 0;

    /**
     * Pour qu'un billboard soit animé il faut lui assigner une texture animée
     * ou lui fournir une texture contenant un tableau de sprites (cf. setSpriteLayout).
     */
    virtual bool isAnimated() const = 0;
    virtual bool hasSpriteLayout() const = 0;

    virtual void setTimeKey(float k) = 0;
    virtual float getTimeKey() const = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;


    // PACKAGE

    virtual void getVertices(DecalVertex pVertices [], bool glowColor) const = 0;
};
}

#endif /*ABSTRACT_IDECAL_H_*/
