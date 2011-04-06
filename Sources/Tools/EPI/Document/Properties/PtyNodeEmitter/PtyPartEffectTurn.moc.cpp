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
#include "PtyPartEffectTurn.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>
#include <QCheckBox>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectTurn::PtyPartEffectTurn( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectTurn>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectTurn::PtyPartEffectTurn(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectTurn>(new Universe::PartEffectTurn()),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectTurn::~PtyPartEffectTurn()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectTurn::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectTurn> pPW (new PtyWidgetPartEffectTurn(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectTurn::updateProperty()
{
    Ptr<Universe::PartEffectTurn>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectTurn> (getEffect());

    _axis = pEffet->getAxis();
    _speed = pEffet->getRadialSpeed();
    _randSpeed = pEffet->getRandSpeed();
    _aroundCenter = pEffet->isAroundCenter();
}
//-----------------------------------------------------------------------------
void PtyPartEffectTurn::updateData()
{
    Ptr<Universe::PartEffectTurn>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectTurn> (getEffect());

    pEffet->setAxis(_axis);
    pEffet->setRadialSpeed(_speed);
    pEffet->setRandSpeed(_randSpeed);
    pEffet->setAroundCenter(_aroundCenter);
}
//-----------------------------------------------------------------------------
void PtyPartEffectTurn::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectTurn>(new Universe::PartEffectTurn());
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectTurn::clone() const
{
    return Ptr<Property>(new PtyPartEffectTurn( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectTurn::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectTurn> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectTurn>(pSrc);

    _axis = pPty->_axis;
    _speed = pPty->_speed;
    _randSpeed = pPty->_randSpeed;
    _aroundCenter = pPty->_aroundCenter;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectTurn::PtyWidgetPartEffectTurn(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectTurn::~PtyWidgetPartEffectTurn()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTurn::readProperty()
{
    Ptr<PtyPartEffectTurn> pP = LM_DEBUG_PTR_CAST<PtyPartEffectTurn>(getDataProxy()->getProperty());

    _axisX->setSingleValue(pP->_axis.x);
    _axisY->setSingleValue(pP->_axis.y);
    _axisZ->setSingleValue(pP->_axis.z);
    _speed->setSingleValue(pP->_speed);
    _randSpeed->setSingleValue(pP->_randSpeed);
    _aroundCenter->setChecked(pP->_aroundCenter);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTurn::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectTurn> pP = LM_DEBUG_PTR_CAST<PtyPartEffectTurn>(getDataProxy()->getProperty());

    _axisX->getSingleValue(pP->_axis.x);
    _axisY->getSingleValue(pP->_axis.y);
    _axisZ->getSingleValue(pP->_axis.z);
    _speed->getSingleValue(pP->_speed);
    _randSpeed->getSingleValue(pP->_randSpeed);
    pP->_aroundCenter = _aroundCenter->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTurn::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Turn effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _axisX      = new QtToolbox::SingleSlidingHDR(this, "Axis X", true);
    _axisY      = new QtToolbox::SingleSlidingHDR(this, "Axis Y", true);
    _axisZ      = new QtToolbox::SingleSlidingHDR(this, "Axis Z", true);
    _speed      = new QtToolbox::SingleSlidingHDR(this, "Radial speed", true);
    _randSpeed  = new QtToolbox::SingleSlidingHDR(this, "Rand. speed", true);
    _aroundCenter = new QCheckBox("Turn around center", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_axisX);
    _groupBox->getLayout()->addWidget(_axisY);
    _groupBox->getLayout()->addWidget(_axisZ);
    _groupBox->getLayout()->addWidget(_speed);
    _groupBox->getLayout()->addWidget(_randSpeed);
    _groupBox->getLayout()->addWidget(_aroundCenter);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_axisX);
    getWidgetsForUndoRedo().push_back(_axisY);
    getWidgetsForUndoRedo().push_back(_axisZ);
    getWidgetsForUndoRedo().push_back(_speed);
    getWidgetsForUndoRedo().push_back(_randSpeed);
    getWidgetsForUndoRedo().push_back(_aroundCenter);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTurn::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI