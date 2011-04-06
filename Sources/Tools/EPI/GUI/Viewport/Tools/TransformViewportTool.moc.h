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
#ifndef TRANSFORM_VIEWPORT_TOOL_H_
#define TRANSFORM_VIEWPORT_TOOL_H_

#include <QObject>
#include <QWidget>
#include <QGridLayout>

#include <Core/Standard.h>
#include <Core/Math/Vector.h>
#include <Core/Math/Rotation.h>

#include <EPI/GUI/Viewport/Tools/ViewportTool.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>


namespace QtToolbox
{
    class ComboBox;
    class ComboBoxActions;
    class SpinBoxVector3;
    class SmartGroupBox;
    class PushButtonMenu;
    class NumberEdit;
    class NumberEditVector3;
}//namespace QtToolbox

namespace Universe
{
    class RessourcePool;
    class NodeGroup;
} //namespace Universe

namespace Renderer
{
    struct Material;
    class IFont;
    class IText;
}

namespace EPI
{
LM_ENUM_4 (EActionCenterType,
           PIVOT_CENTER,
           BOUNDING_BOX_CENTER,
           FREE_USER_CENTER,
           FREE_USER_CENTER_ROTATE_BOXCENTER);

LM_ENUM_2 (EOrientationType,
           LOCAL_ORIENTATION,
           GLOBAL_ORIENTATION);

struct DrawContext;
class MoveRotateScaleTool;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI TransformTool : public IViewportTool
{
	friend class MoveRotateScaleTool;

    Q_OBJECT

protected:
    TransformTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter = PIVOT_CENTER, const EOrientationType& orientationType = LOCAL_ORIENTATION);

public:
    virtual ~TransformTool();

    virtual void mouseMoveEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent(QMouseEvent* event,const DrawContext& dc) {};
    virtual void mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc) {};

    static void createTransformModels(Ptr<Universe::RessourcePool>& pPool);

    virtual const Core::List<Ptr<Property> >& getProperties() const;
    virtual Core::List<Ptr<Property> >&       getProperties();

    const QList<QAction*>& getActionsActionCenter() const;
    const QList<QAction*>& getActionsActionOrientation() const;
    
    EActionCenterType   getActionCenterType() const;
    void setActionCenter(EActionCenterType a) {_actionCenterType = a;}
    EOrientationType    getActionOrientationType() const;
    void setActionOrientationType(EOrientationType o) {_orientationType = o;}

    const Core::Vector3f&     getActionCenter()    const;
    const Core::Quaternionf&  getActionOrientation() const;


    const Core::Vector3f&   getLocalScale() const {return _localScale;}
    void                    setLocalScale(const Core::Vector3f& scale);

    void setActionCenter (const Core::Vector3f& actionPos);
    void setActionOrientation(const Core::Quaternionf& newActionOr);

    static void rotateInWorldAxis(const Core::Rotationf& rotation, Core::List<Ptr<Property> >& _pties, Ptr<Property> pContainer);

    void moveTo(const EOrientationType& type, const Core::Vector3f& pos);

    void setContainer(const Ptr<Property>& pContainer) {_pContainer=pContainer;}


    void connectSignalpropertiesChanged(bool flag);

    virtual void mouseMiddleSingleClick (QMouseEvent* event, const DrawContext& dc);

private Q_SLOTS:
    virtual void selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected);

public Q_SLOTS:
    virtual void updateViewportTool();

private:
    //void makeGuiDocumentConnection(bool makeConnection);
    void updatePropertiesTargets(const Core::List<Ptr<Property> >& ptiesSelected);
    virtual void internalChangeSelection()=0;

protected:
    void computeActionCenters();
    void computeActionOrientations();
    void computeLocalScale();


protected:
    //Ptr<GuiDocument>& _pGDoc;
    Core::List<Ptr<Property> >  _pties;



public Q_SLOTS:
    void enableAcionCenterPivot();
    void enableAcionCenterBoundingBox();
    void enableAcionCenterFreeUser();
    void enableAcionCenterFreeUserRotateCenterBox();

    void enableOrientationLocal();
    void enableOrientationGlobal();

