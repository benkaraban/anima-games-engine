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
#include <UTests.h>

#include <Core/Standard.h>
#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>

#include <Renderer/Stub/RendererStub.h>
#include <Audio/Stub/DriverAudioStub.h>

#include <Universe/Lua/LuaWorld.h>
#include <Universe/Lua/LuaTools.h>

using namespace Universe;

extern "C" int test_add(lua_State *L)
{
    float x = luat_tofloat(L, 1);
    float y = luat_tofloat(L, 2);

    lua_pushnumber(L, double(x + y));
    return 1;
}

#define TEST_MEM_LEAK 0

void testLuaNode(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node");

    Ptr<Node>  pNode1(pWorld->createNode());
    Ptr<Node>  pNode2(pWorld->createNode());
    Ptr<Node>  pNode3(pWorld->createNode());
    Ptr<Node>  pNode4(pWorld->createNode());
    Ptr<Node>  pNodeParent(pWorld->createNode());

    pNodeParent->setLocalPosition(Core::Vector3f(1.0f, 1.0f, 1.0f));
    
    pNodeParent->localYaw(1.0f);
    pNodeParent->localPitch(2.0f);
    pNodeParent->localRoll(3.0f);
    pNodeParent->addTag(L"NODE-PARENT");

    pNodeParent->addChild(pNode1);
    pNodeParent->addChild(pNode2);

    pNode1->setLocalPosition(Core::Vector3f(1.0f, 2.0f, 3.0f));
    pNode1->setLocalScale(Core::Vector3f(4.0f, 5.0f, 6.0f));
    pNode1->setLocalDirection(Core::Vector3f(1.0f, 0.0f, 0.0f));

    pNode1->setVisible(true);
    pNode1->setBoundingBoxVisible(false);
    pNode1->setTimeMultiplier(0.5);
    pNode1->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));

    pNode1->addTag(L"NODE1");
    pNode1->addTag(L"TEST-ME");
    pNode1->addTag(L"REMOVE-ME");

    pNode2->setWorldPosition(Core::Vector3f(-1.0f, -2.0f, -3.0f));
    pNode2->setWorldDirection(Core::Vector3f(-1.0f, -2.0f, -3.0f));
    pNode2->addTag(L"NODE2");

    pNode2->setVisible(false);
    pNode2->setBoundingBoxVisible(false);
    pNode2->setTimeMultiplier(2.0);
    pNode2->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));

    pNode3->localYaw(1.0f);
    pNode3->localPitch(2.0f);
    pNode3->localRoll(3.0f);
    pNode3->localYaw(1.0f);
    pNode3->localPitch(2.0f);
    pNode3->localRoll(3.0f);

    pNode4->worldYaw(1.0f);
    pNode4->worldPitch(2.0f);
    pNode4->worldRoll(3.0f);
    pNode4->worldYaw(1.0f);
    pNode4->worldPitch(2.0f);
    pNode4->worldRoll(3.0f);

    Ptr<ITask> pTask = pWorld->runScript(L"/Data/TUData/Scripts/tu-node-01.lua");
    UTEST_EQU(pTask->isFinished(), true);

    Ptr<Node>  pLuaParent(pWorld->getFirstNodeWithTag(L"LUA-NODE-PARENT"));
    Ptr<Node>  pLuaNode1(pWorld->getFirstNodeWithTag(L"LUA-NODE1"));
    Ptr<Node>  pLuaNode2(pWorld->getFirstNodeWithTag(L"LUA-NODE2"));
    Ptr<Node>  pLuaNode3(pWorld->getFirstNodeWithTag(L"LUA-NODE3"));
    Ptr<Node>  pLuaNode4(pWorld->getFirstNodeWithTag(L"LUA-NODE4"));

    UTEST_NOT_EQU(pLuaParent, null);
    UTEST_NOT_EQU(pLuaNode1, null);
    UTEST_NOT_EQU(pLuaNode2, null);
    UTEST_NOT_EQU(pLuaNode3, null);
    UTEST_NOT_EQU(pLuaNode4, null);

    UTEST_EQU(pNodeParent->getLocalOrient().isEquivalent(pLuaParent->getLocalOrient()), true);

    UTEST_EQU(pNode1->getLocalPosition().isEquivalent(pLuaNode1->getLocalPosition()), true);
    UTEST_EQU(pNode1->getLocalScale().isEquivalent(pLuaNode1->getLocalScale()), true);
    UTEST_EQU(pNode1->getLocalDirection().isEquivalent(pLuaNode1->getLocalDirection()), true);
    UTEST_EQU(pNode1->isVisible(), pLuaNode1->isVisible());
    UTEST_EQU(pNode1->isBoundingBoxVisible(), pLuaNode1->isBoundingBoxVisible());
    UTEST_EQU(pNode1->getTimeMultiplier(), pLuaNode1->getTimeMultiplier());
    UTEST_EQU(pNode1->getUpVector(), pLuaNode1->getUpVector());
    UTEST_EQU(pNode1->hasTag(L"REMOVE-ME"), false);

    UTEST_EQU(pNode2->getWorldPosition().isEquivalent(pLuaNode2->getWorldPosition()), true);
    UTEST_EQU(pNode2->getWorldDirection().isEquivalent(pLuaNode2->getWorldDirection()), true);
    UTEST_EQU(pNode2->isVisible(), pLuaNode2->isVisible());
    UTEST_EQU(pNode2->isBoundingBoxVisible(), pLuaNode2->isBoundingBoxVisible());
    UTEST_EQU(pNode2->getTimeMultiplier(), pLuaNode2->getTimeMultiplier());
    UTEST_EQU(pNode2->getUpVector(), pLuaNode2->getUpVector());

    UTEST_EQU(pNode3->getLocalOrient().isEquivalent(pLuaNode3->getLocalOrient()), true);
    UTEST_EQU(pNode4->getLocalOrient().isEquivalent(pLuaNode4->getLocalOrient()), true);

    const Core::List<Ptr<Node> > & nodes = pWorld->getAllNodes();

    for(int32 ii=0; ii < nodes.size(); ii++)
        nodes[ii]->kill();

    pWorld->update(0.0);

    UTEST_END_TEST;
}

