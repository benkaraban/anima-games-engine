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
#include "CopyLocation.moc.h"

#include <QtToolbox/ComboBox.moc.h>
#include <EPI/Document/Property.moc.h>
#include <EPI/Document/DocumentBase.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/Constants.h>


#include <QPushButton>

namespace EPI
{
Ptr<PtyTransform> getPtyTransformFromPtyTest(const Ptr<Property>& pPty)
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
struct GizmoLocation
{
    String          name;
    Core::Vector3f  worldPosition;
    Core::Matrix3f  worldAxis;
};


struct CopyLocation::implementation
{
    CopyLocation::implementation(const DocumentBase& document) : _doc(document){}

public:
    Core::List<Ptr<Property> >  _pties;
    Ptr<Property>               _pContainer;
    Core::List<GizmoLocation>   _locationGizmoDoc;
    Core::List<GizmoLocation>   _locationGizmoWorldDeco;  
    const DocumentBase&         _doc;
    Core::Vector3f  _pivotInWorldAxis;
    Core::Vector3f  _centerBoundingBoxInWorldAxis;
    Core::Matrix3f  _axis;
};
//-----------------------------------------------------------------------------          
//-----------------------------------------------------------------------------   
CopyLocation::CopyLocation(const Ptr<StateRecorder>& pStateRecorder, const DocumentBase& doc, const Core::List<Ptr<Property> > pties):
    IViewportTool(pStateRecorder),
    _impl(new CopyLocation::implementation (doc))
{
    updatePropertiesTargets(pties);

    updateGizmoDocument();
    updateGizmoWorldDeco();

    connect (&_impl->_doc, SIGNAL(documentContentUpdated()), 
             this, SLOT(updateGizmoDocument()));
}
//----------------------------------------------------------------------------- 
CopyLocation::~CopyLocation()
{
    delete _impl;
}
//----------------------------------------------------------------------------- 
void CopyLocation::updateGizmoDocument()
{
    Core::List<Ptr<Property> > ptiesGizmo = _impl->_doc.getAllGizmoContent();
    _impl->_locationGizmoDoc.clear();

    for (int32 ii=0; ii<ptiesGizmo.size(); ii++)
    {
        Ptr<Property> pPtyT = ptiesGizmo[ii]->getPtyTransform();

        if (pPtyT!=null)
        {
            GizmoLocation gl;
                gl.name          = ptiesGizmo[ii]->getName();
                gl.worldPosition = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyT)->getWorldPosition();
                gl.worldAxis     = LM_DEBUG_PTR_CAST<PtyTransform>(pPtyT)->getWorldAxis();
            _impl->_locationGizmoDoc.push_back(gl);
        }
    }

    emit viewportToolChanged();
}
//----------------------------------------------------------------------------- 
void CopyLocation::computeActionCenters()
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
void CopyLocation::computeActionOrientations()
{
    if (_impl->_pties.size()> 0)
    {
        _impl->_axis = LM_DEBUG_PTR_CAST<PtyTransform>(_impl->_pties[0])->getWorldAxis();
    }
}
//----------------------------------------------------------------------------- 
void CopyLocation::updatePropertiesTargets(const Core::List<Ptr<Property> >& ptiesSelected)
{
    _impl->_pties.clear();

    for (int32 ii=0; ii<ptiesSelected.size(); ++ii)
    {
        Ptr<PtyTransform> pPtyTransform = getPtyTransformFromPtyTest(ptiesSelected[ii]);

        if (pPtyTransform!=null)
        {
            _impl->_pties.push_back(pPtyTransform);
        }
    }

    _impl->_pties = removeChildren(_impl->_pties);
    
    updateViewportTool();
}
//----------------------------------------------------------------------------- 
void CopyLocation::updateGizmoWorldDeco()
{
    Ptr<Universe::World> pWorld = const_cast<DocumentBase&>(_impl->_doc).getWorldDeco();

    _impl->_locationGizmoWorldDeco.clear();

    if (pWorld!=null)
    {
        Core::List<Ptr<Universe::Node> > nodesGizmo;
        pWorld->getNodesWithTag(TAG_GIZMO, nodesGizmo);

        for (int32 ii=0; ii<nodesGizmo.size(); ii++)
        {
            GizmoLocation gl;
                gl.name          = nodesGizmo[ii]->getName();
                gl.worldPosition = nodesGizmo[ii]->getWorldPosition();
                gl.worldAxis     = nodesGizmo[ii]->getLocalToWorldMatrix().getRotation();
            _impl->_locationGizmoWorldDeco.push_back(gl);
        }
    }
    emit viewportToolChanged();
}
//----------------------------------------------------------------------------- 
Ptr<IViewportToolWidget> CopyLocation::createViewportToolWidget(QWidget * parent)
{
    return Ptr<IViewportToolWidget> (new CopyLocationWidget(*this, parent));
}
//----------------------------------------------------------------------------- 
void CopyLocation::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{

}
//----------------------------------------------------------------------------- 
const Core::List<Ptr<Property> >& CopyLocation::getProperties() const
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
Core::List<Ptr<Property> >& CopyLocation::getProperties()
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
void CopyLocation::selectionChanged(const Core::List<Ptr<Property> >& pties)
{
    updatePropertiesTargets(pties);
}
//----------------------------------------------------------------------------- 
bool CopyLocation::isHighlighted() const
{
    return false;
}
//----------------------------------------------------------------------------- 
bool CopyLocation::isSelected() const
{
    return false;
}
//----------------------------------------------------------------------------- 
void CopyLocation::updateViewportTool()
{
    computeActionCenters();
    computeActionOrientations();
}
//----------------------------------------------------------------------------- 
void CopyLocation::setContainer(const Ptr<Property>& pContainer)
{
    _impl->_pContainer = pContainer;
}
//----------------------------------------------------------------------------- 
bool CopyLocation::getLocation(const String& name, Core::Vector3f& worldPos, Core::Matrix3f& worldAxis)
{
    bool ret = false;

    for (int32 ii=0; ii<_impl->_locationGizmoDoc.size(); ii++)
    {
        if (_impl->_locationGizmoDoc[ii].name == name)
        {
            worldPos  = _impl->_locationGizmoDoc[ii].worldPosition;
            worldAxis = _impl->_locationGizmoDoc[ii].worldAxis;
            ret = true;
            break;
        }
    }

    if (ret == false)
    {
        for (int32 ii=0; ii<_impl->_locationGizmoWorldDeco.size(); ii++)
        {
            if (_impl->_locationGizmoWorldDeco[ii].name == name)
            {
                worldPos  = _impl->_locationGizmoWorldDeco[ii].worldPosition;
                worldAxis = _impl->_locationGizmoWorldDeco[ii].worldAxis;
                ret = true;
                break;
            }
        }
    }

    return ret;
}
//----------------------------------------------------------------------------- 
void CopyLocation::apply(const String& name)
{
    Core::Vector3f worldPos;
    Core::Matrix3f worldAxis;

    if (getLocation(name, worldPos, worldAxis) == true)
    {
        bool isModified = false;
        beginPropertiesChange();

        Core::Vector3f offset     = worldPos - _impl->_pivotInWorldAxis;
        Core::Matrix3f offsetAxis = _impl->_axis.getTransposed() * worldAxis;

        for (int32 ii=0; ii<_impl->_pties.size(); ++ii)
        {
            Ptr<PtyTransform>   pPtyT = LM_DEBUG_PTR_CAST<PtyTransform>(_impl->_pties[ii]);
            pPtyT->translateInWorldAxis(offset);
            pPtyT->addRotateInWorldAxis(offsetAxis);
            pPtyT->updateData();
            isModified = true;
        }

        if (isModified==true)
        {
            finishPropertiesChange();
        }
        else
        {
            cancelPropertiesChange();
        }
    }

    if (_impl->_pContainer!=null)
    {
        _impl->_pContainer->updateData();
    }

    updateViewportTool();
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------    
CopyLocationWidget::CopyLocationWidget(CopyLocation& tool, QWidget * parent) :
    IViewportToolWidget(parent),
    _tool(tool)
{
    setupUi();
    updateViewportToolWidget();
}
//-----------------------------------------------------------------------------  
CopyLocationWidget::~CopyLocationWidget()
{
}
//-----------------------------------------------------------------------------  
IViewportTool& CopyLocationWidget::getViewportTool()
{
    return _tool;
}
//-----------------------------------------------------------------------------  
void CopyLocationWidget::setupUi()
{
    _layout = new QGridLayout(NULL);

    _combo = new QtToolbox::ComboBox(this);
    _apply = new QPushButton("apply", this);
        _apply->setFixedSize(80, 20);

    _layout->addWidget(_combo, 0, 0, Qt::AlignCenter);
    _layout->addWidget(_apply, 0, 1, Qt::AlignCenter);

    setLayout(_layout);

    connect(_apply, SIGNAL(released()), this, SLOT(applyLocation()));

    IViewportToolWidget::setupUi();
}
//----------------------------------------------------------------------------- 
void CopyLocationWidget::applyLocation()
{
    _tool.apply(_combo->selectedText());
}
//-----------------------------------------------------------------------------  
void CopyLocationWidget::readViewportTool()
{
    Core::List<GizmoLocation>& gizmoDoc       = _tool._impl->_locationGizmoDoc;
    Core::List<GizmoLocation>& gizmoWorldDeco = _tool._impl->_locationGizmoWorldDeco;

    _combo->clear();

    for (int32 ii=0; ii<gizmoDoc.size(); ++ii)
    {
        _combo->addItem(gizmoDoc[ii].name);
    }
    
    for (int32 ii=0; ii<gizmoWorldDeco.size(); ++ii)
    {
        _combo->addItem(gizmoWorldDeco[ii].name);
    }
}
//-----------------------------------------------------------------------------  
void CopyLocationWidget::writeViewportTool(QWidget* pWidget)
{
    
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------  
}
