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


void testVSetPackIO()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"VerticeSet pack/unpack and IO");

    VerticeSet set1u;

    List<Vertex> & vert = set1u.getVertices();
    vert.resize(3);
    vert[0].position    = Vector3f( 1.0f,  2.0f,  3.0f);
    vert[0].normale     = Vector3f(-1.0f,  0.0f,  1.0f);
    vert[0].tangent     = Vector3f(-1.5f,  1.5f,  0.0f);
    vert[0].bitangent   = Vector3f( 0.0f,  0.5f, -1.0f);
    vert[0].texCoords   = Vector2f( 2.0f,  0.5f);

    {
        List<uint16> & ind = set1u.getIndices();
        for(int32 ii=0; ii < 10; ii++)
            ind.push_back(ii % 3);
    }

    VerticeSet set1p(set1u);

    UTEST_EQU(set1p, set1u);

    {
        List<uint16> & ind = set1u.getIndices();
        for(int32 ii=0; ii < 10; ii++)
            UTEST_EQU(ind[ii], ii % 3);
    }

    List<Vertex> & vertu = set1p.getVertices();

    const float EPSILON = 1.0 / 256.0;

    UTEST_EQU(vertu.size(), 3);
    UTEST_EQU(vertu[0].position, Vector3f( 1.0,  2.0,  3.0));
    UTEST_EQU(vertu[0].normale.isEquivalent(vert[0].normale, EPSILON), true);
    UTEST_EQU(vertu[0].tangent.isEquivalent(Vector3f(-1.5,  1.5,  0.0), EPSILON), true);
    UTEST_EQU(vertu[0].bitangent.isEquivalent(vert[0].bitangent, EPSILON), true);
    UTEST_EQU(vertu[0].texCoords.isEquivalent(vert[0].texCoords, EPSILON), true);

    UTEST_END_TEST;
}

void testVSetBlendingGroup()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"VerticeSet blending groups");

    VerticeSet set1u;

    List<Vertex> & vert = set1u.getVertices();
    List<uint16> & ind = set1u.getIndices();

    vert.resize(9);

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

    Workflow::createBlendingGroups(set1u, 6);

    Core::List<Assets::BlendingGroup> & groups = set1u.getBlendingGroups();

    UTEST_EQU(groups.size(), 3);
    if(groups.size() >= 3)
    {
        Core::List<int32> palette1;

        palette1.push_back(0); palette1.push_back(1); palette1.push_back(2);
        palette1.push_back(3); palette1.push_back(4); palette1.push_back(5);

        Core::List<int32> palette2;

        palette2.push_back(3); palette2.push_back(4); palette2.push_back(5);
        palette2.push_back(6); palette2.push_back(7); palette2.push_back(8);

        Core::List<int32> palette3;

        palette3.push_back(6); palette3.push_back(7); palette3.push_back(8);
        palette3.push_back(9);

        UTEST_EQU(groups[0].getStartIndice(), 0);
        UTEST_EQU(groups[0].getIndiceCount(), 9);
        UTEST_EQU(groups[0].getMatrixPalette(), palette1);

        UTEST_EQU(groups[1].getStartIndice(), 9);
        UTEST_EQU(groups[1].getIndiceCount(), 9);
        UTEST_EQU(groups[1].getMatrixPalette(), palette2);

        UTEST_EQU(groups[2].getStartIndice(), 18);
        UTEST_EQU(groups[2].getIndiceCount(), 3);
        UTEST_EQU(groups[2].getMatrixPalette(), palette3);
    }

    Core::List<uint16> expInd;
    expInd.push_back(0);  expInd.push_back(1);  expInd.push_back(2);
    expInd.push_back(1);  expInd.push_back(2);  expInd.push_back(3);
    expInd.push_back(2);  expInd.push_back(3);  expInd.push_back(4);
    expInd.push_back(9);  expInd.push_back(10); expInd.push_back(5);
    expInd.push_back(10); expInd.push_back(5);  expInd.push_back(6);
    expInd.push_back(5);  expInd.push_back(6);  expInd.push_back(7);
    expInd.push_back(11); expInd.push_back(12); expInd.push_back(8);

    UTEST_EQU(ind, expInd);

    UTEST_EQU(vert.size(), 13);
    if(vert.size() == 13)
    {
        UTEST_EQU(vert[0].boneId[0],  0); UTEST_EQU(vert[0].boneId[1],  1);
        UTEST_EQU(vert[1].boneId[0],  1); UTEST_EQU(vert[1].boneId[1],  2);
        UTEST_EQU(vert[2].boneId[0],  2); UTEST_EQU(vert[2].boneId[1],  3);
        UTEST_EQU(vert[3].boneId[0],  3); UTEST_EQU(vert[3].boneId[1],  4);
        UTEST_EQU(vert[4].boneId[0],  4); UTEST_EQU(vert[4].boneId[1],  5);
        UTEST_EQU(vert[5].boneId[0],  2); UTEST_EQU(vert[5].boneId[1],  3);
        UTEST_EQU(vert[6].boneId[0],  3); UTEST_EQU(vert[6].boneId[1],  4);
        UTEST_EQU(vert[7].boneId[0],  4); UTEST_EQU(vert[7].boneId[1],  5);
        UTEST_EQU(vert[8].boneId[0],  2); UTEST_EQU(vert[8].boneId[1],  3);
        UTEST_EQU(vert[9].boneId[0],  0); UTEST_EQU(vert[9].boneId[1],  1);
        UTEST_EQU(vert[10].boneId[0],  1); UTEST_EQU(vert[10].boneId[1],  2);
        UTEST_EQU(vert[11].boneId[0],  0); UTEST_EQU(vert[11].boneId[1],  1);
        UTEST_EQU(vert[12].boneId[0],  1); UTEST_EQU(vert[12].boneId[1],  2);
    }

    UTEST_END_TEST;
}

void testMaterial()
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Materials");

    Assets::Material mat1;
    Assets::Material mat2;

    UTEST_EQU(mat1, mat2);

    mat1.ambient = Core::Vector4f(1.0, 2.0, 3.0, 1.0);
    UTEST_NOT_EQU(mat1, mat2); mat2 = mat1; UTEST_EQU(mat1, mat2);
    mat1.diffuse = Core::Vector4f(4.0, 0.0, 0.0, 1.0);
    mat1.specular = Core::Vector4f(0.0, 5.0, 0.0, 1.0);
    mat1.emissive = Core::Vector4f(0.0, 0.0, 6.0, 1.0);
    mat1.ambient = Core::Vector4f(7.0, 0.0, 0.0, 1.0);
    mat1.reflexions = Core::Vector4f(0.0, 8.0, 0.0, 0.0);
    mat1.glowColor = Core::Vector4f(1.0, 0.0, 0.0, 0.0);
    mat1.glowExtent = Core::Vector3f(0.0, 0.0, 0.0);
    mat1.shininess = 4.0;

    Assets::Material mat3(mat1);
    UTEST_EQU(mat1, mat3);

    Core::BufferStream buffer;
    Assets::Material mat4;
    buffer.write(mat1);
    buffer.read(mat4);
    UTEST_EQU(mat1, mat4);

    UTEST_END_TEST;
}

void testVSet()
{
    testVSetPackIO();
    testVSetBlendingGroup();
    testMaterial();
}