void testLuaNodeTree(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node hierarchy");

    // A
    // +-A1
    //  +A2 REMOVE-ME
    //  +A3
    // B
    // +-B1   TAGB
    //  +B2   
    //   +B21 TAGB

    Ptr<Node>  pNodeA(pWorld->createNode());
    Ptr<Node>  pNodeA1(pWorld->createNode());
    Ptr<Node>  pNodeA2(pWorld->createNode());
    Ptr<Node>  pNodeA3(pWorld->createNode());

    Ptr<Node>  pNodeB(pWorld->createNode());
    Ptr<Node>  pNodeB1(pWorld->createNode());
    Ptr<Node>  pNodeB2(pWorld->createNode());
    Ptr<Node>  pNodeB21(pWorld->createNode());

    pNodeA2->addTag(L"REMOVE-ME");
    pNodeB1->addTag(L"TAGB");
    pNodeB21->addTag(L"TAGB");

    pNodeA->addChild(pNodeA1);
    pNodeA->addChild(pNodeA2);
    pNodeA->addChild(pNodeA3);
    pNodeA->addTag(L"NODEA");

    pNodeB->addChild(pNodeB1);
    pNodeB->addChild(pNodeB2);
    pNodeB->addTag(L"NODEB");

    pNodeB2->addChild(pNodeB21);

    Ptr<ITask> pTask = pWorld->runScript(L"/Data/TUData/Scripts/tu-node-02.lua");
    UTEST_EQU(pTask->isFinished(), true);

    UTEST_NOT_EQU(pNodeA->hasTag(L"LUA-ERROR"), true);

    UTEST_EQU(pNodeA->getChildCount(), 2);
    UTEST_EQU(pNodeA1->isRoot(), false);
    UTEST_EQU(pNodeA2->isRoot(), true);
    UTEST_EQU(pNodeA3->isRoot(), false);

    UTEST_EQU(pNodeB->getChildCount(), 3);
    UTEST_NOT_EQU(pNodeB->getChildWithTag(L"NEW-NODE"), null);

    Core::List<Ptr<Node> > nodesB;
    Core::List<Ptr<Node> > nodesBRecursive;

    pNodeB->getChildrenWithTag(L"TAGB", nodesB, false);
    pNodeB->getChildrenWithTag(L"TAGB", nodesBRecursive, true);

    for(int32 ii=0; ii < nodesB.size(); ii++)
    {
        UTEST_EQU(nodesB[ii]->hasTag(L"LUA1"), true);
    }

    for(int32 ii=0; ii < nodesBRecursive.size(); ii++)
    {
        UTEST_EQU(nodesBRecursive[ii]->hasTag(L"LUA2"), true);
    }

    const Core::List<Ptr<Node> > & nodes = pWorld->getAllNodes();

    for(int32 ii=0; ii < nodes.size(); ii++)
        nodes[ii]->kill();

    pWorld->update(0.0);

    UTEST_END_TEST;
}

void testLuaScheduling(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Task scheduling 1");

    Ptr<Node>  pNodeA(pWorld->createNode());
    pNodeA->addTag(L"NODEA");

    Ptr<ITask> pTask1A = pWorld->runScript(L"/Data/TUData/Scripts/tu-task-01.lua");
    UTEST_EQU(pTask1A->isFinished(), false);

    pWorld->update(0.6); // total -> 0.6
    Ptr<ITask> pTask1B = pWorld->runScript(L"/Data/TUData/Scripts/tu-task-01.lua");

    UTEST_EQU(pTask1A->isFinished(), false);
    UTEST_EQU(pTask1B->isFinished(), false);

    pWorld->update(0.5); // total -> 1.1
    Ptr<ITask> pTask1C = pWorld->runScript(L"/Data/TUData/Scripts/tu-task-01.lua");

    UTEST_EQU(pTask1A->isFinished(), true);
    UTEST_EQU(pTask1B->isFinished(), false);
    UTEST_EQU(pTask1C->isFinished(), false);

    pWorld->update(0.6); // total -> 1.7
    UTEST_EQU(pTask1A->isFinished(), true);
    UTEST_EQU(pTask1B->isFinished(), true);
    UTEST_EQU(pTask1C->isFinished(), false);

    pWorld->update(0.5);
    UTEST_EQU(pTask1A->isFinished(), true);
    UTEST_EQU(pTask1B->isFinished(), true);
    UTEST_EQU(pTask1C->isFinished(), true);

    UTEST_END_TEST;
}

void testLuaTasks(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Tasks");

    Ptr<NodeDecal> pDecal1 = pWorld->createDecal();
    Ptr<NodeDecal> pDecal2 = pWorld->createDecal();

    pDecal1->addTag(L"DECAL1");
    pDecal2->addTag(L"DECAL2");

    Ptr<Node>  pNode1(pWorld->createNode());
    Ptr<Node>  pNode2(pWorld->createNode());
    Ptr<Node>  pNode3(pWorld->createNode());

    pNode1->addTag(L"NODE1");
    pNode2->addTag(L"NODE2");
    pNode3->addTag(L"NODE3");

    Ptr<NodeEmitter> pEmitter1 = pWorld->createBillboardEmitter(L"/Data/AssetsPacked/TUData/Textures/part00.tex", Renderer::BILLBOARD_ADD);

    pEmitter1->addTag(L"EMITTER1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-task-03.lua");

    pWorld->update(1.0);
    pWorld->update(1.0);
    pWorld->update(1.0);

    UTEST_END_TEST;
}

