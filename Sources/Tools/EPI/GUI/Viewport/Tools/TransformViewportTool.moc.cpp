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
#include <EPI/GUI/Viewport/Tools/TransformViewportTool.moc.h> 

#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>

#include <QAction>
#include <QMouseEvent>

#include <EPI/Gui/GuiDocument.moc.h>

#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/PushButtonMenu.moc.h>
#include <QtToolbox/SpinBoxVector.moc.h>
#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/NumberEdit.moc.h>
#include <QtToolbox/NumberEditVector3.moc.h>

#include <EPI/Document/Property.moc.h>
#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/Properties/PtyNodeInstance.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>
#include <Core/Math/GeometryComputations.h>
#include <Core/Math/MathTools.h>

#include <Core/Logger.h>


#include <qapplication.h>

#include <EPI/Gui/GuiContext.moc.h>

#include <EPI/Gui/Viewport/NavigationStyle.h>

#include <Universe/NodeSpline.h>
#include <Universe/NodeGroup.h>
#include <Universe/World.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
const String    ARROW_MODEL_NAME            = L"$MoveToolArrow";
const String    ARROW_MESH_NAME             = L"$MoveToolArrowMesh";
const String    BIAXIS_MODEL_NAME           = L"$MoveToolBiaxis";
const String    BIAXIS_MESH_NAME            = L"$MoveToolBiaxisMesh";
const float     ARROW_SIZE_FACTOR           = 2.6f;

const String    SCALE_UNIFORM_MODEL_NAME    = L"$ScaleToolUniform";
const String    SCALE_UNIFORM_MESH_NAME     = L"$ScaleToolUniformMesh";
const float     SCALE_UNIFORM_SIZE_FACTOR   = 0.07f;  //sur [0 1]
const String    SCALE_SIZE_MODEL_NAME       = L"$ScaleToolSize";
const String    SCALE_SIZE_MESH_NAME        = L"$ScaleToolSizeMesh";

const String    ROTATE_TORUS_MODEL_NAME     = L"$RotateToolUniform";
const String    ROTATE_TORUS_MESH_NAME      = L"$RotateToolUniformMesh";
const String    ROTATE_TORUS_360_MODEL_NAME = L"$RotateToolUniformTorus360";
const String    ROTATE_TORUS_360_MESH_NAME = L"$RotateToolUniformTorus360Mesh";

const float     TORUS_MINOR_RAIUS_FACTOR    = 0.004f;
const float     TORUS_MAJOR_RAIUS_FACTOR    = 0.2f;

const String    tagTorusAxeX                = L"TorusAxeX";
const String    tagTorusAxeY                = L"TorusAxeY";
const String    tagTorusAxeZ                = L"TorusAxeZ";
const String    tagTorusAxeCamera           = L"TorusAxeCamera";
const String    tagSplineRotStart           = L"RotSplineStart";
const String    tagSplineRotEnd             = L"RotSplineEnd";

const String    tagArrowAxeX                = L"ArrowAxeX";
const String    tagArrowAxeY                = L"ArrowAxeY";
const String    tagArrowAxeZ                = L"ArrowAxeZ";

const String    tagBiaxisAxeXY              = L"BiaxisAxeXY";
const String    tagBiaxisAxeXZ              = L"BiaxisAxeXZ";
const String    tagBiaxisAxeYZ              = L"BiaxisAxeYZ";
const String    tagMoveCenter               = L"MoveCenter";

const String    tagScaleUniform             = L"ScaleUniform";
const String    tagScaleSize                = L"ScaleSize";


const String    tagMultiTransformMove        = L"MultiTrasformMove";
const String    tagMultiTransformRotate      = L"MultiTrasformRotate";
const String    tagMultiTransformRotateAngle = L"MultiTrasformRotateAngle";

