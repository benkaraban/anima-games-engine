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
#ifndef CORE_QUATERNION_H_
#define CORE_QUATERNION_H_

#include <Core/Math/MathTools.h>
#include <Core/Math/Orientation.h>
#include <Core/Math/DecomposeMatrix.h>

#ifdef _DEBUG
#define QUATINLINE
#define QUATFUNCINLINE LM_API_COR
#else
#define QUATINLINE inline
#define QUATFUNCINLINE inline
#endif

namespace Core
{
struct Matrix3f;
struct Matrix4f;
struct Orientationf;

struct LM_API_COR Quaternionf
{
    QUATINLINE          Quaternionf();
    QUATINLINE          Quaternionf(const Quaternionf& other);
    QUATINLINE explicit Quaternionf(Identity_t);
    QUATINLINE explicit Quaternionf(Zero_t);

    QUATINLINE explicit Quaternionf(const float x, const float y, const float z, const float w);
    QUATINLINE explicit Quaternionf(const float w, const Vector3f& v);

    QUATINLINE explicit Quaternionf(const float pitch, const float yaw, const float roll);
    QUATINLINE explicit Quaternionf(Pitch_t, float angle);
    QUATINLINE explicit Quaternionf(Yaw_t, float angle);
    QUATINLINE explicit Quaternionf(Roll_t, float angle);
    QUATINLINE explicit Quaternionf(Axis_t, const Vector3f& axis, float angle);
    QUATINLINE explicit Quaternionf(const Orientationf& Orientation);

    QUATINLINE explicit Quaternionf(const Matrix3f& mat);
    QUATINLINE explicit Quaternionf(const Quaternionf& from, const Quaternionf& to, const float amount);

    QUATINLINE void set(const float x, const float y, const float z, const float w);
    QUATINLINE void set(const Vector3f& axis, const float Orientation);
    QUATINLINE void setIdentity();

    QUATINLINE void setSlerp(const Quaternionf& from, const Quaternionf& to, const float amount);
    QUATINLINE void setSlerpNoInvert(const Quaternionf& from, const Quaternionf& to, const float amount);

    QUATINLINE void setNlerp(const Quaternionf& from, Quaternionf to, const float amount);
    QUATINLINE void setSquad(const Quaternionf& q1, const Quaternionf& q2, 
                             const Quaternionf& q3, const Quaternionf& q4, 
                             float amount);
    QUATINLINE void setNquad(const Quaternionf& q1, const Quaternionf& q2, 
                             const Quaternionf& q3, const Quaternionf& q4, 
                             float amount);

    QUATINLINE void setRotationPitch(const float rad);
    QUATINLINE void setRotationYaw(const float rad);
    QUATINLINE void setRotationRoll(const float rad);
    QUATINLINE void setRotation(const float pitch, const float yaw, const float roll);
    QUATINLINE void setRotation(const Vector3f& axis, float angle);
    QUATINLINE void setRotation(const Orientationf& Orientation);

    QUATINLINE bool isEquivalent(const Quaternionf& other, const float epsilon = f_COMMON_EPSILON) const;
    QUATINLINE bool isIdentity(const float epsilon = f_COMMON_EPSILON) const;
    QUATINLINE bool isUnit(const float epsilon = f_COMMON_EPSILON) const;
    QUATINLINE bool isZero(const float epsilon = f_COMMON_EPSILON) const;

    QUATINLINE Quaternionf  operator*(const float a) const;
    QUATINLINE Quaternionf  operator/(const float a) const;

    QUATINLINE Quaternionf& operator*=(const float a);
    QUATINLINE Quaternionf& operator/=(const float a);

    QUATINLINE Quaternionf  operator- () const;
    QUATINLINE Quaternionf  operator+ (const Quaternionf& other) const;
    QUATINLINE Quaternionf  operator- (const Quaternionf& other) const;
    QUATINLINE Quaternionf  operator* (const Quaternionf& other) const;
    QUATINLINE Quaternionf  operator/ (const Quaternionf& other) const;
    QUATINLINE bool         operator==(const Quaternionf& other) const;
    QUATINLINE bool         operator!=(const Quaternionf& other) const;

    QUATINLINE Quaternionf getInverted() const;
    QUATINLINE void inverseSelf();
    QUATINLINE float getlength() const;
    QUATINLINE void normalizeSafe();
    QUATINLINE void normalizeFast();
    QUATINLINE Quaternionf getNormalizedSafe() const;
    QUATINLINE Quaternionf getNormalizedFast() const;

    QUATINLINE void setLog(const Quaternionf& q);
    QUATINLINE void setExp(const Quaternionf& q);

    QUATINLINE Quaternionf getLog() const;
    QUATINLINE Quaternionf getExp() const;

    QUATINLINE Vector3f apply(const Vector3f& vector) const;

    void setTangent(const Quaternionf& prev, const Quaternionf& cur, const Quaternionf& next);

    Matrix3f toMatrix3() const;
    Matrix4f toMatrix4() const;

    Vector3f _v;
    float _w;
};

//Quaternionf-Quaternionf
QUATFUNCINLINE Quaternionf operator*(const float a, const Quaternionf& quat);

//Quaternionf-Vector3f
QUATFUNCINLINE Vector3f operator*(const Quaternionf& quat, const Vector3f& vector);
QUATFUNCINLINE Vector3f operator*(const Vector3f& vector, const Quaternionf& quat);
QUATFUNCINLINE Vector3f& operator*=(Vector3f& vector, const Quaternionf& quat);

QUATFUNCINLINE float dot(const Quaternionf& quat1, const Quaternionf& quat2);

LM_API_COR Quaternionf matrixToQuaternion(const Matrix3f& mat);

//-----------------------------------------------------------------------------
//toString
LM_API_COR String toString(const Quaternionf& quat);
LM_API_COR String toString(const Quaternionf& quat, int32 precision);

//Quaternionf MatrixToQuaternion(const Matrix3f& m);

} //namespace Core

#ifndef _DEBUG
#include <Core/Math/Quaternion.inl>
#endif


#endif


