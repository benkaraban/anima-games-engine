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
#ifndef UNIVERSE_NODETRAIL_H_
#define UNIVERSE_NODETRAIL_H_

#include <Core/DoubleQueue.h>
#include <Universe/Node.h>
#include <Universe/NodeCamera.h>
#include <Universe/RessourcePool.h>
#include <Renderer/IFreeForm.h>
#include <Renderer/RenderTools.h>

namespace Universe
{
class LM_API_UNI NodeTrail : public Node
{
public:
    NodeTrail(const Ptr<Renderer::IAABox> & pBox,
              const Ptr<Renderer::IFreeForm> & pFreeForm,
              const Ptr<RessourcePool> & pRessourcesPool,
              World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_TRAIL; };

    void setMode(Renderer::EFreeFormMode mode);
    Renderer::EFreeFormMode getMode() const;

    virtual void restart(bool recursive = false);

    void setTexture(const String & textureName);
    const String & getTexture() const { return _textureName; }

    void setLength(float l) { _length = l; };
    void setDivisions(int32 div) { _segCount = div; };
    void setTrailMode(Renderer::ETrailMode mode) { _trail.mode = mode; };

    void setHeadWidth(float w) { _trail.headWidth = w; };
    void setTailWidth(float w) { _trail.tailWidth = w; };

    void setHeadU(float u) { _trail.headU = u; };
    void setTailU(float u) { _trail.tailU = u; };

    void setMainColor(const Core::Vector4f & c) { _mainColor = c; };
    void setHeadColor(const Core::Vector4f & c) { _trail.headColor = c; };
    void setTailColor(const Core::Vector4f & c) { _trail.tailColor = c; };

    void setMainGlow(const Core::Vector4f & c) { _mainGlow = c; };
    void setHeadGlow(const Core::Vector4f & c) { _trail.headGlow = c; };
    void setTailGlow(const Core::Vector4f & c) { _trail.tailGlow = c; };

    float getLength() const { return _length; };
    int32 getDivisions() const { return _segCount; };
    Renderer::ETrailMode getTrailMode() const { return _trail.mode; };

    float getHeadWidth() const { return _trail.headWidth; };
    float getTailWidth() const { return _trail.tailWidth; };

    float getHeadU() const { return _trail.headU; };
    float getTailU() const { return _trail.tailU; };

    const Core::Vector4f & getMainColor() const { return _mainColor; };
    const Core::Vector4f & getHeadColor() const { return _trail.headColor; };
    const Core::Vector4f & getTailColor() const { return _trail.tailColor; };

    const Core::Vector4f & getMainGlow() const { return _mainGlow; };
    const Core::Vector4f & getHeadGlow() const { return _trail.headGlow; };
    const Core::Vector4f & getTailGlow() const { return _trail.tailGlow; };

    virtual void setNodeColor(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeColor() const;

    virtual void setNodeGlow(const Core::Vector4f & color, bool recursive = false);
    virtual const Core::Vector4f & getNodeGlow() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void update(double elapsed);

    virtual void prepareForRendering(const Ptr<NodeCamera> & pCamera);

protected:
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    Ptr<Renderer::IFreeForm>    _pFreeForm;
    Ptr<RessourcePool>          _pRessourcesPool;
    String                      _textureName;

    float                       _length;
    int32                       _segCount;
    Renderer::TrailConfig       _trail;
    Core::Vector4f              _mainColor;
    Core::Vector4f              _mainGlow;

    Core::DoubleQueue<Core::Vector3f> _positions;
    Core::List<Core::Vector3f>        _renderPos;
    double                            _lastTime;
    float                             _relativeSize;
};
}

#endif /* NODEDECAL_H_ */