void testLuaTasksTransform(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Task transform");

    Ptr<Node>  pNodeM1(pWorld->createNode());
    Ptr<Node>  pNodeM2(pWorld->createNode());

    pNodeM1->addTag(L"MOVE1");
    pNodeM2->addTag(L"MOVE2");

    Ptr<Node>  pNodeR1(pWorld->createNode());
    Ptr<Node>  pNodeR2(pWorld->createNode());
    Ptr<Node>  pNodeR3(pWorld->createNode());

    pNodeR1->addTag(L"PITCH");
    pNodeR2->addTag(L"YAW");
    pNodeR3->addTag(L"ROLL");

    pNodeM1->setWorldPosition(Core::Vector3f(1.0, 0.0, 0.0));

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-task-02.lua");

    pWorld->update(1.0);
    UTEST_EQU(pTask1->isFinished(), false);

    UTEST_EQU(pNodeM1->getWorldPosition().isEquivalent(Core::Vector3f(2.0, 0.0, 0.0)), true);
    UTEST_EQU(pNodeM2->getWorldPosition().isEquivalent(Core::Vector3f(5.0, 0.0, 0.0)), true);
 
    UTEST_EQU(pNodeM1->getLocalScale().isEquivalent(Core::Vector3f(1.0, 2.0, 1.0)), true);

    UTEST_EQU(pNodeR1->getWorldDirection().isEquivalent(Core::Vector3f( 0.0, -1.0,  0.0)), true);
    UTEST_EQU(pNodeR2->getWorldDirection().isEquivalent(Core::Vector3f( 1.0,  0.0,  0.0)), true);
    UTEST_EQU(pNodeR3->getWorldDirection().isEquivalent(Core::Vector3f( 0.0,  0.0,  1.0)), true);

    pWorld->update(2.0);
    UTEST_EQU(pNodeM1->getWorldPosition().isEquivalent(Core::Vector3f(3.0, 0.0, 0.0)), true);
    UTEST_EQU(pNodeM2->getWorldPosition().isEquivalent(Core::Vector3f(6.0, 0.0, 0.0)), true);

    UTEST_EQU(pNodeM1->getLocalScale().isEquivalent(Core::Vector3f(1.0, 3.0, 1.0)), true);

    UTEST_EQU(pNodeR1->getWorldDirection().isEquivalent(Core::Vector3f( 0.0,  0.0, -1.0)), true);
    UTEST_EQU(pNodeR2->getWorldDirection().isEquivalent(Core::Vector3f( 0.0,  0.0, -1.0)), true);
    UTEST_EQU(pNodeR3->getWorldDirection().isEquivalent(Core::Vector3f( 0.0,  0.0,  1.0)), true);

    UTEST_END_TEST;
}

void testLuaWorld(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"World");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-world-01.lua");

    Ptr<Node> pInst = pWorld->getFirstNodeWithTag(L"INSTANCE");
    Ptr<Node> pSkeleton = pWorld->getFirstNodeWithTag(L"SKELETON");
    Ptr<Node> pMesh = pWorld->getFirstNodeWithTag(L"MESH");
    Ptr<Node> pModel = pWorld->getFirstNodeWithTag(L"MODEL");
    Ptr<Node> pDecal = pWorld->getFirstNodeWithTag(L"DECAL");
    Ptr<Node> pBill = pWorld->getFirstNodeWithTag(L"BILLBOARD");
    Ptr<Node> pLight = pWorld->getFirstNodeWithTag(L"LIGHT");

    UTEST_NOT_EQU(pInst, null);
    UTEST_NOT_EQU(pSkeleton, null);
    UTEST_NOT_EQU(pMesh, null);
    UTEST_NOT_EQU(pModel, null);
    UTEST_NOT_EQU(pDecal, null);
    UTEST_NOT_EQU(pBill, null);
    UTEST_NOT_EQU(pLight, null);

    UTEST_EQU(pInst->getNodeType(), NODE_EMITTER);
    UTEST_EQU(pSkeleton->getNodeType(), NODE_SKELETON);
    UTEST_EQU(pMesh->getNodeType(), NODE_MESH);
    UTEST_EQU(pModel->getNodeType(), NODE_SKELETON);
    UTEST_EQU(pDecal->getNodeType(), NODE_DECAL);
    UTEST_EQU(pBill->getNodeType(), NODE_EMITTER);
    UTEST_EQU(pLight->getNodeType(), NODE_POINT_LIGHT);

    UTEST_EQU(pInst->isAlive(), true);
    UTEST_EQU(pSkeleton->isAlive(), true);
    UTEST_EQU(pMesh->isAlive(), true);
    UTEST_EQU(pModel->isAlive(), true);
    UTEST_EQU(pDecal->isAlive(), true);
    UTEST_EQU(pBill->isAlive(), true);
    UTEST_EQU(pLight->isAlive(), true);

    UTEST_EQU(pTask1->isFinished(), true);
    pTask1->kill();

    UTEST_EQU(pInst->isAlive(), false);
    UTEST_EQU(pSkeleton->isAlive(), false);
    UTEST_EQU(pMesh->isAlive(), false);
    UTEST_EQU(pModel->isAlive(), false);
    UTEST_EQU(pDecal->isAlive(), false);
    UTEST_EQU(pBill->isAlive(), false);
    UTEST_EQU(pLight->isAlive(), false);

    UTEST_END_TEST;
}

