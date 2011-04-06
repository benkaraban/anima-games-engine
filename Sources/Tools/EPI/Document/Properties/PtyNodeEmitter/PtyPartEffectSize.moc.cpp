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
#include "PtyPartEffectSize.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                          FADE
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectSize::PtyPartEffectSize(   const Ptr<Universe::PartEffectSize>& pSize, 
                                        const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _width0(pSize->getWidth0()),
    _height0(pSize->getHeight0()),
    _width1(pSize->getWidth1()),
    _height1(pSize->getHeight1()),
    _time1(pSize->getDuration()),
    _squareParts(pSize->getSquareParticles()),
    _killPartsAtEnd(pSize->killPartsAtEnd()),
    _pSize(pSize)
{
}*/
PtyPartEffectSize::PtyPartEffectSize( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                      const Ptr<Universe::PartEffectSize>& pEffet,
                                      const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSize::PtyPartEffectSize(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectSize>(new Universe::PartEffectSize(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSize::~PtyPartEffectSize()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectSize::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectSize> pPW (new PtyWidgetPartEffectSize(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize::updateProperty()
{
    Ptr<Universe::PartEffectSize>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSize> (getEffect());

    _width0         = pEffet->getWidth0();
    _height0        = pEffet->getHeight0();
    _width1         = pEffet->getWidth1();
    _height1        = pEffet->getHeight1();
    _time1          = pEffet->getDuration();
    _squareParts    = pEffet->getSquareParticles();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize::updateData()
{
    Ptr<Universe::PartEffectSize>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSize> (getEffect());

    pEffet->setSize0(_width0, _height0);
    pEffet->setSize1(_width1, _height1);
    pEffet->setDuration(_time1);
    pEffet->setSquareParticles(_squareParts);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectSize>(new Universe::PartEffectSize(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectSize::clone() const
{
    return Ptr<Property>(new PtyPartEffectSize( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectSize> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectSize>(pSrc);

    _width0         = pPty->_width0;
    _height0        = pPty->_height0;
    _width1         = pPty->_width1;
    _height1        = pPty->_height1;
    _time1          = pPty->_time1;
    _squareParts    = pPty->_squareParts;
    _killPartsAtEnd = pPty->_killPartsAtEnd;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSize::PtyWidgetPartEffectSize(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSize::~PtyWidgetPartEffectSize()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize::readProperty()
{
    Ptr<PtyPartEffectSize> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSize>(getDataProxy()->getProperty());

    _width0->setSingleValue(pP->_width0);
    _height0->setSingleValue(pP->_height0);
    _width1->setSingleValue(pP->_width1);
    _height1->setSingleValue(pP->_height1);
    _time1->setSingleValue(pP->_time1);
    _squareParts->setChecked(pP->_squareParts);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectSize> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSize>(getDataProxy()->getProperty());
    double width0 = 0;
    double height0 = 0;
    double width1 = 0;
    double height1 = 0;
    _width0->getSingleValue(width0);
    _height0->getSingleValue(height0);
    _width1->getSingleValue(width1);
    _height1->getSingleValue(height1);
    pP->_width0 = width0;
    pP->_height0 = height0;
    pP->_width1 = width1;
    pP->_height1 = height1;

    double time1 = 0;
    _time1->getSingleValue(time1);
    pP->_time1 = time1;

    pP->_squareParts = _squareParts->isChecked();
    pP->_killPartsAtEnd = _killAtEnd->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setSpacing(0);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Size effect");
    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _width0     = new QtToolbox::SingleSlidingHDR(this, "width0", false);
    _height0    = new QtToolbox::SingleSlidingHDR(this, "heigh0", false);
    _width1     = new QtToolbox::SingleSlidingHDR(this, "width1", false);
    _height1    = new QtToolbox::SingleSlidingHDR(this, "height1", false);
    _time1      = new QtToolbox::SingleSlidingHDR(this, "Duration", false);
    _squareParts= new QCheckBox("Square parts", this);
    _killAtEnd  = new QCheckBox("Kill at end", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_width0);
    _groupBox->getLayout()->addWidget(_height0);
    _groupBox->getLayout()->addWidget(_width1);
    _groupBox->getLayout()->addWidget(_height1);
    _groupBox->getLayout()->addWidget(_time1);
    _groupBox->getLayout()->addWidget(_squareParts);
    _groupBox->getLayout()->addWidget(_killAtEnd);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_width0);
    getWidgetsForUndoRedo().push_back(_height0);
    getWidgetsForUndoRedo().push_back(_width1);
    getWidgetsForUndoRedo().push_back(_height1);
    getWidgetsForUndoRedo().push_back(_time1);
    getWidgetsForUndoRedo().push_back(_squareParts);
    getWidgetsForUndoRedo().push_back(_killAtEnd);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                          SIZE2
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/*PtyPartEffectSize2::PtyPartEffectSize2( const Ptr<Universe::PartEffectSize2>& pSize2, 
                                        const Core::String& title)
:   PropertyNode(title, false, false, CONTENT),
    _width0(pSize2->getWidth0()),
    _height0(pSize2->getHeight0()),
    _width1(pSize2->getWidth1()),
    _height1(pSize2->getHeight1()),
    _width2(pSize2->getWidth2()),
    _height2(pSize2->getHeight2()),
    _time1(pSize2->getTime1()),
    _time2(pSize2->getTime2()),
    _squareParts(pSize2->getSquareParticles()),
    _killPartsAtEnd(pSize2->killPartsAtEnd()),
    _pSize2(pSize2)
{
}*/
//-----------------------------------------------------------------------------
PtyPartEffectSize2::PtyPartEffectSize2( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectSize2>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSize2::PtyPartEffectSize2(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectSize2>(new Universe::PartEffectSize2(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectSize2::~PtyPartEffectSize2()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectSize2::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectSize2> pPW (new PtyWidgetPartEffectSize2(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize2::updateProperty()
{
    Ptr<Universe::PartEffectSize2>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSize2> (getEffect());

    _width0         = pEffet->getWidth0();
    _height0        = pEffet->getHeight0();
    _width1         = pEffet->getWidth1();
    _height1        = pEffet->getHeight1();
    _width2         = pEffet->getWidth2();
    _height2        = pEffet->getHeight2();
    _time1          = pEffet->getTime1();
    _time2          = pEffet->getTime2();
    _squareParts    = pEffet->getSquareParticles();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize2::updateData()
{
    Ptr<Universe::PartEffectSize2>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectSize2> (getEffect());

    pEffet->setSize0(_width0, _height0);
    pEffet->setSize1(_width1, _height1);
    pEffet->setSize2(_width2, _height2);
    pEffet->setTime1(_time1);
    pEffet->setTime2(_time2);
    pEffet->setSquareParticles(_squareParts);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize2::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectSize2>(new Universe::PartEffectSize2(1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, true));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectSize2::clone() const
{
    return Ptr<Property>(new PtyPartEffectSize2( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectSize2::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectSize2> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectSize2>(pSrc);

    _width0         = pPty->_width0;
    _height0        = pPty->_height0;
    _width1         = pPty->_width1;
    _height1        = pPty->_height1;
    _width2         = pPty->_width2;
    _height2        = pPty->_height2;
    _time1          = pPty->_time1;
    _time2          = pPty->_time2;
    _squareParts    = pPty->_squareParts;
    _killPartsAtEnd = pPty->_killPartsAtEnd;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSize2::PtyWidgetPartEffectSize2(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectSize2::~PtyWidgetPartEffectSize2()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize2::readProperty()
{
    Ptr<PtyPartEffectSize2> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSize2>(getDataProxy()->getProperty());
    _width0->setSingleValue(pP->_width0);
    _height0->setSingleValue(pP->_height0);
    _width1->setSingleValue(pP->_width1);
    _height1->setSingleValue(pP->_height1);
    _width2->setSingleValue(pP->_width2);
    _height2->setSingleValue(pP->_height2);
    _time1->setSingleValue(pP->_time1);
    _time2->setSingleValue(pP->_time2);
    _squareParts->setChecked(pP->_squareParts);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize2::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectSize2> pP = LM_DEBUG_PTR_CAST<PtyPartEffectSize2>(getDataProxy()->getProperty());

    double width0 = 0;
    double height0 = 0;
    double width1 = 0;
    double height1 = 0;
    double width2 = 0;
    double height2 = 0;
    _width0->getSingleValue(width0);
    _height0->getSingleValue(height0);
    _width1->getSingleValue(width1);
    _height1->getSingleValue(height1);
    _width2->getSingleValue(width2);
    _height2->getSingleValue(height2);
    pP->_width0 = width0;
    pP->_height0 = height0;
    pP->_width1 = width1;
    pP->_height1 = height1;
    pP->_width2 = width2;
    pP->_height2 = height2;

    double time1 = 0;
    _time1->getSingleValue(time1);
    pP->_time1 = time1;
    double time2 = 0;
    _time2->getSingleValue(time2);
    pP->_time2 = time2;

    pP->_squareParts = _squareParts->isChecked();
    pP->_killPartsAtEnd = _killAtEnd->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize2::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Size2 effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _width0     = new QtToolbox::SingleSlidingHDR(this, "width0", false);
    _height0    = new QtToolbox::SingleSlidingHDR(this, "heigh0", false);
    _width1     = new QtToolbox::SingleSlidingHDR(this, "width1", false);
    _height1    = new QtToolbox::SingleSlidingHDR(this, "height1", false);
    _width2     = new QtToolbox::SingleSlidingHDR(this, "width2", false);
    _height2    = new QtToolbox::SingleSlidingHDR(this, "height2", false);
    _time1      = new QtToolbox::SingleSlidingHDR(this, "Time1", false);
    _time2      = new QtToolbox::SingleSlidingHDR(this, "Time2", false);
    _squareParts= new QCheckBox("Square parts", this);
    _killAtEnd  = new QCheckBox("Kill at end", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_width0);
    _groupBox->getLayout()->addWidget(_height0);
    _groupBox->getLayout()->addWidget(_width1);
    _groupBox->getLayout()->addWidget(_height1);
    _groupBox->getLayout()->addWidget(_width2);
    _groupBox->getLayout()->addWidget(_height2);
    _groupBox->getLayout()->addWidget(_time1);
    _groupBox->getLayout()->addWidget(_time2);
    _groupBox->getLayout()->addWidget(_squareParts);
    _groupBox->getLayout()->addWidget(_killAtEnd);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_width0);
    getWidgetsForUndoRedo().push_back(_height0);
    getWidgetsForUndoRedo().push_back(_width1);
    getWidgetsForUndoRedo().push_back(_height1);
    getWidgetsForUndoRedo().push_back(_width2);
    getWidgetsForUndoRedo().push_back(_height2);
    getWidgetsForUndoRedo().push_back(_time1);
    getWidgetsForUndoRedo().push_back(_time2);
    getWidgetsForUndoRedo().push_back(_squareParts);
    getWidgetsForUndoRedo().push_back(_killAtEnd);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectSize2::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI