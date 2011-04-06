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

#include "PtyNodeCamera.h"

#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SingleSlidingValue.moc.h>
#include <QtToolbox/ComboBox.moc.h>

#include <EPI/Document/ContentInfo/CameraRepresentation.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeCamera::PtyNodeCamera(const Ptr<Universe::World>& pWorldForRepresentation,  const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeCamera>& pNCam, const Core::String& title):
    PtyNode(pWorld, pNCam, title),
    _pWorldForRepresentation(pWorldForRepresentation)
{
    setVisibleWidgetShow(false);
    _pRepresentation = Ptr<IContentRepresentation>(new CameraRepresentation(_pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyNodeCamera::PtyNodeCamera(const PtyNodeCamera& other):
    PtyNode(other)
{
    _pWorldForRepresentation = null;
    _pRepresentation = null;
}
//-----------------------------------------------------------------------------
PtyNodeCamera::~PtyNodeCamera()
{

}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyNodeCamera::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetNodeCamera> pPW (new PtyWidgetNodeCamera(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyNodeCamera::updateData()
{
    PtyNode::updateData();

    Ptr<Universe::NodeCamera>  pNodeCam = LM_DEBUG_PTR_CAST<Universe::NodeCamera>(getUniverseNode());

    pNodeCam->setNearPlane(_near);
    pNodeCam->setFarPlane(_far);
    pNodeCam->setFOV(_fov);
    pNodeCam->setAspectRatio(_ratio);

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
void PtyNodeCamera::updateProperty()
{
    PtyNode::updateProperty();

    Ptr<Universe::NodeCamera>  pNodeCam = LM_DEBUG_PTR_CAST<Universe::NodeCamera>(getUniverseNode());

    _near   = pNodeCam->getNearPlane();
    _far    = pNodeCam->getFarPlane();
    _fov    = pNodeCam->getFOV();
    _ratio  = pNodeCam->getAspectRatio();

    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyNodeCamera::clone() const
{
    Ptr<PtyNodeCamera> pRet  (new PtyNodeCamera( *this ));
   
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyNodeCamera::internalCopy(const Ptr<Property>& pSrc)
{
    PtyNode::internalCopy(pSrc);

    Ptr<PtyNodeCamera> pPty = LM_DEBUG_PTR_CAST<PtyNodeCamera>(pSrc);

    _near    = pPty->_near;
    _far     = pPty->_far;
    _fov     = pPty->_fov;
    _ratio   = pPty->_ratio;
    
    updateData();
}
//-----------------------------------------------------------------------------
bool PtyNodeCamera::isItMe(const Ptr<Universe::Node>& pNode)
{
    return _pRepresentation->isItMe(pNode);
}
//-----------------------------------------------------------------------------
bool PtyNodeCamera::isIntersecting  (const Core::Rayf& ray, float& distance)
{
    bool flag = PtyNode::isIntersecting(ray, distance);

    if (flag == false)
    {
        flag = _pRepresentation->isIntersecting(ray, distance);
    }

    return flag;
}
//-----------------------------------------------------------------------------
void PtyNodeCamera::setSelected(bool isSelected)
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
void PtyNodeCamera::internalResurrect(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldInfoContent, const Ptr<Property>& pty)
{
    PtyNode::internalResurrect(pWorld, pWorldInfoContent, pty);
    _pWorldForRepresentation = pWorldInfoContent;

    LM_ASSERT(_pRepresentation==null);

    _pRepresentation = Ptr<IContentRepresentation>(new CameraRepresentation(_pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);

    updateProperty();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetNodeCamera::PtyWidgetNodeCamera(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetNodeCamera::~PtyWidgetNodeCamera()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeCamera::setupUi()
{
    Ptr<PtyNode> pPNode = LM_DEBUG_PTR_CAST<PtyNode>(getDataProxy()->getProperty());
    Ptr<PropertyWidgetDataProxy> pDataPNodeW (new PropertyWidgetDataProxy(pPNode, pPNode->getName(), getDataProxy()->getStateRecorder()));
    _pPWNode = pPNode->PtyNode::internalCreatePropertyWidget(pDataPNodeW, this);
    
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::CollapsibleWidget(this, "Camera", false);
    _groupBox->setStyle(QtToolbox::CW_TITLE_1);
    _groupBox->setAlignmentTitle(Qt::AlignCenter);
    
    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);
        layoutGroupBox->setVerticalSpacing(1);
        layoutGroupBox->setHorizontalSpacing(5);
        layoutGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
     
     
    QtToolbox::CollapsibleWidget * pGroupCamPty = new QtToolbox::CollapsibleWidget(this, "Projection");
    _near  = new QtToolbox::SingleSlidingDouble(pGroupCamPty, "Near",  0., 2000);
    _far   = new QtToolbox::SingleSlidingDouble(pGroupCamPty, "Far",   1., 2000);
    _fov   = new QtToolbox::SingleSlidingDouble(pGroupCamPty, "Fov",   1., 170.);

    
    pGroupCamPty->getLayout()->addWidget(_fov);
    pGroupCamPty->getLayout()->addWidget(_far);
    pGroupCamPty->getLayout()->addWidget(_near);


    layoutGroupBox->addWidget(_pPWNode.get());
    layoutGroupBox->addWidget(pGroupCamPty);
    layoutGroupBox->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    _groupBox->setLayout(layoutGroupBox);

    _layout->addWidget(_groupBox);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    getWidgetsForUndoRedo().push_back(_fov);
    getWidgetsForUndoRedo().push_back(_far);
    getWidgetsForUndoRedo().push_back(_near);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeCamera::readProperty()
{
    Ptr<PtyNodeCamera> pP = LM_DEBUG_PTR_CAST<PtyNodeCamera>(getDataProxy()->getProperty());
    _pPWNode->updatePropertyWidget();

    _fov->setSingleValue(Core::rad2deg(pP->_fov));
    _far->setSingleValue(pP->_far);
    _near->setSingleValue(pP->_near);
}
//-----------------------------------------------------------------------------
void PtyWidgetNodeCamera::writeProperty(QWidget* pWidget)
{
    Ptr<PtyNodeCamera> pP = LM_DEBUG_PTR_CAST<PtyNodeCamera>(getDataProxy()->getProperty());  

    if (pWidget==NULL || pWidget==_fov)
    {
        double val; 
        _fov->getSingleValue(val);
        pP->_fov = Core::deg2rad(val);
    }

    if (pWidget==NULL || pWidget==_far)
    {
        _far->getSingleValue(pP->_far);
    }

    if (pWidget==NULL || pWidget==_near)
    {
        _near->getSingleValue(pP->_near);
    }

    pP->_pRepresentation->updateObjectRepresentation(*pP);
}
//-----------------------------------------------------------------------------
} // namespace EPI