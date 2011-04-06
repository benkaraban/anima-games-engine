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
#include "SunViewportTool.moc.h"

#include <Core/List.h>
#include <EPI/Document/Property.moc.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Document/Properties/PtySun.moc.h>
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>
#include <EPI/GUI/Viewport/NavigationStyle.h>

namespace EPI
{

const String    SUNVTOOL_ARROW_MODEL_NAME            = L"$SplineArrow";
const String    SUNVTOOL_ARROW_MESH_NAME             = L"$SplineArrowMesh";
const float     SUNVTOOL_ARROW_SIZE_FACTOR           = 2.6f;
const String    tagArrow                             = L"ArrowSunVTool";

void createSunVToolModels(Ptr<Universe::RessourcePool> pPool)
{
    //arrow
    Ptr<Assets::ModelMesh> arrow (new Assets::ModelMesh());
    Assets::VerticeSet & vsetArrow = arrow->getLODByID(0);
    Assets::createArrow(vsetArrow, SUNVTOOL_ARROW_SIZE_FACTOR);
    Assets::makeTBNBasis(vsetArrow, false);
    arrow->cleanup();

    Ptr<Assets::Model> pArrowModel (new Assets::Model());
    pArrowModel->addMesh(SUNVTOOL_ARROW_MESH_NAME, *arrow);
    pPool->addModel(SUNVTOOL_ARROW_MODEL_NAME, pArrowModel);
}
//-----------------------------------------------------------------------------   
//-----------------------------------------------------------------------------   
struct SunViewportTool::implementation
{
    SunViewportTool::implementation() {}

public:
    Core::List<Ptr<Property> >  _pties;
    Ptr<Universe::NodeGroup>    _pNode;
};
//-----------------------------------------------------------------------------          
//-----------------------------------------------------------------------------   
SunViewportTool::SunViewportTool(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPtySun):
    IViewportTool(pStateRecorder),
    _impl(new SunViewportTool::implementation ())
{
    getProperties().push_back(pPtySun);
}
//----------------------------------------------------------------------------- 
SunViewportTool::~SunViewportTool()
{
    if (_impl->_pNode != null)
    {
        _impl->_pNode->kill();
        _impl->_pNode = null;
    }

    delete _impl;
}
//----------------------------------------------------------------------------- 
void SunViewportTool::creatToolModels(Ptr<Universe::RessourcePool>& pPool)
{
    createSunVToolModels(pPool);
}
//----------------------------------------------------------------------------- 
Ptr<Universe::NodeGroup>  SunViewportTool::createTool(Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet;

    pRet = pWorld->createGroup();

    //model fleche
    Ptr<Universe::NodeMesh> pNodeArrow = pWorld->createMesh(SUNVTOOL_ARROW_MODEL_NAME, SUNVTOOL_ARROW_MESH_NAME);
    pNodeArrow->addTag(tagArrow);

    pNodeArrow->beginMatrixUpdate();
        pNodeArrow->localRoll(f_PI);
        pNodeArrow->localPitch(f_PI_DIV_2);
        //pNodeArrow->setLocalPosition(Core::Vector3f(0,0,SUNVTOOL_ARROW_SIZE_FACTOR));
    pNodeArrow->endMatrixUpdate();

    pRet->addChild(pNodeArrow);

    return pRet;
}
//----------------------------------------------------------------------------- 
Ptr<IViewportToolWidget> SunViewportTool::createViewportToolWidget(QWidget * parent)
{
    return null;
}
//----------------------------------------------------------------------------- 
void SunViewportTool::guiConfigure(Ptr<Universe::World>& pWorld, const DrawContext& dc)
{
    if (_impl->_pNode == null) {_impl->_pNode = createTool(pWorld);}


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

    Ptr<PtySun> pPtySun = LM_DEBUG_PTR_CAST<PtySun> (getProperties()[0]);
    Core::Vector3f pos = pNodeCam->getWorldPosition() + pNodeCam->getWorldDirection() * 10;
    Core::Vector3f dir = pPtySun->getMainLightSetting().getLightDirection();
        dir.normalizeSafe();

    _impl->_pNode->beginMatrixUpdate();
        _impl->_pNode->setWorldPosition(pos);
        _impl->_pNode->setUpVector(Core::Vector3f(0,1,0));
        _impl->_pNode->setWorldDirection(dir);
    _impl->_pNode->endMatrixUpdate();

}
//----------------------------------------------------------------------------- 
void SunViewportTool::mouseMoveEvent         (QMouseEvent* event,const DrawContext& dc)
{
}
//----------------------------------------------------------------------------- 
void SunViewportTool::mousePressEvent        (QMouseEvent* event,const DrawContext& dc)
{
}
//----------------------------------------------------------------------------- 
void SunViewportTool::mouseReleaseEvent      (QMouseEvent* event,const DrawContext& dc)
{
}
//----------------------------------------------------------------------------- 
const Core::List<Ptr<Property> >& SunViewportTool::getProperties() const
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
Core::List<Ptr<Property> >& SunViewportTool::getProperties()
{
    return _impl->_pties;
}
//----------------------------------------------------------------------------- 
bool SunViewportTool::isHighlighted() const
{
    return false;
}
//----------------------------------------------------------------------------- 
bool SunViewportTool::isSelected() const
{
    return false;
}
//----------------------------------------------------------------------------- 
void SunViewportTool::updateViewportTool()
{
}
//----------------------------------------------------------------------------- 
//-----------------------------------------------------------------------------          
}
