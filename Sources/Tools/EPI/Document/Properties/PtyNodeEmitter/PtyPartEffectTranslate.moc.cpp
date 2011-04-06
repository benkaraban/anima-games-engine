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
#include "PtyPartEffectTranslate.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectTranslate::PtyPartEffectTranslate( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                                const Ptr<Universe::PartEffectTranslate>& pEffect,
                                                const Core::String& title)
:   PtyPartEffect(pNodeE, pEffect, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectTranslate::PtyPartEffectTranslate(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(
        pNodeE,
        Ptr<Universe::PartEffectTranslate>(new Universe::PartEffectTranslate()),
        title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectTranslate::~PtyPartEffectTranslate()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectTranslate::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectTranslate> pPW (new PtyWidgetPartEffectTranslate(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectTranslate::updateData()
{
    Ptr<Universe::PartEffectTranslate>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectTranslate> (getEffect());

    pEffet->setConstSpeed(_constSpeed);
    pEffet->setRandSpeed(_randSpeed);
}
//-----------------------------------------------------------------------------
void PtyPartEffectTranslate::updateProperty()
{
    Ptr<Universe::PartEffectTranslate>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectTranslate> (getEffect());

    _constSpeed = pEffet->getConstSpeed();
    _randSpeed  = pEffet->getRandSpeed();
}
//-----------------------------------------------------------------------------
void PtyPartEffectTranslate::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectTranslate>(new Universe::PartEffectTranslate());
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectTranslate::clone() const
{
    return Ptr<Property>(new PtyPartEffectTranslate( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectTranslate::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectTranslate> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectTranslate>(pSrc);

    _constSpeed     = pPty->_constSpeed;
    _randSpeed      = pPty->_randSpeed;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectTranslate::PtyWidgetPartEffectTranslate(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectTranslate::~PtyWidgetPartEffectTranslate()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTranslate::readProperty()
{
    Ptr<PtyPartEffectTranslate> pP = LM_DEBUG_PTR_CAST<PtyPartEffectTranslate>(getDataProxy()->getProperty());

    _constSpeedX->setSingleValue(pP->_constSpeed.x);
    _constSpeedY->setSingleValue(pP->_constSpeed.y);
    _constSpeedZ->setSingleValue(pP->_constSpeed.z);

    _randSpeedX->setSingleValue(pP->_randSpeed.x);
    _randSpeedY->setSingleValue(pP->_randSpeed.y);
    _randSpeedZ->setSingleValue(pP->_randSpeed.z);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTranslate::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectTranslate> pP = LM_DEBUG_PTR_CAST<PtyPartEffectTranslate>(getDataProxy()->getProperty());
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    _constSpeedX->getSingleValue(x);
    _constSpeedY->getSingleValue(y);
    _constSpeedZ->getSingleValue(z);
    pP->_constSpeed = Core::Vector3f(float(x), float(y), float(z));

    _randSpeedX->getSingleValue(x);
    _randSpeedY->getSingleValue(y);
    _randSpeedZ->getSingleValue(z);
    pP->_randSpeed = Core::Vector3f(float(x), float(y), float(z));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTranslate::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Translate effect");

    _del            = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _constSpeedX    = new QtToolbox::SingleSlidingHDR(this, "Const X", true);
    _constSpeedY    = new QtToolbox::SingleSlidingHDR(this, "Const Y", true);
    _constSpeedZ    = new QtToolbox::SingleSlidingHDR(this, "Const Z", true);
    _randSpeedX     = new QtToolbox::SingleSlidingHDR(this, "Rand X", true);
    _randSpeedY     = new QtToolbox::SingleSlidingHDR(this, "Rand Y", true);
    _randSpeedZ     = new QtToolbox::SingleSlidingHDR(this, "Rand Z", true);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_constSpeedX);
    _groupBox->getLayout()->addWidget(_constSpeedY);
    _groupBox->getLayout()->addWidget(_constSpeedZ);
    _groupBox->getLayout()->addWidget(_randSpeedX);
    _groupBox->getLayout()->addWidget(_randSpeedY);
    _groupBox->getLayout()->addWidget(_randSpeedZ);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_constSpeedX);
    getWidgetsForUndoRedo().push_back(_constSpeedY);
    getWidgetsForUndoRedo().push_back(_constSpeedZ);
    getWidgetsForUndoRedo().push_back(_randSpeedX);
    getWidgetsForUndoRedo().push_back(_randSpeedY);
    getWidgetsForUndoRedo().push_back(_randSpeedZ);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectTranslate::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI