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
#include "PtyPartEffectFade.moc.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>

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
PtyPartEffectFade::PtyPartEffectFade( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                      const Ptr<Universe::PartEffectFade>& pEffect,
                                      const Core::String& title)
:   PtyPartEffect(pNodeE, pEffect, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade::PtyPartEffectFade(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(
        pNodeE,
        Ptr<Universe::PartEffectFade>(new Universe::PartEffectFade(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true)),
        title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade::~PtyPartEffectFade()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectFade::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectFade> pPW (new PtyWidgetPartEffectFade(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade::updateProperty()
{
    Ptr<Universe::PartEffectFade>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade> (getEffect());

    _color0         = pEffet->getSourceColor();
    _color1         = pEffet->getTargetColor();
    _time1          = pEffet->getDuration();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
    _onlyAlpha      = pEffet->onlyAlpha();
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade::updateData()
{
    Ptr<Universe::PartEffectFade>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade> (getEffect());

    pEffet->setSourceColor(_color0);
    pEffet->setTargetColor(_color1);
    pEffet->setDuration(_time1);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
    pEffet->onlyAlpha(_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectFade>(new Universe::PartEffectFade(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, true));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectFade::clone() const
{
    return Ptr<Property>(new PtyPartEffectFade( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);
    Ptr<PtyPartEffectFade> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectFade>(pSrc);

    _color0         = pPty->_color0;
    _color1         = pPty->_color1;
    _time1          = pPty->_time1;
    _killPartsAtEnd = pPty->_killPartsAtEnd;
    _onlyAlpha      = pPty->_onlyAlpha;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade::PtyWidgetPartEffectFade(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade::~PtyWidgetPartEffectFade()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade::readProperty()
{
    Ptr<PtyPartEffectFade> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade>(getDataProxy()->getProperty());

    _color0->setColorLinear(pP->_color0);
    _color1->setColorLinear(pP->_color1);
    _time1->setSingleValue(pP->_time1);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);
    _onlyAlpha->setChecked(pP->_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectFade> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade>(getDataProxy()->getProperty());
    _color0->getColorLinear(pP->_color0);
    _color1->getColorLinear(pP->_color1);

    double time1 = 0.0;
    _time1->getSingleValue(time1);
    pP->_time1 = time1;

    pP->_killPartsAtEnd = _killAtEnd->isChecked();
    pP->_onlyAlpha = _onlyAlpha->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Fade effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _color0     = new QtToolbox::QuickColorPicker(this, "Color0", Qt::white);
    _color1     = new QtToolbox::QuickColorPicker(this, "Color1", Qt::white);
    _time1      = new QtToolbox::SingleSlidingHDR(this, "Duration", false);
    _killAtEnd  = new QCheckBox("Kill at end", this);
    _onlyAlpha  = new QCheckBox("Only alpha", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_color0);
    _groupBox->getLayout()->addWidget(_color1);
    _groupBox->getLayout()->addWidget(_time1);
    _groupBox->getLayout()->addWidget(_killAtEnd);
    _groupBox->getLayout()->addWidget(_onlyAlpha);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_color0);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_time1);
    getWidgetsForUndoRedo().push_back(_killAtEnd);
    getWidgetsForUndoRedo().push_back(_onlyAlpha);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                          FADE2
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectFade2::PtyPartEffectFade2( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectFade2>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade2::PtyPartEffectFade2(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectFade2>(new Universe::PartEffectFade2(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, true, false)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade2::~PtyPartEffectFade2()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectFade2::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectFade2> pPW (new PtyWidgetPartEffectFade2(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade2::updateProperty()
{
    Ptr<Universe::PartEffectFade2>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade2> (getEffect());

    _color0         = pEffet->getColor0();
    _color1         = pEffet->getColor1();
    _color2         = pEffet->getColor2();
    _time1          = pEffet->getTime1();
    _time2          = pEffet->getTime2();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
    _loop           = pEffet->loop();
    _onlyAlpha      = pEffet->onlyAlpha();
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade2::updateData()
{
    Ptr<Universe::PartEffectFade2>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade2> (getEffect());

    pEffet->setColor0(_color0);
    pEffet->setColor1(_color1);
    pEffet->setColor2(_color2);
    pEffet->setTime1(_time1);
    pEffet->setTime2(_time2);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
    pEffet->loop(_loop);
    pEffet->onlyAlpha(_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade2::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectFade2>(new Universe::PartEffectFade2(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, true, false));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectFade2::clone() const
{
    Ptr<Property> pRet (new PtyPartEffectFade2( *this ));
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade2::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectFade2> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectFade2>(pSrc);

    _color0         = pPty->_color0;
    _color1         = pPty->_color1;
    _color2         = pPty->_color2;
    _time1          = pPty->_time1;
    _time2          = pPty->_time2;
    _killPartsAtEnd = pPty->_killPartsAtEnd;
    _loop           = pPty->_loop;
    _onlyAlpha      = pPty->_onlyAlpha;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade2::PtyWidgetPartEffectFade2(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade2::~PtyWidgetPartEffectFade2()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade2::readProperty()
{
    Ptr<PtyPartEffectFade2> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade2>(getDataProxy()->getProperty());

    _color0->setColorLinear(pP->_color0);
    _color1->setColorLinear(pP->_color1);
    _color2->setColorLinear(pP->_color2);
    _time1->setSingleValue(pP->_time1);
    _time2->setSingleValue(pP->_time2);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);
    _loop->setChecked(pP->_loop);
    _onlyAlpha->setChecked(pP->_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade2::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectFade2> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade2>(getDataProxy()->getProperty());
    _color0->getColorLinear(pP->_color0);
    _color1->getColorLinear(pP->_color1);
    _color2->getColorLinear(pP->_color2);

    double time1 = 0.0;
    _time1->getSingleValue(time1);
    pP->_time1 = time1;
    double time2 = 0.0;
    _time2->getSingleValue(time2);
    pP->_time2 = time2;

    pP->_killPartsAtEnd = _killAtEnd->isChecked();
    pP->_loop = _loop->isChecked();
    pP->_onlyAlpha = _onlyAlpha->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade2::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Fade2 effect");
    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _color0     = new QtToolbox::QuickColorPicker(this, "Color0", Qt::white);
    _color1     = new QtToolbox::QuickColorPicker(this, "Color1", Qt::white);
    _color2     = new QtToolbox::QuickColorPicker(this, "Color2", Qt::white);
    _time1      = new QtToolbox::SingleSlidingHDR(this, "Time1", false);
    _time2      = new QtToolbox::SingleSlidingHDR(this, "Time2", false);
    _killAtEnd  = new QCheckBox("Kill at end", this);
    _loop       = new QCheckBox("Loop", this);
    _onlyAlpha  = new QCheckBox("Only alpha", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_color0);
    _groupBox->getLayout()->addWidget(_color1);
    _groupBox->getLayout()->addWidget(_color2);
    _groupBox->getLayout()->addWidget(_time1);
    _groupBox->getLayout()->addWidget(_time2);
    _groupBox->getLayout()->addWidget(_killAtEnd);
    _groupBox->getLayout()->addWidget(_loop);
    _groupBox->getLayout()->addWidget(_onlyAlpha);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_color0);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_color2);
    getWidgetsForUndoRedo().push_back(_time1);
    getWidgetsForUndoRedo().push_back(_time2);
    getWidgetsForUndoRedo().push_back(_killAtEnd);
    getWidgetsForUndoRedo().push_back(_loop);
    getWidgetsForUndoRedo().push_back(_onlyAlpha);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade2::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                          FADE3
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectFade3::PtyPartEffectFade3( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                        const Ptr<Universe::PartEffectFade3>& pEffet,
                                        const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade3::PtyPartEffectFade3(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectFade3>(new Universe::PartEffectFade3(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, 1.0f, true, false)),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectFade3::~PtyPartEffectFade3()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectFade3::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectFade3> pPW (new PtyWidgetPartEffectFade3(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade3::updateProperty()
{
    Ptr<Universe::PartEffectFade3>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade3> (getEffect());

    _color0         = pEffet->getColor0();
    _color1         = pEffet->getColor1();
    _color2         = pEffet->getColor2();
    _color3         = pEffet->getColor3();
    _time1          = pEffet->getTime1();
    _time2          = pEffet->getTime2();
    _time3          = pEffet->getTime3();
    _killPartsAtEnd = pEffet->killPartsAtEnd();
    _loop           = pEffet->loop();
    _onlyAlpha      = pEffet->onlyAlpha();
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade3::updateData()
{
    Ptr<Universe::PartEffectFade3>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectFade3> (getEffect());

    pEffet->setColor0(_color0);
    pEffet->setColor1(_color1);
    pEffet->setColor2(_color2);
    pEffet->setColor3(_color3);
    pEffet->setTime1(_time1);
    pEffet->setTime2(_time2);
    pEffet->setTime3(_time3);
    pEffet->killPartsAtEnd(_killPartsAtEnd);
    pEffet->loop(_loop);
    pEffet->onlyAlpha(_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade3::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectFade3>(new Universe::PartEffectFade3(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1.0f, 1.0f, 1.0f, true, false));
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectFade3::clone() const
{
    return Ptr<Property>(new PtyPartEffectFade3( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectFade3::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectFade3> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectFade3>(pSrc);

    _color0         = pPty->_color0;
    _color1         = pPty->_color1;
    _color2         = pPty->_color2;
    _color3         = pPty->_color3;
    _time1          = pPty->_time1;
    _time2          = pPty->_time2;
    _time3          = pPty->_time3;
    _killPartsAtEnd = pPty->_killPartsAtEnd;
    _loop           = pPty->_loop;
    _onlyAlpha      = pPty->_onlyAlpha;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade3::PtyWidgetPartEffectFade3(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectFade3::~PtyWidgetPartEffectFade3()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade3::readProperty()
{
    Ptr<PtyPartEffectFade3> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade3>(getDataProxy()->getProperty());

    _color0->setColorLinear(pP->_color0);
    _color1->setColorLinear(pP->_color1);
    _color2->setColorLinear(pP->_color2);
    _color3->setColorLinear(pP->_color3);
    _time1->setSingleValue(pP->_time1);
    _time2->setSingleValue(pP->_time2);
    _time3->setSingleValue(pP->_time3);
    _killAtEnd->setChecked(pP->_killPartsAtEnd);
    _loop->setChecked(pP->_loop);
    _onlyAlpha->setChecked(pP->_onlyAlpha);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade3::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectFade3> pP = LM_DEBUG_PTR_CAST<PtyPartEffectFade3>(getDataProxy()->getProperty());
    _color0->getColorLinear(pP->_color0);
    _color1->getColorLinear(pP->_color1);
    _color2->getColorLinear(pP->_color2);
    _color3->getColorLinear(pP->_color3);

    double time1 = 0.0;
    _time1->getSingleValue(time1);
    pP->_time1 = time1;
    double time2 = 0.0;
    _time2->getSingleValue(time2);
    pP->_time2 = time2;
    double time3 = 0.0;
    _time3->getSingleValue(time3);
    pP->_time3 = time3;

    pP->_killPartsAtEnd = _killAtEnd->isChecked();
    pP->_loop = _loop->isChecked();
    pP->_onlyAlpha = _onlyAlpha->isChecked();
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade3::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Fade3 effect");

    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _color0     = new QtToolbox::QuickColorPicker(this, "Color0", Qt::white);
    _color1     = new QtToolbox::QuickColorPicker(this, "Color1", Qt::white);
    _color2     = new QtToolbox::QuickColorPicker(this, "Color2", Qt::white);
    _color3     = new QtToolbox::QuickColorPicker(this, "Color3", Qt::white);
    _time1      = new QtToolbox::SingleSlidingHDR(this, "Time1", false);
    _time2      = new QtToolbox::SingleSlidingHDR(this, "Time2", false);
    _time3      = new QtToolbox::SingleSlidingHDR(this, "Time3", false);
    _killAtEnd  = new QCheckBox("Kill at end", this);
    _loop       = new QCheckBox("Loop", this);
    _onlyAlpha  = new QCheckBox("Only alpha", this);

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_color0);
    _groupBox->getLayout()->addWidget(_color1);
    _groupBox->getLayout()->addWidget(_color2);
    _groupBox->getLayout()->addWidget(_color3);
    _groupBox->getLayout()->addWidget(_time1);
    _groupBox->getLayout()->addWidget(_time2);
    _groupBox->getLayout()->addWidget(_time3);
    _groupBox->getLayout()->addWidget(_killAtEnd);
    _groupBox->getLayout()->addWidget(_loop);
    _groupBox->getLayout()->addWidget(_onlyAlpha);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_color0);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_color2);
    getWidgetsForUndoRedo().push_back(_color3);
    getWidgetsForUndoRedo().push_back(_time1);
    getWidgetsForUndoRedo().push_back(_time2);
    getWidgetsForUndoRedo().push_back(_time3);
    getWidgetsForUndoRedo().push_back(_killAtEnd);
    getWidgetsForUndoRedo().push_back(_loop);
    getWidgetsForUndoRedo().push_back(_onlyAlpha);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectFade3::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI