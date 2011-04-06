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
#include "PtyWorld.h"

#include <QGridLayout>

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>

#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWorld::PtyWorld(const Ptr<Universe::World>& pWorld, const Core::String& title):
    PropertyNode(title, true, false, CONTENT),
    _pWorld(pWorld)
{
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyWorld::~PtyWorld()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyWorld::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetWorld> pPW (new PtyWidgetWorld(pDataProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyWorld::updateData()
{
    _pWorld->setWaterLevel(_waterLevel);
}
//-----------------------------------------------------------------------------
void PtyWorld::updateProperty()
{
    _waterLevel = _pWorld->getWaterLevel();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyWorld::clone() const
{
    return Ptr<Property>(new PtyWorld( *this ));
}
//-----------------------------------------------------------------------------
void PtyWorld::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyWorld> pPtyWorld = LM_DEBUG_PTR_CAST<PtyWorld>(pSrc);

    _waterLevel = pPtyWorld->_waterLevel;
    _pWorld     = pPtyWorld->_pWorld;

    updateData();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetWorld::PtyWidgetWorld(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetWorld::~PtyWidgetWorld()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetWorld::setupUi()
{
    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Misc", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    _waterLevel = new QtToolbox::SingleSlidingDouble(_groupBox, "WaterLevel", -200.0, 800.0);

    _groupBox->getLayout()->addWidget(_waterLevel);

    _layout->addWidget(_groupBox);

    getWidgetsForUndoRedo().push_back(_waterLevel);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetWorld::readProperty()
{
    Ptr<PtyWorld> pPtyWorld = LM_DEBUG_PTR_CAST<PtyWorld>(getDataProxy()->getProperty());

    _waterLevel->setSingleValue(pPtyWorld->_waterLevel);
}
//-----------------------------------------------------------------------------
void PtyWidgetWorld::writeProperty(QWidget* pWidget)
{
    Ptr<PtyWorld> pPtyWorld = LM_DEBUG_PTR_CAST<PtyWorld>(getDataProxy()->getProperty());

    _waterLevel->getSingleValue(pPtyWorld->_waterLevel);
}
//-----------------------------------------------------------------------------
} // namespace EPI