void testLuaNodeDecal(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node decal");

    Ptr<NodeDecal> pDecal = pWorld->createDecal();

    pDecal->setTexture(L"/Data/AssetsPacked/TUData/Textures/part00.tex");
    pDecal->setColor(Core::Vector4f(0.1f, 0.2f, 0.3f, 0.4f));
    pDecal->setGlow(Core::Vector4f(0.5f, 0.6f, 0.7f, 0.8f));
    pDecal->setMode(Renderer::DECAL_REFRAC);
    pDecal->setSize(2.0f, 3.0f);
    pDecal->setSpriteLayout(4, 8);
    pDecal->animateSprite(7.0f, 0.25f, 0.75f, true);
    pDecal->setTimeKey(0.6f);
    pDecal->addTag(L"DECAL1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodedecal-01.lua");

    Ptr<NodeDecal> pDecal2 = LM_DEBUG_PTR_CAST<NodeDecal>(pWorld->getFirstNodeWithTag(L"LUADECAL1"));

    UTEST_EQU(pDecal->getTexture(), pDecal2->getTexture());
    UTEST_EQU(pDecal->getMode(), pDecal2->getMode());
    UTEST_EQU(pDecal->getUSpriteLayout(), pDecal2->getUSpriteLayout());
    UTEST_EQU(pDecal->getVSpriteLayout(), pDecal2->getVSpriteLayout());
    UTEST_EQU(pDecal->getLoop(), pDecal2->getLoop());
    UTEST_EQU(pDecal->getWidth(), pDecal2->getWidth());
    UTEST_EQU(pDecal->getHeight(), pDecal2->getHeight());

    UTEST_EQU(pDecal->getColor().isEquivalent(pDecal2->getColor()), true);
    UTEST_EQU(pDecal->getGlow().isEquivalent(pDecal2->getGlow()), true);
    UTEST_EQU(pDecal->getKeyFrom(), pDecal2->getKeyFrom());
    UTEST_EQU(pDecal->getKeyTo(), pDecal2->getKeyTo());
    UTEST_EQU(pDecal->getDuration(), pDecal2->getDuration());

    UTEST_END_TEST;
}

void testLuaNodeMesh(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node mesh");

    Ptr<NodeMesh> pMesh1 = pWorld->createMesh(L"/Data/AssetsPacked/TUData/Models/Teapot.mdl", L"Teapot01");

    Assets::Material m;
    m.diffuseTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-diffuse.tex";
    m.normalTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-normal.tex";
    m.specularTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-specular.tex";
    m.emissive = Core::Vector4f(0.1f, 0.2f, 0.3f, 0.4f);
    m.diffuse = Core::Vector4f(0.1f, 0.9f, 0.3f, 0.4f);
    m.specular = Core::Vector4f(0.1f, 0.2f, 0.9f, 0.4f);
    m.glowColor = Core::Vector4f(0.1f, 0.2f, 0.3f, 0.9f);
    m.glowExtent = Core::Vector3f(1.0f, 2.0f, 3.0f);
    m.reflexions = Core::Vector4f(0.2f, 0.8f, 0.4f, 1.0f);
    m.refracColor = Core::Vector4f(0.8f, 0.7f, 0.6f, 0.5f);
    m.refracIndex = 0.5f;
    m.refracScale = 0.01f;
    m.shininess = 42.0f;
    m.flags = 0;

    pMesh1->setMaterial(m);
    pMesh1->addTag(L"MESH1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodemesh-01.lua");

    Ptr<NodeMesh> pMesh2 = LM_DEBUG_PTR_CAST<NodeMesh>(pWorld->getFirstNodeWithTag(L"LUAMESH1"));

    Assets::Material m1, m2;

    pMesh1->getMaterial(m1);
    pMesh2->getMaterial(m2);

    UTEST_EQU(m1, m2);

    UTEST_END_TEST;
}

