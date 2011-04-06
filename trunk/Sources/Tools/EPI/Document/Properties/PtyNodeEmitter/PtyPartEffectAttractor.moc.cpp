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
#include "PtyPartEffectAttractor.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectAttractor::PtyPartEffectAttractor( const Ptr<Universe::PartEffectAttractor>& pAttractor, 
                                                const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _position(pAttractor->getPosition()),
    _intensity(pAttractor->getIntensity()),
    _pAttractor(pAttractor)
{
}*/
//-----------------------------------------------------------------------------
PtyPartEffectAttractor::PtyPartEffectAttractor(
    const Ptr<Universe::NodeEmitter>& pNodeE, 
    const Ptr<Universe::PartEffectAttractor>& pEffet,
    const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectAttractor::PtyPartEffectAttractor(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
    Ptr<Universe::PartEffectAttractor>(new Universe::PartEffectAttractor(Core::Vector3f(0.0f, 0.0f, 0.0f), 0.0f)),
    title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectAttractor::~PtyPartEffectAttractor()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectAttractor::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectAttractor> pPW (new PtyWidgetPartEffectAttractor(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectAttractor::updateProperty()
{
    Ptr<Universe::PartEffectAttractor>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectAttractor> (getEffect());

    _position  = pEffet->getPosition();
    _intensity = pEffet->getIntensity();
    _magnet = pEffet->getMagnet();
    _magnetRadius = pEffet->getMagnetRadius();
}
//-----------------------------------------------------------------------------
void PtyPartEffectAttractor::updateData()
{
    Ptr<Universe::PartEffectAttractor>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectAttractor> (getEffect());

    pEffet->setPosition(_position);
    pEffet->setIntensity(_intensity);
    pEffet->setMagnet(_magnet);
    pEffet->setMagnetRadius(_magnetRadius);
}
//-----------------------------------------------------------------------------
void PtyPartEffectAttractor::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectAttractor>(new Universe::PartEffectAttractor(Core::Vector3f(0.0f, 0.0f, 0.0f), 0.0f));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectAttractor::clone() const
{
    return Ptr<Property>(new PtyPartEffectAttractor( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectAttractor::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectAttractor> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectAttractor>(pSrc);

    _position     = pPty->_position;
    _intensity    = pPty->_intensity;
    _magnet       = pPty->_magnet;
    _magnetRadius = pPty->_magnetRadius;

    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectAttractor::PtyWidgetPartEffectAttractor(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectAttractor::~PtyWidgetPartEffectAttractor()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAttractor::readProperty()
{
    Ptr<PtyPartEffectAttractor> pP = LM_DEBUG_PTR_CAST<PtyPartEffectAttractor>(getDataProxy()->getProperty());

    _X->setSingleValue(pP->_position.x);
    _Y->setSingleValue(pP->_position.y);
    _Z->setSingleValue(pP->_position.z);
    _intensity->setSingleValue(pP->_intensity);
    _magnet->setChecked(pP->_magnet);
    _magnetRadius->setSingleValue(pP->_magnetRadius);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAttractor::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectAttractor> pP = LM_DEBUG_PTR_CAST<PtyPartEffectAttractor>(getDataProxy()->getProperty());
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    _X->getSingleValue(x);
    _Y->getSingleValue(y);
    _Z->getSingleValue(z);
    pP->_position.x = x;
    pP->_position.y = y;
    pP->_position.z = z;

    double intensity = 0.0;
    _intensity->getSingleValue(intensity);
    pP->_intensity = intensity;

    pP->_magnet = _magnet->isChecked();

    float magnetRadius = 0.0;
    _magnetRadius->getSingleValue(magnetRadius);
    pP->_magnetRadius = magnetRadius;
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAttractor::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Attractor effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _X          = new QtToolbox::SingleSlidingHDR(this, "X", true);
    _Y          = new QtToolbox::SingleSlidingHDR(this, "Y", true);
    _Z          = new QtToolbox::SingleSlidingHDR(this, "Z", true);
    _intensity  = new QtToolbox::SingleSlidingHDR(this, "Intensity", true);
    _magnet     = new QCheckBox("Magnet", this);
    _magnetRadius  = new QtToolbox::SingleSlidingHDR(this, "Magnet radius", true);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_X);
    _groupBox->getLayout()->addWidget(_Y);
    _groupBox->getLayout()->addWidget(_Z);
    _groupBox->getLayout()->addWidget(_intensity);
    _groupBox->getLayout()->addWidget(_magnet);
    _groupBox->getLayout()->addWidget(_magnetRadius);

    _layout->addWidget(_groupBox, 0, 0, 1, 1);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_X);
    getWidgetsForUndoRedo().push_back(_Y);
    getWidgetsForUndoRedo().push_back(_Z);
    getWidgetsForUndoRedo().push_back(_intensity);
    getWidgetsForUndoRedo().push_back(_magnet);
    getWidgetsForUndoRedo().push_back(_magnetRadius);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAttractor::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI