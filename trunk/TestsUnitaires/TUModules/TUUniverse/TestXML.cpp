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
#include <Core/Logger.h>
#include <Core/IO/BufferStream.h>
#include <Core/XML/XMLDocument.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/VFS/VFSXMLTranslator.h>
#include <Universe/World.h>
#include <Renderer/Stub/RendererStub.h>

using namespace Core;
using namespace Renderer;
using namespace Universe;

void testImportExportNode(const Ptr<World> & pWorld)
{
    BufferStream buffer;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Import/export Node");

    Core::Matrix4f matNode1;
    Core::Matrix4f matNode2;
    Core::Matrix4f matMesh1;
    Core::Matrix4f matMesh2;

    Core::Vector3f posNode1;
    Core::Vector3f posNode2;
    Core::Vector3f posMesh1;
    Core::Vector3f posMesh2;

    Core::Vector3f scaleNode1;
    Core::Vector3f scaleNode2;
    Core::Vector3f scaleMesh1;
    Core::Vector3f scaleMesh2;

    Core::Quaternionf oriNode1;
    Core::Quaternionf oriNode2;
    Core::Quaternionf oriMesh1;
    Core::Quaternionf oriMesh2;

    Core::Vector3f upNode1;
    Core::Vector3f upNode2;
    Core::Vector3f upMesh1;
    Core::Vector3f upMesh2;

    {
        Ptr<Node> pNode1(pWorld->createNode());
        Ptr<Node> pNode2(pWorld->createNode());

        Ptr<NodeMesh> pMesh1(pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01"));
        Ptr<NodeMesh> pMesh2(pWorld->createMesh());

        pNode1->addChild(pNode2);
        pNode2->addChild(pMesh1);
        pMesh1->addChild(pMesh2);

        pNode1->beginMatrixUpdate();
        pNode1->setWorldPosition(Core::Vector3f(100.0, 200.0, 300.0));
        pNode1->localPitch(f_PI_DIV_2);
        pNode1->endMatrixUpdate();

        pNode2->setUpVector(Core::Vector3f(1.0, 1.0, 0.0).getNormalizedSafe());
        pNode2->beginMatrixUpdate();
        pNode2->setWorldDirection(Core::Vector3f(1.0, 0.0, 0.0));
        pNode2->endMatrixUpdate();

        pMesh1->beginMatrixUpdate();
        pMesh1->setWorldPosition(Core::Vector3f(-300.0, -200.0, -100.0));
        pMesh1->localYaw(f_PI_DIV_2);
        pMesh1->endMatrixUpdate();

        pMesh2->setUpVector(Core::Vector3f(0.0, -1.0, 0.0));
        pMesh2->beginMatrixUpdate();
        pMesh2->setLocalScale(Core::Vector3f(1.0, 2.0, 3.0));
        pMesh2->worldLookAt(Core::Vector3f(100.0, 0.0, 0.0));
        pMesh2->endMatrixUpdate();

        matNode1 = pNode1->getLocalToWorldMatrix();
        matNode2 = pNode2->getLocalToWorldMatrix();
        matMesh1 = pMesh1->getLocalToWorldMatrix();
        matMesh2 = pMesh2->getLocalToWorldMatrix();

        posNode1 = pNode1->getLocalPosition();
        posNode2 = pNode2->getLocalPosition();
        posMesh1 = pMesh1->getLocalPosition();
        posMesh2 = pMesh2->getLocalPosition();

        scaleNode1 = pNode1->getLocalScale();
        scaleNode2 = pNode2->getLocalScale();
        scaleMesh1 = pMesh1->getLocalScale();
        scaleMesh2 = pMesh2->getLocalScale();

        oriNode1 = pNode1->getLocalOrient();
        oriNode2 = pNode2->getLocalOrient();
        oriMesh1 = pMesh1->getLocalOrient();
        oriMesh2 = pMesh2->getLocalOrient();

        upNode1 = pNode1->getUpVector();
        upNode2 = pNode2->getUpVector();
        upMesh1 = pMesh1->getUpVector();
        upMesh2 = pMesh2->getUpVector();

        Ptr<XMLNode> pXMLNode(new XMLNode(L"node"));
        pWorld->exportXML(pXMLNode, pNode1);

        Core::XMLDocument doc;
        doc.addNode(L"root", pXMLNode);
        doc.saveDocument(buffer);
        doc.saveDocument(L"Local/Test.xml");
    }

    {
        String value;
        Core::XMLDocument doc;
        if(!doc.loadDocument(L"Local/Test.xml"))
        {
            ERR << L"Unable to reload node XML\n";
            return;
        }

        List<Ptr<XMLNode> > nodes;
        doc.getNodes(L"root", L"node", nodes);

        if(nodes.size() != 1)
        {
            ERR << L"Incorrect XML hierarchy 1\n";
            UTEST_TEST_FAILED;
            return;
        }

        Ptr<Node> pNode1(pWorld->importXML(nodes[0]));

        UTEST_EQU(pNode1->getChildCount(), 1);
        Ptr<Node> pNode2(pNode1->getChild(0));

        UTEST_EQU(pNode2->getChildCount(), 1);
        Ptr<Node> pNode3(pNode2->getChild(0));

        UTEST_EQU(pNode1->getNodeType(), NODE);
        UTEST_EQU(pNode2->getNodeType(), NODE);
        UTEST_EQU(pNode3->getNodeType(), NODE_MESH);
        Ptr<NodeMesh> pMesh1 = LM_DEBUG_PTR_CAST<NodeMesh>(pNode3);

        UTEST_EQU(pMesh1->getChildCount(), 1);
        Ptr<Node> pNode4(pMesh1->getChild(0));

        UTEST_EQU(pNode4->getNodeType(), NODE_MESH);
        Ptr<NodeMesh> pMesh2 = LM_DEBUG_PTR_CAST<NodeMesh>(pNode4);
        UTEST_EQU(pMesh2->getChildCount(), 0);

        UTEST_EQU(pNode1->getLocalPosition().isEquivalent(posNode1), true);
        UTEST_EQU(pNode2->getLocalPosition().isEquivalent(posNode2), true);
        UTEST_EQU(pMesh1->getLocalPosition().isEquivalent(posMesh1), true);
        UTEST_EQU(pMesh2->getLocalPosition().isEquivalent(posMesh2), true);

        UTEST_EQU(pNode1->getLocalOrient().isEquivalent(oriNode1), true);
        UTEST_EQU(pNode2->getLocalOrient().isEquivalent(oriNode2), true);
        UTEST_EQU(pMesh1->getLocalOrient().isEquivalent(oriMesh1), true);
        UTEST_EQU(pMesh2->getLocalOrient().isEquivalent(oriMesh2), true);

        UTEST_EQU(pNode1->getLocalScale().isEquivalent(scaleNode1), true);
        UTEST_EQU(pNode2->getLocalScale().isEquivalent(scaleNode2), true);
        UTEST_EQU(pMesh1->getLocalScale().isEquivalent(scaleMesh1), true);
        UTEST_EQU(pMesh2->getLocalScale().isEquivalent(scaleMesh2), true);

        UTEST_EQU(pNode1->getLocalToWorldMatrix().isEquivalent(matNode1), true);
        UTEST_EQU(pNode2->getLocalToWorldMatrix().isEquivalent(matNode2), true);
        UTEST_EQU(pMesh1->getLocalToWorldMatrix().isEquivalent(matMesh1), true);
        UTEST_EQU(pMesh2->getLocalToWorldMatrix().isEquivalent(matMesh2), true);

        UTEST_EQU(pNode1->getUpVector().isEquivalent(upNode1), true);
        UTEST_EQU(pNode2->getUpVector().isEquivalent(upNode2), true);
        UTEST_EQU(pMesh1->getUpVector().isEquivalent(upMesh1), true);
        UTEST_EQU(pMesh2->getUpVector().isEquivalent(upMesh2), true);
    }

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Static model instance");

    BufferStream buffer;
    {
        Ptr<Universe::Node> pInst1 = pWorld->createInstance(L"/TUData/Models/teapot.mdl");
        Ptr<Universe::Node> pNodeTeapot1 = pInst1->getChildWithTag(L"Teapot01", true);

        pInst1->addTag(L"INST1");

        UTEST_EQU(pInst1->getNodeType(), Universe::NODE_SKELETON);
        UTEST_NOT_EQU(pNodeTeapot1, null);

        Ptr<XMLNode> pXMLNode(new XMLNode(L"node"));
        pWorld->exportXML(pXMLNode, pInst1);

        Ptr<Core::XMLDocument> pDoc1(new Core::XMLDocument());
        pDoc1->addNode(L"root", pXMLNode);
        pDoc1->saveDocument(buffer);
        pDoc1->saveDocument(L"./Local/tmp.xml");

        Ptr<Universe::Node> pSNode2 = pWorld->importXML(pXMLNode);

        UTEST_EQU(pSNode2->getNodeType(), Universe::NODE_SKELETON);

        Ptr<Universe::Node> pNodeTeapot2 = pSNode2->getChildWithTag(L"Teapot01", true);

        UTEST_EQU(pSNode2->getNodeType(), Universe::NODE_SKELETON);
        UTEST_NOT_EQU(pNodeTeapot2, null);
    }


    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Skinnel model instance");

    BufferStream buffer;
    {
        Ptr<Universe::Node> pInst1 = pWorld->createInstance(L"/TUData/Models/skinned.mdl");
        Ptr<Universe::Node> pNodeSkin1 = pInst1->getChildWithTag(L"CylinderThing", true);
        pInst1->addTag(L"INST1");

        UTEST_EQU(pInst1->getNodeType(), Universe::NODE_SKELETON);
        UTEST_NOT_EQU(pNodeSkin1, null);

        Ptr<XMLNode> pXMLNode(new XMLNode(L"node"));
        pWorld->exportXML(pXMLNode, pInst1);

        Ptr<Core::XMLDocument> pDoc1(new Core::XMLDocument());
        pDoc1->addNode(L"root", pXMLNode);
        pDoc1->saveDocument(buffer);

        Ptr<Universe::Node> pSNode2 = pWorld->importXML(pXMLNode);

        UTEST_EQU(pSNode2->getNodeType(), Universe::NODE_SKELETON);

        Ptr<Universe::Node> pInst2 = LM_DEBUG_PTR_CAST<Universe::Node>(pSNode2);
        Ptr<Universe::Node> pNodeSkin2 = pInst2->getChildWithTag(L"CylinderThing", true);

        UTEST_NOT_EQU(pNodeSkin2, null);
    }

    UTEST_END_TEST;


    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Instance with child");

    BufferStream buffer;
    {
        Ptr<Universe::Node> pInst1 = pWorld->createInstance(L"/TUData/Models/teapot.mdl");

        Ptr<Universe::Node> pNode1 = pWorld->createNode();
        Ptr<Universe::Node> pNode2 = pWorld->createNode();

        pNode1->addTag(L"TAG1");
        pNode2->addTag(L"TAG2");

        pNode1->setLocalScale(Core::Vector3f(1.0f, 2.0f, 3.0f));
        pNode2->setLocalScale(Core::Vector3f(4.0f, 5.0f, 6.0f));

        pInst1->addChild(pNode1);
        pInst1->addChild(pNode2);

        Ptr<XMLNode> pXMLNode(new XMLNode(L"node"));
        pWorld->exportXML(pXMLNode, pInst1);

        Ptr<Core::XMLDocument> pDoc1(new Core::XMLDocument());
        pDoc1->addNode(L"root", pXMLNode);
        pDoc1->saveDocument(buffer);

        Ptr<Universe::Node> pInst2 = pWorld->importXML(pXMLNode);

        Ptr<Universe::Node> pInst12 = pInst2->getChildWithTag(L"TAG1");
        Ptr<Universe::Node> pInst22 = pInst2->getChildWithTag(L"TAG2");

        UTEST_NOT_EQU(pInst12, null);
        UTEST_NOT_EQU(pInst22, null);

        UTEST_EQU(pInst12->getLocalScale().isEquivalent(pNode1->getLocalScale()), true);
        UTEST_EQU(pInst22->getLocalScale().isEquivalent(pNode2->getLocalScale()), true);
    }


    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"XML instance");

    BufferStream buffer;

    {
        Ptr<Universe::Node> pInst1 = pWorld->createInstance(L"/TUDataRaw/Universe/Flame00.node.xml");
        Ptr<Universe::Node> pNode1 = pInst1;
        UTEST_EQU(pNode1->getNodeType(), Universe::NODE_EMITTER);

        Ptr<XMLNode> pXMLNode(new XMLNode(L"node"));
        pWorld->exportXML(pXMLNode, pInst1);

        Ptr<Core::XMLDocument> pDoc1(new Core::XMLDocument());
        pDoc1->addNode(L"root", pXMLNode);
        pDoc1->saveDocument(buffer);

        Ptr<Universe::Node> pSNode2 = pWorld->importXML(pXMLNode);
        UTEST_EQU(pSNode2->getNodeType(), Universe::NODE_EMITTER);

        Ptr<Universe::Node> pInst2 = LM_DEBUG_PTR_CAST<Universe::Node>(pSNode2);
        Ptr<Universe::Node> pNode2 = pInst2;
        UTEST_EQU(pNode2->getNodeType(), Universe::NODE_EMITTER);
    }

    UTEST_END_TEST;
}



void testXML()
{
    UTEST_PACKAGE("Import/export XML");

    Ptr<Core::VirtualFileSystem> pVFS;
    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-TUUniverse.xml"))
            pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }

    Ptr<IRenderer> pRenderer(new RendererStub());
    Ptr<World> pWorld(new World(pRenderer, pVFS));
    testImportExportNode(pWorld);
}

