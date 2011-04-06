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
#include "PtyNodePlayerPosition.moc.h"


#include <QPushButton>

#include "PtyNodePlayerPosition.moc.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>

#include <EPI/Document/ContentInfo/PlayerPositionRepresentation.h>

#include <EPI/GUI/Widget/CustomLine.moc.h>
#include <EPI/Constants.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodePlayerPosition::PtyNodePlayerPosition(const Ptr<Universe::World>& pWorldForRepresentation,  const Ptr<Universe::World>& pWorld,  const Ptr<Universe::Node>& pNode, const Core::String& title):
    _pWorldForRepresentation(pWorldForRepresentation),
    PtyNode(pWorld, pNode, title),
    _playersDist(PLAYER_DISTANCE)
{
    _pRepresentation = Ptr<IContentRepresentation>(new PlayerPositionRepresentation(pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    setVisibleWidgetTag(true);
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodePlayerPosition::PtyNodePlayerPosition(const PtyNodePlayerPosition& other):
    PtyNode(other)
{
  //  _pRepresentation = other._pRepresentation->clone();
    _pRepresentation = null;
    _playersDist = other._playersDist;
    _modelName = other._modelName;
    _pWorldForRepresentation = null;
}
//-----------------------------------------------------------------------------
PtyNodePlayerPosition::~PtyNodePlayerPosition()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodePlayerPosition::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodePlayerPosition> pPW (new PtyWidgetNodePlayerPosition(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodePlayerPosition::updateData()
{
    PtyNode::updateData();
}
//-----------------------------------------------------------------------------
void PtyNodePlayerPosition::updateProperty()
{
    PtyNode::updateProperty();



    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodePlayerPosition::clone() const
{
    Ptr<PtyNodePlayerPosition> pRet  (new PtyNodePlayerPosition( *this ));
   
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNodePlayerPosition::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodePlayerPosition> pPty = LM_DEBUG_PTR_CAST<PtyNodePlayerPosition>(pSrc);

    _playersDist = pPty->_playersDist;
    _modelName = pPty->_modelName;
    _pWorldForRepresentation = pPty->_pWorldForRepresentation;

    _pRepresentation->updateObjectRepresentation(*this);

    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodePlayerPosition::isItMe(const Ptr<Universe::Node>& pNode)
{
    return _pRepresentation->isItMe(pNode);
}
//-----------------------------------------------------------------------------
bool PtyNodePlayerPosition::isIntersecting (const Core::Rayf& ray, float& distance)
{
    bool flag = PtyNode::isIntersecting(ray, distance);

    if (flag == false)
    {
        flag = _pRepresentation->isIntersecting(ray, distance);
    }

    return flag;
}
//-----------------------------------------------------------------------------
void PtyNodePlayerPosition::setSelected(bool isSelected)
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
void PtyNodePlayerPosition::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(_pWorldForRepresentation, pWorldInfoContent, pty);
    _pWorldForRepresentation = pWorldInfoContent;

    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new PlayerPositionRepresentation(_pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    _pRepresentation->updateObjectRepresentation(*this);
/*
    _pRepresentation->setNodeToRepresent(getUniverseNode());
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    updateProperty();*/
    //_pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Core::AABoxf PtyNodePlayerPosition::getBoundingBox()
{
    return _pRepresentation->getBoundingBox();
}
//-----------------------------------------------------------------------------
void PtyNodePlayerPosition::setVisible(bool flag)
{
    PtyNode::setVisible(flag);
    _pRepresentation->setVisible(flag);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodePlayerPosition::PtyWidgetNodePlayerPosition(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodePlayerPosition::~PtyWidgetNodePlayerPosition()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePlayerPosition::setupUi()
{
        Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
        Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);

    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    _groupBox = new QtToolbox::CollapsibleWidget(this, "Player Position", false);
        _groupBox->setStyle(QtToolbox::CW_TITLE_1);
        _groupBox->setAlignmentTitle(Qt::AlignCenter);

    
    _modelName    = new CustomLine(this, "Model name");
        _modelName->pushAuthorizedDropMimeData("asset/model");
        _modelName->setReadOnly(true);

    _groupBoxRepresentation = new QtToolbox::CollapsibleWidget(this, "Representation");
    _groupBoxRepresentation->getLayout()->addWidget(_modelName);

    _groupBox->getLayout()->addWidget(_pPWNode.get());
    _groupBox->getLayout()->addWidget(_groupBoxRepresentation);

    _layout->addWidget(_groupBox);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_modelName);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePlayerPosition::readProperty()
{
    Ptr<PtyNodePlayerPosition> pP = LM_DEBUG_PTR_CAST<PtyNodePlayerPosition>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _modelName->setText(pP->_modelName);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodePlayerPosition::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodePlayerPosition> pP = LM_DEBUG_PTR_CAST<PtyNodePlayerPosition>(getDataProxy()->getProperty());  

    if (pWidget==NULL || pWidget==_modelName)
    {
        String modelName;
        _modelName->getText(modelName);
        pP->_modelName = modelName;
    }

    pP->_pRepresentation->updateObjectRepresentation(*pP);
}
//-----------------------------------------------------------------------------
} // namespace EPI