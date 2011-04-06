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
#include "PtyDocSpline.h"

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

#include <EPI/Document/Properties/PtyTrajectory/PtyTrajectory.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocSpline::PtyDocSpline(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<StateRecorder>& pStateRecorder, const Core::String& title):
    PropertyNode(title, true, false, CONTENT)
{
    Ptr<PtyTrajectory>    pPtySpline (new PtyTrajectory(guiDoc, pWorldForRepresentation, pWorld, pStateRecorder, pWorld->createNode(), L"Spline"));
    addChild(pPtySpline);


    updateProperty();
}
//-----------------------------------------------------------------------------
PtyDocSpline::~PtyDocSpline()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyDocSpline::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    Ptr<PtyWidgetDocSpline> pPW (new PtyWidgetDocSpline(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyDocSpline::updateData()
{

}
//-----------------------------------------------------------------------------
void PtyDocSpline::updateProperty()
{
     
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyDocSpline::clone() const
{
    return Ptr<Property>(new PtyDocSpline( *this ));
}
//-----------------------------------------------------------------------------
void PtyDocSpline::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyDocSpline> pPty = LM_DEBUG_PTR_CAST<PtyDocSpline>(pSrc);


    updateData();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocSpline::getPtySpline()
{
    return getChild(0);
}
//-----------------------------------------------------------------------------
const Core::String& PtyDocSpline::getWorldDecoPath()
{
    return _worldDeco;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetDocSpline::PtyWidgetDocSpline(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetDocSpline::~PtyWidgetDocSpline()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetDocSpline::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "Spline", true);

    /*
    Ptr<PtyDocSpline> pP = LM_DEBUG_PTR_CAST<PtyDocSpline>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPWSpline (new PropertyWidgetDataProxy(pP->getPtySpline(), pP->getName(), getDataProxy()->getStateRecorder()));
    _pPtyWidgetSpline = pP->getPtySpline()->createPropertyWidget(pDataPWSpline, this);
    */


    _worldDeco = new QtToolbox::LineEdit(this, "Drop world here");
    _worldDeco->pushAuthorizedDropMimeData("asset/world");
    _worldDeco->setReadOnly(true);


    _groupBox->addWidget(_worldDeco);

    _layout->addWidget(_groupBox, 0, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    //_layout->addWidget(_pPtyWidgetSpline.get(), 1, 0, 1, 1, Qt::AlignTop | Qt::AlignLeft);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_worldDeco);  

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocSpline::readProperty()
{
    Ptr<PtyDocSpline> pPtyDocSpline = LM_DEBUG_PTR_CAST<PtyDocSpline>(getDataProxy()->getProperty());

    _worldDeco->setText(Core::String8(pPtyDocSpline->_worldDeco).c_str());
    //_pPtyWidgetMat->updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetDocSpline::writeProperty(QWidget* pWidget)
{
    Ptr<PtyDocSpline> pPtyDocSpline = LM_DEBUG_PTR_CAST<PtyDocSpline>(getDataProxy()->getProperty());

    if (pWidget==NULL || pWidget==_worldDeco)
        pPtyDocSpline->_worldDeco = Core::String(_worldDeco->text().toStdString().c_str());
}
//-----------------------------------------------------------------------------
} // namespace EPI