protected:
    QList<QAction*> _actionsActionCenter;
    QList<QAction*> _actionsOrientation;

    Core::Vector3f  _pivotInWorldAxis;
    Core::Vector3f  _centerBoundingBoxInWorldAxis;
    Core::Vector3f  _pivotFreeUserInWorldAxis;

    Core::Vector3f  _localScale;

    Core::Quaternionf  _localOrientation;
    Core::Quaternionf  _globalOrientation;


    static Renderer::Material   _sMatHighlighted;
    static Renderer::Material   _sMatBlack;
    static Renderer::Material   _sMatAxeX;
    static Renderer::Material   _sMatAxeY;
    static Renderer::Material   _sMatAxeZ;
    static Renderer::Material   _sMatAxeXY;
    static Renderer::Material   _sMatAxeXZ;
    static Renderer::Material   _sMatAxeYZ;
    static Renderer::Material   _sMatAxeXYZ;

    EActionCenterType   _actionCenterType;
    EOrientationType    _orientationType;
    Ptr<Property>       _pContainer;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_ENUM_8(EMoveToolElement,
        NOTHING,
        AXE_X,
        AXE_Y,
        AXE_Z,
        AXE_XY,
        AXE_XZ,
        AXE_YZ,
        AXE_XYZ)

class LM_API_EPI MoveTool : public TransformTool
{
	friend class MoveRotateScaleTool;
public:
    MoveTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter = PIVOT_CENTER, const EOrientationType& orientationType = LOCAL_ORIENTATION);
    virtual ~MoveTool();

    virtual void mouseMoveEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent(QMouseEvent* event,const DrawContext& dc);
    virtual void mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc);

    void setVisible(bool show);
    virtual bool isHighlighted() const;
    virtual bool isSelected() const;
    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc); 

    static void                      createMoveToolModels   (Ptr<Universe::RessourcePool>& pPool);
    static Ptr<Universe::NodeGroup>  createMoveTool         (Ptr<Universe::World>& pWorld);
    static void                      configureMoveTool      (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected);
    static void                      configureVisible       (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected);
    static void                      hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc);

    virtual EViewportToolType   getType() const {return MOVE_TOOL;}
private:
    void disableHighlighted();
    
    void configureHighlighted();
    virtual void internalChangeSelection();
    

    static void initMoveAction(const Core::Rayf& ray, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Vector3f& actionCenter, Core::Planef& workPlane, Core::Vector3f& startPoint);
    static void makeMoveAction(const Core::Rayf& ray, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Planef& workPlane, Core::Vector3f& startPoint, Core::List<Ptr<Property> >& pties, Ptr<Property> pContainer);

private:
    EMoveToolElement            _elementHighlighted;
    EMoveToolElement            _elementSelected;

    Ptr<Universe::NodeGroup>    _pNode;
    bool                        _isVisible;

    Core::Planef                _workPlane;
    Core::Vector3f              _startPoint;
};
//-----------------------------------------------------------------------------
class LM_API_EPI MoveToolWidget : public IViewportToolWidget
{
public:
    MoveToolWidget(MoveTool& mt, QWidget * parent);
    virtual ~MoveToolWidget();
    virtual IViewportTool& getViewportTool();

private:
    void setupUi();
    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);

