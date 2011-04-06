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
#ifndef NODEPOINTLIGHT_H_
#define NODEPOINTLIGHT_H_

#include <Universe/Node.h>
#include <Renderer/ILight.h>

namespace Universe
{
class LM_API_UNI NodePointLight : public Node
{
public:
    NodePointLight(const Ptr<Renderer::IAABox> & pBox,
                   const Ptr<Renderer::IPointLight> & pLight,
                   World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_POINT_LIGHT; };
    
    virtual void setShadowCasting(bool active) { _pLight->setShadowCasting(active); };
    virtual void setAmbientColor(const Core::Vector4f & color);
    virtual void setDiffuseColor(const Core::Vector4f & color);
    virtual void setSpecularColor(const Core::Vector4f & color);
    virtual void setRadius(float radius);
    virtual void setStatic(bool s) { _isStatic = s; };

    virtual bool getShadowCasting() const { return _pLight->getShadowCasting(); };
    virtual const Core::Vector4f & getAmbientColor() const { return _ambient; };
    virtual const Core::Vector4f & getDiffuseColor() const { return _diffuse; };
    virtual const Core::Vector4f & getSpecularColor() const { return _specular; };
    virtual float getRadius() const { return _pLight->getRadius(); };
    virtual bool isStatic() const { return _isStatic; };

    virtual void setTurnOnLatency(float l) { _turnOnLatency = l; };
    virtual float getTurnOnLatency() const { return _turnOnLatency; };

    virtual void setTurnOffLatency(float l) { _turnOffLatency = l; };
    virtual float getTurnOffLatency() const { return _turnOffLatency; };

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setBurst(bool b) { _burst = b; };
    virtual bool getBurst() const { return _burst; };

    virtual void restart(bool recursive = false);
    virtual void stop(bool recursive = false, float inTime = 0.0f);

    virtual void update(double elapsed);

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onVisibilityChange(bool flag);
    virtual void onKill();
    virtual void updateLight();
    virtual void updateLifeTime(double elapsed);
    virtual void internalPrepareForPacking();

    Ptr<Renderer::IPointLight>  _pLight;
    Core::Vector4f  _mainColor;
    Core::Vector4f  _mainGlow;
    Core::Vector4f  _ambient;
    Core::Vector4f  _diffuse;
    Core::Vector4f  _specular;
    float           _radius;
    float           _turnOnLatency;
    float           _turnOffLatency;
    bool            _burst;
    bool            _isStatic;

    bool            _stopProgrammed;
    bool            _colorChangeInProgress;
    Core::Vector4f  _color1;
    Core::Vector4f  _color2;
    float           _elapsed;
    float           _colorChangeTime;
    float           _remainingLifeTime;
};

}

#endif
