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
#include <Core/XML/XMLMath.h>
#include <Universe/NodeEmitter.h>
#include <Universe/Particles/PartRepresentationBillboard.h>
#include <Universe/Particles/PartRepresentationGroup.h>
#include <Universe/Particles/PartEffectColorByPos.h>
#include <Universe/Particles/PartEffectFade.h>
#include <Universe/Particles/PartEffectSize.h>
#include <Universe/Particles/PartEffectForce.h>
#include <Universe/Particles/PartEffectAnimate.h>
#include <Universe/Particles/PartEffectAttractor.h>
#include <Universe/Particles/PartEffectResistance.h>
#include <Universe/Particles/PartEffectRoll.h>
#include <Universe/Particles/PartEffectSpeedLimiter.h>
#include <Universe/Particles/PartEffectTranslate.h>
#include <Universe/Particles/PartEffectTurn.h>
#include <Universe/Particles/EmitterShapeBox.h>
#include <Universe/Particles/EmitterShapeCone.h>
#include <Universe/Particles/EmitterShapeModel.h>
#include <Universe/Particles/EmitterShapeSpline.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeEmitter::NodeEmitter(const Ptr<Renderer::IAABox> & pBox,
                         const Ptr<RessourcePool> & pResPool,
                         const Ptr<IPartRepresentation> & pRepresentation,
                         const Ptr<IEmitterShape> & pShape,
                         int32 maxCount,
                         World * pWorld) :
    Node(pBox, pWorld),
    _pResPool(pResPool),
    _pShape(pShape),
    _pRepresentation(pRepresentation),
    _maxCount(maxCount),
    _color1(1.0, 1.0, 1.0, 1.0),
    _color2(1.0, 1.0, 1.0, 1.0),
    _minPeriod(1.0),
    _maxPeriod(1.0),
    _periodRange(0.0),
    _minTimeMul(1.0),
    _maxTimeMul(1.0),
    _timeMulRange(0.0),
    _minWidth(1.0),
    _maxWidth(1.0),
    _widthRange(0.0),
    _minHeight(1.0),
    _maxHeight(1.0),
    _heightRange(0.0),
    _minAngle(0.0),
    _maxAngle(0.0),
    _angleRange(0.0),
    _minT(0.0),
    _maxT(0.0),
    _tRange(0.0),
    _minBurst(0.0),
    _maxBurst(0.0),
    _squareParticles(false),
    _stickyParticles(true),
    _nsize(false),
    _burstStart(false),
    _startTime(0.0),
    _endTime(INFINITY),
    _curEndTime(INFINITY),
    _lifeTime(0.0),
    _killAtEnd(false),
    _nextGenDuration(0.0),
    _partIndex(0)
{

}
NodeEmitter::~NodeEmitter()
{
}
//-----------------------------------------------------------------------------
void NodeEmitter::restart(bool recursive)
{
    Node::restart(recursive);

    _particles.clear();
    _lifeTime = 0.0;
    _isAlive = true;
    _nextGenDuration = 0.0;
    _curEndTime = _endTime;

    if(_burstStart)
    {
        while(_particles.size() < _maxCount)
        {
            Particle newPart;
            createPart(newPart);

            newPart.lifeTime = _minBurst + Core::frand() * _burstRange * newPart.timeMul;

            _particles.push_back(newPart);
        }

        update(0.0);
    }
}
//-----------------------------------------------------------------------------
void NodeEmitter::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeEmitter::stop(bool recursive, float inTime)
{
    Node::stop(recursive, inTime);
    _curEndTime = _lifeTime + inTime;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setShape(const Ptr<IEmitterShape> & pShape)
{
    _pShape = pShape;
}
//-----------------------------------------------------------------------------
void NodeEmitter::addEffect(const Ptr<IPartEffect> & pEffect)
{
    _effects.push_back(pEffect);
}
//-----------------------------------------------------------------------------
void NodeEmitter::clearEffects()
{
    _effects.clear();
}
//-----------------------------------------------------------------------------
void NodeEmitter::setPeriod(float minPeriod, float maxPeriod)
{
    _minPeriod = minPeriod;
    _maxPeriod = maxPeriod;
    _periodRange = maxPeriod - minPeriod;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setTimeMul(float minTimeMul, float maxTimeMul)
{
    _minTimeMul = minTimeMul;
    _maxTimeMul = maxTimeMul;
    _timeMulRange = maxTimeMul - minTimeMul;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setStartColor(const Core::Vector4f & color1, const Core::Vector4f & color2)
{
    _color1 = color1;
    _color2 = color2;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setStartWidth(float minWidth, float maxWidth)
{
    _minWidth = minWidth;
    _maxWidth = maxWidth;
    _widthRange = maxWidth - minWidth;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setStartHeight(float minHeight, float maxHeight)
{
    _minHeight = minHeight;
    _maxHeight = maxHeight;
    _heightRange = maxHeight - minHeight;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setStartAngle(float minAngle, float maxAngle)
{
    _minAngle = minAngle;
    _maxAngle = maxAngle;
    _angleRange = maxAngle - minAngle;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setStartTimeKey(float minT, float maxT)
{
    _minT = minT;
    _maxT = maxT;
    _tRange = maxT - minT;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setBurstLifeTime(float minBurst, float maxBurst)
{
    _minBurst = minBurst;
    _maxBurst = maxBurst;
    _burstRange = _maxBurst - _minBurst;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setGenerationTime(double startTime, double endTime)
{
    _startTime  = startTime;
    _endTime    = endTime;
    _curEndTime = endTime;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getPeriod(float & minPeriod, float & maxPeriod) const
{
    minPeriod = _minPeriod;
    maxPeriod = _maxPeriod;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getTimeMul(float & minTimeMul, float & maxTimeMul) const
{
    minTimeMul = _minTimeMul;
    maxTimeMul = _maxTimeMul;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getStartColor(Core::Vector4f & color1, Core::Vector4f & color2) const
{
    color1 = _color1;
    color2 = _color2;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getStartWidth(float & minWidth, float & maxWidth) const
{
    minWidth = _minWidth;
    maxWidth = _maxWidth;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getStartHeight(float & minHeight, float & maxHeight) const
{
    minHeight = _minHeight;
    maxHeight = _maxHeight;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getStartAngle(float & minAngle, float & maxAngle) const
{
    minAngle = _minAngle;
    maxAngle = _maxAngle;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getStartTimeKey(float & minT, float & maxT)
{
    minT = _minT;
    maxT = _maxT;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getGenerationTime(double & startTime, double & endTime) const
{
    startTime  = _startTime;
    endTime    = _endTime;
}
//-----------------------------------------------------------------------------
void NodeEmitter::getBurstLifeTime(float & minBurst, float & maxBurst)
{
    minBurst = _minBurst;
    maxBurst = _maxBurst;
}
//-----------------------------------------------------------------------------
const Ptr<IEmitterShape> & NodeEmitter::getShape() const
{
    return _pShape;
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<IPartEffect> > & NodeEmitter::getEffects() const
{
    return _effects;
}
//-----------------------------------------------------------------------------
void NodeEmitter::removeEffects(EEffect type)
{
    Core::List<Ptr<IPartEffect>>::iterator ii;
    bool found = false;
    Core::List<Ptr<IPartEffect>>::iterator iEraseMe;

    for(ii = _effects.begin(); ii != _effects.end(); ++ii)
        if((*ii)->getType() == type)
        {
            found = true;
            iEraseMe = ii;
        }
    if(found)
        _effects.erase(iEraseMe);
}
//----------------------------------------------------------------------------
void NodeEmitter::createPart(Particle & newPart)
{
    _pShape->createParticle(newPart);

    if(!_stickyParticles)
    {
        newPart.position = _partsMatrix.apply(newPart.position);
        newPart.speed = _partsMatrix.apply3x3(newPart.speed);
    }

    newPart.oldPosition = newPart.position;
    newPart.randU = Core::frand();
    newPart.randV = Core::frand();
    newPart.randW = Core::frand();
    newPart.timeMul = _minTimeMul + Core::frand() * _timeMulRange;
    newPart.timeKey = _minT + Core::frand() * _tRange;
    newPart.lifeTime = -_nextGenDuration * newPart.timeMul;
    newPart.color.setLerp(_color1, _color2, Core::frand());
    newPart.width = _minWidth + Core::frand() * _widthRange;
    newPart.id = _partIndex ++;
    if(_squareParticles)
        newPart.height = newPart.width;
    else
        newPart.height = _minHeight + Core::frand() * _heightRange;
    newPart.angle = _minAngle + Core::frand() * _angleRange;
    newPart.oldAngle = newPart.angle;
}
//-----------------------------------------------------------------------------
void NodeEmitter::update(double elapsed)
{
    _lifeTime += elapsed;
    if(int32(_particles.size()) < _maxCount
    && _lifeTime >= _startTime && _lifeTime < _curEndTime)
    {
        _nextGenDuration -= elapsed;

        while(_nextGenDuration <= 0.0 && int32(_particles.size()) < _maxCount)
        {
            Particle newPart;
            createPart(newPart);

            _newParticles.clear();
            _newParticles.push_back(newPart);

            for(Core::List<Ptr<IPartEffect> >::const_iterator ipEffect = _effects.begin(); ipEffect != _effects.end(); ++ipEffect)
                (*ipEffect)->updateParticles(_newParticles, -_nextGenDuration);

            _particles.push_back(newPart);

            _nextGenDuration += _minPeriod + Core::frand() * _periodRange;
        }
    }

    _pShape->updateParticles(_particles, elapsed);

    for(Core::List<Particle>::iterator iPart = _particles.begin(); iPart != _particles.end(); ++iPart)
        iPart->lifeTime += elapsed * iPart->timeMul;

    for(Core::List<Ptr<IPartEffect> >::const_iterator ipEffect = _effects.begin(); ipEffect != _effects.end(); ++ipEffect)
        (*ipEffect)->updateParticles(_particles, elapsed);

    _pRepresentation->update(_particles);
    updateBoundingBox();

    for(Core::List<Particle>::iterator iPart = _particles.begin(); iPart != _particles.end(); ++iPart)
    {
        iPart->oldPosition = iPart->position;
        iPart->oldAngle = iPart->angle;
        iPart->position += float(elapsed) * iPart->speed;
    }

    if(_particles.size() == 0 && _lifeTime >= _curEndTime && _killAtEnd)
        kill();
}
//-----------------------------------------------------------------------------
void NodeEmitter::setMaxCount(int32 maxCount)
{
    _maxCount = maxCount;
    _pRepresentation->setMaxSize(_maxCount);
    _particles.resize(std::min(_particles.size(), _maxCount));
}
//-----------------------------------------------------------------------------
Ptr<IPartRepresentation> NodeEmitter::getRepresentation() const
{
    return _pRepresentation;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setRepresentation(const Ptr<IPartRepresentation> & pRepresentation)
{
    if(_pRepresentation != pRepresentation)
    {
        _pRepresentation->kill();
        _pRepresentation = pRepresentation;
    }
}
//-----------------------------------------------------------------------------
void NodeEmitter::setNormalizedSize(bool nsize)
{ 
    _nsize = nsize;
    onWorldMatrixUpdate();
}
//-----------------------------------------------------------------------------
void NodeEmitter::setColorFactor(float f)
{
    _pRepresentation->setColorFactor(f);
}
//-----------------------------------------------------------------------------
void NodeEmitter::setGlowFactor(float f)
{
    _pRepresentation->setGlowFactor(f);
}
//-----------------------------------------------------------------------------
float NodeEmitter::getColorFactor() const
{
    return _pRepresentation->getColorFactor();
}
//-----------------------------------------------------------------------------
float NodeEmitter::getGlowFactor() const
{
    return _pRepresentation->getGlowFactor();
}
//-----------------------------------------------------------------------------
void NodeEmitter::setNodeColor(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    setColorFactor(color.a);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeEmitter::getNodeColor() const
{
    _nodeColor = Core::Vector4f(1.0f, 1.0f, 1.0f, getColorFactor());
    return _nodeColor;
}
//-----------------------------------------------------------------------------
void NodeEmitter::setNodeGlow(const Core::Vector4f & color, bool recursive)
{
    Node::setNodeColor(color, recursive);
    setGlowFactor(color.a);
}
//-----------------------------------------------------------------------------
const Core::Vector4f & NodeEmitter::getNodeGlow() const
{
    _nodeGlow = Core::Vector4f(1.0f, 1.0f, 1.0f, getGlowFactor());
    return _nodeGlow;
}
//-----------------------------------------------------------------------------
void NodeEmitter::onWorldMatrixUpdate()
{
    _partsMatrix = _localToWorldMatrix;

    if(_nsize)
        _partsMatrix.normalize();

    float partScale = _partsMatrix.getXVector().getLengthSafe();
    _pRepresentation->setPartScale(partScale);

    if(_stickyParticles)
        _pRepresentation->setWorldMatrix(_partsMatrix);
    else
        _pRepresentation->setWorldMatrix(Core::Matrix4f::IDENTITY_MATRIX);
}
//-----------------------------------------------------------------------------
void NodeEmitter::onKill()
{
    _pRepresentation->kill();
}
//-----------------------------------------------------------------------------
void NodeEmitter::onVisibilityChange(bool flag)
{
    _pRepresentation->setVisible(flag);
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeEmitter::internalGetBoundingBox() const
{
    return _pRepresentation->getBoundingBox();
}
//-----------------------------------------------------------------------------
bool NodeEmitter::internalIsIntersecting(const Core::Rayf & ray, float & distance)
{
    return isIntersectingBoundingBox(ray, distance);
}
//-----------------------------------------------------------------------------
void NodeEmitter::importXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::importXML(pNode);

    bool success = true;

    _pRepresentation->importXML(pNode->getNode(L"representation"));

    String color1;
    String color2;

    success &= pNode->getValue(L"color1", color1);
    success &= pNode->getValue(L"color2", color2);

    Core::fromXMLString(color1, _color1);
    Core::fromXMLString(color2, _color2);

    float minPeriod,   maxPeriod;
    float minTimeMul,  maxTimeMul;
    float minWidth,    maxWidth;
    float minHeight,   maxHeight;
    float minAngle,    maxAngle;
    float minT, maxT;
    float minBurst, maxBurst;
    float colorFactor = 1.0f;
    float glowFactor = 0.0f;

    Core::String startTime;
    Core::String endTime;

    success &= pNode->getValue(L"min-period", minPeriod);
    success &= pNode->getValue(L"max-period", maxPeriod);

    if(!pNode->getValue(L"min-time-mul", minTimeMul))
        minTimeMul = 1.0f;

    if(!pNode->getValue(L"max-time-mul", maxTimeMul))
        maxTimeMul = 1.0f;

    success &= pNode->getValue(L"max-period", maxPeriod);

    success &= pNode->getValue(L"min-width", minWidth);
    success &= pNode->getValue(L"max-width", maxWidth);

    success &= pNode->getValue(L"min-height", minHeight);
    success &= pNode->getValue(L"max-height", maxHeight);

    success &= pNode->getValue(L"min-angle", minAngle);
    success &= pNode->getValue(L"max-angle", maxAngle);

    if(!(pNode->getValue(L"min-timekey", minT) &&
         pNode->getValue(L"max-timekey", maxT)))
    {
        minT = maxT = 0.0;
    }

    if(!(pNode->getValue(L"min-burstlife", minBurst) &&
         pNode->getValue(L"max-burstlife", maxBurst)))
    {
        minBurst = maxBurst = 0.0;
    }

    success &= pNode->getValue(L"square-parts", _squareParticles);
    success &= pNode->getValue(L"sticky-parts", _stickyParticles);

    int32 maxCount;
    if(!pNode->getValue(L"max-count",  maxCount))
        maxCount = 1000;
    setMaxCount(maxCount);

    success &= pNode->getValue(L"start-time",  startTime);
    success &= pNode->getValue(L"end-time",    endTime);
    success &= pNode->getValue(L"kill-at-end", _killAtEnd);

    if(!pNode->getValue(L"normalized-size", _nsize))
        _nsize = true;

    if(!pNode->getValue(L"burst-start", _burstStart))
        _burstStart = false;

    if(!pNode->getValue(L"color-factor", colorFactor))
        colorFactor = 1.0f;

    if(!pNode->getValue(L"glow-factor", glowFactor))
        glowFactor = 0.0f;

    setColorFactor(colorFactor);
    setGlowFactor(glowFactor);

    setPeriod(minPeriod, maxPeriod);
    setTimeMul(minTimeMul, maxTimeMul);
    setStartWidth(minWidth, maxWidth);
    setStartHeight(minHeight, maxHeight);
    setStartAngle(minAngle, maxAngle);
    setStartTimeKey(minT, maxT);
    setBurstLifeTime(minBurst, maxBurst);

    _startTime  = Core::toFloat(startTime);
    _endTime    = Core::toFloat(endTime);

    float zBias = 0.0f;
    float fogFactor = 1.0f;

    if(!pNode->getValue(L"zbias", zBias))
        zBias = 0.0f;

    if(!pNode->getValue(L"fog-factor", fogFactor))
        fogFactor = 1.0f;

    setZBias(zBias);
    setFogFactor(fogFactor);

    String shapeType;
    success &= pNode->getAttribute(L"shape", L"type", shapeType);

    if(shapeType == Universe::toString(SHAPE_CONE))
        _pShape = Ptr<IEmitterShape>(new Universe::EmitterShapeCone());
    else if(shapeType == Universe::toString(SHAPE_MODEL))
        _pShape = Ptr<IEmitterShape>(new Universe::EmitterShapeModel(_pResPool));
    else if(shapeType == Universe::toString(SHAPE_BOX))
        _pShape = Ptr<IEmitterShape>(new Universe::EmitterShapeBox());
    else if(shapeType == Universe::toString(SHAPE_SPLINE))
        _pShape = Ptr<IEmitterShape>(new Universe::EmitterShapeSpline(_pResPool));
    else
        throw Core::Exception(L"Unknown NodeEmitter Shape type :" + shapeType);

    _pShape->importXML(pNode);

    Core::List<Ptr<Core::XMLNode> > effects;
    pNode->getNodes(L"effects", L"effect", effects);

    for(Core::List<Ptr<Core::XMLNode> >::const_iterator iEffect = effects.begin(); iEffect != effects.end(); ++iEffect)
    {
        Ptr<IPartEffect> pEffect;
        String effectType;
        success &= (*iEffect)->getAttribute(L"", L"type", effectType);
        if(effectType == Universe::toString(EFFECT_FADE))
            pEffect = Ptr<IPartEffect>(new PartEffectFade(Core::Vector4f(), Core::Vector4f(), 0.0f, false));
        else if(effectType == Universe::toString(EFFECT_FADE2))
            pEffect = Ptr<IPartEffect>(new PartEffectFade2(Core::Vector4f(), Core::Vector4f(), Core::Vector4f(), 0.0f, 0.0f, false, false));
        else if(effectType == Universe::toString(EFFECT_FADE3))
            pEffect = Ptr<IPartEffect>(new PartEffectFade3(Core::Vector4f(), Core::Vector4f(), Core::Vector4f(), Core::Vector4f(), 0.0f, 0.0f, 0.0f, false, false));
        else if(effectType == Universe::toString(EFFECT_FORCE))
            pEffect = Ptr<IPartEffect>(new PartEffectForce(Core::Vector3f(1.0f, 1.0f, 1.0f), 1.0f));
        else if(effectType == Universe::toString(EFFECT_ATTRACTOR))
            pEffect = Ptr<IPartEffect>(new PartEffectAttractor(Core::Vector3f(1.0f, 1.0f, 1.0f), 1.0f));
        else if(effectType == Universe::toString(EFFECT_SPEED_LIMIT))
            pEffect = Ptr<IPartEffect>(new PartEffectSpeedLimiter(1.0f));
        else if(effectType == Universe::toString(EFFECT_RESISTANCE))
            pEffect = Ptr<IPartEffect>(new PartEffectResistance(0.0f));
        else if(effectType == Universe::toString(EFFECT_ANIMATE))
            pEffect = Ptr<IPartEffect>(new PartEffectAnimate(0.0f, 1.0f, 3.0f, true, true));
        else if(effectType == Universe::toString(EFFECT_SIZE))
            pEffect = Ptr<IPartEffect>(new PartEffectSize(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
        else if(effectType == Universe::toString(EFFECT_SIZE2))
            pEffect = Ptr<IPartEffect>(new PartEffectSize2(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
        else if(effectType == Universe::toString(EFFECT_ROLL))
            pEffect = Ptr<IPartEffect>(new PartEffectRoll(0.0f, 0.0f));
        else if(effectType == Universe::toString(EFFECT_TRANSLATE))
            pEffect = Ptr<IPartEffect>(new PartEffectTranslate());
        else if(effectType == Universe::toString(EFFECT_COLOR_BY_POS))
            pEffect = Ptr<IPartEffect>(new PartEffectColorByPos());
        else if(effectType == Universe::toString(EFFECT_TURN))
            pEffect = Ptr<IPartEffect>(new PartEffectTurn());
        else
            throw Core::Exception(L"Unknown NodeEmitter Effect type :" + effectType);

        pEffect->importXML(*iEffect);

        addEffect(pEffect);
    }

    _curEndTime = _endTime;

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML NodeEmitter values.");
}
//-----------------------------------------------------------------------------
void NodeEmitter::exportXML(const Ptr<Core::XMLNode> & pNode)
{
    Node::exportXML(pNode);

    Ptr<Universe::IPartRepresentation> pRep(getRepresentation());
    pNode->setValue(L"representation.type", Universe::toString(pRep->getType()));
    pRep->exportXML(pNode->getNode(L"representation"));

    pNode->setValue(L"color1", Core::toXMLString(_color1));
    pNode->setValue(L"color2", Core::toXMLString(_color2));

    pNode->setValue(L"min-period", _minPeriod);
    pNode->setValue(L"max-period", _maxPeriod);

    pNode->setValue(L"min-time-mul", _minTimeMul);
    pNode->setValue(L"max-time-mul", _maxTimeMul);

    pNode->setValue(L"min-width", _minWidth);
    pNode->setValue(L"max-width", _maxWidth);

    pNode->setValue(L"min-height", _minHeight);
    pNode->setValue(L"max-height", _maxHeight);

    pNode->setValue(L"min-angle", _minAngle);
    pNode->setValue(L"max-angle", _maxAngle);

    pNode->setValue(L"min-timekey", _minT);
    pNode->setValue(L"max-timekey", _maxT);

    pNode->setValue(L"min-burstlife", _minBurst);
    pNode->setValue(L"max-burstlife", _maxBurst);

    pNode->setValue(L"square-parts", _squareParticles);
    pNode->setValue(L"sticky-parts", _stickyParticles);

    pNode->setValue(L"max-count",  _maxCount);
    pNode->setValue(L"start-time",  _startTime);
    pNode->setValue(L"end-time",    _endTime);
    pNode->setValue(L"kill-at-end", _killAtEnd);

    pNode->setValue(L"normalized-size", _nsize);
    pNode->setValue(L"burst-start", _burstStart);

    pNode->setValue(L"color-factor", getColorFactor());
    pNode->setValue(L"glow-factor", getGlowFactor());

    pNode->setValue(L"zbias", getZBias());
    pNode->setValue(L"fog-factor", getFogFactor());

    pNode->setAttribute(L"shape", L"type", Universe::toString(_pShape->getType()));
    _pShape->exportXML(pNode);

    for(Core::List<Ptr<IPartEffect> >::const_iterator iEffect = _effects.begin(); iEffect != _effects.end(); ++iEffect)
    {
        Core::Ptr<Core::XMLNode> pXMLEffect(new Core::XMLNode(L"effect"));
        pXMLEffect->setAttribute(L"", L"type", Universe::toString((*iEffect)->getType()));
        (*iEffect)->exportXML(pXMLEffect);
        pNode->addNode(L"effects", pXMLEffect);
    }
}
//-----------------------------------------------------------------------------
}
