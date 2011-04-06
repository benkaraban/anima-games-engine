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
#include "PtyPartEffectAnimate.moc.h"

#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectAnimate::PtyPartEffectAnimate( const Ptr<Universe::PartEffectAnimate>& pAnimate, 
                                            const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _key0(pAnimate->getKey0()),
    _key1(pAnimate->getKey1()),
    _duration(pAnimate->getDuration()),
    _loop(pAnimate->getLoop()),
    _killPartsAtEnd(pAnimate->killPartsAtEnd()),
    _pAnimate(pAnimate)
{
}*/
PtyPartEffectAnimate::PtyPartEffectAnimate( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                            const Ptr<Universe::PartEffectAnimate>& pEffet,
                                            const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectAnimate::PtyPartEffectAnimate(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectAnimate>(new Universe::PartEffectAnimate(0.0f, 1.0f, 3.0f, true, true)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectAnimate::~PtyPartEffectAnimate()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectAnimate::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectAnimate> pPW (new PtyWidgetPartEffectAnimate(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectAnimate::updateProperty()
{
    Ptr<Universe::PartEffectAnimate>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectAnimate> (getEffect());

    _key0           = pEffet->getKey0();
    _key1           = pEffet->getKey1();
    _duration       = pEffet->getDuration();
    _loop           = pEffet->getLoop();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
}
//-----------------------------------------------------------------------------
void PtyPartEffectAnimate::updateData()
{
    Ptr<Universe::PartEffectAnimate>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectAnimate> (getEffect());

    pEffet->setKey0(_key0);
    pEffet->setKey1(_key1);
    pEffet->setDuration(_duration);
    pEffet->setLoop(_loop);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
}
//-----------------------------------------------------------------------------
void PtyPartEffectAnimate::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::PartEffectAnimate> pEffet = Ptr<Universe::PartEffectAnimate>(new Universe::PartEffectAnimate(0.0f, 1.0f, 3.0f, true, true));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectAnimate::clone() const
{
    return Ptr<Property>(new PtyPartEffectAnimate( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectAnimate::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectAnimate> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectAnimate>(pSrc);

    _key0           = pPty->_key0;
    _key1           = pPty->_key1;
    _duration       = pPty->_duration;
    _loop           = pPty->_loop;
    _killPartsAtEnd = pPty->_killPartsAtEnd;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectAnimate::PtyWidgetPartEffectAnimate(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectAnimate::~PtyWidgetPartEffectAnimate()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAnimate::readProperty()
{
    Ptr<PtyPartEffectAnimate> pP = LM_DEBUG_PTR_CAST<PtyPartEffectAnimate>(getDataProxy()->getProperty());

    _keys->setRangedValue(pP->_key0, pP->_key1);
    _duration->setSingleValue(pP->_duration);
    _loop->setChecked(pP->_loop);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);

}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAnimate::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectAnimate> pP = LM_DEBUG_PTR_CAST<PtyPartEffectAnimate>(getDataProxy()->getProperty());
    double key0 = 0.0;
    double key1 = 0.0;
    _keys->getRangedValue(key0, key1);
    pP->_key0 = key0;
    pP->_key1 = key1;
    
    double duration = 0.0;
    _duration->getSingleValue(duration);
    pP->_duration = duration;
    pP->_loop = _loop->isChecked();
    pP->_killPartsAtEnd = _killAtEnd->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAnimate::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Animate effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _keys       = new QtToolbox::RangedSlidingDouble(this, "Keys", 0.0, 1.0);
    _duration   = new QtToolbox::SingleSlidingHDR(this, "Duration", true);
    _loop       = new QCheckBox("Loop", this);
    _killAtEnd  = new QCheckBox("Kill at end", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_keys);
    _groupBox->getLayout()->addWidget(_duration);
    _groupBox->getLayout()->addWidget(_loop);
    _groupBox->getLayout()->addWidget(_killAtEnd);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_keys);
    getWidgetsForUndoRedo().push_back(_duration);
    getWidgetsForUndoRedo().push_back(_loop);
    getWidgetsForUndoRedo().push_back(_killAtEnd);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectAnimate::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI