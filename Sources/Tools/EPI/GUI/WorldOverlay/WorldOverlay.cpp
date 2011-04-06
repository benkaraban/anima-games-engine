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
#include "WorldOverlay.h"

#include <Assets/geometricPrimitive.h>
#include <EPI/Document/Properties/PtyDocEdition.moc.h>
#include <EPI/Document/Properties/PtyNode.moc.h>

#include <EPI/GUI/Viewport/Tools/TransformViewportTool.moc.h>
#include <EPI/GUI/Viewport/Tools/SunViewportTool.moc.h>
#include <EPI/GUI/WorldOverlay/NodeCameraEdition.h>

#include <Renderer/IViewVolume.h>
#include <EPI/Document/PropertySelection.moc.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Constants.h>
namespace EPI
{
const String    ARROW_MODEL_NAME            = L"$GlobalAxisArrow";
const String    ARROW_MESH_NAME             = L"$GlobalAxisArrowMesh";

const float     ARROW_SIZE_FACTOR           =1.5f;

void createGlobalAxisModels(Ptr<Universe::RessourcePool> pPool)
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
}

Ptr<Universe::Node> createGlobalAxis(Universe::World& world)
{
    createGlobalAxisModels(world.getRessourcesPool());
    Ptr<Universe::NodeGroup> pRet = world.createGroup();



    Ptr<Universe::NodeGroup> pGroup = world.createGroup();
    pGroup->setFixedSize(true, 1/20.f);

    Ptr<Universe::NodeMesh> pNodeArrowAxeX = world.createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeY = world.createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeArrowAxeZ = world.createMesh(ARROW_MODEL_NAME, ARROW_MESH_NAME);

    pNodeArrowAxeX->getMeshInstance()->setMaterial(RdrConst::sMatAxisX);
    pNodeArrowAxeY->getMeshInstance()->setMaterial(RdrConst::sMatAxisY);
    pNodeArrowAxeZ->getMeshInstance()->setMaterial(RdrConst::sMatAxisZ);

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

    pGroup->addChild(pNodeArrowAxeX);
    pGroup->addChild(pNodeArrowAxeY);
    pGroup->addChild(pNodeArrowAxeZ);

    pRet->addChild(pGroup);

    return pRet;
}

