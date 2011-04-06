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
#include "PlayerPositionRepresentation.h"
#include <Assets/GeometricPrimitive.h>
#include <Universe/World.h>
#include <EPI/Document/Properties/PtyNodePlayerPosition.moc.h>
#include <EPI/Constants.h>

namespace EPI
{
//-----------------------------------------------------------------------------
const String    PLAYER_POSITION_MODEL_NAME            = L"$PlayerPositionCube";
const String    PLAYER_POSITION_MESH_NAME             = L"$PlayerPositionCubeMesh";
const String    tagCubePlayer1                        = L"cubePlayerPosition1";
const String    tagCubePlayer2                        = L"cubePlayerPosition2";
const String    tagModelPlayer1                        = L"modelPlayerPosition1";
const String    tagModelPlayer2                        = L"modelPlayerPosition2";
const Core::Vector3f cubeSize (0.5f, 2.f, 0.5f);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void createPlayerPositionModels(const Ptr<Universe::RessourcePool>& pPool)
{
    Ptr<Assets::ModelMesh> cube (new Assets::ModelMesh());
    Assets::VerticeSet & vset = cube->getLODByID(0);
    
    Assets::createCubeWithNormals(vset, cubeSize);
    Core::List<Assets::Vertex> & vertices = vset.getVertices();

    for (int32 ii=0; ii<vertices.size(); ++ii)
    {
        vertices[ii].position.y += cubeSize.y/2;
    }

    cube->cleanup();

    Ptr<Assets::Model> pCubeModel (new Assets::Model());
    pCubeModel->addMesh(PLAYER_POSITION_MESH_NAME, *cube);
    pPool->addModel(PLAYER_POSITION_MODEL_NAME, pCubeModel);
}
//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> PlayerPositionRepresentation::createPlayerPositionRepresentation(const Ptr<Universe::World>& pWorld)
{
    createPlayerPositionModels(pWorld->getRessourcesPool());

    Ptr<Universe::NodeGroup> pGroup = pWorld->createGroup();

    Ptr<Universe::NodeMesh> pNodeCube1 = pWorld->createMesh(PLAYER_POSITION_MODEL_NAME, PLAYER_POSITION_MESH_NAME);
    Ptr<Universe::NodeMesh> pNodeCube2 = pWorld->createMesh(PLAYER_POSITION_MODEL_NAME, PLAYER_POSITION_MESH_NAME);
    Ptr<Universe::Node> pNodeModel1 = pWorld->createNode();
    Ptr<Universe::Node> pNodeModel2 = pWorld->createNode();

    pNodeCube1->addTag(tagCubePlayer1);
    pNodeCube2->addTag(tagCubePlayer2);
    pNodeModel1->addTag(tagModelPlayer1);
    pNodeModel2->addTag(tagModelPlayer2);

    pNodeCube1->setMaterial(AssetsConst::sMatPlayer1);
    pNodeCube2->setMaterial(AssetsConst::sMatPlayer2);

    pGroup->addChild(pNodeCube1);
    pGroup->addChild(pNodeCube2);
    pGroup->addChild(pNodeModel1);
    pGroup->addChild(pNodeModel2);

    float distance = PLAYER_DISTANCE;

    Core::Vector3f  posP1 (0, 0, -distance/2.f);
    Core::Vector3f  posP2 (0, 0, distance/2.f);

    pNodeCube1->setLocalPosition(posP1);
    pNodeCube2->setLocalPosition(posP2);
    pNodeCube2->setLocalOrientation(Core::Quaternionf(Core::YAW, f_PI));

    return pGroup;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PlayerPositionRepresentation::PlayerPositionRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    
    _pNodeRepresentation = createPlayerPositionRepresentation(_pWorld);
    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
PlayerPositionRepresentation::PlayerPositionRepresentation(const PlayerPositionRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createPlayerPositionRepresentation(_pWorld);
    //pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
PlayerPositionRepresentation::~PlayerPositionRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void PlayerPositionRepresentation::setVisible(bool flag)
{
    IContentRepresentation::setVisible(flag);

    if (flag == true)
    {
        Ptr<Universe::NodeMesh> pNodeCube1  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer1));
        Ptr<Universe::NodeMesh> pNodeCube2  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer2));
        Ptr<Universe::Node>     pNodeModel1 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer1));
        Ptr<Universe::Node>     pNodeModel2 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer2));


        if (_currentModelName == L"")
        {
            pNodeCube1->setVisible(true, true);
            pNodeCube2->setVisible(true, true);
            pNodeModel1->setVisible(false, true);
            pNodeModel2->setVisible(false, true);   
        }
        else
        {
            pNodeCube1->setVisible(false, true);
            pNodeCube2->setVisible(false, true);
            pNodeModel1->setVisible(true, true);
            pNodeModel2->setVisible(true, true);
        }

    }
}
//-----------------------------------------------------------------------------
void PlayerPositionRepresentation::updateObjectRepresentation(const Property& pPty)
{
    const PtyNodePlayerPosition& ptyPP = static_cast<const PtyNodePlayerPosition&>(pPty);

    Ptr<Universe::NodeMesh> pNodeCube1  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer1));
    Ptr<Universe::NodeMesh> pNodeCube2  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer2));
    Ptr<Universe::Node>     pNodeModel1 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer1));
    Ptr<Universe::Node>     pNodeModel2 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer2));

    float distance = ptyPP.getPlayersDistance();
    const String& modelName = ptyPP.getModelName();

    Core::Vector3f  posP1 (0, 0, -distance/2.f);
    Core::Vector3f  posP2 (0, 0, distance/2.f);



    if (modelName == L"")
    {
        _currentModelName = L"";
        pNodeCube1->setVisible(true, true);
        pNodeCube2->setVisible(true, true);
        pNodeModel1->setVisible(false, true);
        pNodeModel2->setVisible(false, true);
    }
    else
    {
        if (_currentModelName != modelName)
        {
            _currentModelName = modelName;
            
            _pNodeRepresentation->removeChild(pNodeModel1);
            _pNodeRepresentation->removeChild(pNodeModel2);

            pNodeModel1->kill();
            pNodeModel2->kill();

            pNodeModel1 = _pWorld->createInstance(_currentModelName);
            pNodeModel2 = _pWorld->createInstance(_currentModelName);
            _pNodeRepresentation->addChild(pNodeModel1);
            _pNodeRepresentation->addChild(pNodeModel2);
            pNodeModel1->addTag(tagModelPlayer1);
            pNodeModel2->addTag(tagModelPlayer2);
        }

        pNodeCube1->setVisible(false, true);
        pNodeCube2->setVisible(false, true);
        pNodeModel1->setVisible(true, true);
        pNodeModel2->setVisible(true, true);
    }


    pNodeCube1->setLocalPosition(posP1);
    pNodeCube2->setLocalPosition(posP2);
    pNodeCube2->setLocalOrientation(Core::Quaternionf(Core::YAW, f_PI));
    pNodeModel1->setLocalPosition(posP1);
    pNodeModel2->setLocalPosition(posP2);
    pNodeModel2->setLocalOrientation(Core::Quaternionf(Core::YAW, f_PI));
}
//-----------------------------------------------------------------------------
void PlayerPositionRepresentation::setRepresentationMode(ECRMode mode)
{
    _mode = mode;

    if (_mode==SELECTION_REPRESENTATION)
    {
        _pNodeRepresentation->setBoundingBoxVisible(false);
    }
    else
    {
        _pNodeRepresentation->setBoundingBoxVisible(true);
    }
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& PlayerPositionRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool PlayerPositionRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
bool PlayerPositionRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::NodeMesh> pNodeCube1  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer1));
    Ptr<Universe::NodeMesh> pNodeCube2  = LM_DEBUG_PTR_CAST<Universe::NodeMesh>(_pNodeRepresentation->getChildWithTag(tagCubePlayer2));
    Ptr<Universe::Node>     pNodeModel1 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer1));
    Ptr<Universe::Node>     pNodeModel2 = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagModelPlayer2));


    bool ret = false;

    ret = pNodeCube1->isIntersecting(ray, distance);
    if (ret==false)      ret = pNodeCube2->isIntersecting(ray, distance);
    else if (ret==false) ret = pNodeModel1->isIntersecting(ray, distance);
    else if (ret==false) ret = pNodeModel2->isIntersecting(ray, distance);

    return ret;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> PlayerPositionRepresentation::clone()
{
    Ptr<PlayerPositionRepresentation>   pRet = Ptr<PlayerPositionRepresentation>(new PlayerPositionRepresentation (*this));
        
    return pRet;
}
//-----------------------------------------------------------------------------
void PlayerPositionRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
   // pNode->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