private:
    MoveTool& _mt;
    QGridLayout * _layout;

    //QtToolbox::ComboBox*    _pActionCenter;
    //QtToolbox::SmartGroupBox * _groupBox;
    QtToolbox::NumberEditVector3* _pMove;

    QtToolbox::ComboBoxActions*  _pActionCenter;
    QtToolbox::ComboBoxActions*  _pOrientation;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI RotateTool : public TransformTool
{
    friend class MoveRotateScaleTool;
public:
    RotateTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const Ptr<Renderer::IFont>& pFont=null, const Ptr<Renderer::IHUD>& pHUD = null, const EActionCenterType& actionsActionCenter = PIVOT_CENTER, const EOrientationType& orientationType = LOCAL_ORIENTATION);
    virtual ~RotateTool();
    
    virtual void mouseMoveEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent(QMouseEvent* event,const DrawContext& dc);
    virtual void mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc);

    void setVisible(bool show);
    virtual bool isHighlighted() const;
    virtual bool isSelected() const;

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc); 

    static void createRotateToolModels(Ptr<Universe::RessourcePool>& pPool);
    static void  createRotateTool         (Ptr<Universe::World>& pWorld, Ptr<Universe::NodeGroup>& pNodeTool, Ptr<Universe::NodeGroup>& pNodeAngle);
    static void                      configureRotateTool      (Ptr<Universe::NodeGroup>& pNodeTool, Ptr<Universe::NodeGroup>& pNodeAngle, const DrawContext& dc, const EMoveToolElement elementSelected, const Core::Vector3f& splineFirstPoint, const Core::Vector3f& splineLastPoint, bool interactionPositionCursor);
    static void                      configureVisible       (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected);
    static void                      hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc);

    virtual Ptr<Renderer::IHUD> getHUD() const;
    virtual EViewportToolType   getType() const {return ROTATE_TOOL;}
private:
    void disableHighlighted();
    void configureHighlighted();
    virtual void internalChangeSelection();

    static void initRotateAction(const Core::Rayf& ray, const DrawContext& dc, const Core::Vector2f& cursorPos, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Vector3f& actionCenter,
                                  bool & interactionPositionCursor, Core::Vector2f& lastCursorPos, bool& invertAxis, Core::Planef& workPlane, Core::Vector3f& startPoint, Core::Vector3f& splineFirstPointInWorldAxis, Core::Vector3f& splineLastPointInWorldAxis);
    static void makeRotateAction(const Core::Rayf& ray, const Core::Vector2f& cursorPos, const DrawContext& dc, const bool& interactionPositionCursor, const Core::Planef& workPlane,
                                  const bool& invertAxis, const EMoveToolElement& elementSelected, const Ptr<Universe::Node> pNode, const Core::Vector3f& actionCenter, Core::Vector3f& startPoint, Core::Vector3f& splineLastPointInWorldAxis, Core::Vector2f& lastCursorPos, Core::List<Ptr<Property> >& pties, Ptr<Property> pContainer);

private:
    EMoveToolElement _elementHighlighted;
    EMoveToolElement _elementSelected;
    Ptr<Universe::NodeGroup>   _pNode;
    Ptr<Universe::NodeGroup>   _pNodeAngle;
    bool _isVisible;
    Core::Planef                _workPlane;
    Core::Vector3f              _startPoint;
    
    Core::Vector3f              _splineFirstPointInWorldAxis;
    Core::Vector3f              _splineLastPointInWorldAxis;
    Ptr<Renderer::IHUD>         _pHUD;
    Ptr<Renderer::IText>        _pTextInfo;
    bool                        _interactionPositionCursor;
    bool                        _invertAxis;
    Core::Vector2f              _lastCursorPos;
};
//-----------------------------------------------------------------------------
class LM_API_EPI RotateToolWidget : public IViewportToolWidget
{
public:
    RotateToolWidget(RotateTool& rt, QWidget * parent);
    virtual ~RotateToolWidget();

    virtual IViewportTool& getViewportTool();

private:
    void setupUI();
    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);

private:
    RotateTool& _rt;
    QGridLayout * _layout;

    QtToolbox::ComboBoxActions*  _pActionCenter;
    QtToolbox::ComboBoxActions*  _pOrientation;
    QtToolbox::NumberEditVector3* _pValue;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI ScaleTool : public TransformTool
{
    friend class MoveRotateScaleTool;
public:
    ScaleTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties);
    virtual ~ScaleTool();

    virtual void mouseMoveEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent(QMouseEvent* event,const DrawContext& dc);
    virtual void mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc);

    void setVisible(bool show);
    virtual bool isHighlighted() const;
    virtual bool isSelected() const;

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc); 

    static void createScaleToolModels(Ptr<Universe::RessourcePool>& pPool);
    static Ptr<Universe::NodeGroup>  createScaleTool         (Ptr<Universe::World>& pWorld);
    static void                      configureScaleTool      (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected);
    static void                      configureVisible       (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected);
    static void                      hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc);

    virtual EViewportToolType   getType() const {return SCALE_TOOL;}
