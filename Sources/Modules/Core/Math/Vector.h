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
#ifndef CORE_VECTOR_H_
#define CORE_VECTOR_H_

#include <Core/Math/MathTools.h>
#include <Core/Types.h>
#include <Core/String.h>

#ifdef _DEBUG
#define VECINLINE
#define VECFUNCINLINE LM_API_COR
#else
#define VECINLINE inline
#define VECFUNCINLINE inline
#endif

//#define LM_MATH_SHOW_CONSTRUCTOR
//#include <iostream>

namespace Core
{
struct LM_API_COR Vector2f
{
    Vector2f () {}
    Vector2f (const float X, const float Y) : x(X), y(Y) {}

    VECINLINE explicit Vector2f(const Vector2f & v1, const Vector2f & v2, float a);

    VECINLINE bool isEquivalent(const Vector2f & v, float epsilon = f_COMMON_EPSILON) const;

    VECINLINE float getLengthFast() const;
    VECINLINE float getLengthSafe() const;
    VECINLINE float getLengthSquared() const;

    VECINLINE void setLerp(const Vector2f & from, const Vector2f & to, float a);

    VECINLINE float   operator[](size_t index) const;
    VECINLINE float & operator[](size_t index);

    VECINLINE bool operator == (const Vector2f & v) const;
    VECINLINE bool operator != (const Vector2f & v) const;

    VECINLINE Vector2f operator + (const Vector2f & v) const;
    VECINLINE Vector2f operator - (const Vector2f & v) const;
    VECINLINE Vector2f operator * (float s) const;

    union
    {
        struct { float x, y; };
    };

    const static Vector2f ONE;
    const static Vector2f ZERO;
};

VECFUNCINLINE Vector2f operator * (float s, const Vector2f & v);

struct LM_API_COR Vector3f
{
    VECINLINE          Vector3f();
    VECINLINE          Vector3f(const Vector3f&);
    VECINLINE explicit Vector3f(Zero_t);
    VECINLINE explicit Vector3f(One_t);
    VECINLINE explicit Vector3f(NAN_t);
    VECINLINE explicit Vector3f(const float, const float, const float);
    VECINLINE explicit Vector3f(const float);
    VECINLINE explicit Vector3f(const float * pValues);
    VECINLINE explicit Vector3f(const Vector3f & v1, const Vector3f & v2, float a);

    VECINLINE float  operator[](const size_t&) const;
    VECINLINE float& operator[](const size_t&);

    VECINLINE       float* data();
    VECINLINE const float* data() const;

    VECINLINE void   set(const float, const float, const float);
    VECINLINE void   setZero();
    VECINLINE void   setOne();

    VECINLINE Vector3f  operator* (const float) const;
    VECINLINE Vector3f  operator/ (const float) const;

    VECINLINE Vector3f& operator*=(const float);
    VECINLINE Vector3f& operator/=(const float);

    VECINLINE Vector3f  operator- () const;
    VECINLINE Vector3f  operator+ (const Vector3f& other) const;
    VECINLINE Vector3f  operator- (const Vector3f& other) const;

    VECINLINE Vector3f& operator+=(const Vector3f& other);
    VECINLINE Vector3f& operator-=(const Vector3f& other);

    VECINLINE bool operator==(const Vector3f&) const;
    VECINLINE bool operator!=(const Vector3f&) const;

    VECINLINE bool isEquivalent(const Vector3f&, const float epsilon=f_COMMON_EPSILON) const;
    VECINLINE bool isZero(float epsilon=f_COMMON_EPSILON) const;
    VECINLINE bool isUnit(float epsilon=f_COMMON_EPSILON) const;
    VECINLINE bool isValid() const;

    VECINLINE float getLengthFast() const;
    VECINLINE float getLengthSafe() const;
    VECINLINE float getLengthSquared() const;
    VECINLINE void  setLength(float newLength);

    VECINLINE void     normalizeFast();
    VECINLINE void     normalizeSafe();
    VECINLINE Vector3f getNormalizedFast() const;
    VECINLINE Vector3f getNormalizedSafe() const;

    VECINLINE float getDistance(const Vector3f& other) const;
    VECINLINE float getDistanceSquared(const Vector3f& other) const;

    VECINLINE void setLerp(const Vector3f& from, const Vector3f& to, const float amount);
    VECINLINE void clamp(const Vector3f&, const Vector3f&);
    VECINLINE void snap();

    union
    {
        struct { float _x, _y, _z; };
        struct { float x, y, z; };
    };


