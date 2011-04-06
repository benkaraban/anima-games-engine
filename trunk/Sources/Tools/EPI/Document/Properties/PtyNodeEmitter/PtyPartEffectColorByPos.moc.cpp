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
#include "PtyPartEffectColorByPos.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <QGridLayout>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QtToolbox/ComboBox.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//                          ColorByPos
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyPartEffectColorByPos::PtyPartEffectColorByPos( const Ptr<Universe::NodeEmitter>& pNodeE, 
                                                  const Ptr<Universe::PartEffectColorByPos>& pEffet,
                                                  const Core::String& title)
:   PtyPartEffect(pNodeE, pEffet, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectColorByPos::PtyPartEffectColorByPos(const Ptr<Universe::NodeEmitter>& pNodeE, const Core::String& title)
:   PtyPartEffect(pNodeE,
                  Ptr<Universe::PartEffectColorByPos>(new Universe::PartEffectColorByPos()),
                  title)
{
     updateProperty();
}
//-----------------------------------------------------------------------------
PtyPartEffectColorByPos::~PtyPartEffectColorByPos()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyPartEffectColorByPos::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetPartEffectColorByPos> pPW (new PtyWidgetPartEffectColorByPos(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyPartEffectColorByPos::updateData()
{
    Ptr<Universe::PartEffectColorByPos>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectColorByPos> (getEffect());

    pEffet->setColorAxis(_axis);
    pEffet->setColor0(_color0);
    pEffet->setColor1(_color1);
    pEffet->setRangePos(_minPos, _maxPos);
    pEffet->setRandomFactor(_randFactor);
}
//-----------------------------------------------------------------------------
void PtyPartEffectColorByPos::updateProperty()
{
    Ptr<Universe::PartEffectColorByPos>  pEffet = LM_DEBUG_PTR_CAST<Universe::PartEffectColorByPos> (getEffect());

    _axis   = pEffet->getColorAxis();
    _color0 = pEffet->getColor0();
    _color1 = pEffet->getColor1();
    pEffet->getRangePos(_minPos, _maxPos);
    _randFactor  = pEffet->getRandomFactor();
}
//-----------------------------------------------------------------------------
void PtyPartEffectColorByPos::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getEffect()==null);

    Ptr<Universe::IPartEffect> pEffet = Ptr<Universe::PartEffectColorByPos>(new Universe::PartEffectColorByPos());
    setEffect(pEffet);
    getUniverseNodeEmitter()->addEffect(getEffect());

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyPartEffectColorByPos::clone() const
{
    return Ptr<Property>(new PtyPartEffectColorByPos( *this ));
}
//-----------------------------------------------------------------------------
void PtyPartEffectColorByPos::internalCopy(const Ptr<Property>& pSrc)
{
    PtyPartEffect::internalCopy(pSrc);

    Ptr<PtyPartEffectColorByPos> pPty = LM_DEBUG_PTR_CAST<PtyPartEffectColorByPos>(pSrc);

    _axis           = pPty->_axis;
    _color0         = pPty->_color0;
    _color1         = pPty->_color1;
    _minPos         = pPty->_minPos;
    _maxPos         = pPty->_maxPos;
    _randFactor     = pPty->_randFactor;
    
    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetPartEffectColorByPos::PtyWidgetPartEffectColorByPos(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetPartEffectColorByPos::~PtyWidgetPartEffectColorByPos()
{}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectColorByPos::readProperty()
{
    Ptr<PtyPartEffectColorByPos> pP = LM_DEBUG_PTR_CAST<PtyPartEffectColorByPos>(getDataProxy()->getProperty());

    _color0->setColorLinear(pP->_color0);
    _color1->setColorLinear(pP->_color1);
    _pos->setRangedValue(pP->_minPos, pP->_maxPos);
    _axis->selectIndex(int(pP->_axis));
    _randFactor->setSingleValue(pP->_randFactor);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectColorByPos::writeProperty(QWidget* pWidget)
{
    Ptr<PtyPartEffectColorByPos> pP = LM_DEBUG_PTR_CAST<PtyPartEffectColorByPos>(getDataProxy()->getProperty());
    
    _color0->getColorLinear(pP->_color0);
    _color1->getColorLinear(pP->_color1);

    double minPos, maxPos, randFactor;
    _pos->getRangedValue(minPos, maxPos);
    _randFactor->getSingleValue(randFactor);
    pP->_minPos = float(minPos);
    pP->_maxPos = float(maxPos);
    pP->_axis = Universe::EColorAxis(_axis->selectedIndex());
    pP->_randFactor = float(randFactor);
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectColorByPos::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "ColorByPos effect");
    _del        = new QPushButton(QIcon(":/icons/smallClearBW.png"), "", this);
    _color0     = new QtToolbox::QuickColorPicker(this, "Color0", Qt::white);
    _color1     = new QtToolbox::QuickColorPicker(this, "Color1", Qt::white);
    _axis       = new QtToolbox::ComboBox(this, "Axis");
    _pos        = new QtToolbox::RangedSlidingHDR(this, "Position range", true);
    _randFactor = new QtToolbox::SingleSlidingDouble(this, "Random factor", 0.0, 1.0);

    _axis->addItem("X");
    _axis->addItem("Y");
    _axis->addItem("Z");

    _groupBox->addWidgetToTitle(_del);
    _groupBox->getLayout()->addWidget(_color0);
    _groupBox->getLayout()->addWidget(_color1);
    _groupBox->getLayout()->addWidget(_pos);
    _groupBox->getLayout()->addWidget(_axis);
    _groupBox->getLayout()->addWidget(_randFactor);

    _layout->addWidget(_groupBox, 0, 0, 1, 1);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_color0);
    getWidgetsForUndoRedo().push_back(_color1);
    getWidgetsForUndoRedo().push_back(_pos);
    getWidgetsForUndoRedo().push_back(_axis);
    getWidgetsForUndoRedo().push_back(_randFactor);

    PropertyWidget::setupUi();
    connect(_del, SIGNAL(clicked()), this, SLOT(deleteWidget()));
}
//-----------------------------------------------------------------------------
void PtyWidgetPartEffectColorByPos::deleteWidget()
{
    emit deletePtyWidgetEffect(this);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

}//namespace EPI