void testLuaNodePointLight(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node point light");

    Ptr<NodePointLight> pLightA1 = pWorld->createPointLight();
    Ptr<NodePointLight> pLightB1 = pWorld->createPointLight();

    pLightA1->addTag(L"LIGHTA");
    pLightB1->addTag(L"LIGHTB");

    pLightA1->setAmbientColor(Core::Vector4f(1.0f, 0.5f, 0.2f, 0.3f));
    pLightA1->setDiffuseColor(Core::Vector4f(0.0f, 0.5f, 0.2f, 0.3f));
    pLightA1->setSpecularColor(Core::Vector4f(1.0f, 0.8f, 0.2f, 0.3f));
    pLightA1->setRadius(42.0f);

    pLightA1->setAmbientColor(Core::Vector4f(1.0f, 0.5f, 0.2f, 0.3f));
    pLightA1->setDiffuseColor(Core::Vector4f(0.0f, 0.5f, 0.2f, 0.3f));
    pLightA1->setSpecularColor(Core::Vector4f(1.0f, 0.8f, 0.2f, 0.3f));
    pLightA1->setRadius(42.0f);


    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodepointlight-01.lua");

    Ptr<NodePointLight> pLightA2 = LM_DEBUG_PTR_CAST<NodePointLight>(pWorld->getFirstNodeWithTag(L"LUALIGHTA"));
    Ptr<NodePointLight> pLightB2 = LM_DEBUG_PTR_CAST<NodePointLight>(pWorld->getFirstNodeWithTag(L"LUALIGHTB"));
    
    UTEST_EQU(pLightA1->getAmbientColor().isEquivalent(pLightA2->getAmbientColor()), true);
    UTEST_EQU(pLightA1->getDiffuseColor().isEquivalent(pLightA2->getDiffuseColor()), true);
    UTEST_EQU(pLightA1->getSpecularColor().isEquivalent(pLightA2->getSpecularColor()), true);

    UTEST_EQU(pLightA1->getRadius(), pLightA2->getRadius());

    UTEST_END_TEST;
}

void testLuaNodeSkeleton(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node mesh");

    Ptr<NodeSkeleton> pSkel1 = pWorld->createModel(L"/Data/AssetsPacked/TUData/Models/skinned.mdl");
    pSkel1->addTag(L"SKELETON1");

    Assets::Material m;
    m.diffuseTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-diffuse.tex";
    m.normalTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-normal.tex";
    m.specularTexName = L"/Data/AssetsPacked/TUData/Textures/stones01-specular.tex";
    m.emissive = Core::Vector4f(0.1f, 0.2f, 0.3f, 0.4f);
    m.diffuse = Core::Vector4f(0.1f, 0.9f, 0.3f, 0.4f);
    m.specular = Core::Vector4f(0.1f, 0.2f, 0.9f, 0.4f);
    m.glowColor = Core::Vector4f(0.1f, 0.2f, 0.3f, 0.9f);
    m.glowExtent = Core::Vector3f(1.0f, 2.0f, 3.0f);
    m.reflexions = Core::Vector4f(0.2f, 0.8f, 0.4f, 1.0f);
    m.refracColor = Core::Vector4f(0.8f, 0.7f, 0.6f, 0.5f);
    m.refracIndex = 0.5f;
    m.refracScale = 0.01f;
    m.shininess = 42.0f;
    m.flags = 0;

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodeskeleton-01.lua");

    UTEST_END_TEST;
}

void testLuaNodeEmitter(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node emitter");

    Ptr<NodeEmitter> pEmitter1 = pWorld->createBillboardEmitter(L"/Data/AssetsPacked/TUData/Textures/part00.tex", Renderer::BILLBOARD_ADD);

    pEmitter1->setPeriod(1.0, 2.0);
    pEmitter1->setStickyParticles(false);
    pEmitter1->killAtEnd(true);
    pEmitter1->setGenerationTime(0.0, 5.0);
    pEmitter1->addTag(L"EMITTER");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodeemitter-01.lua");

    Ptr<NodeEmitter> pEmitter2 = LM_DEBUG_PTR_CAST<NodeEmitter>(pWorld->getFirstNodeWithTag(L"LUAEMITTER"));

    float  p11, p12, p21, p22;
    double g11, g12, g21, g22;

    pEmitter1->getPeriod(p11, p12);
    pEmitter2->getPeriod(p21, p22);

    pEmitter1->getGenerationTime(g11, g12);
    pEmitter2->getGenerationTime(g21, g22);

    UTEST_EQU(p11, p21);
    UTEST_EQU(p12, p22);
    UTEST_EQU(g11, g21);
    UTEST_EQU(g12, g22);

    UTEST_EQU(pEmitter1->killAtEnd(), pEmitter2->killAtEnd());
    UTEST_EQU(pEmitter1->getStickyParticles(), pEmitter2->getStickyParticles());

    UTEST_END_TEST;
}

void testLuaNodeStorm(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node storm");

    Ptr<Node> pNode = pWorld->createNode();
    Ptr<NodeStorm> pStorm1 = pWorld->createStorm();

    pStorm1->killAtEnd(true);
    pStorm1->setHeadSphereCenter(Core::Vector3f(1.0f, 0.0, 0.0f));
    pStorm1->setHeadSphereSize(Core::Vector3f(0.0f, 2.0, 0.0f));
    pStorm1->setTailSphereCenter(Core::Vector3f(1.0f, 0.0, 3.0f));
    pStorm1->setTailSphereSize(Core::Vector3f(1.0f, 0.0, 4.0f));
    pStorm1->setPeriod(0.5f, 1.0f);
    pStorm1->setGenerationTime(0.25, 0.75);
    pStorm1->setTailNode(pNode);
    pStorm1->addTag(L"STORM1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodestorm-01.lua");

    Ptr<NodeStorm> pStorm2 = LM_DEBUG_PTR_CAST<NodeStorm>(pWorld->getFirstNodeWithTag(L"LUASTORM1"));

    float  pmin1, pmin2, pmax1, pmax2;
    double gmin1, gmin2, gmax1, gmax2;

    pStorm1->getPeriod(pmin1, pmax1);
    pStorm2->getPeriod(pmin2, pmax2);
    pStorm1->getGenerationTime(gmin1, gmax1);
    pStorm2->getGenerationTime(gmin2, gmax2);

    UTEST_EQUF(pmin1, pmin2);
    UTEST_EQUF(pmax1, pmax2);
    UTEST_EQUF(gmin1, gmin2);
    UTEST_EQUF(gmax1, gmax2);
    UTEST_EQU(pStorm1->killAtEnd(), pStorm2->killAtEnd());
    UTEST_EQU(pStorm1->getTailNode(), pStorm2->getTailNode());
    UTEST_EQU(pStorm1->getHeadSphereCenter().isEquivalent(pStorm2->getHeadSphereCenter()), true);
    UTEST_EQU(pStorm1->getHeadSphereSize().isEquivalent(pStorm2->getHeadSphereSize()), true);
    UTEST_EQU(pStorm1->getTailSphereCenter().isEquivalent(pStorm2->getTailSphereCenter()), true);
    UTEST_EQU(pStorm1->getTailSphereSize().isEquivalent(pStorm2->getTailSphereSize()), true);

    UTEST_END_TEST;
}