    const static Vector3f ONE;
    const static Vector3f ZERO;
};

VECFUNCINLINE Vector3f operator*(const float a, const Vector3f& vec);

struct LM_API_COR Vector4f
{
    VECINLINE          Vector4f();
    VECINLINE          Vector4f(const Vector4f&);
    VECINLINE explicit Vector4f(Zero_t);
    VECINLINE explicit Vector4f(One_t);
    VECINLINE explicit Vector4f(const float, const float, const float, const float);
    VECINLINE explicit Vector4f(const float * pValues);
    VECINLINE explicit Vector4f(const Vector3f&);
    VECINLINE explicit Vector4f(const Vector3f&, float w);
    VECINLINE explicit Vector4f(const Vector4f& v1, const Vector4f& v2, float alpha);
    VECINLINE explicit Vector4f(const float);

    VECINLINE float  operator[](const size_t&) const;
    VECINLINE float& operator[](const size_t&);

    VECINLINE float*       data();
    VECINLINE const float* data() const;

    VECINLINE void set(const float, const float, const float, const float);
    VECINLINE void setZero();

    VECINLINE bool isEquivalent(const Vector4f&, const float epsilon = f_COMMON_EPSILON) const;
    VECINLINE bool isZero(const float epsilon = f_COMMON_EPSILON) const;
    VECINLINE bool isUnit(float epsilon = f_COMMON_EPSILON) const;

    VECINLINE Vector4f  operator* (const float) const;
    VECINLINE Vector4f  operator/ (const float) const;

    VECINLINE Vector4f& operator*=(const float);
    VECINLINE Vector4f& operator/=(const float);

    VECINLINE Vector4f  operator- () const;
    VECINLINE Vector4f  operator+ (const Vector4f& other) const;
    VECINLINE Vector4f  operator- (const Vector4f& other) const;

    VECINLINE Vector4f& operator+=(const Vector4f& other);
    VECINLINE Vector4f& operator-=(const Vector4f& other);

    VECINLINE bool operator==(const Vector4f&) const;
    VECINLINE bool operator!=(const Vector4f&) const;

    VECINLINE float getLengthFast() const;
    VECINLINE float getLengthSafe() const;
    VECINLINE float getLengthSquared() const;
    VECINLINE void  setLength(float newLength);

    VECINLINE void     normalizeFast();
    VECINLINE void     normalizeSafe();
    VECINLINE Vector4f getNormalizedFast() const;
    VECINLINE Vector4f getNormalizedSafe() const;

    VECINLINE Vector3f homogeneousDiv() const;
    VECINLINE Vector3f getXYZ() const;

    VECINLINE float getDistance(const Vector4f& other) const;
    VECINLINE float getDistanceSquared(const Vector4f& other) const;

    VECINLINE void setLerp(const Vector4f& from, const Vector4f& to, const float amount);
    VECINLINE void clamp(const Vector4f&, const Vector4f&);

    union
    {
        struct { float _x, _y, _z, _w; };
        struct { float x, y, z, w; };
        struct { float r, g, b, a; };
    };

    const static Vector4f ONE;
    const static Vector4f ZERO;
};

VECFUNCINLINE Vector3f L_abs(const Vector3f& vec);
VECFUNCINLINE Vector4f operator*(const float, const Vector4f&);

//-----------------------------------------------------------------------------
LM_API_COR void genTangentBasis(const Core::Vector3f & n, Core::Vector3f & t1, Core::Vector3f & t2);

//-----------------------------------------------------------------------------
LM_API_COR String toString(const Vector2f& vec);
LM_API_COR String toString(const Vector2f& vec, int32 precision);
LM_API_COR String toString(const Vector3f& vec);
LM_API_COR String toString(const Vector3f& vec, int32 precision);
LM_API_COR String toString(const Vector4f& vec);
LM_API_COR String toString(const Vector4f& vec, int32 precision);

//-----------------------------------------------------------------------------
LM_API_COR void randomize(Vector3f& vec);
LM_API_COR void randomize(Vector4f& vec);

LM_API_COR void randomize(Vector3f& vec, const float min, const float max);
LM_API_COR void randomize(Vector4f& vec, const float min, const float max);

//----------------------------------------------------------
const Vector4f L_RED (1.f, 0.f, 0.f, 1.f);
const Vector4f L_GREEN (0.f, 1.f, 0.f, 1.f);
} //namespace Core

#include <Core/Math/Operation.h>

#ifndef _DEBUG
#include <Core/Math/Vector2.inl>
#include <Core/Math/Vector3.inl>
#include <Core/Math/Vector4.inl>
#endif

#endif /*VECTOR_H_*/
