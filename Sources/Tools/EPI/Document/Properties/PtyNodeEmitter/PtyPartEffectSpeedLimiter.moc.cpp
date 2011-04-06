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
#include "PtyPartEffectSpeedLimiter.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectSpeedLimiter::PtyPartEffectSpeedLimiter(   const Ptr<Universe::PartEffectSpeedLimiter>& pSpeedLimit,
                                                        const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _speedLimit(pSpeedLimit->getSpeedLimit()),
    _pSpeedLimit(pSpeedLimit)
{
}
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectSpeedLimiter::PtyPartEffectSpeedLimiter(const Ptr<Universe::NodeEmitter>& pNodeE,
                                                     const Ptr<Universe::PartEffectSpeedLimiter>& pEffect,
                                                     const Core::String& title)
:   PtyPartEffect(pNodeE, pEffect, title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSpeedLimiter::PtyPartEffectSpeedLimiter(const Ptr<Universe::NodeEmitter>& pNodeE,
                                                     const Core::String& title)
:   PtyPartEffect(
        pNodeE,
        Ptr<Universe::PartEffectSpeedLimiter>(new Universe::PartEffectSpeedLimiter(1.0f)),
        title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSpeedLimiter::~PtyPartEffectSpeedLimiter()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectSpeedLimiter::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectSpeedLimiter> pPW (new PtyWidgetPartEffectSpeedLimiter(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectSpeedLimiter::updateProperty()
{
    Ptr<Universe::PartEffectSpeedLimiter>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSpeedLimiter> (getEffect());

    _speedLimit  = pEffet->getSpeedLimit();
}
//-----------------------------------------------------------------------------
void PtyPartEffectSpeedLimiter::updateData()
{
    Ptr<Universe::PartEffectSpeedLimiter>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSpeedLimiter> (getEffect());

    pEffet->setSpeedLimit(_speedLimit);
}
//-----------------------------------------------------------------------------
void PtyPartEffectSpeedLimiter::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectSpeedLimiter>(new Universe::PartEffectSpeedLimiter(1.0f));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectSpeedLimiter::clone() const
{
    return Ptr<Property>(new PtyPartEffectSpeedLimiter( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectSpeedLimiter::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectSpeedLimiter> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectSpeedLimiter>(pSrc);

    _speedLimit  = pPty->_speedLimit;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSpeedLimiter::PtyWidgetPartEffectSpeedLimiter(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSpeedLimiter::~PtyWidgetPartEffectSpeedLimiter()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSpeedLimiter::readProperty()
{
    Ptr<PtyPartEffectSpeedLimiter> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSpeedLimiter>(getDataProxy()->getProperty());

    _speedLimit->setSingleValue(pP->_speedLimit);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSpeedLimiter::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectSpeedLimiter> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSpeedLimiter>(getDataProxy()->getProperty());
    
    double speedLimit = 0.0;
    _speedLimit->getSingleValue(speedLimit);
    pP->_speedLimit = speedLimit;
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSpeedLimiter::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Speed limiter effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _speedLimit  = new QtToolbox::SingleSlidingHDR(this, "Speed limit", false);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_speedLimit);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_speedLimit);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSpeedLimiter::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI