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
#ifndef RENDERER_WATERPATCH_H_
#define RENDERER_WATERPATCH_H_

#include <Renderer/IWaterPatch.h>

namespace Renderer
{
class LM_API_RDR WaterPatch : public Core::Object, public IWaterPatch
{
public:
    WaterPatch(float width, float height);

    virtual void getLayer(int32 id, WaterLayer & layer) const;
    virtual void setLayer(int32 id, const WaterLayer & layer);

    virtual float getWidth() const { return _width; };
    virtual float getHeight() const { return _height; };
    virtual void setSize(float width, float height);

    virtual void setDiffuse(const Core::Vector4f & color) { _diffuse = color; };
    virtual void setSpecular(const Core::Vector4f & color) { _specular = color; };
    virtual void setShininess(float shininess) { _shininess = shininess; };

    virtual void setDiffuseTexture(const Ptr<ITextureMap> & pTexture) { _pDiffuseTex = pTexture; }
    virtual Ptr<ITextureMap> getDiffuseTexture() const { return _pDiffuseTex; }

    virtual void setReflectionScale(float scale) { _reflScale = scale; };
    virtual void setRefractionScale(float scale) { _refrScale = scale; };
    virtual void setFresnelMin(float minReflection) { _fresnelMin = minReflection; };
    virtual void setFresnelPower(float power) { _fresnelPower = power; };

    virtual void setRefractive(bool refractive) { _isRefractive = refractive; };
    virtual void setReflective(bool reflective) { _isReflective = reflective; };


    virtual const Core::Vector4f & getDiffuse() const { return _diffuse; };
    virtual const Core::Vector4f & getSpecular() const { return _specular; };
    virtual float getShininess() const { return _shininess; };

    virtual float getReflectionScale() const { return _reflScale; };
    virtual float getRefractionScale() const { return _refrScale; };
    virtual float getFresnelMin() const { return _fresnelMin; };
    virtual float getFresnelPower() const { return _fresnelPower; };

    virtual bool isRefractive() const { return _isRefractive; };
    virtual bool isReflective() const { return _isReflective; };

    virtual const Core::Vector2f & getPosition() const;
    virtual void setPosition(const Core::Vector2f & position);

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void setWaterLevel(float level);

protected:
    virtual void updateBoundingBox();

    Ptr<ITextureMap>    _pDiffuseTex;
    WaterLayer     _layers[WATER_LAYER_COUNT];
    Core::Vector4f _diffuse;
    Core::Vector4f _specular;
    Core::Vector2f _position;
    float          _shininess;
    float          _reflScale;
    float          _refrScale;
    float          _fresnelMin;
    float          _fresnelPower;
    float          _opacity;
    float          _width;
    float          _height;
    float          _waterLevel;
    Core::AABoxf   _box;
    bool           _isAlive;
    bool           _isVisible;
    bool           _isRefractive;
    bool           _isReflective;
};
}

#endif /* WATERPATCH_H_ */
