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
#include <Core/Math/Random.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Interpolate.h>
#include <Core/XML/XMLMath.h>
#include <Renderer/RenderTools.h>
#include <Universe/NodeStorm.h>

namespace Universe
{
//-----------------------------------------------------------------------------
static void genInterPoint(Core::Vector3f & pMid, const Core::Vector3f & p1, const Core::Vector3f & p2, float disp)
{
    Core::Vector3f n(p2 - p1);

    if(n.isZero())
    {
        pMid = p1;
    }
    else
    {
        n.normalizeSafe();
        Core::Vector3f  u;
        Core::Vector3f  v;
        Core::genTangentBasis(n, u, v);

        Core::Vector3f dispVec(Core::frand(-disp, disp) * u + Core::frand(-disp, disp) * v);
        pMid = 0.5f * (p1 + p2) + dispVec;
    }
}
//-----------------------------------------------------------------------------
static void genLightningPath(Core::List<Core::Vector3f> & pts, int32 i0, int32 i2, float entropy)
{
    int32 i1 = (i0 + i2) / 2;

    if(i1 != i0 && i1 != i2)
    {
        genInterPoint(pts[i1], pts[i0], pts[i2], entropy);

        genLightningPath(pts, i0, i1, 0.5f * entropy);
        genLightningPath(pts, i1, i2, 0.5f * entropy);
    }
}
//-----------------------------------------------------------------------------
NodeStorm::NodeStorm(const Ptr<Renderer::IAABox> & pBox,
                     const Ptr<RessourcePool> & pResPool,
                     const Ptr<Renderer::IScene> & pScene,
                     int32 maxCount,
                     World * pWorld) :
    Node(pBox, pWorld),
    _pResPool(pResPool),
    _pScene(pScene),
    _maxCount(maxCount),
    _minPeriod(1.0f),
    _maxPeriod(1.0f),
    _burstStart(false),
    _entropy(0.25f),
    _fertility(0),
    _resolution(16),
    _color1(1.0, 1.0, 1.0, 1.0),
    _color2(1.0, 1.0, 1.0, 1.0),
    _glow1(1.0, 1.0, 1.0, 1.0),
    _glow2(1.0, 1.0, 1.0, 1.0),
    _fadeIn(0.0f),
    _duration(0.2f),
    _fadeOut(0.0f),
    _headWidth(1.0f),
    _tailWidth(0.0f),
    _headCenter(Core::Vector3f(0.0f, 10.0f, 0.0f)),
    _headSize(Core::Vector3f::ZERO),
    _tailCenter(Core::Vector3f(0.0f, -10.0f, 0.0f)),
    _tailSize(Core::Vector3f::ZERO),
    _startTime(0.0),
    _endTime(INFINITY),
    _lifeTime(0.0),
    _nextGenDuration(0.0),
    _killAtEnd(false),
    _travelTime(2.0)
{
    _pFreeForm = _pScene->createFreeForm(null, Renderer::FREE_FORM_ADD);
    internalSetMax();
}
//-----------------------------------------------------------------------------
void NodeStorm::setPeriod(float minPeriod, float maxPeriod) 
{ 
    _minPeriod = minPeriod; 
    _maxPeriod = maxPeriod; 
    _nextGenDuration = std::min(_nextGenDuration, double(_maxPeriod));
}
//-----------------------------------------------------------------------------
void NodeStorm::restart(bool recursive)
{
    Node::restart(recursive);

    _arcs.clear();
    _lifeTime = 0.0;
    _isAlive = true;
    _nextGenDuration = 0.0;

    if(_burstStart)
    {
        while(_arcs.size() < _maxCount)
            newRootArc();

        update(0.0);
    }
}
//-----------------------------------------------------------------------------
void NodeStorm::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeStorm::setTexture(const String & texture)
{
    Ptr<Renderer::ITextureMap> pTexture;

    if(texture == String::EMPTY)
        pTexture = _pResPool->getDefaultTexture();
    else
        pTexture = _pResPool->getTexture(texture);

    _texture = texture;
    _pFreeForm->setTexture(pTexture);
}
//-----------------------------------------------------------------------------
void NodeStorm::newRootArc()
{
    Ptr<LightningArc> pArc(new LightningArc());

    Core::Vector3f head(_headCenter);
    Core::Vector3f tail(_tailCenter);

    if(_pTailNode != null)
        tail = _worldToLocalMatrix.apply(_pTailNode->getWorldPosition());

    head.x += Core::frand(-_headSize.x, _headSize.x);
    head.y += Core::frand(-_headSize.y, _headSize.y);
    head.z += Core::frand(-_headSize.z, _headSize.z);

    tail.x += Core::frand(-_tailSize.x, _tailSize.x);
    tail.y += Core::frand(-_tailSize.y, _tailSize.y);
    tail.z += Core::frand(-_tailSize.z, _tailSize.z);

    newArc(head, tail, _headWidth, _tailWidth, _resolution, _fertility);
}
//-----------------------------------------------------------------------------
void NodeStorm::update(double elapsed)
{
    static const double MAX_STEP_LENGTH = 0.01;

    // Update arcs

    float startFadeOut = _fadeIn + _duration;
    float totalTime = startFadeOut + _fadeOut;

    for(int32 ii=0; ii < _arcs.size(); ii++)
    {
        LightningArc & arc = *_arcs[ii];

        float alpha = 0.0f;

        if(arc.lifeTime >= startFadeOut)
            if(_fadeOut != 0.0f)
                alpha = 1.0f - ((arc.lifeTime - startFadeOut) / _fadeOut);
            else
                alpha = 0.0f;
        else if(arc.lifeTime < _fadeIn)
            alpha = arc.lifeTime / _fadeIn;
        else
            alpha = 1.0f;

        alpha = Core::clamp(0.0f, 1.0f, alpha);
        arc.color.a = arc.glow.a = alpha;
        arc.lifeTime += float(elapsed);

        // Update current head position
        if(arc.nextPositionIndex < arc.positions.size())
        {
            if(arc.headLerpCoefficient >= 1.0)
            {
                arc.currentPositions.pop_back();
                arc.currentPositions.push_back(arc.positions[arc.nextPositionIndex]);
                arc.headLerpCoefficient = 0.0;
                arc.nextPositionIndex++;
                if(arc.nextPositionIndex < arc.positions.size())
                    arc.currentPositions.push_back(arc.positions[arc.nextPositionIndex]);
            }
            if(arc.nextPositionIndex < arc.positions.size())
            {
                arc.headLerpCoefficient += (elapsed * (arc.positions.size()-1))/ _travelTime;
                arc.headPosition.setLerp(arc.positions[arc.nextPositionIndex-1], arc.positions[arc.nextPositionIndex], (float) arc.headLerpCoefficient);
                arc.currentPositions.pop_back();
                arc.currentPositions.push_back(arc.headPosition);
            }
        }
    }

    // Remove dead arcs

    int32 ii = 0;

    while(ii < _arcs.size())
    {
        if(_arcs[ii]->lifeTime >= totalTime)
            _arcs.qerase(ii);
        else
            ii ++;
    }

    // Generate new arcs

    double frameElapsed = std::min(elapsed, 1.0);

    while(frameElapsed > 0.0)
    {
        double stepElapsed = std::min(frameElapsed, MAX_STEP_LENGTH);

        if(_arcs.size() < _maxCount && _lifeTime >= _startTime && _lifeTime < _endTime)
        {
            _nextGenDuration -= stepElapsed;

            while(_nextGenDuration <= 0.0 && _arcs.size() < _maxCount)
            {
                newRootArc();
                _nextGenDuration += _minPeriod + Core::frand() * (_maxPeriod - _minPeriod);
            }
        }

        frameElapsed -= stepElapsed;
        _lifeTime += stepElapsed;
    }

    updateBoundingBox();

    if(_arcs.size() == 0 && _lifeTime >= _endTime && _killAtEnd)
        kill();
}
//-----------------------------------------------------------------------------
void NodeStorm::prepareForRendering(const Ptr<NodeCamera> & pCamera)
{
    Core::Vector3f viewVec(_worldToLocalMatrix.apply(pCamera->getWorldPosition()));

    Renderer::FreeFormData & data = _pFreeForm->beginUpdate();
    data.indices.clear();
    data.vertices.clear();

    for(int32 ii=0; ii < _arcs.size(); ii++)
    {
        const LightningArc & arc = *_arcs[ii];
        Renderer::TrailConfig trail;

        trail.headWidth     = arc.headWidth;
        trail.tailWidth     = arc.tailWidth;

        trail.headU         = 0.0f;
        trail.tailU         = 1.0f;

        trail.headColor     = arc.color;
        trail.tailColor     = arc.color;

        trail.headGlow      = arc.glow;
        trail.tailGlow      = arc.glow;

        trail.mode          = Renderer::TRAIL_STANDARD_NORM;
        trail.viewVec       = viewVec;
        trail.duStart       = 0.0f;
        trail.tailClamp     = false;
        trail.tailFade      = false;

        if(arc.currentPositions.size() * 2 + data.vertices.size() < 65535)
            Renderer::generateTrail(data.vertices, data.indices, arc.currentPositions, trail, true);
            //Renderer::generateTrail(data.vertices, data.indices, arc.positions, trail, true);
    }
    _pFreeForm->endUpdate();
}
//-----------------------------------------------------------------------------
void NodeStorm::newArc(const Core::Vector3f & head, const Core::Vector3f & tail, float headWidth, float tailWidth, int32 resolution, int32 subDiv)
{
    Ptr<LightningArc> pArc(new LightningArc());
    float length = head.getDistance(tail);

    pArc->color.setLerp(_color1, _color2, Core::frand());
    pArc->glow.setLerp(_glow1, _glow2, Core::frand());
    pArc->lifeTime = 0.0f;
    pArc->headWidth = headWidth;
    pArc->tailWidth = tailWidth;

    pArc->positions.resize(std::max(2, resolution));
    pArc->positions[0] = head;
    pArc->positions[pArc->positions.size() - 1] = tail;

    genLightningPath(pArc->positions, 0, pArc->positions.size() - 1, _entropy * length);

    // Make tree

    for(int32 ii=0; ii < subDiv; ii++)
    {
        int32 iPoint = Core::irand(0, pArc->positions.size() - 1);
        float u = float(iPoint) / float(pArc->positions.size() - 1);

        float childHeadWidth = 0.5f * Core::lerp(headWidth, 0.0f, u);
        float childTailWidth = 0.5f * Core::lerp(tailWidth, 0.0f, u);

        Core::Vector3f childHead(pArc->positions[iPoint]);
        Core::Vector3f childTail;

        genInterPoint(childTail, childHead, tail, 4.0f * _entropy * Core::lerp(length, 0.0f, u));

        float childLength = childHead.getDistance(childTail);
        float lengthMul = childLength / length;

        float clampedLengthMul = std::min(1.0f, lengthMul);
        int32 childResolution = int32(float(resolution) * clampedLengthMul);
        int32 childSubDiv = int32(float(subDiv / 2) * clampedLengthMul);

        if(childResolution >= 5)
            newArc(childHead, childTail, childHeadWidth, childTailWidth, childResolution, childSubDiv);
    }

    pArc->headPosition = pArc->positions[0];
    pArc->nextPositionIndex = 0;
    pArc->headLerpCoefficient = 1.0;
    pArc->currentPositions.push_back(pArc->positions[0]);
    //pArc->currentPositions.push_back(pArc->positions[0]);
    _arcs.push_back(pArc);
}
//-----------------------------------------------------------------------------
void NodeStorm::internalSetMax()
{
    int32 ptCount = _resolution * _maxCount;
    _pFreeForm->setMax(2 * ptCount, 6 * ptCount);
}
//-----------------------------------------------------------------------------
void NodeStorm::setMaxCount(int32 maxCount)
{
    if(maxCount != _maxCount)
    {
        _maxCount = maxCount;
        _arcs.resize(std::min(_arcs.size(), _maxCount));
        internalSetMax();
    }
}
//-----------------------------------------------------------------------------
void NodeStorm::setResolution(int32 r) 
{ 
    if(r != _resolution)
    {
        _resolution = r;
        internalSetMax();
    }
}
//-----------------------------------------------------------------------------
void NodeStorm::onWorldMatrixUpdate()
{
    _pFreeForm->setWorldMatrix(_localToWorldMatrix);
}
//-----------------------------------------------------------------------------
void NodeStorm::onKill()
{
    _pFreeForm->kill();
}
//-----------------------------------------------------------------------------
void NodeStorm::onVisibilityChange(bool flag)
{
    _pFreeForm->setVisible(flag);
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeStorm::internalGetBoundingBox() const
{
    return _pFreeForm->getBoundingBox();
}
//-----------------------------------------------------------------------------
bool NodeStorm::internalIsIntersecting(const Core::Rayf & ray, float & distance)
{
    return isIntersectingBoundingBox(ray, distance);
}
//-----------------------------------------------------------------------------
void NodeStorm::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    String texture;

    pNode->getValue(L"start-time", _startTime);
    pNode->getValue(L"end-time", _endTime);

    pNode->getValue(L"max-count", _maxCount);
    pNode->getValue(L"min-period", _minPeriod);
    pNode->getValue(L"max-period", _maxPeriod);
    pNode->getValue(L"entropy", _entropy);
    pNode->getValue(L"fertility", _fertility);
    pNode->getValue(L"resolution", _resolution);
    pNode->getValue(L"texture", texture);

    Core::getXMLValue(*pNode, L"color1", _color1);
    Core::getXMLValue(*pNode, L"color2", _color2);
    Core::getXMLValue(*pNode, L"glow1", _glow1);
    Core::getXMLValue(*pNode, L"glow2", _glow2);

    pNode->getValue(L"fade-in", _fadeIn);
    pNode->getValue(L"duration", _duration);
    pNode->getValue(L"fade-out", _fadeOut);

    pNode->getValue(L"head-width", _headWidth);
    pNode->getValue(L"tail-width", _tailWidth);

    Core::getXMLValue(*pNode, L"head-center", _headCenter);
    Core::getXMLValue(*pNode, L"head-size", _headSize);
    Core::getXMLValue(*pNode, L"tail-center", _tailCenter);
    Core::getXMLValue(*pNode, L"tail-size", _tailSize);

    pNode->getValue(L"travel-time", _travelTime);

    String mode;
    pNode->getValue(L"mode", mode);

    if(mode == Renderer::toString(Renderer::FREE_FORM_ADD))
        setMode(Renderer::FREE_FORM_ADD);
    else if(mode == Renderer::toString(Renderer::FREE_FORM_LERP))
        setMode(Renderer::FREE_FORM_LERP);
    else if(mode == Renderer::toString(Renderer::FREE_FORM_REFRAC))
        setMode(Renderer::FREE_FORM_REFRAC);
    else if(mode == L"")
        setMode(Renderer::FREE_FORM_ADD);
    else
        throw Core::Exception(L"Unknown NodeStorm Representation Mode :" + mode);

    _lifeTime = 0.0;
    _nextGenDuration = 0.0;
    setTexture(texture);
    internalSetMax();
}
//-----------------------------------------------------------------------------
void NodeStorm::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    pNode->setValue(L"start-time", _startTime);
    pNode->setValue(L"end-time", _endTime);

    pNode->setValue(L"max-count", _maxCount);
    pNode->setValue(L"min-period", _minPeriod);
    pNode->setValue(L"max-period", _maxPeriod);
    pNode->setValue(L"entropy", _entropy);
    pNode->setValue(L"fertility", _fertility);
    pNode->setValue(L"resolution", _resolution);

    pNode->setValue(L"texture", _texture);
    pNode->setValue(L"color1", Core::toXMLString(_color1));
    pNode->setValue(L"color2", Core::toXMLString(_color2));
    pNode->setValue(L"glow1", Core::toXMLString(_glow1));
    pNode->setValue(L"glow2", Core::toXMLString(_glow2));

    pNode->setValue(L"fade-in", _fadeIn);
    pNode->setValue(L"duration", _duration);
    pNode->setValue(L"fade-out", _fadeOut);

    pNode->setValue(L"head-width", _headWidth);
    pNode->setValue(L"tail-width", _tailWidth);

    pNode->setValue(L"head-center", Core::toXMLString(_headCenter));
    pNode->setValue(L"head-size", Core::toXMLString(_headSize));
    pNode->setValue(L"tail-center", Core::toXMLString(_tailCenter));
    pNode->setValue(L"tail-size", Core::toXMLString(_tailSize));

    pNode->setValue(L"mode", Renderer::toString(getMode()));
    pNode->setValue(L"travel-time", _travelTime);
}
//-----------------------------------------------------------------------------
void NodeStorm::setMode(Renderer::EFreeFormMode mode)
{
    _pFreeForm->setMode(mode);
}
//-----------------------------------------------------------------------------
Renderer::EFreeFormMode NodeStorm::getMode()
{
    return _pFreeForm->getMode();
}
}