void testLuaNodeTrail(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node trail");

    Ptr<NodeTrail> pTrail1 = pWorld->createTrail(L"/Data/AssetsPacked/TUData/Textures/part00.tex", Renderer::FREE_FORM_LERP);

    pTrail1->setLength(13.0f);
    pTrail1->setDivisions(17);
    pTrail1->setHeadWidth(2.5f);
    pTrail1->setTailWidth(0.7f);
    pTrail1->setHeadU(0.25f);
    pTrail1->setTailU(0.25f);
    pTrail1->setHeadColor(Core::Vector4f(1.0f, 2.0f, 3.0f, 4.0f));
    pTrail1->setTailColor(Core::Vector4f(5.0f, 6.0f, 7.0f, 8.0f));
    pTrail1->setHeadGlow(Core::Vector4f(4.0f, 3.0f, 2.0f, 1.0f));
    pTrail1->setTailGlow(Core::Vector4f(8.0f, 7.0f, 6.0f, 5.0f));
    pTrail1->addTag(L"TRAIL1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodetrail-01.lua");

    Ptr<NodeTrail> pTrail2 = LM_DEBUG_PTR_CAST<NodeTrail>(pWorld->getFirstNodeWithTag(L"LUATRAIL1"));

    UTEST_EQU(pTrail1->getLength(), pTrail2->getLength());
    UTEST_EQU(pTrail1->getDivisions(), pTrail2->getDivisions());
    UTEST_EQU(pTrail1->getHeadWidth(), pTrail2->getHeadWidth());
    UTEST_EQU(pTrail1->getTailWidth(), pTrail2->getTailWidth());
    UTEST_EQU(pTrail1->getHeadU(), pTrail2->getHeadU());
    UTEST_EQU(pTrail1->getTailU(), pTrail2->getTailU());
    UTEST_EQU(pTrail1->getHeadColor().isEquivalent(pTrail2->getHeadColor()), true);
    UTEST_EQU(pTrail1->getHeadGlow().isEquivalent(pTrail2->getHeadGlow()), true);
    UTEST_EQU(pTrail1->getTailColor().isEquivalent(pTrail2->getTailColor()), true);
    UTEST_EQU(pTrail1->getTailGlow().isEquivalent(pTrail2->getTailGlow()), true);

    UTEST_END_TEST;
}

void testLuaNodeSoundSource(const Ptr<World> & pWorld)
{
    //-------------------------------------------------------------------------

    UTEST_START_TEST(L"Node sound source");

    Ptr<NodeSoundSource> pSound1 = pWorld->createSoundSource();

    pSound1->setGain(0.8f);
    pSound1->killAtEnd(true);
    pSound1->setPause(true);
    pSound1->setStaticSound(true);
    pSound1->addTag(L"SOUND1");

    Ptr<ITask> pTask1 = pWorld->runScript(L"/Data/TUData/Scripts/tu-nodesoundsource-01.lua");

    Ptr<NodeSoundSource> pSound2 = LM_DEBUG_PTR_CAST<NodeSoundSource>(pWorld->getFirstNodeWithTag(L"LUASOUND1"));

    UTEST_EQU(pSound1->getGain(),           pSound2->getGain());
    UTEST_EQU(pSound1->killAtEnd(),         pSound2->killAtEnd());
    UTEST_EQU(pSound1->getPaused(),         pSound2->getPaused());
    UTEST_EQU(pSound1->isStaticSound(),     pSound2->isStaticSound());

    UTEST_END_TEST;
}

void testLua()
{
    Ptr<Renderer::IRenderer> pRenderer(new Renderer::RendererStub());
    Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
    pVFS->linkDirectory(L".");

    Ptr<Audio::IDriver> pAudioDriver (new Audio::DriverAudioStub());
    Ptr<World> pWorld(new World(pRenderer, pVFS, null, pAudioDriver->createDevice(0)));

    UTEST_PACKAGE("Lua");
#if TEST_MEM_LEAK
    for(int32 itera=0; itera < 1000000; itera++)
    {
#endif
        
        testLuaNodeStorm(pWorld);
        testLuaNodeTrail(pWorld);
        testLuaNodeEmitter(pWorld);
        testLuaNodePointLight(pWorld);
        testLuaNodeMesh(pWorld);
        testLuaNodeSkeleton(pWorld);
        testLuaWorld(pWorld);
        testLuaNode(pWorld);
        testLuaNodeTree(pWorld);
        testLuaNodeDecal(pWorld);
        testLuaNodeSoundSource(pWorld);
        testLuaScheduling(pWorld);
        testLuaTasksTransform(pWorld);
        testLuaTasks(pWorld);
 
#if TEST_MEM_LEAK
    }
#endif
}