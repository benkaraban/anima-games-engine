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
#include "PtyPartEffectRoll.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectRoll::PtyPartEffectRoll(   const Ptr<Universe::PartEffectRoll>& pRoll, 
                                        const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _roll(pRoll->getRoll()),
    _pRoll(pRoll)
{
}*/
PtyPartEffectRoll::PtyPartEffectRoll( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectRoll>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectRoll::PtyPartEffectRoll(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectRoll>(new Universe::PartEffectRoll(0.0f, 0.0f)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectRoll::~PtyPartEffectRoll()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectRoll::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectRoll> pPW (new PtyWidgetPartEffectRoll(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectRoll::updateProperty()
{
    Ptr<Universe::PartEffectRoll>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectRoll> (getEffect());

    _roll = pEffet->getRoll();
    _randRoll = pEffet->getRandomRoll();
}
//-----------------------------------------------------------------------------
void PtyPartEffectRoll::updateData()
{
    Ptr<Universe::PartEffectRoll>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectRoll> (getEffect());

    pEffet->setRoll(_roll);
    pEffet->setRandomRoll(_randRoll);
}
//-----------------------------------------------------------------------------
void PtyPartEffectRoll::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectRoll>(new Universe::PartEffectRoll(0.0f, 0.0f));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectRoll::clone() const
{
    return Ptr<Property>(new PtyPartEffectRoll( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectRoll::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectRoll> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectRoll>(pSrc);

    _roll  = pPty->_roll;
    _randRoll = pPty->_randRoll;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectRoll::PtyWidgetPartEffectRoll(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectRoll::~PtyWidgetPartEffectRoll()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectRoll::readProperty()
{
    Ptr<PtyPartEffectRoll> pP = LM_DEBUG_PTR_CAST<PtyPartEffectRoll>(getDataProxy()->getProperty());

    _roll->setSingleValue(pP->_roll);
    _randRoll->setSingleValue(pP->_randRoll);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectRoll::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectRoll> pP = LM_DEBUG_PTR_CAST<PtyPartEffectRoll>(getDataProxy()->getProperty());
    
    _roll->getSingleValue(pP->_roll);
    _randRoll->getSingleValue(pP->_randRoll);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectRoll::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Roll effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _roll       = new QtToolbox::SingleSlidingHDR(this, "Roll speed(rps)", true);
    _randRoll   = new QtToolbox::SingleSlidingHDR(this, "Random roll speed(rps)", true);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_roll);
    _groupBox->getLayout()->addWidget(_randRoll);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_roll);
    getWidgetsForUndoRedo().push_back(_randRoll);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectRoll::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI