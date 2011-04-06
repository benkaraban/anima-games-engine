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
#include <Core/Exception.h>

#include <Core/Math/ModuleMath.h>
#include <Core/Math/GeometryComputations.h>

using namespace Core;



void testRayf()
{
    const Vector3f rayOrigin (2.f, 3.5f, 4.f);
    const Vector3f rayDirection (10.f, 12.5f, 13.f);
    
    Rayf ray_1 (Core::Ray_OriginDirection, rayOrigin, rayDirection);
    UTEST_EQU(ray_1.origin, rayOrigin);
    UTEST_EQU(ray_1.direction, rayDirection.getNormalizedSafe());
    
    Rayf ray_2 = ray_1;
    UTEST_EQU(ray_1==ray_2, true);
    UTEST_EQU(ray_1!=ray_2, false);
    
    Rayf ray_3 (Core::Ray_OriginDirection_SkipNormalize, rayOrigin, ray_1.direction);
    UTEST_EQU(ray_1, ray_3);
    
    Rayf ray_4 (Core::Ray_OriginTarget, rayOrigin, rayOrigin + rayDirection*2.6f);
    UTEST_EQU(ray_1, ray_4);
    
    Rayf ray_5 (Core::Ray_OriginDirection, ray_1.origin+Vector3f(.5f, .5f, .5f), ray_1.direction);
    UTEST_EQU(ray_1.isEquivalent(ray_5, .6f), true);
    UTEST_EQU(ray_1.isEquivalent(ray_5, .01f), false);
}


void testPlanef()
{
    //on definit le plan
    const Vector3f planeNormal (2.f, 3.5f, 4.f);
    const float planeDistance = 15.5f;
    
    Planef p_1 (Core::Plane_NormalDistance, planeNormal, planeDistance);
    UTEST_EQU(p_1.normal, planeNormal.getNormalizedSafe());
    UTEST_EQUE(p_1.distance, planeDistance/planeNormal.getLengthSafe(), f_COMMON_EPSILON);

    //on prend trois points situes sur le plan
    Vector3f pt_1 (45.3f, 456.789f, 0.f);
    pt_1._z = (planeDistance - pt_1._x*planeNormal._x - pt_1._y*planeNormal._y) / planeNormal._z;
    Vector3f pt_2 (4575.3f, 45756.789f, 0.f);
    pt_2._z = (planeDistance - pt_2._x*planeNormal._x - pt_2._y*planeNormal._y) / planeNormal._z;
    Vector3f pt_3 (4542.3f, 4427556.78759f, 0.f);
    pt_3._z = (planeDistance - pt_3._x*planeNormal._x - pt_3._y*planeNormal._y) / planeNormal._z;
    
    Planef p_2 = p_1;
    UTEST_EQU(p_1==p_2, true);
    UTEST_EQU(p_1!=p_2, false);
    
    Planef p_3 (Core::Plane_NormalDistance_SkipNormalize, p_1.normal, planeDistance/planeNormal.getLengthSafe() );
    UTEST_EQU(p_1, p_3);
    
    Planef p_4 (Core::Plane_NormalPoint, planeNormal, pt_1);
    UTEST_EQU(p_1.isEquivalent(p_4), true);

    Planef p_5(pt_1, pt_2, pt_3);
    UTEST_EQU(p_1.isEquivalent(p_5), true);
    
    Planef p_6 (Core::Plane_NormalDistance, p_1.normal, p_1.distance + .5f);
    UTEST_EQU(p_1.isEquivalent(p_6, f_COMMON_EPSILON, .6f), true);
    UTEST_EQU(p_1.isEquivalent(p_6, f_COMMON_EPSILON, .01f), false);
    
    Vector3f pt_4 (0.f, 0.f, -2*p_1.distance/p_1.normal._z);
    UTEST_EQUE(p_1.distanceToPoint(pt_1), 0.f, f_COMMON_EPSILON);
    //UTEST_EQUE(p_1.distanceToPoint(pt_4), -p_1.distance, f_COMMON_EPSILON);
}


void testAABoxf()
{
    AABoxf box1;
    AABoxf box2;

    UTEST_EQU(box1.isEmpty(), true);
    UTEST_EQU(box1.inter(box2), false);

    box1.addPoint(Vector3f(1.0, 0.0, 0.0));
    UTEST_EQU(box1.isEmpty(), false);
    UTEST_EQU(box1.min, Vector3f(1.0, 0.0, 0.0));
    UTEST_EQU(box1.max, Vector3f(1.0, 0.0, 0.0));

    box1.addPoint(Vector3f(-1.0, 2.0, -3.0));
    UTEST_EQU(box1.min, Vector3f(-1.0, 0.0, -3.0));
    UTEST_EQU(box1.max, Vector3f(1.0, 2.0, 0.0));

    box1.addPoint(Vector3f(0.0, -2.0, 3.0));
    UTEST_EQU(box1.min, Vector3f(-1.0, -2.0, -3.0));
    UTEST_EQU(box1.max, Vector3f(1.0, 2.0, 3.0));

    UTEST_EQU(box1.contains(Vector3f(0.0, 0.0, 0.0)), true);

    UTEST_EQU(box1.contains(Vector3f(-0.9f, 0.0f, 0.0f)), true);
    UTEST_EQU(box1.contains(Vector3f(-1.1f, 0.0f, 0.0f)), false);
    UTEST_EQU(box1.contains(Vector3f( 0.9f, 0.0f, 0.0f)), true);
    UTEST_EQU(box1.contains(Vector3f( 1.1f, 0.0f, 0.0f)), false);

    UTEST_EQU(box1.contains(Vector3f(0.0f, -1.9f, 0.0f)), true);
    UTEST_EQU(box1.contains(Vector3f(0.0f, -2.1f, 0.0f)), false);
    UTEST_EQU(box1.contains(Vector3f(0.0f, 1.9f, 0.0f)), true);
    UTEST_EQU(box1.contains(Vector3f(0.0f, 2.1f, 0.0f)), false);

    UTEST_EQU(box1.contains(Vector3f(0.0f, 0.0f, -2.9f)), true);
    UTEST_EQU(box1.contains(Vector3f(0.0f, 0.0f, -3.1f)), false);
    UTEST_EQU(box1.contains(Vector3f(0.0f, 0.0f, 2.9f)), true);
    UTEST_EQU(box1.contains(Vector3f(0.0f, 0.0f, 3.1f)), false);

    //box1 : (-1, -2, -3) -> (1, 2, 3)

    box2.max = Vector3f(3.0, 4.0, 5.0);
    box2.min = Vector3f(-3.0, -4.0, -5.0); //box2 : (-3, -4, -5) -> (3, 4, 5)
    {
        AABoxf uBox(UNION, box1, box2);
        AABoxf iBox(INTER, box1, box2);
        UTEST_EQU(uBox.min, Vector3f(-3.0, -4.0, -5.0)); UTEST_EQU(uBox.max, Vector3f(3.0, 4.0, 5.0));
        UTEST_EQU(iBox.min, Vector3f(-1.0, -2.0, -3.0)); UTEST_EQU(iBox.max, Vector3f(1.0, 2.0, 3.0));

        UTEST_EQU(box1.includes(box2), false);
        UTEST_EQU(box1.isInside(box2), true);

        UTEST_EQU(box2.includes(box1), true);
        UTEST_EQU(box2.isInside(box1), false);

        UTEST_EQU(box1.inter(box2), true);
    }

    box2.min = Vector3f(0.0, 0.0, 0.0); //box2 : (0.0, 0.0, 0.0) -> (3, 4, 5)
    {
        AABoxf uBox(UNION, box1, box2);
        AABoxf iBox(INTER, box1, box2);
        UTEST_EQU(uBox.min, Vector3f(-1.0, -2.0, -3.0)); UTEST_EQU(uBox.max, Vector3f(3.0, 4.0, 5.0));
        UTEST_EQU(iBox.min, Vector3f( 0.0,  0.0,  0.0)); UTEST_EQU(iBox.max, Vector3f(1.0, 2.0, 3.0));

        UTEST_EQU(box1.includes(box2), false);
        UTEST_EQU(box1.isInside(box2), false);

        UTEST_EQU(box2.includes(box1), false);
        UTEST_EQU(box2.isInside(box1), false);

        UTEST_EQU(box1.inter(box2), true);
    }

    box2.min = Vector3f(1.1f, 0.0f, 0.0f); //box2 : (1.1, 0.0, 0.0) -> (3, 4, 5)
    {
        AABoxf uBox(UNION, box1, box2);
        AABoxf iBox(INTER, box1, box2);
        UTEST_EQU(uBox.min, Vector3f(-1.0, -2.0, -3.0)); UTEST_EQU(uBox.max, Vector3f(3.0, 4.0, 5.0));
        UTEST_EQU(iBox.isEmpty(), true);

        UTEST_EQU(box1.includes(box2), false);
        UTEST_EQU(box1.isInside(box2), false);

        UTEST_EQU(box2.includes(box1), false);
        UTEST_EQU(box2.isInside(box1), false);

        UTEST_EQU(box1.inter(box2), false);
    }

    box2.min = Vector3f(0.0f, 2.1f, 0.0f); //box2 : (0.0, 2.1, 0.0) -> (3, 4, 5)
    UTEST_EQU(box1.inter(box2), false);

    box2.min = Vector3f(0.0f, 0.0f, 3.1f); //box2 : (0.0, 0.0, 3.1) -> (3, 4, 5)
    UTEST_EQU(box1.inter(box2), false);

    AABoxf box3(UNION, box1, box2);
    UTEST_EQU(box3.min, Vector3f(-1.0, -2.0, -3.0));
    UTEST_EQU(box3.max, Vector3f(3.0, 4.0, 5.0));

    AABoxf box4;
    UTEST_EQU(box4.isEmpty(), true);
    box4.setUnion(box3);
    UTEST_EQU(box4.min, Vector3f(-1.0, -2.0, -3.0));
    UTEST_EQU(box4.max, Vector3f(3.0, 4.0, 5.0));
}


