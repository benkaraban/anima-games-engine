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
#include "PtyPartEffectForce.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectForce::PtyPartEffectForce( const Ptr<Universe::PartEffectForce>& pForce, 
                                        const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _direction(pForce->getDirection()),
    _acceleration(pForce->getAcceleration()),
    _pForce(pForce)
{
}*/
PtyPartEffectForce::PtyPartEffectForce( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectForce>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectForce::PtyPartEffectForce(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectForce>(new Universe::PartEffectForce(Core::Vector3f(0.0f, 1.0f, 0.0f), 1.0f)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectForce::~PtyPartEffectForce()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectForce::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectForce> pPW (new PtyWidgetPartEffectForce(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectForce::updateProperty()
{
    Ptr<Universe::PartEffectForce>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectForce> (getEffect());

    _direction    = pEffet->getDirection();
    _acceleration = pEffet->getAcceleration();
}
//-----------------------------------------------------------------------------
void PtyPartEffectForce::updateData()
{
    Ptr<Universe::PartEffectForce>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectForce> (getEffect());

    pEffet->setDirection(_direction);
    pEffet->setAcceleration(_acceleration);
}
//-----------------------------------------------------------------------------
void PtyPartEffectForce::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectForce>(new Universe::PartEffectForce(Core::Vector3f(0.0f, 1.0f, 0.0f), 1.0f));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectForce::clone() const
{
    return Ptr<Property>(new PtyPartEffectForce( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectForce::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectForce> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectForce>(pSrc);

    _direction      = pPty->_direction;
    _acceleration   = pPty->_acceleration;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectForce::PtyWidgetPartEffectForce(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectForce::~PtyWidgetPartEffectForce()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectForce::readProperty()
{
    Ptr<PtyPartEffectForce> pP = LM_DEBUG_PTR_CAST<PtyPartEffectForce>(getDataProxy()->getProperty());

    _X->setSingleValue(pP->_direction.x);
    _Y->setSingleValue(pP->_direction.y);
    _Z->setSingleValue(pP->_direction.z);
    _acceleration->setSingleValue(pP->_acceleration);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectForce::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectForce> pP = LM_DEBUG_PTR_CAST<PtyPartEffectForce>(getDataProxy()->getProperty());
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    _X->getSingleValue(x);
    _Y->getSingleValue(y);
    _Z->getSingleValue(z);
    pP->_direction.x = x;
    pP->_direction.y = y;
    pP->_direction.z = z;

    double acceleration = 0.0;
    _acceleration->getSingleValue(acceleration);
    pP->_acceleration = acceleration;
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectForce::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox       = new QtToolbox::CollapsibleWidget(this, "Force effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _del->setFixedHeight(16);
    _del->setFixedWidth(20);
    _X              = new QtToolbox::SingleSlidingDouble(this, "X", -1.0, 1.0);
    _Y              = new QtToolbox::SingleSlidingDouble(this, "Y", -1.0, 1.0);
    _Z              = new QtToolbox::SingleSlidingDouble(this, "Z", -1.0, 1.0);
    _acceleration   = new QtToolbox::SingleSlidingHDR(this, "Acceleration", true);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_X);
    _groupBox->getLayout()->addWidget(_Y);
    _groupBox->getLayout()->addWidget(_Z);
    _groupBox->getLayout()->addWidget(_acceleration);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_X);
    getWidgetsForUndoRedo().push_back(_Y);
    getWidgetsForUndoRedo().push_back(_Z);
    getWidgetsForUndoRedo().push_back(_acceleration);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectForce::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI