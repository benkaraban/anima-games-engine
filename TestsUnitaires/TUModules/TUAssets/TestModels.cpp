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

void testVSet();
void testModelMesh(Assets::Model & model, int32 & idNodeA, int32 & idNodeB,
                   int32 & idNodeBA, int32 & idNodeBB);
void testModelAnim(Assets::Model & model, Assets::Animation & anim1);

void testModels()
{
    UTEST_PACKAGE("Models");

    Assets::Model model;
    Assets::Animation anim1(5);
    int32 idNodeA  = 0;
    int32 idNodeB  = 0;
    int32 idNodeBA = 0;
    int32 idNodeBB = 0;

    testVSet();
    testModelMesh(model, idNodeA, idNodeB, idNodeBA, idNodeBB);
    testModelAnim(model, anim1);

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Model serialisation");

    Assets::Model model2;
    Core::BufferStream buffer;

    model.write(buffer);
    model2.read(buffer);

    UTEST_EQU(model2.getNodeCount(), 5);

    const ModelNode & rRoot = model2.getRootNode();
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

    const Assets::ModelMesh & rMesh3 = model2.getMesh(2);
    model.cleanup();

    UTEST_EQU(model.getMeshCount(), 3);
    UTEST_EQU(rMesh3.getName(), L"MeshTest01");
    UTEST_EQU(rMesh3, model.getMesh(model.getMeshId(L"MeshTest01")));

    UTEST_EQU(model2.getAnimationCount(), 1);
    UTEST_EQU(model2.getAnimationName(0), L"Anim01");

    Assets::Skeleton & skeletor = model2.getSkeleton();
    Assets::ModelPose pose;
    Assets::SkeletonPose skelPose;

    model2.getPose(pose, 0, 0.60f);
    skeletor.getSkeletonPose(skelPose, pose);

    UTEST_EQU(skelPose.getBoneMatrix(0).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 1.0, 0.0)), true);
    UTEST_EQU(skelPose.getBoneMatrix(1).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 2.0, 0.0)), true);
    UTEST_EQU(skelPose.getBoneMatrix(2).isEquivalent(Matrix4f(Core::TRANSLATION, 4.5, 0.0, 3.0)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Bones weight normalisation");

    Assets::Model boneModel;

    ModelMesh mesh;
    mesh.setLODCount(1);
    {
        VerticeSet & vset = *mesh.getLOD(Assets::LOD_HIGH);
        List<Vertex> & vert = vset.getVertices();

        vert.resize(4);
        vert[0].boneId[0] = 0; vert[0].boneWeight[0] = 0.1f;
        vert[0].boneId[1] = 1; vert[0].boneWeight[1] = 0.3f;
        vert[0].boneId[2] = 2; vert[0].boneWeight[2] = 0.1f;
        vert[0].boneId[3] = 3; vert[0].boneWeight[3] = 0.2f;

        vert[1].boneId[0] = 0; vert[1].boneWeight[0] = 0.7f;
        vert[1].boneId[1] = 1; vert[1].boneWeight[1] = 0.0f;
        vert[1].boneId[2] = 2; vert[1].boneWeight[2] = 0.3f;
        vert[1].boneId[3] = 3; vert[1].boneWeight[3] = 0.2f;

        vert[2].boneId[0] = 0; vert[2].boneWeight[0] = 1.0f;
        vert[2].boneId[1] = 1; vert[2].boneWeight[1] = 0.0f;
        vert[2].boneId[2] = 2; vert[2].boneWeight[2] = 3.0f;
        vert[2].boneId[3] = 3; vert[2].boneWeight[3] = 0.0f;

        vert[3].boneId[0] = 0; vert[3].boneWeight[0] = 0.0f;
        vert[3].boneId[1] = 1; vert[3].boneWeight[1] = 0.5f;
        vert[3].boneId[2] = 2; vert[3].boneWeight[2] = 0.0f;
        vert[3].boneId[3] = 3; vert[3].boneWeight[3] = 0.0f;
    }
    boneModel.addMesh(L"Mesh00", mesh);
    Workflow::normalizeBonesWeights(boneModel, 2);

    VerticeSet & vset = *boneModel.getMesh(0).getLOD(Assets::LOD_HIGH);
    List<Vertex> & vert = vset.getVertices();

    UTEST_EQU(vert[0].boneId[0], 1); UTEST_EQUF(vert[0].boneWeight[0], 0.6);
    UTEST_EQU(vert[0].boneId[1], 3); UTEST_EQUF(vert[0].boneWeight[1], 0.4);
    UTEST_EQU(vert[0].boneId[2], 0); UTEST_EQUF(vert[0].boneWeight[2], 0.0);
    UTEST_EQU(vert[0].boneId[3], 0); UTEST_EQUF(vert[0].boneWeight[3], 0.0);

    UTEST_EQU(vert[1].boneId[0], 0); UTEST_EQUF(vert[1].boneWeight[0], 0.7);
    UTEST_EQU(vert[1].boneId[1], 2); UTEST_EQUF(vert[1].boneWeight[1], 0.3);
    UTEST_EQU(vert[1].boneId[2], 0); UTEST_EQUF(vert[1].boneWeight[2], 0.0);
    UTEST_EQU(vert[1].boneId[3], 0); UTEST_EQUF(vert[1].boneWeight[3], 0.0);

    UTEST_EQU(vert[2].boneId[0], 2); UTEST_EQUF(vert[2].boneWeight[0], 0.75);
    UTEST_EQU(vert[2].boneId[1], 0); UTEST_EQUF(vert[2].boneWeight[1], 0.25);
    UTEST_EQU(vert[2].boneId[2], 0); UTEST_EQUF(vert[2].boneWeight[2], 0.0);
    UTEST_EQU(vert[2].boneId[3], 0); UTEST_EQUF(vert[2].boneWeight[3], 0.0);

    UTEST_EQU(vert[3].boneId[0], 1); UTEST_EQUF(vert[3].boneWeight[0], 1.0);
    UTEST_EQU(vert[3].boneId[1], 0); UTEST_EQUF(vert[3].boneWeight[1], 0.0);
    UTEST_EQU(vert[3].boneId[2], 0); UTEST_EQUF(vert[3].boneWeight[2], 0.0);
    UTEST_EQU(vert[3].boneId[3], 0); UTEST_EQUF(vert[3].boneWeight[3], 0.0);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Model optimisation");

    Assets::Model optModel;
    {
        ModelMesh mesh;
        mesh.setLODCount(1);
        optModel.addMesh(L"Mesh00", mesh);
    }
    {
        VerticeSet & vset = *optModel.getMesh(0).getLOD(Assets::LOD_HIGH);
        List<Vertex> & vert = vset.getVertices();
        List<uint16> & ind = vset.getIndices();

        vert.resize(18);
        for(int32 ii=0; ii < 18; ii++)
            vert[ii].position._x = float(ii);

        ind.push_back( 0); ind.push_back( 1); ind.push_back( 2);
        ind.push_back( 3); ind.push_back( 4); ind.push_back( 5);
        ind.push_back( 6); ind.push_back( 7); ind.push_back( 8);
        ind.push_back( 9); ind.push_back(10); ind.push_back(11);
        ind.push_back(12); ind.push_back(13); ind.push_back(14);
        ind.push_back(15); ind.push_back(16); ind.push_back(17);

        ind.push_back( 1); ind.push_back( 2); ind.push_back( 3);
        ind.push_back( 4); ind.push_back( 5); ind.push_back( 6);
        ind.push_back( 7); ind.push_back( 8); ind.push_back( 9);
        ind.push_back(10); ind.push_back(11); ind.push_back(12);
        ind.push_back(13); ind.push_back(14); ind.push_back(15);

        ind.push_back( 2); ind.push_back( 3); ind.push_back( 4);
        ind.push_back( 5); ind.push_back( 6); ind.push_back( 7);
        ind.push_back( 8); ind.push_back( 9); ind.push_back(10);
        ind.push_back(11); ind.push_back(12); ind.push_back(13);
        ind.push_back(14); ind.push_back(15); ind.push_back(16);
    }

    Workflow::optimizeVerticesForCache(optModel);

    /*{
        VerticeSet & vset = optModel.getMesh(0).getLOD(0);

        List<Vertex> & vert = vset.getVertices();
        foreach(iVert, vert)
            std::cout << iVert->position._x << std::endl;

        List<uint16> & ind = vset.getIndices();
        for(int32 ii=0; ii < ind.size(); ii +=3)
        {
            std::cout << ind[ii+0] << " " << ind[ii+1] << " " << ind[ii+2]
                      << " -> " << vert[ind[ii+0]].position._x << ", "
                                << vert[ind[ii+1]].position._x << ", "
                                << vert[ind[ii+2]].position._x << std::endl;
        }
    }*/
    UTEST_END_TEST;
}

