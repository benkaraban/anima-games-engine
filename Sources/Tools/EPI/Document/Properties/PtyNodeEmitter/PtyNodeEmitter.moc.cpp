/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "PtyNodeEmitter.moc.h"

#include <Universe/Particles/PartRepresentationBillboard.h>
#include <Universe/NodeEmitter.h>

#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/RangedSlidingTimeValue.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterRepBillboard.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterRepGroup.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterShapeBox.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterShapeCone.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterShapeModel.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyEmitterShapeSpline.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectAnimate.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectAttractor.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectColorByPos.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectFade.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectForce.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectResistance.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectRoll.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectSpeedLimiter.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectSize.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectTranslate.moc.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyPartEffectTurn.moc.h>


namespace EPI
{
Core::String EShapeToString(Universe::EShape shape)
{
    String ret;

    switch(shape)
    {
    case Universe::SHAPE_CONE:
        ret = L"Cone";
        break;
    case Universe::SHAPE_MODEL:
        ret = L"Model";
        break;
    case Universe::SHAPE_BOX:
        ret = L"Box";
        break;
    case Universe::SHAPE_SPLINE:
        ret = L"Spline";
        break;
    }

    return ret;
}

Universe::EShape StringToEShape(const String& str)
{
    Universe::EShape ret = Universe::SHAPE_CONE;

    if(str == EShapeToString(Universe::SHAPE_CONE))
        ret = Universe::SHAPE_CONE;
    else if(str == EShapeToString(Universe::SHAPE_MODEL))
        ret = Universe::SHAPE_MODEL;
    else if(str == EShapeToString(Universe::SHAPE_BOX))
        ret = Universe::SHAPE_BOX;
    else if(str == EShapeToString(Universe::SHAPE_SPLINE))
        ret = Universe::SHAPE_SPLINE;

    return ret;
}

Core::String ERepresentationToString(Universe::EPartRepresentation rep)
{
    String ret;

    switch(rep)
    {
    case Universe::PR_BILLBOARD:
        ret = L"Billboard";
        break;
    case Universe::PR_GROUP:
        ret = L"Group";
        break;
    }

    return ret;
}

Universe::EPartRepresentation StringToERepresentation(const String& str)
{
    Universe::EPartRepresentation ret = Universe::PR_BILLBOARD;

    if(str == ERepresentationToString(Universe::PR_BILLBOARD))
        ret = Universe::PR_BILLBOARD;
    else if(str == ERepresentationToString(Universe::PR_GROUP))
        ret = Universe::PR_GROUP;

    return ret;
}

Core::String EEffectToString(Universe::EEffect effect)
{
    String ret;

    switch(effect)
    {
    case Universe::EFFECT_ANIMATE:
        ret = L"Animate";
        break;
    case Universe::EFFECT_ATTRACTOR:
        ret = L"Attractor";
        break;
    case Universe::EFFECT_FADE:
        ret = L"Fade";
        break;
    case Universe::EFFECT_FADE2:
        ret = L"Fade2";
        break;
    case Universe::EFFECT_FADE3:
        ret = L"Fade3";
        break;
    case Universe::EFFECT_FORCE:
        ret = L"Force";
        break;
    case Universe::EFFECT_RESISTANCE:
        ret = L"Resistance";
        break;
    case Universe::EFFECT_SPEED_LIMIT:
        ret = L"Speed limiter";
        break;
    case Universe::EFFECT_SIZE:
        ret = L"Size";
        break;
    case Universe::EFFECT_SIZE2:
        ret = L"Size2";
        break;
    case Universe::EFFECT_ROLL:
        ret = L"Roll";
        break;
    case Universe::EFFECT_TRANSLATE:
        ret = L"Translate";
        break;
    case Universe::EFFECT_COLOR_BY_POS:
        ret = L"Color by pos";
        break;
    case Universe::EFFECT_TURN:
        ret = L"Turn";
        break;
    }

    return ret;
}

Universe::EEffect StringToEEffect(const String& str)
{
    Universe::EEffect ret = Universe::EFFECT_ANIMATE;

    if(str == EEffectToString(Universe::EFFECT_ANIMATE))
        ret = Universe::EFFECT_ANIMATE;
    else if(str == EEffectToString(Universe::EFFECT_ATTRACTOR))
        ret = Universe::EFFECT_ATTRACTOR;
    else if(str == EEffectToString(Universe::EFFECT_FADE))
        ret = Universe::EFFECT_FADE;
    else if(str == EEffectToString(Universe::EFFECT_FADE2))
        ret = Universe::EFFECT_FADE2;
    else if(str == EEffectToString(Universe::EFFECT_FADE3))
        ret = Universe::EFFECT_FADE3;
    else if(str == EEffectToString(Universe::EFFECT_FORCE))
        ret = Universe::EFFECT_FORCE;
    else if(str == EEffectToString(Universe::EFFECT_RESISTANCE))
        ret = Universe::EFFECT_RESISTANCE;
    else if(str == EEffectToString(Universe::EFFECT_SPEED_LIMIT))
        ret = Universe::EFFECT_SPEED_LIMIT;
    else if(str == EEffectToString(Universe::EFFECT_SIZE))
        ret = Universe::EFFECT_SIZE;
    else if(str == EEffectToString(Universe::EFFECT_SIZE2))
        ret = Universe::EFFECT_SIZE2;
    else if(str == EEffectToString(Universe::EFFECT_ROLL))
        ret = Universe::EFFECT_ROLL;
    else if(str == EEffectToString(Universe::EFFECT_TRANSLATE))
        ret = Universe::EFFECT_TRANSLATE;
    else if(str == EEffectToString(Universe::EFFECT_COLOR_BY_POS))
        ret = Universe::EFFECT_COLOR_BY_POS;
    else if(str == EEffectToString(Universe::EFFECT_TURN))
        ret = Universe::EFFECT_TURN;

    return ret;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeEmitter::PtyNodeEmitter( const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeEmitter>& pNodeE, 
                                const Core::String& title)
:   PtyNode(pWorld, pNodeE, title),
    _shape(Universe::SHAPE_CONE),
    _representation(Universe::PR_BILLBOARD),
    _addEffectType(EEffectToString(Universe::EFFECT_FADE2))
{
    setVisibleWidgetTransform(true);
    setVisibleWidgetTag(true);
    setVisibleWidgetShow(true);

    Ptr<Property> pPtyAllEffets = Ptr<PropertyNode>(new PropertyNode(L"AllEffets", false, false, CONTENT));
    addChild(pPtyAllEffets);
    connect(pPtyAllEffets.get(), SIGNAL(propertyChanged()), this, SLOT(makeUpdateEffect()));

    _pPtyShape = Ptr<Property>(new PtyEmitterShapeCone(pNodeE, L"Bidon"));
    _pPtyRepresentation = Ptr<Property>(new PtyEmitterRepBillboard(pNodeE, L"Bidon"));

    //pour s'assurer la creation de la ptyShape on initialise la valeur du shape a une valeur aberrante
    _shape = (Universe::EShape)-1;  
    _representation = (Universe::EPartRepresentation)-1;  
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeEmitter::PtyNodeEmitter(const PtyNodeEmitter& other) :
    PtyNode(other)
{
    setVisibleWidgetTransform(true);
    setVisibleWidgetTag(true);
    setVisibleWidgetShow(true);

    _minPeriod     = other._minPeriod; 
    _maxPeriod     = other._maxPeriod;
    _minTimeMul    = other._minTimeMul;
    _maxTimeMul    = other._maxTimeMul;
    _maxCount      = other._maxCount;

    _startTime     = other._startTime;
    _endTime       = other._endTime;

    _burstStart    = other._burstStart;
    _minBurst      = other._minBurst;
    _maxBurst      = other._maxBurst;

    _stickyParts   = other._stickyParts;

    _addEffectType  = other._addEffectType;
    _shape          = other._shape;
    _representation = other._representation;

    _pPtyShape          = other._pPtyShape->clone();
    _pPtyShape->copyId(*other._pPtyShape);

    _pPtyRepresentation = other._pPtyRepresentation->clone();
    _pPtyRepresentation->copyId(*other._pPtyRepresentation);
}
//-----------------------------------------------------------------------------
PtyNodeEmitter::~PtyNodeEmitter()
{
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::makeUpdateEffect()
{
    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeEmitter::getAllEffets()
{
    return getChild(0);
}
//-----------------------------------------------------------------------------
bool PtyNodeEmitter::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if(id == _pPtyShape->getId())
    {
        pResult = _pPtyShape;
        return true;
    }
    else if(id ==  _pPtyRepresentation->getId())
    {
        pResult = _pPtyRepresentation;
        return true;
    }
    
    return PtyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeEmitter::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeEmitter> pPW (new PtyWidgetNodeEmitter(pDataProxy, parent));
    connect(getAllEffets().get(), SIGNAL(propertyChanged()), this, SLOT(makeUpdateEffect()));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::updateProperty()
{
    Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

    Core::List<Ptr<Universe::IPartEffect> > effects = pNodeEmitter->getEffects();
    getAllEffets()->removeAllChildren();
    Ptr<Property> pProperty = null;
    for(int32 iEffect = 0; iEffect < effects.size(); ++iEffect)
    {
        pProperty = null;
        switch(effects[iEffect]->getType())
        {
        case Universe::EFFECT_ANIMATE:
            pProperty = Ptr<PtyPartEffectAnimate>(new PtyPartEffectAnimate(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectAnimate>(effects[iEffect])));
            break;
        case Universe::EFFECT_ATTRACTOR:
            pProperty = Ptr<PtyPartEffectAttractor>(new PtyPartEffectAttractor(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectAttractor>(effects[iEffect])));
            break;
        case Universe::EFFECT_FADE:
            pProperty = Ptr<PtyPartEffectFade>(new PtyPartEffectFade(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectFade>(effects[iEffect])));
            break;
        case Universe::EFFECT_FADE2:
            pProperty = Ptr<PtyPartEffectFade2>(new PtyPartEffectFade2(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectFade2>(effects[iEffect])));
            break;
        case Universe::EFFECT_FADE3:
            pProperty = Ptr<PtyPartEffectFade3>(new PtyPartEffectFade3(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectFade3>(effects[iEffect])));
            break;
        case Universe::EFFECT_FORCE:
            pProperty = Ptr<PtyPartEffectForce>(new PtyPartEffectForce(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectForce>(effects[iEffect])));
            break;
        case Universe::EFFECT_RESISTANCE:
            pProperty = Ptr<PtyPartEffectResistance>(new PtyPartEffectResistance(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectResistance>(effects[iEffect])));
            break;
        case Universe::EFFECT_SPEED_LIMIT:
            pProperty = Ptr<PtyPartEffectSpeedLimiter>(new PtyPartEffectSpeedLimiter(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectSpeedLimiter>(effects[iEffect])));
            break;
        case Universe::EFFECT_SIZE:
            pProperty = Ptr<PtyPartEffectSize>(new PtyPartEffectSize(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectSize>(effects[iEffect])));
            break;
        case Universe::EFFECT_SIZE2:
            pProperty = Ptr<PtyPartEffectSize2>(new PtyPartEffectSize2(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectSize2>(effects[iEffect])));
            break;
        case Universe::EFFECT_ROLL:
            pProperty = Ptr<PtyPartEffectRoll>(new PtyPartEffectRoll(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectRoll>(effects[iEffect])));
            break;
        case Universe::EFFECT_TRANSLATE:
            pProperty = Ptr<PtyPartEffectTranslate>(new PtyPartEffectTranslate(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectTranslate>(effects[iEffect])));
            break;
        case Universe::EFFECT_COLOR_BY_POS:
            pProperty = Ptr<PtyPartEffectColorByPos>(new PtyPartEffectColorByPos(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectColorByPos>(effects[iEffect])));
            break;
        case Universe::EFFECT_TURN:
            pProperty = Ptr<PtyPartEffectTurn>(new PtyPartEffectTurn(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartEffectTurn>(effects[iEffect])));
            break;
        }

        if(pProperty != null)
        {
            getAllEffets()->addChild(pProperty);
        }
    }

    pNodeEmitter->getPeriod        (_minPeriod, _maxPeriod);
    pNodeEmitter->getTimeMul       (_minTimeMul, _maxTimeMul);
    pNodeEmitter->getGenerationTime(_startTime, _endTime);
    pNodeEmitter->getBurstLifeTime (_minBurst,  _maxBurst);

    _maxCount    = pNodeEmitter->getMaxCount();
    _burstStart  = pNodeEmitter->burstStart();
    _stickyParts = pNodeEmitter->getStickyParticles();
    
    if(pNodeEmitter->getShape()->getType() != _shape)
    {
        _shape = pNodeEmitter->getShape()->getType();

        if(_shape == Universe::SHAPE_CONE)
        {
            _pPtyShape = Ptr<PtyEmitterShapeCone>(new PtyEmitterShapeCone(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::EmitterShapeCone>(pNodeEmitter->getShape())));
        }
        else if(_shape == Universe::SHAPE_MODEL)
        {
            _pPtyShape = Ptr<PtyEmitterShapeModel>(new PtyEmitterShapeModel(pNodeEmitter, getUniverseWorld()->getRessourcesPool(), LM_DEBUG_PTR_CAST<Universe::EmitterShapeModel>(pNodeEmitter->getShape())));
        }
        else if(_shape == Universe::SHAPE_BOX)
        {
            _pPtyShape = Ptr<PtyEmitterShapeBox>(new PtyEmitterShapeBox(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::EmitterShapeBox>(pNodeEmitter->getShape())));
        }
        else if(_shape == Universe::SHAPE_SPLINE)
        {
            _pPtyShape = Ptr<PtyEmitterShapeSpline>(new PtyEmitterShapeSpline(pNodeEmitter, getUniverseWorld()->getRessourcesPool(), LM_DEBUG_PTR_CAST<Universe::EmitterShapeSpline>(pNodeEmitter->getShape())));
        }
    }

    if(pNodeEmitter->getRepresentation()->getType() != _representation)
    {
        _representation = pNodeEmitter->getRepresentation()->getType();

        if(_representation == Universe::PR_BILLBOARD)
            _pPtyRepresentation = Ptr<PtyEmitterRepBillboard>(new PtyEmitterRepBillboard(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartRepresentationBillboard>(pNodeEmitter->getRepresentation())));
        else if(_representation == Universe::PR_GROUP)
            _pPtyRepresentation = Ptr<PtyEmitterRepGroup>(new PtyEmitterRepGroup(pNodeEmitter, LM_DEBUG_PTR_CAST<Universe::PartRepresentationGroup>(pNodeEmitter->getRepresentation())));
    }
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::updateData()
{
    PtyNode::updateData();
    
    Ptr<PtyEmitterShape> pShape = LM_DEBUG_PTR_CAST<PtyEmitterShape>(_pPtyShape);
    Ptr<PtyEmitterRep> pRep = LM_DEBUG_PTR_CAST<PtyEmitterRep>(_pPtyRepresentation);

    Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

    pNodeEmitter->setPeriod(_minPeriod, _maxPeriod);
    pNodeEmitter->setTimeMul(_minTimeMul, _maxTimeMul);
    pNodeEmitter->setMaxCount(_maxCount);
    pNodeEmitter->setGenerationTime(_startTime, _endTime);
    pNodeEmitter->burstStart(_burstStart);
    pNodeEmitter->setBurstLifeTime(_minBurst, _maxBurst);
    pNodeEmitter->setStickyParticles(_stickyParts);

    pNodeEmitter->setShape(pShape->getShape());
    pNodeEmitter->setRepresentation(pRep->getRepresentation());


    pNodeEmitter->clearEffects();
    Ptr<Property> pAllEffets = getAllEffets();
    for(int32 iProperty = 0; iProperty < pAllEffets->getNbChildren(); ++iProperty)
    {
        Ptr<PtyPartEffect> pPtyEffedt = LM_DEBUG_PTR_CAST<PtyPartEffect> (pAllEffets->getChild(iProperty));

        pNodeEmitter->addEffect(pPtyEffedt->getEffect());
    }
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);

    Ptr<Universe::NodeEmitter> pNodeE = LM_DEBUG_PTR_CAST<Universe::NodeEmitter> (getUniverseNode());

    Ptr<Property>   pPtyAllEffet = getAllEffets();

    for (int32 ii=0; ii<pPtyAllEffet->getNbChildren(); ++ii)
    {
        Ptr<PtyPartEffect> pPtyEffect = LM_DEBUG_PTR_CAST<PtyPartEffect> (pPtyAllEffet->getChild(ii));
        pPtyEffect->setUniverseNodeEmitter(pNodeE);
    }

    Ptr<PtyEmitterShape> pShape = LM_DEBUG_PTR_CAST<PtyEmitterShape> (_pPtyShape);
    pShape->setUniverseNodeEmitter(pNodeE);
    pShape->resurrect(getUniverseWorld(), pWorldInfoContent);

    Ptr<PtyEmitterRep> pRepresentation = LM_DEBUG_PTR_CAST<PtyEmitterRep> (_pPtyRepresentation);
    pRepresentation->setUniverseNodeEmitter(pNodeE);
    pRepresentation->resurrect(getUniverseWorld(), pWorldInfoContent);
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::updateShape(Ptr<StateRecorder> pStateRecorder)
{
    LM_ASSERT(pStateRecorder!=null);

    Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

    if(_shape == Universe::SHAPE_CONE)
    {
        _pPtyShape = Ptr<PtyEmitterShapeCone>(new PtyEmitterShapeCone(pNodeEmitter));
    }
    else if(_shape == Universe::SHAPE_MODEL)
    {
        _pPtyShape = Ptr<PtyEmitterShapeModel>(new PtyEmitterShapeModel(pNodeEmitter, getUniverseWorld()->getRessourcesPool()));
    }
    else if(_shape == Universe::SHAPE_BOX)
    {
        _pPtyShape = Ptr<PtyEmitterShapeBox>(new PtyEmitterShapeBox(pNodeEmitter));
    }
    else if(_shape == Universe::SHAPE_SPLINE)
    {
        _pPtyShape = Ptr<PtyEmitterShapeSpline>(new PtyEmitterShapeSpline(pNodeEmitter, getUniverseWorld()->getRessourcesPool()));
    }
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::updateRepresentation(Ptr<StateRecorder> pStateRecorder)
{
    LM_ASSERT(pStateRecorder!=null);

    Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

    if(_representation == Universe::PR_BILLBOARD)
    {
        _pPtyRepresentation = Ptr<PtyEmitterRepBillboard>(new PtyEmitterRepBillboard(pNodeEmitter));
    }
    else if(_representation == Universe::PR_GROUP)
    {
        _pPtyRepresentation = Ptr<PtyEmitterRepGroup>(new PtyEmitterRepGroup(pNodeEmitter));
    }
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeEmitter::clone() const
{
    Ptr<PtyNodeEmitter> pRet (new PtyNodeEmitter( *this ));

    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::setUniverseNode(const Ptr<Universe::Node>& pNode)
{
    PtyNode::setUniverseNode(pNode);

    Ptr<Universe::NodeEmitter> pEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pNode);
    LM_DEBUG_PTR_CAST<PtyEmitterShape>(_pPtyShape)->setUniverseNodeEmitter(pEmitter);
    LM_DEBUG_PTR_CAST<PtyEmitterRep>(_pPtyRepresentation)->setUniverseNodeEmitter(pEmitter);
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeEmitter> pPty = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(pSrc);

    _minPeriod      = pPty->_minPeriod;
    _maxPeriod      = pPty->_maxPeriod;
    _minTimeMul     = pPty->_minTimeMul;
    _maxTimeMul     = pPty->_maxTimeMul;
    _maxCount       = pPty->_maxCount;
    _startTime      = pPty->_startTime;
    _endTime        = pPty->_endTime;
    _burstStart     = pPty->_burstStart;
    _minBurst       = pPty->_minBurst;
    _maxBurst       = pPty->_maxBurst;
    _stickyParts    = pPty->_stickyParts;

    _addEffectType  = pPty->_addEffectType;
    _shape          = pPty->_shape;
    _representation = pPty->_representation;

    Ptr<PtyEmitterShape> pShapePtr    = LM_DEBUG_PTR_CAST<PtyEmitterShape> (_pPtyShape);
    Ptr<PtyEmitterShape> pShapePtrSrc = LM_DEBUG_PTR_CAST<PtyEmitterShape> (pPty->_pPtyShape);
    if (pShapePtr->getShapeType() == pShapePtrSrc->getShapeType())
    {
        _pPtyShape->copy(pPty->_pPtyShape);
    }
    else
    {
        _pPtyShape = pPty->_pPtyShape->clone();
        _pPtyShape->resurrect(getUniverseWorld());
        _pPtyShape->copyId(*pPty->_pPtyShape);
    }
 
    Ptr<PtyEmitterRep> pRepresentationPtr    = LM_DEBUG_PTR_CAST<PtyEmitterRep> (_pPtyRepresentation);
    Ptr<PtyEmitterRep> pRepresentationPtrSrc = LM_DEBUG_PTR_CAST<PtyEmitterRep> (pPty->_pPtyRepresentation);
    if (pRepresentationPtr->getRepresentationType() == pRepresentationPtrSrc->getRepresentationType())
    {
        _pPtyRepresentation->copy(pPty->_pPtyRepresentation);
    }
    else
    {
        _pPtyRepresentation = pPty->_pPtyRepresentation->clone();
        _pPtyRepresentation->resurrect(getUniverseWorld());
        _pPtyRepresentation->copyId(*pPty->_pPtyRepresentation);
    }

    updateData();
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::addEffect(const Core::String & effectName, Ptr<StateRecorder>& pStateRecorder)
{
    Universe::EEffect effectType = StringToEEffect(effectName);
    Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

    Ptr<Universe::IPartEffect> pIPartEffect = null;
    Ptr<Property> pProperty = null;

    switch(effectType)
    {
    case Universe::EFFECT_ANIMATE:
        pIPartEffect = Ptr<Universe::PartEffectAnimate>(new Universe::PartEffectAnimate(0.0f, 1.0f, 3.0f, true, true));
        pProperty = Ptr<PtyPartEffectAnimate>(new PtyPartEffectAnimate(pNodeEmitter));
        break;
    case Universe::EFFECT_ATTRACTOR:
        pIPartEffect = Ptr<Universe::PartEffectAttractor>(new Universe::PartEffectAttractor(Core::Vector3f(0.0f, 0.0f, 0.0f), 0.0f));
        pProperty = Ptr<PtyPartEffectAttractor>(new PtyPartEffectAttractor(pNodeEmitter));
        break;
    case Universe::EFFECT_FADE:
        pIPartEffect = Ptr<Universe::PartEffectFade>(new Universe::PartEffectFade(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true));
        pProperty = Ptr<PtyPartEffectFade>(new PtyPartEffectFade(pNodeEmitter));
        break;
    case Universe::EFFECT_FADE2:
        pProperty = Ptr<PtyPartEffectFade2>(new PtyPartEffectFade2(pNodeEmitter));
        break;
    case Universe::EFFECT_FADE3:
        pIPartEffect = Ptr<Universe::PartEffectFade3>(new Universe::PartEffectFade3(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, 1.0f, true, false));
        pProperty = Ptr<PtyPartEffectFade3>(new PtyPartEffectFade3(pNodeEmitter));
        break;
    case Universe::EFFECT_FORCE:
        pIPartEffect = Ptr<Universe::PartEffectForce>(new Universe::PartEffectForce(Core::Vector3f(0.0f, 1.0f, 0.0f), 1.0f));
        pProperty = Ptr<PtyPartEffectForce>(new PtyPartEffectForce(pNodeEmitter));
        break;
    case Universe::EFFECT_RESISTANCE:
        pIPartEffect = Ptr<Universe::PartEffectResistance>(new Universe::PartEffectResistance(1.0f));
        pProperty = Ptr<PtyPartEffectResistance>(new PtyPartEffectResistance(pNodeEmitter));
        break;
    case Universe::EFFECT_SPEED_LIMIT:
        pIPartEffect = Ptr<Universe::PartEffectSpeedLimiter>(new Universe::PartEffectSpeedLimiter(1.0f));
        pProperty = Ptr<PtyPartEffectSpeedLimiter>(new PtyPartEffectSpeedLimiter(pNodeEmitter));
        break;
    case Universe::EFFECT_SIZE:
        pIPartEffect = Ptr<Universe::PartEffectSize>(new Universe::PartEffectSize(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
        pProperty = Ptr<PtyPartEffectSize>(new PtyPartEffectSize(pNodeEmitter));
        break;
    case Universe::EFFECT_SIZE2:
        pIPartEffect = Ptr<Universe::PartEffectSize2>(new Universe::PartEffectSize2(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
        pProperty = Ptr<PtyPartEffectSize2>(new PtyPartEffectSize2(pNodeEmitter));
        break;
    case Universe::EFFECT_ROLL:
        pIPartEffect = Ptr<Universe::PartEffectRoll>(new Universe::PartEffectRoll(0.0f, 0.0f));
        pProperty = Ptr<PtyPartEffectRoll>(new PtyPartEffectRoll(pNodeEmitter));
        break;
    case Universe::EFFECT_TRANSLATE:
        pIPartEffect = Ptr<Universe::PartEffectTranslate>(new Universe::PartEffectTranslate());
        pProperty = Ptr<PtyPartEffectTranslate>(new PtyPartEffectTranslate(pNodeEmitter));
        break;
    case Universe::EFFECT_COLOR_BY_POS:
        pIPartEffect = Ptr<Universe::PartEffectColorByPos>(new Universe::PartEffectColorByPos());
        pProperty = Ptr<PtyPartEffectColorByPos>(new PtyPartEffectColorByPos(pNodeEmitter));
        break;
    case Universe::EFFECT_TURN:
        pIPartEffect = Ptr<Universe::PartEffectTurn>(new Universe::PartEffectTurn());
        pProperty = Ptr<PtyPartEffectTurn>(new PtyPartEffectTurn(pNodeEmitter));
        break;
    }

    if(pProperty != null)
    {
        Ptr<Universe::NodeEmitter>  pNodeEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(getUniverseNode());

        addChildRecording       (pStateRecorder, pProperty, getAllEffets());
    }

    updateData();
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::removeEffect(int32 iEffect, Ptr<StateRecorder>& pStateRecorder)
{
    Ptr<Property> pEffectToRemove = getAllEffets()->getChild(iEffect);
    removeChildRecording (pStateRecorder, pEffectToRemove);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyNodeEmitter::copyId(const Property& pSrc)
{
    PtyNode::copyId(pSrc);

    const PtyNodeEmitter& other = (const PtyNodeEmitter&)pSrc;
    _pPtyShape->copyId(*other._pPtyShape);
    _pPtyRepresentation->copyId(*other._pPtyRepresentation);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeEmitter::PtyWidgetNodeEmitter(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent),
    _ptyWidgetShape(null)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeEmitter::~PtyWidgetNodeEmitter()
{}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::readProperty()
{
    Ptr<PtyNodeEmitter> pP = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    int32 minFrequency = 1000;
    int32 maxFrequency = 1000;
    if(pP->_minPeriod != 0)
        maxFrequency = 1 / pP->_minPeriod;
    if(pP->_maxPeriod != 0)
        minFrequency = 1 / pP->_maxPeriod;
    _frequency->setRangedValue(minFrequency, maxFrequency);
    _timeMul->setRangedValue(pP->_minTimeMul, pP->_maxTimeMul);
    _maxCount->setSingleValue(pP->_maxCount);
    _generationTime->setRangedValue(pP->_startTime, pP->_endTime);
    _burstStart->setChecked(pP->_burstStart);
    _burst->setRangedValue(pP->_minBurst, pP->_maxBurst);
    _stickyParts->setChecked(pP->_stickyParts);

    _shape->selectItem(Core::String8(EShapeToString(pP->_shape)).c_str());
    _representation->selectItem(Core::String8(ERepresentationToString(pP->_representation)).c_str());

    _addEffectType->selectItem(String8(pP->_addEffectType).c_str());

    updatePtyWidgetShape();
    updatePtyWidgetRepresentation();
    updatePtyWidgetNodeEmitter();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeEmitter> pP = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());

    float minFrequency = 1.0;
    float maxFrequency = 1.0;
    _frequency->getRangedValue(minFrequency, maxFrequency);
    pP->_minPeriod = 1 / (double)maxFrequency;
    pP->_maxPeriod = 1 / (double)minFrequency;
    
    _timeMul->getRangedValue(pP->_minTimeMul, pP->_maxTimeMul);
    _maxCount->getSingleValue(pP->_maxCount);
    _generationTime->getRangedValue(pP->_startTime, pP->_endTime);
    _burst->getRangedValue(pP->_minBurst, pP->_maxBurst);
    pP->_burstStart  = _burstStart->isChecked();
    pP->_stickyParts = _stickyParts->isChecked();

    if(pWidget == NULL || pWidget == _shape)
    {
        pP->_shape = StringToEShape(_shape->selectedText());
        pP->updateData();
        pP->updateShape(getDataProxy()->getStateRecorder());

        updatePtyWidgetShape();
    }

    if(pWidget == NULL || pWidget == _representation)
    {
        pP->_representation = StringToERepresentation(_representation->selectedText());
        pP->updateData();
        pP->updateRepresentation(getDataProxy()->getStateRecorder());

        updatePtyWidgetRepresentation();
    }

    if(pWidget == NULL || pWidget == _addEffectType)
    {
         pP->_addEffectType = _addEffectType->selectedText();
    }
    
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _restartEmitter = new QPushButton(QIcon(":/icons/refresh.png"), "Restart emitter", this);
        _restartEmitter->setFixedSize(110, 20);

    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Node emitter", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _groupBoxShape          = new QtToolbox::CollapsibleWidget(this, "Shape");
    _groupBoxGeneration     = new QtToolbox::CollapsibleWidget(this, "Generation");
    _groupBoxRepresentation = new QtToolbox::CollapsibleWidget(this, "Representation");

    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);
        layoutGroupBox->setSpacing(0);
        _groupBox->setLayout(layoutGroupBox);

        Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);
    _pPWNode->collapse(true);

    _frequency      = new QtToolbox::RangedSlidingHDR(this, "Frequency", false);
    _maxCount       = new QtToolbox::SingleSlidingInt(this, "Max count", 1, 2000);
    _timeMul        = new QtToolbox::RangedSlidingHDR(this, "Time mul", false);

    _generationTime = new QtToolbox::RangedSlidingTimeValue(this, "Generation", 0.0, 60.0);
    _burstStart     = new QCheckBox("Burst start", this);
    _burst          = new QtToolbox::RangedSlidingHDR(this, "Burst life time", false);
    _stickyParts    = new QCheckBox("Sticky particles", this);
   
    _shape          = new QtToolbox::ComboBox(this, "Shape");
    _shape->addItem(Core::String8(EShapeToString(Universe::SHAPE_CONE)).c_str());
    _shape->addItem(Core::String8(EShapeToString(Universe::SHAPE_BOX)).c_str());
    _shape->addItem(Core::String8(EShapeToString(Universe::SHAPE_SPLINE)).c_str());
    _shape->addItem(Core::String8(EShapeToString(Universe::SHAPE_MODEL)).c_str());

    _representation = new QtToolbox::ComboBox(this, "Representation");
    _representation->addItem(Core::String8(ERepresentationToString(Universe::PR_BILLBOARD)).c_str());
    _representation->addItem(Core::String8(ERepresentationToString(Universe::PR_GROUP)).c_str());

    _groupBoxShape->getLayout()->addWidget(_shape);
    _groupBoxRepresentation->getLayout()->addWidget(_representation);

    _groupBoxGeneration->getLayout()->addWidget(_frequency);
    _groupBoxGeneration->getLayout()->addWidget(_maxCount);
    _groupBoxGeneration->getLayout()->addWidget(_timeMul);
    _groupBoxGeneration->getLayout()->addWidget(_generationTime);
    _groupBoxGeneration->getLayout()->addWidget(_burstStart);
    _groupBoxGeneration->getLayout()->addWidget(_burst);
    _groupBoxGeneration->getLayout()->addWidget(_stickyParts);

    _addEffectBar = new QWidget(this);
    _addEffectLayout = new QHBoxLayout(this);
    _addEffectLayout->setSpacing(1);
    _addEffectLayout->setContentsMargins(0, 0, 0, 0);

    _addEffectBar->setFixedHeight(20);
    _addEffectBar->setFixedWidth(250);
    _addEffectButton = new QPushButton(QIcon(":/icons/add.png"), "", this);
    _addEffectType = new QtToolbox::ComboBox(this, "Effect type :");
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_ANIMATE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_ATTRACTOR)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_COLOR_BY_POS)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_FADE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_FADE2)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_FADE3)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_FORCE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_RESISTANCE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_ROLL)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_SIZE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_SIZE2)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_SPEED_LIMIT)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_TRANSLATE)).c_str());
    _addEffectType->addItem(Core::String8(EEffectToString(Universe::EFFECT_TURN)).c_str());
    
    _addEffectType->setMaxVisibleItems(20);

    _addEffectLayout->addWidget(_addEffectButton);
    _addEffectLayout->addWidget(_addEffectType);

    _addEffectBar->setLayout(_addEffectLayout);
 
    layoutGroupBox->addWidget(_restartEmitter, 0, 0, Qt::AlignCenter);
    layoutGroupBox->addWidget(_pPWNode.get());
    layoutGroupBox->addWidget(_groupBoxShape);
    layoutGroupBox->addWidget(_groupBoxGeneration);
    layoutGroupBox->addWidget(_groupBoxRepresentation);

    _layout->addWidget(_groupBox);
    _layout->addWidget(_addEffectBar);

    _layout->setAlignment(Qt::AlignTop);

    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_frequency);
    getWidgetsForUndoRedo().push_back(_maxCount);
    getWidgetsForUndoRedo().push_back(_timeMul);
    getWidgetsForUndoRedo().push_back(_generationTime);
    getWidgetsForUndoRedo().push_back(_burstStart);
    getWidgetsForUndoRedo().push_back(_burst);
    getWidgetsForUndoRedo().push_back(_stickyParts);
    getWidgetsForUndoRedo().push_back(_shape);
    getWidgetsForUndoRedo().push_back(_representation);
    getWidgetsForUndoRedo().push_back(_addEffectType);

    PropertyWidget::setupUi();

    connect(_addEffectButton, SIGNAL(clicked()), this, SLOT(addEffect()));
    connect(_restartEmitter, SIGNAL(clicked()), this, SLOT(restartEmitter()));

    updatePtyWidgetNodeEmitter();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::updatePtyWidgetShape()
{
    if (_ptyWidgetShape!=null)
    {
        _groupBoxShape->getLayout()->removeWidget(_ptyWidgetShape.get());
        _ptyWidgetShape = null;
    }

    Ptr<PtyNodeEmitter> pP = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
    Ptr<Property>   pShape = pP->_pPtyShape;

    if (pShape != null)
    {
        Ptr<PropertyWidgetDataProxy> pDataPWidgetShape(new PropertyWidgetDataProxy(pShape, pShape->getName(), getDataProxy()->getStateRecorder()));
        _ptyWidgetShape = pShape->createPropertyWidget(pDataPWidgetShape, this);
        _groupBoxShape->getLayout()->addWidget(_ptyWidgetShape.get());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::updatePtyWidgetRepresentation()
{
    if(_ptyWidgetRepresentation != null)
    {
        _groupBoxRepresentation->getLayout()->removeWidget(_ptyWidgetRepresentation.get());
        _ptyWidgetRepresentation = null;
    }

    Ptr<PtyNodeEmitter> pP = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
    Ptr<Property>   pRep = pP->_pPtyRepresentation;

    if(pRep != null)
    {
        Ptr<PropertyWidgetDataProxy> pDataPWidgetRep(new PropertyWidgetDataProxy(pRep, pRep->getName(), getDataProxy()->getStateRecorder()));
        _ptyWidgetRepresentation = pRep->createPropertyWidget(pDataPWidgetRep, this);
        _groupBoxRepresentation->getLayout()->addWidget(_ptyWidgetRepresentation.get());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::restartEmitter()
{
    Ptr<PtyNodeEmitter> pPtyNodeEmitter = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
    pPtyNodeEmitter->restart();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::deletePtyWidgetEffect(QWidget * pEffect)
{
    //On recherche la widget correspondant ‡ l'effet ‡ supprimer
    bool found = false;
    for(int32 iEffect = 0; !found && iEffect < _effects.size(); ++iEffect)
    {
        if(_effects[iEffect].get() == pEffect)
        {
            found = true;
            Ptr<PtyNodeEmitter> pPtyNodeEmitter = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
            pPtyNodeEmitter->removeEffect(iEffect, getDataProxy()->getStateRecorder());

            updatePropertyWidget();
        }
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::addEffect()
{
    Universe::EEffect effectType = StringToEEffect(_addEffectType->selectedText());

    Ptr<PtyNodeEmitter> pPtyNodeEmitter = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());
    pPtyNodeEmitter->addEffect(_addEffectType->selectedText(), getDataProxy()->getStateRecorder());

    updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeEmitter::updatePtyWidgetNodeEmitter()
{
    Ptr<PtyNodeEmitter> pPtyNodeEmitter = LM_DEBUG_PTR_CAST<PtyNodeEmitter>(getDataProxy()->getProperty());

    for(int32 iEffect = 0; iEffect < _effects.size(); ++iEffect)
    {
        _layout->removeWidget(_effects[iEffect].get());
    }

    _effects.clear();

   
    Ptr<Property>   pAllEffets = pPtyNodeEmitter->getAllEffets();
    for(int32 iPtyChild = 0; iPtyChild<pAllEffets->getNbChildren(); ++iPtyChild)
    {
        Ptr<Property> pChild = pAllEffets->getChild(iPtyChild);
        Ptr<PropertyWidgetDataProxy> pPWidgetDataProxy (new PropertyWidgetDataProxy(pChild, pChild->getName(), getDataProxy()->getStateRecorder()));
        _effects.push_back(pChild->createPropertyWidget(pPWidgetDataProxy, this));
        connect(_effects.back().get(), SIGNAL(deletePtyWidgetEffect(QWidget *)), this, SLOT(deletePtyWidgetEffect(QWidget *)));
        _effects.back()->updatePropertyWidget();
    }

    for(int32 iEffect = 0; iEffect < _effects.size(); ++iEffect)
    {
        _layout->addWidget(_effects[iEffect].get());
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI