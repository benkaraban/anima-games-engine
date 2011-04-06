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
#include <iomanip>

using namespace Core;

void testMathGeometry();
void testKDTree();

void testMathTool()
{
    double d1 = 0;
   
    d1 = 0.000001;
    UTEST_EQU(isValid(d1), true);

    d1 = INFINITY;
    UTEST_EQU(isValid(d1), false);
}

static void testVec3fConstructor(const float X1, const float Y1, const float Z1)
{
    //constructeur
    Vector3f vec_1(X1, Y1, Z1);
    UTEST_EQU(vec_1._x, X1)
    UTEST_EQU(vec_1._y, Y1)
    UTEST_EQU(vec_1._z, Z1)

    const Vector3f const_vec_1 (vec_1);
    UTEST_EQU(vec_1._x, const_vec_1._x)
    UTEST_EQU(vec_1._y, const_vec_1._y)
    UTEST_EQU(vec_1._z, const_vec_1._z)
    Vector3f vec_3 (const_vec_1);
    UTEST_EQU(vec_3._x, const_vec_1._x)
    UTEST_EQU(vec_3._y, const_vec_1._y)
    UTEST_EQU(vec_3._z, const_vec_1._z)
    Vector3f vec_4 (ZERO);
    UTEST_EQU(vec_4._x, 0)
    UTEST_EQU(vec_4._y, 0)
    UTEST_EQU(vec_4._z, 0)
    Vector3f vec_5 (ONE);
    UTEST_EQU(vec_5._x, 1)
    UTEST_EQU(vec_5._y, 1)
    UTEST_EQU(vec_5._z, 1)
    Vector3f vec_tmp1 (const_vec_1);
    UTEST_EQU(vec_tmp1.isEquivalent(const_vec_1, f_COMMON_EPSILON), true);
}

static void testVec3fAccessor(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    const Vector3f const_vec_1 (vec_1);

    //accesseur
    UTEST_EQU(vec_1[0], X1)
    UTEST_EQU(vec_1[1], Y1)
    UTEST_EQU(vec_1[2], Z1)
    UTEST_EQU(const_vec_1[0], X1)
    UTEST_EQU(const_vec_1[1], Y1)
    UTEST_EQU(const_vec_1[2], Z1)
    UTEST_EQU(vec_1.data(), (float*)(&vec_1))
    UTEST_EQU(const_vec_1.data(), &(const_vec_1._x))
    vec_1.set(X1, Y1, Z1);
    UTEST_EQU(vec_1._x, X1)
    UTEST_EQU(vec_1._y, Y1)
    UTEST_EQU(vec_1._z, Z1)
    vec_1.setZero();
    UTEST_EQU(vec_1._x, 0)
    UTEST_EQU(vec_1._y, 0)
    UTEST_EQU(vec_1._z, 0)
    vec_1.setOne();
    UTEST_EQU(vec_1._x, 1)
    UTEST_EQU(vec_1._y, 1)
    UTEST_EQU(vec_1._z, 1)
}

static void testVec3fLength(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float length1Squared = X1*X1 + Y1*Y1 + Z1*Z1;
    const float length1 = L_sqrt(length1Squared);

    //length
    UTEST_EQUE(const_vec_1.getLengthSquared(), length1Squared, f_COMMON_EPSILON);
    UTEST_EQUE(const_vec_1.getLengthSafe(), length1, f_COMMON_EPSILON);
    UTEST_EQUE(const_vec_1.getLengthFast(), length1, f_COMMON_EPSILON_FAST);

    vec_3.setLength(length1);
    UTEST_EQUE(vec_3.getLengthSafe(), length1, f_COMMON_EPSILON);
}

static void testVec3fComp(const float X1, const float Y1, const float Z1)
{
    const float eps = 2.456f;
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);

   ////comparaison
    UTEST_EQU(const_vec_1==vec_1, true);
    UTEST_EQU(const_vec_1!=vec_1, false);
    vec_4._x = vec_1._x + eps;
    vec_4._y = vec_1._y + eps;
    vec_4._z = vec_1._z + eps;

    UTEST_EQU(vec_1.isEquivalent(vec_4, eps+0.0000000000001f), true);
    UTEST_EQU(vec_1.isEquivalent(vec_4, eps-f_COMMON_EPSILON), false);
    vec_3.set(eps, eps, eps);
    UTEST_EQU(vec_3.isZero(eps), true);
    UTEST_EQU(vec_3.isZero(eps-f_COMMON_EPSILON), false);
    vec_3.setLength(1);
    bool a = vec_3.isUnit();
    UTEST_EQU(a, true);
}

static void testVec3fNorm(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float length1Squared = X1*X1 + Y1*Y1 + Z1*Z1;
    const float length1 = L_sqrt(length1Squared);
    const Vector3f vec_1_norm(X1/length1, Y1/length1, Z1/length1);

    //normalize
    UTEST_EQU(const_vec_1.getNormalizedFast().isEquivalent(vec_1_norm, f_COMMON_EPSILON), true);
    UTEST_EQU(const_vec_1.getNormalizedSafe().isEquivalent(vec_1_norm, f_COMMON_EPSILON_FAST), true);
    vec_3 = const_vec_1;
    vec_4 = const_vec_1;
    vec_3.normalizeFast();
    UTEST_EQU(vec_3.isEquivalent(vec_1_norm, f_COMMON_EPSILON_FAST), true);
    vec_4.normalizeSafe();
    UTEST_EQU(vec_4.isEquivalent(vec_1_norm, f_COMMON_EPSILON), true);
}

static void testVec3fDistance(const float X1, const float Y1, const float Z1)
{
    //distance
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    vec_4.set(Z1, X1, Y1);
    const Vector3f const_vec_1 (vec_1);

    float valTmp1 = 0;
    float valTmp2 = 0;

    valTmp1 = const_vec_1.getDistanceSquared(vec_4);
    valTmp2 = (const_vec_1._x-vec_4._x)*(const_vec_1._x-vec_4._x) + (const_vec_1._y-vec_4._y)*(const_vec_1._y-vec_4._y)
            + (const_vec_1._z-vec_4._z)*(const_vec_1._z-vec_4._z);
    UTEST_EQUE(valTmp1, valTmp2, f_COMMON_EPSILON);
    valTmp1 = const_vec_1.getDistance(vec_4);
    UTEST_EQUE(valTmp1, sqrt(valTmp2), f_COMMON_EPSILON);
}

static void testVec3fLerp(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float eps = 2.456f;

    float valTmp1 = 0;
    float valTmp2 = 0;

    //lerp
    const Vector3f vec_min(-5, 2, -2);
    const Vector3f vec_max(vec_min._x+2, vec_min._y+8, vec_min._z+12);
    const float amount = 0.7f;
    vec_4.setLerp(vec_min, vec_max, amount);
    UTEST_EQUE(vec_4._x, vec_min._x + (vec_max._x-vec_min._x)*amount, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_min._y + (vec_max._y-vec_min._y)*amount, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_min._z + (vec_max._z-vec_min._z)*amount, f_COMMON_EPSILON);
}

static void testVec3fClamp(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float eps = 2.456f;

    float valTmp1 = 0;
    float valTmp2 = 0;

    //clamp
    const Vector3f vec_min(-5, 2, -2);
    const Vector3f vec_max(vec_min._x+2, vec_min._y+8, vec_min._z+12);
    vec_4.set(X1, Y1, Z1);
    vec_4.clamp(vec_min, vec_max);
    UTEST_EQU(vec_4._x >= vec_min._x, true);
    UTEST_EQU(vec_4._y >= vec_min._y, true);
    UTEST_EQU(vec_4._z >= vec_min._z, true);
    UTEST_EQU(vec_4._x <= vec_max._x, true);
    UTEST_EQU(vec_4._y <= vec_max._y, true);
    UTEST_EQU(vec_4._z <= vec_max._z, true);
}

static void testVec3fSnap(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float eps = 2.456f;

    float valTmp1 = 0;
    float valTmp2 = 0;

    //snap
    vec_4.set(X1, Y1, Z1);
    vec_4.snap();
    UTEST_EQU(vec_4._x, floor(X1 + 0.5));
    UTEST_EQU(vec_4._y, floor(Y1 + 0.5));
    UTEST_EQU(vec_4._z, floor(Z1 + 0.5));

    //operateur
    vec_3 = -const_vec_1;
    UTEST_EQUE(vec_3._x, X1*-1, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*-1, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*-1, f_COMMON_EPSILON);
}

static void testVec3fScalar(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float eps = 2.456f;

    float valTmp1 = 0;
    float valTmp2 = 0;

    //Vector3f _ Scalar
    vec_3 = const_vec_1;
    vec_3 = vec_3*eps;
    UTEST_EQUE(vec_3._x, X1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*eps, f_COMMON_EPSILON);
    vec_4 = eps*const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);

    vec_3 = const_vec_1;
    vec_3 *= eps;
    UTEST_EQUE(vec_3._x, X1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*eps, f_COMMON_EPSILON);

    vec_3 = const_vec_1;
    vec_3 = vec_3/eps;
    UTEST_EQUE(vec_3._x, X1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1/eps, f_COMMON_EPSILON);

    vec_3 = const_vec_1;
    vec_3 /= eps;
    UTEST_EQUE(vec_3._x, X1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1/eps, f_COMMON_EPSILON);
}

static void testVec3fOp(const float X1, const float Y1, const float Z1)
{
    Vector3f vec_1(X1, Y1, Z1);
    Vector3f vec_3(1.0f, 0.0f, 0.0f);
    Vector3f vec_4(1.0f, 0.0f, 0.0f);
    const Vector3f const_vec_1 (vec_1);
    const float eps = 2.456f;

    float valTmp1 = 0;
    float valTmp2 = 0;

    //Vector3f _ Vector3f
    vec_3 = const_vec_1*eps;
    vec_4 = vec_3 + const_vec_1;
    UTEST_EQUE(vec_4._x, vec_3._x+const_vec_1._x, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_3._y+const_vec_1._y, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_3._z+const_vec_1._z, f_COMMON_EPSILON);
    vec_3 += const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);

    vec_3 = const_vec_1*eps;
    vec_4 = vec_3 - const_vec_1;
    UTEST_EQUE(vec_4._x, vec_3._x-const_vec_1._x, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_3._y-const_vec_1._y, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_3._z-const_vec_1._z, f_COMMON_EPSILON);
    vec_3 -= const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);
}