const Core::Vector4f    colorInfo (0.9f, 0.6f, 0.9f, 1.f);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void setLine(Ptr<Universe::NodeSpline> pNode, const Core::Vector3f& start, const Core::Vector3f& end)
{
    Core::CRSpline  spline;
    Core::List<Core::Vector3f>& pts = spline.getPoints();
        pts.push_back(start);
        pts.push_back(start);
        pts.push_back(start);
        pts.push_back(end);
        pts.push_back(end);
        pts.push_back(end);
    pNode->setSpline(spline);

    pNode->setResolution(1);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
QtToolbox::ComboBoxActions* createActionCenterComboBox(QWidget* parent, const TransformTool& tool)
{
    QtToolbox::ComboBoxActions* pCombo = new QtToolbox::ComboBoxActions(parent, L"Action Center");

    EActionCenterType actionCenter = tool.getActionCenterType();
    pCombo->initialiseActions(const_cast<QList<QAction *> *>(&tool.getActionsActionCenter()));
    const_cast<TransformTool&>(tool).setActionCenter(actionCenter);

    pCombo->selectAction(tool.getActionsActionCenter()[tool.getActionCenterType()]->text());

    return pCombo;
}
//-----------------------------------------------------------------------------
QtToolbox::ComboBoxActions* createOrientationComboBox(QWidget* parent, const TransformTool& tool)
{
    QtToolbox::ComboBoxActions* pCombo = new QtToolbox::ComboBoxActions(parent, L"Orientation");

    EOrientationType orientation = tool.getActionOrientationType();
    pCombo->initialiseActions(const_cast<QList<QAction *> *>(&tool.getActionsActionOrientation()));
    const_cast<TransformTool&>(tool).setActionOrientationType(orientation);

    pCombo->selectAction(tool.getActionsActionOrientation()[tool.getActionOrientationType()]->text());

    return pCombo;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
float computeScaleFactorCamOrthogonale(const Ptr<Universe::NodeGroup>& pNode, const Ptr<NodeCameraEdition>& pCamera)
{
    Core::Vector3f posVTool = pNode->getWorldPosition();

    Ptr<Universe::NodeOrthogonalCamera> pCamOrtho = LM_DEBUG_PTR_CAST<Universe::NodeOrthogonalCamera>(pCamera->getActiveViewCamera());

    const Ptr<Renderer::IOrthoCamera> & camOtho = pCamOrtho->getCamera();


    float width = camOtho->getRightPlane() - camOtho->getLeftPlane();

   
    //rayon du VTool
    const Core::AABoxf & nodeBox =  pNode->getBoundingBox();
    Core::Vector3f nodeRadius1 = (nodeBox.min - posVTool);
    Core::Vector3f nodeRadius2 = (nodeBox.max - posVTool);

    Core::Vector3f nodeRadius;
    
    if (nodeRadius1.getLengthSafe() > nodeRadius2.getLengthSafe())
    {
        nodeRadius = nodeRadius1;
    }
    else
    {
        nodeRadius = nodeRadius2;
    }

    float goodius = width / 3.f;



    float a = nodeRadius.getLengthSafe();

    return goodius/a;
}
//-----------------------------------------------------------------------------
float computeScaleFactorCamPerspective(const Ptr<Universe::NodeGroup>& pNode, const Ptr<Universe::NodeCamera>& pCamera)
{
    const float angle = f_PI * 0.25f *0.25f;

    //calcul du plan qui passe par le centre du VTool ayant pour normal le vecteur (posNode-posCam)
    Core::Vector3f posVTool = pNode->getWorldPosition();
    Core::Vector3f posCam = pCamera->getWorldPosition();

    Core::Planef toolPlane (Core::Plane_NormalPoint, (posVTool-posCam).getNormalizedSafe(), posVTool);

    //on cherche la rotation
    Core::Vector3f coplanaire;

    if (toolPlane.normal.isZero() == true)
    {
        return 1.f;
    }

    if ( Core::L_abs(toolPlane.normal.z) > f_COMMON_EPSILON)
    {
        Core::Vector3f posPlan  (5, 12, 0);
        posPlan.z = (toolPlane.distance - toolPlane.normal.x*posPlan.x - toolPlane.normal.y*posPlan.y) / toolPlane.normal.z;
        coplanaire = (posPlan - posVTool);
    }
    else
    {
        if (Core::L_abs(toolPlane.normal.x) > f_COMMON_EPSILON)
        {
            Core::Vector3f posPlan  (0, 12, 5);
            posPlan.x = (toolPlane.distance - toolPlane.normal.z*posPlan.z - toolPlane.normal.y*posPlan.y) / toolPlane.normal.x;
            coplanaire = (posPlan - posVTool);
        }
        else
        {
            Core::Vector3f posPlan  (12, 0, 5);
            posPlan.y = (toolPlane.distance - toolPlane.normal.z*posPlan.z - toolPlane.normal.x*posPlan.x) / toolPlane.normal.y;
            coplanaire = (posPlan - posVTool);
        }
    }
    
   
    Core::Quaternionf rotation (Core::AXIS, coplanaire.getNormalizedSafe(), angle);

    Core::Rayf ray(Core::Ray_OriginDirection, pCamera->getWorldPosition(), (rotation * (posVTool-posCam)).getNormalizedSafe());
    
    float t = - (dot(ray.origin, -toolPlane.normal) + toolPlane.distance) / dot(ray.direction, -toolPlane.normal);
    Core::Vector3f p = ray.origin + t * ray.direction;

    //rayon du VTool
    const Core::AABoxf & nodeBox =  pNode->getBoundingBox();
    Core::Vector3f nodeRadius1 = (nodeBox.min - posVTool);
    Core::Vector3f nodeRadius2 = (nodeBox.max - posVTool);

    Core::Vector3f nodeRadius;
    
    if (nodeRadius1.getLengthSafe() > nodeRadius2.getLengthSafe())
    {
        nodeRadius = nodeRadius1;
    }
    else
    {
        nodeRadius = nodeRadius2;
    }

    //calcul du rayon souhaite
    Core::Vector3f  goodRadius (p - posVTool);

    float a = nodeRadius.getLengthSafe();
    float b = goodRadius.getLengthSafe();

    return b/a;
}
//-----------------------------------------------------------------------------
float computeScaleFactor(const Ptr<Universe::NodeGroup>& pNode, const Ptr<Universe::NodeCamera>& pCamera)
{
    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(pCamera);

    if (pNodeCamEdit != null)   return computeScaleFactorCamOrthogonale(pNode, pNodeCamEdit);
    else                        return computeScaleFactorCamPerspective(pNode, pCamera);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Ptr<PtyTransform> getPtyTransformFromPty(const Ptr<Property>& pPty)
{
    Ptr<PtyTransform> ret;

    if (pPty->isTransformable() == true)
    {
        Ptr<ITransformable> pTransformable = LM_DEBUG_PTR_CAST<ITransformable>(pPty);
        ret = LM_DEBUG_PTR_CAST<PtyTransform>(pTransformable->getPtyTransform());
    }
    else
    {
        ret = null;
    }

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Renderer::Material TransformTool::_sMatAxeX;
Renderer::Material TransformTool::_sMatAxeY;
Renderer::Material TransformTool::_sMatAxeZ;
Renderer::Material TransformTool::_sMatAxeXY;
Renderer::Material TransformTool::_sMatAxeXZ;
Renderer::Material TransformTool::_sMatAxeYZ;
Renderer::Material TransformTool::_sMatAxeXYZ;
Renderer::Material TransformTool::_sMatHighlighted;
Renderer::Material TransformTool::_sMatBlack;
//-----------------------------------------------------------------------------
TransformTool::TransformTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter, const EOrientationType& orientationType):
    IViewportTool(pStateRecorder),
    _actionCenterType(actionsActionCenter),
    _orientationType(orientationType),
    _pContainer(null),
    _localScale(Core::ONE)
{
    //material
    const float matAlpha = 1.0f;
    Renderer::Material mat;
        mat.diffuse.r = 1.0f;
        mat.diffuse.g = 0.f;
        mat.diffuse.b = 0.f;
        mat.diffuse.a = matAlpha;
        mat.ambient = Core::Vector4f(0.01f);
        mat.ambient.a = 1.f;
        mat.dynamicShadowCaster = false;
        mat.dynamicShadowReceiver = false;
        mat.emissive = Core::Vector4f(0.f);
        mat.emissive.a = 1.f;
        mat.shininess = 12.f;
        mat.specular = Core::Vector4f(0.05f);
        mat.specular.a = 1.0f;
        mat.glowColor = Core::Vector4f(0.f);
        mat.glowColor.a = 1.f;
        mat.glowExtent = Core::Vector3f(0.f);
        mat.reflexions = Core::Vector4f(0.f);
        mat.reflexions.a = 0.f;
        mat.staticLighting = false;
        mat.transDiffuseTex = false;
        mat.zBias= 0.1f;

        TransformTool::_sMatAxeX = mat;
            TransformTool::_sMatAxeX.diffuse.r = 1.f;
            TransformTool::_sMatAxeX.diffuse.g = 0.08f;
            TransformTool::_sMatAxeX.diffuse.b = 0.08f;
        TransformTool::_sMatAxeY = mat;
            TransformTool::_sMatAxeY.diffuse.r = 0.08f;
            TransformTool::_sMatAxeY.diffuse.g = 1.f;
            TransformTool::_sMatAxeY.diffuse.b = 0.08f;
        TransformTool::_sMatAxeZ = mat;
            TransformTool::_sMatAxeZ.diffuse.r = 0.08f;
            TransformTool::_sMatAxeZ.diffuse.g = 0.08f;
            TransformTool::_sMatAxeZ.diffuse.b = 1.f;
        TransformTool::_sMatAxeXY = mat;
            TransformTool::_sMatAxeXY.diffuse.r = 1.f;
            TransformTool::_sMatAxeXY.diffuse.g = 1.f;
            TransformTool::_sMatAxeXY.diffuse.b = 0.08f;
        TransformTool::_sMatAxeXZ = mat;
            TransformTool::_sMatAxeXZ.diffuse.r = 1.f;
            TransformTool::_sMatAxeXZ.diffuse.g = 0.08f;
            TransformTool::_sMatAxeXZ.diffuse.b = 1.f;
        TransformTool::_sMatAxeYZ = mat;
            TransformTool::_sMatAxeYZ.diffuse.r = 0.08f;
            TransformTool::_sMatAxeYZ.diffuse.g = 1.f;
            TransformTool::_sMatAxeYZ.diffuse.b = 1.f;
        TransformTool::_sMatAxeXYZ = mat;
            TransformTool::_sMatAxeXYZ.diffuse.r = 0.2f;
            TransformTool::_sMatAxeXYZ.diffuse.g = 0.2f;
            TransformTool::_sMatAxeXYZ.diffuse.b = 0.1f;
        TransformTool::_sMatHighlighted = mat;
            TransformTool::_sMatHighlighted.diffuse.r = 0.8f;
            TransformTool::_sMatHighlighted.diffuse.g = 0.8f;
            TransformTool::_sMatHighlighted.diffuse.b = 0.8f;
        TransformTool::_sMatBlack = mat;
            TransformTool::_sMatBlack.specular = Core::Vector4f(0.f);
            TransformTool::_sMatBlack.diffuse.r = 0.2f;
            TransformTool::_sMatBlack.diffuse.g = 0.2f;
            TransformTool::_sMatBlack.diffuse.b = 0.25f;
            

    for (int32 ii=0; ii<EActionCenterType_COUNT; ++ii)
    {
        QAction* pAct = new QAction(this);

        switch(ii)
        {
        case PIVOT_CENTER:
            pAct->setText("Pivot");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableAcionCenterPivot()));
            break;
        case BOUNDING_BOX_CENTER:
            pAct->setText("Center");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableAcionCenterBoundingBox()));
            break;
        case FREE_USER_CENTER:
            pAct->setText("Free");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableAcionCenterFreeUser()));
            break;
        case FREE_USER_CENTER_ROTATE_BOXCENTER:
            pAct->setText("Free Rotate Center");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableAcionCenterFreeUserRotateCenterBox()));
            break;
        }

        _actionsActionCenter.push_back(pAct);
    }


    for (int32 ii=0; ii<EOrientationType_COUNT; ++ii)
    {
        QAction* pAct = new QAction(this);

        switch(ii)
        {
        case LOCAL_ORIENTATION:
            pAct->setText("Local");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableOrientationLocal()));
            break;
        case GLOBAL_ORIENTATION:
            pAct->setText("Global");
            connect(pAct, SIGNAL(triggered()), this, SLOT(enableOrientationGlobal()));
            break;
        }

        _actionsOrientation.push_back(pAct);
    }
 
    updatePropertiesTargets(pties);
    _pivotFreeUserInWorldAxis = _centerBoundingBoxInWorldAxis;
}
//-----------------------------------------------------------------------------
TransformTool::~TransformTool()
{
}
//-----------------------------------------------------------------------------
const QList<QAction*>& TransformTool::getActionsActionCenter() const
{
    return _actionsActionCenter;
}
//-----------------------------------------------------------------------------
void TransformTool::enableAcionCenterPivot()
{
    _actionCenterType = PIVOT_CENTER;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::enableAcionCenterBoundingBox()
{
    _actionCenterType = BOUNDING_BOX_CENTER;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::enableAcionCenterFreeUser()
{
    _actionCenterType = FREE_USER_CENTER;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::enableAcionCenterFreeUserRotateCenterBox()
{
    _actionCenterType = FREE_USER_CENTER_ROTATE_BOXCENTER;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::enableOrientationLocal()
{
    _orientationType = LOCAL_ORIENTATION;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::enableOrientationGlobal()
{
    _orientationType = GLOBAL_ORIENTATION;
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
const QList<QAction*>& TransformTool::getActionsActionOrientation() const
{
    return _actionsOrientation;
}
//-----------------------------------------------------------------------------
void TransformTool::createTransformModels(Ptr<Universe::RessourcePool>& pPool)
{
    MoveTool::createMoveToolModels(pPool);
    ScaleTool::createScaleToolModels(pPool);
    RotateTool::createRotateToolModels(pPool);
}
//-----------------------------------------------------------------------------
void TransformTool::selectionChanged(const Core::List<Ptr<Property> >& ptiesSelected)
{
    updatePropertiesTargets(ptiesSelected);
    internalChangeSelection();
}
//-----------------------------------------------------------------------------
EActionCenterType   TransformTool::getActionCenterType() const
{
    return _actionCenterType;
}
//-----------------------------------------------------------------------------
EOrientationType    TransformTool::getActionOrientationType() const
{
    return _orientationType;
}
//-----------------------------------------------------------------------------
void TransformTool::connectSignalpropertiesChanged(bool flag)
{
    if (flag==true)
    {
        for (int32 ii=0; ii<_pties.size(); ++ii)
        {
            connect(_pties[ii].get(), SIGNAL(propertyChanged()),
                        this, SLOT(updateViewportTool()));
        }
    }
    else
    {
        for (int32 ii=0; ii<_pties.size(); ++ii)
        {
            disconnect(_pties[ii].get(), SIGNAL(propertyChanged()),
                        this, SLOT(updateViewportTool()));
        }
    }
}
//-----------------------------------------------------------------------------
void TransformTool::updatePropertiesTargets(const Core::List<Ptr<Property> >& ptiesSelected)
{
    connectSignalpropertiesChanged(false);

    _pties.clear();

    for (int32 ii=0; ii<ptiesSelected.size(); ++ii)
    {
        Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPty(ptiesSelected[ii]);

        if (pPtyTransform!=null)
        {
            _pties.push_back(pPtyTransform);
        }
    }

    _pties = removeChildren(_pties);

    connectSignalpropertiesChanged(true);

    updateViewportTool();
}
//-----------------------------------------------------------------------------
void TransformTool::updateViewportTool()
{
    computeActionCenters();
    computeActionOrientations();
    computeLocalScale();
    emit viewportToolChanged();
}
//-----------------------------------------------------------------------------
void TransformTool::computeLocalScale()
{
    _localScale.setZero();

    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        _localScale += LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii])->getLocalScale();
    }

    _localScale /= _pties.size();
}
//-----------------------------------------------------------------------------
void TransformTool::computeActionCenters()
{
    _pivotInWorldAxis.setZero();
    _centerBoundingBoxInWorldAxis.setZero();
    
    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        _pivotInWorldAxis        += LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii])->getWorldPosition();
        _centerBoundingBoxInWorldAxis  += LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii])->getCenterBoundingBoxInWorldAxis();
    }

    _pivotInWorldAxis               /= _pties.size();
    _centerBoundingBoxInWorldAxis   /= _pties.size();
}
//-----------------------------------------------------------------------------
void TransformTool::computeActionOrientations()
{
    if (_pties.size()> 0)
    {
        _localOrientation = Core::Quaternionf(LM_DEBUG_PTR_CAST<PtyTransform>(_pties[0])->getWorldAxis());
        //_localOrientation.normalizeSafe();
        _globalOrientation.setIdentity();
    }
}
//-----------------------------------------------------------------------------
const Core::Vector3f&  TransformTool::getActionCenter() const
{
    switch(_actionCenterType)
    {
        case PIVOT_CENTER:                       return _pivotInWorldAxis;               break;
        case BOUNDING_BOX_CENTER:                return _centerBoundingBoxInWorldAxis;   break;
        case FREE_USER_CENTER:                   return _pivotFreeUserInWorldAxis;       break;
        case FREE_USER_CENTER_ROTATE_BOXCENTER : return _pivotFreeUserInWorldAxis;       break;
    }

    return _pivotInWorldAxis;
}
//-----------------------------------------------------------------------------
void TransformTool::setActionCenter(const Core::Vector3f& newActionPos)
{
    const Core::Vector3f oldActionPos = getActionCenter();

    Core::Vector3f  deltaPos = newActionPos - oldActionPos;

    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii]);
        pPtyT->translateInWorldAxis(deltaPos);
        pPtyT->updateData();
    }

    updateViewportTool();
}
//-----------------------------------------------------------------------------
void TransformTool::moveTo(const EOrientationType& type, const Core::Vector3f& pos)
{
    switch(type)
    {
    case GLOBAL_ORIENTATION:
        setActionCenter(pos);
        break;
    case LOCAL_ORIENTATION:
        {
            Core::Vector3f delta = _localOrientation.apply(pos);
            Core::Vector3f worldPos = getActionCenter() + delta;
            setActionCenter(worldPos);
        }
        break;
    }
}
//-----------------------------------------------------------------------------
void TransformTool::setLocalScale(const Core::Vector3f& newScale)
{
    const Core::Vector3f oldlocalScale = getLocalScale();

    Core::Vector3f  scaleFacor = Core::Vector3f(newScale.x / oldlocalScale.x, newScale.y / oldlocalScale.y, newScale.z / oldlocalScale.z);

    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii]);

        Core::Vector3f oldPtyScale = pPtyT->getLocalScale();
        Core::Vector3f newPtyScale = Core::Vector3f(oldPtyScale.x*scaleFacor.x, oldPtyScale.y*scaleFacor.y, oldPtyScale.z*scaleFacor.z);

        pPtyT->setLocalScale(newPtyScale);

        pPtyT->updateData();
    }

    updateViewportTool();
}
//-----------------------------------------------------------------------------
void TransformTool::rotateInWorldAxis(const Core::Rotationf& rotation, Core::List<Ptr<Property> >& pties, Ptr<Property> pContainer)
{
    for (int32 ii=0; ii<pties.size(); ++ii)
    {
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pties[ii]);
        pPtyT->rotateInWorldAxis(rotation);
        pPtyT->updateData();
    }

    if (pContainer!=null)
    {
        pContainer->updateData();
        pContainer->launchSignalProeprtyChanged();
    }
}
//-----------------------------------------------------------------------------
const Core::Quaternionf&  TransformTool::getActionOrientation() const
{
    switch(_orientationType)
    {
        case LOCAL_ORIENTATION:    return _localOrientation;   break;
        case GLOBAL_ORIENTATION:   return _globalOrientation;  break;
    }

    return _localOrientation;
}
//-----------------------------------------------------------------------------
void TransformTool::setActionOrientation(const Core::Quaternionf& newActionOr)
{/*
    Core::Quaternionf oldActionOr = getActionOrientation();
    oldActionOr.inverseSelf();

    const Core::Quaternionf delta = oldActionOr * newActionOr;
    

    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii]);
        pPtyT->setLocalOrientation(delta * pPtyT->getLocalOrientation());
        pPtyT->updateData();
    }

    if (_pContainer!=null)
    {
        _pContainer->updateData();
        _pContainer->launchSignalProeprtyChanged();
    }

    updateViewportTool();*/

    Core::Quaternionf oldActionOr = getActionOrientation();
    oldActionOr.inverseSelf();

    const Core::Quaternionf delta = oldActionOr * newActionOr;
    

    for (int32 ii=0; ii<_pties.size(); ++ii)
    {
        Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii]);
        pPtyT->setLocalAxis((delta * Core::Quaternionf(pPtyT->getLocalAxis())).toMatrix3());
        pPtyT->updateData();
    }

    if (_pContainer!=null)
    {
        _pContainer->updateData();
        _pContainer->launchSignalProeprtyChanged();
    }

    updateViewportTool();
}
//-----------------------------------------------------------------------------
void TransformTool::mouseMoveEvent (QMouseEvent* event,const DrawContext& dc)
{

}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& TransformTool::getProperties() const
{
    return _pties;
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Property> >& TransformTool::getProperties()
{
    return _pties;
}
//-----------------------------------------------------------------------------
void TransformTool::mouseMiddleSingleClick (QMouseEvent* event, const DrawContext& dc)
{
    if (getActionCenterType() == FREE_USER_CENTER || getActionCenterType() == FREE_USER_CENTER_ROTATE_BOXCENTER)
    {
        Core::Vector3f pivotPtiesInWorldAxis = _centerBoundingBoxInWorldAxis;

        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

        Core::Planef plane (Core::Plane_NormalPoint, (pivotPtiesInWorldAxis-dc.pCamera->getWorldPosition()).getNormalizedSafe(), pivotPtiesInWorldAxis);

        if (plane.normal.isUnit()==true)
        {
            _pivotFreeUserInWorldAxis = plane.project(ray.origin, ray.direction);
            updateViewportTool();
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MoveTool::MoveTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter, const EOrientationType& orientationType) :
    TransformTool       (pStateRecorder, pties, actionsActionCenter, orientationType),
    _elementHighlighted (NOTHING),
    _elementSelected    (NOTHING),
    _isVisible          (true)
{
    internalChangeSelection();
}
//-----------------------------------------------------------------------------
MoveTool::~MoveTool()
{
    if (_pNode != null)
    {
        _pNode->kill();
        _pNode = null;
    }
}
//-----------------------------------------------------------------------------
void MoveTool::createMoveToolModels(Ptr<Universe::RessourcePool>& pPool)
{
    //arrow
    Ptr<Assets::ModelMesh> arrow (new Assets::ModelMesh());
    Assets::VerticeSet & vsetArrow = arrow->getLODByID(0);
    Assets::createArrow(vsetArrow, ARROW_SIZE_FACTOR);
    Assets::makeTBNBasis(vsetArrow, false);
    arrow->cleanup();

    Ptr<Assets::Model> pArrowModel (new Assets::Model());
    pArrowModel->addMesh(ARROW_MESH_NAME, *arrow);
    pPool->addModel(ARROW_MODEL_NAME, pArrowModel);

    //BIAXIS_MODEL_NAME
    Ptr<Assets::ModelMesh> biaxis (new Assets::ModelMesh());
    Assets::VerticeSet & vsetBiaxis = biaxis->getLODByID(0);
    createCylinder(vsetBiaxis, ARROW_SIZE_FACTOR/8.f, ARROW_SIZE_FACTOR/18.f);
   // Assets::makeTBNBasis(vsetBiaxis, false);
    biaxis->cleanup();

    Ptr<Assets::Model> pBiaxisModel (new Assets::Model());
    pBiaxisModel->addMesh(BIAXIS_MESH_NAME, *biaxis);
    pPool->addModel(BIAXIS_MODEL_NAME, pBiaxisModel);
}
//-----------------------------------------------------------------------------
void MoveTool::internalChangeSelection()
{
    if (_pties.size() == 0)
    {
        if (_pNode != null)
        {
            _pNode->setVisible(false, true);
        }
    }
    else
    {
        if (_pNode != null)
        {
            _pNode->setVisible(true, true);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<IViewportToolWidget> MoveTool::createViewportToolWidget(QWidget * parent)
{
    return Ptr<IViewportToolWidget> (new MoveToolWidget(*this, parent));
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> MoveTool::createMoveTool(Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet;

    pRet = pWorld->createGroup();

    //model fleche
    Ptr<Universe::NodeMesh> pNodeArrowAxeX = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeY = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeZ = pWorld->createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);

    pNodeArrowAxeX->addTag(tagArrowAxeX);
    pNodeArrowAxeY->addTag(tagArrowAxeY);
    pNodeArrowAxeZ->addTag(tagArrowAxeZ);

    pNodeArrowAxeX->getMeshInstance()->setMaterial(TransformTool::_sMatAxeX);
    pNodeArrowAxeY->getMeshInstance()->setMaterial(TransformTool::_sMatAxeY);
    pNodeArrowAxeZ->getMeshInstance()->setMaterial(TransformTool::_sMatAxeZ);

    //
    pNodeArrowAxeX->beginMatrixUpdate();
        pNodeArrowAxeX->localRoll(-f_PI_DIV_2);
        pNodeArrowAxeX->setLocalPosition(Core::Vector3f(ARROW_SIZE_FACTOR,0,0));
    pNodeArrowAxeX->endMatrixUpdate();

    pNodeArrowAxeY->beginMatrixUpdate();
        pNodeArrowAxeY->setLocalPosition(Core::Vector3f(0,ARROW_SIZE_FACTOR,0));
    pNodeArrowAxeY->endMatrixUpdate();

    pNodeArrowAxeZ->beginMatrixUpdate();
        pNodeArrowAxeZ->localPitch(f_PI_DIV_2);
        pNodeArrowAxeZ->setLocalPosition(Core::Vector3f(0,0,ARROW_SIZE_FACTOR));
    pNodeArrowAxeZ->endMatrixUpdate();

    pRet->addChild(pNodeArrowAxeX);
    pRet->addChild(pNodeArrowAxeY);
    pRet->addChild(pNodeArrowAxeZ);

    //model biAXIS
    Ptr<Universe::NodeMesh> pNodeBiaxisAxeXY = pWorld->createMesh(BIAXIS_MODEL_NAME, BIAXIS_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeBiaxisAxeXZ = pWorld->createMesh(BIAXIS_MODEL_NAME, BIAXIS_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeBiaxisAxeYZ = pWorld->createMesh(BIAXIS_MODEL_NAME, BIAXIS_MESH_NAME);

    pNodeBiaxisAxeXY->addTag(tagBiaxisAxeXY);
    pNodeBiaxisAxeXZ->addTag(tagBiaxisAxeXZ);
    pNodeBiaxisAxeYZ->addTag(tagBiaxisAxeYZ);

    pNodeBiaxisAxeXY->getMeshInstance()->setMaterial(TransformTool::_sMatAxeXY);
    pNodeBiaxisAxeXZ->getMeshInstance()->setMaterial(TransformTool::_sMatAxeXZ);
    pNodeBiaxisAxeYZ->getMeshInstance()->setMaterial(TransformTool::_sMatAxeYZ);


    //
    pNodeBiaxisAxeXY->beginMatrixUpdate();
        pNodeBiaxisAxeXY->localPitch(-f_PI_DIV_2);
        pNodeBiaxisAxeXY->setLocalPosition(Core::Vector3f(ARROW_SIZE_FACTOR, ARROW_SIZE_FACTOR, 0));
    pNodeBiaxisAxeXY->endMatrixUpdate();

    pNodeBiaxisAxeXZ->beginMatrixUpdate();
        pNodeBiaxisAxeXZ->setLocalPosition(Core::Vector3f(ARROW_SIZE_FACTOR, 0, ARROW_SIZE_FACTOR));
    pNodeBiaxisAxeXZ->endMatrixUpdate();

    pNodeBiaxisAxeYZ->beginMatrixUpdate();
        pNodeBiaxisAxeYZ->localRoll(-f_PI_DIV_2);
        pNodeBiaxisAxeYZ->setLocalPosition(Core::Vector3f(0, ARROW_SIZE_FACTOR, ARROW_SIZE_FACTOR));
    pNodeBiaxisAxeYZ->endMatrixUpdate();

    pRet->addChild(pNodeBiaxisAxeXY);
    pRet->addChild(pNodeBiaxisAxeXZ);
    pRet->addChild(pNodeBiaxisAxeYZ);


    //croix centre
    Ptr<Universe::NodeGroup> pNodeCenter = pWorld->createGroup();
        Ptr<Universe::NodeSpline>   pNodeSplineAxeX = pWorld->createSpline();
        Ptr<Universe::NodeSpline>   pNodeSplineAxeY = pWorld->createSpline();
        Ptr<Universe::NodeSpline>   pNodeSplineAxeZ = pWorld->createSpline();

        pNodeCenter->addTag(tagMoveCenter);
        
        setLine(pNodeSplineAxeX, Core::Vector3f(-.2f,    0,    0), Core::Vector3f(.2f,   0,   0));
        setLine(pNodeSplineAxeY, Core::Vector3f(   0, -.2f,    0), Core::Vector3f(  0, .2f,   0));
        setLine(pNodeSplineAxeZ, Core::Vector3f(   0,    0, -.2f), Core::Vector3f(  0,   0, .2f));
        
        pNodeSplineAxeX->setColor(colorInfo);
        pNodeSplineAxeY->setColor(colorInfo);
        pNodeSplineAxeZ->setColor(colorInfo);

        pNodeCenter->addChild(pNodeSplineAxeX);
        pNodeCenter->addChild(pNodeSplineAxeY);
        pNodeCenter->addChild(pNodeSplineAxeZ);

    pRet->addChild(pNodeCenter);

    return pRet;
}
//-----------------------------------------------------------------------------
void MoveTool::configureMoveTool (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected)
{
    Core::Quaternionf orient = pNodeTool->getLocalOrient();
   

    pNodeTool->beginMatrixUpdate();
        pNodeTool->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
        pNodeTool->setLocalScale(Core::Vector3f (1));
    pNodeTool->endMatrixUpdate();

    float scaleFactor = computeScaleFactor(pNodeTool, dc.pCamera);

    pNodeTool->beginMatrixUpdate();
    pNodeTool->setLocalOrientation(orient);
        pNodeTool->setLocalScale(Core::Vector3f (scaleFactor));
    pNodeTool->endMatrixUpdate();

    configureVisible(pNodeTool, dc, elementSelected);
}
//-----------------------------------------------------------------------------
void MoveTool::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{
    if (_pNode == null) {_pNode = createMoveTool(pWorld);}

    

    //position
    _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(getActionCenter());
    _pNode->endMatrixUpdate();

    //orientation
    _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(getActionOrientation());
    _pNode->endMatrixUpdate();

    //scale & visible
    configureMoveTool (_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void MoveTool::mouseMoveEvent (QMouseEvent* event,const DrawContext& dc)
{
    if (_pNode!=null && _elementSelected == NOTHING)
    {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

            float distance;
            Ptr<Universe::Node> pNodeIntersecting;
        ///
        configureMoveTool (_pNode, dc, _elementSelected);
        ///
        Universe::intersect_RayNodeRecursive(_pNode, ray, distance, pNodeIntersecting);

        if(pNodeIntersecting!=null)
        {
            if      (pNodeIntersecting->hasTag(tagArrowAxeX) == true)   _elementHighlighted = AXE_X;
            else if (pNodeIntersecting->hasTag(tagArrowAxeY) == true)   _elementHighlighted = AXE_Y;
            else if (pNodeIntersecting->hasTag(tagArrowAxeZ) == true)   _elementHighlighted = AXE_Z; 
            else if (pNodeIntersecting->hasTag(tagBiaxisAxeXY) == true) _elementHighlighted = AXE_XY;
            else if (pNodeIntersecting->hasTag(tagBiaxisAxeXZ) == true) _elementHighlighted = AXE_XZ; 
            else if (pNodeIntersecting->hasTag(tagBiaxisAxeYZ) == true) _elementHighlighted = AXE_YZ; 
            else _elementHighlighted = NOTHING;
        }
        else
        {
            _elementHighlighted = NOTHING;
        }

        configureHighlighted();
    }
    else if (_pNode!=null && _elementSelected != NOTHING)
    {
        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
        makeMoveAction(ray, _pNode, _elementSelected, _workPlane, _startPoint, _pties,_pContainer);
//        updateViewportTool();
    }
}
//-----------------------------------------------------------------------------
void MoveTool::mousePressEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (event->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            _elementSelected = NOTHING;
            _elementHighlighted = NOTHING;

            cancelPropertiesChange();
            updateViewportTool();
            connectSignalpropertiesChanged(true);
            //QApplication::restoreOverrideCursor();
            event->accept();
        }
    }
    else
    {
        if (_elementHighlighted != NOTHING && _pties.size() > 0)
        {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

            _elementSelected = _elementHighlighted;
            _elementHighlighted = NOTHING;

            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
            
            initMoveAction(ray, _pNode, _elementSelected, getActionCenter(), _workPlane, _startPoint);

            //QApplication::setOverrideCursor(Qt::BlankCursor);
            connectSignalpropertiesChanged(false);
            beginPropertiesChange();
        }
    }

    configureHighlighted();
    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void MoveTool::mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_elementSelected != NOTHING)
    {
        _elementSelected = NOTHING;
      
        finishPropertiesChange();
        updateViewportTool();
        connectSignalpropertiesChanged(true);
        //QApplication::restoreOverrideCursor();
    }

    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void MoveTool::configureVisible(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected)
{
    if (pNodeTool!=null)
    {
       /* if (_isVisible==false)
        {
            pNodeTool->setVisible(false, true);
        }
        else*/
        {
            bool AxeXisVisible = false;
            bool AxeYisVisible = false;
            bool AxeZisVisible = false;
            bool AxeXYisVisible = true;
            bool AxeXZisVisible = true;
            bool AxeYZisVisible = true;

            if (elementSelected == AXE_X  || elementSelected == NOTHING) AxeXisVisible = true;
            if (elementSelected == AXE_Y  || elementSelected == NOTHING) AxeYisVisible = true;
            if (elementSelected == AXE_Z  || elementSelected == NOTHING) AxeZisVisible = true;

            if (elementSelected == AXE_XY) {AxeXisVisible = true;   AxeYisVisible = true;}
            if (elementSelected == AXE_XZ) {AxeXisVisible = true;   AxeZisVisible = true;}
            if (elementSelected == AXE_YZ) {AxeYisVisible = true;   AxeZisVisible = true;}

            if (elementSelected != NOTHING)
            {
                AxeXYisVisible = false;
                AxeXZisVisible = false;
                AxeYZisVisible = false;
            }

            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeX))->setVisible(AxeXisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeY))->setVisible(AxeYisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeZ))->setVisible(AxeZisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXY))->setVisible(AxeXYisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXZ))->setVisible(AxeXZisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeYZ))->setVisible(AxeYZisVisible);
            hideComponentWithDrawContext(pNodeTool, dc);
        }
    }
}
//-----------------------------------------------------------------------------
void MoveTool::hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc)
{
    bool AxeXisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeX))->isVisible();
    bool AxeYisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeY))->isVisible();
    bool AxeZisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeZ))->isVisible();

    bool BiAxeXYisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXY))->isVisible();
    bool BiAxeXZisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXZ))->isVisible();
    bool BiAxeYZisVisible = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeYZ))->isVisible();
    
    const float angleMin = Core::deg2rad(10);
    const float angleMax = Core::deg2rad(170);

    Ptr<Universe::NodeCamera>   pNodeCam;

    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(dc.pCamera);
    if (pNodeCamEdit!=null)
    {
        pNodeCam = pNodeCamEdit->getActiveViewCamera();
    }
    else
    {
        pNodeCam = dc.pCamera;
    }

    Core::Vector3f camDirInLocalAxis = pNodeTool->getWorldToLocalMatrix().apply(pNodeCam->getWorldPosition()).getNormalizedSafe();
    float angleX = Core::L_acos(Core::dot(Core::Vector3f(1,0,0), camDirInLocalAxis));
    float angleY = Core::L_acos(Core::dot(Core::Vector3f(0,1,0), camDirInLocalAxis));
    float angleZ = Core::L_acos(Core::dot(Core::Vector3f(0,0,1), camDirInLocalAxis));


    if (angleX<angleMin || angleX>angleMax)    AxeXisVisible = false;
    if (angleY<angleMin || angleY>angleMax)    AxeYisVisible = false;
    if (angleZ<angleMin || angleZ>angleMax)    AxeZisVisible = false;

    if (AxeXisVisible==false || AxeYisVisible==false)    BiAxeXYisVisible = false;
    if (AxeXisVisible==false || AxeZisVisible==false)    BiAxeXZisVisible = false;
    if (AxeYisVisible==false || AxeZisVisible==false)    BiAxeYZisVisible = false;

    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeX))->setVisible(AxeXisVisible);
    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeY))->setVisible(AxeYisVisible);
    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagArrowAxeZ))->setVisible(AxeZisVisible);
    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXY))->setVisible(BiAxeXYisVisible);
    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeXZ))->setVisible(BiAxeXZisVisible);
    LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagBiaxisAxeYZ))->setVisible(BiAxeYZisVisible);
}
//-----------------------------------------------------------------------------
void MoveTool::configureHighlighted()
{
    if (_pNode!=null)
    {
        Renderer::Material matAxeX = TransformTool::_sMatAxeX;
        Renderer::Material matAxeY = TransformTool::_sMatAxeY;
        Renderer::Material matAxeZ = TransformTool::_sMatAxeZ;

        if (_elementHighlighted == AXE_X)   matAxeX = TransformTool::_sMatHighlighted;
        if (_elementHighlighted == AXE_Y)   matAxeY = TransformTool::_sMatHighlighted;
        if (_elementHighlighted == AXE_Z)   matAxeZ = TransformTool::_sMatHighlighted;

        if (_elementHighlighted == AXE_XY) {matAxeX = TransformTool::_sMatHighlighted;   matAxeY = TransformTool::_sMatHighlighted;}
        if (_elementHighlighted == AXE_XZ) {matAxeX = TransformTool::_sMatHighlighted;   matAxeZ = TransformTool::_sMatHighlighted;}
        if (_elementHighlighted == AXE_YZ) {matAxeY = TransformTool::_sMatHighlighted;   matAxeZ = TransformTool::_sMatHighlighted;}

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagArrowAxeX))->getMeshInstance()->setMaterial(matAxeX);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagArrowAxeY))->getMeshInstance()->setMaterial(matAxeY);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagArrowAxeZ))->getMeshInstance()->setMaterial(matAxeZ);
    }
}
//-----------------------------------------------------------------------------
void MoveTool::setVisible(bool show)
{
    _isVisible = show;

    if (_pNode!=null)
    {
        _pNode->setVisible(show);
    }
}
//-----------------------------------------------------------------------------
void MoveTool::disableHighlighted()
{
    _elementSelected = NOTHING;
    _elementHighlighted = NOTHING;
    /*configureHighlighted();
    configureVisible();*/
}
//-----------------------------------------------------------------------------
bool MoveTool::isHighlighted() const
{
    if (_pties.size() == 0) return false;
    return _elementHighlighted != NOTHING;
}
//-----------------------------------------------------------------------------
bool MoveTool::isSelected() const
{
    return _elementSelected != NOTHING;
}
//-----------------------------------------------------------------------------
void MoveTool::initMoveAction(const Core::Rayf& ray, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Vector3f& actionCenter, Core::Planef& workPlane, Core::Vector3f& startPoint)
{
    Core::Vector3f normalWorkPlane;

    switch(elementSelected)
    {
    case AXE_X:
        if ( dot(ray.direction, pNode->getLocalMatrix().getZVector()) > dot(ray.direction, pNode->getLocalMatrix().getYVector()) )
            normalWorkPlane = pNode->getLocalMatrix().getZVector();
        else
            normalWorkPlane = pNode->getLocalMatrix().getYVector();
        break;
    case AXE_Y:
        if ( dot(ray.direction, pNode->getLocalMatrix().getZVector()) > dot(ray.direction, pNode->getLocalMatrix().getXVector()) )
            normalWorkPlane = pNode->getLocalMatrix().getZVector();
        else
            normalWorkPlane = pNode->getLocalMatrix().getXVector();
        break;
    case AXE_Z:
        if ( dot(ray.direction, pNode->getLocalMatrix().getYVector()) > dot(ray.direction, pNode->getLocalMatrix().getXVector()) )
            normalWorkPlane = pNode->getLocalMatrix().getYVector();
        else
            normalWorkPlane = pNode->getLocalMatrix().getXVector();
        break;
    case AXE_XY:
        normalWorkPlane = pNode->getLocalMatrix().getZVector();
        break;
    case AXE_XZ:
        normalWorkPlane = pNode->getLocalMatrix().getYVector();
        break;
    case AXE_YZ:
        normalWorkPlane = pNode->getLocalMatrix().getXVector();
        break;

    case NOTHING:
    case AXE_XYZ:
        normalWorkPlane = Core::Vector3f::ZERO; //TODO
        break;
    }

    workPlane = Core::Planef(Core::Plane_NormalPoint, normalWorkPlane, actionCenter);

    bool isIntersecting = false;

    if (workPlane.normal.isUnit() == true)
    {
        isIntersecting = intersect_RayPlane(ray, workPlane, startPoint);
    }

    if (isIntersecting==false)  {startPoint = actionCenter;}
}
//-----------------------------------------------------------------------------
void MoveTool::makeMoveAction(const Core::Rayf& ray, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Planef& workPlane, Core::Vector3f& startPoint, Core::List<Ptr<Property> >& pties, Ptr<Property> pContainer)
{
    Core::Vector3f moveWorldPoint;

    if (workPlane.normal.isUnit())
    {
        bool isIntersecting = intersect_RayPlane(ray, workPlane, moveWorldPoint);

        

        Core::Vector3f delta(Core::Vector3f::ZERO);
        Core::Vector3f deltaWorld(Core::Vector3f::ZERO);

        if (isIntersecting==true)
        {
            switch(elementSelected)
            {

            case AXE_X:
                delta.x = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getXVector().getNormalizedSafe());
                break;
            case AXE_Y:
                delta.y = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getYVector().getNormalizedSafe());
                break;
            case AXE_Z:
                delta.z = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getZVector().getNormalizedSafe());
                break;
            case AXE_XY:
                delta.x = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getXVector().getNormalizedSafe());
                delta.y = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getYVector().getNormalizedSafe());
                break;
            case AXE_XZ:
                delta.x = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getXVector().getNormalizedSafe());
                delta.z = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getZVector().getNormalizedSafe());
                break;
            case AXE_YZ:
                delta.y = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getYVector().getNormalizedSafe());
                delta.z = dot((moveWorldPoint - startPoint), pNode->getLocalMatrix().getZVector().getNormalizedSafe());
                break;
            case NOTHING:;
            case AXE_XYZ:
                //TODO
                break;
            }

            switch(elementSelected)
            {
            case AXE_X:
                deltaWorld = delta.x * pNode->getLocalMatrix().getXVector().getNormalizedSafe();
                break;
            case AXE_Y:
                deltaWorld = delta.y * pNode->getLocalMatrix().getYVector().getNormalizedSafe();
                break;
            case AXE_Z:
                deltaWorld = delta.z * pNode->getLocalMatrix().getZVector().getNormalizedSafe();
                break;
            case AXE_XY:
                deltaWorld = delta.x * pNode->getLocalMatrix().getXVector().getNormalizedSafe() +
                             delta.y * pNode->getLocalMatrix().getYVector().getNormalizedSafe();
                break;
            case AXE_XZ:
                deltaWorld = delta.x * pNode->getLocalMatrix().getXVector().getNormalizedSafe() +
                             delta.z * pNode->getLocalMatrix().getZVector().getNormalizedSafe();
                break;
            case AXE_YZ:
                deltaWorld = delta.y * pNode->getLocalMatrix().getYVector().getNormalizedSafe() +
                             delta.z * pNode->getLocalMatrix().getZVector().getNormalizedSafe();
                break;
            case NOTHING:
            case AXE_XYZ:
                break;
            }
        }

        for (int32 ii=0; ii<pties.size(); ++ii)
        {
            Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pties[ii]);
            pPtyT->translateInWorldAxis(deltaWorld);
            pPtyT->updateData();
        }

        if (pContainer!=null)
        {
            pContainer->updateData();
        }

        startPoint = moveWorldPoint;
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MoveToolWidget::MoveToolWidget(MoveTool& mt, QWidget * parent) :
    IViewportToolWidget(parent),
    _mt(mt)
{
    setupUi();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------
MoveToolWidget::~MoveToolWidget()
{
}
//-----------------------------------------------------------------------------
IViewportTool& MoveToolWidget::getViewportTool()
{
    return _mt;
}
//-----------------------------------------------------------------------------
void MoveToolWidget::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _pActionCenter = createActionCenterComboBox(this, _mt);
    _pOrientation = createOrientationComboBox(this, _mt);

    _pMove = new QtToolbox::NumberEditVector3(this, "Move", " m");

    _layout->addWidget(_pActionCenter, 0, 0, Qt::AlignCenter);
    _layout->addWidget(_pOrientation, 0, 1, Qt::AlignCenter);
    _layout->addWidget(_pMove, 1, 0, 1, 2, Qt::AlignCenter);

    getWidgetsForUndoRedo().push_back(_pMove);

    IViewportToolWidget::setupUi();
}
//-----------------------------------------------------------------------------
void MoveToolWidget::readViewportTool()
{
    _pMove->setValue(_mt.getActionCenter());
}
//-----------------------------------------------------------------------------
void MoveToolWidget::writeViewportTool(QWidget* pWidget)
{
    Core::Vector3f actionPos;
    _pMove->getValue(actionPos);

    _mt.setActionCenter(actionPos);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RotateTool::RotateTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const Ptr<Renderer::IFont>& pFont, const Ptr<Renderer::IHUD>& pHUD, const EActionCenterType& actionsActionCenter, const EOrientationType& orientationType) :
    TransformTool           (pStateRecorder, pties, actionsActionCenter, orientationType),
    _elementHighlighted     (NOTHING),
    _elementSelected        (NOTHING),
    _isVisible              (true),
    _invertAxis             (false),
    _pHUD                   (pHUD)
{
    internalChangeSelection();

    if (_pHUD != null && pFont!=null)
    {
        _pTextInfo = _pHUD->createText(pFont, L"");
        _pTextInfo->setBold(false);
        _pTextInfo->setColor(Core::Vector4f(0.8f, 0.8f, 0.2f, 1.0f));
        _pTextInfo->setPriority(1);
    }
}
//-----------------------------------------------------------------------------
RotateTool::~RotateTool()
{
    if (_pNode != null)
    {
        _pNode->kill();
        _pNode = null;
    }

    if (_pNodeAngle != null)
    {
        _pNodeAngle->kill();
        _pNodeAngle = null;
    }
    
}
//-----------------------------------------------------------------------------
Ptr<Renderer::IHUD> RotateTool::getHUD() const
{
    return _pHUD;
}
//-----------------------------------------------------------------------------
void RotateTool::createRotateToolModels(Ptr<Universe::RessourcePool>& pPool)
{
    Ptr<Assets::ModelMesh> rotateTorus (new Assets::ModelMesh());
    Assets::VerticeSet & vsetRotateTorus = rotateTorus->getLODByID(0);
    Assets::createTorus(vsetRotateTorus, TORUS_MINOR_RAIUS_FACTOR, TORUS_MAJOR_RAIUS_FACTOR, 0.5f);
    Assets::makeTBNBasis(vsetRotateTorus, false);
    rotateTorus->cleanup();

    Ptr<Assets::ModelMesh> rotateTorus360 (new Assets::ModelMesh());
    Assets::VerticeSet & vsetRotateTorus360 = rotateTorus360->getLODByID(0);
    Assets::createTorus(vsetRotateTorus360, TORUS_MINOR_RAIUS_FACTOR/2.f, TORUS_MAJOR_RAIUS_FACTOR, 1.0f);
    Assets::makeTBNBasis(vsetRotateTorus360, false);
    rotateTorus360->cleanup();

    Ptr<Assets::Model> pRotateTorusModel (new Assets::Model());
    pRotateTorusModel->addMesh(ROTATE_TORUS_MESH_NAME, *rotateTorus);
    Ptr<Assets::Model> pRotateTorus360Model (new Assets::Model());
    pRotateTorus360Model->addMesh(ROTATE_TORUS_360_MESH_NAME, *rotateTorus360);

    pPool->addModel(ROTATE_TORUS_MODEL_NAME, pRotateTorusModel);
    pPool->addModel(ROTATE_TORUS_360_MODEL_NAME, pRotateTorus360Model);
}
//-----------------------------------------------------------------------------
void RotateTool::internalChangeSelection()
{
    if (_pties.size() == 0)
    {
        if (_pNode != null)
        {
            _pNode->setVisible(false, true);
        }
    }
    else
    {
        if (_pNode != null)
        {
            _pNode->setVisible(true, true);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<IViewportToolWidget> RotateTool::createViewportToolWidget(QWidget* parent)
{
    return Ptr<IViewportToolWidget> (new RotateToolWidget(*this, parent));
}
//-----------------------------------------------------------------------------
void RotateTool::createRotateTool(Ptr<Universe::World>& pWorld, Ptr<Universe::NodeGroup>& pNodeTool, Ptr<Universe::NodeGroup>& pNodeAngle)
{
    pNodeTool = pWorld->createGroup();
    pNodeAngle = pWorld->createGroup();

    //model torus
    Ptr<Universe::NodeMesh> pNodeTorusAxeX = pWorld->createMesh(ROTATE_TORUS_MODEL_NAME, ROTATE_TORUS_MESH_NAME);
    pNodeTorusAxeX->addTag(tagTorusAxeX);
    pNodeTorusAxeX->getMeshInstance()->setMaterial(TransformTool::_sMatAxeX);

    Ptr<Universe::NodeMesh> pNodeTorusAxeY = pWorld->createMesh(ROTATE_TORUS_MODEL_NAME, ROTATE_TORUS_MESH_NAME);
    pNodeTorusAxeY->addTag(tagTorusAxeY);
    pNodeTorusAxeY->getMeshInstance()->setMaterial(TransformTool::_sMatAxeY);

    Ptr<Universe::NodeMesh> pNodeTorusAxeZ = pWorld->createMesh(ROTATE_TORUS_MODEL_NAME, ROTATE_TORUS_MESH_NAME);
    pNodeTorusAxeZ->addTag(tagTorusAxeZ);
    pNodeTorusAxeZ->getMeshInstance()->setMaterial(TransformTool::_sMatAxeZ);

    Ptr<Universe::NodeMesh> pNodeTorusAxeCamera = pWorld->createMesh(ROTATE_TORUS_360_MODEL_NAME, ROTATE_TORUS_360_MESH_NAME);
    pNodeTorusAxeCamera->addTag(tagTorusAxeCamera);
    pNodeTorusAxeCamera->getMeshInstance()->setMaterial(TransformTool::_sMatBlack);

    
    //spline
    Ptr<Universe::NodeSpline>   pNodeSplineStart = pWorld->createSpline();
        pNodeSplineStart->addTag(tagSplineRotStart);
    Ptr<Universe::NodeSpline>   pNodeSplineEnd = pWorld->createSpline();
        pNodeSplineEnd->addTag(tagSplineRotEnd);

    pNodeTool->addChild(pNodeTorusAxeX); 
    pNodeTool->addChild(pNodeTorusAxeY); 
    pNodeTool->addChild(pNodeTorusAxeZ); 
    pNodeTool->addChild(pNodeTorusAxeCamera);
    pNodeAngle->addChild(pNodeSplineStart); 
    pNodeAngle->addChild(pNodeSplineEnd);

    pNodeTorusAxeY->beginMatrixUpdate();
    pNodeTorusAxeY->localPitch(-f_PI_DIV_2);
    pNodeTorusAxeY->endMatrixUpdate();

    pNodeTorusAxeX->beginMatrixUpdate();
    pNodeTorusAxeX->localYaw(-f_PI_DIV_2);
    pNodeTorusAxeX->endMatrixUpdate();
}
//-----------------------------------------------------------------------------

Core::Matrix3f RotAngonst (Core::Vector3f from, Core::Vector3f to)
{
    from.normalizeSafe();
    to.normalizeSafe();

    Core::Vector3f vs = Core::cross(from, to); // axis multiplied by sin

    Core::Vector3f v(vs);
    v.normalizeSafe(); // axis of rotation
    float ca = Core::dot(from, to) ; // cos angle

    Core::Vector3f vt(v*(1.0f - ca));

    Core::Matrix3f rotM;
    rotM._00 = vt.x * v.x + ca;
    rotM._11 = vt.y * v.y + ca;
    rotM._22 = vt.z * v.z + ca;

    vt.x *= v.y;
    vt.z *= v.x;
    vt.y *= v.z;

    rotM._01 = vt.x - vs.z;
    rotM._02 = vt.z + vs.y;
    rotM._10 = vt.x + vs.z;
    rotM._12 = vt.y - vs.x;
    rotM._20 = vt.z - vs.y;
    rotM._21 = vt.y + vs.x;
    return rotM;
}


void RotateTool::configureRotateTool (Ptr<Universe::NodeGroup>& pNodeTool, Ptr<Universe::NodeGroup>& pNodeAngle, const DrawContext& dc, const EMoveToolElement elementSelected, const Core::Vector3f& splineFirstPoint, const Core::Vector3f& splineLastPoint, bool interactionPositionCursor)
{
    Core::Quaternionf orient = pNodeTool->getLocalOrient();

    Ptr<Universe::Node> pNodeTorusX      = pNodeTool->getChildWithTag(tagTorusAxeX);
    Ptr<Universe::Node> pNodeTorusY      = pNodeTool->getChildWithTag(tagTorusAxeY);
    Ptr<Universe::Node> pNodeTorusZ      = pNodeTool->getChildWithTag(tagTorusAxeZ);
    Ptr<Universe::Node> pNodeTorusCamera = pNodeTool->getChildWithTag(tagTorusAxeCamera);
    
    //////
    Ptr<Universe::NodeCamera>   pNodeCam;
    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(dc.pCamera);
    if (pNodeCamEdit!=null)
    {
        pNodeCam = pNodeCamEdit->getActiveViewCamera();
    }
    else
    {
        pNodeCam = dc.pCamera;
    }
    //////

    pNodeTorusX->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
    pNodeTorusY->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
    pNodeTorusZ->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
    pNodeTorusCamera->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));





    pNodeTool->beginMatrixUpdate();
        pNodeTool->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
        pNodeTool->setLocalScale(Core::Vector3f (1));
    pNodeTool->endMatrixUpdate();

    float scaleFactor = computeScaleFactor(pNodeTool, dc.pCamera);

    pNodeTool->beginMatrixUpdate();
        pNodeTool->setLocalOrientation(orient);
        pNodeTool->setLocalScale(Core::Vector3f (scaleFactor));
    pNodeTool->endMatrixUpdate();


    //on aligne les demi torus avec la camera
    
    const Core::Vector3f planXNormalInLocalAxis = Core::Vector3f(1,0,0);
    const Core::Vector3f posCamInLocalAxis            = pNodeTool->getWorldToLocalMatrix().apply(pNodeCam->getWorldPosition());
    Core::Planef planTorusX = Core::Planef(Core::Plane_NormalPoint, planXNormalInLocalAxis, Core::Vector3f(Core::ZERO));
    if (planTorusX.normal.isUnit() == true && pNodeCamEdit==null)
    {
        Core::Vector3f projectPosCamInPlaneAxeX = planTorusX.project(posCamInLocalAxis);
        Core::Vector3f projectDirX = projectPosCamInPlaneAxeX.getNormalizedSafe();

        float angle = Core::L_acos( Core::dot(Core::Vector3f(0,1,0), projectDirX) ); 
        Core::Vector3f crossAngle = Core::cross(Core::Vector3f(0,1,0), projectDirX);
        if (crossAngle.x <0) angle*=-1;

        pNodeTorusX->setLocalOrientation(Core::Quaternionf(Core::PITCH, angle + f_PI) * Core::Quaternionf(Core::YAW, -f_PI_DIV_2));
    }
    else
    {
        pNodeTorusX->setLocalOrientation(Core::Quaternionf(Core::YAW, -f_PI_DIV_2));
    }
    
    
    const Core::Vector3f planYNormalInLocalAxis       = Core::Vector3f(0,1,0);

    Core::Planef planTorusY = Core::Planef(Core::Plane_NormalPoint, planYNormalInLocalAxis, Core::Vector3f(Core::ZERO));
    if (planTorusY.normal.isUnit() == true && pNodeCamEdit==null)
    {
        Core::Vector3f projectPosCamInPlaneAxeY = planTorusY.project(posCamInLocalAxis);
        Core::Vector3f projectDirY = projectPosCamInPlaneAxeY.getNormalizedSafe();

        float angle = Core::L_acos( Core::dot(Core::Vector3f(1,0,0), projectDirY) ); 
        Core::Vector3f crossAngle = Core::cross(Core::Vector3f(1,0,0), projectDirY);
        if (crossAngle.y <0) angle*=-1;

        pNodeTorusY->setLocalOrientation(Core::Quaternionf(Core::YAW, angle+f_PI_DIV_2) * Core::Quaternionf(Core::PITCH, -f_PI_DIV_2));
    }
    else
    {
        pNodeTorusY->setLocalOrientation(Core::Quaternionf(Core::PITCH, -f_PI_DIV_2));
    }



    const Core::Vector3f planZNormalInLocalAxis       = Core::Vector3f(0,0,1);

    Core::Planef planTorusZ = Core::Planef(Core::Plane_NormalPoint, planZNormalInLocalAxis, Core::Vector3f(Core::ZERO));

    if (planTorusZ.normal.isUnit() == true && pNodeCamEdit==null)
    {
        Core::Vector3f projectPosCamInPlaneAxeZ = planTorusZ.project(posCamInLocalAxis);
        Core::Vector3f projectDirZ              = (projectPosCamInPlaneAxeZ).getNormalizedSafe();

        float angle = Core::L_acos( Core::dot(Core::Vector3f(0,1,0), projectDirZ) ); 
        Core::Vector3f crossAngle = Core::cross(Core::Vector3f(0,1,0), projectDirZ);
        if (crossAngle.z <0) angle*=-1;

        pNodeTorusZ->setLocalOrientation(Core::Quaternionf(Core::ROLL, angle-f_PI));
    }


    pNodeTorusCamera->setLocalDirection(posCamInLocalAxis.getNormalizedSafe());

    //spline
    Ptr<Universe::NodeSpline> pNodeSplineStart = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(pNodeAngle->getChildWithTag(tagSplineRotStart));
    Ptr<Universe::NodeSpline> pNodeSplineEnd   = LM_DEBUG_PTR_CAST<Universe::NodeSpline>(pNodeAngle->getChildWithTag(tagSplineRotEnd));
    if (elementSelected != NOTHING)
    {   
        if (interactionPositionCursor==false)
        {
            pNodeSplineStart->setVisible(true);
            pNodeSplineEnd->setVisible(true);
        }
        else
        {
            pNodeSplineStart->setVisible(false);
            pNodeSplineEnd->setVisible(false);
        }

        Core::Vector3f radius = Core::Vector3f(TORUS_MAJOR_RAIUS_FACTOR - TORUS_MINOR_RAIUS_FACTOR) * scaleFactor;

        


        float fct = (TORUS_MAJOR_RAIUS_FACTOR - TORUS_MINOR_RAIUS_FACTOR) * scaleFactor;
     
        Core::Vector3f ptFirstEnd = pNodeAngle->getWorldToLocalMatrix().apply(splineFirstPoint).getNormalizedSafe() * fct;
            setLine(pNodeSplineStart, Core::Vector3f (Core::ZERO), ptFirstEnd);
            pNodeSplineStart->setColor(colorInfo);

/*

        Core::Vector3f dirStart = -pNodeTool->getWorldToLocalMatrix().apply(splineFirstPoint).getNormalizedSafe();
        Core::Vector3f ptFirstEnd (dirStart.x*radius.x, dirStart.y*radius.y, dirStart.z*radius.z);
        setLine(pNodeSplineStart, Core::Vector3f (Core::ZERO), ptFirstEnd);
        pNodeSplineStart->setColor(colorInfo);
*/

        
        
     /*   Core::Vector3f ptFirstEnd = (splineFirstPoint.getNormalizedSafe()) * fct;
        setLine(pNodeSplineStart, Core::Vector3f (Core::ZERO), ptFirstEnd);
        pNodeSplineStart->setColor(colorInfo);*/
/*
        Core::Vector3f dirEnd = -pNodeTool->getWorldToLocalMatrix().apply(splineLastPoint).getNormalizedSafe();
        Core::Vector3f ptLastEnd (dirEnd.x*radius.x, dirEnd.y*radius.y, dirEnd.z*radius.z);
        setLine(pNodeSplineEnd, Core::Vector3f (Core::ZERO), ptLastEnd);
        pNodeSplineEnd->setColor(colorInfo);*/

        Core::Vector3f ptLastEnd = pNodeAngle->getWorldToLocalMatrix().apply(splineLastPoint).getNormalizedSafe() * fct;
        setLine(pNodeSplineEnd, Core::Vector3f (Core::ZERO), ptLastEnd);
        pNodeSplineEnd->setColor(colorInfo);
    }
    else
    {
        pNodeSplineStart->setVisible(false);
        pNodeSplineEnd->setVisible(false);
    }

 
    configureVisible(pNodeTool, dc, elementSelected);
}
//-----------------------------------------------------------------------------
void RotateTool::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{
    if (_pNode == null) {createRotateTool(pWorld,_pNode , _pNodeAngle);}



    //position
    _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(getActionCenter());
    _pNode->endMatrixUpdate();

    _pNodeAngle->beginMatrixUpdate();
        _pNodeAngle->setWorldPosition(getActionCenter());
    _pNodeAngle->endMatrixUpdate();

    //orientation
    _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(getActionOrientation());
    _pNode->endMatrixUpdate();

    _pNodeAngle->beginMatrixUpdate();
        _pNodeAngle->setLocalOrientation(getActionOrientation());
    _pNodeAngle->endMatrixUpdate();

    //scale & visible
    configureRotateTool (_pNode, _pNodeAngle, dc, _elementSelected, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis, _interactionPositionCursor);


}
//-----------------------------------------------------------------------------
void RotateTool::mouseMoveEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_pNode!=null && _elementSelected == NOTHING)
    {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

            float distance;
            Ptr<Universe::Node> pNodeIntersecting;
        ///
        configureRotateTool (_pNode, _pNodeAngle, dc, _elementSelected, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis, _interactionPositionCursor);
        ///
        Universe::intersect_RayNodeRecursive(_pNode, ray, distance, pNodeIntersecting);

        if(pNodeIntersecting!=null)
        {
            if      (pNodeIntersecting->hasTag(tagTorusAxeX) == true)   _elementHighlighted = AXE_X;
            else if (pNodeIntersecting->hasTag(tagTorusAxeY) == true)   _elementHighlighted = AXE_Y;
            else if (pNodeIntersecting->hasTag(tagTorusAxeZ) == true)   _elementHighlighted = AXE_Z; 
            else _elementHighlighted = NOTHING;
        }
        else
        {
            _elementHighlighted = NOTHING;
        }

        configureHighlighted();
    }
    else if (_pNode!=null && _elementSelected != NOTHING)
    {
        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

        Core::Vector3f center = getActionCenter();
        if (getActionCenterType() == FREE_USER_CENTER_ROTATE_BOXCENTER)
        {
            center = _centerBoundingBoxInWorldAxis;
        }

        makeRotateAction(ray, Core::Vector2f (event->posF().x(), event->posF().y()), dc, _interactionPositionCursor,_workPlane, _invertAxis, _elementSelected, _pNode, center, _startPoint,_splineLastPointInWorldAxis,_lastCursorPos, _pties, _pContainer);
//        updateViewportTool();
    }
}





