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
#include "PtyEmitterShapeCone.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>

#include <QCheckBox>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyEmitterShapeCone::PtyEmitterShapeCone(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::EmitterShapeCone>& pShape,
    const Core::String& title):
    PtyEmitterShape(pNodeE, pShape, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeCone::PtyEmitterShapeCone(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Core::String& title):
    PtyEmitterShape(pNodeE, Ptr<Universe::EmitterShapeCone> (new Universe::EmitterShapeCone()), title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
/*PtyEmitterShapeCone::PtyEmitterShapeCone(const PtyEmitterShapeCone & other)
    :PtyEmitterShape(other.getUniverseNodeEmitter(), Ptr<Universe::EmitterShapeCone> (new Universe::EmitterShapeCone()), L"Shape Cone")
{
    _minRadius = other._minRadius;
    _maxRadius = other._maxRadius;
    _minAngle = other._minAngle;
    _maxAngle = other._maxAngle;
    _minSpeed = other._minSpeed;
    _maxSpeed = other._maxSpeed;
    _tangentialSpeed = other._tangentialSpeed;

    updateData();
}*/
//-----------------------------------------------------------------------------
PtyEmitterShapeCone::~PtyEmitterShapeCone()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterShapeCone::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterShapeCone> pPW (new PtyWidgetEmitterShapeCone(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeCone::updateProperty()
{
    Ptr<Universe::EmitterShapeCone> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeCone>(getShape());

    pShape->getRadius(_minRadius, _maxRadius);
    pShape->getAngle(_minAngle, _maxAngle);
    pShape->getSpeed(_minSpeed, _maxSpeed);
    _tangentialSpeed = pShape->getTangentialSpeed();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeCone::updateData()
{
    Ptr<Universe::EmitterShapeCone> pShapeCone = LM_DEBUG_PTR_CAST<Universe::EmitterShapeCone>(getShape());

    pShapeCone->setRadius(_minRadius, _maxRadius);
    pShapeCone->setAngle(_minAngle, _maxAngle);
    pShapeCone->setSpeed(_minSpeed, _maxSpeed);
    pShapeCone->setTangentialSpeed(_tangentialSpeed);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterShapeCone::clone() const
{
    return Ptr<Property>(new PtyEmitterShapeCone( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeCone::internalCopy(const Ptr<Property>& pSrc)
{
    PtyEmitterShape::internalCopy(pSrc);
    Ptr<PtyEmitterShapeCone> pPty = LM_DEBUG_PTR_CAST<PtyEmitterShapeCone>(pSrc);

    _minRadius = pPty->_minRadius;
    _maxRadius = pPty->_maxRadius;
    _minAngle = pPty->_minAngle;
    _maxAngle = pPty->_maxAngle;
    _minSpeed = pPty->_minSpeed;
    _maxSpeed = pPty->_maxSpeed;
    _tangentialSpeed = pPty->_tangentialSpeed;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeCone::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getShape()==null);

    Ptr<Universe::EmitterShapeCone> pShape = Ptr<Universe::EmitterShapeCone> (new Universe::EmitterShapeCone());
    
    setShape(pShape);
    getUniverseNodeEmitter()->setShape(getShape());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeCone::PtyWidgetEmitterShapeCone(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeCone::~PtyWidgetEmitterShapeCone()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeCone::readProperty()
{
    Ptr<PtyEmitterShapeCone> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeCone>(getDataProxy()->getProperty());

    _radius->setRangedValue(pP->_minRadius, pP->_maxRadius);
    _angle->setRangedValue(Core::rad2deg(pP->_minAngle), Core::rad2deg(pP->_maxAngle));
    _speed->setRangedValue(pP->_minSpeed, pP->_maxSpeed);
    _tangentialSpeed->setChecked(pP->_tangentialSpeed);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeCone::writeProperty(QWidget* pWidget)
{
    Ptr<PtyEmitterShapeCone> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeCone>(getDataProxy()->getProperty());

    double minRadius = 0.0;
    double maxRadius = 0.0;
    _radius->getRangedValue(minRadius, maxRadius);
    pP->_minRadius = minRadius;
    pP->_maxRadius = maxRadius;
    
    double minAngle = 0.0;
    double maxAngle = 0.0;
    _angle->getRangedValue(minAngle, maxAngle);
    pP->_minAngle = Core::deg2rad(minAngle);
    pP->_maxAngle = Core::deg2rad(maxAngle);

    double minSpeed = 0.0;
    double maxSpeed = 0.0;
    _speed->getRangedValue(minSpeed, maxSpeed);
    pP->_minSpeed = minSpeed;
    pP->_maxSpeed = maxSpeed;
    pP->_tangentialSpeed = _tangentialSpeed->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeCone::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _radius = new QtToolbox::RangedSlidingHDR(this, "Radius", false);
    _angle  = new QtToolbox::RangedSlidingDouble(this, "Angle", 0.0, Core::rad2deg(d_PI));
    _speed  = new QtToolbox::RangedSlidingHDR(this, "Speed", true);
    _tangentialSpeed = new QCheckBox("Tangential speed", this);

    _layout->addWidget(_radius);
    _layout->addWidget(_angle);
    _layout->addWidget(_speed);
    _layout->addWidget(_tangentialSpeed);

    setLayout(_layout);

    readProperty();

    getWidgetsForUndoRedo().push_back(_radius);
    getWidgetsForUndoRedo().push_back(_angle);
    getWidgetsForUndoRedo().push_back(_speed);
    getWidgetsForUndoRedo().push_back(_tangentialSpeed);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI