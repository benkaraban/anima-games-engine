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
#ifndef RENDERER_IWATERPATCH_H_
#define RENDERER_IWATERPATCH_H_

#include <Core/Math/Matrix.h>
#include <Core/Math/GeometryComputations.h>
#include <Renderer/ITexture.h>

namespace Renderer
{
struct LM_API_RDR WaterLayer
{
    Ptr<ITextureMap>  pNormalMap; // La texture utilisée pour ce motif
    float             scaleU;     // La largeur du motif, en mètres
    float             scaleV;     // La longueur du motif, en mètres
    float             speedU;     // La vitesse de déplacement du motif dans le sens de la largeur, en mètres par seconde
    float             speedV;     // La vitesse de déplacement du motif dans le sens de la largeur, en mètres par seconde
    float             amplitude;  // Le coefficient du motif

    WaterLayer()
    : scaleU(1.0), scaleV(1.0), speedU(0.0), speedV(0.0), amplitude(1.0)
    {};
};

const int32 WATER_LAYER_COUNT = 4;

class LM_API_RDR IWaterPatch
{
public:
    virtual ~IWaterPatch() {};

    // Génération des vaguelettes
    virtual void getLayer(int32 id, WaterLayer & layer) const = 0;
    virtual void setLayer(int32 id, const WaterLayer & layer) = 0;

    virtual float getWidth() const = 0;
    virtual float getHeight() const = 0;
    virtual void setSize(float width, float height) = 0;

    virtual void setDiffuse(const Core::Vector4f & color) = 0;
    virtual void setSpecular(const Core::Vector4f & color) = 0;
    virtual void setShininess(float shininess) = 0;

    virtual void setDiffuseTexture(const Ptr<ITextureMap> & pTexture) = 0;
    virtual Ptr<ITextureMap> getDiffuseTexture() const = 0;

    virtual void setReflectionScale(float scale) = 0;
    virtual void setRefractionScale(float scale) = 0;
    virtual void setFresnelMin(float minReflection) = 0;
    virtual void setFresnelPower(float power) = 0;

    virtual void setRefractive(bool refractive) = 0;
    virtual void setReflective(bool reflective) = 0;


    virtual const Core::Vector4f & getDiffuse() const = 0;
    virtual const Core::Vector4f & getSpecular() const = 0;
    virtual float getShininess() const = 0;

    virtual float getReflectionScale() const = 0;
    virtual float getRefractionScale() const = 0;
    virtual float getFresnelMin() const = 0;
    virtual float getFresnelPower() const = 0;

    virtual bool isRefractive() const = 0;
    virtual bool isReflective() const = 0;

    virtual const Core::Vector2f & getPosition() const = 0;
    virtual void setPosition(const Core::Vector2f & position) = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};
}

#endif /* IWATERPATCH_H_ */