static void testVector3(const float X1, const float Y1, const float Z1)
{
    testVec3fConstructor(X1, Y1, Z1);
    testVec3fAccessor(X1, Y1, Z1);
    testVec3fLength(X1, Y1, Z1);
    testVec3fComp(X1, Y1, Z1);
    testVec3fNorm(X1, Y1, Z1);
    testVec3fDistance(X1, Y1, Z1);
    testVec3fLerp(X1, Y1, Z1);
    testVec3fClamp(X1, Y1, Z1);
    testVec3fSnap(X1, Y1, Z1);
    testVec3fScalar(X1, Y1, Z1);
    testVec3fOp(X1, Y1, Z1);
}
static void testVector4(const float X1, const float Y1, const float Z1, const float W1)
{
    const float length1Squared = X1*X1 + Y1*Y1 + Z1*Z1 + W1*W1;
    const float length1 = L_sqrt(length1Squared);
    const Vector4f vec_1_norm(X1/length1, Y1/length1, Z1/length1, W1/length1);

    const float eps = 2.456f;
    float valTmp1 = 0;
    float valTmp2 = 0;

    //constructeur
    Vector4f vec_1(X1, Y1, Z1, W1);
    UTEST_EQU(vec_1._x, X1)
    UTEST_EQU(vec_1._y, Y1)
    UTEST_EQU(vec_1._z, Z1)
    UTEST_EQU(vec_1._w, W1)

    const Vector4f const_vec_1 (vec_1);
    UTEST_EQU(vec_1._x, const_vec_1._x)
    UTEST_EQU(vec_1._y, const_vec_1._y)
    UTEST_EQU(vec_1._z, const_vec_1._z)
    UTEST_EQU(vec_1._w, const_vec_1._w)
    Vector4f vec_3 (const_vec_1);
    UTEST_EQU(vec_3._x, const_vec_1._x)
    UTEST_EQU(vec_3._y, const_vec_1._y)
    UTEST_EQU(vec_3._z, const_vec_1._z)
    UTEST_EQU(vec_3._w, const_vec_1._w)
    Vector4f vec_4 (ZERO);
    UTEST_EQU(vec_4._x, 0)
    UTEST_EQU(vec_4._y, 0)
    UTEST_EQU(vec_4._z, 0)
    UTEST_EQU(vec_4._w, 0)
    Vector4f vec_5 (ONE);
    UTEST_EQU(vec_5._x, 1)
    UTEST_EQU(vec_5._y, 1)
    UTEST_EQU(vec_5._z, 1)
    UTEST_EQU(vec_5._w, 1)

    Vector4f vec_tmp1 (const_vec_1);
    UTEST_EQU(vec_tmp1.isEquivalent(const_vec_1, f_COMMON_EPSILON), true);

    //accesseur
    UTEST_EQU(vec_1[0], X1)
    UTEST_EQU(vec_1[1], Y1)
    UTEST_EQU(vec_1[2], Z1)
    UTEST_EQU(vec_1[3], W1)
    UTEST_EQU(const_vec_1[0], X1)
    UTEST_EQU(const_vec_1[1], Y1)
    UTEST_EQU(const_vec_1[2], Z1)
    UTEST_EQU(const_vec_1[3], W1)
    UTEST_EQU(vec_1.data(), (float*)(&vec_1))
    UTEST_EQU(const_vec_1.data(), &(const_vec_1._x))
    vec_4.set(X1, Y1, Z1, W1);
    UTEST_EQU(vec_4._x, X1)
    UTEST_EQU(vec_4._y, Y1)
    UTEST_EQU(vec_4._z, Z1)
    UTEST_EQU(vec_4._w, W1)
    vec_4.setZero();
    UTEST_EQU(vec_4._x, 0)
    UTEST_EQU(vec_4._y, 0)
    UTEST_EQU(vec_4._z, 0)
    UTEST_EQU(vec_4._w, 0)

    //length
    UTEST_EQUE(const_vec_1.getLengthSquared(), length1Squared, f_COMMON_EPSILON);
    UTEST_EQUE(const_vec_1.getLengthSafe(), length1, f_COMMON_EPSILON);
    UTEST_EQUE(const_vec_1.getLengthFast(), length1, f_COMMON_EPSILON_FAST);

    vec_3.setLength(length1);
    UTEST_EQUE(vec_3.getLengthSafe(), length1, f_COMMON_EPSILON);

    //comparaison
    UTEST_EQU(const_vec_1==vec_1, true);
    UTEST_EQU(const_vec_1!=vec_1, false);
    vec_4._x = vec_1._x + eps;
    vec_4._y = vec_1._y + eps;
    vec_4._z = vec_1._z + eps;
    vec_4._w = vec_1._w + eps;
    UTEST_EQU(vec_1.isEquivalent(vec_4, eps+0.00001f), true);
    UTEST_EQU(vec_1.isEquivalent(vec_4, eps-f_COMMON_EPSILON), false);
    vec_3.set(eps, eps, eps, eps);
    UTEST_EQU(vec_3.isZero(eps), true);
    UTEST_EQU(vec_3.isZero(eps-f_COMMON_EPSILON), false);
    vec_3.setLength(1);
    UTEST_EQU(vec_3.isUnit(), true);

    //normalize
    UTEST_EQU(const_vec_1.getNormalizedFast().isEquivalent(vec_1_norm, f_COMMON_EPSILON), true);
    UTEST_EQU(const_vec_1.getNormalizedSafe().isEquivalent(vec_1_norm, f_COMMON_EPSILON_FAST), true);
    vec_3 = const_vec_1;
    vec_4 = const_vec_1;
    vec_3.normalizeFast();
    UTEST_EQU(vec_3.isEquivalent(vec_1_norm, f_COMMON_EPSILON_FAST), true);
    vec_4.normalizeSafe();
    UTEST_EQU(vec_4.isEquivalent(vec_1_norm, f_COMMON_EPSILON), true);

    //distance
    vec_4.set(Z1, X1, Y1, W1);

    valTmp1 = const_vec_1.getDistanceSquared(vec_4);
    valTmp2 = (const_vec_1._x-vec_4._x)*(const_vec_1._x-vec_4._x) + (const_vec_1._y-vec_4._y)*(const_vec_1._y-vec_4._y)
            + (const_vec_1._z-vec_4._z)*(const_vec_1._z-vec_4._z) + (const_vec_1._w-vec_4._w)*(const_vec_1._w-vec_4._w);
    UTEST_EQUE(valTmp1, valTmp2, f_COMMON_EPSILON);
    valTmp1 = const_vec_1.getDistance(vec_4);
    UTEST_EQUE(valTmp1, sqrt(valTmp2), f_COMMON_EPSILON);

    //lerp
    const Vector4f vec_min(-5, 2, -2, 8);
    const Vector4f vec_max(vec_min._x+2, vec_min._y+8, vec_min._z+12, vec_min._w+22.235f);
    const float amount = 0.7f;
    vec_4.setLerp(vec_min, vec_max, amount);
    UTEST_EQUE(vec_4._x, vec_min._x + (vec_max._x-vec_min._x)*amount, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_min._y + (vec_max._y-vec_min._y)*amount, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_min._z + (vec_max._z-vec_min._z)*amount, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._w, vec_min._w + (vec_max._w-vec_min._w)*amount, f_COMMON_EPSILON);

    //clamp
    vec_4.set(X1, Y1, Z1, W1);
    vec_4.clamp(vec_min, vec_max);
    UTEST_EQU(vec_4._x >= vec_min._x, true);
    UTEST_EQU(vec_4._y >= vec_min._y, true);
    UTEST_EQU(vec_4._z >= vec_min._z, true);
    UTEST_EQU(vec_4._w >= vec_min._w, true);
    UTEST_EQU(vec_4._x <= vec_max._x, true);
    UTEST_EQU(vec_4._y <= vec_max._y, true);
    UTEST_EQU(vec_4._z <= vec_max._z, true);
    UTEST_EQU(vec_4._w <= vec_max._w, true);

    //operateur
    vec_3 = -const_vec_1;
    UTEST_EQUE(vec_3._x, X1*-1, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*-1, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*-1, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._w, W1*-1, f_COMMON_EPSILON);

    //Vector4f _ Scalar
    vec_3 = const_vec_1;
    vec_3 = vec_3*eps;
    UTEST_EQUE(vec_3._x, X1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._w, W1*eps, f_COMMON_EPSILON);
    vec_4 = eps*const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);

    vec_3 = const_vec_1;
    vec_3 *= eps;
    UTEST_EQUE(vec_3._x, X1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1*eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._w, W1*eps, f_COMMON_EPSILON);

    vec_3 = const_vec_1;
    vec_3 = vec_3/eps;
    UTEST_EQUE(vec_3._x, X1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._w, W1/eps, f_COMMON_EPSILON);

    vec_3 = const_vec_1;
    vec_3 /= eps;
    UTEST_EQUE(vec_3._x, X1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._y, Y1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._z, Z1/eps, f_COMMON_EPSILON);
    UTEST_EQUE(vec_3._w, W1/eps, f_COMMON_EPSILON);

    //Vector4f _ Vector4f
    vec_3 = const_vec_1*eps;
    vec_4 = vec_3 + const_vec_1;
    UTEST_EQUE(vec_4._x, vec_3._x+const_vec_1._x, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_3._y+const_vec_1._y, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_3._z+const_vec_1._z, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._w, vec_3._w+const_vec_1._w, f_COMMON_EPSILON);
    vec_3 += const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);

    vec_3 = const_vec_1*eps;
    vec_4 = vec_3 - const_vec_1;
    UTEST_EQUE(vec_4._x, vec_3._x-const_vec_1._x, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._y, vec_3._y-const_vec_1._y, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._z, vec_3._z-const_vec_1._z, f_COMMON_EPSILON);
    UTEST_EQUE(vec_4._w, vec_3._w-const_vec_1._w, f_COMMON_EPSILON);
    vec_3 -= const_vec_1;
    UTEST_EQU(vec_3.isEquivalent(vec_4, f_COMMON_EPSILON), true);
}

