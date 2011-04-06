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
#include "PtyTrajectory.moc.h"
#include <QMessageBox>

#include <QtToolbox/ComboBox.moc.h>
#include <QtToolbox/SingleSlidingHDR.moc.h>

#include <Universe/Tasks/MoveTrajectory.h>
#include <Assets/GeometricPrimitive.h>


#include <EPI/Gui/Widget/CustomLine.moc.h>
#include <EPI/Document/ContentInfo/SplineRepresentation.h>

#include <EPI/Document/Properties/PtyTrajectory/TableCheckPointModel.moc.h>
#include <EPI/Document/Properties/PtyTrajectory/TableCheckPointView.moc.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Document/PropertySelection.moc.h>

#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
#include <EPI/Gui/Viewport/WorldViewport.moc.h>

#include <QtToolbox/CollapsibleWidget.moc.h>

/*
#include <QAbstractTableModel>
#include <QAction>
#include <QPushButton>

#include <QtToolbox/SmartGroupBox.moc.h>

#include <EPI/Document/ContentInfo/SplineRepresentation.h>




*/



namespace EPI
{

const Core::String CONE_MESH_NAME = L"$ConePtyTrajMesh";
const Core::String CONE_MODEL_NAME = L"$ConePtyTraj";
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
String trajTaskModeToString(ETrajTaskMode mode)
{
    String ret;

    switch(mode)
    {
        case TRAJ_MODEL: ret = L"Model"; break;
        case TRAJ_CAMERA: ret = L"Camera"; break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
ETrajTaskMode stringToTrajTaskMode(String mode)
{
    ETrajTaskMode ret;

    if (mode == L"Model")       ret = TRAJ_MODEL;
    else if (mode == L"Camera") ret = TRAJ_CAMERA;

    return ret;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyTrajectory::PtyTrajectory(GuiDocument& guiDoc, const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::World>& pWorld, const Ptr<StateRecorder>& pStateRecorder, const Ptr<Universe::Node>& pNode, const Core::String& title):
    PtyNodeEditableContent(pStateRecorder, pWorld, pNode, title),
    _pWorldForRepresentation(pWorldForRepresentation),
    _pStateRecorder(pStateRecorder),
    _guiDoc(guiDoc),
    _factor(1.f),
    _mode(TRAJ_CAMERA),
    _editMode(true),
    _isInCameraCtrl(false),
    _nodePivot(null)
{
    LOG_FCT_NAME

    /////////////////////
    Ptr<Assets::ModelMesh> cone (new Assets::ModelMesh());
    Assets::VerticeSet & vsetCone = cone->getLODByID(0);
    Assets::createCone(vsetCone, 1.5f, 2);
    //Assets::makeTBNBasis(vsetPlan, false);
    cone->cleanup();

    Ptr<Assets::Model> pConeModel (new Assets::Model());
    pConeModel->addMesh(CONE_MESH_NAME, *cone);

    pWorldForRepresentation->getRessourcesPool()->addModel(CONE_MODEL_NAME, pConeModel);

    _pNodeModel = _pWorldForRepresentation->createGroup();
    Ptr<Universe::Node>  pCone =  _pWorldForRepresentation->createMesh(CONE_MODEL_NAME, CONE_MESH_NAME);
    pCone->localPitch(f_PI_DIV_2);

    _pNodeModel->addChild(pCone);
    _pNodeModel->setVisible(false, true);
    ///////////////


    _pRepresentation = Ptr<IContentRepresentation>(new SplineRepresentation(pWorldForRepresentation, getUniverseNode()));
    _pRepresentation->setRepresentationMode(SELECTION_REPRESENTATION);

     Core::CheckPoint a;

            a.position = Core::Vector3f(0,0,0);
            a.time = 1;
        insertCheckPoint(0, a.position, a.orient, a.time);
            a.position = Core::Vector3f(0,0,6);
            a.time = 2;
        insertCheckPoint(1, a.position, a.orient, a.time);
            a.position = Core::Vector3f(0,0,12);
            a.time = 3;
        insertCheckPoint(2, a.position, a.orient, a.time);
            a.position = Core::Vector3f(0,0,18);
            a.time = 4;
        insertCheckPoint(3, a.position, a.orient, a.time);
            a.position = Core::Vector3f(0,0,24);
            a.time = 5;
        insertCheckPoint(4, a.position, a.orient, a.time);


        
/*
        Core::CheckPoint a;
        a.position = Core::Vector3f(0.0f, 0.0f, 0.0f);
        a.time = 1;
        _traject.getCheckPoints().push_back(a);
        a.position = Core::Vector3f(0.0f, 0.0f, 10.0f);
        a.time = 2;
        _traject.getCheckPoints().push_back(a);
        a.position = Core::Vector3f(0.0f, 0.0f, 20.0f);
        a.time = 3;
        _traject.getCheckPoints().push_back(a);
        a.position = Core::Vector3f(0.0f, 0.0f, 30.0f);
        a.time = 4;
        _traject.getCheckPoints().push_back(a);
        a.position = Core::Vector3f(0.0f, 0.0f, 40.0f);
        a.time = 5;
        _traject.getCheckPoints().push_back(a);


    */
   // initPtyChildren();

    _pNodeCamTraj = pWorldForRepresentation->createPerspectiveCamera();

    Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform> (getPtyTransform());
    connect(pPtyT.get(), SIGNAL(propertyChanged()), this, SLOT(ptyTransformChanged()));



  //  updateProperty();
     updateData();
}
//-----------------------------------------------------------------------------
PtyTrajectory::~PtyTrajectory()
{
    LOG_FCT_NAME

    _pNodeCamTraj->kill();
    _pNodeModel->kill();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::ptyTransformChanged()
{
    for (int32 iPty=0; iPty<getPtiesContentSelectable().size(); iPty++)
    {
        Ptr<PtyCheckPoint> pPtyCP = LM_DEBUG_PTR_CAST<PtyCheckPoint>(getPtiesContentSelectable()[iPty]);
        Ptr<PtyTransformContent> pPtyT   = LM_DEBUG_PTR_CAST<PtyTransformContent>(pPtyCP->getPtyTransform());

        pPtyT->setParentToWorldMatrix(getUniverseNode()->getLocalToWorldMatrix());
        pPtyCP->updateData();
    }
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeCamera>& PtyTrajectory::getCamera() const
{
    return _pNodeCamTraj;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::Node>& PtyTrajectory::getModel() const
{
    return _pNodeModel;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::updateModel(const String& newModelName)
{
    if (newModelName != _modelName)
    {
        _modelName = newModelName;

        if (_pNodeModel!=null)  _pNodeModel->kill();

        if (_modelName != L"")
        {
            _pNodeModel = _pWorldForRepresentation->createModel(newModelName);
            
        }
        else
        {
            _pNodeModel = _pWorldForRepresentation->createGroup();
            Ptr<Universe::Node>  pCone =  _pWorldForRepresentation->createMesh(CONE_MODEL_NAME, CONE_MESH_NAME);
            pCone->localPitch(f_PI_DIV_2);
            _pNodeModel->addChild(pCone);
        }

        _pNodeModel->setVisible(false, true);
    }
}
//-----------------------------------------------------------------------------
Ptr<PropertyWidget> PtyTrajectory::internalCreatePropertyWidget(const Ptr<PropertyWidgetDataProxy>& pDataProxy, QWidget * parent)
{
    LOG_FCT_NAME

    Ptr<PtyWidgetTrajectory> pPW (new PtyWidgetTrajectory(pDataProxy, parent));

    return pPW;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::updateData()
{
    LOG_FCT_NAME

    PtyNodeEditableContent::updateData();

    if (getPtiesContentSelectable().size()>2)
    {
        Core::List<Core::CheckPoint>    cps;
        for (int32 iPty=0; iPty<getPtiesContentSelectable().size(); iPty++)
        {
            Ptr<PtyCheckPoint> pPtyCP = LM_DEBUG_PTR_CAST<PtyCheckPoint>(getPtiesContentSelectable()[iPty]);
            Ptr<PtyTransform> pPtyT   = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyCP->getPtyTransform());

            Core::CheckPoint  cp (pPtyT->getLocalPosition(), Core::Quaternionf(pPtyT->getLocalAxis()), pPtyCP->getTime());
            cps.push_back(cp);

            bool showCheckPoint = true;
            if (_editMode == false)             showCheckPoint = false;
            else if (_isInCameraCtrl == true)   showCheckPoint = false;

            pPtyCP->setVisible(showCheckPoint);
            pPtyCP->updateData();
        }

        _traject = Core::Trajectory (cps);
        _pRepresentation->updateObjectRepresentation(*this);
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::updateProperty()
{
    LOG_FCT_NAME
    PtyNodeEditableContent::updateProperty();
    _pRepresentation->updateObjectRepresentation(*this);
}
//-----------------------------------------------------------------------------
Ptr<Property> PtyTrajectory::clone() const
{
    LOG_FCT_NAME

    return Ptr<Property>(new PtyTrajectory( *this ));
}
//-----------------------------------------------------------------------------
void PtyTrajectory::internalCopy(const Ptr<Property>& pSrc)
{
    LOG_FCT_NAME

    PtyNodeEditableContent::internalCopy(pSrc);

    Ptr<PtyTrajectory> pPty = LM_DEBUG_PTR_CAST<PtyTrajectory>(pSrc);

//    _pWorldForRepresentation = pPty->_pWorldForRepresentation;
    //_traject 
    //_pRepresentation;
    //_pStateRecorder;
    //_pNodeCamTraj;
    //_pNodeModel 
    //_guiDoc     = pPty->_guiDoc;
    _modelName  = pPty->_modelName;
    _factor     = pPty->_factor;
    _mode       = pPty->_mode;
    _editMode   = pPty->_editMode;


    _isInCameraCtrl = false;//pPty->_isInCameraCtrl;
    _memoViewportCameraBeforeCamMode = pPty->_memoViewportCameraBeforeCamMode;
    _viewportCamera = pPty->_viewportCamera;

    _camModeCheckPointId  = pPty->_camModeCheckPointId;
    _nodePivot = pPty->_nodePivot;
    if(_pRepresentation!=null)
        _pRepresentation->updateObjectRepresentation(*this);

    updateData();
}
//-----------------------------------------------------------------------------
Core::Trajectory& PtyTrajectory::getTrajectory()
{
    return  _traject;
}
//-----------------------------------------------------------------------------
const Core::Trajectory& PtyTrajectory::getTrajectory() const
{
    return  _traject;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::setTrajectory(const Core::Trajectory& tra)
{
    _traject = tra;
    initPtyChildren();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::editingSelectionChanged()
{
    Ptr<PropertySelection>  ptyS = LM_DEBUG_PTR_CAST<PropertySelection>(getPtySelection()); 
    emit selectionChanged(ptyS->getSelectedPties());
 //   emit propertyChanged();

    if (ptyS->getSelectedPties().size()>0)
    {
        Ptr<PtyTransform> pPty = LM_DEBUG_PTR_CAST<PtyTransform> (ptyS->getSelectedPties().back()->getPtyTransform());
        getCamera()->setWorldPosition(pPty->getWorldPosition());
        getCamera()->setLocalOrientation(Core::Quaternionf(pPty->getWorldAxis()));
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::initPtyChildren()
{
    getPtiesContentSelectable().clear();

    const Core::List<Core::CheckPoint> & cp = _traject.getCheckPoints();
    for (int32 ii=0; ii<cp.size(); ++ii)
    {
        insertCheckPoint(ii, cp[ii].position, cp[ii].orient, cp[ii].time, false);
        /*
        Ptr<PtyCheckPoint> pPtyCP = Ptr<PtyCheckPoint>(new PtyCheckPoint(_pWorldForRepresentation));

        Ptr<PtyTransformContent> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformContent>(pPtyCP->getPtyTransform());
        pPtyT->setLocalPosition(cp[ii].position);
        pPtyT->setLocalOrientation(cp[ii].orient);
        pPtyCP->setTime(cp[ii].time);
        pPtyCP->updateData();

        getPtiesContentSelectable().push_back(pPtyCP);
        connect(pPtyCP.get(), SIGNAL(updateContainer(const Property&)), this, SLOT(propertyContentChanged(const Property&)));*/
    }

    updateData();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::deleteSelectedCheckPoint()
{
    Ptr<PropertySelection>  ptyS = LM_DEBUG_PTR_CAST<PropertySelection>(getPtySelection());

    Core::List<Ptr<Property> > ptiesToDelete= ptyS->getSelectedPties();
    ptyS->deselectAll();

    for (int32 ii=0; ii<ptiesToDelete.size(); ++ii)
    {
        for (int32 iPty=0; iPty<getPtiesContentSelectable().size(); ++iPty)
        {
            if (getPtiesContentSelectable()[iPty]->getId() == ptiesToDelete[ii]->getId())
            {
                getPtiesContentSelectable().erase(getPtiesContentSelectable().begin()+iPty);
            }
        }
    }

    updateData();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::propertyContentChanged(const Property& pty)
{
    updateData();
    launchSignalProeprtyChanged();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::insertCheckPoint (int32 index, const Core::Vector3f& pos, const Core::Quaternionf& orient, float time, bool mustUpdateData)
{
    Ptr<PtyCheckPoint> pPtyCP;

    if (getPtiesContentSelectable().size()==0)
    {
        pPtyCP = Ptr<PtyCheckPoint>(new PtyCheckPoint(_pWorldForRepresentation));

        Ptr<PtyTransformContent> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformContent>(pPtyCP->getPtyTransform());
        pPtyT->setLocalPosition(pos);
        pPtyT->setLocalAxis(orient.toMatrix3());
        pPtyCP->setTime(time);
    }
    else
    {
        pPtyCP = LM_DEBUG_PTR_CAST<PtyCheckPoint>(getPtiesContentSelectable()[0]->clone());
        Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyCP->getPtyTransform());
        pPtyT->setLocalPosition(pos);
        pPtyT->setLocalAxis(orient.toMatrix3());

        if (time<0)
        {
            int32 posIdex = index-1;
            if (posIdex<0) posIdex = 0;
            time = LM_DEBUG_PTR_CAST<PtyCheckPoint>(getPtiesContentSelectable()[posIdex])->getTime();
        }

        pPtyCP->setTime(time);

        //getPtiesContentSelectable().insert(getPtiesContentSelectable().begin()+index, pPtyCP);
        

        pPtyCP->internalResurrect(getUniverseWorld(), _pWorldForRepresentation, *this);
        
        if(mustUpdateData)
            updateData();
    }
   // getUniverseNode()->addChild(pPtyCP->getUniverseNodeRepresentation());
    Ptr<PtyTransformContent> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformContent> (pPtyCP->getPtyTransform());
    pPtyT->setParentToWorldMatrix(getUniverseNode()->getLocalToWorldMatrix());

    addContentRecording(getStateRecorder(), LM_DEBUG_PTR_CAST<Property>(pPtyCP), *this, index);
    connect(pPtyCP.get(), SIGNAL(updateContainer(const Property&)), this, SLOT(propertyContentChanged(const Property&)));
}
//-----------------------------------------------------------------------------
void PtyTrajectory::insertLocalisationCamera(int32 index)
{
    Ptr<Universe::NodeCamera> pNodeCamView = _guiDoc.getGuiContext()->getCurrentViewport()->getView()->getNodeCamera();

    float               time = -1;
    int32 indexBefore = index-1;
    int32 indexAfter  = index;

    float timeBefore=-1;
    float timeAfter=-1;

    if (indexBefore>=0 && indexBefore<getPtiesContentSelectable().size())
    {   
        timeBefore = LM_DEBUG_PTR_CAST<PtyCheckPoint> (getPtiesContentSelectable()[indexBefore])->getTime();
    }

    if ((indexAfter)>=0 && (indexAfter)<getPtiesContentSelectable().size())
    {
        timeAfter = LM_DEBUG_PTR_CAST<PtyCheckPoint> (getPtiesContentSelectable()[indexAfter])->getTime();
    }

    if (timeBefore>=0 && timeAfter>=0)  time = (timeAfter + timeBefore)/2.f;
    else if (timeBefore>=0)             time = timeBefore;
    else if (timeAfter>=0)              time = timeAfter;

    //insertCheckPoint(index, pNodeCamView->getLocalPosition(), pNodeCamView->getLocalOrient(), time);
    const Core::Matrix4f& mat = pNodeCamView->getLocalToWorldMatrix();
    Core::Vector3f      posInWorldAxis = mat.getTranslation();
    insertCheckPoint(index, posInWorldAxis, Core::Quaternionf(mat.getRotation()), time);
}
//-----------------------------------------------------------------------------
void PtyTrajectory::createCheckPoint(int32 index)
{
    float               time = 0;
    Core::Vector3f      pos (Core::ZERO);
    Core::Quaternionf   orien (Core::IDENTITY);

    Ptr<PtyCheckPoint>  before;
    Ptr<PtyCheckPoint>  after;

    int32 indexBefore = index-1;
    int32 indexAfter  = index;

    if (indexBefore>=0 && indexBefore<getPtiesContentSelectable().size())
    {   
        before = LM_DEBUG_PTR_CAST<PtyCheckPoint> (getPtiesContentSelectable()[indexBefore]);
    }

    if ((indexAfter)>=0 && (indexAfter)<getPtiesContentSelectable().size())
    {
        after = LM_DEBUG_PTR_CAST<PtyCheckPoint> (getPtiesContentSelectable()[indexAfter]);
    }

    if (before!= null && after!=null)
    {   
        time    = (before->getTime() + after->getTime())/2.f;    
        pos     = (before->getLocalPosition() + after->getLocalPosition())/2.f;
        orien   = (before->getLocalOrientation() + after->getLocalOrientation())/2.f;
    }
    else if (before != null)
    {
        time    = before->getTime() + 1;
        pos     = before->getLocalPosition();
        orien   = before->getLocalOrientation();
    }
    else if (after  != null)
    {
        time    = after->getTime() + 1;
        pos     = after->getLocalPosition();
        orien   = after->getLocalOrientation();
    }

    insertCheckPoint (index, pos, orien, time);
}
//-----------------------------------------------------------------------------
void PtyTrajectory::deleteCheckPoint(int32 start, int32 end)
{
    Ptr<PropertySelection>  ptyS = LM_DEBUG_PTR_CAST<PropertySelection>(getPtySelection());
    ptyS->deselectAll();
/*
    if (getStateRecorder()->isRecording() == true)
    {
        int a = 5;
    }

    for (int32 iPty=start; iPty<=end; ++iPty)
    {
        getPtiesContentSelectable().erase(getPtiesContentSelectable().begin()+(iPty -(iPty-start)));
    }*/

    removeContentRecording (getStateRecorder(), start, end, *this);
}
//-----------------------------------------------------------------------------
void PtyTrajectory::runTask()
{
    if (isInCameraCtrl() == true)
    {
        QMessageBox::information(NULL, "", "disable camera control mode");
    }
    else
    {
        if (getPtiesContentSelectable().size()>2)
        {
            Ptr<Universe::Node> pNode;

            if(_mode==TRAJ_MODEL)
            {
                pNode = getModel();
                pNode->setVisible(true, true);
            }
            else if(_mode==TRAJ_CAMERA)
            {
                pNode = getCamera();
                _viewportCamera = _guiDoc.getGuiContext()->getCurrentViewport()->getDesc()._cameraView;
                _guiDoc.getGuiContext()->setCurrentViewportCameraViewEdition();
            }


            if (pNode!=null)
            {
                Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform> (getPtiesContentSelectable()[0]->getPtyTransform());

                _pTask = Ptr<Universe::ITask> (new Universe::TaskMoveTrajectory(pNode, getUniverseNode(), getTrajectory().getCheckPoints(), _factor));
                getUniverseWorld()->registerTask(_pTask);
                _guiDoc.addSpyTask(createSpyTaskForEndTask(_pTask));

                showRepresentation(false);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::stopTask()
{
    if (_pTask!=null)
    {
        showRepresentation(true);
        _pTask->kill();
        //_pTask = null;
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::runStopTask()
{
    if (_pTask==null)
    {
        runTask();
    }
    else
    {
        stopTask();
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::showRepresentation(bool flag)
{
    _pRepresentation->setVisible(flag);
    for (int32 iPty=0; iPty<getPtiesContentSelectable().size(); iPty++)
    {
        Ptr<PtyCheckPoint> pPtyCP = LM_DEBUG_PTR_CAST<PtyCheckPoint>(getPtiesContentSelectable()[iPty]);
      

        bool showCheckPoint = flag;
        if (_editMode == false)             showCheckPoint = false;
        else if (_isInCameraCtrl == true)   showCheckPoint = false;

        pPtyCP->setVisible(showCheckPoint);
        pPtyCP->updateData();
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::signalEndTask(const Ptr<Universe::ITask>& pTask)
{
    LM_ASSERT(_pTask == pTask);

    _pTask = null;
    _pNodeModel->setVisible(false, true);

    showRepresentation(true);




    if (_mode==TRAJ_CAMERA)
    {
        _guiDoc.getGuiContext()->setCurrentViewportCameraView(_viewportCamera);
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::setEditMode(bool isEditMode)
{
    _editMode = isEditMode;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::startCameraCtrlMode()
{
    _isInCameraCtrl = true;

    _memoViewportCameraBeforeCamMode = _guiDoc.getGuiContext()->getCurrentViewport()->getDesc()._cameraView;
    _guiDoc.getGuiContext()->setCurrentViewportCameraViewEdition();

    Ptr<PropertySelectionContent> pPtyS = LM_DEBUG_PTR_CAST<PropertySelectionContent>(getPtySelection());

    LM_ASSERT(pPtyS->getSelectedPtiesCount()==1);

    //_camModeCheckPointId = pPtyS->getSelectedPty(0)->getId();
    updateData();
    cameraCtrlStartAction(pPtyS->getSelectedPty(0)->getId());
}
//-----------------------------------------------------------------------------
void PtyTrajectory::endCameraCtrlMode()
{
    cameraCtrlEndAction();
    _guiDoc.getGuiContext()->setCurrentViewportCameraView(_memoViewportCameraBeforeCamMode);
    _isInCameraCtrl = false;
    
    updateData();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::cameraCtrlStartAction(int32 checkPointId)
{
    Ptr<Property> pCP;
    bool res = findPtyWithId(pCP, checkPointId);

    LM_ASSERT(res == true);

    getStateRecorder()->startRecording();
    Ptr<PropertySelectionContent> pPtyS = LM_DEBUG_PTR_CAST<PropertySelectionContent>(getPtySelection());

    beginChangeProperty(getStateRecorder(), pPtyS);
        pPtyS->select(pCP);
    finishChangeProperty(getStateRecorder(), pPtyS, false);

    _camModeCheckPointId = checkPointId;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::cameraCtrlEndAction()
{
    Ptr<Property> pPtyCP;
    bool res = findPtyWithId(pPtyCP, _camModeCheckPointId);

    LM_ASSERT(res == true);
    Ptr<Universe::NodeCamera> pNodeCamView = _guiDoc.getGuiContext()->getCurrentViewport()->getView()->getNodeCamera();
    const Core::Matrix4f& mat = pNodeCamView->getLocalToWorldMatrix();
    Core::Vector3f      posInWorldAxis = mat.getTranslation();
 

    Ptr<PtyTransformContent> pPtyT = LM_DEBUG_PTR_CAST<PtyTransformContent>(pPtyCP->getPtyTransform());

    if (pPtyT->getWorldPosition() != posInWorldAxis || pPtyT->getWorldAxis() != mat.getRotation())
    {
        beginChangeProperty(getStateRecorder(), pPtyT);
            pPtyT->setWorldPosition(posInWorldAxis);
            pPtyT->setWorldOrientation(mat.getRotation());
        finishChangeProperty(getStateRecorder(), pPtyT, false);
        getStateRecorder()->stopRecording();
    }
    else
    {
        getStateRecorder()->eraseRecording();
    }

    updateData();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::moveTrajectory()
{
    Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(getPtyTransform());

    if (_nodePivot ==null)
    {
        beginChangeProperty(getStateRecorder(), pPtyT);
            pPtyT->setLocalPosition(Core::Vector3f(Core::ZERO));
            pPtyT->setLocalAxis(Core::Matrix3f(Core::IDENTITY));
        finishChangeProperty(getStateRecorder(), pPtyT);
    }
    else
    {
        Core::Vector3f localPos(0.0f, 0.0f, -12.0f);
        Core::Vector3f worldPos = _nodePivot->getLocalToWorldMatrix().apply(localPos);

        beginChangeProperty(getStateRecorder(), pPtyT);
            pPtyT->setLocalPosition(worldPos);
            pPtyT->setLocalAxis(Core::Matrix3f(_nodePivot->getLocalToWorldMatrix()));
        finishChangeProperty(getStateRecorder(), pPtyT);
    }

    pPtyT->updateData();
    ptyTransformChanged();
    updateData();
}
//-----------------------------------------------------------------------------
void PtyTrajectory::setNodePivotTrajectory(const Ptr<Universe::Node>& pivot)
{
    _nodePivot = pivot;
}
//-----------------------------------------------------------------------------
bool PtyTrajectory::beforeUndo()
{
    if (isInCameraCtrl() == true)
    {
        QMessageBox::information(NULL, "", "disable camera control mode for undo");
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool PtyTrajectory::beforeRedo()
{
    if (isInCameraCtrl() == true)
    {
        QMessageBox::information(NULL, "", "disable camera control mode for redo");
        return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
void PtyTrajectory::cameraCtrlSelectNextCP()
{
    if (isInCameraCtrl() == true)
    {
        int32 oldIndexCP = findIndexWithId(_camModeCheckPointId);
        LM_ASSERT(oldIndexCP>=0);

        int32 newIndexCP = oldIndexCP + 1;
        if (newIndexCP >= getPtiesContentSelectable().size())    newIndexCP = 0;

        cameraCtrlEndAction();
        cameraCtrlStartAction(getPtiesContentSelectable()[newIndexCP]->getId());
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::cameraCtrlSelectPrevCP()
{
    if (isInCameraCtrl() == true)
    {
        int32 oldIndexCP = findIndexWithId(_camModeCheckPointId);
        LM_ASSERT(oldIndexCP>=0);
        
        int32 newIndexCP = oldIndexCP - 1;
        if (newIndexCP < 0) newIndexCP = getPtiesContentSelectable().size()-1;

        cameraCtrlEndAction();
        cameraCtrlStartAction(getPtiesContentSelectable()[newIndexCP]->getId());
    }
}
//-----------------------------------------------------------------------------
void PtyTrajectory::cameraCtrlSelectInsertCP()
{
    if (isInCameraCtrl() == true)
    {
 
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyWidgetTrajectory::PtyWidgetTrajectory(const Ptr<PropertyWidgetDataProxy>& data, QWidget * parent):
    PropertyWidget(data, parent)
{
    LOG_FCT_NAME

    setupUi();
}
//-----------------------------------------------------------------------------
PtyWidgetTrajectory::~PtyWidgetTrajectory()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::setupUi()
{
    LOG_FCT_NAME

    Ptr<PtyTrajectory> pPtyTrajectory = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());

  //  connect(pPtyTrajectory.get(), SIGNAL(propertyChanged()), this, SLOT(ptyTrajectoryChanged()));

    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);


    _editTraj  = new QPushButton("Edit", this);
    _editTraj->setCheckable(true);
    _editTraj->setFixedSize(80,20);

    _moveTraj  = new QPushButton("Move To Players Position", this);
    _moveTraj->setFixedSize(140,20);


    _cameraModeGroup = new QtToolbox::CollapsibleWidget(this, "Camera Mode");
    QGridLayout* layoutCameraGroup = new QGridLayout(NULL);
        layoutCameraGroup->setContentsMargins(0, 0, 0, 0);
        layoutCameraGroup->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        _cameraModeGroup->setLayout(layoutCameraGroup);

    _cameraCtrl  = new QPushButton("Camera Ctrl", this);
    _cameraCtrl->setCheckable(true);
    _cameraCtrl->setFixedSize(80,20);

    _cameraCtrlNextCP   = new QPushButton(QIcon(":/icons/next.png"), "", this);
    _cameraCtrlNextCP->setFixedSize(20,20);
    _cameraCtrlNextCP->setShortcut(Qt::Key_PageUp);

    _cameraCtrlPrevCP   = new QPushButton(QIcon(":/icons/prev.png"), "", this);
    _cameraCtrlPrevCP->setFixedSize(20,20);
    _cameraCtrlPrevCP->setShortcut(Qt::Key_PageDown);

    _cameraCtrlInsertCP = new QPushButton(QIcon(":/icons/add.png"), "", this);
    _cameraCtrlInsertCP->setFixedSize(20,20);
    _cameraCtrlInsertCP->setShortcut(Qt::Key_Insert);

    _modelCP = new TableCheckPointModel(*pPtyTrajectory.get(), this);
    _viewCP = new TableCheckPointView (this);
    _viewCP->setModel(_modelCP);
    _viewCP->setVisiblePosition(true);
    _viewCP->setVisibleRotation(false);


    _insertLocalisationCamera = new QPushButton("Insert Check Point Camera", this);
    _insertLocalisationCamera->setShortcut(Qt::Key_F2);
    _insertLocalisationCamera->setShortcutEnabled(true);
    //_viewCP->horizontalHeader()->hideSection(7);
    //_viewCP->setFixedWidth(500);
    //_viewCP->setFixedHeight(500);
    //_groupBox->addWidget(_viewCP);

    _factor = new QtToolbox::SingleSlidingHDR(this, "Factor", false);


    _modelName    = new CustomLine(this, "Model name");
    _modelName->pushAuthorizedDropMimeData("asset/model");
    _modelName->setReadOnly(true);

    _taskMode = new QtToolbox::ComboBox(this);
    _taskMode->addItem(trajTaskModeToString(TRAJ_MODEL));
    _taskMode->addItem(trajTaskModeToString(TRAJ_CAMERA));


    _layout->addWidget(_editTraj,0,0);
    _layout->addWidget(_taskMode,0,1);
    _layout->addWidget(_modelName,0,2);

    _layout->addWidget(_moveTraj,1,0);
    

    
    layoutCameraGroup->addWidget(_cameraCtrl,2,0);
    layoutCameraGroup->addWidget(_cameraCtrlPrevCP,2,1);
    layoutCameraGroup->addWidget(_cameraCtrlNextCP,2,2);
    layoutCameraGroup->addWidget(_cameraCtrlInsertCP,2,3);
    
    
    _layout->addWidget(_cameraModeGroup,2,0,1,3);
    _layout->addWidget(_insertLocalisationCamera,3,0);
    _layout->addWidget(_factor,4,0, 1, 3);
    _layout->addWidget(_viewCP,5,0,1,3);
    //_layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setLayout(_layout);


    
    QItemSelectionModel  * selectModel = _viewCP->selectionModel ();
    connect(selectModel, SIGNAL(selectionChanged ( const QItemSelection  &, const QItemSelection  &) ), 
            this, SLOT(viewCPselectionChanged ( const QItemSelection  &, const QItemSelection  & )));
    connect(pPtyTrajectory->getPtySelection().get(), SIGNAL(selectionChanged()), this, SLOT(ptySelectSelectionChanged()));
    
    connect(_insertLocalisationCamera, SIGNAL(released ()), this, SLOT(insertLocalisationCamera()));
    connect(_editTraj, SIGNAL(clicked  (bool)), this, SLOT(swapEditMode(bool)));
    connect(_cameraCtrl, SIGNAL(clicked  (bool)), this, SLOT(cameraCtrlMode(bool)));
    connect(_moveTraj, SIGNAL(clicked  (bool)), this, SLOT(moveTrajectory()));
    connect(_cameraCtrlNextCP, SIGNAL(clicked  (bool)), this, SLOT(cameraCtrlSelectNextCP()));
    connect(_cameraCtrlPrevCP, SIGNAL(clicked  (bool)), this, SLOT(cameraCtrlSelectPrevCP()));
    connect(_cameraCtrlInsertCP, SIGNAL(clicked  (bool)), this, SLOT(cameraCtrlSelectInsertCP()));
    
    getWidgetsForUndoRedo().push_back(_taskMode);
    getWidgetsForUndoRedo().push_back(_modelName);
    getWidgetsForUndoRedo().push_back(_factor);
    getWidgetsForUndoRedo().push_back(_editTraj);

    PropertyWidget::setupUi();
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::readProperty()
{
    LOG_FCT_NAME

    Ptr<PtyTrajectory> pPtyTrajectory = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());

    _factor->setSingleValue(pPtyTrajectory->_factor);
/*
    const Core::Trajectory& traj = pPtyTrajectory->getTrajectory();
*/
    _taskMode->selectItem(trajTaskModeToString(pPtyTrajectory->_mode));
    _modelName->setText(pPtyTrajectory->_modelName);

    ((TableCheckPointModel*)(_viewCP->model()))->emitAllDataChanged();

    _editTraj->setChecked(pPtyTrajectory->_editMode);
    swapEditMode(_editTraj->isChecked());

    if (pPtyTrajectory->_editMode==true)
    {
        _cameraCtrl->setChecked(pPtyTrajectory->isInCameraCtrl());
        swapCameraCtrlMode(pPtyTrajectory->isInCameraCtrl());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::writeProperty(QWidget* pWidget)
{
    LOG_FCT_NAME

    Ptr<PtyTrajectory> pPtyTrajectory = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());
    
    if (pWidget == NULL || pWidget == _modelName)
    {
        String modelName;
        _modelName->getText(modelName);
         pPtyTrajectory->updateModel(modelName); 
    }

    if (pWidget == NULL || pWidget == _taskMode)
    {
        pPtyTrajectory->_mode = stringToTrajTaskMode(_taskMode->selectedText());
    }

    if (pWidget == NULL || pWidget == _factor)
    {
        _factor->getSingleValue(pPtyTrajectory->_factor);
        if (pPtyTrajectory->_factor<f_COMMON_EPSILON)
        {
            pPtyTrajectory->_factor = f_COMMON_EPSILON;
        }
    }

    if (pWidget == NULL || pWidget == _editTraj)
    {
        pPtyTrajectory->setEditMode(_editTraj->isChecked());
    }
/*
    if (pWidget == NULL || pWidget == _cameraCtrl)
    {
        bool startCameraCtrl = _cameraCtrl->isChecked();
        if (startCameraCtrl == true)
        {
            pPtyTrajectory->startCameraCtrlMode();
        }
        else
        {
            pPtyTrajectory->endCameraCtrlMode();
        }
        
    }*/

    ((TableCheckPointModel*)(_viewCP->model()))->emitAllDataChanged();
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::ptySelectSelectionChanged()
{
    QItemSelectionModel  * selectModel = _viewCP->selectionModel ();
    disconnect(selectModel, SIGNAL(selectionChanged ( const QItemSelection  &, const QItemSelection  &) ), 
            this, SLOT(viewCPselectionChanged ( const QItemSelection  &, const QItemSelection  & )));

    Ptr<PtyTrajectory> pPtyTrajectory = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());
    Ptr<PropertySelection> pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyTrajectory->getPtySelection());

    const Core::List<Ptr<Property> >& ptiesSelected = pPtyS->getSelectedPties();
    const Core::List<Ptr<Property> >& ptiesSelectable = pPtyTrajectory->getPtiesContentSelectable();

    Core::List<int32>    indexSelected;
    for (int32 iSelected=0; iSelected<ptiesSelected.size(); ++iSelected)
    {
        for (int32 iSelectable=0; iSelected<ptiesSelectable.size(); ++iSelectable)
        {
            if(ptiesSelected[iSelected]->getId() == ptiesSelectable[iSelectable]->getId())
            {
                indexSelected.push_back(iSelectable);
                break;
            }
        }
    }


   // selectModel->reset();
    selectModel->clearSelection();
/*
    selectModel->select(_modelCP->index(0,0),QItemSelectionModel::Clear);

    for (int32 iSelectable=0; iSelectable<ptiesSelectable.size(); ++iSelectable)
    {
        selectModel->select(_modelCP->index(iSelectable,0), QItemSelectionModel::Clear | QItemSelectionModel::Rows);
    }*/

    for (int32 ii=0; ii<indexSelected.size(); ++ii)
    {
        selectModel->select(_modelCP->index(indexSelected[ii],0), QItemSelectionModel::Select | QItemSelectionModel::Rows);
    }
    
    connect(selectModel, SIGNAL(selectionChanged ( const QItemSelection  &, const QItemSelection  &) ), 
            this, SLOT(viewCPselectionChanged ( const QItemSelection  &, const QItemSelection  & )));
}
//-----------------------------------------------------------------------------
/*void PtyWidgetTrajectory::ptyTrajectoryChanged()
{
    _modelCP->emitAllDataChanged();
}*/
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::viewCPselectionChanged ( const QItemSelection  & selected, const QItemSelection  & deselected )
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());
    Ptr<PropertySelectionContent> pPtyS = LM_DEBUG_PTR_CAST<PropertySelectionContent>(pPtyTrajectory->getPtySelection());

    disconnect(pPtyTrajectory->getPtySelection().get(), SIGNAL(selectionChanged()), this, SLOT(ptySelectSelectionChanged()));

    bool stopRecording = false;

    if(pPtyTrajectory->getStateRecorder()->isRecording() == false)
    {
        stopRecording = true;
        pPtyTrajectory->_pStateRecorder->startRecording();
    }

    Ptr<IState> pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*pPtyS));

    for (int32 ii=0; ii<deselected.size(); ++ii)
    {
        int32 top = deselected[ii].top();
        int32 bottom = deselected[ii].bottom();

        if (top >=0 && bottom >=0)
        {
            for (int32 iPos=top; iPos<=bottom; ++iPos)
            {
                pPtyS->removeFromSelection(iPos);
            }
        }
    }


    for (int32 ii=0; ii<selected.size(); ++ii)
    {
        int32 top = selected[ii].top();
        int32 bottom = selected[ii].bottom();

        if (top >=0 && bottom >=0)
        {
            for (int32 iPos=top; iPos<=bottom; ++iPos)
            {
                pPtyS->addToSelection(iPos);
            }
        }
    }

    

    Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*pPtyS));
//    pPtyTrajectory->_pStateRecorder->recording(pUndoState, pRedoState);
    pPtyTrajectory->_pStateRecorder->getCurrentChangeSet()->recordUndoState(pUndoState);
    pPtyTrajectory->_pStateRecorder->getCurrentChangeSet()->recordRedoState(pRedoState);

    if (stopRecording==true)
        pPtyTrajectory->_pStateRecorder->stopRecording();

    connect(pPtyTrajectory->getPtySelection().get(), SIGNAL(selectionChanged()), this, SLOT(ptySelectSelectionChanged()));
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::insertLocalisationCamera()
{
    int32 index = _viewCP->currentIndex().row() + 1;
    _modelCP->insertLocalisationCamera(index);
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::swapEditMode(bool checked)
{
    if (checked==true)
    {
        _insertLocalisationCamera->setEnabled(true);
        _viewCP->setEnabled(true);
        _factor->setEnabled(true);
        _cameraCtrl->setEnabled(true);
        //_modelName->setEnabled(true);
        //_taskMode->setEnabled(true);
    }
    else
    {
        _insertLocalisationCamera->setEnabled(false);
        _viewCP->setEnabled(false);
        _factor->setEnabled(false);
        _cameraCtrl->setEnabled(false);
        //_modelName->setEnabled(false);
        //_taskMode->setEnabled(false);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::cameraCtrlMode(bool checked)
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());

    if (checked==true)
    {
        Ptr<PropertySelectionContent> pPtyS = LM_DEBUG_PTR_CAST<PropertySelectionContent>(pPtyTrajectory->getPtySelection());

        int32 nbPtyCP = pPtyS->getSelectedPtiesCount();

        if (nbPtyCP==0)
        {
            QMessageBox::information(NULL, "", "You must select one Check Point");
            _cameraCtrl->setChecked(false);
        }
        else if (nbPtyCP>1)
        {
            QMessageBox::information(NULL, "", "Too Many Check Points Are Selected");
            _cameraCtrl->setChecked(false);
        }
        else
        {
            //commitPropertyChange(_cameraCtrl);
            pPtyTrajectory->startCameraCtrlMode();
            swapCameraCtrlMode(pPtyTrajectory->isInCameraCtrl());
        }
    }
    else
    {
        //commitPropertyChange(_cameraCtrl);
        pPtyTrajectory->endCameraCtrlMode();
        swapCameraCtrlMode(pPtyTrajectory->isInCameraCtrl());
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::swapCameraCtrlMode(bool checked)
{
    if (checked==true)
    {
        _editTraj->setEnabled(false);
        _insertLocalisationCamera->setEnabled(false);
        _factor->setEnabled(false);
        _modelName->setEnabled(false);
        _taskMode->setEnabled(false);
        _viewCP->setEnabled(false);
        _cameraCtrlPrevCP->setEnabled(true);
        _cameraCtrlNextCP->setEnabled(true);
        _cameraCtrlInsertCP->setEnabled(true);
        _cameraCtrlNextCP->setFlat(false);
        _cameraCtrlPrevCP->setFlat(false);
        _cameraCtrlInsertCP->setFlat(false);
    }
    else
    {
        _editTraj->setEnabled(true);
        _insertLocalisationCamera->setEnabled(true);
        _factor->setEnabled(true);
        _modelName->setEnabled(true);
        _taskMode->setEnabled(true); 
        _viewCP->setEnabled(true);
        _cameraCtrlPrevCP->setEnabled(false);
        _cameraCtrlNextCP->setEnabled(false);
        _cameraCtrlInsertCP->setEnabled(false);
        _cameraCtrlNextCP->setFlat(true);
        _cameraCtrlPrevCP->setFlat(true);
        _cameraCtrlInsertCP->setFlat(true);
    }
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::moveTrajectory()
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());

    pPtyTrajectory->moveTrajectory();
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::cameraCtrlSelectNextCP()
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());
    pPtyTrajectory->cameraCtrlSelectNextCP();
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::cameraCtrlSelectPrevCP()
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());
    pPtyTrajectory->cameraCtrlSelectPrevCP();
}
//-----------------------------------------------------------------------------
void PtyWidgetTrajectory::cameraCtrlSelectInsertCP()
{
    Ptr<PtyTrajectory> pPtyTrajectory   = LM_DEBUG_PTR_CAST<PtyTrajectory>(getDataProxy()->getProperty());

    //pPtyTrajectory->getStateRecorder()->stopRecording();

    pPtyTrajectory->cameraCtrlEndAction();
    _viewCP->insertRow();


    int32 oldIndexCP = pPtyTrajectory->findIndexWithId(pPtyTrajectory->_camModeCheckPointId);
    LM_ASSERT(oldIndexCP>=0);

    int32 newIndexCP = oldIndexCP + 1;
    if (newIndexCP >= pPtyTrajectory->getPtiesContentSelectable().size())    newIndexCP = 0;

    pPtyTrajectory->cameraCtrlStartAction(pPtyTrajectory->getPtiesContentSelectable()[newIndexCP]->getId());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI