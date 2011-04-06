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
#include <Core/Math/MathTools.h>
#include <Core/Math/Random.h>
#include <Core/IO/BufferStream.h>

#include <Assets/Animation.h>
#include <Assets/VerticeSet.h>
#include <Assets/ModelMesh.h>
#include <Assets/Model.h>
#include <Assets/SkeletonPose.h>

#include <Workflow/ModelOptimize.h>

using namespace Core;
using namespace Assets;

void fillVertices(List<Vertex> & vert, List<uint16> & ind, int32 vertCount)
{
    vert.resize(vertCount);

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        vert[ii].boneId[0] = ii;
        vert[ii].boneId[1] = ii + 1;
        vert[ii].boneWeight[0] = vert[ii].boneWeight[1] = 0.5;
    }

    for(int32 ii=0; ii < vert.size() - 2; ii++)
    {
        ind.push_back(ii + 0);
        ind.push_back(ii + 1);
        ind.push_back(ii + 2);
    }
}

void fillVertices(VerticeSet & vset, int32 vertCount)
{
    List<Vertex> & vert = vset.getVertices();
    List<uint16> & ind = vset.getIndices();
    fillVertices(vert, ind, vertCount);
}


void testModelMeshBasic()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"ModelMesh");

    Assets::ModelMesh mesh1(0, L"Mesh00");
    Assets::ModelMesh mesh2(0, L"Mesh00");

    UTEST_EQU(mesh1.getName(), L"Mesh00");
    UTEST_EQU(mesh1.isSkin(), false);
    UTEST_EQU(mesh1.getLODCount(), 1);
    UTEST_EQU(mesh1, mesh2);
    fillVertices(*mesh1.getLOD(Assets::LOD_HIGH), 10);
    mesh1.cleanup();
    UTEST_NOT_EQU(mesh1, mesh2);

    Core::BufferStream buffer;
    mesh1.write(buffer); mesh2.read(buffer);
    UTEST_EQU(mesh1, mesh2);

    mesh1.setLODCount(2);
    UTEST_EQU(mesh1.getLODCount(), 2);
    UTEST_NOT_EQU(mesh1, mesh2);
    fillVertices(*mesh1.getLOD(Assets::LOD_HIGH), 5);
    mesh1.cleanup();

    mesh1.write(buffer); mesh2.read(buffer);
    UTEST_EQU(mesh2.getLODCount(), 2);
    UTEST_EQU(mesh1, mesh2);

    mesh1.setSkin(true);
    UTEST_NOT_EQU(mesh1, mesh2);
    mesh1.write(buffer); mesh2.read(buffer);
    UTEST_EQU(mesh2.isSkin(), true);
    UTEST_EQU(mesh1, mesh2);

    mesh1.setName(L"Toto");
    UTEST_NOT_EQU(mesh1, mesh2);
    mesh1.write(buffer); mesh2.read(buffer);
    UTEST_EQU(mesh2.getName(), L"Toto");
    UTEST_EQU(mesh1, mesh2);

    Assets::ModelMesh mesh3;
    mesh3.setLODCount(1);
    UTEST_EQU(mesh3.getLOD(Assets::LOD_HIGH), mesh3.getLOD(Assets::LOD_MEDIUM));

    UTEST_END_TEST;
}

void testMeshBoundingBox()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Mesh bounding box");

    Assets::ModelMesh mesh(0, L"Mesh00");
    List<Vertex> & vert = mesh.getLOD(LOD_HIGH)->getVertices();

    vert.resize(4);
    vert[0].position = Vector3f(1.0, 0.0, 3.0);
    vert[1].position = Vector3f(0.0, 1.0, 2.0);
    vert[2].position = Vector3f(0.5, 2.0, 0.0);
    vert[3].position = Vector3f(0.0, 1.0, 1.0);

    mesh.cleanup();
    AABoxf box = mesh.getBoundingBox();

    UTEST_EQU(box.min._x, 0.0);
    UTEST_EQU(box.min._y, 0.0);
    UTEST_EQU(box.min._z, 0.0);
    UTEST_EQU(box.max._x, 1.0);
    UTEST_EQU(box.max._y, 2.0);
    UTEST_EQU(box.max._z, 3.0);

    UTEST_END_TEST;
}

void testMeshLib(Assets::Model & model)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Model mesh lib");

    UTEST_EQU(model.getNodeCount(), 1);
    UTEST_EQU(model.getMeshCount(), 0);
    UTEST_EQU(model.getAnimationCount(), 0);

    UTEST_EQU(model.getNode(Assets::ROOT_NODE_ID), model.getRootNode());
    UTEST_EQU(model.getRootNode().getId(), Assets::ROOT_NODE_ID);
    UTEST_EQU(model.getRootNode().getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(model.getRootNode().getName(), L"Root");
    UTEST_EQU(model.getMeshId(L"NoMesh"), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(model.getNodeId(L"NoNode"), Assets::UNDEFINED_NODE_ID);
    UTEST_EQU(model.getRootNode().getSubNodeCount(), 0);

    Assets::ModelMesh mesh1(666, L"UnamedMesh");
    fillVertices(*mesh1.getLOD(Assets::LOD_HIGH), 10);
    int32 id1 = model.addMesh(L"MeshTest", mesh1);

    UTEST_EQU(id1, 0);
    UTEST_EQU(model.getMeshCount(), 1);
    UTEST_EQU(model.getNodeCount(), 1);
    UTEST_EQU(model.getAnimationCount(), 0);

    const Assets::ModelMesh & rMesh1 = model.getMesh(id1);

    UTEST_EQU(rMesh1.getName(), L"MeshTest");
    UTEST_EQU(rMesh1, model.getMesh(model.getMeshId(L"MeshTest")));
    UTEST_EQU(*rMesh1.getLOD(Assets::LOD_MEDIUM), *mesh1.getLOD(Assets::LOD_MEDIUM));


    Assets::ModelMesh mesh2(123, L"Other");
    fillVertices(*mesh2.getLOD(Assets::LOD_HIGH), 12);
    int32 id2 = model.addMesh(L"MeshTest", mesh2);

    UTEST_EQU(id2, 1);
    UTEST_EQU(model.getMeshCount(), 2);

    const Assets::ModelMesh & rMesh2 = model.getMesh(id2);

    UTEST_EQU(rMesh2.getName(), L"MeshTest00");
    UTEST_EQU(rMesh2, model.getMesh(model.getMeshId(L"MeshTest00")));
    UTEST_EQU(*rMesh2.getLOD(Assets::LOD_MEDIUM), *mesh2.getLOD(Assets::LOD_MEDIUM));
    UTEST_NOT_EQU(*rMesh2.getLOD(Assets::LOD_MEDIUM), *rMesh1.getLOD(Assets::LOD_MEDIUM));


    Assets::ModelMesh mesh3(123, L"Yet another");
    fillVertices(*mesh3.getLOD(Assets::LOD_HIGH), 13);
    int32 id3 = model.addMesh(L"MeshTest", mesh3);

    UTEST_EQU(id3, 2);
    UTEST_EQU(model.getMeshCount(), 3);

    const Assets::ModelMesh & rMesh3 = model.getMesh(id3);

    UTEST_EQU(rMesh3.getName(), L"MeshTest01");
    UTEST_EQU(rMesh3, model.getMesh(model.getMeshId(L"MeshTest01")));
    UTEST_EQU(*rMesh3.getLOD(Assets::LOD_MEDIUM), *mesh3.getLOD(Assets::LOD_MEDIUM));
    UTEST_NOT_EQU(*rMesh3.getLOD(Assets::LOD_MEDIUM), *rMesh2.getLOD(Assets::LOD_MEDIUM));

    UTEST_END_TEST;
}

void testModelHierarchy(Assets::Model & model, int32 & idNodeA, int32 & idNodeB,
                        int32 & idNodeBA, int32 & idNodeBB)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Model hierarchy");

    idNodeA  = model.addNode(L"NodeA");
    idNodeB  = model.addNode(L"NodeB");
    idNodeBA = model.addNode(L"NodeBA", idNodeB);
    idNodeBB = model.addMeshNode(L"NodeBB", idNodeB, 0);

    UTEST_EQU(model.getNodeCount(), 5);

    const ModelNode & rRoot = model.getRootNode();
    UTEST_EQU(rRoot.getId(), Assets::ROOT_NODE_ID);
    UTEST_EQU(rRoot.getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(rRoot.getName(), L"Root");
    UTEST_EQU(rRoot.getSubNodeCount(), 2);

    const ModelNode & rNodeA = rRoot.getSubNode(0);
    UTEST_EQU(rNodeA, model.getNode(idNodeA));
    UTEST_EQU(rNodeA.getId(), idNodeA);
    UTEST_EQU(rNodeA.getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(rNodeA.getName(), L"NodeA");
    UTEST_EQU(rNodeA.getSubNodeCount(), 0);

    const ModelNode & rNodeB = rRoot.getSubNode(1);
    UTEST_EQU(rNodeB, model.getNode(idNodeB));
    UTEST_EQU(rNodeB.getId(), idNodeB);
    UTEST_EQU(rNodeB.getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(rNodeB.getName(), L"NodeB");
    UTEST_EQU(rNodeB.getSubNodeCount(), 2);

    const ModelNode & rNodeBA = rNodeB.getSubNode(0);
    UTEST_EQU(rNodeBA, model.getNode(idNodeBA));
    UTEST_EQU(rNodeBA.getId(), idNodeBA);
    UTEST_EQU(rNodeBA.getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(rNodeBA.getName(), L"NodeBA");
    UTEST_EQU(rNodeBA.getSubNodeCount(), 0);

    const ModelNode & rNodeBB = rNodeB.getSubNode(1);
    UTEST_EQU(rNodeBB, model.getNode(idNodeBB));
    UTEST_EQU(rNodeBB.getId(), idNodeBB);
    UTEST_EQU(rNodeBB.getMeshId(), 0);
    UTEST_EQU(rNodeBB.getName(), L"NodeBB");
    UTEST_EQU(rNodeBB.getSubNodeCount(), 0);

    UTEST_END_TEST;
}

void testModelMesh(Assets::Model & model, int32 & idNodeA, int32 & idNodeB,
                   int32 & idNodeBA, int32 & idNodeBB)
{
    testModelMeshBasic();
    testMeshBoundingBox();
    testMeshLib(model);
    testModelHierarchy(model, idNodeA, idNodeB, idNodeBA, idNodeBB);
}