void testIntersect()
{
    //ray - AABox
    float distanceMin;
    Rayf ray_1 (Core::Ray_OriginTarget, Vector3f(10.f, 0.f, 0.f), Vector3f(0.f, 0.f, 0.f));
    Rayf ray_2 (Core::Ray_OriginTarget, Vector3f(10.f, 10.f, 10.f), Vector3f(20.f, 10.f, 10.f));
    AABoxf box_1 (Vector3f(-5.f, -5.f, -5.f), Vector3f(5.f, 5.f, 5.f));

    UTEST_EQU(intersect_RayAABox(ray_1, box_1, distanceMin), true);
    UTEST_EQU(distanceMin, 5.f);
    UTEST_EQU(intersect_RayAABox(ray_2, box_1, distanceMin), false);
    
    //AABox - Plane
    const Vector3f planeNormal (2.f, 3.5f, 4.f);
    Planef p_1 (Core::Plane_NormalDistance, Vector3f(1.f, 0.f, 0.f), 0);
    UTEST_EQU(intersect_AABoxPlane_Fast(box_1, p_1), true);
    
    Planef p_2 (Core::Plane_NormalDistance, Vector3f(0., 1.f, 0.f), 10);
    UTEST_EQU(intersect_AABoxPlane_Fast(box_1, p_2), true);
    
    Planef p_3 (Core::Plane_NormalDistance, Vector3f(0., 1.f, 0.f), -10);
    UTEST_EQU(intersect_AABoxPlane_Fast(box_1, p_3), false);
    
    UTEST_EQU(intersect_AABoxPlane_Accurate(box_1, p_1), IR_OVERLAP);
    UTEST_EQU(intersect_AABoxPlane_Accurate(box_1, p_2), IR_INSIDE);
    UTEST_EQU(intersect_AABoxPlane_Accurate(box_1, Planef(Core::Plane_NormalDistance, p_2.normal, -p_2.distance)), IR_OUTSIDE);
    
    
    //AABox - Frustum
    AABoxf box_2 (Vector3f(-5.f, -5.f, -5.f), Vector3f(-50.f, -50.f, -50.f));
    Core::Frustumf f_1;
    Core::Matrix4f mf_1;
    float left=-100, right=100, bottom=-100, top=100, near=0, far=100;
    mf_1.setFrustumProj(left, right, bottom, top, near, far);
    matrixToFrustum(mf_1, f_1);
    
    UTEST_EQU(intersect_AABoxFrustum_Fast(box_1,  f_1), true);
    UTEST_EQU(intersect_AABoxFrustum_Fast(box_2,  f_1), false);
}

