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
#ifndef UNIVERSE_NODEEMITTER_H_
#define UNIVERSE_NODEEMITTER_H_

#include <Universe/Node.h>
#include <Universe/Particles/IEmitterShape.h>
#include <Universe/Particles/IPartEffect.h>
#include <Universe/Particles/IPartRepresentation.h>
#include <Universe/RessourcePool.h>

namespace Universe
{

class LM_API_UNI NodeEmitter : public Node
{
public:
    NodeEmitter(const Ptr<Renderer::IAABox> & pBox,
                const Ptr<RessourcePool> & pResPool,
                const Ptr<IPartRepresentation> & pRepresentation, 
                const Ptr<IEmitterShape> & pShape, 
                int32 maxCount,
                World * pWorld);
    virtual ~NodeEmitter();

    virtual ENodeType getNodeType() const { return NODE_EMITTER; };

    void setMaxCount(int32 maxCount);
    void setPeriod(float minPeriod, float maxPeriod);
    void setTimeMul(float minTimeMul, float maxTimeMul);
    void setStartColor(const Core::Vector4f & color1, const Core::Vector4f & color2);
    void setStartWidth(float minWidth, float maxWidth);
    void setStartHeight(float minHeight, float maxHeight);
    void setStartAngle(float minAngle, float maxAngle);
    void setStartTimeKey(float minT, float maxT);
    void setBurstLifeTime(float minBurst, float maxBurst);
    inline void setSquareParticles(bool square) { _squareParticles = square; }
    inline void setStickyParticles(bool sticky) { _stickyParticles = sticky; }
    void setNormalizedSize(bool nsize);
    void setGenerationTime(double startTime, double endTime);
    void setColorFactor(float f);
    void setGlowFactor(float f);
    inline void killAtEnd(bool killAtEnd) { _killAtEnd = killAtEnd; }
    inline void burstStart(bool burstStart) { _burstStart = burstStart; }

    void setShape(const Ptr<IEmitterShape> & pShape);
    void addEffect(const Ptr<IPartEffect> & pEffect);
    void clearEffects();

    virtual void setZBias(float z) { _pRepresentation->setZBias(z); }
    virtual float getZBias() const { return _pRepresentation->getZBias(); }

    virtual void setFogFactor(float f) { _pRepresentation->setFogFactor(f); }
    virtual float getFogFactor() const { return _pRepresentation->getFogFactor(); }

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    Ptr<IPartRepresentation> getRepresentation() const;
    void setRepresentation(const Ptr<IPartRepresentation> & pRepresentation);

    int32 getMaxCount() const { return _maxCount; };
    void getPeriod(float & minPeriod, float & maxPeriod) const ;
    void getTimeMul(float & minTimeMul, float & maxTimeMul) const;
    void getStartColor(Core::Vector4f & color1, Core::Vector4f & color2) const;
    void getStartWidth(float & minWidth, float & maxWidth) const;
    void getStartHeight(float & minHeight, float & maxHeight) const;
    void getStartAngle(float & minAngle, float & maxAngle) const;
    void getStartTimeKey(float & minT, float & maxT);
    void getBurstLifeTime(float & minBurst, float & maxBurst);
    inline bool getSquareParticles() const { return _squareParticles; }
    inline bool getStickyParticles() const { return _stickyParticles; }
    bool getNormalizedSize() const { return _nsize; };
    void getGenerationTime(double & startTime, double & endTime) const;
    float getColorFactor() const;
    float getGlowFactor() const;
    inline bool killAtEnd() const { return _killAtEnd; }
    inline bool burstStart() const { return _burstStart; }

    virtual void restart(bool recursive = false);
    virtual void stop(bool recursive = false, float inTime = 0.0f);

    const Ptr<IEmitterShape> & getShape() const;
    const Core::List<Ptr<IPartEffect> > & getEffects() const;
    void removeEffects(EEffect);

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setNodeGlow(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeGlow() const;

    const Ptr<RessourcePool> & getResPool() const { return _pResPool; };

protected:
    void createPart(Particle & newPart);

    virtual void update(double elapsed);
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf & ray, float & distance);
    virtual void internalPrepareForPacking();

    Ptr<RessourcePool>            _pResPool;
    Ptr<IEmitterShape>            _pShape;
    Ptr<IPartRepresentation>      _pRepresentation;
    Core::List<Ptr<IPartEffect> > _effects;
    Core::List<Particle>          _particles;
    Core::List<Particle>          _newParticles;

    Core::Matrix4f _partsMatrix;

    int32   _maxCount;

    Core::Vector4f _color1;
    Core::Vector4f _color2;

    mutable Core::Vector4f _nodeColor;
    mutable Core::Vector4f _nodeGlow;

    float   _minPeriod;
    float   _maxPeriod;
    float   _periodRange;

    float   _minTimeMul;
    float   _maxTimeMul;
    float   _timeMulRange;

    float   _minWidth;
    float   _maxWidth;
    float   _widthRange;

    float   _minHeight;
    float   _maxHeight;
    float   _heightRange;

    float   _minAngle;
    float   _maxAngle;
    float   _angleRange;

    float   _minT;
    float   _maxT;
    float   _tRange;

    float   _minBurst;
    float   _maxBurst;
    float   _burstRange;

    bool    _squareParticles;
    bool    _stickyParticles;
    bool    _nsize;
    bool    _burstStart;

    double  _startTime;
    double  _endTime;
    double  _lifeTime;
    double  _curEndTime;
    bool    _killAtEnd;

    double  _nextGenDuration;
    int32   _partIndex;
};

}

#endif /* NODEEMITTER_H_ */
