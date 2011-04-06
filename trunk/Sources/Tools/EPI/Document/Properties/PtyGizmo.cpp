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
#include <QGridLayout>

#include "PtyGizmo.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/CollapsibleWidget.moc.h>

#include <EPI/Document/ContentInfo/GizmoRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeGizmo::PtyNodeGizmo(const Ptr<Universe::World>& pWorldForRepresentation,  const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNode, const Core::String& title):
    PtyNode(pWorld, pNode, title),
    _pWorldForRepresentation(pWorldForRepresentation)
{
    setVisibleWidgetShow(false);

    _pRepresentation = Ptr<IContentRepresentation>(new GizmoRepresentation(_pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeGizmo::PtyNodeGizmo(const PtyNodeGizmo& other):
    PtyNode(other)
{
    _pWorldForRepresentation = null;
    _pRepresentation = null;
}
//-----------------------------------------------------------------------------
PtyNodeGizmo::~PtyNodeGizmo()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeGizmo::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeGizmo> pPW (new PtyWidgetNodeGizmo(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeGizmo::updateData()
{
    PtyNode::updateData();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyNodeGizmo::updateProperty()
{
    PtyNode::updateProperty();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeGizmo::clone() const
{
    Ptr<PtyNodeGizmo> pRet  (new PtyNodeGizmo( *this ));
   
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNodeGizmo::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeGizmo> pPty = LM_DEBUG_PTR_CAST<PtyNodeGizmo>(pSrc);

    if(_pRepresentation!=null)
        _pRepresentation->updateObjectRepresentation(*this);

    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodeGizmo::isItMe(const Ptr<Universe::Node>& pNode)
{
    return _pRepresentation->isItMe(pNode);
}
//-----------------------------------------------------------------------------
void PtyNodeGizmo::setSelected(bool isSelected)
{
    if (isSelected == true)
    {
        _pRepresentation->setRepresentationMode(OBJECT_REPRESENTATION);
    }
    else
    {
        _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    }
}
//-----------------------------------------------------------------------------
void PtyNodeGizmo::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);
    _pWorldForRepresentation = pWorldInfoContent;
    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new GizmoRepresentation(_pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);

    updateProperty();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeGizmo::PtyWidgetNodeGizmo(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeGizmo::~PtyWidgetNodeGizmo()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeGizmo::setupUi()
{
        Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);
    
    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);    
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Gizmo", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);
        
    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _layout->addWidget(_groupBox);
    setLayout(_layout);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeGizmo::readProperty()
{
    Ptr<PtyNodeGizmo> pP = LM_DEBUG_PTR_CAST<PtyNodeGizmo>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeGizmo::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeGizmo> pP = LM_DEBUG_PTR_CAST<PtyNodeGizmo>(getDataProxy()->getProperty());  

    pP->_pRepresentation->updateObjectRepresentation(*pP);
}
//-----------------------------------------------------------------------------
} // namespace EPI