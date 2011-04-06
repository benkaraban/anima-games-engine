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
#ifndef RENDERER_LIGHT_H_
#define RENDERER_LIGHT_H_

#include <Core/Math/GeometryComputations.h>
#include <Renderer/ILight.h>

namespace Renderer
{
class PointLight;

struct LM_API_RDR PointLightInfluence
{
    float        coeff;
    PointLight * pLight;

    PointLightInfluence();
    PointLightInfluence(float coeff, PointLight * pLight);
};

class LM_API_RDR PointLight : public Core::Object, public IPointLight
{
public:
    PointLight();

    virtual void setShadowCasting(bool active) { _shadowCast = active; };
    virtual void setAmbientColor(const Core::Vector4f & color);
    virtual void setDiffuseColor(const Core::Vector4f & color);
    virtual void setSpecularColor(const Core::Vector4f & color);

    virtual bool getShadowCasting() const { return _shadowCast; };
    virtual const Core::Vector4f & getAmbientColor() const { return _ambient; };
    virtual const Core::Vector4f & getDiffuseColor() const { return _diffuse; };
    virtual const Core::Vector4f & getSpecularColor() const { return _specular; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void setPosition(const Core::Vector3f & position) { _position = position; };
    virtual void setRadius(float radius);

    virtual const Core::Vector3f & getPosition() const { return _position; };
    virtual float getRadius() const { return _radius; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    // PACKAGE

    virtual PointLightInfluence getInfluence(const Core::AABoxf & box);
    virtual float getInvRadius() const { return _invRadius; };

protected:
    Core::Vector4f    _ambient;
    Core::Vector4f    _diffuse;
    Core::Vector4f    _specular;
    Core::Vector3f    _position;
    float             _radius;
    float             _invRadius;

    float             _ambientLum;
    float             _diffuseLum;
    float             _specularLum;
    float             _luminosity;

    bool              _shadowCast;
    bool              _isVisible;
    bool              _isAlive;

    void updateLuminosity();
};
}

#endif
