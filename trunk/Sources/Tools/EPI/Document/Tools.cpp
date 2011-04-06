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
#include "Tools.h"

#include <Universe/World.h>
#include <Core/Strings/Tools.h>
#include <EPI/Document/Properties/PtyNodeEmitter/PtyNodeEmitter.moc.h>
#include <EPI/Document/Properties/PtyWaterPatch.h>
#include <EPI/Document/Properties/PtyTerrain.moc.h>
#include <EPI/Document/Properties/PtyNodeDecal.moc.h>
#include <EPI/Document/Properties/PtyNodeInstance.h>
#include <EPI/Document/Properties/PtyNodeCamera.h>
#include <EPI/Document/Properties/PtyNodeTrail.h>
#include <EPI/Document/Properties/PtyNodeSound.moc.h>
#include <EPI/Document/Properties/PtyNodePointLight.h>
#include <EPI/Document/Properties/PtyGizmo.h>
#include <EPI/Document/Properties/PtyNodeStorm.moc.h>
#include <EPI/Document/Properties/PtyNodePlayerPosition.moc.h>

#include <EPI/Document/ContentInfo/PlayerPositionRepresentation.h>
#include <EPI/Document/ContentInfo/GizmoRepresentation.h>

#include <EPI/DocumentRenderer.h>
#include <EPI/ImportInfo/ImportGroupInfo.h>
#include <EPI/Document/StateRecorder.h>

namespace EPI
{
Ptr<PropertyNode> travelUniverseNodesToProperties(const Ptr<Universe::World> pWorldInfoContent, const Ptr<Universe::World> pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, const Ptr<Universe::Node> & pNode, PropertyNode & ptyNode, Core::List<int32 >& generationPtyNode,
                                     const Ptr<StateRecorder>& pStateRecorder = null)
{
    Ptr<PropertyNode> pProperty = null;

    if(pNode->isInstanceChild())
    {
        // rien � faire
    }
    else if(pNode->isInstance())
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeInstance(pWorld, pNode, pNode->getName()));
    }
    else if(pNode->getNodeType() == Universe::NODE_TERRAIN)
    {
        pProperty = Ptr<PropertyNode>(new PtyTerrain(pWorld, LM_DEBUG_PTR_CAST<Universe::NodeTerrain>(pNode), pDocumentRenderer));
        generationPtyNode.push_back(pProperty->getId());
    }
    else if(pNode->getNodeType() == Universe::NODE_EMITTER)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeEmitter(pWorld, LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_DECAL)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeDecal(pWorld, LM_DEBUG_PTR_CAST<Universe::NodeDecal>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_STORM)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeStorm(pWorld, LM_DEBUG_PTR_CAST<Universe::NodeStorm>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_TRAIL)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeTrail(pWorldInfoContent, pWorld, LM_DEBUG_PTR_CAST<Universe::NodeTrail>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_SOUND)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodeSound(pWorldInfoContent, pWorld, LM_DEBUG_PTR_CAST<Universe::NodeSoundSource>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_POINT_LIGHT)
    {
        pProperty = Ptr<PropertyNode>(new PtyNodePointLight(pWorldInfoContent, pWorld, LM_DEBUG_PTR_CAST<Universe::NodePointLight>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_WATER_PATCH)
    {
        pProperty = Ptr<PropertyNode>(new PtyWaterPatch(pWorld, LM_DEBUG_PTR_CAST<Universe::NodeWaterPatch>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_CAMERA)
    {
        pProperty = Ptr<PtyNodeCamera> (new PtyNodeCamera(pWorldInfoContent, pWorld, LM_DEBUG_PTR_CAST<Universe::NodeCamera>(pNode)));
    }
    else if(pNode->getNodeType() == Universe::NODE_GROUP)
    {
        pProperty = Ptr<PtyNode> (new PtyNode(pWorld, pNode, L"Group"));
    }
    else if(pNode->getNodeType() == Universe::NODE)
    {
        if (pNode->hasTag(TAG_PLAYERS_POSITION))
        {
            pProperty = Ptr<PtyNodePlayerPosition> (new PtyNodePlayerPosition(pWorldInfoContent, pWorld, pNode));
        }
        else if(pNode->hasTag(TAG_GROUP_MEMO_CAM))
        {
            pProperty = Ptr<PtyNodeGroupMemoCam> (new PtyNodeGroupMemoCam(pWorld, pNode));
        }
        else if (pNode->hasTag(TAG_GIZMO))
        {
            pProperty = Ptr<PtyNodeGizmo> (new PtyNodeGizmo(pWorldInfoContent, pWorld, pNode));
        }
        else
        {
            pProperty = Ptr<PtyNode> (new PtyNode(pWorld, pNode, pNode->getName()));
        }
    }
    else if(pNode->getNodeType() == Universe::NODE_SKELETON)
    {
        pProperty = Ptr<PtyNode> (new PtyNode(pWorld, pNode, pNode->getName()));
    }
    else if(pNode->getNodeType() == Universe::NODE_MESH)
    {
        pProperty = Ptr<PtyNode> (new PtyNode(pWorld, pNode, pNode->getName()));
    }


    if(pProperty != null)
    {
        //recursion
        for(int32 iChild = 0; iChild < pNode->getChildCount(); ++iChild)
            travelUniverseNodesToProperties(pWorldInfoContent, pWorld, pDocumentRenderer, pNode->getChild(iChild), *pProperty, generationPtyNode);

        if (pStateRecorder == null)
            ptyNode.PropertyNode::addChild(pProperty);
        else
            addChildRecording(pStateRecorder, LM_DEBUG_PTR_CAST<Property>(pProperty), ptyNode);
    }

    return pProperty;
}
//-----------------------------------------------------------------------------
void universeNodesToProperties(const Ptr<Universe::World> pWorldInfoContent, const Ptr<Universe::World> pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, const Core::List<Ptr<Universe::Node> >& nodes, Property & pty, Core::List<int32 >& generationPtyNode, const Ptr<StateRecorder>& pStateRecorder)
{
    PropertyNode& propertyN = (PropertyNode&) pty;
    generationPtyNode.clear();

    for(int32 iNode = 0; iNode < nodes.size(); ++iNode)
    {
        travelUniverseNodesToProperties(pWorldInfoContent, pWorld, pDocumentRenderer, nodes[iNode], propertyN, generationPtyNode, pStateRecorder);
    }
}
//-----------------------------------------------------------------------------
void saveGroup(const Ptr<Core::VirtualFileSystem>& pVFS, const Ptr<Universe::World>& pWorld, const Ptr<Universe::NodeGroup>& pNodeG, const Core::String & fileName)
{
    Core::XMLDocument xmlDocument;
    Ptr<Core::XMLNode> pXMLNode(new Core::XMLNode(L"node"));

    pWorld->exportXML(pXMLNode, pNodeG, true);
    xmlDocument.addNode(L"root", pXMLNode);

    Ptr<Core::OutputStream> pOutput = pVFS->writeFile(fileName);
    xmlDocument.saveDocument(*pOutput);
    pOutput->close();

    Ptr<ImportGroupInfo> pInfo(new ImportGroupInfo());
    pInfo->fileName = fileName;

    Ptr<Core::XMLDocument> pXMLInfo(new Core::XMLDocument());
    pInfo->exportXML(pXMLInfo);

    Core::String infoFile;
    infoFile << fileName << L".xml";
    Ptr<Core::OutputStream> pInfoOutput = pVFS->writeFile(infoFile);
    pXMLInfo->saveDocument(*pInfoOutput);
    pInfoOutput->close();
}
//-----------------------------------------------------------------------------
LM_API_EPI void convertInstanceProperty(const Ptr<StateRecorder>& pStateRecorder, Ptr<PtyNodeInstance> & ptyToInstanciate, const Ptr<Universe::World> pWorldInfoContent, const Ptr<Universe::World> pWorld, const Ptr<DocumentRenderer> & pDocumentRenderer, Property & ptyNode, Core::List<int32 >& generationPtyNode)
{
    Ptr<Universe::Node> pNode = ptyToInstanciate->getUniverseNode();
    
    LM_ASSERT(pNode->isInstance() == true);


    Ptr<Core::XMLNode> _pXmlN (new Core::XMLNode(L"xmlForInstanciate"));
        pWorld->exportXML(_pXmlN, pNode, true);
    Ptr<Universe::Node> pNewNode = pWorld->importXML(_pXmlN);
        pNewNode->instanciate();
    
    

    PropertyNode& propertyN = (PropertyNode&) ptyNode;
    Ptr<PropertyNode> pNewPty = travelUniverseNodesToProperties(pWorldInfoContent, pWorld, pDocumentRenderer, pNewNode, propertyN, generationPtyNode, pStateRecorder);
    if (pNewPty !=null)
    {
        pNewPty->setName(L"instantiateFrom_" + pNewPty->getName());
    }
    removeChildRecording(pStateRecorder, LM_DEBUG_PTR_CAST<Property>(ptyToInstanciate));
}
//-----------------------------------------------------------------------------
LM_API_EPI void configWorldDeco(const Ptr<Universe::World>& pWorld, const Ptr<Universe::World>& pWorldDeco)
{
    Ptr<Universe::Node> pNodePlayerPosition = pWorldDeco->getFirstNodeWithTag(TAG_PLAYERS_POSITION);

    if (pNodePlayerPosition!=null)
    {
        Ptr<Universe::NodeGroup> pRepresentation = PlayerPositionRepresentation::createPlayerPositionRepresentation(pWorldDeco);
        pNodePlayerPosition->addChild(pRepresentation);
    }

    Core::List<Ptr<Universe::Node> > nodesGizmo;
    pWorldDeco->getNodesWithTag(TAG_GIZMO, nodesGizmo);
    for (int32 ii=0; ii<nodesGizmo.size(); ++ii)
    {
        Ptr<Universe::NodeGroup> pRepresentation = GizmoRepresentation::createGizmoUniverseRepresentation(pWorldDeco);
        nodesGizmo[ii]->addChild(pRepresentation);
    }

}


LM_API_EPI void prepareWorldForRunning(Universe::World& world, const String& tagGamePlay)
{
    Ptr<Universe::Node> pNodePlayers = world.getFirstNodeWithTag(TAG_PLAYERS_POSITION);

    if (pNodePlayers!=null)
    {
        Ptr<Universe::Node> pAvatarCaster = world.createModel(L"/HOO/Characters/Silene/Silene.mdl");
        Ptr<Universe::Node> pAvatarTarget = world.createModel(L"/HOO/Characters/Silene/Silene.mdl");

        Ptr<Universe::Node> pPivotCaster = world.createNode();
        Ptr<Universe::Node> pPivotTarget = world.createNode();

        pPivotCaster->setLocalScale(Core::Vector3f(1.0f, 1.0f, 1.0f));
        pPivotTarget->setLocalScale(Core::Vector3f(-1.0f, 1.0f, 1.0f));

        pPivotCaster->addTag(L"pivot");
        pPivotTarget->addTag(L"pivot");

        pAvatarCaster->addChild(pPivotCaster);
        pAvatarTarget->addChild(pPivotTarget);

        pNodePlayers->addChild(pAvatarCaster);
        pNodePlayers->addChild(pAvatarTarget);

        float distance = PLAYER_DISTANCE;
        Core::Vector3f  posP1 (0, 0, -distance/2.f);
        Core::Vector3f  posP2 (0, 0, distance/2.f);

        pAvatarCaster->setLocalPosition(posP1);
        pAvatarTarget->setLocalPosition(posP2);
        
        pAvatarCaster->addTag(TAG_PLAYERS_CASTER);
        pAvatarCaster->addTag(TAG_PLAYER);

        pAvatarTarget->addTag(TAG_PLAYERS_TARGET);
        pAvatarTarget->addTag(TAG_OPPONENT);

        pAvatarTarget->setLocalOrientation(Core::Quaternionf(Core::YAW, f_PI));


        ////
        Ptr<Universe::Node> pNodeGamePlay = world.getFirstNodeWithTag(TAG_NODE_GAMEPLAY);
            
        if (pNodeGamePlay==null)
        {
            pNodeGamePlay = world.createNode();
            pNodeGamePlay->addTag(TAG_NODE_GAMEPLAY);
        }
        Core::List<String>  tagsGP;
        Core::tokenize(tagGamePlay, tagsGP, L" ");
        for (int32 ii=0; ii<tagsGP.size(); ++ii)
        {
            pNodeGamePlay->addTag(tagsGP[ii]);
        }
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace EPI
