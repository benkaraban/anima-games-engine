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
#ifndef UNIVERSE_NODESTORM_H_
#define UNIVERSE_NODESTORM_H_

#include <Universe/RessourcePool.h>
#include <Universe/Node.h>
#include <Universe/NodeCamera.h>
#include <Universe/RessourcePool.h>

namespace Universe
{

class LM_API_UNI NodeStorm : public Node
{
public:
    NodeStorm(const Ptr<Renderer::IAABox> & pBox,
              const Ptr<RessourcePool> & pResPool,
              const Ptr<Renderer::IScene> & pScene,
              int32 maxCount,
              World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_STORM; };

    // texture
    // mode

    void setMaxCount(int32 maxCount);
    void setPeriod(float minPeriod, float maxPeriod);
    void setBurstStart(bool b) { _burstStart = b; }

    void setEntropy(float e) { _entropy = e; }
    void setFertility(int32 f) { _fertility = f; }
    void setResolution(int32 r);

    void setTexture(const String & texture);
    void setColor(const Core::Vector4f & color1, const Core::Vector4f & color2) { _color1 = color1; _color2 = color2; }
    void setGlow(const Core::Vector4f & glow1, const Core::Vector4f & glow2) { _glow1 = glow1; _glow2 = glow2; }
    
    void setFadeInTime(float t) { _fadeIn = t; }
    void setDuration(float t) { _duration = t; }
    void setFadeOutTime(float t) { _fadeOut = t; }
    
    void setHeadWidth(float w) { _headWidth = w; }
    void setTailWidth(float w) { _tailWidth = w; }

    void setHeadSphereCenter(const Core::Vector3f & v) { _headCenter = v; }
    void setHeadSphereSize(const Core::Vector3f & v)   { _headSize = v; }
    void setTailSphereCenter(const Core::Vector3f & v) { _tailCenter = v; }
    void setTailSphereSize(const Core::Vector3f & v)   { _tailSize = v; }
    void setTailNode(const Ptr<Node> & pNode) { _pTailNode = pNode; }

    void setGenerationTime(double startTime, double endTime) { _startTime = startTime; _endTime = endTime; }
    void setTravelTime(float travelTime) { _travelTime = travelTime; }

    void killAtEnd(bool killAtEnd) { _killAtEnd = killAtEnd; }


    int32 getMaxCount() const { return _maxCount; }
    void getPeriod(float & minPeriod, float & maxPeriod) const { minPeriod = _minPeriod; maxPeriod = _maxPeriod; }
    bool getBurstStart() const { return _burstStart; }
    float getEntropy() const { return _entropy; }
    int32 getFertility() const { return _fertility; }
    int32 getResolution() const { return _resolution; }
    const String & getTexture() const { return _texture; };
    void getColor(Core::Vector4f & color1, Core::Vector4f & color2) { color1 = _color1; color2 = _color2; }
    void getGlow(Core::Vector4f & glow1, Core::Vector4f & glow2) { glow1 = _glow1; glow2 = _glow2; }
    float getFadeInTime() const { return _fadeIn; }
    float getDuration() const { return _duration; }
    float getFadeOutTime() const { return _fadeOut; }
    float getHeadWidth() const { return _headWidth; }
    float getTailWidth() const { return _tailWidth; }
    const Core::Vector3f & getHeadSphereCenter() const { return _headCenter; }
    const Core::Vector3f & getHeadSphereSize() const { return _headSize; }
    const Core::Vector3f & getTailSphereCenter() const { return _tailCenter; }
    const Core::Vector3f & getTailSphereSize() const { return _tailSize; }
    float getTravelTime() const { return _travelTime; }
    const Ptr<Node> & getTailNode() const { return _pTailNode; }

    void getGenerationTime(double & startTime, double & endTime) { startTime = _startTime; endTime = _endTime; }
    bool killAtEnd() const { return _killAtEnd; }


    virtual void restart(bool recursive = false);

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void prepareForRendering(const Ptr<NodeCamera> & pCamera);

    void setMode(Renderer::EFreeFormMode mode);
    Renderer::EFreeFormMode getMode();

protected:
    virtual void update(double elapsed);
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf & ray, float & distance);
    virtual void internalPrepareForPacking();

    void newRootArc();
    void newArc(const Core::Vector3f & head, const Core::Vector3f & tail, float headWidth, float tailWidth, int32 resolution, int32 subDiv);
    void internalSetMax();

    struct LightningArc : public Core::Object
    {
        Core::List<Core::Vector3f>  positions;
        Core::List<Core::Vector3f>  currentPositions;
        Core::Vector3f              headPosition;
        int32                       nextPositionIndex;
        double                      headLerpCoefficient;
        Core::Vector4f              color;
        Core::Vector4f              glow;
        float                       lifeTime;
        float                       headWidth;
        float                       tailWidth;
    };

    Ptr<RessourcePool>            _pResPool;
    Ptr<Renderer::IScene>         _pScene;
    Ptr<Renderer::IFreeForm>      _pFreeForm;
    Ptr<Node>                     _pTailNode;

    String          _texture;
    int32           _maxCount;
    float           _minPeriod;
    float           _maxPeriod;
    bool            _burstStart;

    float           _entropy;
    int32           _fertility;
    int32           _resolution;

    Core::Vector4f  _color1;
    Core::Vector4f  _color2;

    Core::Vector4f  _glow1;
    Core::Vector4f  _glow2;

    float           _fadeIn;
    float           _duration;
    float           _fadeOut;

    float           _headWidth;
    float           _tailWidth;

    Core::Vector3f  _headCenter;
    Core::Vector3f  _headSize;
    Core::Vector3f  _tailCenter;
    Core::Vector3f  _tailSize;

    Core::List<Ptr<LightningArc> >  _arcs;

    double          _startTime;
    double          _endTime;
    double          _lifeTime;

    double          _nextGenDuration;
    bool            _killAtEnd;

    float           _travelTime;
};

}

#endif /* NODEEMITTER_H_ */