static void testMatrix3(const float M00, const float M01, const float M02, const float M10, const float M11, const float M12,
                 const float M20, const float M21, const float M22)
{
    const Core::Vector3f line_0(M00, M01, M02);
    const Core::Vector3f line_1(M10, M11, M12);
    const Core::Vector3f line_2(M20, M21, M22);

    const float value = 45.45687f;
    float valueTmp = 45.45687f;
    const float det_1 = (M00*M11*M22) + (M01*M12*M20) + (M02*M10*M21) - (M02*M11*M20) - (M00*M12*M21) - (M01*M10*M22);
    const Core::Vector3f vec_1(M00, M11, M22);
    Core::Vector3f vecTmp;

    //constructeur
    Matrix3f mat_1(line_0, line_1, line_2);
    UTEST_EQU(mat_1._00, M00) UTEST_EQU(mat_1._01, M01) UTEST_EQU(mat_1._02, M02)
    UTEST_EQU(mat_1._10, M10) UTEST_EQU(mat_1._11, M11) UTEST_EQU(mat_1._12, M12)
    UTEST_EQU(mat_1._20, M20) UTEST_EQU(mat_1._21, M21) UTEST_EQU(mat_1._22, M22)

    const Matrix3f const_mat_1 (M00, M01, M02,
            M10, M11, M12,
            M20, M21, M22);
    UTEST_EQU(const_mat_1.isEquivalent(mat_1, f_COMMON_EPSILON), true);

    Matrix3f mat_tmp1(IDENTITY);
    UTEST_EQU(mat_tmp1._00, 1) UTEST_EQU(mat_tmp1._01, 0) UTEST_EQU(mat_tmp1._02, 0)
    UTEST_EQU(mat_tmp1._10, 0) UTEST_EQU(mat_tmp1._11, 1) UTEST_EQU(mat_tmp1._12, 0)
    UTEST_EQU(mat_tmp1._20, 0) UTEST_EQU(mat_tmp1._21, 0) UTEST_EQU(mat_tmp1._22, 1)

    Matrix3f mat_tmp2 (ZERO);
    UTEST_EQU(mat_tmp2._00, 0) UTEST_EQU(mat_tmp2._01, 0) UTEST_EQU(mat_tmp2._02, 0)
    UTEST_EQU(mat_tmp2._10, 0) UTEST_EQU(mat_tmp2._11, 0) UTEST_EQU(mat_tmp2._12, 0)
    UTEST_EQU(mat_tmp2._20, 0) UTEST_EQU(mat_tmp2._21, 0) UTEST_EQU(mat_tmp2._22, 0)

    Matrix3f mat_tmp3 (const_mat_1);
    UTEST_EQU(mat_tmp3.isEquivalent(const_mat_1, f_COMMON_EPSILON), true);

    //comparaison
    UTEST_EQU(mat_tmp1.isIdentity(), true);
    UTEST_EQU(mat_tmp2.isZero(), true);
    UTEST_EQU(mat_1 == const_mat_1, true);
    mat_1._00 += value;
    UTEST_EQU(mat_1 != const_mat_1, true);
    UTEST_EQU(const_mat_1.isEquivalent(mat_1, f_COMMON_EPSILON), false);
    mat_1._00 -= value;

    //accesseur
    mat_tmp1.set(M00, M01, M02, M10, M11, M12, M20, M21, M22);
    UTEST_EQU(mat_tmp1.isEquivalent(const_mat_1, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[0].isEquivalent(line_0, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[1].isEquivalent(line_1, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[2].isEquivalent(line_2, f_COMMON_EPSILON), true)

    UTEST_EQU(mat_1._line[1][0], M10)

    UTEST_EQU(const_mat_1[0].isEquivalent(line_0, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[1].isEquivalent(line_1, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[2].isEquivalent(line_2, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[0][0], M00)
    mat_tmp1.setZero();
    UTEST_EQU(mat_tmp1.isIdentity(), false);
    mat_tmp1.setIdentity();
    UTEST_EQU(mat_tmp1.isIdentity(), true);
    UTEST_EQU(mat_1.data(), (float*)(&mat_1))
    UTEST_EQU(const_mat_1.data(), &(const_mat_1._00))

    //transpose
    mat_tmp1 = const_mat_1.getTransposed();
    UTEST_EQU(mat_tmp1._00, M00) UTEST_EQU(mat_tmp1._01, M10) UTEST_EQU(mat_tmp1._02, M20)
    UTEST_EQU(mat_tmp1._10, M01) UTEST_EQU(mat_tmp1._11, M11) UTEST_EQU(mat_tmp1._12, M21)
    UTEST_EQU(mat_tmp1._20, M02) UTEST_EQU(mat_tmp1._21, M12) UTEST_EQU(mat_tmp1._22, M22)
    mat_1.transposeSelf();
    UTEST_EQU(mat_tmp1.isEquivalent(mat_1, f_COMMON_EPSILON), true);
    mat_1 = const_mat_1;

    //to matrice4
    Core::Matrix4f mat44 = const_mat_1.toMatrix4();
    UTEST_EQU(mat44._00, M00) UTEST_EQU(mat44._01, M01) UTEST_EQU(mat44._02, M02) UTEST_EQU(mat44._03, 0)
    UTEST_EQU(mat44._10, M10) UTEST_EQU(mat44._11, M11) UTEST_EQU(mat44._12, M12) UTEST_EQU(mat44._13, 0)
    UTEST_EQU(mat44._20, M20) UTEST_EQU(mat44._21, M21) UTEST_EQU(mat44._22, M22) UTEST_EQU(mat44._23, 0)
    UTEST_EQU(mat44._30, 0) UTEST_EQU(mat44._31, 0) UTEST_EQU(mat44._32, 0) UTEST_EQU(mat44._33, 1)

    //operateur
    //Matrice3 _ Scalar
    mat_tmp1 = const_mat_1*value;
    UTEST_EQUE(mat_tmp1._00, M00*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._01, M01*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._02, M02*value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._10, M10*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._11, M11*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._12, M12*value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._20, M20*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._21, M21*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._22, M22*value, f_COMMON_EPSILON)
    mat_tmp2 = const_mat_1;
    mat_tmp2 *= value;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    mat_tmp1 = const_mat_1/value;
    UTEST_EQUE(mat_tmp1._00, M00/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._01, M01/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._02, M02/value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._10, M10/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._11, M11/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._12, M12/value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._20, M20/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._21, M21/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._22, M22/value, f_COMMON_EPSILON)
    mat_tmp2 = const_mat_1;
    mat_tmp2 /= value;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);

    //Matrice3 _ Vecteur3
    //postmul
    vecTmp = const_mat_1 * vec_1;
    UTEST_EQUE(vecTmp._x, const_mat_1._00 * vec_1._x + const_mat_1._01 * vec_1._y + const_mat_1._02 * vec_1._z, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._y, const_mat_1._10 * vec_1._x + const_mat_1._11 * vec_1._y + const_mat_1._12 * vec_1._z, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._z, const_mat_1._20 * vec_1._x + const_mat_1._21 * vec_1._y + const_mat_1._22 * vec_1._z, f_COMMON_EPSILON)
    //premul
    vecTmp = vec_1 * const_mat_1;
    UTEST_EQUE(vecTmp._x, const_mat_1._00 * vec_1._x + const_mat_1._10 * vec_1._y + const_mat_1._20 * vec_1._z, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._y, const_mat_1._01 * vec_1._x + const_mat_1._11 * vec_1._y + const_mat_1._21 * vec_1._z, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._z, const_mat_1._02 * vec_1._x + const_mat_1._12 * vec_1._y + const_mat_1._22 * vec_1._z, f_COMMON_EPSILON)

    //Matrice3 _ Matrice3
    //addition
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 + const_mat_1;
    UTEST_EQUE(mat_tmp2._00, mat_tmp1._00+const_mat_1._00, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._01, mat_tmp1._01+const_mat_1._01, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._02, mat_tmp1._02+const_mat_1._02, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._10, mat_tmp1._10+const_mat_1._10, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._11, mat_tmp1._11+const_mat_1._11, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._12, mat_tmp1._12+const_mat_1._12, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._20, mat_tmp1._20+const_mat_1._20, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._21, mat_tmp1._21+const_mat_1._21, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._22, mat_tmp1._22+const_mat_1._22, f_COMMON_EPSILON)
    mat_tmp1 += const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    //soustraction
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 - const_mat_1;
    UTEST_EQUE(mat_tmp2._00, mat_tmp1._00-const_mat_1._00, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._01, mat_tmp1._01-const_mat_1._01, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._02, mat_tmp1._02-const_mat_1._02, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._10, mat_tmp1._10-const_mat_1._10, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._11, mat_tmp1._11-const_mat_1._11, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._12, mat_tmp1._12-const_mat_1._12, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._20, mat_tmp1._20-const_mat_1._20, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._21, mat_tmp1._21-const_mat_1._21, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._22, mat_tmp1._22-const_mat_1._22, f_COMMON_EPSILON)
    mat_tmp1 -= const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    //mul
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 * const_mat_1;
    UTEST_EQU(mat_tmp2._00, mat_tmp1._00*const_mat_1._00 + mat_tmp1._01*const_mat_1._10 + mat_tmp1._02*const_mat_1._20)
    UTEST_EQU(mat_tmp2._10, mat_tmp1._10*const_mat_1._00 + mat_tmp1._11*const_mat_1._10 + mat_tmp1._12*const_mat_1._20)
    UTEST_EQU(mat_tmp2._20, mat_tmp1._20*const_mat_1._00 + mat_tmp1._21*const_mat_1._10 + mat_tmp1._22*const_mat_1._20)
    UTEST_EQU(mat_tmp2._01, mat_tmp1._00*const_mat_1._01 + mat_tmp1._01*const_mat_1._11 + mat_tmp1._02*const_mat_1._21)
    UTEST_EQU(mat_tmp2._11, mat_tmp1._10*const_mat_1._01 + mat_tmp1._11*const_mat_1._11 + mat_tmp1._12*const_mat_1._21)
    UTEST_EQU(mat_tmp2._21, mat_tmp1._20*const_mat_1._01 + mat_tmp1._21*const_mat_1._11 + mat_tmp1._22*const_mat_1._21)
    UTEST_EQU(mat_tmp2._02, mat_tmp1._00*const_mat_1._02 + mat_tmp1._01*const_mat_1._12 + mat_tmp1._02*const_mat_1._22)
    UTEST_EQU(mat_tmp2._12, mat_tmp1._10*const_mat_1._02 + mat_tmp1._11*const_mat_1._12 + mat_tmp1._12*const_mat_1._22)
    UTEST_EQU(mat_tmp2._22, mat_tmp1._20*const_mat_1._02 + mat_tmp1._21*const_mat_1._12 + mat_tmp1._22*const_mat_1._22)
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp1 *= const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);

    //inverse
    valueTmp = const_mat_1.determinant();
    UTEST_EQUE(valueTmp, det_1, f_COMMON_EPSILON);
    const Matrix3f inv_1((M11*M22-M12*M21)/det_1, (M02*M21-M01*M22)/det_1, (M01*M12-M02*M11)/det_1, (M12*M20-M10*M22)
            /det_1, (M00*M22-M02*M20)/det_1, (M02*M10-M00*M12)/det_1, (M10*M21-M11*M20)/det_1, (M01*M20-M00*M21)/det_1,
                        (M00*M11-M01*M10)/det_1);
    mat_tmp1 = const_mat_1.getInvertedSafe();
    UTEST_EQU(mat_tmp1.isEquivalent(inv_1, 0.000000001f), true);
    mat_tmp1 = const_mat_1;
    mat_tmp1.inverseSafeSelf();
    UTEST_EQU(mat_tmp1.isEquivalent(inv_1, 0.000000001f), true);

    //rotation
    //TODO

    //make
    //TODO
}

static void testMatrix4(const float M00, const float M01, const float M02, const float M03, const float M10, const float M11,
                                      const float M12, const float M13, const float M20, const float M21, const float M22, const float M23,
                                      const float M30, const float M31, const float M32, const float M33)
{
    const Core::Vector4f line_0(M00, M01, M02, M03);
    const Core::Vector4f line_1(M10, M11, M12, M13);
    const Core::Vector4f line_2(M20, M21, M22, M23);
    const Core::Vector4f line_3(M30, M31, M32, M33);

    const Core::Vector3f vx3(M00, M10, M20);
    const Core::Vector3f vy3(M01, M11, M21);
    const Core::Vector3f vz3(M02, M12, M22);
    const Core::Vector3f vt3(M03, M13, M23);

    const Core::Vector4f vx4(M00, M10, M20, M30);
    const Core::Vector4f vy4(M01, M11, M21, M31);
    const Core::Vector4f vz4(M02, M12, M22, M32);
    const Core::Vector4f vt4(M03, M13, M23, M33);

    const float value = 45.45687f;
    float valueTmp = 45.45687f;
    //const float det_1 = (M00*M11*M22) + (M01*M12*M20) + (M02*M10*M21) - (M02*M11*M20) - (M00*M12*M21) - (M01*M10*M22);
    const Core::Vector4f vec_1(M00, M11, M22, M33);
    Core::Vector4f vecTmp;

    const Core::Matrix3f mat33_1(M10, M31, M00, M30, M30, M13, M13, M02, M23);

    //constructeur
    Matrix4f mat_1(MAT_LINES, line_0, line_1, line_2, line_3);
    UTEST_EQU(mat_1._00, M00) UTEST_EQU(mat_1._01, M01) UTEST_EQU(mat_1._02, M02) UTEST_EQU(mat_1._03, M03)
    UTEST_EQU(mat_1._10, M10) UTEST_EQU(mat_1._11, M11) UTEST_EQU(mat_1._12, M12) UTEST_EQU(mat_1._13, M13)
    UTEST_EQU(mat_1._20, M20) UTEST_EQU(mat_1._21, M21) UTEST_EQU(mat_1._22, M22) UTEST_EQU(mat_1._23, M23)
    UTEST_EQU(mat_1._30, M30) UTEST_EQU(mat_1._31, M31) UTEST_EQU(mat_1._32, M32) UTEST_EQU(mat_1._33, M33)

    const Matrix4f const_mat_1 (M00, M01, M02, M03,
            M10, M11, M12, M13,
            M20, M21, M22, M23,
            M30, M31, M32, M33);
    UTEST_EQU(const_mat_1.isEquivalent(mat_1, 3*f_COMMON_EPSILON), true);

    Matrix4f mat_1_axis3(MAT_AXIS, vx3, vy3, vz3, vt3);
    Matrix4f mat_1_axis4(MAT_AXIS, vx4, vy4, vz4, vt4);

    UTEST_EQU(const_mat_1, mat_1_axis4);
    for(int32 ii=0; ii < 3; ii++)
        UTEST_EQU(mat_1_axis3[ii], mat_1_axis4[ii]
                                               );

    Matrix4f mat_tmp1(IDENTITY);
    UTEST_EQU(mat_tmp1._00, 1) UTEST_EQU(mat_tmp1._01, 0) UTEST_EQU(mat_tmp1._02, 0) UTEST_EQU(mat_tmp1._03, 0)
    UTEST_EQU(mat_tmp1._10, 0) UTEST_EQU(mat_tmp1._11, 1) UTEST_EQU(mat_tmp1._12, 0) UTEST_EQU(mat_tmp1._13, 0)
    UTEST_EQU(mat_tmp1._20, 0) UTEST_EQU(mat_tmp1._21, 0) UTEST_EQU(mat_tmp1._22, 1) UTEST_EQU(mat_tmp1._23, 0)
    UTEST_EQU(mat_tmp1._30, 0) UTEST_EQU(mat_tmp1._31, 0) UTEST_EQU(mat_tmp1._32, 0) UTEST_EQU(mat_tmp1._33, 1)

    Matrix4f mat_tmp2 (ZERO);
    UTEST_EQU(mat_tmp2._00, 0) UTEST_EQU(mat_tmp2._01, 0) UTEST_EQU(mat_tmp2._02, 0) UTEST_EQU(mat_tmp2._03, 0)
    UTEST_EQU(mat_tmp2._10, 0) UTEST_EQU(mat_tmp2._11, 0) UTEST_EQU(mat_tmp2._12, 0) UTEST_EQU(mat_tmp2._13, 0)
    UTEST_EQU(mat_tmp2._20, 0) UTEST_EQU(mat_tmp2._21, 0) UTEST_EQU(mat_tmp2._22, 0) UTEST_EQU(mat_tmp2._23, 0)
    UTEST_EQU(mat_tmp2._30, 0) UTEST_EQU(mat_tmp2._31, 0) UTEST_EQU(mat_tmp2._32, 0) UTEST_EQU(mat_tmp2._33, 0)

    Matrix4f mat_tmp3 (const_mat_1);
    UTEST_EQU(mat_tmp3.isEquivalent(const_mat_1, f_COMMON_EPSILON), true);

    //comparaison
    UTEST_EQU(mat_tmp1.isIdentity(), true);
    UTEST_EQU(mat_tmp2.isZero(), true);
    UTEST_EQU(mat_1 == const_mat_1, true);
    mat_1._00 += value;
    UTEST_EQU(mat_1 != const_mat_1, true);
    UTEST_EQU(const_mat_1.isEquivalent(mat_1, f_COMMON_EPSILON), false);
    mat_1._00 -= value;

    //accesseur
    mat_tmp1.set(M00, M01, M02, M03, M10, M11, M12, M13, M20, M21, M22, M23, M30, M31, M32, M33);
    UTEST_EQU(mat_tmp1.isEquivalent(const_mat_1, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[0].isEquivalent(line_0, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[1].isEquivalent(line_1, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[2].isEquivalent(line_2, f_COMMON_EPSILON), true)
    UTEST_EQU(mat_1[3].isEquivalent(line_3, f_COMMON_EPSILON), true)

    UTEST_EQU(mat_tmp1._line[1][0], M10)

    UTEST_EQU(const_mat_1[0].isEquivalent(line_0, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[1].isEquivalent(line_1, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[2].isEquivalent(line_2, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[3].isEquivalent(line_3, f_COMMON_EPSILON), true)
    UTEST_EQU(const_mat_1[0][0], M00)
    mat_tmp1.setZero();
    UTEST_EQU(mat_tmp1.isIdentity(), false);
    mat_tmp1.setIdentity();
    UTEST_EQU(mat_tmp1.isIdentity(), true);
    UTEST_EQU(mat_1.data(), (float*)(&mat_1))
    UTEST_EQU(const_mat_1.data(), &(const_mat_1._00))

    //transpose
    mat_tmp1 = const_mat_1.getTransposed();
    UTEST_EQU(mat_tmp1._00, M00) UTEST_EQU(mat_tmp1._01, M10) UTEST_EQU(mat_tmp1._02, M20) UTEST_EQU(mat_tmp1._03, M30)
    UTEST_EQU(mat_tmp1._10, M01) UTEST_EQU(mat_tmp1._11, M11) UTEST_EQU(mat_tmp1._12, M21) UTEST_EQU(mat_tmp1._13, M31)
    UTEST_EQU(mat_tmp1._20, M02) UTEST_EQU(mat_tmp1._21, M12) UTEST_EQU(mat_tmp1._22, M22) UTEST_EQU(mat_tmp1._23, M32)
    UTEST_EQU(mat_tmp1._30, M03) UTEST_EQU(mat_tmp1._31, M13) UTEST_EQU(mat_tmp1._32, M23) UTEST_EQU(mat_tmp1._33, M33)
    mat_1.transposeSelf();
    UTEST_EQU(mat_tmp1.isEquivalent(mat_1, f_COMMON_EPSILON), true);
    mat_1 = const_mat_1;

    //operateur
    //Matrice4 _ Scalar
    mat_tmp1 = const_mat_1*value;
    UTEST_EQUE(mat_tmp1._00, M00*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._01, M01*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._02, M02*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._03, M03*value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._10, M10*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._11, M11*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._12, M12*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._13, M13*value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._20, M20*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._21, M21*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._22, M22*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._23, M23*value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._30, M30*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._31, M31*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._32, M32*value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._33, M33*value, f_COMMON_EPSILON)
    mat_tmp2 = const_mat_1;
    mat_tmp2 *= value;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    mat_tmp1 = const_mat_1/value;
    UTEST_EQUE(mat_tmp1._00, M00/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._01, M01/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._02, M02/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._03, M03/value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._10, M10/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._11, M11/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._12, M12/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._13, M13/value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._20, M20/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._21, M21/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._22, M22/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._23, M23/value, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp1._30, M30/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._31, M31/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._32, M32/value, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp1._33, M33/value, f_COMMON_EPSILON)
    mat_tmp2 = const_mat_1;
    mat_tmp2 /= value;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);

    //Matrice4 _ Vecteur4
    //postmul
    vecTmp = const_mat_1 * vec_1;
    UTEST_EQUE(vecTmp._x, const_mat_1._00 * vec_1._x + const_mat_1._01 * vec_1._y + const_mat_1._02 * vec_1._z + const_mat_1._03 * vec_1._w, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._y, const_mat_1._10 * vec_1._x + const_mat_1._11 * vec_1._y + const_mat_1._12 * vec_1._z + const_mat_1._13 * vec_1._w, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._z, const_mat_1._20 * vec_1._x + const_mat_1._21 * vec_1._y + const_mat_1._22 * vec_1._z + const_mat_1._23 * vec_1._w, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._w, const_mat_1._30 * vec_1._x + const_mat_1._31 * vec_1._y + const_mat_1._32 * vec_1._z + const_mat_1._33 * vec_1._w, f_COMMON_EPSILON)
    //premul
    vecTmp = vec_1 * const_mat_1;
    UTEST_EQUE(vecTmp._x, const_mat_1._00 * vec_1._x + const_mat_1._10 * vec_1._y + const_mat_1._20 * vec_1._z + const_mat_1._30 * vec_1._w, f_COMMON_EPSILON)
    valueTmp = const_mat_1._01 * vec_1._x + const_mat_1._11 * vec_1._y + const_mat_1._21 * vec_1._z + const_mat_1._31 * vec_1._w;
    UTEST_EQUE(vecTmp._y, valueTmp, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._z, const_mat_1._02 * vec_1._x + const_mat_1._12 * vec_1._y + const_mat_1._22 * vec_1._z + const_mat_1._32 * vec_1._w, f_COMMON_EPSILON)
    UTEST_EQUE(vecTmp._w, const_mat_1._03 * vec_1._x + const_mat_1._13 * vec_1._y + const_mat_1._23 * vec_1._z + const_mat_1._33 * vec_1._w, f_COMMON_EPSILON)
    //Matrice4 _ Matrice4
    //addition
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 + const_mat_1;
    UTEST_EQUE(mat_tmp2._00, mat_tmp1._00+const_mat_1._00, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._01, mat_tmp1._01+const_mat_1._01, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._02, mat_tmp1._02+const_mat_1._02, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._03, mat_tmp1._03+const_mat_1._03, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._10, mat_tmp1._10+const_mat_1._10, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._11, mat_tmp1._11+const_mat_1._11, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._12, mat_tmp1._12+const_mat_1._12, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._13, mat_tmp1._13+const_mat_1._13, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._20, mat_tmp1._20+const_mat_1._20, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._21, mat_tmp1._21+const_mat_1._21, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._22, mat_tmp1._22+const_mat_1._22, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._23, mat_tmp1._23+const_mat_1._23, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._30, mat_tmp1._30+const_mat_1._30, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._31, mat_tmp1._31+const_mat_1._31, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._32, mat_tmp1._32+const_mat_1._32, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._33, mat_tmp1._33+const_mat_1._33, f_COMMON_EPSILON)
    mat_tmp1 += const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    //soustraction
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 - const_mat_1;
    UTEST_EQUE(mat_tmp2._00, mat_tmp1._00-const_mat_1._00, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._01, mat_tmp1._01-const_mat_1._01, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._02, mat_tmp1._02-const_mat_1._02, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._03, mat_tmp1._03-const_mat_1._03, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._10, mat_tmp1._10-const_mat_1._10, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._11, mat_tmp1._11-const_mat_1._11, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._12, mat_tmp1._12-const_mat_1._12, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._13, mat_tmp1._13-const_mat_1._13, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._20, mat_tmp1._20-const_mat_1._20, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._21, mat_tmp1._21-const_mat_1._21, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._22, mat_tmp1._22-const_mat_1._22, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._23, mat_tmp1._23-const_mat_1._23, f_COMMON_EPSILON)
    UTEST_EQUE(mat_tmp2._30, mat_tmp1._30-const_mat_1._30, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._31, mat_tmp1._31-const_mat_1._31, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._32, mat_tmp1._32-const_mat_1._32, f_COMMON_EPSILON) UTEST_EQUE(mat_tmp2._33, mat_tmp1._33-const_mat_1._33, f_COMMON_EPSILON)
    mat_tmp1 -= const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);
    //mul
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 * const_mat_1;
    valueTmp = mat_tmp1._00*const_mat_1._00 + mat_tmp1._01*const_mat_1._10 + mat_tmp1._02*const_mat_1._20
            + mat_tmp1._03*const_mat_1._30;
    UTEST_EQU(mat_tmp2._00, valueTmp)
    valueTmp = mat_tmp1._10*const_mat_1._00 + mat_tmp1._11*const_mat_1._10 + mat_tmp1._12*const_mat_1._20 + mat_tmp1._13*const_mat_1._30;
    UTEST_EQU(mat_tmp2._10, valueTmp)
    valueTmp = mat_tmp1._20*const_mat_1._00 + mat_tmp1._21*const_mat_1._10 + mat_tmp1._22*const_mat_1._20 + mat_tmp1._23*const_mat_1._30;
    UTEST_EQU(mat_tmp2._20, valueTmp)
    valueTmp = mat_tmp1._30*const_mat_1._00 + mat_tmp1._31*const_mat_1._10 + mat_tmp1._32*const_mat_1._20 + mat_tmp1._33*const_mat_1._30;
    UTEST_EQU(mat_tmp2._30, valueTmp)
    valueTmp = mat_tmp1._00*const_mat_1._01 + mat_tmp1._01*const_mat_1._11 + mat_tmp1._02*const_mat_1._21 + mat_tmp1._03*const_mat_1._31;
    UTEST_EQU(mat_tmp2._01, valueTmp)
    valueTmp = mat_tmp1._10*const_mat_1._01 + mat_tmp1._11*const_mat_1._11 + mat_tmp1._12*const_mat_1._21 + mat_tmp1._13*const_mat_1._31;
    UTEST_EQU(mat_tmp2._11, valueTmp)
    valueTmp = mat_tmp1._20*const_mat_1._01 + mat_tmp1._21*const_mat_1._11 + mat_tmp1._22*const_mat_1._21 + mat_tmp1._23*const_mat_1._31;
    UTEST_EQU(mat_tmp2._21, valueTmp)
    valueTmp = mat_tmp1._30*const_mat_1._01 + mat_tmp1._31*const_mat_1._11 + mat_tmp1._32*const_mat_1._21 + mat_tmp1._33*const_mat_1._31;
    UTEST_EQU(mat_tmp2._31, valueTmp)
    valueTmp = mat_tmp1._00*const_mat_1._02 + mat_tmp1._01*const_mat_1._12 + mat_tmp1._02*const_mat_1._22 + mat_tmp1._03*const_mat_1._32;
    UTEST_EQU(mat_tmp2._02, valueTmp)
    valueTmp = mat_tmp1._10*const_mat_1._02 + mat_tmp1._11*const_mat_1._12 + mat_tmp1._12*const_mat_1._22 + mat_tmp1._13*const_mat_1._32;
    UTEST_EQU(mat_tmp2._12, valueTmp)
    valueTmp = mat_tmp1._20*const_mat_1._02 + mat_tmp1._21*const_mat_1._12 + mat_tmp1._22*const_mat_1._22 + mat_tmp1._23*const_mat_1._32;
    UTEST_EQU(mat_tmp2._22, valueTmp)
    valueTmp = mat_tmp1._30*const_mat_1._02 + mat_tmp1._31*const_mat_1._12 + mat_tmp1._32*const_mat_1._22 + mat_tmp1._33*const_mat_1._32;
    UTEST_EQU(mat_tmp2._32, valueTmp)
    valueTmp = mat_tmp1._00*const_mat_1._03 + mat_tmp1._01*const_mat_1._13 + mat_tmp1._02*const_mat_1._23 + mat_tmp1._03*const_mat_1._33;
    UTEST_EQU(mat_tmp2._03, valueTmp)
    valueTmp = mat_tmp1._10*const_mat_1._03 + mat_tmp1._11*const_mat_1._13 + mat_tmp1._12*const_mat_1._23 + mat_tmp1._13*const_mat_1._33;
    UTEST_EQU(mat_tmp2._13, valueTmp)
    valueTmp = mat_tmp1._20*const_mat_1._03 + mat_tmp1._21*const_mat_1._13 + mat_tmp1._22*const_mat_1._23 + mat_tmp1._23*const_mat_1._33;
    UTEST_EQU(mat_tmp2._23, valueTmp)
    valueTmp = mat_tmp1._30*const_mat_1._03 + mat_tmp1._31*const_mat_1._13 + mat_tmp1._32*const_mat_1._23 + mat_tmp1._33*const_mat_1._33;
    UTEST_EQU(mat_tmp2._33, valueTmp)
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp1 *= const_mat_1;
    UTEST_EQU(mat_tmp1.isEquivalent(mat_tmp2, f_COMMON_EPSILON), true);

    //pre-post multiply
    mat_tmp1 = const_mat_1.getTransposed();
    mat_tmp2 = mat_tmp1 * const_mat_1;
    Matrix4f mat_tmp_3 = const_mat_1;
    mat_tmp_3.preMultiply(mat_tmp1);
    UTEST_EQU(mat_tmp2.isEquivalent(mat_tmp_3, f_COMMON_EPSILON), true);
    mat_tmp2 = const_mat_1 * mat_tmp1;
    mat_tmp_3 = const_mat_1;
    mat_tmp_3.postMultiply(mat_tmp1);
    UTEST_EQU(mat_tmp2.isEquivalent(mat_tmp_3, f_COMMON_EPSILON), true);

    //translation
    Vector3f translate(M03, M13, M23);
    mat_tmp1.setZero();
    mat_tmp1.setTranslation(translate);
    Vector3f vec_tmp1 = mat_tmp1.getTranslation();
    UTEST_EQU(vec_tmp1.isEquivalent(translate, f_COMMON_EPSILON), true);
    mat_tmp1.addTranslation(translate);
    vec_tmp1 = mat_tmp1.getTranslation();
    Vector3f vec_tmp2 = 2*translate;
    UTEST_EQU(vec_tmp1.isEquivalent(vec_tmp2, f_COMMON_EPSILON), true);

    //scale
    Vector3f scale (L_abs(M03), L_abs(M13), L_abs(M23));
    mat_tmp1.setZero();
    mat_tmp1.setScale(scale);
    vec_tmp1 = mat_tmp1.getScale();
    UTEST_EQU(vec_tmp1.isEquivalent(scale, f_COMMON_EPSILON), true);

    //projection
    //TODO

    //decompose
    //TODO

    //rotation
    //TODO

    //make
    //TODO

    //isValid
    //TODO

    //transform
    const Vector3f translation(10, -20, 30);
    Quaternionf rot(-0.3f, 0.2f, 0.8f, 1.0f);
    scale = Vector3f (5, 4, 3);
    Quaternionf so(0.4f, 0.3f, -0.2f, 1.0f);
    Vector3f translation2;
    Quaternionf rot2;
    Vector3f scale2;
    Quaternionf so2;

    rot.normalizeSafe();
    so.normalizeSafe();
    Transformf te(translation, rot, scale, so);
    mat_tmp1.setTransform(te._translation, te._rotation, te._scale, te._scaleOrientation);
    mat_tmp1.getTransformSafe(translation2, rot2, scale2, so2);

    UTEST_EQU(te._translation.isEquivalent(translation2, f_COMMON_EPSILON), true);
    UTEST_EQU(te._rotation.isEquivalent(rot2, f_COMMON_EPSILON), true);
    UTEST_EQU(te._scale.isEquivalent(scale2, f_COMMON_EPSILON), true);
    UTEST_EQU(te._scaleOrientation.isEquivalent(so2, f_COMMON_EPSILON), true);

    Vector3f vec_tmp3 (M03, M13, M22);
    vec_tmp1 = mat_tmp1.apply(vec_tmp3);
    vec_tmp2 = te.apply(vec_tmp3);

    UTEST_EQU(vec_tmp1.isEquivalent(vec_tmp2, f_COMMON_EPSILON), true);
    /*

     //inverse
     valueTmp = const_mat_1.determinant();
     UTEST_EQUE(valueTmp, det_1, f_COMMON_EPSILON);
     const Matrix4f inv_1 ((M11*M22-M12*M21)/det_1, (M02*M21-M01*M22)/det_1, (M01*M12-M02*M11)/det_1,
     (M12*M20-M10*M22)/det_1, (M00*M22-M02*M20)/det_1, (M02*M10-M00*M12)/det_1,
     (M10*M21-M11*M20)/det_1, (M01*M20-M00*M21)/det_1, (M00*M11-M01*M10)/det_1);
     mat_tmp1 = const_mat_1.getInvertedSafe();
     UTEST_EQU(mat_tmp1.isEquivalent(inv_1, 0.000000001), true);
     mat_tmp1 = const_mat_1;
     mat_tmp1.inverseSafeSelf();
     UTEST_EQU(mat_tmp1.isEquivalent(inv_1, 0.000000001), true);*/
}

static void testOrientation(const float pitch1, const float yaw1, const float roll1)
{
    Core::Vector3f vec_1(pitch1, yaw1, roll1);
    const float value = 45.12357f;
    const float rot_X = pitch1;
    const float rot_Y = yaw1;
    const float rot_Z = roll1;

    Core::Matrix3f matRotX(1, 0, 0, 0, cos(rot_X), -sin(rot_X), 0, sin(rot_X), cos(rot_X));
    Core::Matrix3f matRotY(cos(rot_Y), 0, sin(rot_Y), 0, 1, 0, -sin(rot_Y), 0, cos(rot_Y));
    Core::Matrix3f matRotZ(cos(rot_Z), -sin(rot_Z), 0, sin(rot_Z), cos(rot_Z), 0, 0, 0, 1);
    Core::Matrix3f matRot =matRotZ * matRotY * matRotX;

    Quaternionf quatRotX(cos(pitch1/2), Vector3f(sin(pitch1/2), 0, 0));
    Quaternionf quatRotY(cos(yaw1/2), Vector3f(0, sin(yaw1/2), 0));
    Quaternionf quatRotZ(cos(roll1/2), Vector3f(0, 0, sin(roll1/2)));
    Quaternionf quatRot = quatRotZ *quatRotY * quatRotX;

    Orientationf ang_min(pitch1+0.2f, yaw1+0.3f, roll1-0.2f);
    Orientationf ang_max(pitch1+0.4f, yaw1+0.6f, roll1+0.2f);

    //constructeur
    Orientationf ang_1(pitch1, yaw1, roll1);
    UTEST_EQU(ang_1._pitch, pitch1);
    UTEST_EQU(ang_1._yaw, yaw1);
    UTEST_EQU(ang_1._roll, roll1);
    Orientationf const_ang_1 (ang_1);
    UTEST_EQU(const_ang_1._pitch, pitch1);
    UTEST_EQU(const_ang_1._yaw, yaw1);
    UTEST_EQU(const_ang_1._roll, roll1);
    Orientationf ang_tmp_1 (ZERO);
    UTEST_EQU(ang_tmp_1._pitch, 0);
    UTEST_EQU(ang_tmp_1._yaw, 0);
    UTEST_EQU(ang_tmp_1._roll, 0);
    Orientationf ang_tmp_2 (vec_1);
    UTEST_EQU(ang_tmp_2._pitch, pitch1);
    UTEST_EQU(ang_tmp_2._yaw, yaw1);
    UTEST_EQU(ang_tmp_2._roll, roll1);
    Orientationf ang_tmp_3 (matRot);
    UTEST_EQU(ang_tmp_3._pitch, pitch1);
    UTEST_EQU(ang_tmp_3._yaw, yaw1);
    UTEST_EQU(ang_tmp_3._roll, roll1);
    Orientationf ang_tmp_4(quatRotX);
    UTEST_EQU(ang_tmp_4._pitch, pitch1);
    Orientationf ang_tmp_5(quatRotY);
    UTEST_EQU(ang_tmp_5._yaw, yaw1);
    Orientationf ang_tmp_6(quatRotZ);
    UTEST_EQU(ang_tmp_6._roll, roll1);
    Orientationf ang_tmp_7(quatRot);
    UTEST_EQUE(ang_tmp_7._pitch, pitch1, f_COMMON_EPSILON);
    UTEST_EQUE(ang_tmp_7._yaw, yaw1, f_COMMON_EPSILON);
    UTEST_EQUE(ang_tmp_7._roll, roll1, f_COMMON_EPSILON);

    Orientationf ang_2(yaw1, roll1, pitch1);

    //comparaison
    UTEST_EQU(const_ang_1==ang_1, true)
    UTEST_EQU(ang_tmp_1==ang_1, false)
    UTEST_EQU(const_ang_1.isEquivalent(ang_1, f_COMMON_EPSILON), true)
    ang_1._pitch += value;
    UTEST_EQU(const_ang_1.isEquivalent(ang_1, f_COMMON_EPSILON), false)
    ang_1._pitch -= value;

    //accesseur
    UTEST_EQU(ang_1.data(), &ang_1._pitch)
    UTEST_EQU(const_ang_1.data(), &const_ang_1._pitch)
    UTEST_EQUE(const_ang_1[0], pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(const_ang_1[1], yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(const_ang_1[2], roll1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_1[0], pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_1[1], yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_1[2], roll1, f_COMMON_EPSILON)
    ang_tmp_1.set(10, 15, 20);
    UTEST_EQUE(ang_tmp_1._pitch, 10, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, 15, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, 20, f_COMMON_EPSILON)
    ang_tmp_1.setZero();
    UTEST_EQUE(ang_tmp_1._pitch, 0, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, 0, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, 0, f_COMMON_EPSILON)

    //operateur
    //Orientation _ Scalar
    ang_tmp_1 = const_ang_1 * value;
    UTEST_EQUE(ang_tmp_1._pitch, pitch1*value, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1*value, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1*value, f_COMMON_EPSILON)
    ang_tmp_2 = value * const_ang_1;
    UTEST_EQU(ang_tmp_1.isEquivalent(ang_tmp_2, f_COMMON_EPSILON), true)
    ang_tmp_2 = const_ang_1;
    ang_tmp_2 *= value;
    UTEST_EQU(ang_tmp_1.isEquivalent(ang_tmp_2, f_COMMON_EPSILON), true)
    ang_tmp_1 = const_ang_1 / value;
    UTEST_EQUE(ang_tmp_1._pitch, pitch1/value, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1/value, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1/value, f_COMMON_EPSILON)
    ang_tmp_2 = const_ang_1;
    ang_tmp_2 /= value;
    UTEST_EQU(ang_tmp_1.isEquivalent(ang_tmp_2, f_COMMON_EPSILON), true)
    ang_tmp_1 = -const_ang_1;
    UTEST_EQU(ang_tmp_1._pitch, -1*pitch1)
    UTEST_EQU(ang_tmp_1._yaw, -1*yaw1)
    UTEST_EQU(ang_tmp_1._roll, -1*roll1)

    //Orientation _ Orientation
    ang_tmp_1 = const_ang_1 + ang_2;
    UTEST_EQUE(ang_tmp_1._pitch, const_ang_1._pitch + ang_2._pitch, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, const_ang_1._yaw + ang_2._yaw, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, const_ang_1._roll + ang_2._roll, f_COMMON_EPSILON)
    ang_tmp_2 = const_ang_1;
    ang_tmp_2 += ang_2;
    UTEST_EQU(ang_tmp_1.isEquivalent(ang_tmp_2, f_COMMON_EPSILON), true)

    ang_tmp_1 = const_ang_1 - ang_2;
    UTEST_EQUE(ang_tmp_1._pitch, const_ang_1._pitch - ang_2._pitch, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, const_ang_1._yaw - ang_2._yaw, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, const_ang_1._roll - ang_2._roll, f_COMMON_EPSILON)
    ang_tmp_2 = const_ang_1;
    ang_tmp_2 -= ang_2;
    UTEST_EQU(ang_tmp_1.isEquivalent(ang_tmp_2, f_COMMON_EPSILON), true)

    //clamp
    ang_tmp_1 = const_ang_1;
    ang_tmp_1.clamp(ang_min, ang_max);
    UTEST_EQU(ang_tmp_1._pitch>=ang_min._pitch, true)
    UTEST_EQU(ang_tmp_1._yaw>=ang_min._yaw, true)
    UTEST_EQU(ang_tmp_1._roll>=ang_min._roll, true)
    UTEST_EQU(ang_tmp_1._pitch<=ang_max._pitch, true)
    UTEST_EQU(ang_tmp_1._yaw<=ang_max._yaw, true)
    UTEST_EQU(ang_tmp_1._roll<=ang_max._roll, true)

    //normalizePI
    ang_tmp_1 = const_ang_1 * 180;
    ang_tmp_1.normalizePISafe();
    UTEST_EQU(ang_tmp_1._pitch<=f_PI, true)
    UTEST_EQU(ang_tmp_1._pitch>=-f_PI, true)
    UTEST_EQU(ang_tmp_1._yaw<=f_PI, true)
    UTEST_EQU(ang_tmp_1._yaw>=-f_PI, true)
    UTEST_EQU(ang_tmp_1._roll<=f_PI, true)
    UTEST_EQU(ang_tmp_1._roll>=-f_PI, true)

    //setOrientation
    ang_tmp_1.setOrientations(matRot);
    UTEST_EQUE(ang_tmp_1._pitch, pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1, f_COMMON_EPSILON)
    ang_tmp_1.setOrientations(quatRot);
    UTEST_EQUE(ang_tmp_1._pitch, pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1, f_COMMON_EPSILON)

    //make
    ang_tmp_1 = Orientationf::makeOrientations(quatRot);
    UTEST_EQUE(ang_tmp_1._pitch, pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1, f_COMMON_EPSILON)
    ang_tmp_1 = Orientationf::makeOrientations(matRot);
    UTEST_EQUE(ang_tmp_1._pitch, pitch1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._yaw, yaw1, f_COMMON_EPSILON)
    UTEST_EQUE(ang_tmp_1._roll, roll1, f_COMMON_EPSILON)
}

static void testQuaternion(const float X1, const float Y1, const float Z1, const float W1)
{
    Core::Vector3f axis(X1, Y1, Z1);
    const float value = 45.12357f;

    const float length_squared_1 = X1*X1 + Y1*Y1 + Z1*Z1 + W1*W1;
    const float length_1 = L_sqrt(length_squared_1);
    Quaternionf const_norm_1(X1/length_1, Y1/length_1, Z1/length_1, W1/length_1);

    //constructeur
    Quaternionf quat_1(X1, Y1, Z1, W1);
    UTEST_EQU(quat_1._v._x, X1);
    UTEST_EQU(quat_1._v._y, Y1);
    UTEST_EQU(quat_1._v._z, Z1);
    UTEST_EQU(quat_1._w, W1);

    Orientationf ang_1 = Orientationf::makeOrientations(const_norm_1);

    const Quaternionf const_quat_1(quat_1);
    UTEST_EQU(const_quat_1._v._x, X1)
    UTEST_EQU(const_quat_1._v._y, Y1)
    UTEST_EQU(const_quat_1._v._z, Z1)
    UTEST_EQU(const_quat_1._w, W1)

    Quaternionf quat_tmp_1 (ZERO);
    UTEST_EQU(quat_tmp_1._v._x, 0)
    UTEST_EQU(quat_tmp_1._v._y, 0)
    UTEST_EQU(quat_tmp_1._v._z, 0)
    UTEST_EQU(quat_tmp_1._w, 0)

    Quaternionf quat_tmp_2 (IDENTITY);
    UTEST_EQU(quat_tmp_2._v._x, 0)
    UTEST_EQU(quat_tmp_2._v._y, 0)
    UTEST_EQU(quat_tmp_2._v._z, 0)
    UTEST_EQU(quat_tmp_2._w, 1)

    Quaternionf quat_tmp_3(W1, Vector3f(X1, Y1, Z1));
    UTEST_EQU(quat_tmp_3._v._x, X1)
    UTEST_EQU(quat_tmp_3._v._y, Y1)
    UTEST_EQU(quat_tmp_3._v._z, Z1)
    UTEST_EQU(quat_tmp_3._w, W1)
    /*//TODO
     quat_tmp_1=const_quat_1;
     quat_tmp_1.normalizeSafe();
     Matrix mat33_1 = quat_tmp_1.toMatrix3();
     Quaternionf quat_tmp_4(mat33_1);
     UTEST_EQUE(quat_tmp_4._v._x, quat_tmp_1._v._x, f_COMMON_EPSILON)
     UTEST_EQUE(quat_tmp_4._v._y, quat_tmp_1._v._y, f_COMMON_EPSILON)
     UTEST_EQUE(quat_tmp_4._v._z, quat_tmp_1._v._z, f_COMMON_EPSILON)
     UTEST_EQUE(quat_tmp_4._w, quat_tmp_1._w, f_COMMON_EPSILON)
     */
    //comparaison
    quat_tmp_1 = const_quat_1;
    UTEST_EQU(quat_tmp_1==const_quat_1, true)
    UTEST_EQU(quat_tmp_1!=const_quat_1, false)
    quat_tmp_1._w += 25;
    UTEST_EQU(quat_tmp_1==const_quat_1, false)
    UTEST_EQU(quat_tmp_1!=const_quat_1, true)
    UTEST_EQU(const_quat_1.isEquivalent(quat_1, f_COMMON_EPSILON), true)
    quat_1._w += value;
    UTEST_EQU(const_quat_1.isEquivalent(quat_1, f_COMMON_EPSILON), false)
    quat_1._w -= value;
    UTEST_EQU(quat_tmp_2.isIdentity(), true);
    UTEST_EQU(quat_tmp_3.isIdentity(), false);
    UTEST_EQU(quat_tmp_2.isUnit(), true);
    UTEST_EQU(quat_tmp_3.isUnit(), false);

    //set
    quat_tmp_1.set(X1, Y1, Z1, W1);
    UTEST_EQU(quat_tmp_1._v._x, X1)
    UTEST_EQU(quat_tmp_1._v._y, Y1)
    UTEST_EQU(quat_tmp_1._v._z, Z1)
    UTEST_EQU(quat_tmp_1._w, W1)
    quat_tmp_1.set(axis, W1);
    UTEST_EQU(quat_tmp_1._v._x, X1)
    UTEST_EQU(quat_tmp_1._v._y, Y1)
    UTEST_EQU(quat_tmp_1._v._z, Z1)
    UTEST_EQU(quat_tmp_1._w, W1)
    quat_tmp_1.setIdentity();
    UTEST_EQU(quat_tmp_1._v._x, 0)
    UTEST_EQU(quat_tmp_1._v._y, 0)
    UTEST_EQU(quat_tmp_1._v._z, 0)
    UTEST_EQU(quat_tmp_1._w, 1)
    quat_tmp_1.setRotation(ang_1._pitch, ang_1._yaw, ang_1._roll);
    UTEST_EQUE(quat_tmp_1._v._x, const_norm_1._v._x, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._y, const_norm_1._v._y, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._z, const_norm_1._v._z, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._w, const_norm_1._w, f_COMMON_EPSILON)
    quat_tmp_1.setRotation(ang_1);
    UTEST_EQUE(quat_tmp_1._v._x, const_norm_1._v._x, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._y, const_norm_1._v._y, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._z, const_norm_1._v._z, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._w, const_norm_1._w, f_COMMON_EPSILON)
    quat_tmp_1.setRotationPitch (value);
    UTEST_EQUE(quat_tmp_1._v._x, L_sin(value/2), f_COMMON_EPSILON)
    quat_tmp_1.setRotationYaw (value);
    UTEST_EQUE(quat_tmp_1._v._y, L_sin(value/2), f_COMMON_EPSILON)
    quat_tmp_1.setRotationRoll (value);
    UTEST_EQUE(quat_tmp_1._v._z, L_sin(value/2), f_COMMON_EPSILON)

    /*TODO
     inline void              setSlerp         (const Quaternionf& from, Quaternionf to, const float amount);
     inline void              setRotationVec1toVec2(const Vector3f& vec_1, const Vector3f& vec_2);
     */

    //operation
    quat_tmp_1 = const_quat_1;
    quat_tmp_1.normalizeSafe();
    UTEST_EQU(quat_tmp_1.isEquivalent(const_norm_1, f_COMMON_EPSILON), true)
    quat_tmp_1 = const_quat_1;
    quat_tmp_1.normalizeFast();
    UTEST_EQU(quat_tmp_1.isEquivalent(const_norm_1, f_COMMON_EPSILON_FAST), true)
    quat_tmp_1 = const_quat_1.getNormalizedSafe();
    UTEST_EQU(quat_tmp_1.isEquivalent(const_norm_1, f_COMMON_EPSILON), true)
    quat_tmp_1 = const_quat_1.getNormalizedFast();
    UTEST_EQU(quat_tmp_1.isEquivalent(const_norm_1, f_COMMON_EPSILON_FAST), true)
    float val_tmp_1 = const_quat_1.getlength();
    UTEST_EQUE(val_tmp_1, length_1, f_COMMON_EPSILON)

    /*TODO
     inline Quaternionf        getInverted      () const;
     inline void              inverseSelf      ();

     inline Matrix3f        toMatrix3        () const;
     inline Matrix4f        toMatrix4        () const;
     */

    //operateur
    quat_tmp_1 = -const_quat_1;
    UTEST_EQU(quat_tmp_1._v._x, -X1)
    UTEST_EQU(quat_tmp_1._v._y, -Y1)
    UTEST_EQU(quat_tmp_1._v._z, -Z1)
    UTEST_EQU(quat_tmp_1._w, -W1)

    //Quaternionf _ Scalar
    quat_tmp_1 = const_quat_1 * value;
    UTEST_EQUE(quat_tmp_1._v._x, X1 * value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._y, Y1 * value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._z, Z1 * value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._w, W1 * value, f_COMMON_EPSILON)
    quat_tmp_2 = value * const_quat_1;
    UTEST_EQU(quat_tmp_1.isEquivalent(quat_tmp_2, f_COMMON_EPSILON), true)
    quat_tmp_2 = const_quat_1;
    quat_tmp_2 *= value;
    UTEST_EQU(quat_tmp_1.isEquivalent(quat_tmp_2, f_COMMON_EPSILON), true)
    quat_tmp_1 = const_quat_1 / value;
    UTEST_EQUE(quat_tmp_1._v._x, X1 / value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._y, Y1 / value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._z, Z1 / value, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._w, W1 / value, f_COMMON_EPSILON)
    quat_tmp_2 = const_quat_1;
    quat_tmp_2 /= value;
    UTEST_EQU(quat_tmp_1.isEquivalent(quat_tmp_2, f_COMMON_EPSILON), true)

    //make
    quat_tmp_1 = Quaternionf(ang_1._pitch, ang_1._yaw, ang_1._roll);
    UTEST_EQUE(quat_tmp_1._v._x, const_norm_1._v._x, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._y, const_norm_1._v._y, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._v._z, const_norm_1._v._z, f_COMMON_EPSILON)
    UTEST_EQUE(quat_tmp_1._w, const_norm_1._w, f_COMMON_EPSILON)

    quat_tmp_1 = Quaternionf(PITCH, value);
    UTEST_EQUE(quat_tmp_1._v._x, L_sin(value/2), f_COMMON_EPSILON)

    quat_tmp_1 = Quaternionf(YAW, value);
    UTEST_EQUE(quat_tmp_1._v._y, L_sin(value/2), f_COMMON_EPSILON)

    quat_tmp_1 = Quaternionf(ROLL, value);
    UTEST_EQUE(quat_tmp_1._v._z, L_sin(value/2), f_COMMON_EPSILON)

    // Rotation axis arbitraire
    Quaternionf  q1a(PITCH, 1.0), q1b(AXIS, Vector3f(1.0, 0.0, 0.0), 1.0);
    Quaternionf  q2a(YAW,   2.0), q2b(AXIS, Vector3f(0.0, 1.0, 0.0), 2.0);
    Quaternionf  q3a(ROLL,  3.0), q3b(AXIS, Vector3f(0.0, 0.0, 1.0), 3.0);
    UTEST_EQU(q1a.isEquivalent(q1b), true);
    UTEST_EQU(q2a.isEquivalent(q2b), true);
    UTEST_EQU(q3a.isEquivalent(q3b), true);
}

static void testTransform(const float T1, const float T2, const float T3, const float R1, const float R2, const float R3,
                                         const float R4, const float S1, const float S2, const float S3, const float SO1, const float SO2,
                                         const float SO3, const float SO4)
{
    const Vector3f tra_1(T1, T2, T3);
    const Quaternionf rot_1(R1, R2, R3, R4);
    const Vector3f sca_1(S1, S2, S3);
    const Quaternionf sco_1(SO1, SO2, SO3, SO4);
    const Vector3f vec_zero(ZERO);
    const Vector3f vec_one(ONE);
    const Quaternionf quat_identity(IDENTITY);
    //constructeur
    Transformf te_1(T1, T2, T3, R1, R2, R3, R4, S1, S2, S3, SO1, SO2, SO3, SO4);
    UTEST_EQU(te_1._translation==tra_1, true)
    UTEST_EQU(te_1._rotation==rot_1, true)
    UTEST_EQU(te_1._scale==sca_1, true)
    UTEST_EQU(te_1._scaleOrientation==sco_1, true)

    const Transformf const_te_1 (te_1);
    UTEST_EQU(const_te_1.isEquivalent(te_1), true)

    Transformf te_2 (tra_1, rot_1, sca_1, sco_1);
    UTEST_EQU(te_2.isEquivalent(te_1), true)

    const Transformf te_3 (IDENTITY);
    UTEST_EQU(te_3._translation.isEquivalent(vec_zero, f_COMMON_EPSILON), true)
    UTEST_EQU(te_3._rotation.isEquivalent(quat_identity, f_COMMON_EPSILON), true)
    UTEST_EQU(te_3._scale.isEquivalent(vec_one, f_COMMON_EPSILON), true)
    UTEST_EQU(te_3._scaleOrientation.isEquivalent(quat_identity, f_COMMON_EPSILON), true)

    //comparaison
    UTEST_EQU(const_te_1 == te_1, true)
    UTEST_EQU(const_te_1 == te_3, false)

    //set
    te_2.setIdentity();
    UTEST_EQU(te_2.isEquivalent(te_3), true)

    //invert
    //TODO

    //operateur
    //TODO
}

static void testTransformMatrix()
{
    // Translation ------------------------------------------------------------

    Core::Vector3f vec(1.0, 2.0, 3.0);
    Core::Matrix3f identity3x3(Core::IDENTITY);
    Core::Matrix4f matTrans0(1.0, 0.0, 0.0, 1.0,
                             0.0, 1.0, 0.0, 2.0,
                             0.0, 0.0, 1.0, 3.0,
                             0.0, 0.0, 0.0, 1.0);
    Core::Matrix4f matTrans1(identity3x3, vec);
    Core::Matrix4f matTrans2(TRANSLATION, vec);
    Core::Matrix4f matTrans3(TRANSLATION, vec._x, vec._y, vec._z);

    UTEST_EQU(matTrans0.isEquivalent(matTrans1), true);
    UTEST_EQU(matTrans1.isEquivalent(matTrans2), true);
    UTEST_EQU(matTrans1.isEquivalent(matTrans3), true);

    // Scale ------------------------------------------------------------------

    Core::Matrix4f matScale0(1.0, 0.0, 0.0, 0.0,
                             0.0, 2.0, 0.0, 0.0,
                             0.0, 0.0, 3.0, 0.0,
                             0.0, 0.0, 0.0, 1.0);
    Core::Matrix4f matScale1(Core::IDENTITY);
    matScale1.setScale(vec);
    Core::Matrix4f matScale2(SCALE, vec);
    Core::Matrix4f matScale3(SCALE, vec._x, vec._y, vec._z);

    UTEST_EQU(matScale0.isEquivalent(matScale1), true);
    UTEST_EQU(matScale1.isEquivalent(matScale2), true);
    UTEST_EQU(matScale1.isEquivalent(matScale3), true);

    // Rotation pitch ---------------------------------------------------------

    Core::Matrix4f matPitch0( 1.0,  0.0,  0.0,  0.0,
                              0.0,  0.0, -1.0,  0.0,
                              0.0,  1.0,  0.0,  0.0,
                              0.0,  0.0,  0.0,  1.0);
    Core::Matrix4f matPitch1(Core::IDENTITY);
    matPitch1.setRotationPitch(f_PI_DIV_2);
    Core::Matrix4f matPitch2(Core::ROTATION, f_PI_DIV_2, ROT_X_PITCH);
    Core::Matrix4f matPitch3(Core::ROTATION, f_PI_DIV_2, Core::Vector3f(1.0, 0.0, 0.0));
    Core::Matrix4f matPitch4(Core::ROTATION, f_PI_DIV_2, 1.0, 0.0, 0.0);

    UTEST_EQU(matPitch0.isEquivalent(matPitch1), true);
    UTEST_EQU(matPitch0.isEquivalent(matPitch2), true);
    UTEST_EQU(matPitch0.isEquivalent(matPitch3), true);
    UTEST_EQU(matPitch0.isEquivalent(matPitch4), true);

    // Rotation yaw   ---------------------------------------------------------

    Core::Matrix4f matYaw0( 0.0,  0.0,  1.0,  0.0,
                            0.0,  1.0,  0.0,  0.0,
                           -1.0,  0.0,  0.0,  0.0,
                            0.0,  0.0,  0.0,  1.0);
    Core::Matrix4f matYaw1(Core::IDENTITY);
    matYaw1.setRotationYaw(f_PI_DIV_2);
    Core::Matrix4f matYaw2(Core::ROTATION, f_PI_DIV_2, ROT_Y_YAW);
    Core::Matrix4f matYaw3(Core::ROTATION, f_PI_DIV_2, Core::Vector3f(0.0, 1.0, 0.0));
    Core::Matrix4f matYaw4(Core::ROTATION, f_PI_DIV_2, 0.0, 1.0, 0.0);

    UTEST_EQU(matYaw0.isEquivalent(matYaw1), true);
    UTEST_EQU(matYaw0.isEquivalent(matYaw2), true);
    UTEST_EQU(matYaw0.isEquivalent(matYaw3), true);
    UTEST_EQU(matYaw0.isEquivalent(matYaw4), true);

    // Rotation roll  ---------------------------------------------------------

    Core::Matrix4f matRoll0( 0.0, -1.0,  0.0,  0.0,
                             1.0,  0.0,  0.0,  0.0,
                             0.0,  0.0,  1.0,  0.0,
                             0.0,  0.0,  0.0,  1.0);
    Core::Matrix4f matRoll1(Core::IDENTITY);
    matRoll1.setRotationRoll(f_PI_DIV_2);
    Core::Matrix4f matRoll2(Core::ROTATION, f_PI_DIV_2, ROT_Z_ROLL);
    Core::Matrix4f matRoll3(Core::ROTATION, f_PI_DIV_2, Core::Vector3f(0.0, 0.0, 1.0));
    Core::Matrix4f matRoll4(Core::ROTATION, f_PI_DIV_2, 0.0, 0.0, 1.0);

    UTEST_EQU(matRoll0.isEquivalent(matRoll1), true);
    UTEST_EQU(matRoll0.isEquivalent(matRoll2), true);
    UTEST_EQU(matRoll0.isEquivalent(matRoll3), true);
    UTEST_EQU(matRoll0.isEquivalent(matRoll4), true);
}

void testMath()
{
    UTEST_PACKAGE("Math");

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"MathTool");
    testMathTool();
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Vector3f");
    testVector3(1.5f, 0.f, -456.23f);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Vector4f");
    testVector4(1.5f, 0.f, -456.23f, -78.415f);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Matrix3f");
    testMatrix3(56, -23, 45,
                5, 2983, -786,
                -76, 75, 32);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Matrix4f");
    testMatrix4(56, -23, 45, 23,
                5, 2983, -786, -78.23f,
                -76, 75, 32, 45,
                -796.23f, 23.56f, 98, -65);
    testTransformMatrix();
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Orientationf");
    testOrientation(1.38f, -0.21f, -1.056f);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Quaternionf");
    testQuaternion(0.38f, -0.21f, -0.456f, 0.42f);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Transformf")
    testTransform(0.38f, -0.21f, -0.456f,
                  0.64f, 0.56f, -0.78f, 0.18f,
                  0.48f, -0.51f, -0.176f,
                  0.14f, 0.46f, -0.588f, -0.48f);
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"xxxPowerOfTwo functions")

    UTEST_EQU(nextPowerOfTwo(0), 0);
    UTEST_EQU(nextPowerOfTwo(1), 1);
    UTEST_EQU(nextPowerOfTwo(2), 2);
    UTEST_EQU(nextPowerOfTwo(3), 4);
    UTEST_EQU(nextPowerOfTwo(4), 4);
    UTEST_EQU(nextPowerOfTwo(5), 8);
    UTEST_EQU(nextPowerOfTwo(7), 8);
    UTEST_EQU(nextPowerOfTwo(8), 8);
    UTEST_EQU(nextPowerOfTwo(4000), 4096);

    UTEST_EQU(previousPowerOfTwo(0), 0);
    UTEST_EQU(previousPowerOfTwo(1), 1);
    UTEST_EQU(previousPowerOfTwo(2), 2);
    UTEST_EQU(previousPowerOfTwo(3), 2);
    UTEST_EQU(previousPowerOfTwo(4), 4);
    UTEST_EQU(previousPowerOfTwo(5), 4);
    UTEST_EQU(previousPowerOfTwo(7), 4);
    UTEST_EQU(previousPowerOfTwo(8), 8);
    UTEST_EQU(previousPowerOfTwo(4000), 2048);

    UTEST_EQU(nearestPowerOfTwo(0), 0);
    UTEST_EQU(nearestPowerOfTwo(1), 1);
    UTEST_EQU(nearestPowerOfTwo(2), 2);
    UTEST_EQU(nearestPowerOfTwo(3), 4);
    UTEST_EQU(nearestPowerOfTwo(4), 4);
    UTEST_EQU(nearestPowerOfTwo(5), 4);
    UTEST_EQU(nearestPowerOfTwo(7), 8);
    UTEST_EQU(nearestPowerOfTwo(8), 8);
    UTEST_EQU(nearestPowerOfTwo(4000), 4096);

    UTEST_EQU(isPowerOfTwo(0), true);
    UTEST_EQU(isPowerOfTwo(1), true);
    UTEST_EQU(isPowerOfTwo(2), true);
    UTEST_EQU(isPowerOfTwo(3), false);
    UTEST_EQU(isPowerOfTwo(4), true);
    UTEST_EQU(isPowerOfTwo(5), false);
    UTEST_EQU(isPowerOfTwo(6), false);
    UTEST_EQU(isPowerOfTwo(7), false);
    UTEST_EQU(isPowerOfTwo(8), true);
    UTEST_EQU(isPowerOfTwo(4000), false);

    UTEST_END_TEST;

    

    testMathGeometry();
    testKDTree();
}