private:
    void disableHighlighted();
    void configureHighlighted();
    virtual void internalChangeSelection();

    void initScaleAction(const Core::Rayf& ray, const DrawContext& dc);
    void makeScaleAction(const Core::Rayf& ray);

private:
    EMoveToolElement _elementHighlighted;
    EMoveToolElement _elementSelected;
    Ptr<Universe::NodeGroup>   _pNode;
    bool _isVisible;

    Core::Planef                _workPlane;
    Core::Vector3f              _startPoint;
    float                       _startDistance;

    Core::Matrix4f              _WtoView;
};
//-----------------------------------------------------------------------------
class LM_API_EPI ScaleToolWidget : public IViewportToolWidget
{
public:
    ScaleToolWidget(ScaleTool& st, QWidget * parent);
    virtual ~ScaleToolWidget();

    virtual IViewportTool& getViewportTool();

private:
    void setupUI();
    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);

private:
    ScaleTool& _st;
    QGridLayout * _layout;
    QtToolbox::SmartGroupBox *          _groupBox;
    QtToolbox::NumberEdit* _scale;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_EPI MoveRotateScaleTool : public TransformTool
{

public:
    MoveRotateScaleTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter = PIVOT_CENTER, const EOrientationType& orientationType = LOCAL_ORIENTATION);
    virtual ~MoveRotateScaleTool();

    virtual void mouseMoveEvent        (QMouseEvent* event,const DrawContext& dc);
    virtual void mousePressEvent(QMouseEvent* event,const DrawContext& dc);
    virtual void mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc);

    void setVisible(bool show);
    virtual bool isHighlighted() const;
    virtual bool isSelected() const;

    virtual Ptr<IViewportToolWidget> createViewportToolWidget(QWidget * parent);
    virtual void guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc); 

    static Ptr<Universe::NodeGroup> createTools(Ptr<Universe::World>& pWorld);
    void configureTools (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement moveElementSelected, const EMoveToolElement rotateElementSelected, const Core::Vector3f& splineFirstPoint, const Core::Vector3f& splineLastPoint);
    static void hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc);

    


    virtual EViewportToolType   getType() const {return MULTI_TRANSFORM_TOOL;}
private:
    void configureVisible();
    void disableHighlighted();
    void configureHighlighted();
    virtual void internalChangeSelection();

private:
	Ptr<Universe::NodeGroup>    _pNode;

    EMoveToolElement            _elementMoveHighlighted;
    EMoveToolElement            _elementMoveSelected;

    EMoveToolElement            _elementRotateHighlighted;
    EMoveToolElement            _elementRotateSelected;

    Core::Vector3f              _splineFirstPointInWorldAxis;
    Core::Vector3f              _splineLastPointInWorldAxis;

    bool                        _isVisible;

    bool                        _interactionPositionCursor;
        bool                        _invertAxis;
    Core::Vector2f              _lastCursorPos;

    Core::Planef                _moveWorkPlane;
    Core::Vector3f              _moveStartPoint;

    Core::Planef                _rotateWorkPlane;
    Core::Vector3f              _rotateStartPoint;
};
//-----------------------------------------------------------------------------
class LM_API_EPI MoveRotateScaleToolWidget : public IViewportToolWidget
{
public:
    MoveRotateScaleToolWidget(MoveRotateScaleTool& tool, QWidget * parent);
    virtual ~MoveRotateScaleToolWidget();

    virtual IViewportTool& getViewportTool();

private:
    void setupUI();
    virtual void readViewportTool();
    virtual void writeViewportTool(QWidget* pWidget = NULL);

private:
    MoveRotateScaleTool& _tool;
    QGridLayout * _layout;

    QtToolbox::ComboBoxActions*  _pActionCenter;
    QtToolbox::ComboBoxActions*  _pOrientation;


    QtToolbox::NumberEditVector3* _move;
    QtToolbox::NumberEditVector3* _rotate;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI

#endif //TRANSFORM_VIEWPORT_TOOL_H_