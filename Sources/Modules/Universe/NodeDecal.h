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
#ifndef UNIVERSE_NODEDECAL_H_
#define UNIVERSE_NODEDECAL_H_

#include <Universe/Node.h>
#include <Universe/RessourcePool.h>
#include <Renderer/IDecal.h>

namespace Universe
{

class LM_API_UNI NodeDecal : public Node
{
public:
    NodeDecal(const Ptr<Renderer::IAABox> & pBox,
              const Ptr<Renderer::IDecal> & pDecal,
              const Ptr<RessourcePool> & pRessourcesPool, World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_DECAL; };

    virtual void setTexture(const String & textureName);
    virtual inline const String & getTexture() const { return _textureName; }

    virtual void setSize(float width, float height);
    virtual float getWidth() const;
    virtual float getHeight() const;

    virtual void setBillboard(bool b);
    virtual bool isBillboard() const;

    virtual void setDecalNormal(Renderer::EDecalNormal n);
    virtual Renderer::EDecalNormal getDecalNormal() const;

    virtual void setMode(Renderer::EDecalMode mode);
    virtual Renderer::EDecalMode getMode() const;

    virtual void setZBias(float z) { _pDecal->setZBias(z); }
    virtual float getZBias() const { return _pDecal->getZBias(); }

    virtual void setFogFactor(float f) { _pDecal->setFogFactor(f); }
    virtual float getFogFactor() const { return _pDecal->getFogFactor(); }

    /**
     * Modifie à la fois la largeur et la hauteur pour un décal carré.
     */
    virtual void setSize(float size);

    virtual void setColor(const Core::Vector4f & color);
    virtual const Core::Vector4f & getColor() const;

    virtual void setGlow(const Core::Vector4f & color);
    virtual const Core::Vector4f & getGlow() const;

    virtual void setSpriteLayout(int32 uCount, int32 vCount) { _pDecal->setSpriteLayout(uCount, vCount); };
    virtual int32 getUSpriteLayout() const { return _pDecal->getUSpriteLayout(); };
    virtual int32 getVSpriteLayout() const { return _pDecal->getVSpriteLayout(); };

    virtual bool hasSpriteLayout() const { return _pDecal->hasSpriteLayout(); };

    virtual void animateSprite(double duration, float keyFrom = 0.0, float keyTo = 1.0, bool loop = false);
    virtual double getDuration() const;
    virtual float getKeyFrom() const;
    virtual float getKeyTo() const;
    virtual bool getLoop() const;

    virtual void setTimeKey(float k);
    virtual float getTimeKey() const;

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setNodeGlow(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeGlow() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void update(double elapsed);

    virtual void prepareForRendering(const Ptr<NodeCamera> & pCamera);

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    Ptr<Renderer::IDecal> _pDecal;
    Ptr<RessourcePool>    _pRessourcesPool;
    String                _textureName;
    Core::Matrix4f        _decalOrientMat;
    bool                  _billboardOn;

    bool                  _animOn;
    bool                  _animLoop;
    float                 _animKeyFrom;
    float                 _animKeyTo;
    double                _animDuration;
};
}

#endif /* NODEDECAL_H_ */
