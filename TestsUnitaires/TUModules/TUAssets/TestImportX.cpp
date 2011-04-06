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
#include <Core/IO/FileStream.h>
#include <Assets/SkeletonPose.h>
#include <Workflow/ModelImporterX.h>

using namespace Core;
using namespace Workflow;

void testImportX()
{
    UTEST_PACKAGE("XFile import");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Unique root mesh");

    FileInputStream input(L"Data/TUData/Modeles/RootMesh.x");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterX importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_EQU(pModel->getMeshCount(), 1);
    UTEST_EQU(pModel->getNodeCount(), 2);

    Core::Matrix4f matIdentity(Core::IDENTITY);

    const Assets::ModelNode & root = pModel->getRootNode();
    UTEST_EQU(root.getSubNodeCount(), 1);

    const Assets::ModelNode & child = root.getSubNode(0);
    UTEST_EQU(child.getName(), L"Quad");
    UTEST_EQU(child.getSubNodeCount(), 0);
    UTEST_EQU(child.getMeshId(), 0);
    UTEST_EQU(child.getMatrix().isEquivalent(matIdentity), true);

    const Assets::ModelMesh & mesh = pModel->getMesh(0);

    UTEST_EQU(mesh.getName(), L"Quad");
    UTEST_EQU(mesh.isSkin(), false);
    UTEST_EQU(mesh.getLODCount(), 1);

    const Assets::VerticeSet & vset = *mesh.getLOD(Assets::LOD_HIGH);

    const Core::List<Assets::Vertex> & vertices = vset.getVertices();
    const Core::List<uint16> & indices = vset.getIndices();

    UTEST_EQU(vertices.size(), 4);
    UTEST_EQU(indices.size(), 12);

    UTEST_EQU(indices[0], 0); UTEST_EQU(indices[1], 1); UTEST_EQU(indices[2], 2);
    UTEST_EQU(indices[3], 0); UTEST_EQU(indices[4], 2); UTEST_EQU(indices[5], 3);
    UTEST_EQU(indices[6], 2); UTEST_EQU(indices[7], 1); UTEST_EQU(indices[8], 0);
    UTEST_EQU(indices[9], 3); UTEST_EQU(indices[10], 2); UTEST_EQU(indices[11], 0);

    UTEST_EQU(vertices[0].position, Core::Vector3f(-1.0, -1.0,  0.0));
    UTEST_EQU(vertices[1].position, Core::Vector3f( 1.0, -1.0,  0.0));
    UTEST_EQU(vertices[2].position, Core::Vector3f( 1.0,  1.0,  0.0));
    UTEST_EQU(vertices[3].position, Core::Vector3f(-1.0,  1.0,  0.0));

    UTEST_EQU(vertices[0].normale, Core::Vector3f( 1.0,  0.0,  0.0));
    UTEST_EQU(vertices[1].normale, Core::Vector3f( 0.0,  1.0,  0.0));
    UTEST_EQU(vertices[2].normale, Core::Vector3f( 0.0,  0.0, -1.0));
    UTEST_EQU(vertices[3].normale, Core::Vector3f( 0.0, -1.0,  0.0));

    UTEST_EQU(vertices[0].texCoords, Core::Vector2f( 0.0,  0.0));
    UTEST_EQU(vertices[1].texCoords, Core::Vector2f( 1.0,  0.0));
    UTEST_EQU(vertices[2].texCoords, Core::Vector2f( 1.0,  1.0));
    UTEST_EQU(vertices[3].texCoords, Core::Vector2f( 0.0,  1.0));

    UTEST_END_TEST;


    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Mesh with duplicate normales");

    FileInputStream input(L"Data/TUData/Modeles/DuplicateNormales.x");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterX importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_EQU(pModel->getMeshCount(), 1);
    UTEST_EQU(pModel->getNodeCount(), 2);

    Core::Matrix4f matIdentity(Core::IDENTITY);

    const Assets::ModelNode & root = pModel->getRootNode();
    UTEST_EQU(root.getSubNodeCount(), 1);

    const Assets::ModelNode & child = root.getSubNode(0);
    UTEST_EQU(child.getName(), L"Quad");
    UTEST_EQU(child.getSubNodeCount(), 0);
    UTEST_EQU(child.getMeshId(), 0);
    UTEST_EQU(child.getMatrix().isEquivalent(matIdentity), true);

    const Assets::ModelMesh & mesh = pModel->getMesh(0);

    UTEST_EQU(mesh.getName(), L"Quad");
    UTEST_EQU(mesh.isSkin(), false);
    UTEST_EQU(mesh.getLODCount(), 1);

    const Assets::VerticeSet & vset = *mesh.getLOD(Assets::LOD_HIGH);

    const Core::List<Assets::Vertex> & vertices = vset.getVertices();
    const Core::List<uint16> & indices = vset.getIndices();

    UTEST_EQU(vertices.size(), 6);
    UTEST_EQU(indices.size(), 12);

    UTEST_EQU(vertices[0].position, Core::Vector3f(-1.0, -1.0,  0.0));
    UTEST_EQU(vertices[1].position, Core::Vector3f( 1.0, -1.0,  0.0));
    UTEST_EQU(vertices[2].position, Core::Vector3f( 1.0,  1.0,  0.0));
    UTEST_EQU(vertices[3].position, Core::Vector3f(-1.0,  1.0,  0.0));
    UTEST_EQU(vertices[4].position, Core::Vector3f( 1.0, -1.0,  0.0));
    UTEST_EQU(vertices[5].position, Core::Vector3f(-1.0, -1.0,  0.0));

    UTEST_EQU(vertices[0].normale, Core::Vector3f( 1.0,  0.0,  0.0));
    UTEST_EQU(vertices[1].normale, Core::Vector3f( 0.0,  1.0,  0.0));
    UTEST_EQU(vertices[2].normale, Core::Vector3f( 0.0,  0.0, -1.0));
    UTEST_EQU(vertices[3].normale, Core::Vector3f( 0.0, -1.0,  0.0));
    UTEST_EQU(vertices[4].normale, Core::Vector3f( 0.0, -1.0,  0.0));
    UTEST_EQU(vertices[5].normale, Core::Vector3f(-1.0,  0.0,  0.0));

    UTEST_EQU(vertices[0].texCoords, Core::Vector2f( 0.0,  0.0));
    UTEST_EQU(vertices[1].texCoords, Core::Vector2f( 1.0,  0.0));
    UTEST_EQU(vertices[2].texCoords, Core::Vector2f( 1.0,  1.0));
    UTEST_EQU(vertices[3].texCoords, Core::Vector2f( 0.0,  1.0));
    UTEST_EQU(vertices[4].texCoords, Core::Vector2f( 1.0,  0.0));
    UTEST_EQU(vertices[5].texCoords, Core::Vector2f( 0.0,  0.0));

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Hierarchie");

    FileInputStream input(L"Data/TUData/Modeles/Hierarchie.x");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterX importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_EQU(pModel->getMeshCount(), 4);
    UTEST_EQU(pModel->getNodeCount(), 6);

    Core::Matrix4f matIdentity(Core::IDENTITY);
    Core::Matrix4f matTrans5X(Core::TRANSLATION, 5.0, 0.0, 0.0);
    Core::Matrix4f matTrans10Y(Core::TRANSLATION, 0.0, 10.0, 0.0);
    Core::Matrix4f matTrans10Z(Core::TRANSLATION, 0.0, 0.0, 10.0);

    const Assets::ModelNode & root = pModel->getRootNode();
    UTEST_EQU(root.getSubNodeCount(), 2);

    const Assets::ModelNode & nodeX = root.getSubNode(0);
    UTEST_EQU(nodeX.getName(), L"XFrame");
    UTEST_EQU(nodeX.getSubNodeCount(), 2);
    UTEST_EQU(nodeX.getMeshId(), Assets::UNDEFINED_MESH_ID);
    UTEST_EQU(nodeX.getMatrix().isEquivalent(matIdentity), true);

    const Assets::ModelNode & subNodeX1 = nodeX.getSubNode(0);
    UTEST_EQU(subNodeX1.getName(), L"XSubFrame1");
    UTEST_EQU(subNodeX1.getSubNodeCount(), 0);
    UTEST_EQU(subNodeX1.getMeshId(), 0);
    UTEST_EQU(subNodeX1.getMatrix().isEquivalent(matIdentity), true);

    const Assets::ModelNode & subNodeX2 = nodeX.getSubNode(1);
    UTEST_EQU(subNodeX2.getName(), L"XSubFrame2");
    UTEST_EQU(subNodeX2.getSubNodeCount(), 0);
    UTEST_EQU(subNodeX2.getMeshId(), 1);
    UTEST_EQU(subNodeX2.getMatrix().isEquivalent(matTrans5X), true);

    const Assets::ModelNode & nodeY = root.getSubNode(1);
    UTEST_EQU(nodeY.getName(), L"YFrame");
    UTEST_EQU(nodeY.getSubNodeCount(), 1);
    UTEST_EQU(nodeY.getMeshId(), 2);
    UTEST_EQU(nodeY.getMatrix().isEquivalent(matTrans10Y), true);

    const Assets::ModelNode & subNodeY = nodeY.getSubNode(0);
    UTEST_EQU(subNodeY.getName(), L"YSubFrame");
    UTEST_EQU(subNodeY.getSubNodeCount(), 0);
    UTEST_EQU(subNodeY.getMeshId(), 3);
    UTEST_EQU(subNodeY.getMatrix().isEquivalent(matTrans10Z), true);

    UTEST_EQU(pModel->getMesh(0).getName(), L"XSubFrame1");
    UTEST_EQU(pModel->getMesh(1).getName(), L"XSubFrame2");
    UTEST_EQU(pModel->getMesh(2).getName(), L"NamedMesh");
    UTEST_EQU(pModel->getMesh(3).getName(), L"OtherNamedMesh");

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Animation");

    FileInputStream input(L"Data/TUData/Modeles/Animation.x");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterX importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_EQU(pModel->getMeshCount(), 4);
    UTEST_EQU(pModel->getNodeCount(), 6);
    UTEST_EQU(pModel->getAnimationCount(), 2);
    UTEST_EQU(pModel->getAnimationName(0), L"AnimSet00");
    UTEST_EQU(pModel->getAnimationName(1), L"AnimSet01");

    UTEST_EQU(pModel->getNode(1).getName(), L"RootFrame");
    UTEST_EQU(pModel->getNode(2).getName(), L"XFrame");
    UTEST_EQU(pModel->getNode(3).getName(), L"YFrame");
    UTEST_EQU(pModel->getNode(4).getName(), L"ZFrame");
    UTEST_EQU(pModel->getNode(5).getName(), L"WFrame");

    Assets::ModelPose pose;

    // ----> Animation par matrice

    pModel->getPose(pose, 1, 0.75);

    Core::Matrix4f matXFrame(Core::TRANSLATION,  1.5, 0.0, 0.0);
    Core::Matrix4f matYFrame(Core::TRANSLATION, -2.0, 0.0, 0.0);
    Core::Matrix4f matZFrame(Core::TRANSLATION, -2.0, 0.0, 0.0);
    Core::Matrix4f matWFrame(Core::TRANSLATION, -2.0, 0.0, 0.0);

    UTEST_EQU(matXFrame.isEquivalent(pose.getNodeMatrix(2)), true);
    UTEST_EQU(matYFrame.isEquivalent(pose.getNodeMatrix(3)), true);
    UTEST_EQU(matZFrame.isEquivalent(pose.getNodeMatrix(4)), true);
    UTEST_EQU(matWFrame.isEquivalent(pose.getNodeMatrix(5)), true);

    // ----> Animation par trans/rotation/scale

    Quaternionf midq(Core::Quaternionf(0.0, 0.0, 0.0, 1.0),
                     Core::Quaternionf(1.0, 0.0, 0.0, 0.0),
                     0.5);

    matXFrame.setTransform(Core::Vector3f(0.5, 1.0, 1.5),
                           midq,
                           Core::Vector3f(1.0, 1.5, 1.0));

    matYFrame.setTransform(Core::Vector3f(-2.0, 0.0, 0.0),
                           Core::Quaternionf(Core::IDENTITY),
                           Core::Vector3f(1.0, 1.0, 1.0));

    matZFrame.setTransform(Core::Vector3f(1.5, 0.0, 0.0),
                           midq,
                           Core::Vector3f(1.0, 1.5, 1.0));

    matWFrame.setTransform(Core::Vector3f(1.0, 0.0, 0.0),
                           midq,
                           Core::Vector3f(2.0, 1.0, 1.0));

    pModel->getPose(pose, 0, 0.5);

    UTEST_EQU(matXFrame.isEquivalent(pose.getNodeMatrix(2)), true);
    UTEST_EQU(matYFrame.isEquivalent(pose.getNodeMatrix(3)), true);
    UTEST_EQU(matZFrame.isEquivalent(pose.getNodeMatrix(4)), true);
    UTEST_EQU(matWFrame.isEquivalent(pose.getNodeMatrix(5)), true);

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Skinned animation");

    FileInputStream input(L"Data/TUData/Modeles/SkinnedAnimation.x");

    UTEST_EQU(input.isOpened(), true);

    ModelImporterX importer;
    Ptr<Assets::Model> pModel = importer.importModel(input);

    UTEST_EQU(pModel->getMeshCount(), 1);
    UTEST_EQU(pModel->getNodeCount(), 4);
    UTEST_EQU(pModel->getAnimationCount(), 1);
    UTEST_EQU(pModel->getAnimationName(0), L"AnimSet00");

    UTEST_EQU(pModel->getNode(1).getName(), L"RootFrame");
    UTEST_EQU(pModel->getNode(2).getName(), L"XFrame");
    UTEST_EQU(pModel->getNode(3).getName(), L"YFrame");

    Assets::ModelPose pose;
    Assets::SkeletonPose skelPose;

    pModel->getPose(pose, 0, 0.5);
    pModel->getSkeleton().getSkeletonPose(skelPose, pose);

    Core::Matrix4f matXFrame(Core::TRANSLATION,  1.0, 0.0, -1.0);
    Core::Matrix4f matYFrame(Core::TRANSLATION,  0.0, 1.0,  1.0);

    UTEST_EQU(skelPose.getBoneCount(), 2);
    UTEST_EQU(matXFrame.isEquivalent(skelPose.getBoneMatrix(0)), true);
    UTEST_EQU(matYFrame.isEquivalent(skelPose.getBoneMatrix(1)), true);

    const Core::List<Assets::Vertex> & vert = pModel->getMesh(0).getLODByID(0).getVertices();

    UTEST_EQU(vert[0].boneId[0], 0); UTEST_EQUF(vert[0].boneWeight[0], 1.0);
    UTEST_EQUF(vert[0].boneWeight[1], 0.0);
    UTEST_EQUF(vert[0].boneWeight[2], 0.0);
    UTEST_EQUF(vert[0].boneWeight[3], 0.0);

    UTEST_EQU(vert[1].boneId[0], 0); UTEST_EQUF(vert[1].boneWeight[0], 0.3);
    UTEST_EQU(vert[1].boneId[1], 1); UTEST_EQUF(vert[1].boneWeight[1], 0.7);
    UTEST_EQUF(vert[1].boneWeight[2], 0.0);
    UTEST_EQUF(vert[1].boneWeight[3], 0.0);

    UTEST_EQU(vert[2].boneId[0], 1); UTEST_EQUF(vert[2].boneWeight[0], 1.0);
    UTEST_EQUF(vert[2].boneWeight[1], 0.0);
    UTEST_EQUF(vert[2].boneWeight[2], 0.0);
    UTEST_EQUF(vert[2].boneWeight[3], 0.0);

    UTEST_EQU(vert[3].boneId[0], 0); UTEST_EQUF(vert[3].boneWeight[0], 0.7);
    UTEST_EQU(vert[3].boneId[1], 1); UTEST_EQUF(vert[3].boneWeight[1], 0.3);
    UTEST_EQUF(vert[3].boneWeight[2], 0.0);
    UTEST_EQUF(vert[3].boneWeight[3], 0.0);

    UTEST_END_TEST;
}