void testPlanefClip()
{
    Planef plane(Plane_NormalPoint, Vector3f(0.0f, -1.0f, 0.0f), Vector3f(3.0f, 2.0f, 0.0f));

    Segmentf seg1(Vector3f(-1.0f, 3.0f, 0.0f), Vector3f( 1.0f, 1.0f, 0.0f));
    Segmentf seg2(Vector3f( 0.0f, 1.0f, 0.0f), Vector3f( 2.0f, 3.0f, 0.0f));
    Segmentf seg3(Vector3f(-3.0f, 3.0f, 0.0f), Vector3f(-1.0f, 1.0f, 0.0f));
    Segmentf seg4(Vector3f(-1.0f, 1.0f, 0.0f), Vector3f( 1.0f, 0.0f, 0.0f));

    plane.clip(seg1);
    plane.clip(seg2);
    plane.clip(seg3);
    plane.clip(seg4);

    // no clip
    UTEST_EQU(seg1.p1.isEquivalent(Vector3f(-1.0f, 3.0f, 0.0f)), true);
    UTEST_EQU(seg1.p2.isEquivalent(Vector3f( 0.0f, 2.0f, 0.0f)), true);

    // clip p1
    UTEST_EQU(seg2.p1.isEquivalent(Vector3f( 1.0f, 2.0f, 0.0f)), true);
    UTEST_EQU(seg2.p2.isEquivalent(Vector3f( 2.0f, 3.0f, 0.0f)), true);

    // clip p2
    UTEST_EQU(seg3.p1.isEquivalent(Vector3f(-3.0f, 3.0f, 0.0f)), true);
    UTEST_EQU(seg3.p2.isEquivalent(Vector3f(-2.0f, 2.0f, 0.0f)), true);

    // clip p1p2
    UTEST_EQU(seg4.p1.isEquivalent(Vector3f(-1.0f, 2.0f, 0.0f)), true);
    UTEST_EQU(seg4.p2.isEquivalent(Vector3f( 1.0f, 2.0f, 0.0f)), true);
}

void testFrustumfClip()
{
    Matrix4f  mat;
    Frustumf frustum;

    mat.setOrthoProj(-2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);
    matrixToFrustum(mat, frustum);

    Segmentf segA(Vector3f( 0.0f,-3.0f, 0.0f), Vector3f( 3.0f, 0.0f, 0.0f));
    Segmentf segB(Vector3f(-1.0f, 0.0f, 0.0f), Vector3f( 0.0f, 1.0f, 0.0f));
    Segmentf segC(Vector3f(-1.0f, 1.0f, 0.0f), Vector3f(-1.0f, 3.0f, 0.0f));
    Segmentf segD(Vector3f(-1.0f,-1.0f, 0.0f), Vector3f(-3.0f,-1.0f, 0.0f));
    Segmentf segE(Vector3f( 0.0f, 0.0f,-3.0f), Vector3f( 0.0f, 0.0f, 3.0f));

    frustum.clip(segA);
    frustum.clip(segB);
    frustum.clip(segC);
    frustum.clip(segD);
    frustum.clip(segE);

    // clip p1p2 YX
    UTEST_EQU(segA.p1.isEquivalent(Vector3f( 1.0f,-2.0f, 0.0f)), true);
    UTEST_EQU(segA.p2.isEquivalent(Vector3f( 2.0f,-1.0f, 0.0f)), true);

    // no clip
    UTEST_EQU(segB.p1.isEquivalent(Vector3f(-1.0f, 0.0f, 0.0f)), true);
    UTEST_EQU(segB.p2.isEquivalent(Vector3f( 0.0f, 1.0f, 0.0f)), true);

    // clip p2 Y
    UTEST_EQU(segC.p1.isEquivalent(Vector3f(-1.0f, 1.0f, 0.0f)), true);
    UTEST_EQU(segC.p2.isEquivalent(Vector3f(-1.0f, 2.0f, 0.0f)), true);

    // clip p2 X
    UTEST_EQU(segD.p1.isEquivalent(Vector3f(-1.0f,-1.0f, 0.0f)), true);
    UTEST_EQU(segD.p2.isEquivalent(Vector3f(-2.0f,-1.0f, 0.0f)), true);

    // clip p1p2 ZZ
    UTEST_EQU(segE.p1.isEquivalent(Vector3f( 0.0f, 0.0f,-2.0f)), true);
    UTEST_EQU(segE.p2.isEquivalent(Vector3f( 0.0f, 0.0f, 2.0f)), true);
}

void testMathGeometry()
{
    UTEST_START_TEST(L"MathGeometry")

    testRayf();
    testPlanef();
    testAABoxf();
    testIntersect();
    testPlanefClip();
    testFrustumfClip();
    
    UTEST_END_TEST;
}
