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

void testNodeAnim(Assets::Animation & anim1)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Node animation");

    anim1 = Assets::Animation(5);
    UTEST_EQU(anim1.getFrameCount(), 0);
    UTEST_EQU(anim1.getNodeCount(), 5);

    Core::Matrix4f mat000(Core::TRANSLATION, 1.0, 0.0, 0.0);
    Core::Matrix4f mat050(Core::TRANSLATION, 2.0, 0.0, 0.0);
    Core::Matrix4f mat070(Core::TRANSLATION, 3.0, 0.0, 0.0);
    Core::Matrix4f mat100(Core::TRANSLATION, 6.0, 0.0, 0.0);

    anim1.addKeyFrame(1, KeyFrame( 50, mat050));
    anim1.addKeyFrame(1, KeyFrame(  0, mat000));
    anim1.addKeyFrame(1, KeyFrame(100, mat100));
    anim1.addKeyFrame(1, KeyFrame( 70, mat070));

    anim1.addKeyFrame(2, KeyFrame( 50, mat050));
    anim1.addKeyFrame(2, KeyFrame( 70, mat070));

    anim1.addKeyFrame(3, KeyFrame( 50, mat050));

    UTEST_EQU(anim1.getFrameCount(), 101);
    {
        Core::Matrix4f mat025(Core::TRANSLATION, 1.5, 0.0, 0.0);
        Core::Matrix4f mat060(Core::TRANSLATION, 2.5, 0.0, 0.0);
        Core::Matrix4f mat080(Core::TRANSLATION, 4.0, 0.0, 0.0);
        Core::Matrix4f mat090(Core::TRANSLATION, 5.0, 0.0, 0.0);

        Core::Matrix4f mat0_00, mat0_25, mat0_50, mat0_60, mat0_70;
        Core::Matrix4f mat0_80, mat0_90, mat1_00, mat1_10;

        anim1.getNodeMatrix(mat0_00, 1, 0.00f); anim1.getNodeMatrix(mat0_25, 1, 0.25f);
        anim1.getNodeMatrix(mat0_50, 1, 0.50f); anim1.getNodeMatrix(mat0_60, 1, 0.60f);
        anim1.getNodeMatrix(mat0_70, 1, 0.70f); anim1.getNodeMatrix(mat0_80, 1, 0.80f);
        anim1.getNodeMatrix(mat0_90, 1, 0.90f); anim1.getNodeMatrix(mat1_00, 1, 1.00f);
        anim1.getNodeMatrix(mat1_10, 1, 1.10f);

        UTEST_EQU(mat0_00.isEquivalent(mat000), true);
        UTEST_EQU(mat0_25.isEquivalent(mat025), true);
        UTEST_EQU(mat0_50.isEquivalent(mat050), true);
        UTEST_EQU(mat0_60.isEquivalent(mat060), true);
        UTEST_EQU(mat0_70.isEquivalent(mat070), true);
        UTEST_EQU(mat0_80.isEquivalent(mat080), true);
        UTEST_EQU(mat0_90.isEquivalent(mat090), true);
        UTEST_EQU(mat1_00.isEquivalent(mat100), true);
        UTEST_EQU(mat1_10.isEquivalent(mat100), true);
    }
    {
        Core::Matrix4f mat060(Core::TRANSLATION, 2.5, 0.0, 0.0);
        Core::Matrix4f mat0_00, mat0_50, mat0_60, mat0_90;

        anim1.getNodeMatrix(mat0_00, 2, 0.00f); anim1.getNodeMatrix(mat0_50, 2, 0.50f);
        anim1.getNodeMatrix(mat0_60, 2, 0.60f); anim1.getNodeMatrix(mat0_90, 2, 0.90f);

        UTEST_EQU(mat0_00.isEquivalent(mat050), true);
        UTEST_EQU(mat0_50.isEquivalent(mat050), true);
        UTEST_EQU(mat0_60.isEquivalent(mat060), true);
        UTEST_EQU(mat0_90.isEquivalent(mat070), true);
    }
    {
        Core::Matrix4f mat0_00, mat0_50, mat0_90;

        anim1.getNodeMatrix(mat0_00, 3, 0.00f); anim1.getNodeMatrix(mat0_50, 3, 0.50f);
        anim1.getNodeMatrix(mat0_90, 3, 0.90f);

        UTEST_EQU(mat0_00.isEquivalent(mat050), true);
        UTEST_EQU(mat0_50.isEquivalent(mat050), true);
        UTEST_EQU(mat0_90.isEquivalent(mat050), true);
    }
    {
        Core::Matrix4f matIdentity(Core::IDENTITY);
        Core::Matrix4f mat0_00, mat0_50;
        anim1.getNodeMatrix(mat0_00, 4, 0.00); anim1.getNodeMatrix(mat0_50, 4, 0.50);

        UTEST_EQU(mat0_00.isEquivalent(matIdentity), true);
        UTEST_EQU(mat0_50.isEquivalent(matIdentity), true);
    }

    Assets::Animation anim2(anim1);
    Assets::Animation anim3(4);//, 102);

    UTEST_EQU(anim1, anim2);
    UTEST_NOT_EQU(anim1, anim3);
    {
        Core::BufferStream buffer;
        anim1.write(buffer);
        anim3.read(buffer);
        UTEST_EQU(anim1, anim3);
    }

    UTEST_END_TEST;
}

void testAnims(Assets::Model & model, const Assets::Animation & anim1)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Model animation");

    Assets::Animation anim2(anim1);

    UTEST_EQU(model.getNodeCount(), 5);
    UTEST_EQU(model.getAnimationCount(), 0);
    model.addAnimation(L"Anim01", anim1);

    UTEST_EQU(model.getAnimationCount(), 1);
    UTEST_EQU(model.getAnimationName(0), L"Anim01");

    Assets::ModelPose pose;

    model.getPose(pose, 0, 0.25f);

    UTEST_EQU(pose.getNodeMatrix(0).isEquivalent(Matrix4f(Core::TRANSLATION, 0.0, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(1).isEquivalent(Matrix4f(Core::TRANSLATION, 1.5, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(2).isEquivalent(Matrix4f(Core::TRANSLATION, 2.0, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(3).isEquivalent(Matrix4f(Core::TRANSLATION, 4.0, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(4).isEquivalent(Matrix4f(Core::TRANSLATION, 2.0, 0.0, 0.0)), true);

    model.getPose(pose, 0, 0.60f);

    UTEST_EQU(pose.getNodeMatrix(0).isEquivalent(Matrix4f(Core::TRANSLATION, 0.0, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(1).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(2).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(3).isEquivalent(Matrix4f(Core::TRANSLATION, 4.5, 0.0, 0.0)), true);
    UTEST_EQU(pose.getNodeMatrix(4).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 0.0, 0.0)), true);

    UTEST_END_TEST;
}

void testSkeleton(Assets::Model & model)
{
    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Skeleton animation");

    Assets::Skeleton & skeletor = model.getSkeleton();

    UTEST_EQU(skeletor.isEmpty(), true);

    skeletor.addBone(1, Matrix4f(Core::TRANSLATION, 0.0, 1.0, 0.0));
    skeletor.addBone(2, Matrix4f(Core::TRANSLATION, 0.0, 2.0, 0.0));
    skeletor.addBone(3, Matrix4f(Core::TRANSLATION, 0.0, 0.0, 3.0));

    UTEST_EQU(skeletor.isEmpty(), false);

    Assets::ModelPose pose;
    Assets::SkeletonPose skelPose;

    model.getPose(pose, 0, 0.60f);
    skeletor.getSkeletonPose(skelPose, pose);

    UTEST_EQU(skelPose.getBoneMatrix(0).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 1.0, 0.0)), true);
    UTEST_EQU(skelPose.getBoneMatrix(1).isEquivalent(Matrix4f(Core::TRANSLATION, 2.5, 2.0, 0.0)), true);
    UTEST_EQU(skelPose.getBoneMatrix(2).isEquivalent(Matrix4f(Core::TRANSLATION, 4.5, 0.0, 3.0)), true);

    UTEST_END_TEST;
}

void testModelAnim(Assets::Model & model, Assets::Animation & anim1)
{
    testNodeAnim(anim1);
    testAnims(model, anim1);
    testSkeleton(model);
}
