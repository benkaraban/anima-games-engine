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
#include "PtyEmitterShapeSpline.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
Core::String ESplineShapeModeToString(Universe::ESplineShapeMode mode)
{
    switch(mode)
    {
    case Universe::SPLINE_DONT_MOVE:            return L"Don't move";
    case Universe::SPLINE_MOVE_BORDER_WRAP:     return L"Wrap";
    case Universe::SPLINE_MOVE_BORDER_MIRROR:   return L"Mirror";
    case Universe::SPLINE_MOVE_BORDER_KILL:     return L"Kill";
    }
    return L"Don't move";
}
//-----------------------------------------------------------------------------
Universe::ESplineShapeMode StringToEShapeMode(const String& str)
{
    if     (str == ESplineShapeModeToString(Universe::SPLINE_DONT_MOVE))            return Universe::SPLINE_DONT_MOVE;
    else if(str == ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_WRAP))     return Universe::SPLINE_MOVE_BORDER_WRAP;
    else if(str == ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_MIRROR))   return Universe::SPLINE_MOVE_BORDER_MIRROR;
    else if(str == ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_KILL))     return Universe::SPLINE_MOVE_BORDER_KILL;
    else                                                                            return Universe::SPLINE_DONT_MOVE;
}
//-----------------------------------------------------------------------------
PtyEmitterShapeSpline::PtyEmitterShapeSpline(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::RessourcePool> & pResPool,
    const Ptr<Universe::EmitterShapeSpline>& pShape,
    const Core::String& title):
    PtyEmitterShape(pNodeE, pShape, title),
    _pResPool(pResPool)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeSpline::PtyEmitterShapeSpline(
    const Ptr<Universe::NodeEmitter>& pNodeE,
    const Ptr<Universe::RessourcePool> & pResPool,
    const Core::String& title):
    PtyEmitterShape(pNodeE, Ptr<Universe::EmitterShapeSpline> (new Universe::EmitterShapeSpline(pResPool)), title),
    _pResPool(pResPool)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyEmitterShapeSpline::~PtyEmitterShapeSpline()
{}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyEmitterShapeSpline::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetEmitterShapeSpline> pPW(new PtyWidgetEmitterShapeSpline(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeSpline::updateProperty()
{
    Ptr<Universe::EmitterShapeSpline> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeSpline>(getShape());

    _splineName = pShape->getSplineName();
    _mode = pShape->getMode();
    pShape->getUStart(_uStartMin, _uStartMax);
    pShape->getUSpeed(_uSpeedMin, _uSpeedMax);
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeSpline::updateData()
{
    Ptr<Universe::EmitterShapeSpline> pShape = LM_DEBUG_PTR_CAST<Universe::EmitterShapeSpline>(getShape());

    pShape->setSpline(_splineName);
    pShape->setMode(_mode);
    pShape->setUStart(_uStartMin, _uStartMax);
    pShape->setUSpeed(_uSpeedMin, _uSpeedMax);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyEmitterShapeSpline::clone() const
{
    return Ptr<Property>(new PtyEmitterShapeSpline( *this ));
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeSpline::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyEmitterShapeSpline> pPty = LM_DEBUG_PTR_CAST<PtyEmitterShapeSpline>(pSrc);

    _splineName = pPty->_splineName;
    _mode = pPty->_mode;
    _uStartMin = pPty->_uStartMin;
    _uStartMax = pPty->_uStartMax;
    _uSpeedMin = pPty->_uSpeedMin;
    _uSpeedMax = pPty->_uSpeedMax;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterShapeSpline::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Property>& pty)
{
    LM_ASSERT(getShape()==null);

    Ptr<Universe::EmitterShapeSpline> pShape = Ptr<Universe::EmitterShapeSpline> (new Universe::EmitterShapeSpline(_pResPool));
    
    setShape(pShape);
    getUniverseNodeEmitter()->setShape(getShape());

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeSpline::PtyWidgetEmitterShapeSpline(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent)
:   PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetEmitterShapeSpline::~PtyWidgetEmitterShapeSpline()
{}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeSpline::readProperty()
{
    String SplineName;
    Ptr<PtyEmitterShapeSpline> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeSpline>(getDataProxy()->getProperty());
    _splineName->setText(pP->_splineName);
    _mode->selectItem(Core::String8(ESplineShapeModeToString(pP->_mode)).c_str());
    _uStart->setRangedValue(pP->_uStartMin, pP->_uStartMax);
    _uSpeed->setRangedValue(pP->_uSpeedMin, pP->_uSpeedMax);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeSpline::writeProperty(QWidget* pWidget)
{
    Ptr<PtyEmitterShapeSpline> pP = LM_DEBUG_PTR_CAST<PtyEmitterShapeSpline>(getDataProxy()->getProperty());

    _splineName->getText(pP->_splineName);
    pP->_mode = StringToEShapeMode(_mode->selectedText());
    _uStart->getRangedValue(pP->_uStartMin, pP->_uStartMax);
    _uSpeed->getRangedValue(pP->_uSpeedMin, pP->_uSpeedMax);
}
//-----------------------------------------------------------------------------
void PtyWidgetEmitterShapeSpline::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->setSpacing(0);

    _splineName    = new CustomLine(this, "Spline name");
    _splineName->pushAuthorizedDropMimeData("asset/spline");
    _splineName->setReadOnly(true);

    _mode       = new QtToolbox::ComboBox(this, "Border mode");
    _mode->addItem(ESplineShapeModeToString(Universe::SPLINE_DONT_MOVE));
    _mode->addItem(ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_WRAP));
    _mode->addItem(ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_MIRROR));
    _mode->addItem(ESplineShapeModeToString(Universe::SPLINE_MOVE_BORDER_KILL));

    _uStart     = new QtToolbox::RangedSlidingDouble(this, "U start", 0.0, 1.0);
    _uSpeed     = new QtToolbox::RangedSlidingHDR(this, "U speed");

    _layout->addWidget(_splineName);
    _layout->addWidget(_mode);
    _layout->addWidget(_uStart);
    _layout->addWidget(_uSpeed);

    setLayout(_layout);

    readProperty();

    getWidgetsForUndoRedo().push_back(_splineName);
    getWidgetsForUndoRedo().push_back(_mode);
    getWidgetsForUndoRedo().push_back(_uStart);
    getWidgetsForUndoRedo().push_back(_uSpeed);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace EPI