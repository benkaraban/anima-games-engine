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
#include "PtyPartEffectResistance.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectResistance::PtyPartEffectResistance(   const Ptr<Universe::PartEffectResistance>& pResistance, 
                                                    const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _resistance(pResistance->getResistance()),
    _pResistance(pResistance)
{
}

*/
PtyPartEffectResistance::PtyPartEffectResistance( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                                  const Ptr<Universe::PartEffectResistance>& pEffect,
                                                  const Core::String& title)
:   PtyPartEffect(pNodeE, pEffect, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectResistance::PtyPartEffectResistance(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(
        pNodeE,
        Ptr<Universe::PartEffectResistance>(new Universe::PartEffectResistance(1.0f)),
        title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectResistance::~PtyPartEffectResistance()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectResistance::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectResistance> pPW (new PtyWidgetPartEffectResistance(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectResistance::updateProperty()
{
    Ptr<Universe::PartEffectResistance>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectResistance> (getEffect());

    _resistance = pEffet->getResistance();
}
//-----------------------------------------------------------------------------
void PtyPartEffectResistance::updateData()
{
    Ptr<Universe::PartEffectResistance>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectResistance> (getEffect());

    pEffet->setResistance(_resistance);
}
//-----------------------------------------------------------------------------
void PtyPartEffectResistance::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectResistance>(new Universe::PartEffectResistance(1.0f));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectResistance::clone() const
{
    return Ptr<Property>(new PtyPartEffectResistance( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectResistance::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectResistance> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectResistance>(pSrc);

    _resistance  = pPty->_resistance;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectResistance::PtyWidgetPartEffectResistance(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectResistance::~PtyWidgetPartEffectResistance()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectResistance::readProperty()
{
    Ptr<PtyPartEffectResistance> pP = LM_DEBUG_PTR_CAST<PtyPartEffectResistance>(getDataProxy()->getProperty());

    _resistance->setSingleValue(pP->_resistance);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectResistance::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectResistance> pP = LM_DEBUG_PTR_CAST<PtyPartEffectResistance>(getDataProxy()->getProperty());
    
    double resistance = 0.0;
    _resistance->getSingleValue(resistance);
    pP->_resistance = resistance;
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectResistance::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Resistance effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _del->setFixedHeight(16);
    _del->setFixedWidth(20);
    _resistance  = new QtToolbox::SingleSlidingHDR(this, "Resistance", false);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_resistance);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_resistance);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectResistance::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI