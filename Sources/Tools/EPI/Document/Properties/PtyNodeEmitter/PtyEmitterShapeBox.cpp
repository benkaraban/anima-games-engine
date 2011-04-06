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
#include "PtyEmitterShapeBox.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyEmitterShapeBox::PtyEmitterShapeBox(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::EmitterShapeBox>& pShape,
    const Core::String& title):
    PtyEmitterShape(pNodeE, pShape, title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeBox::PtyEmitterShapeBox(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Core::String& title):
    PtyEmitterShape(pNodeE, Ptr<Universe::EmitterShapeBox> (new Universe::EmitterShapeBox()), title)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeBox::~PtyEmitterShapeBox()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterShapeBox::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterShapeBox> pPW (new PtyWidgetEmitterShapeBox(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeBox::updateData()
{
    Ptr<Universe::EmitterShapeBox> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeBox>(getShape());

    pShape->setBox(Core::AABoxf(Core::Vector3f(_minX, _minY, _minZ), Core::Vector3f(_maxX, _maxY, _maxZ)));
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeBox::updateProperty()
{
    Ptr<Universe::EmitterShapeBox> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeBox>(getShape());

    const Core::AABoxf & box = pShape->getBox();

    _minX = box.min.x;
    _minY = box.min.y;
    _minZ = box.min.z;

    _maxX = box.max.x;
    _maxY = box.max.y;
    _maxZ = box.max.z;
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterShapeBox::clone() const
{
    return Ptr<Property>(new PtyEmitterShapeBox( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeBox::internalCopy(const Ptr<Property>& pSrc)
{
    PtyEmitterShape::internalCopy(pSrc);
    Ptr<PtyEmitterShapeBox> pPty = LM_DEBUG_PTR_CAST<PtyEmitterShapeBox>(pSrc);

    _minX = pPty->_minX;
    _minY = pPty->_minY;
    _minZ = pPty->_minZ;

    _maxX = pPty->_maxX;
    _maxY = pPty->_maxY;
    _maxZ = pPty->_maxZ;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeBox::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    LM_ASSERT(getShape()==null);

    Ptr<Universe::EmitterShapeBox> pShape = Ptr<Universe::EmitterShapeBox> (new Universe::EmitterShapeBox());
    
    setShape(pShape);
    getUniverseNodeEmitter()->setShape(getShape());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeBox::PtyWidgetEmitterShapeBox(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeBox::~PtyWidgetEmitterShapeBox()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeBox::readProperty()
{
    Ptr<PtyEmitterShapeBox> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeBox>(getDataProxy()->getProperty());

    _x->setRangedValue(pP->_minX, pP->_maxX);
    _y->setRangedValue(pP->_minY, pP->_maxY);
    _z->setRangedValue(pP->_minZ, pP->_maxZ);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeBox::writeProperty(QWidget* pWidget)
{
    Ptr<PtyEmitterShapeBox> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeBox>(getDataProxy()->getProperty());

    double minX, minY, minZ = 0.0;
    double maxX, maxY, maxZ = 0.0;
    _x->getRangedValue(minX, maxX);
    _y->getRangedValue(minY, maxY);
    _z->getRangedValue(minZ, maxZ);

    pP->_minX = minX;
    pP->_minY = minY;
    pP->_minZ = minZ;

    pP->_maxX = maxX;
    pP->_maxY = maxY;
    pP->_maxZ = maxZ;
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeBox::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _x      = new QtToolbox::RangedSlidingHDR(this, "X", true);
    _y      = new QtToolbox::RangedSlidingHDR(this, "Y", true);
    _z      = new QtToolbox::RangedSlidingHDR(this, "Z", true);

    _layout->addWidget(_x);
    _layout->addWidget(_y);
    _layout->addWidget(_z);

    setLayout(_layout);

    readProperty();

    getWidgetsForUndoRedo().push_back(_x);
    getWidgetsForUndoRedo().push_back(_y);
    getWidgetsForUndoRedo().push_back(_z);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI