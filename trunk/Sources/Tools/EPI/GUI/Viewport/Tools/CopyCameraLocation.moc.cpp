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
#include "CopyCameraLocation.moc.h"

#include <QtToolbox/SingleSlidingValue.moc.h>
#include <EPI/Document/Property.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/StateRecorder.h>
#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Gui/GuiContext.moc.h>
#include <EPI/Gui/Viewport/WorldViewport.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Constants.h>


#include <QPushButton>

namespace EPI
{
Ptr<PtyTransform> getPtyTransformFromPtyTest2(const Ptr<Property>& pPty)
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
struct CopyCameraLocation::implementation
{
    CopyCameraLocation::implementation(const GuiDocument& gDoc) : _gDoc(gDoc){}

public:
    Core::List<Ptr<Property> >  _pties;
    Ptr<Property>               _pContainer;
    const GuiDocument&          _gDoc;
    Core::Vector3f              _pivotInWorldAxis;
    Core::Vector3f              _centerBoundingBoxInWorldAxis;
    Core::Matrix3f              _axis;
    float                       _distance;
    bool                        _lockPtiesToCamera;
    Core::List<Ptr<Property> > _ptyTransformForUndo;
};
//-----------------------------------------------------------------------------          
//-----------------------------------------------------------------------------   
CopyCameraLocation::CopyCameraLocation(const Ptr<StateRecorder>& pStateRecorder, const GuiDocument& gDoc, const Core::List<Ptr<Property> > pties, bool attach):
    IViewportTool(pStateRecorder),
    _impl(new CopyCameraLocation::implementation (gDoc))
{
    _impl->_distance = 10;
    _impl->_lockPtiesToCamera = false;
    updatePropertiesTargets(pties);
    makeAttach(attach);
}
//----------------------------------------------------------------------------- 
CopyCameraLocation::~CopyCameraLocation()
{
    makeAttach(false);
    delete _impl;
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::computeActionCenters()
{
    _impl->_pivotInWorldAxis.setZero();
    _impl->_centerBoundingBoxInWorldAxis.setZero();
    
    for (int32 ii=0; ii<_impl->_pties.size(); ++ii)
    {
        _impl->_pivotInWorldAxis               += LM_DEBUG_PTR_CAST<PtyTransform>(_impl->_pties[ii])->getWorldPosition();
        _impl->_centerBoundingBoxInWorldAxis   += LM_DEBUG_PTR_CAST<PtyTransform>(_impl->_pties[ii])->getCenterBoundingBoxInWorldAxis();
    }

    _impl->_pivotInWorldAxis               /= _impl->_pties.size();
    _impl->_centerBoundingBoxInWorldAxis   /= _impl->_pties.size();
}
//-----------------------------------------------------------------------------
void CopyCameraLocation::computeActionOrientations()
{
    if (_impl->_pties.size()> 0)
    {
        _impl->_axis = LM_DEBUG_PTR_CAST<PtyTransform>(_impl->_pties[0])->getWorldAxis();
    }
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::updatePropertiesTargets(const Core::List<Ptr<Property> >& ptiesSelected)
{
    _impl->_pties.clear();

    for (int32 ii=0; ii<ptiesSelected.size(); ++ii)
    {
        Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPtyTest2(ptiesSelected[ii]);

        if (pPtyTransform!=null)
        {
            _impl->_pties.push_back(pPtyTransform);
        }
    }

    _impl->_pties = removeChildren(_impl->_pties);
    
    updateViewportTool();
}
//----------------------------------------------------------------------------- 
Ptr<IViewportToolWidget> CopyCameraLocation::createViewportToolWidget(QWidget * parent)
{
    return Ptr<IViewportToolWidget> (new CopyCameraLocationWidget(*this, parent));
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{

}
//----------------------------------------------------------------------------- 
const Core::List<Ptr<Property> >& CopyCameraLocation::getProperties() const
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
Core::List<Ptr<Property> >& CopyCameraLocation::getProperties()
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::selectionChanged(const Core::List<Ptr<Property> >& pties)
{
    makeAttach(false);
    updatePropertiesTargets(pties);
    emit viewportToolChanged();
}
//----------------------------------------------------------------------------- 
bool CopyCameraLocation::isHighlighted() const
{
    return false;
}
//----------------------------------------------------------------------------- 
bool CopyCameraLocation::isSelected() const
{
    return false;
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::updateViewportTool()
{
    computeActionCenters();
    computeActionOrientations();
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::setContainer(const Ptr<Property>& pContainer)
{
    _impl->_pContainer = pContainer;
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::apply()
{
    Ptr<GuiContext> pGContext = _impl->_gDoc.getGuiContext();
    WorldViewportContainer* pVContainer = pGContext->getCurrentViewport();

    if (pVContainer!=NULL)
    {
        WorldViewport* pView = pVContainer->getView();
        if (pView!=NULL)
        {
            Ptr<Universe::NodeCamera> pNCam = pView->getNodeRenderCamera();
            if (pNCam!=null)
            {
                toCameraLocation(
                    _impl->_distance, 
                    pNCam, 
                    _impl->_pties, 
                    _impl->_gDoc.getDocument()->getStateRecorder(), 
                    false);

                if (_impl->_pContainer!=null)
                {
                    _impl->_pContainer->updateData();
                }
            }
        }
    }
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::updateCameraMove  (Ptr<Universe::NodeCamera> pNCam)
{
    if (_impl->_lockPtiesToCamera == true)
    {
        if (pNCam!=null)
        {
            toCameraLocation(
                _impl->_distance, 
                pNCam, 
                _impl->_pties, 
                _impl->_gDoc.getDocument()->getStateRecorder(), 
                false,
                !_impl->_lockPtiesToCamera);

            if (_impl->_pContainer!=null)
            {
                _impl->_pContainer->updateData();
            }
        }
    }
}
//-----------------------------------------------------------------------------
void CopyCameraLocation::makeAttach(bool doAttach)
{
    if (doAttach == true)
    {
        _impl->_ptyTransformForUndo.clear();
        for (int32 iPty=0; iPty<_impl->_pties.size(); ++iPty)
        {
            if (_impl->_pties[iPty]->hasFlag(FLAG_PTY_TRANSFORM) == true)
            {
                _impl->_ptyTransformForUndo.push_back(_impl->_pties[iPty]->clone());
                _impl->_ptyTransformForUndo.back()->copyId(*_impl->_pties[iPty]);
            }
            else
            {
                Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPtyTest2(_impl->_pties[iPty]);
                if (pPtyTransform!=null)
                {
                    _impl->_ptyTransformForUndo.push_back(_impl->_pties[iPty]->clone());
                    _impl->_ptyTransformForUndo.back()->copyId(*_impl->_pties[iPty]);
                }
            }
        }

        _impl->_lockPtiesToCamera = true;
    }
    else if (_impl->_lockPtiesToCamera == true)
    {
        _impl->_lockPtiesToCamera = false;

        if (_impl->_ptyTransformForUndo.size() > 0)
        {
            Core::List<Ptr<Property> > ptyTransformForRedo;
            for (int32 iPty=0; iPty<_impl->_pties.size(); ++iPty)
            {
                if (_impl->_pties[iPty]->hasFlag(FLAG_PTY_TRANSFORM) == true)
                {
                    ptyTransformForRedo.push_back(_impl->_pties[iPty]->clone());
                    ptyTransformForRedo.back()->copyId(*_impl->_pties[iPty]);
                }
                else
                {
                    Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPtyTest2(_impl->_pties[iPty]);
                    if (pPtyTransform!=null)
                    {
                        ptyTransformForRedo.push_back(_impl->_pties[iPty]->clone());
                        ptyTransformForRedo.back()->copyId(*_impl->_pties[iPty]);
                    }
                }
            }

            if (_impl->_ptyTransformForUndo.size() == ptyTransformForRedo.size())
            {
                Core::List<Ptr<IState> > undoStates;
                for (int32 iPty=0; iPty<_impl->_ptyTransformForUndo.size(); ++iPty)
                {
                    undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyChanged (*(_impl->_ptyTransformForUndo[iPty]))) );
                }

                Core::List<Ptr<IState> > redoStates;
                for (int32 iPty=0; iPty<ptyTransformForRedo.size(); ++iPty)
                {
                    redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyChanged (*(ptyTransformForRedo[iPty]))) );
                }

                _impl->_gDoc.getDocument()->getStateRecorder()->recording(undoStates, redoStates);
            }

            
            _impl->_ptyTransformForUndo.clear();
        }
    }
}
//----------------------------------------------------------------------------- 
void CopyCameraLocation::toCameraLocation(float distance, const Ptr<Universe::NodeCamera>& pNCam, const Core::List<Ptr<Property> > pties, Ptr<StateRecorder> pStateRecorder, bool findPtyTransform, bool makeUndo)
{
    Core::List<Ptr<Property> > ptiesNoChildren;

    if (pNCam->getBaseCamera()->getCameraType() == Renderer::CAMERA_PERSPECTIVE)
    {
        if (findPtyTransform == true)
        {
            for (int32 ii=0; ii<pties.size(); ++ii)
            {
                Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPtyTest2(pties[ii]);

                if (pPtyTransform!=null)
                {
                    ptiesNoChildren.push_back(pPtyTransform);
                }
            }

            ptiesNoChildren = removeChildren(ptiesNoChildren);
        }
        else
        {
            ptiesNoChildren = pties;
        }

        if (ptiesNoChildren.size()>0)
        {
            bool isModified = false;
            bool makeReording = false;
            if (makeUndo == true && pStateRecorder->isRecording() == false)
            {
                makeReording = true;
                beginChangeProperties(pStateRecorder, ptiesNoChildren);
            }

            //////
            Core::Vector3f localPos(0.0f, 0.0f, distance);
            Core::Vector3f worldPositionFinal = pNCam->getLocalToWorldMatrix().apply(localPos);

            Core::Vector3f pivotInWorldAxis (Core::ZERO);
            Core::Vector3f centerBoundingBoxInWorldAxis  (Core::ZERO);
            
            for (int32 ii=0; ii<ptiesNoChildren.size(); ++ii)
            {
                pivotInWorldAxis                    += LM_DEBUG_PTR_CAST<PtyTransform>(ptiesNoChildren[ii])->getWorldPosition();
                centerBoundingBoxInWorldAxis        += LM_DEBUG_PTR_CAST<PtyTransform>(ptiesNoChildren[ii])->getCenterBoundingBoxInWorldAxis();
            }

            pivotInWorldAxis               /= ptiesNoChildren.size();
            centerBoundingBoxInWorldAxis   /= ptiesNoChildren.size();


            Core::Vector3f offset     = worldPositionFinal - centerBoundingBoxInWorldAxis;

            ///////
            for (int32 ii=0; ii<ptiesNoChildren.size(); ++ii)
            {
                Ptr<PtyTransform> pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(ptiesNoChildren[ii]);

                pPtyT->translateInWorldAxis(offset);

                pPtyT->updateData();
                isModified = true;
            }

            if (makeUndo == true &&  makeReording == true)
            {
                if (isModified==true)
                {
                    finishChangeProperties(pStateRecorder, ptiesNoChildren);
                }
                else
                {
                    cancelChangeProperties(pStateRecorder, ptiesNoChildren);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------    
CopyCameraLocationWidget::CopyCameraLocationWidget(CopyCameraLocation& tool, QWidget * parent) :
    IViewportToolWidget(parent),
    _tool(tool)
{
    setupUi();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------  
CopyCameraLocationWidget::~CopyCameraLocationWidget()
{
}
//-----------------------------------------------------------------------------  
IViewportTool& CopyCameraLocationWidget::getViewportTool()
{
    return _tool;
}
//-----------------------------------------------------------------------------  
void CopyCameraLocationWidget::setupUi()
{
    _layout = new QGridLayout(NULL);

    _distance = new QtToolbox::SingleSlidingDouble(this, "distance", 0, 30);
    _apply = new QPushButton("apply", this);
        _apply->setFixedSize(80, 20);

    _lockPtiesToCamera = new QPushButton("attach", this);
    _lockPtiesToCamera->setCheckable(true);

    _layout->addWidget(_distance, 0, 0, Qt::AlignCenter);
    _layout->addWidget(_apply, 1, 0, Qt::AlignCenter);
    _layout->addWidget(_lockPtiesToCamera, 2, 0, Qt::AlignCenter);

    setLayout(_layout);

    connect(_apply, SIGNAL(released()), this, SLOT(applyLocation()));
    connect(_lockPtiesToCamera, SIGNAL(toggled(bool)), this, SLOT(makeAttach(bool)));

    getWidgetsForUndoRedo().push_back(_distance);   
  

    IViewportToolWidget::setupUi();
}
//----------------------------------------------------------------------------- 
void CopyCameraLocationWidget::applyLocation()
{
    _tool.apply();
}
//-----------------------------------------------------------------------------  
void CopyCameraLocationWidget::readViewportTool()
{
    _distance->setSingleValue(_tool._impl->_distance);

    disconnect(_lockPtiesToCamera, SIGNAL(toggled(bool)), this, SLOT(makeAttach(bool)));
    _lockPtiesToCamera->setChecked(_tool._impl->_lockPtiesToCamera);
    connect(_lockPtiesToCamera, SIGNAL(toggled(bool)), this, SLOT(makeAttach(bool)));
}
//-----------------------------------------------------------------------------  
void CopyCameraLocationWidget::writeViewportTool(QWidget* pWidget)
{
    _distance->getSingleValue(_tool._impl->_distance);
}
//----------------------------------------------------------------------------- 
void CopyCameraLocationWidget::makeAttach(bool doAttach)
{
    _tool.makeAttach(doAttach);
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------  
}
