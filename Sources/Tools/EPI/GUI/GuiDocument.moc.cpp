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
#include "GuiDocument.moc.h"

#include <QTimer>
#include <QMessageBox>
#include <QAction>

#include <Universe/Tools.h>

#include <EPI/Constants.h>

#include <EPI/DocumentRenderer.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/DocumentScript.moc.h>
#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/Properties/PtyNode.moc.h>
#include <EPI/Document/Properties/PtyMaterial.moc.h>
#include <EPI/Document/Properties/PtyNodePlayerPosition.moc.h>
#include <EPI/GUI/GuiContext.moc.h>
#include <EPI/GUI/WorldOverlay/WorldOverlay.h>
#include <EPI/GUI/Viewport/CameraManager.h>
#include <EPI/GUI/Viewport/WorldViewport.moc.h>
#include <EPI/GUI/Viewport/Tools/ViewportTool.moc.h>

#include <EPI/GUI/Viewport/Tools/TransformViewportTool.moc.h>
#include <EPI/GUI/Viewport/Tools/CopyLocation.moc.h>
#include <EPI/GUI/Viewport/Tools/CopyCameraLocation.moc.h>
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>
#include <EPI/Document/PropertySelection.moc.h>

#include <EPI/GUI/Viewport/INavigationStyle.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class GuiDocument::implementation
{
public:

    implementation();
    virtual ~implementation();

    int32 getRendererId() const {return _idRenderer;}
    int32 getID() {return _id;}

    const Ptr<DocumentBase>& getDocument() const;
    Ptr<DocumentBase>& getDocument();

    const Ptr<Universe::World>&         getGuiWorld     () const;
    Ptr<Universe::World>                getWorldOverlay (const DrawContext& dc) const;
    const Ptr<WorldOverlay>&            getWorldOverlay2() const;
    Core::List<Ptr<Universe::World> >   getWorlds       (const DrawContext& dc) const;
    
    Ptr<GuiContext>&                    getGuiContext();
    const Ptr<GuiContext>&              getGuiContext() const;


    const Ptr<IViewportTool>&           getViewportTool     ();
    Ptr<Property>                       getPtyForEdition    () const;
   

    QAction* getActionExtend();

    void setViewportTool(const Ptr<IViewportTool>& pVTool);

    void startAnimate();
    void stopAnimate();

    void generateInfo(const Ptr<ImportInfo> & pImportInfo);
    void zoomExtent();
    void zoomExtendPlayerPosition(const Ptr<Property>& pPty);

    void SelectionChanged(const Core::List<Ptr<Property> >&  pties);

public:
    int32               _id;
    int32               _idRenderer;
    bool                _isModified;
    bool                _isAnimated;

    Ptr<DocumentBase>   _pDoc;
    Ptr<GuiContext>     _pGuiContext;
    Ptr<WorldOverlay>   _pWorldOverlay;
    Ptr<IViewportTool>  _pActiveTool;

    double              _elapsed;
    Core::TimeValue     _lastTime;
    
    QAction*            _pActionZoomExtent;
    Core::List<Ptr<ISpyTask> >  spyTasks;
    
    //counter de gui document permettant d'affecter l'identifiant
    static int32 countGuiDocument;
    bool makeSleep;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
int32 GuiDocument::implementation::countGuiDocument = 0;
//-----------------------------------------------------------------------------
GuiDocument::implementation::implementation():
    _id(countGuiDocument++),
    _elapsed(0.),
    _lastTime(Core::Clock::universalTime()),
    makeSleep(true)
{
    
    
 
}
//-----------------------------------------------------------------------------
GuiDocument::implementation::~implementation()
{
    delete _pActionZoomExtent;

    _pGuiContext   = null;
    spyTasks.clear();
    _pWorldOverlay = null;
    _pActiveTool   = null;

    _pDoc          = null;
}
//-----------------------------------------------------------------------------
const Ptr<DocumentBase>& GuiDocument::implementation::getDocument() const
{
    return _pDoc;
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase>& GuiDocument::implementation::getDocument()
{
    return _pDoc;
}
//-----------------------------------------------------------------------------
const Ptr<WorldOverlay>& GuiDocument::implementation::getWorldOverlay2() const
{
    return _pWorldOverlay;
}
//-----------------------------------------------------------------------------
Ptr<GuiContext>& GuiDocument::implementation::getGuiContext()
{
    return _pGuiContext;
}
//-----------------------------------------------------------------------------
const Ptr<GuiContext>& GuiDocument::implementation::getGuiContext() const
{
    return _pGuiContext;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& GuiDocument::implementation::getGuiWorld() const
{
    return _pDoc->getWorld();
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> GuiDocument::implementation::getWorldOverlay(const DrawContext& dc) const
{
    if (_pActiveTool != null)
    {
        Ptr<Universe::World> pTest = LM_DEBUG_PTR_CAST<Universe::World>(_pWorldOverlay);
        _pActiveTool->guiConfigure(pTest, dc);
    }

    return _pWorldOverlay;
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::setViewportTool(const Ptr<IViewportTool>& pVTool)
{
    _pActiveTool = pVTool;
}
//-----------------------------------------------------------------------------
const Ptr<IViewportTool>& GuiDocument::implementation::getViewportTool()
{
    return _pActiveTool;
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::generateInfo(const Ptr<ImportInfo> & pImportInfo)
{
    
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Universe::World> > GuiDocument::implementation::getWorlds(const DrawContext& dc) const
{
    Core::List<Ptr<Universe::World> > worlds;
        worlds.push_back(getGuiWorld());

    if (_pDoc->isEditing() == true)
    {
        worlds.push_back(getWorldOverlay(dc));
        worlds.push_back(_pDoc->getWorldInfoContent());
    }

    Ptr<Universe::World> pWorldDeco = getDocument()->getWorldDeco();
    if (pWorldDeco !=null)
    {
        worlds.push_back(pWorldDeco);
    }

    return worlds;
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::startAnimate()
{
    _isAnimated = true;
    _elapsed = 0.;
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::stopAnimate()
{
    _isAnimated = false;
    _elapsed = 0.;
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::SelectionChanged(const Core::List<Ptr<Property> >&  pties)
{
    Ptr<PtyDocEdition> pPtyDocEdit = LM_DEBUG_PTR_CAST<PtyDocEdition> (getDocument()->getPropertyDocumentEdition());
    Ptr<PropertySelection> pPtySelection = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyDocEdit->getPtySelection());
    getWorldOverlay2()->updateSelection(pPtySelection);
}
//-----------------------------------------------------------------------------
void GuiDocument::implementation::zoomExtendPlayerPosition(const Ptr<Property>& pPty)
{
	if (pPty!=null)
	{
		LM_ASSERT(pPty->getCalssName() == PTYNODE_PLAYER_POSITION);

		Ptr<PtyNodePlayerPosition>  pPtyPP = LM_DEBUG_PTR_CAST<PtyNodePlayerPosition>(pPty);
		Ptr<PtyTransformNode>       pPtyTN = LM_DEBUG_PTR_CAST<PtyTransformNode>(pPtyPP->getPtyTransform());

		Ptr<Property> pPtyEdit = getPtyForEdition();

		 WorldViewport* pViewportTest = _pGuiContext->getCurrentViewport()->getView();

		float distancePlayer = pPtyPP->getPlayersDistance();
		Core::AABoxf boxNode = pPtyPP->getBoundingBox();

		Core::Vector3f centerPlayer = boxNode.getCenter();
	   // Core::Vector3f posPlayer1   = centerPlayer + pPtyTN->getWorldOrientation().apply(Core::Vector3f(0,0,-distancePlayer/2.f));
		Core::Vector3f posPlayer1   = centerPlayer + pPtyTN->getWorldAxis() * (Core::Vector3f(0,0,-distancePlayer/2.f));

		Ptr<Universe::Node> pCam = pViewportTest->getNodeCamera();

		//Core::Quaternionf quatDir (Core::deg2rad(-15), Core::deg2rad(-28), Core::deg2rad(0));
		Core::Matrix3f matDir;
		matDir.setRotation(Core::Orientationf(Core::deg2rad(15), Core::deg2rad(-28), Core::deg2rad(0)));
	//    Core::Vector3f camPos = posPlayer1 + pPtyTN->getWorldOrientation().apply(Core::Vector3f(3.1f, 0.9f, -2.7f));
		Core::Vector3f camPos = posPlayer1 + pPtyTN->getWorldAxis() * (Core::Vector3f(3.1f, 0.9f, -2.7f));
		Core::Vector3f camDir = matDir * pPtyTN->getWorldDirection();


	    
		pCam->setUpVector(Core::Vector3f(0,1,0));
		pCam->setWorldDirection(camDir);
		pCam->setWorldPosition(camPos);
	}
}


void GuiDocument::implementation::zoomExtent()
{
    if (_pDoc!=null)
    {
        Core::AABoxf box;
        box.clear();

        Ptr<Property> pPtyEdit = getPtyForEdition();

        Core::List<Ptr<Property> >  ptiesSelected;

        if (pPtyEdit!=null && pPtyEdit->isEditable() == true)   ptiesSelected = LM_DEBUG_PTR_CAST<PropertySelection> (pPtyEdit->getPtySelection())->getSelectedPties();
        else                                                    ptiesSelected = _pDoc->getSelectedProperties();


        WorldViewport* pViewportTest = _pGuiContext->getCurrentViewport()->getView();
        if (pViewportTest!=NULL && ptiesSelected.size() == 1 && ptiesSelected[0]->getCalssName() == PTYNODE_PLAYER_POSITION && 
            pViewportTest->getNavigationStyle()->getType() == NAVIGATION_QUAKE)
        {
            zoomExtendPlayerPosition(ptiesSelected[0]);
        }
        else if(ptiesSelected.size() == 0 && pViewportTest!=NULL && pViewportTest->getNavigationStyle()->getType() == NAVIGATION_QUAKE && 
              getDocument()->getActivePlayerPosition() != null)
        {
            zoomExtendPlayerPosition(getDocument()->getActivePlayerPosition());
        }
        else if (_pDoc->getType() == DOC_SCRIPT && LM_DEBUG_PTR_CAST<DocumentScript>(_pDoc)->isInInEditWorld() == false)
        {
            zoomExtendPlayerPosition(getDocument()->getActivePlayerPosition());
        }
        else
        {
            if (ptiesSelected.size() == 0)
            {
                getHierarchyBox(box, *_pDoc->getWorld());
            }
            else
            {
                
                for (int32 ii=0; ii<ptiesSelected.size(); ++ii)
                {
                    if (ptiesSelected[ii]->getCalssName() == PTYNODE_CAMERA_CLASS || ptiesSelected[ii]->getCalssName() == PTYNODE_SOUND)
                    {
                        Ptr<PtyNode> pPtyNode = LM_DEBUG_PTR_CAST<PtyNode>(ptiesSelected[ii]);
                        Core::Vector3f pos = pPtyNode->getUniverseNode()->getWorldPosition();
                        Core::AABoxf boxNode (pos-Core::Vector3f(Core::ONE), pos+Core::Vector3f(Core::ONE));
                        box.setUnion(boxNode);
                    }
                    else if(ptiesSelected[ii]->hasFlag(FLAG_PTY_NODE)==true)
                    {
                        Ptr<PtyNode> pPtyNode = LM_DEBUG_PTR_CAST<PtyNode>(ptiesSelected[ii]);

                        Core::AABoxf boxNode = pPtyNode->getBoundingBox();
                        box.setUnion(boxNode);
                    }
                    else if (ptiesSelected[ii]->getCalssName() == PTY_MATERIAL)
                    {
                        Ptr<PtyMaterial> pPtyMat = LM_DEBUG_PTR_CAST<PtyMaterial>(ptiesSelected[ii]);
                        Core::AABoxf boxNode = pPtyMat->getBoundingBox();
                        box.setUnion(boxNode);
                    }
                    else
                    {
                        getHierarchyBox(box, *_pDoc->getWorld());
                        break;
                    }
                }
            }

            Core::Vector3f boxSize = box.getSize();

            if (boxSize.isValid() == true)
            {
                WorldViewport* pViewport = _pGuiContext->getCurrentViewport()->getView();

                if (pViewport!=NULL)
                {
                    pViewport->getNavigationStyle()->zoomExtend(box, pViewport->getNodeCamera());
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
QAction* GuiDocument::implementation::getActionExtend()
{
    return _pActionZoomExtent;
}
//-----------------------------------------------------------------------------
Ptr<Property> GuiDocument::implementation::getPtyForEdition() const
{
    return _pDoc->getPtyForEdition();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GuiDocument::GuiDocument() :
    _impl(new implementation())
{
    LOG_FCT_NAME

    _pTimer = new QTimer(this); 
    connect(_pTimer, SLOT(start()), this, SLOT(animate()));
    connect(_pTimer, SIGNAL(timeout()), this, SLOT(animate()));
    connect(_pTimer, SLOT(stop()), this, SLOT(animate()));

    ////////
    //_impl->_isAnimated = true;
    //_pTimer->start();
    //////
    
}
//-----------------------------------------------------------------------------
GuiDocument::~GuiDocument()
{
    LOG_FCT_NAME

    _pTimer->stop();
    delete _pTimer;
    delete _impl;
}
//-----------------------------------------------------------------------------
void GuiDocument::setDocument(const Ptr<DocumentBase>& pDoc)
{
    _impl->_pDoc = pDoc;

    const Ptr<GuiDescription>& guiDesc = _impl->_pDoc->getGuiDescription();

    _impl->_pWorldOverlay = Ptr<WorldOverlay>( new WorldOverlay(_impl->_pDoc->getDocumentRenderer()->getRenderer(),  _impl->_pDoc->getDocumentRenderer()->getVFS()));

    if (_impl->_pDoc->getType() == DOC_MODEL)
    {
        _impl->_pWorldOverlay->setSelectedBoundingBoxVisible(false);
    }



    _impl->_pActionZoomExtent = new QAction("ZoomExtent", NULL);
    
    _impl->_pActionZoomExtent->setShortcut(QKeySequence(Qt::Key_X));

    //_impl->animate();
    animate();


    connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >& )),
        this, SLOT(SelectionChanged(const Core::List<Ptr<Property> >& )));

    connect(getActionExtend(), SIGNAL(triggered()), this, SLOT(zoomExtent()));

    
    Ptr<CameraManager> pCamMng (new CameraManager(_impl->_pWorldOverlay, _impl->_pDoc->getDefaultCameraOrientation()));
    _impl->_pGuiContext = Ptr<GuiContext>(new GuiContext(guiDesc, pCamMng));
}
//-----------------------------------------------------------------------------
const Ptr<DocumentBase>& GuiDocument::getDocument() const
{
    LOG_FCT_NAME

    return _impl->getDocument();
}
//-----------------------------------------------------------------------------
Ptr<DocumentBase>& GuiDocument::getDocument()
{
    LOG_FCT_NAME

    return _impl->getDocument();
}
//-----------------------------------------------------------------------------
int32 GuiDocument::getID()
{
    LOG_FCT_NAME

    return _impl->getID();
}
//-----------------------------------------------------------------------------
int32 GuiDocument::getRendererId()
{
    LOG_FCT_NAME

    return _impl->getRendererId();
}
//-----------------------------------------------------------------------------
bool GuiDocument::canClose(bool userInteraction)
{
    return _impl->_pDoc->canClose(userInteraction);
}
//-----------------------------------------------------------------------------
const Ptr<Universe::World>& GuiDocument::getGuiWorld() const
{
    LOG_FCT_NAME

    return _impl->getGuiWorld();
}
//-----------------------------------------------------------------------------
Ptr<Universe::World> GuiDocument::getWorldOverlay(const DrawContext& dc) const
{
    LOG_FCT_NAME

    return _impl->getWorldOverlay(dc);
}
//-----------------------------------------------------------------------------
const Ptr<WorldOverlay>& GuiDocument::getWorldOverlay2() const
{
    LOG_FCT_NAME

    return _impl->getWorldOverlay2();
}
//-----------------------------------------------------------------------------
void GuiDocument::prepareWorldOverlayForRendering(const DrawContext& dc) const
{
    _impl->_pWorldOverlay->configureOverlayForRendering(dc.pCamera);
}
//-----------------------------------------------------------------------------
Core::List<Ptr<Universe::World> > GuiDocument::getWorlds(const DrawContext& dc) const
{
    LOG_FCT_NAME

    
    return _impl->getWorlds(dc);
}
//-----------------------------------------------------------------------------
Ptr<GuiContext>& GuiDocument::getGuiContext()
{
    LOG_FCT_NAME

    return _impl->getGuiContext();
}
//-----------------------------------------------------------------------------
const Ptr<GuiContext>& GuiDocument::getGuiContext() const
{
    LOG_FCT_NAME

    return _impl->getGuiContext();
}
//-----------------------------------------------------------------------------
const Ptr<IViewportTool>& GuiDocument::getViewportTool()
{
    LOG_FCT_NAME

    return _impl->getViewportTool();
}
//-----------------------------------------------------------------------------
Ptr<Property> GuiDocument::getPtyForEdition() const
{
    LOG_FCT_NAME

    return _impl->getPtyForEdition();
}
//-----------------------------------------------------------------------------  
QAction* GuiDocument::getActionExtend()
{
    LOG_FCT_NAME

    return _impl->getActionExtend();
}
//-----------------------------------------------------------------------------
void GuiDocument::setViewportTool(const Ptr<IViewportTool>& pVTool)
{
    LOG_FCT_NAME

    _impl->setViewportTool(pVTool);
    emit signalActivatedViewportTool(getViewportTool());
}
//-----------------------------------------------------------------------------
void GuiDocument::startAnimate()
{
    LOG_FCT_NAME

    _pTimer->start();
    _impl->startAnimate();
    emit playStopChanged();
}
//-----------------------------------------------------------------------------
void GuiDocument::stopAnimate()
{
    LOG_FCT_NAME

    _pTimer->stop();
    _impl->stopAnimate();
    emit playStopChanged();
}
//-----------------------------------------------------------------------------
void GuiDocument::animate()
{
    LOG_FCT_NAME

    try
    {
        Core::TimeValue nextTime(Core::Clock::universalTime());
        Core::TimeDuration frameDuration(nextTime - _impl->_lastTime);
        _impl->_elapsed = frameDuration.ftotalSeconds();

        _impl->_pDoc->getWorld()->getNodeListener()->setGain(MiscConst::sSoundGlobalGain);
        _impl->_pDoc->getWorld()->update(_impl->_elapsed);

        Ptr<Universe::World> pWDeco = _impl->_pDoc->getWorldDeco();
            if (pWDeco!=null)
            {
                pWDeco->getNodeListener()->setGain(MiscConst::sSoundGlobalGain);
                pWDeco->update(_impl->_elapsed);
            }

        Ptr<Universe::World> pWInfo = _impl->_pDoc->getWorldInfoContent();
            if (pWInfo!=null)
            {
                pWInfo->getNodeListener()->setGain(MiscConst::sSoundGlobalGain);
                pWInfo->update(_impl->_elapsed);
            }

        _impl->getWorldOverlay2()->update(_impl->_elapsed);  

        int32 nbRemove = 0;
        for (int32 iSP=0; iSP<_impl->spyTasks.size(); iSP++)
        {
            if (_impl->spyTasks[iSP-nbRemove]->update() == true)
            {
                _impl->spyTasks.erase(_impl->spyTasks.begin()+(iSP-nbRemove));
                nbRemove++;
            }
        }

        if (_impl->makeSleep == true)
        {
            Core::Thread::sleep(5);
        }

        _impl->_lastTime = nextTime;
    }
    catch(Core::Exception & exception)
    {
        ERR << exception.getMessage() << L"\n";
        ERR << exception.getCallStack() << L"\n";
        QMessageBox::critical( NULL, "Nyx", Core::String8(exception.getMessage()).c_str());

        if (getDocument()->isEditing() == false)
        {
            getDocument()->runStopScript();
        }
        else
        {
            stopAnimate();
        }
    }

    emit reDrawMe();
}
//-----------------------------------------------------------------------------
void GuiDocument::generateInfo(const Ptr<ImportInfo> & pImportInfo)
{
    LOG_FCT_NAME

    _impl->generateInfo(pImportInfo);
    emit generate(pImportInfo);
}
//-----------------------------------------------------------------------------
void GuiDocument::zoomExtent()
{
    LOG_FCT_NAME

    _impl->zoomExtent();
}
//-----------------------------------------------------------------------------
void GuiDocument::SelectionChanged(const Core::List<Ptr<Property> >&  pties)
{
    LOG_FCT_NAME

    _impl->SelectionChanged(pties);
}
//-----------------------------------------------------------------------------
bool GuiDocument::isAnimated() const
{
    LOG_FCT_NAME

    return _impl->_isAnimated;
}
//-----------------------------------------------------------------------------
void GuiDocument::documentContentIsModified()
{
    LOG_FCT_NAME

    _impl->_pDoc->contentIsModified();
    if (isAnimated() == false)
    {
        emit reDrawMe();
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::addSpyTask(const Ptr<ISpyTask>& sp)
{
    _impl->spyTasks.push_back(sp);
}
//-----------------------------------------------------------------------------
void GuiDocument::setMakeSleep(bool make)
{
    _impl->makeSleep = make;
}
//-----------------------------------------------------------------------------
void GuiDocument::selectMultiTransformVTool()
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());

    if (getViewportTool()!=null && getViewportTool()->getType()==MULTI_TRANSFORM_TOOL)
    {
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;
        
        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

        EActionCenterType center      = getGuiContext()->getMemoViewportTool().vToolActionCenter;
        EOrientationType  orientation = getGuiContext()->getMemoViewportTool().vToolOrientationType;

        if (pPtyEdit==null)
        {
            pVTool = Ptr<IViewportTool>(new MoveRotateScaleTool(getDocument()->getStateRecorder(), getDocument()->getSelectedProperties(), center, orientation));
                

            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new MoveRotateScaleTool(getDocument()->getStateRecorder(), pPtyS->getSelectedPties(), center, orientation));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<MoveRotateScaleTool>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::selectMoveVTool()
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());

    if (getViewportTool()!=null && getViewportTool()->getType()==MOVE_TOOL)
    {
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;
        

        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();
        EActionCenterType center      = getGuiContext()->getMemoViewportTool().vToolActionCenter;
        EOrientationType  orientation = getGuiContext()->getMemoViewportTool().vToolOrientationType;

        if (pPtyEdit==null)
        {
            pVTool = Ptr<IViewportTool>(new MoveTool(getDocument()->getStateRecorder(), getDocument()->getSelectedProperties(), center, orientation));
                

            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new MoveTool(getDocument()->getStateRecorder(), pPtyS->getSelectedPties(), center, orientation));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<MoveTool>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::selectRotateVTool()
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());


    if (getViewportTool()!=null && getViewportTool()->getType()==ROTATE_TOOL)
    {
        
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;

        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

        EActionCenterType center      = getGuiContext()->getMemoViewportTool().vToolActionCenter;
        EOrientationType  orientation = getGuiContext()->getMemoViewportTool().vToolOrientationType;

        Ptr<Renderer::IFont> pFont = getDocument()->getDocumentRenderer()->getDefaultFont();
        Ptr<Renderer::IHUD> pHUD = getDocument()->getDocumentRenderer()->getRenderer()->createHUD();

        if (pPtyEdit==null)
        {


            pVTool = Ptr<IViewportTool>(new RotateTool(getDocument()->getStateRecorder(), getDocument()->getSelectedProperties(), pFont, pHUD, center, orientation));
                

            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new RotateTool(getDocument()->getStateRecorder(), pPtyS->getSelectedPties(), pFont, pHUD, center, orientation));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<RotateTool>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::selectScaleVTool()
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());

    if (getViewportTool()!=null && getViewportTool()->getType()==SCALE_TOOL)
    {
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;

        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

        if (pPtyEdit==null)
        {

            pVTool = Ptr<IViewportTool>(new ScaleTool(getDocument()->getStateRecorder(), getDocument()->getSelectedProperties()));
                

            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new ScaleTool(getDocument()->getStateRecorder(), pPtyS->getSelectedPties()));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<ScaleTool>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::selectCopyLocationVTool()
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());

    if (getViewportTool()!=null && getViewportTool()->getType()==COPY_LOCATION_TOOL)
    {
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;

        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

        if (pPtyEdit==null)
        {
            pVTool = Ptr<IViewportTool>(new CopyLocation(getDocument()->getStateRecorder(), *getDocument(), getDocument()->getSelectedProperties()));
                
            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new CopyLocation(getDocument()->getStateRecorder(), *getDocument(), pPtyS->getSelectedPties()));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<CopyLocation>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::selectCopyCameraLocationVTool(bool makeAttach)
{
    getGuiContext()->getMemoViewportTool().stockViewportToolConfig(getViewportTool());

    if (getViewportTool()!=null && getViewportTool()->getType()==COPY_CAMERA_LOCATION_TOOL)
    {
        setViewportTool(null);
    }
    else
    {
        Ptr<IViewportTool> pVTool;

        Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

        if (pPtyEdit==null)
        {
            pVTool = Ptr<IViewportTool>(new CopyCameraLocation(getDocument()->getStateRecorder(), *this, getDocument()->getSelectedProperties(), makeAttach));
                
            connect(getDocument().get(), SIGNAL(currentSelectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));
            
            connect(getDocument().get(), SIGNAL(documentContentUpdated()),
                    pVTool.get(), SLOT(updateViewportTool()));

            setViewportTool(pVTool);
        }
        else
        {
            Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
            pVTool = Ptr<IViewportTool>(new CopyCameraLocation(getDocument()->getStateRecorder(), *this, pPtyS->getSelectedPties(), makeAttach));

            connect(pPtyEdit.get(), SIGNAL(selectionChanged(const Core::List<Ptr<Property> >&)),
                        pVTool.get(), SLOT(selectionChanged(const Core::List<Ptr<Property> >&)));

            connect(pPtyEdit.get(), SIGNAL(propertyChanged()),
                    pVTool.get(), SLOT(updateViewportTool()));

            LM_DEBUG_PTR_CAST<CopyCameraLocation>(pVTool)->setContainer(pPtyEdit);
            setViewportTool(pVTool);
        }
    }
}
//-----------------------------------------------------------------------------
void GuiDocument::copyCameraLocationToCurrentSelection()
{
    WorldViewportContainer* pVContainer = getGuiContext()->getCurrentViewport();

    if (pVContainer!=NULL)
    {
        WorldViewport* pView = pVContainer->getView();
        if (pView!=NULL)
        {
            Ptr<Universe::NodeCamera> pNCam = pView->getNodeRenderCamera();
            if (pNCam!=null)
            {
                Ptr<Property> pPtyEdit = getDocument()->getPtyForEdition();

                if (pPtyEdit==null)
                {
                    CopyCameraLocation::toCameraLocation(10.f, pNCam, getDocument()->getSelectedProperties(), getDocument()->getStateRecorder(), true, true);
                }
                else
                {
                    Ptr<PropertySelection>  pPtyS = LM_DEBUG_PTR_CAST<PropertySelection>(pPtyEdit->getPtySelection());
                    CopyCameraLocation::toCameraLocation(10.f, pNCam, pPtyS->getSelectedPties(), getDocument()->getStateRecorder(), true, true);
                    pPtyEdit->updateData();
                }
            }
        }
    }  
}
//-----------------------------------------------------------------------------
void GuiDocument::engageCameraLocationAttach()
{
    selectCopyCameraLocationVTool(true);
}
//-----------------------------------------------------------------------------
void GuiDocument::restart()
{
    getDocument()->restart();
}
//-----------------------------------------------------------------------------
void GuiDocument::stop()
{
    getDocument()->stop();
}
//-----------------------------------------------------------------------------
void GuiDocument::activeDesactiveIHM()
{
    getDocument()->activeDesactiveIHM();
    getGuiContext()->setCurrentViewportActiveDesactiveShowIHM();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

bool SpyTaskForEndTask::update()
{
    if (pTask->isFinished() == true)
    {
        (inst->*call)(pTask);
        return true;
    }

    return false;
}
} // namespace EPI

