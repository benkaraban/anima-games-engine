/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef PROPERTY_TRANSFORM_H_
#define PROPERTY_TRANSFORM_H_

#include <QWidget>

#include <Core/Standard.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Transform.h>

#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/PropertyWidget.moc.h>

namespace Core
{
    struct Rotationf;
}

#include <Universe/Node.h>

class QGridLayout;
namespace QtToolbox
{
    class CollapsibleWidget;
    class NumberEditVector3;
    class NumberEdit;
}//namespace QtToolbox

namespace EPI
{
class PtyNode;
class PtyWidgetTransform;
//-----------------------------------------------------------------------------
LM_ENUM_2 (ETransformType,
           MOVE_ROTATE_SCALE,
           MOVE_ROTATE_SCALE_UNIFORM)

class LM_API_EPI PtyTransform : public PropertyNode
{
   // Q_OBJECT

  /*  friend class PtyWidgetTransform;*/    //ici on fait ne exeption la PtyWidget ne doit pas acceder au membre de la pty

public:
    PtyTransform(const Core::String& title, ETransformType type = MOVE_ROTATE_SCALE_UNIFORM);
    PtyTransform(const PtyTransform& other);
    virtual ~PtyTransform();

    virtual EPropertyClassName      getCalssName() const {return PTY_TRANSFORM;}

    virtual Core::Vector3f          getLocalPosition() const =0;
    virtual const Core::Matrix3f&   getLocalAxis() const =0;
    virtual Core::Orientationf      getLocalOrientationEuler() const =0;

    virtual Core::Vector3f          getWorldPosition() const =0;
    virtual Core::Matrix3f          getWorldAxis() const =0;
    virtual Core::Vector3f          getCenterBoundingBoxInWorldAxis() const =0;
    
    virtual void reset() =0;
    virtual Core::Vector3f          getLocalScale() const =0;

    //
    virtual void setLocalPosition(const Core::Vector3f& position) =0;
    virtual void setLocalOrientationEuler(const Core::Orientationf& orientation) =0;
    virtual void setLocalAxis(const Core::Matrix3f& axis) =0;
    virtual void setLocalScale(const Core::Vector3f& scale) =0;

    virtual void setWorldOrientation(const Core::Matrix3f& orient) = 0;

    //offset Transform
    virtual void translateInWorldAxis   (const Core::Vector3f& offset) =0;
    virtual void rotateInWorldAxis      (const Core::Rotationf& rotation) = 0;
    virtual void addRotateInWorldAxis   (const Core::Matrix3f& matAxis) = 0;

    const ETransformType& getType() const;
    

private:
    ETransformType _type;
};


class LM_API_EPI PtyTransformContent : public PtyTransform
{
public:
    PtyTransformContent(const Core::String& title, const Core::Matrix4f& parentToWorld = Core::Matrix4f(Core::IDENTITY), ETransformType type = MOVE_ROTATE_SCALE_UNIFORM);
    virtual ~PtyTransformContent();

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    virtual void updateProperty();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    
    virtual Core::Vector3f          getLocalPosition() const;
    virtual const Core::Matrix3f&   getLocalAxis() const;
    virtual Core::Orientationf      getLocalOrientationEuler() const;

    virtual Core::Vector3f          getWorldPosition() const;
    virtual Core::Matrix3f          getWorldAxis() const;
    virtual Core::Vector3f          getCenterBoundingBoxInWorldAxis() const;
    
    virtual void reset();
    virtual Core::Vector3f          getLocalScale() const;

    void setWorldPosition(const Core::Vector3f& pos);
    virtual void setWorldOrientation(const Core::Matrix3f& orient);

    //
    virtual void setLocalPosition(const Core::Vector3f& position);
    virtual void setLocalOrientationEuler(const Core::Orientationf& orientation);
    virtual void setLocalAxis(const Core::Matrix3f& axis);
    virtual void setLocalScale(const Core::Vector3f& scale);

    //offset Transform
    virtual void translateInWorldAxis   (const Core::Vector3f& offset);
    virtual void rotateInWorldAxis      (const Core::Rotationf& rotation);
    virtual void addRotateInWorldAxis   (const Core::Matrix3f& matAxis);

    void setParentToWorldMatrix(const Core::Matrix4f& mat) {_masterAxisInWorldCoord = mat.getRotation(); _masterOriginInWorldCoord = mat.getTranslation();}

