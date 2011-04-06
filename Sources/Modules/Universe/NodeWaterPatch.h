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
#ifndef UNIVERSE_NODEWATERPATCH_H_
#define UNIVERSE_NODEWATERPATCH_H_

#include <Universe/Node.h>
#include <Universe/RessourcePool.h>
#include <Renderer/IWaterPatch.h>

namespace Universe
{

class LM_API_UNI NodeWaterPatch : public Node
{
public:
    NodeWaterPatch( const Ptr<Renderer::IAABox> & pBox,
                    const Ptr<Renderer::IWaterPatch> & pWaterPatch,
                    const Ptr<RessourcePool> & pRessourcesPool,
                    World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_WATER_PATCH; };

    virtual void setLayerTexture(int32 id, const String & textureName);
    virtual void setLayerScaleU(int32 id, float scaleU);
    virtual void setLayerScaleV(int32 id, float scaleV);
    virtual void setLayerSpeedU(int32 id, float speedU);
    virtual void setLayerSpeedV(int32 id, float speedV);
    virtual void setLayerAmplitude(int32 id, float amplitude);

    virtual const String & getLayerTexture(int32 id) const;
    virtual float getLayerScaleU(int32 id) const;
    virtual float getLayerScaleV(int32 id) const;
    virtual float getLayerSpeedU(int32 id) const;
    virtual float getLayerSpeedV(int32 id) const;
    virtual float getLayerAmplitude(int32 id) const;

    virtual float getWidth() const;
    virtual float getHeight() const;
    virtual void setSize(float width, float height);

    virtual void setDiffuseTexture(const String & name);
    virtual const String & getDiffuseTexture() const;

    virtual void setDiffuse(const Core::Vector4f & color);
    virtual void setSpecular(const Core::Vector4f & color);
    virtual void setShininess(float shininess);

    virtual void setRefractive(bool refractive);
    virtual void setReflective(bool reflective);
    virtual void setReflectionScale(float scale);
    virtual void setRefractionScale(float scale);
    virtual void setFresnelMin(float minReflection);
    virtual void setFresnelPower(float power);

    virtual const Core::Vector4f & getDiffuse() const;
    virtual const Core::Vector4f & getSpecular() const;
    virtual float getShininess() const;

    virtual bool isRefractive() const;
    virtual bool isReflective() const;
    virtual float getReflectionScale() const;
    virtual float getRefractionScale() const;
    virtual float getFresnelMin() const;
    virtual float getFresnelPower() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);
            void exportNodeXML(const Ptr<Core::XMLNode> & pNode);

protected:
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual void onWorldMatrixUpdate();
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

private:
    Ptr<Renderer::IWaterPatch>  _pWaterPatch;
    Ptr<RessourcePool>          _pRessourcesPool;

    String      _textureNames[Renderer::WATER_LAYER_COUNT];
    String      _diffuseTexName;
};

}//namespace Universe

#endif/*UNIVERSE_NODEWATERPATCH_H_*/
