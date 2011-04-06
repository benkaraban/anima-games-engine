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
#include "PtyFog.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingValue.moc.h>
#include <QtToolbox/RangedSlidingHDR.moc.h>
#include <QtToolbox/ColorPicker/QuickColorPicker.moc.h>

#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyFog::PtyFog(const Ptr<Universe::World>& pWorld, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pWorld(pWorld),
    _color(pWorld->getFogColor()),
    _start(0),
    _end(0), 
   _opacity(pWorld->getFogSkyOpacity())
{
    float start = 0.0;
    float end   = 0.0;
    pWorld->getFogRange(start, end);
    _start = start;
    _end = end;
}
//-----------------------------------------------------------------------------
PtyFog::~PtyFog()
{
    int a = 5;
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyFog::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetFog> pPW (new PtyWidgetFog(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyFog::updateData()
{
    _pWorld->setFogColor(_color);
    _pWorld->setFogRange(_start, _end);
    _pWorld->setFogSkyOpacity(_opacity);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyFog::clone() const
{
    return Ptr<Property>(new PtyFog( *this ));
}
//-----------------------------------------------------------------------------
void PtyFog::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyFog> pPty = LM_DEBUG_PTR_CAST<PtyFog>(pSrc);

    _color   =  pPty->_color;
    _start   = pPty->_start;
    _end     = pPty->_end;
    _opacity = pPty->_opacity;

    _pWorld = pPty->_pWorld;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetFog::PtyWidgetFog(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetFog::~PtyWidgetFog()
{
     int a = 5;
}
//-----------------------------------------------------------------------------
void PtyWidgetFog::setupUi()
{
    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);    
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Fog", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);    
        _groupBox->setLayout(layoutGroupBox);

    _color      = new QtToolbox::QuickColorPicker(this, "Color");
    _range      = new QtToolbox::RangedSlidingHDR(this, "Range", false);
    _opacity    = new QtToolbox::SingleSlidingDouble(this, "Opacity", 0.0, 1.0);


    layoutGroupBox->addWidget(_color, 0, 0, Qt::AlignCenter);
    layoutGroupBox->addWidget(_range, 1, 0, Qt::AlignLeft);
    layoutGroupBox->addWidget(_opacity, 2, 0, Qt::AlignLeft);

    _layout->addWidget(_groupBox);


    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_color);
    getWidgetsForUndoRedo().push_back(_range);
    getWidgetsForUndoRedo().push_back(_opacity);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetFog::readProperty()
{
    Ptr<PtyFog> pP = LM_DEBUG_PTR_CAST<PtyFog>(getDataProxy()->getProperty());

    _color->setColorLinear(pP->_color.r, pP->_color.g, pP->_color.b, pP->_color.a);
    _range->setRangedValue(pP->_start, pP->_end);
    _opacity->setSingleValue(pP->_opacity);
}
//-----------------------------------------------------------------------------
void PtyWidgetFog::writeProperty(QWidget* pWidget)
{
    Ptr<PtyFog> pP = LM_DEBUG_PTR_CAST<PtyFog>(getDataProxy()->getProperty());

    _color->getColorLinear(pP->_color.r, pP->_color.g, pP->_color.b, pP->_color.a);
    _range->getRangedValue(pP->_start, pP->_end);
    _opacity->getSingleValue(pP->_opacity);

    
}
//-----------------------------------------------------------------------------

} // namespace EPI