    Core::Vector3f  convertWorldPositionToLocalPosition(const Core::Vector3f& worldPos);

private:
    Core::Vector3f         _originParentCoord;          //position d'origine dans le repère parent;
    Core::Matrix3f         _axisParentCoord;            //orientation des axes dans le repère parent
    Core::Vector3f         _scale;

    Core::Vector3f         _masterOriginInWorldCoord;
    Core::Matrix3f         _masterAxisInWorldCoord;
    Core::AABoxf           _aaboxInWorldCoord;
    PtyNode*               _ptyN;
};

class LM_API_EPI PtyTransformNode : public PtyTransform
{
public:
    PtyTransformNode(const Ptr<Universe::Node> pNode, const Core::String& title, ETransformType type = MOVE_ROTATE_SCALE_UNIFORM);
    virtual ~PtyTransformNode();
    virtual EPropertyClassName getCalssName() const {return PTY_TRANSFORM_NODE;}

    virtual Ptr<PropertyWidget> internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDtaProxy, QWidget * parent = 0);

    virtual void updateData();
    virtual void updateProperty();
    void updateMasterData(const Core::Vector3f& orig, const Core::Matrix3f& axis);

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    
    virtual Core::Vector3f          getLocalPosition() const;
    virtual const Core::Matrix3f&   getLocalAxis() const;
    virtual Core::Orientationf      getLocalOrientationEuler() const;

    virtual Core::Vector3f          getWorldPosition() const;
    Core::Vector3f                  getWorldDirection() const;
    virtual Core::Matrix3f          getWorldAxis() const;
    virtual Core::Vector3f          getCenterBoundingBoxInWorldAxis() const;
    
    virtual void reset();
    virtual Core::Vector3f          getLocalScale() const;

    void setWorldPosition(const Core::Vector3f& pos);
    virtual void setWorldOrientation(const Core::Matrix3f& orient);
    //
    virtual void setLocalPosition(const Core::Vector3f& position);
    virtual void setLocalOrientationEuler(const Core::Orientationf& orientation);
    virtual void setLocalAxis(const Core::Matrix3f& axis);
    virtual void setLocalScale(const Core::Vector3f& scale);

    //offset Transform
    virtual void translateInWorldAxis   (const Core::Vector3f& offset);

    virtual void rotateInWorldAxis      (const Core::Rotationf& rotation);
    virtual void addRotateInWorldAxis   (const Core::Matrix3f& matAxis);

    void setUniverseNode(const Ptr<Universe::Node>& pNode);
    const Ptr<Universe::Node>& getUniverseNode() const;


    Core::Vector3f  convertWorldPositionToLocalPosition(const Core::Vector3f& worldPos);

    void setPtyNode(const PtyNode* ptyN) {_ptyN = const_cast<PtyNode*>(ptyN);}

public:
    Ptr<Universe::Node>     _pNode;
    ETransformType          _type;

    Core::Vector3f          _originParentCoord;          //position d'origine dans le repère parent;
    Core::Matrix3f          _axisParentCoord;            //orientation des axes dans le repère parent
    Core::Vector3f          _scale;

    Core::Vector3f          _masterOriginInWorldCoord;
    Core::Matrix3f          _masterAxisInWorldCoord;
    Core::AABoxf            _aaboxInWorldCoord;
    PtyNode*                _ptyN;
};
//-----------------------------------------------------------------------------
class LM_API_EPI PtyWidgetTransform : public PropertyWidget
{
    Q_OBJECT

    friend class PtyTransform;

public:
    PtyWidgetTransform(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent = 0);
    virtual ~PtyWidgetTransform();
    
    void readProperty();
    void writeProperty(QWidget* pWidget = NULL);
    void setupUi();
    virtual void collapse(bool flag);
private Q_SLOTS:
    void reset();

private:
    QGridLayout *                   _layout;
    QPushButton*                    _reset;

    QtToolbox::CollapsibleWidget * _groupBox;
    QtToolbox::NumberEditVector3*   _position;
    QtToolbox::NumberEditVector3*   _orientation;

    QtToolbox::NumberEditVector3*   _scale;
    QtToolbox::NumberEdit  *        _scaleUniform;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//si la liste pties contient les enfant de ses propre membre on les retire
LM_API_EPI  Core::List<Ptr<Property> > removeChildren(Core::List<Ptr<Property> >& pties);
//-----------------------------------------------------------------------------

} // namespace EPI

#endif //PROPERTY_TRANSFORM_H_