//-----------------------------------------------------------------------------
void RotateTool::mousePressEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (event->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            _elementSelected = NOTHING;
            _elementHighlighted = NOTHING;

            cancelPropertiesChange();
            updateViewportTool();
            connectSignalpropertiesChanged(true);
            //QApplication::restoreOverrideCursor();
            event->accept();
        }
    }
    else
    {
        if (_elementHighlighted != NOTHING && _pties.size() > 0)
        {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

            _elementSelected = _elementHighlighted;
            _elementHighlighted = NOTHING;

            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
            
            initRotateAction(ray, dc, Core::Vector2f (event->posF().x(), event->posF().y()), _pNode, _elementSelected, getActionCenter(),
                _interactionPositionCursor, _lastCursorPos, _invertAxis, _workPlane, _startPoint, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis);

            //QApplication::setOverrideCursor(Qt::BlankCursor);


            connectSignalpropertiesChanged(false);
            beginPropertiesChange();
        }
    }

    configureHighlighted();
    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void RotateTool::mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_elementSelected != NOTHING)
    {
        _elementSelected = NOTHING;
      
        finishPropertiesChange();
        updateViewportTool();
        connectSignalpropertiesChanged(true);
        //QApplication::restoreOverrideCursor();
    }

    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void RotateTool::setVisible(bool show)
{
    _isVisible = show;

    if (_pNode!=null)
    {
        _pNode->setVisible(show);
    }
}
//-----------------------------------------------------------------------------
void RotateTool::configureVisible(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected)
{
    if (pNodeTool!=null)
    {
        bool AxeXisVisible = false;
        bool AxeYisVisible = false;
        bool AxeZisVisible = false;
        bool AxeCameraisVisible = false;

        if (elementSelected == AXE_X || elementSelected == NOTHING) AxeXisVisible = true;
        if (elementSelected == AXE_Y || elementSelected == NOTHING) AxeYisVisible = true;
        if (elementSelected == AXE_Z || elementSelected == NOTHING) AxeZisVisible = true;
        if (elementSelected == NOTHING)                             AxeCameraisVisible = true;

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagTorusAxeX))->setVisible(AxeXisVisible);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagTorusAxeY))->setVisible(AxeYisVisible);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagTorusAxeZ))->setVisible(AxeZisVisible);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagTorusAxeCamera))->setVisible(AxeCameraisVisible);
        hideComponentWithDrawContext(pNodeTool, dc);
    }
}
//-----------------------------------------------------------------------------
void RotateTool::hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc)
{
}
//-----------------------------------------------------------------------------
void RotateTool::configureHighlighted()
{
    if (_pNode!=null)
    {
        Renderer::Material matAxeX = TransformTool::_sMatAxeX;
        Renderer::Material matAxeY = TransformTool::_sMatAxeY;
        Renderer::Material matAxeZ = TransformTool::_sMatAxeZ;

        if (_elementHighlighted == AXE_X)   matAxeX = TransformTool::_sMatHighlighted;
        if (_elementHighlighted == AXE_Y)   matAxeY = TransformTool::_sMatHighlighted;
        if (_elementHighlighted == AXE_Z)   matAxeZ = TransformTool::_sMatHighlighted;

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagTorusAxeX))->getMeshInstance()->setMaterial(matAxeX);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagTorusAxeY))->getMeshInstance()->setMaterial(matAxeY);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagTorusAxeZ))->getMeshInstance()->setMaterial(matAxeZ);
    }
}
//-----------------------------------------------------------------------------
void RotateTool::disableHighlighted()
{
    _elementSelected = NOTHING;
    _elementHighlighted = NOTHING;
  /*  configureHighlighted();
    configureVisible();*/
}
//-----------------------------------------------------------------------------
bool RotateTool::isHighlighted() const
{
    if (_pties.size() == 0) return false;
    return _elementHighlighted != NOTHING;
}
//-----------------------------------------------------------------------------
bool RotateTool::isSelected() const
{
    return _elementSelected != NOTHING;
}
//-----------------------------------------------------------------------------
void RotateTool::initRotateAction(const Core::Rayf& ray, const DrawContext& dc, const Core::Vector2f& cursorPos, const Ptr<Universe::Node>& pNode, const EMoveToolElement& elementSelected, const Core::Vector3f& actionCenter,
                                  bool & interactionPositionCursor, Core::Vector2f& lastCursorPos, bool& invertAxis, Core::Planef& workPlane, Core::Vector3f& startPoint, Core::Vector3f& splineFirstPointInWorldAxis, Core::Vector3f& splineLastPointInWorldAxis)
{
    Ptr<Universe::NodeCamera>   pNodeCam;

    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(dc.pCamera);
    if (pNodeCamEdit!=null)
    {
        pNodeCam = pNodeCamEdit->getActiveViewCamera();
    }
    else
    {
        pNodeCam = dc.pCamera;
    }

    Core::Vector3f camDirInLocalAxis = pNode->getWorldToLocalMatrix().apply(pNodeCam->getWorldPosition()).getNormalizedSafe();
    float angleX = Core::L_acos(Core::dot(Core::Vector3f(1,0,0), camDirInLocalAxis));
    float angleY = Core::L_acos(Core::dot(Core::Vector3f(0,1,0), camDirInLocalAxis));
    float angleZ = Core::L_acos(Core::dot(Core::Vector3f(0,0,1), camDirInLocalAxis));
    
    interactionPositionCursor = false;
    const float deltaAngle = Core::deg2rad(10);


    switch(elementSelected)
    {
        case AXE_X:
            if ((angleX>f_PI_DIV_2-deltaAngle && angleX<f_PI_DIV_2+deltaAngle) ||
                (angleX>f_PI+f_PI_DIV_2-deltaAngle && angleX<f_PI+f_PI_DIV_2+deltaAngle))    
                interactionPositionCursor = true;
            break;
        case AXE_Y:
            if ((angleY>f_PI_DIV_2-deltaAngle && angleY<f_PI_DIV_2+deltaAngle) ||
                (angleY>f_PI+f_PI_DIV_2-deltaAngle && angleY<f_PI+f_PI_DIV_2+deltaAngle))    
                interactionPositionCursor = true;
            break;
        case AXE_Z:
            if ((angleZ>f_PI_DIV_2-deltaAngle && angleZ<f_PI_DIV_2+deltaAngle) ||
                (angleZ>f_PI+f_PI_DIV_2-deltaAngle && angleZ<f_PI+f_PI_DIV_2+deltaAngle))    
                interactionPositionCursor = true;
            break;
    }

    

    if (interactionPositionCursor==true)
    {
        lastCursorPos = cursorPos;
        
        Core::Vector3f camDirInLocalAxis = pNodeCam->getWorldDirection();//pNodeCam->getWorldToLocalMatrix().apply(pNode->getWorldPosition()).getNormalizedSafe();//pNode->getWorldToLocalMatrix().apply(pNodeCam->getWorldPosition()).getNormalizedSafe();
            camDirInLocalAxis.normalizeSafe();
          

        float dot;

        invertAxis = true;

        Core::Vector3f nodeX = pNode->getLocalToWorldMatrix().getXVector().getNormalizedSafe();
        Core::Vector3f nodeY = pNode->getLocalToWorldMatrix().getYVector().getNormalizedSafe();
        Core::Vector3f nodeZ = pNode->getLocalToWorldMatrix().getZVector().getNormalizedSafe();

        Core::Vector3f camX = pNodeCam->getLocalToWorldMatrix().getXVector().getNormalizedSafe();
        Core::Vector3f camY = pNodeCam->getLocalToWorldMatrix().getYVector().getNormalizedSafe();
        Core::Vector3f camZ = pNodeCam->getLocalToWorldMatrix().getZVector().getNormalizedSafe();

        switch(elementSelected)
        {
        case AXE_X:
            dot = Core::dot(camX, nodeX);
            if (dot<0) invertAxis = false;
            break;
        case AXE_Y:
            dot = Core::dot(camY, nodeY);
            if (dot<0) invertAxis = false;
            break;
        case AXE_Z:
            dot = Core::dot(camZ, nodeZ);
             if (dot>0) invertAxis = false;
            break;
        }

       


    }
    else
    {
        Core::Vector3f normalWorkPlane;

        switch(elementSelected)
        {
        case AXE_X:
            normalWorkPlane = pNode->getLocalToWorldMatrix().getXVector().getNormalizedSafe();
                break;
        case AXE_Y:
            normalWorkPlane = pNode->getLocalToWorldMatrix().getYVector().getNormalizedSafe();
            break;
        case AXE_Z:
            normalWorkPlane = pNode->getLocalToWorldMatrix().getZVector().getNormalizedSafe();
            break;
        case NOTHING:
        case AXE_XY:
        case AXE_XZ:
        case AXE_YZ:
        case AXE_XYZ:
            normalWorkPlane = Core::Vector3f::ZERO; //TODO
            break;
        }

        workPlane = Core::Planef(Core::Plane_NormalPoint, normalWorkPlane, actionCenter);
        bool isIntersecting = intersect_RayPlane(ray, workPlane, startPoint);

        LM_ASSERT(isIntersecting==true);
        splineFirstPointInWorldAxis = startPoint;
        splineLastPointInWorldAxis =  splineFirstPointInWorldAxis;


        
    }
}
//-----------------------------------------------------------------------------
void RotateTool::makeRotateAction(const Core::Rayf& ray, const Core::Vector2f& cursorPos, const DrawContext& dc, const bool& interactionPositionCursor, const Core::Planef& workPlane,
                                  const bool& invertAxis, const EMoveToolElement& elementSelected, const Ptr<Universe::Node> pNode, const Core::Vector3f& actionCenter, Core::Vector3f& startPoint, Core::Vector3f& splineLastPointInWorldAxis, Core::Vector2f& lastCursorPos, Core::List<Ptr<Property> >& pties, Ptr<Property> pContainer)
{
    Ptr<Universe::NodeCamera>   pNodeCam;

    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(dc.pCamera);
    if (pNodeCamEdit!=null)
    {
        pNodeCam = pNodeCamEdit->getActiveViewCamera();
    }
    else
    {
        pNodeCam = dc.pCamera;
    }



    if (interactionPositionCursor==false)
    {
        Core::Vector3f rotateWorldPoint;
        bool isIntersecting = intersect_RayPlane(ray, workPlane, rotateWorldPoint);

        const Core::Matrix4f& matCamWtoL = pNodeCam->getWorldToLocalMatrix();
        Core::Matrix4f matToolWtoL = pNode->getWorldToLocalMatrix();

        Core::Vector3f centerInLocalCam  = matToolWtoL.apply(pNode->getWorldPosition());
        Core::Vector3f startInLocalCam   = matToolWtoL.apply(startPoint);
        Core::Vector3f endInLocalCam     = matToolWtoL.apply(rotateWorldPoint);

        Core::Vector3f      origineStart    = (startInLocalCam- centerInLocalCam).getNormalizedSafe();
        Core::Vector3f      origineEnd      = (endInLocalCam - centerInLocalCam).getNormalizedSafe();
        
        float angle;
        Core::Vector3f axisForRotationIWorldAxis;
        float angle2;
        if (isIntersecting==true)
        {
            angle = Core::L_acos(Core::dot(origineStart, origineEnd));
            Core::Vector3f crossAngle = Core::cross(origineStart, origineEnd).getNormalizedSafe();

            switch(elementSelected)
            {
            case AXE_X:
                axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getXVector();
                if (crossAngle.x > 0)   angle2 = angle;
                else                    angle2 = -angle;
                break;
            case AXE_Y:
                axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getYVector();
                if (crossAngle.y > 0)   angle2 = angle;
                else                    angle2 = -angle;
                break;
            case AXE_Z:
                axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getZVector();
                if (crossAngle.z > 0)   angle2 = angle;
                else                    angle2 = -angle;
                break;
            case NOTHING:
            case AXE_XY:
            case AXE_XZ:
            case AXE_YZ:
            case AXE_XYZ:
                //TODO
                break;
            }
        }


        if (Core::L_abs(angle2)>0.001)
        {
            Core::Rotationf    rotation (actionCenter, axisForRotationIWorldAxis.getNormalizedSafe(), angle2);
            rotateInWorldAxis(rotation, pties, pContainer);

            startPoint = rotateWorldPoint /*- getActionCenter()*/;
            
            splineLastPointInWorldAxis =  startPoint;
            
        }
/*
        float angleText = Core::L_acos(Core::dot(_splineFirstPoint.getNormalizedSafe(), _splineLastPoint.getNormalizedSafe()));
            Core::Vector3f crossAngle = Core::cross(_splineFirstPoint.getNormalizedSafe(), _splineLastPoint.getNormalizedSafe());

        switch(_elementSelected)
        {
        case AXE_X:
            if (crossAngle.x > 0)   angleText = angleText;
            else                    angleText = -angleText;
            break;
        case AXE_Y:
            if (crossAngle.y > 0)   angleText = angleText;
            else                    angleText = -angleText;
            break;
        case AXE_Z:
            if (crossAngle.z > 0)   angleText = angleText;
            else                    angleText = -angleText;
            break;
        case NOTHING:
        case AXE_XY:
        case AXE_XZ:
        case AXE_YZ:
        case AXE_XYZ:
            //TODO
            break;
        }

        while (angleText >= f_PI_MUL_2)    angleText -= f_PI_MUL_2;
        while (angleText < 0)             angleText += f_PI_MUL_2;

        _pTextInfo->setText(Core::toString(Core::rad2deg(angleText), 2));*/
    }
    else
    {
        Core::Vector3f axisForRotationIWorldAxis;

        Core::Vector2f offset;
        offset.x = cursorPos.x - lastCursorPos.x;
        offset.y = lastCursorPos.y - cursorPos.y;
        
        float fctRotate = Core::max(Core::L_abs(offset.x), Core::L_abs(offset.y));

        if (fctRotate == Core::L_abs(offset.x))     fctRotate = offset.x;
        else                                        fctRotate = offset.y;


        float angle = Core::deg2rad(0.5) * fctRotate;
        
        

        switch(elementSelected)
        {
        case AXE_X:
            axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getXVector().getNormalizedSafe();
            break;
        case AXE_Y:
            axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getYVector().getNormalizedSafe();
            break;
        case AXE_Z:
            axisForRotationIWorldAxis = pNode->getLocalToWorldMatrix().getZVector().getNormalizedSafe();
            break;
        }
        if (invertAxis==true) axisForRotationIWorldAxis *= -1;
        
        Core::Rotationf    rotation (actionCenter, axisForRotationIWorldAxis.getNormalizedSafe(), angle);
        rotateInWorldAxis(rotation, pties, pContainer);
 
        lastCursorPos = cursorPos;
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RotateToolWidget::RotateToolWidget(RotateTool& rt, QWidget * parent):
    _rt(rt),
    IViewportToolWidget(parent)
{
    setupUI();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------
RotateToolWidget::~RotateToolWidget()
{
}
//-----------------------------------------------------------------------------
IViewportTool& RotateToolWidget::getViewportTool()
{
    return _rt;
}
//-----------------------------------------------------------------------------
void RotateToolWidget::setupUI()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    //_groupBox   = new QtToolbox::SmartGroupBox(this, "Rotate");

    _pActionCenter = createActionCenterComboBox(this, _rt);
    _pOrientation = createOrientationComboBox(this, _rt);

  //  _pValue = new QtToolbox::NumberEditVector3(this, "Rotation", " �");



    _layout->addWidget(_pActionCenter, 0, 0, Qt::AlignCenter);
    _layout->addWidget(_pOrientation, 0, 1, Qt::AlignCenter);
  //  _layout->addWidget(_pValue, 1, 0, 1, 2, Qt::AlignCenter);

   // getWidgetsForUndoRedo().push_back(_pValue);

    IViewportToolWidget::setupUi();
}
//-----------------------------------------------------------------------------
void RotateToolWidget::readViewportTool()
{/*
    Core::Quaternionf q = _rt.getActionOrientation();
    Core::Orientationf  orient (q);
    _pValue->setValue(Core::Vector3f(Core::rad2deg(orient._pitch), Core::rad2deg(orient._yaw), Core::rad2deg(orient._roll)));*/
}
//-----------------------------------------------------------------------------
void RotateToolWidget::writeViewportTool(QWidget* pWidget)
{/*
    Core::Vector3f  val;
    _pValue->getValue(val);
    Core::Orientationf orient (Core::deg2rad(val.x), Core::deg2rad(val.y), Core::deg2rad(val.z));

    Core::Quaternionf q (orient);
    _rt.setActionOrientation(q);*/


}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ScaleTool::ScaleTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties) :
    TransformTool       (pStateRecorder, pties),
    _elementHighlighted (NOTHING),
    _elementSelected    (NOTHING),
    _isVisible          (true)
{
    internalChangeSelection();
}
//-----------------------------------------------------------------------------
ScaleTool::~ScaleTool()
{
    if (_pNode != null)
    {
        _pNode->kill();
        _pNode = null;
    }
}
//-----------------------------------------------------------------------------
void ScaleTool::createScaleToolModels(Ptr<Universe::RessourcePool>& pPool)
{
    Ptr<Assets::ModelMesh> scaleUniform (new Assets::ModelMesh());
    Assets::VerticeSet & vsetScaleUniform = scaleUniform->getLODByID(0);
    Assets::createGeoSphere(vsetScaleUniform, SCALE_UNIFORM_SIZE_FACTOR, 3, true);
    Assets::makeTBNBasis(vsetScaleUniform, false);
    scaleUniform->cleanup();

    Ptr<Assets::Model> pScaleUniformModel (new Assets::Model());
    pScaleUniformModel->addMesh(SCALE_UNIFORM_MESH_NAME, *scaleUniform);

    pPool->addModel(SCALE_UNIFORM_MODEL_NAME, pScaleUniformModel);


    Ptr<Assets::ModelMesh> scaleSize (new Assets::ModelMesh());
    Assets::VerticeSet & vsetScaleSize = scaleSize->getLODByID(0);
    Assets::createGeoSphere(vsetScaleSize, 1, 3, true);
    Assets::makeTBNBasis(vsetScaleSize, false);
    scaleSize->cleanup();

    Ptr<Assets::Model> pScaleSizeModel (new Assets::Model());
    pScaleSizeModel->addMesh(SCALE_SIZE_MESH_NAME, *scaleSize);

    pPool->addModel(SCALE_SIZE_MODEL_NAME, pScaleSizeModel);
}
//-----------------------------------------------------------------------------
void ScaleTool::internalChangeSelection()
{
    if (_pties.size() == 0)
    {
        if (_pNode != null)
        {
            _pNode->setVisible(false, true);
        }
    }
    else
    {
        if (_pNode != null)
        {
            _pNode->setVisible(true, true);
            _pNode->getChildWithTag(tagScaleSize)->setVisible(false);
        }
    }
}

//-----------------------------------------------------------------------------
Ptr<IViewportToolWidget> ScaleTool::createViewportToolWidget(QWidget* parent)
{
    return Ptr<IViewportToolWidget> (new ScaleToolWidget(*this, parent));
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> ScaleTool::createScaleTool(Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet;

    pRet = pWorld->createGroup();

    //model fleche
    Ptr<Universe::NodeMesh> pNodeScaleUniform = pWorld->createMesh(SCALE_UNIFORM_MODEL_NAME, SCALE_UNIFORM_MESH_NAME);
    pNodeScaleUniform->addTag(tagScaleUniform);
    pNodeScaleUniform->getMeshInstance()->setMaterial(TransformTool::_sMatAxeXYZ);

    Ptr<Universe::NodeMesh> pNodeScaleSize = pWorld->createMesh(SCALE_SIZE_MODEL_NAME, SCALE_SIZE_MESH_NAME);
    pNodeScaleSize->addTag(tagScaleSize);
    pNodeScaleSize->getMeshInstance()->setMaterial(TransformTool::_sMatAxeXYZ);

    pRet->addChild(pNodeScaleUniform); 
    pRet->addChild(pNodeScaleSize); 
    pNodeScaleSize->setVisible(false);

    return pRet;
}
//-----------------------------------------------------------------------------
void ScaleTool::configureScaleTool (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected)
{
    Core::Quaternionf orient = pNodeTool->getLocalOrient();
   

    pNodeTool->beginMatrixUpdate();
        pNodeTool->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
        pNodeTool->setLocalScale(Core::Vector3f (1));
    pNodeTool->endMatrixUpdate();

    float scaleFactor = computeScaleFactor(pNodeTool, dc.pCamera);

    pNodeTool->beginMatrixUpdate();
    pNodeTool->setLocalOrientation(orient);
        pNodeTool->setLocalScale(Core::Vector3f (scaleFactor));
    pNodeTool->endMatrixUpdate();

    configureVisible(pNodeTool, dc, elementSelected);
}
//-----------------------------------------------------------------------------
void ScaleTool::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{
    if (_pNode == null) {_pNode = createScaleTool(pWorld);}

    _pNode->beginMatrixUpdate();
        _pNode->setLocalScale(Core::Vector3f (1));
        _pNode->setWorldPosition(getActionCenter());
    _pNode->endMatrixUpdate();

    float scaleFactor = computeScaleFactor(_pNode, dc.pCamera);

    configureScaleTool (_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void ScaleTool::mouseMoveEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_pNode!=null && _elementSelected == NOTHING)
    {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

        float distance;
        Ptr<Universe::Node> pNodeIntersecting;

        ///
        configureScaleTool (_pNode, dc, _elementSelected);
        ///

        Universe::intersect_RayNodeRecursive(_pNode, ray, distance, pNodeIntersecting);

        if(pNodeIntersecting!=null)
        {
            if (pNodeIntersecting->hasTag(tagScaleUniform) == true) _elementHighlighted = AXE_XYZ;
            else _elementHighlighted = NOTHING;
        }
        else
        {
            _elementHighlighted = NOTHING;
        }

        configureHighlighted();
    }
    else if (_pNode!=null && _elementSelected != NOTHING)
    {
        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
        makeScaleAction(ray);
    }
}
//-----------------------------------------------------------------------------
void ScaleTool::mousePressEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (event->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            _elementSelected = NOTHING;
            _elementHighlighted = NOTHING;

            cancelPropertiesChange();
            updateViewportTool();

            QApplication::restoreOverrideCursor();
            event->accept();
        }
    }
    else
    {
        if (_elementHighlighted != NOTHING && _pties.size() > 0)
        {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

            _elementSelected = _elementHighlighted;
            _elementHighlighted = NOTHING;

            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

            initScaleAction(ray, dc);

            QApplication::setOverrideCursor(Qt::BlankCursor);

            beginPropertiesChange();
        }
    }

    configureHighlighted();
    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void ScaleTool::mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_elementSelected != NOTHING)
    {
        _elementSelected = NOTHING;

        finishPropertiesChange();
        updateViewportTool();
        QApplication::restoreOverrideCursor();
    }

    configureVisible(_pNode, dc, _elementSelected);
}
//-----------------------------------------------------------------------------
void ScaleTool::configureVisible(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement elementSelected)
{
    if (pNodeTool!=null)
    {/*
        if (_isVisible==false)
        {
            _pNode->setVisible(false, true);
        }
        else*/
        {
            bool AxeXYZisVisible = false;

            if (elementSelected == AXE_XYZ || elementSelected == NOTHING) AxeXYZisVisible = true;

            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagScaleUniform))->setVisible(AxeXYZisVisible);
            LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeTool->getChildWithTag(tagScaleSize))->setVisible(false);
            hideComponentWithDrawContext(pNodeTool, dc);
        }
    }
}
//-----------------------------------------------------------------------------
void ScaleTool::hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc)
{
}
//-----------------------------------------------------------------------------
void ScaleTool::configureHighlighted()
{
    if (_pNode!=null)
    {
        Renderer::Material matAxeXYZ = TransformTool::_sMatAxeXYZ;

        if (_elementHighlighted == AXE_XYZ) matAxeXYZ = TransformTool::_sMatHighlighted;

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNode->getChildWithTag(tagScaleUniform))->getMeshInstance()->setMaterial(matAxeXYZ);
    }
}
//-----------------------------------------------------------------------------
void ScaleTool::setVisible(bool show)
{
    _isVisible = show;

    if (_pNode!=null)
    {
        _pNode->setVisible(show);
    }
}
//-----------------------------------------------------------------------------
void ScaleTool::disableHighlighted()
{
    _elementSelected = NOTHING;
    _elementHighlighted = NOTHING;
  /*  configureHighlighted();
    configureVisible();*/
}
//-----------------------------------------------------------------------------
bool ScaleTool::isHighlighted() const
{
    if (_pties.size() == 0) return false;
    return _elementHighlighted != NOTHING;
}
//-----------------------------------------------------------------------------
bool ScaleTool::isSelected() const
{
    return _elementSelected != NOTHING;
}
//-----------------------------------------------------------------------------
void ScaleTool::initScaleAction(const Core::Rayf& ray, const DrawContext& dc)
{
    Core::Vector3f normalWorkPlane;

    Ptr<NodeCameraEdition>  pNodeCamEdit = Core::ptrDynamicCast<NodeCameraEdition>(dc.pCamera);
    Ptr<Universe::NodeCamera> pCam;

    if (pNodeCamEdit == null)
        pCam = dc.pCamera;
    else
        pCam = pNodeCamEdit->getActiveViewCamera();

    _WtoView = pCam->getWorldToLocalMatrix();

    normalWorkPlane = -(pCam->getWorldDirection().getNormalizedSafe());
/*
    float dotX = dot(ray.direction, _pNode->getLocalMatrix().getXVector());
    float dotY = dot(ray.direction, _pNode->getLocalMatrix().getYVector());
    float dotZ = dot(ray.direction, _pNode->getLocalMatrix().getZVector());

    float dotMax = Core::max(dotX, dotY, dotZ);
    if(dotMax==dotX)
    {
        normalWorkPlane = _pNode->getLocalMatrix().getXVector();
    }
    else if(dotMax==dotY)
    {
        normalWorkPlane = _pNode->getLocalMatrix().getYVector();
    }
    else
    {
        normalWorkPlane = _pNode->getLocalMatrix().getZVector();
    }*/

    _workPlane = Core::Planef(Core::Plane_NormalPoint, normalWorkPlane, getActionCenter());

    bool isIntersecting = intersect_RayPlane(ray, _workPlane, _startPoint);
    if (isIntersecting==false)  {_startPoint = getActionCenter();}
    _startDistance = (_startPoint -  getActionCenter()).getLengthSafe();
}
//-----------------------------------------------------------------------------
void ScaleTool::makeScaleAction(const Core::Rayf& ray)
{
    Core::Vector3f scaleWorldPoint;

    bool isIntersecting = intersect_RayPlane(ray, _workPlane, scaleWorldPoint);

    if (isIntersecting == true)
    {
  
        Core::Vector3f gre = _WtoView.apply(_startPoint);
        Core::Vector3f offset = _WtoView.apply(scaleWorldPoint) -  gre;

        float offsetDistance = Core::max(Core::L_abs(offset.x), Core::L_abs(offset.y), Core::L_abs(offset.z));

        if (offsetDistance == Core::L_abs(offset.x))          offsetDistance = offset.x;
        else if (offsetDistance == Core::L_abs(offset.y))     offsetDistance = offset.y;
        else                                            offsetDistance = offset.z;


        float fctScale = (offsetDistance);
       

        for (int32 ii=0; ii<_pties.size(); ++ii)
        {
            Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_pties[ii]);
            Core::Vector3f scale = pPtyT->getLocalScale();
            pPtyT->setLocalScale(scale + Core::Vector3f(fctScale));
            pPtyT->updateData();
        }

        _startPoint = scaleWorldPoint;
    }

    updateViewportTool();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ScaleToolWidget::ScaleToolWidget(ScaleTool& st, QWidget * parent):
    _st(st),
    IViewportToolWidget(parent)
{
    setupUI();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------
ScaleToolWidget::~ScaleToolWidget()
{

}
//-----------------------------------------------------------------------------
IViewportTool& ScaleToolWidget::getViewportTool()
{
    return _st;
}
//-----------------------------------------------------------------------------
void ScaleToolWidget::setupUI()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox   = new QtToolbox::SmartGroupBox(this, "Scale");

    _scale = new QtToolbox::NumberEdit("", " %", this);

    _groupBox->addWidget(_scale);

    _layout->addWidget(_groupBox, 0, 0, Qt::AlignCenter);
    
    this->setFixedWidth(_groupBox->width());

    getWidgetsForUndoRedo().push_back(_scale);

    IViewportToolWidget::setupUi();
}
//-----------------------------------------------------------------------------
void ScaleToolWidget::readViewportTool()
{
    _scale->setValue(_st.getLocalScale().x*100);
}
//-----------------------------------------------------------------------------
void ScaleToolWidget::writeViewportTool(QWidget* pWidget)
{
    float s = (float)_scale->getValue()/100.f;
    Core::Vector3f scale (s);
    _st.setLocalScale(scale);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MoveRotateScaleTool::MoveRotateScaleTool(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> > pties, const EActionCenterType& actionsActionCenter, const EOrientationType& orientationType) :
    TransformTool(pStateRecorder, pties, actionsActionCenter, orientationType),
    _elementMoveHighlighted (NOTHING),
    _elementMoveSelected    (NOTHING),
    _elementRotateHighlighted (NOTHING),
    _elementRotateSelected    (NOTHING),
    _isVisible          (true)
{
    internalChangeSelection();
}
//-----------------------------------------------------------------------------
MoveRotateScaleTool::~MoveRotateScaleTool()
{
    if (_pNode != null)
    {
        _pNode->kill();
        _pNode = null;
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::internalChangeSelection()
{
    if (_pties.size() == 0)
    {
        if (_pNode != null)
        {
            _pNode->setVisible(false, true);
        }
    }
    else
    {
        if (_pNode != null)
        {
            _pNode->setVisible(true, true);
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<IViewportToolWidget> MoveRotateScaleTool::createViewportToolWidget(QWidget* parent)
{
    return Ptr<IViewportToolWidget> (new MoveRotateScaleToolWidget(*this, parent));
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> MoveRotateScaleTool::createTools(Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet;

    pRet = pWorld->createGroup();

    Ptr<Universe::NodeGroup> pMove = MoveTool::createMoveTool(pWorld);
    Ptr<Universe::NodeGroup> pRotate;
    Ptr<Universe::NodeGroup> pNodeAngle;

    RotateTool::createRotateTool(pWorld, pRotate, pNodeAngle);

    pMove->addTag(tagMultiTransformMove);
    pRotate->addTag(tagMultiTransformRotate);
    pNodeAngle->addTag(tagMultiTransformRotateAngle);

    pRet->addChild(pMove);
    pRet->addChild(pRotate);
    pRet->addChild(pNodeAngle);

    return pRet;
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::configureTools (Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc, const EMoveToolElement moveElementSelected, const EMoveToolElement rotateElementSelected, const Core::Vector3f& splineFirstPoint, const Core::Vector3f& splineLastPoint)
{
    Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pNodeTool->getChildWithTag(tagMultiTransformMove));
    Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pNodeTool->getChildWithTag(tagMultiTransformRotate));
    Ptr<Universe::NodeGroup> pNodeAngle      = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pNodeTool->getChildWithTag(tagMultiTransformRotateAngle));

    if (pNodeMoveTool->isVisible()==true)
    {
        MoveTool::configureMoveTool (pNodeMoveTool, dc, moveElementSelected);
    }

    if (pNodeRotateTool->isVisible()==true)
    {
        RotateTool::configureRotateTool (pNodeRotateTool, pNodeAngle, dc, rotateElementSelected, splineFirstPoint, splineLastPoint, _interactionPositionCursor);
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{
    if (_pNode == null) {_pNode = createTools(pWorld);}

    Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));
    Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));
    Ptr<Universe::NodeGroup> pNodeAngle      = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotateAngle));

    //position
    _pNode->beginMatrixUpdate();
        _pNode->setWorldPosition(getActionCenter());
    _pNode->endMatrixUpdate();

    //orientation
    _pNode->beginMatrixUpdate();
        _pNode->setLocalOrientation(Core::Quaternionf (Core::IDENTITY));
        _pNode->setLocalScale(Core::Vector3f (1));
    _pNode->endMatrixUpdate();

    pNodeMoveTool->setLocalOrientation(getActionOrientation());
    pNodeRotateTool->setLocalOrientation(getActionOrientation());
    pNodeAngle->setLocalOrientation(getActionOrientation());

    //scale & visible
    configureTools (_pNode, dc, _elementMoveSelected, _elementRotateSelected, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis);
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::mouseMoveEvent (QMouseEvent* event,const DrawContext& dc)
{
    if (_pNode!=null && _elementMoveSelected == NOTHING && _elementRotateSelected == NOTHING)
    {
        Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));
        Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));
        Ptr<Universe::NodeGroup> pNodeAngle      = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotateAngle));

        //test MoveTool
        {
                Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
                Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

                float distance;
                Ptr<Universe::Node> pNodeIntersecting;
            ///
            configureTools (_pNode, dc, _elementMoveSelected, _elementRotateSelected, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis);
            ///
            Universe::intersect_RayNodeRecursive(pNodeMoveTool, ray, distance, pNodeIntersecting);

            if(pNodeIntersecting!=null)
            {
                if      (pNodeIntersecting->hasTag(tagArrowAxeX) == true)   _elementMoveHighlighted = AXE_X;
                else if (pNodeIntersecting->hasTag(tagArrowAxeY) == true)   _elementMoveHighlighted = AXE_Y;
                else if (pNodeIntersecting->hasTag(tagArrowAxeZ) == true)   _elementMoveHighlighted = AXE_Z; 
                else if (pNodeIntersecting->hasTag(tagBiaxisAxeXY) == true) _elementMoveHighlighted = AXE_XY;
                else if (pNodeIntersecting->hasTag(tagBiaxisAxeXZ) == true) _elementMoveHighlighted = AXE_XZ; 
                else if (pNodeIntersecting->hasTag(tagBiaxisAxeYZ) == true) _elementMoveHighlighted = AXE_YZ; 
                else _elementMoveHighlighted = NOTHING;
            }
            else
            {
                _elementMoveHighlighted = NOTHING;
            }

            
        }

        //test Rotate Tool
        if (_elementMoveHighlighted == NOTHING)
        {
                Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();
                Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

                float distance;
                Ptr<Universe::Node> pNodeIntersecting;
            ///
            configureTools (_pNode, dc, _elementMoveSelected, _elementRotateSelected, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis);
            ///
            Universe::intersect_RayNodeRecursive(pNodeRotateTool, ray, distance, pNodeIntersecting);

            if(pNodeIntersecting!=null)
            {
                if      (pNodeIntersecting->hasTag(tagTorusAxeX) == true)   _elementRotateHighlighted = AXE_X;
                else if (pNodeIntersecting->hasTag(tagTorusAxeY) == true)   _elementRotateHighlighted = AXE_Y;
                else if (pNodeIntersecting->hasTag(tagTorusAxeZ) == true)   _elementRotateHighlighted = AXE_Z; 
                else _elementRotateHighlighted = NOTHING;
            }
            else
            {
                _elementRotateHighlighted = NOTHING;
            }
        }

        configureHighlighted();
    }
    else if (_pNode!=null && _elementMoveSelected != NOTHING)
    {
        Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));

        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
        MoveTool::makeMoveAction(ray, pNodeMoveTool, _elementMoveSelected, _moveWorkPlane, _moveStartPoint, _pties, _pContainer);

      //  updateViewportTool();
    }
    else if (_pNode!=null && _elementRotateSelected != NOTHING)
    {
        Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));
        Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

        Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
        Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);

        Core::Vector3f center = getActionCenter();
        if (getActionCenterType() == FREE_USER_CENTER_ROTATE_BOXCENTER)
        {
            center = _centerBoundingBoxInWorldAxis;
        }

        RotateTool::makeRotateAction(ray, Core::Vector2f (event->posF().x(), event->posF().y()), dc, _interactionPositionCursor,_rotateWorkPlane, _invertAxis, _elementRotateSelected, pNodeRotateTool, center, _rotateStartPoint,_splineLastPointInWorldAxis,_lastCursorPos, _pties, _pContainer);
    //    updateViewportTool();
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::mousePressEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (event->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (event->buttons() & Qt::LeftButton || event->buttons() & Qt::MidButton)     //lors de l'evenement le bouton "valide" est enfonce
        {
            _elementMoveSelected      = NOTHING;
            _elementMoveHighlighted   = NOTHING;
            _elementRotateSelected    = NOTHING;
            _elementRotateHighlighted = NOTHING;

            cancelPropertiesChange();
            updateViewportTool();
            connectSignalpropertiesChanged(true);
            if (_pContainer!=null)
            {
                _pContainer->updateData();
                _pContainer->launchSignalProeprtyChanged();
            }

            //QApplication::restoreOverrideCursor();
            event->accept();
        }
    }
    else
    {
        Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));
        Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));
        Ptr<Universe::NodeGroup> pNodeAngle      = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotateAngle));

        //MoveTool
        if (_elementMoveHighlighted != NOTHING && _pties.size() > 0)
        {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

            _elementMoveSelected      = _elementMoveHighlighted;
            _elementMoveHighlighted   = NOTHING;
            _elementRotateSelected    = NOTHING;
            _elementRotateHighlighted = NOTHING;

            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
            
            MoveTool::initMoveAction(ray, pNodeMoveTool, _elementMoveSelected, getActionCenter(), _moveWorkPlane, _moveStartPoint);

            connectSignalpropertiesChanged(false);
            beginPropertiesChange();
        }
        else if (_elementRotateHighlighted != NOTHING && _pties.size() > 0)   //RotateTool
        {
            Ptr<Renderer::IViewVolume> p2D3D = dc.pCamera->getBaseCamera()->createViewVolume();

            _elementRotateSelected    = _elementRotateHighlighted;
            _elementRotateHighlighted = NOTHING;
            _elementMoveSelected      = NOTHING;
            _elementMoveHighlighted   = NOTHING;

            Core::Vector2f pos (event->posF().x()/dc.width, event->posF().y()/dc.height);
            Core::Rayf ray = p2D3D->makeRayFrom2DPoint(pos);
            
            RotateTool::initRotateAction(ray, dc, Core::Vector2f (event->posF().x(), event->posF().y()), pNodeRotateTool, _elementRotateSelected, getActionCenter(),
                _interactionPositionCursor, _lastCursorPos, _invertAxis, _rotateWorkPlane, _rotateStartPoint, _splineFirstPointInWorldAxis, _splineLastPointInWorldAxis);


            connectSignalpropertiesChanged(false);
            beginPropertiesChange();
        }
    }

    configureHighlighted();
    configureVisible();
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::mouseReleaseEvent(QMouseEvent* event,const DrawContext& dc)
{
    if (_elementMoveSelected != NOTHING || _elementRotateSelected != NOTHING)
    {
        _elementMoveSelected = _elementRotateSelected = NOTHING;
      
        finishPropertiesChange();
        updateViewportTool();
        connectSignalpropertiesChanged(true);
        //QApplication::restoreOverrideCursor();

        if (_pContainer!=null)
        {
            _pContainer->launchSignalProeprtyChanged();
        }
    }

    configureVisible();
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::configureVisible()
{
    Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));
    Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));
    Ptr<Universe::NodeGroup> pNodeAngle      = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotateAngle));

    if (_elementMoveSelected != NOTHING)
    {
        pNodeAngle->setVisible(false, true);
        pNodeRotateTool->setVisible(false, true);
    }
    else
    {
        if (_elementRotateSelected != NOTHING)
        {
            pNodeMoveTool->setVisible(false, true);
        }
        else
        {
            pNodeAngle->setVisible(true, true);
            pNodeMoveTool->setVisible(true, true);
            pNodeRotateTool->setVisible(true, true);
        }
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::hideComponentWithDrawContext(Ptr<Universe::NodeGroup>& pNodeTool, const DrawContext& dc)
{
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::configureHighlighted()
{
    if (_pNode!=null)
    {
        //Move
        Ptr<Universe::NodeGroup> pNodeMoveTool   = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformMove));
        Ptr<Universe::NodeGroup> pNodeRotateTool = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(_pNode->getChildWithTag(tagMultiTransformRotate));

        Renderer::Material matAxeX = TransformTool::_sMatAxeX;
        Renderer::Material matAxeY = TransformTool::_sMatAxeY;
        Renderer::Material matAxeZ = TransformTool::_sMatAxeZ;

        if (_elementMoveHighlighted == AXE_X)   matAxeX = TransformTool::_sMatHighlighted;
        if (_elementMoveHighlighted == AXE_Y)   matAxeY = TransformTool::_sMatHighlighted;
        if (_elementMoveHighlighted == AXE_Z)   matAxeZ = TransformTool::_sMatHighlighted;

        if (_elementMoveHighlighted == AXE_XY) {matAxeX = TransformTool::_sMatHighlighted;   matAxeY = TransformTool::_sMatHighlighted;}
        if (_elementMoveHighlighted == AXE_XZ) {matAxeX = TransformTool::_sMatHighlighted;   matAxeZ = TransformTool::_sMatHighlighted;}
        if (_elementMoveHighlighted == AXE_YZ) {matAxeY = TransformTool::_sMatHighlighted;   matAxeZ = TransformTool::_sMatHighlighted;}

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeMoveTool->getChildWithTag(tagArrowAxeX))->getMeshInstance()->setMaterial(matAxeX);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeMoveTool->getChildWithTag(tagArrowAxeY))->getMeshInstance()->setMaterial(matAxeY);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeMoveTool->getChildWithTag(tagArrowAxeZ))->getMeshInstance()->setMaterial(matAxeZ);

        //Rotate
        Renderer::Material matRotateAxeX = TransformTool::_sMatAxeX;
        Renderer::Material matRotateAxeY = TransformTool::_sMatAxeY;
        Renderer::Material matRotateAxeZ = TransformTool::_sMatAxeZ;

        if (_elementRotateHighlighted == AXE_X)   matRotateAxeX = TransformTool::_sMatHighlighted;
        if (_elementRotateHighlighted == AXE_Y)   matRotateAxeY = TransformTool::_sMatHighlighted;
        if (_elementRotateHighlighted == AXE_Z)   matRotateAxeZ = TransformTool::_sMatHighlighted;

        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeRotateTool->getChildWithTag(tagTorusAxeX))->getMeshInstance()->setMaterial(matRotateAxeX);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeRotateTool->getChildWithTag(tagTorusAxeY))->getMeshInstance()->setMaterial(matRotateAxeY);
        LM_DEBUG_PTR_CAST<Universe::NodeMesh>(pNodeRotateTool->getChildWithTag(tagTorusAxeZ))->getMeshInstance()->setMaterial(matRotateAxeZ);
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::setVisible(bool show)
{

}
//-----------------------------------------------------------------------------
void MoveRotateScaleTool::disableHighlighted()
{

}
//-----------------------------------------------------------------------------
bool MoveRotateScaleTool::isHighlighted() const
{
    if (_pties.size() == 0) return false;

    bool isH = false;

    if (_elementMoveHighlighted         != NOTHING)       isH = true;
    else if (_elementRotateHighlighted  != NOTHING)       isH = true;

    return isH;
}
//-----------------------------------------------------------------------------
bool MoveRotateScaleTool::isSelected() const
{
    bool isS = false;

    if (_elementMoveSelected        != NOTHING)       isS = true;
    else if (_elementRotateSelected != NOTHING)       isS = true;

    return isS;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MoveRotateScaleToolWidget::MoveRotateScaleToolWidget(MoveRotateScaleTool& tool, QWidget * parent):
    IViewportToolWidget(parent),
    _tool(tool)
{
    setupUI();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------
MoveRotateScaleToolWidget::~MoveRotateScaleToolWidget()
{
}
//-----------------------------------------------------------------------------
IViewportTool& MoveRotateScaleToolWidget::getViewportTool()
{
    return _tool;
}
//-----------------------------------------------------------------------------
void MoveRotateScaleToolWidget::setupUI()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _pActionCenter = createActionCenterComboBox(this, _tool);
    _pOrientation = createOrientationComboBox(this, _tool);

    _move   = new QtToolbox::NumberEditVector3(this, "Move", " m");
   // _rotate = new QtToolbox::SpinBoxVector3(this, "Rotate");


    _layout->addWidget(_pActionCenter,  0, 0, Qt::AlignCenter);
    _layout->addWidget(_pOrientation, 0, 1, Qt::AlignCenter);
    _layout->addWidget(_move);
    //_layout->addWidget(_rotate);

    
    getWidgetsForUndoRedo().push_back(_move);
  //  getWidgetsForUndoRedo().push_back(_rotate);
    IViewportToolWidget::setupUi();
}
//-----------------------------------------------------------------------------
void MoveRotateScaleToolWidget::readViewportTool()
{
    if (_tool.getActionOrientationType() == GLOBAL_ORIENTATION)
    {
        _move->setValue(_tool.getActionCenter());
    }
    else
    {
        _move->setValue(Core::Vector3f(Core::ZERO));
    }
}
//-----------------------------------------------------------------------------
void MoveRotateScaleToolWidget::writeViewportTool(QWidget* pWidget)
{    
    Core::Vector3f actionPos;
    _move->getValue(actionPos);

    _tool.moveTo(_tool.getActionOrientationType(), actionPos);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