//-----------------------------------------------------------------------------
NodeSelection::NodeSelection(const Ptr<Renderer::IAABox> & box, Universe::World * pWorld) : 
    Node(box, pWorld),
    NodeEditor()
{
}
//-----------------------------------------------------------------------------
NodeSelection::~NodeSelection()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
WorldOverlay::WorldOverlay(const Ptr<Renderer::IRenderer> & pRenderer,
          const Ptr<Core::VirtualFileSystem> & pVFS,
          const Ptr<Universe::RessourcePool> & pMasterPool):
    World(pRenderer, pVFS, pMasterPool),
    _showSelectedBoundingBoxVisible(true)
{
    TransformTool::createTransformModels(_pRessourcesPool);
    SunViewportTool::creatToolModels(_pRessourcesPool);
    _globalAxis = createGlobalAxis(*this);
}
//-----------------------------------------------------------------------------
WorldOverlay::~WorldOverlay()
{
}
//-----------------------------------------------------------------------------
void WorldOverlay::setSelectedBoundingBoxVisible(bool flag)
{
    _showSelectedBoundingBoxVisible = flag;
}
//-----------------------------------------------------------------------------
Ptr<NodeSelection> WorldOverlay::createSelection(const Core::AABoxf& box)
{
    //le parent
    Ptr<NodeSelection> pNode(new NodeSelection(_pScene->createAABox(), this));
 
    return pNode;
}
//-----------------------------------------------------------------------------
void WorldOverlay::updateSelection(const Ptr<PropertySelection>& pPtySelection)
{/*
    for (int32 iSelect=0; iSelect<_nodesSelected.size(); ++iSelect)
    {
        _nodesSelected[iSelect]->kill();
    }


    _nodesSelected.clear();
*/
    for (int32 ii=0; ii<_memoNodeSelected.size(); ++ii)
    {
        _memoNodeSelected[ii]->setBoundingBoxVisible(false);
    }

    _memoNodeSelected.clear();

    for (int32 iPty=0; iPty<pPtySelection->getSelectedPtiesCount(); iPty++)
    {
        Ptr<Property> pPtyS = pPtySelection->getSelectedPty(iPty);

        if (pPtyS->hasFlag(FLAG_PTY_NODE) == true)
        {
            Ptr<PtyNode> pPtyN = LM_DEBUG_PTR_CAST<PtyNode>(pPtySelection->getSelectedPty(iPty));

            if (pPtyN != null)
            {
                Ptr<Universe::Node> p = pPtyN->getUniverseNode();

                if (p!=null && _showSelectedBoundingBoxVisible == true)
                {
                    p->setBoundingBoxVisible(true);
                    _memoNodeSelected.push_back(p);
                }
            }
        }
    }
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeCamera> WorldOverlay::createCameraEdition()
{
    const int32 nbCam = 6;
    const float near = 0.1f;
    const float far = 1000.f;

    Ptr<Universe::NodeOrthogonalCamera> tabNodeCam [nbCam];



    for (int32 iCam=0; iCam<nbCam; ++iCam)
    {
        Ptr<Renderer::IOrthoCamera>       pOrthogonalCamera(_pScene->createOrthoCamera(-10, 10, -10, 10, near, far));

        tabNodeCam[iCam] = Ptr<Universe::NodeOrthogonalCamera> (new Universe::NodeOrthogonalCamera(_pScene->createAABox(), pOrthogonalCamera, this));
        //LM_DEBUG_PTR_CAST<Universe::NodeCameraPO>(tabNodeCam[iCam])->setActiveCameraType(Renderer::CAMERA_ORTHOGONAL);
    }

    Ptr<Universe::NodeCamera> pNode(new NodeCameraEdition(  tabNodeCam[0],
                                                            tabNodeCam[1],
                                                            tabNodeCam[2],
                                                            tabNodeCam[3],
                                                            tabNodeCam[4],
                                                            tabNodeCam[5],
                                                            _pScene->createAABox(),
                                                            this));

    _nodes.push_back(pNode);
    return pNode;
}
//-----------------------------------------------------------------------------
void WorldOverlay::configureOverlayForRendering(const Ptr<Universe::NodeCamera>& camera) const
{
    
    Ptr<Renderer::IViewVolume> pVV = camera->getBaseCamera()->createViewVolume();
    Core::Rayf ray =  pVV->makeRayFrom2DPoint(Core::Vector2f(0.035f, 0.965f));
    Core::Vector3f globalAxisWorldPos = ray.origin + ray.direction*1;

    if (camera->getBaseCamera()->getCameraType() == Renderer::CAMERA_ORTHOGONAL)
    {
        if (Core::L_abs(globalAxisWorldPos.y) > Core::L_abs(globalAxisWorldPos.x) && Core::L_abs(globalAxisWorldPos.y) > Core::L_abs(globalAxisWorldPos.z))
            globalAxisWorldPos.y = 0;
        else if (Core::L_abs(globalAxisWorldPos.x) > Core::L_abs(globalAxisWorldPos.y) && Core::L_abs(globalAxisWorldPos.x) > Core::L_abs(globalAxisWorldPos.z))
            globalAxisWorldPos.x = 0;
        else if (Core::L_abs(globalAxisWorldPos.z) > Core::L_abs(globalAxisWorldPos.x) && Core::L_abs(globalAxisWorldPos.z) > Core::L_abs(globalAxisWorldPos.y))
            globalAxisWorldPos.z = 0;
    }


    _globalAxis->setWorldPosition(globalAxisWorldPos);
}
//-----------------------------------------------------------------------------
}