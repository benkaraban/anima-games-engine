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
#include "PtyTransform.moc.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QtToolbox/CollapsibleWidget.moc.h>
#include <QtToolbox/SpinBox.moc.h>
#include <QtToolbox/SpinBoxVector.moc.h>

#include <QtToolbox/NumberEdit.moc.h>
#include <QtToolbox/NumberEditVector3.moc.h>

#include <Universe/Node.h>

#include <Core/Logger.h>
#include <Universe/Tools.h>

#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/StateRecorder.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTransform::PtyTransform(const Core::String& title, ETransformType type):
    PropertyNode(title, false, false, CONTENT),
    _type(type)
{
    addFlag(FLAG_PTY_TRANSFORM);
}
//-----------------------------------------------------------------------------
    PtyTransform::PtyTransform(const PtyTransform& other) :
    PropertyNode(other),
    _type(other._type)
{
}
//-----------------------------------------------------------------------------
PtyTransform::~PtyTransform()
{
}
//-----------------------------------------------------------------------------
const ETransformType& PtyTransform::getType() const
{
    return _type;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTransformNode::PtyTransformNode(const Ptr<Universe::Node> pNode, const Core::String& title, ETransformType type):
    PtyTransform(title, type),
    _pNode(pNode),
    _ptyN(NULL)
{  
    updateProperty();
}
//-----------------------------------------------------------------------------
PtyTransformNode::~PtyTransformNode()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyTransformNode::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetTransform> pPW (new PtyWidgetTransform(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyTransformNode::updateData()
{
    if (_pNode!=null)
    {
        _pNode->beginMatrixUpdate();
            _pNode->setLocalPosition    (getLocalPosition());
            _pNode->setLocalOrientation (Core::Quaternionf(getLocalAxis()));
            _pNode->setLocalScale       (getLocalScale());
        _pNode->endMatrixUpdate();
        
        _aaboxInWorldCoord = _pNode->getBoundingBox();
    }

    if (_ptyN!=NULL)
    {
        _ptyN->updateChildren();
    }
}
//-----------------------------------------------------------------------------
void PtyTransformNode::updateProperty()
{
    //--------------
    if (_pNode!=null)
    {
        _originParentCoord        = _pNode->getLocalPosition();
        _axisParentCoord          = _pNode->getLocalOrient().toMatrix3();
        _scale                    = _pNode->getLocalScale();

        _masterOriginInWorldCoord = _pNode->getParentLocalToWorldMatrix().getTVector();
        _masterAxisInWorldCoord   = _pNode->getParentLocalToWorldMatrix().getRotation();
        _aaboxInWorldCoord        = _pNode->getBoundingBox();
    }
    //--------------

    if (_ptyN!=NULL)
    {
        _ptyN->updateChildren();
    }
}
//-----------------------------------------------------------------------------
void PtyTransformNode::updateMasterData(const Core::Vector3f& orig, const Core::Matrix3f& axis)
{
    _masterOriginInWorldCoord = orig;
    _masterAxisInWorldCoord   = axis;

    if (_ptyN!=NULL)
    {
        _ptyN->updateChildren();
    }
}
//-----------------------------------------------------------------------------
void PtyTransformNode::reset()
{
    //--------------
    _originParentCoord.setZero();
    _axisParentCoord.setIdentity();
    _scale.setOne();
    //--------------

    updateData();
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyTransformNode::clone() const
{
    Ptr<PtyTransformNode> pRet (new PtyTransformNode( *this ));
    pRet->_pNode = null;
    return pRet;
}
//-----------------------------------------------------------------------------
void PtyTransformNode::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<Universe::Node> tmpPNode = _pNode;

    Ptr<PtyTransformNode> pPty = LM_DEBUG_PTR_CAST<PtyTransformNode>(pSrc);

    //--------------
    _originParentCoord        = pPty->_originParentCoord;
    _axisParentCoord          = pPty->_axisParentCoord;
    _scale                    = pPty->_scale;

    _masterOriginInWorldCoord = pPty->_masterOriginInWorldCoord;
    _masterAxisInWorldCoord   = pPty->_masterAxisInWorldCoord;
    _aaboxInWorldCoord        = pPty->_aaboxInWorldCoord;
    //--------------

    setUniverseNode(tmpPNode);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setUniverseNode(const Ptr<Universe::Node>& pNode)
{
    _pNode = pNode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::Node>& PtyTransformNode::getUniverseNode() const
{
    return _pNode;
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformNode::getLocalPosition() const
{
    return _originParentCoord;
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformNode::getWorldPosition() const
{
    return _masterOriginInWorldCoord + _masterAxisInWorldCoord * getLocalPosition();
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformNode::getWorldDirection() const
{
    Core::Matrix3f mat =_axisParentCoord * _masterAxisInWorldCoord;
    return Core::Vector3f (
        mat._line[0][2],
        mat._line[1][2],
        mat._line[2][2]).getNormalizedSafe();
}
//-----------------------------------------------------------------------------
const Core::Matrix3f& PtyTransformNode::getLocalAxis() const
{
    return _axisParentCoord;
}
//-----------------------------------------------------------------------------
Core::Matrix3f PtyTransformNode::getWorldAxis() const
{
    return _masterAxisInWorldCoord * _axisParentCoord;
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformNode::getCenterBoundingBoxInWorldAxis() const
{
    return _aaboxInWorldCoord.getCenter();
}
//-----------------------------------------------------------------------------
Core::Orientationf PtyTransformNode::getLocalOrientationEuler() const
{
    return Core::Orientationf(getLocalAxis());
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformNode::getLocalScale() const
{
    return _scale;
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setLocalPosition(const Core::Vector3f& position)
{
    _originParentCoord = position;
    _ptyN->updateChildren();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setLocalAxis(const Core::Matrix3f& axis)
{
    _axisParentCoord = axis;
    _ptyN->updateChildren();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setLocalOrientationEuler(const Core::Orientationf& orientation)
{
    _axisParentCoord.setRotation(orientation);
    _ptyN->updateChildren();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setLocalScale(const Core::Vector3f& scale)
{
    const float limit = 0.05f;

    Core::Vector3f finalScale = scale;

    if (scale.x < limit)   finalScale.x = limit;
    if (scale.y < limit)   finalScale.y = limit;
    if (scale.z < limit)   finalScale.z = limit;
    _scale = finalScale;

    _ptyN->updateChildren();
    propertyChanged();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setWorldPosition(const Core::Vector3f& pos)
{
    _originParentCoord = convertWorldPositionToLocalPosition(pos);
}
//-----------------------------------------------------------------------------
void PtyTransformNode::setWorldOrientation(const Core::Matrix3f& orient)
{
    _axisParentCoord = (_masterAxisInWorldCoord.getTransposed() * orient);
}
//-----------------------------------------------------------------------------
Core::Vector3f  PtyTransformNode::convertWorldPositionToLocalPosition(const Core::Vector3f& worldPos)
{
    Core::Vector3f pos = worldPos - _masterOriginInWorldCoord;
    Core::Vector3f dst = _masterAxisInWorldCoord.getTransposed() *  pos;

    return dst;
}
//-----------------------------------------------------------------------------
void PtyTransformNode::translateInWorldAxis(const Core::Vector3f& offset)
{
    _originParentCoord = convertWorldPositionToLocalPosition(getWorldPosition() + offset);

    _ptyN->updateChildren();
    emit propertyChanged();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::rotateInWorldAxis(const Core::Rotationf& rotation)
{
    Core::Vector3f newWorldPosition = rotation.rotatePoint(getWorldPosition());
        _originParentCoord          = convertWorldPositionToLocalPosition(newWorldPosition);
   
        Core::Matrix3f mat = rotation.getAxisMatrix() * (_masterAxisInWorldCoord * _axisParentCoord);

        _axisParentCoord = _masterAxisInWorldCoord.getTransposed() * mat;

    _ptyN->updateChildren();
    emit propertyChanged();
}
//-----------------------------------------------------------------------------
void PtyTransformNode::addRotateInWorldAxis(const Core::Matrix3f& matAxis)
{
    setWorldOrientation(getWorldAxis()*matAxis);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTransformContent::PtyTransformContent(const Core::String& title, const Core::Matrix4f& parentToWorld, ETransformType type):
    PtyTransform(title, type),
    _masterAxisInWorldCoord(parentToWorld.getRotation()),
    _masterOriginInWorldCoord(parentToWorld.getTranslation())
{
    //updateProperty();
}
//-----------------------------------------------------------------------------
PtyTransformContent:: ~PtyTransformContent()
{
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyTransformContent::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent)
{
    Ptr<PtyWidgetTransform> pPW (new PtyWidgetTransform(pDtaProxy, parent));
    return pPW;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::updateData()
{
 
}
//-----------------------------------------------------------------------------
void PtyTransformContent::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyTransformContent::clone() const
{
    Ptr<PtyTransformContent> pRet (new PtyTransformContent( *this ));

    return pRet;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PtyTransformContent> pPty = LM_DEBUG_PTR_CAST<PtyTransformContent>(pSrc);

    //--------------
    _originParentCoord        = pPty->_originParentCoord;
    _axisParentCoord          = pPty->_axisParentCoord;
    _scale                    = pPty->_scale;

    _masterOriginInWorldCoord = pPty->_masterOriginInWorldCoord;
    _masterAxisInWorldCoord   = pPty->_masterAxisInWorldCoord;
    _aaboxInWorldCoord        = pPty->_aaboxInWorldCoord;
    //--------------

    updateData();
}
//-----------------------------------------------------------------------------
Core::Vector3f          PtyTransformContent::getLocalPosition() const
{
    return _originParentCoord;
}
//-----------------------------------------------------------------------------
const Core::Matrix3f&       PtyTransformContent::getLocalAxis() const
{
    return _axisParentCoord;
}
//-----------------------------------------------------------------------------
Core::Orientationf      PtyTransformContent::getLocalOrientationEuler() const
{
    return Core::Orientationf(getLocalAxis());
}
//-----------------------------------------------------------------------------
Core::Vector3f          PtyTransformContent::getWorldPosition() const
{
    return _masterOriginInWorldCoord + _masterAxisInWorldCoord * getLocalPosition();
}
//-----------------------------------------------------------------------------
Core::Matrix3f       PtyTransformContent::getWorldAxis() const
{
    return _masterAxisInWorldCoord * _axisParentCoord;
}
//-----------------------------------------------------------------------------
Core::Vector3f          PtyTransformContent::getCenterBoundingBoxInWorldAxis() const
{
    return getWorldPosition();
}
//-----------------------------------------------------------------------------
void PtyTransformContent::reset()
{
    _originParentCoord.setZero();
    _axisParentCoord.setIdentity();
    _scale.setOne();
}
//-----------------------------------------------------------------------------
Core::Vector3f PtyTransformContent::getLocalScale() const
{
    return _scale;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setLocalPosition(const Core::Vector3f& position)
{
    _originParentCoord = position;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setLocalOrientationEuler(const Core::Orientationf& orientation)
{
    _axisParentCoord.setRotation(orientation);
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setLocalAxis(const Core::Matrix3f& axis)
{
    _axisParentCoord = axis;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setLocalScale(const Core::Vector3f& scale)
{
    _scale = scale;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setWorldPosition(const Core::Vector3f& pos)
{
    _originParentCoord = convertWorldPositionToLocalPosition(pos);
}
//-----------------------------------------------------------------------------
Core::Vector3f  PtyTransformContent::convertWorldPositionToLocalPosition(const Core::Vector3f& worldPos)
{
    Core::Vector3f pos = worldPos - _masterOriginInWorldCoord;
    Core::Vector3f dst = _masterAxisInWorldCoord.getTransposed() *  pos;

    return dst;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::setWorldOrientation(const Core::Matrix3f& orient)
{
    _axisParentCoord = (_masterAxisInWorldCoord.getTransposed() * orient);
}
//-----------------------------------------------------------------------------
void PtyTransformContent::translateInWorldAxis   (const Core::Vector3f& offset)
{
    setWorldPosition(getWorldPosition()+offset);
  //  _localPosition += offset; // TODO
}
//-----------------------------------------------------------------------------
void PtyTransformContent::rotateInWorldAxis      (const Core::Rotationf& rotation)
{
    Core::Vector3f newWorldPosition = rotation.rotatePoint(getWorldPosition());
    _originParentCoord          = convertWorldPositionToLocalPosition(newWorldPosition);

    Core::Matrix3f mat = rotation.getAxisMatrix() * (_masterAxisInWorldCoord * _axisParentCoord);

    _axisParentCoord = _masterAxisInWorldCoord.getTransposed() * mat;
}
//-----------------------------------------------------------------------------
void PtyTransformContent::addRotateInWorldAxis   (const Core::Matrix3f& matAxis)
{
    setWorldOrientation(getWorldAxis()*matAxis);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetTransform::PtyWidgetTransform(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent) :
    PropertyWidget(data, parent)
{
    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetTransform::~PtyWidgetTransform()
{
}
//-----------------------------------------------------------------------------
void PtyWidgetTransform::readProperty()
{
    Ptr<PtyTransform> pP = LM_DEBUG_PTR_CAST<PtyTransform>(getDataProxy()->getProperty());

    _position->setValue(pP->getLocalPosition());

    Core::Orientationf orientation= pP->getLocalOrientationEuler();
    orientation.normalize2PI();
    _orientation->setValue(Core::Vector3f(Core::rad2deg(orientation._pitch), Core::rad2deg(orientation._yaw), Core::rad2deg(orientation._roll)));
    

    switch (pP->getType())
    {
    case MOVE_ROTATE_SCALE:
        _scale->setValue(pP->getLocalScale()*100);
        break;
    case MOVE_ROTATE_SCALE_UNIFORM:
        _scaleUniform->setValue(pP->getLocalScale().x*100);
        break;
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTransform::writeProperty(QWidget* pWidget)
{
    Ptr<PtyTransform> pP = LM_DEBUG_PTR_CAST<PtyTransform>(getDataProxy()->getProperty());

    if (pWidget == NULL || pWidget == _position)
    {
        Core::Vector3f  localPosition;
        _position->getValue(localPosition);
        pP->setLocalPosition(localPosition);
    }

    if (pWidget == NULL || pWidget == _orientation)
    {
        Core::Vector3f a;
        _orientation->getValue(a);
        pP->setLocalOrientationEuler(Core::Orientationf(Core::deg2rad(a.x), Core::deg2rad(a.y), Core::deg2rad(a.z)));
    }

    if (pWidget == NULL || pWidget == _scale || pWidget == _scaleUniform)
    {
        Core::Vector3f scaleLocal;
        switch (pP->getType())
        {
        case MOVE_ROTATE_SCALE:
            _scale->getValue(scaleLocal);
            scaleLocal /= 100;
            break;
        case MOVE_ROTATE_SCALE_UNIFORM:
            float valScale = _scaleUniform->getValue();
            valScale /= 100;
            scaleLocal.x = scaleLocal.y = scaleLocal.z = valScale;
            break;
        }
        pP->setLocalScale(scaleLocal);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTransform::setupUi()
{
    Ptr<PtyTransform> pP = LM_DEBUG_PTR_CAST<PtyTransform>(getDataProxy()->getProperty());

    _layout = new QGridLayout(this);
        _layout->setContentsMargins(0, 0, 0, 0);
        _layout->setSpacing(2);

    _groupBox = new QtToolbox::CollapsibleWidget(this, "Transform", true);

    QGridLayout* layoutGroupBox = new QGridLayout(NULL);
        layoutGroupBox->setContentsMargins(0, 0, 0, 0);
        layoutGroupBox->setVerticalSpacing(1);
        layoutGroupBox->setHorizontalSpacing(5);
    
    _reset          = new QPushButton("reset", this);   _reset->setFixedSize(60,20);
    _position       = new QtToolbox::NumberEditVector3(this, "Position", " m");
    _orientation    = new QtToolbox::NumberEditVector3(this, "Rotation", " �");

    layoutGroupBox->addWidget(_reset, 0,2, Qt::AlignRight);
    layoutGroupBox->addWidget(_position,1,0);
    layoutGroupBox->addWidget(_orientation,1,1);

    int32 widthScale;
    switch (pP->getType())
    {
    case MOVE_ROTATE_SCALE:
        _scale     = new QtToolbox::NumberEditVector3(this, "Scale", " %");

        widthScale = _scale->width();
        layoutGroupBox->addWidget(_scale,1,2);
        break;
    case MOVE_ROTATE_SCALE_UNIFORM:
        {
            _scaleUniform = new QtToolbox::NumberEdit("", " %", this);

            QLabel* labelScaleUniform  = new QLabel("Scale", this);
            QWidget* bidonScaleUniform = new QWidget(this);
            QGridLayout* layoutScaleUniform = new QGridLayout(bidonScaleUniform);
            

            layoutScaleUniform->setContentsMargins(0, 0, 0, 0);
            layoutScaleUniform->setSpacing(0);

            layoutScaleUniform->addWidget(labelScaleUniform);
            layoutScaleUniform->addWidget(_scaleUniform);
            QSpacerItem* Spacer = new QSpacerItem(120, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);
            layoutScaleUniform->addItem(Spacer, 2, 0, 1, 1);

           // bidonScaleUniform->setFixedSize(120, 80);
            bidonScaleUniform->setFixedHeight(80);
            bidonScaleUniform->setLayout(layoutScaleUniform);
            layoutGroupBox->addWidget(bidonScaleUniform,1,2);

            widthScale = bidonScaleUniform->width();
        }
        break;
    }

    _groupBox->setLayout(layoutGroupBox);
    _layout->addWidget(_groupBox);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);

    

    getWidgetsForUndoRedo().push_back(_position);
    getWidgetsForUndoRedo().push_back(_orientation);

    switch (pP->getType())
    {
    case MOVE_ROTATE_SCALE:
         getWidgetsForUndoRedo().push_back(_scale);
        break;
    case MOVE_ROTATE_SCALE_UNIFORM:
         getWidgetsForUndoRedo().push_back(_scaleUniform);
        break;
    }
   
    connect(_reset, SIGNAL(clicked()), this, SLOT(reset()));

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetTransform::reset()
{
    Core::List<Ptr<Property> >& pties = getDataProxy()->getProperties();
    Ptr<StateRecorder> pSR = getDataProxy()->getStateRecorder();

    beginChangeProperties(pSR, pties);

    for (int32 ii=0; ii<pties.size();++ii)
    {
        LM_DEBUG_PTR_CAST<PtyTransform>(pties[ii])->reset();
    }

    finishChangeProperties(pSR, pties);

    updatePropertyWidget();
}
//-----------------------------------------------------------------------------
void PtyWidgetTransform::collapse(bool flag)
{
    _groupBox->collapse(true);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI  Core::List<Ptr<Property> > removeChildren(Core::List<Ptr<Property> >& pties)
{
    Core::List<Ptr<Property> > ret;

    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        bool isPresent = false;
        for (int32 ij=0; ij<pties.size(); ++ij)
        {
            if (ii != ij)
            {
                Core::List<Ptr<Universe::Node> > children;
                if (pties[ij]->getCalssName() == PTY_TRANSFORM_NODE && pties[ii]->getCalssName() == PTY_TRANSFORM_NODE)
                {
                    Universe::getAllChildren(children, *LM_DEBUG_PTR_CAST<PtyTransformNode>(pties[ij])->getUniverseNode());

                    for (int32 iChild=0; iChild<children.size(); iChild++)
                    {
                        if(LM_DEBUG_PTR_CAST<PtyTransformNode>(pties[ii])->getUniverseNode() == children[iChild])
                        {
                            isPresent = true;
                        }
                    }
                }
            }
            
        }

        if (isPresent==false)
        {
            ret.push_back(pties[ii]);
        }
    }

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

} // namespace EPI


