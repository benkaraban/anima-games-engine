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
#include <Core/Math/Matrix.h>
#include <Core/Math/Random.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/DecomposeMatrix.h>
#include <Core/Math/Transform.h>

#ifdef _DEBUG
#include <Core/Math/Matrix3.inl>
#include <Core/Math/Matrix4.inl>
#endif


namespace Core
{
const Matrix4f Matrix4f::IDENTITY_MATRIX(1.0, 0.0, 0.0, 0.0,
                            0.0, 1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0, 0.0,
                            0.0, 0.0, 0.0, 1.0);
//-----------------------------------------------------------------------------
bool Matrix3f::isIdentity(const float epsilon/* = MATRIX_EPSILON*/) const
{
    return (L_abs(_00 - 1)<=epsilon && L_abs(_01 - 0)<=epsilon && L_abs(_02 - 0)<=epsilon &&
            L_abs(_10 - 0)<=epsilon && L_abs(_11 - 1)<=epsilon && L_abs(_12 - 0)<=epsilon &&
            L_abs(_20 - 0)<=epsilon && L_abs(_21 - 0)<=epsilon && L_abs(_22 - 1)<=epsilon);
}
//-----------------------------------------------------------------------------
void Matrix3f::setZVec(const Vector3f & zVec, const Vector3f & up1, const Vector3f & up2)
{
    static const float EPSILON = 0.1f;

    Vector3f upVector(up1);

    if(Core::isEquivalent(L_abs(dot(zVec, upVector)), 1.0f, EPSILON))
        upVector = up2;

    if(Core::isEquivalent(L_abs(dot(zVec, upVector)), 1.0f, EPSILON))
        upVector = Core::Vector3f(1.0, 0.0, 0.0);

    if(Core::isEquivalent(L_abs(dot(zVec, upVector)), 1.0f, EPSILON))
        upVector = Core::Vector3f(0.0, 1.0, 0.0);

    Vector3f xVec(Core::cross(upVector, zVec).getNormalizedSafe());
    Vector3f yVec(Core::cross(zVec, xVec).getNormalizedSafe());
    _00 = xVec.x; _01 = yVec.x; _02 = zVec.x;
    _10 = xVec.y; _11 = yVec.y; _12 = zVec.y;
    _20 = xVec.z; _21 = yVec.z; _22 = zVec.z;
}
//-----------------------------------------------------------------------------
bool Matrix3f::inverseSafeSelf()
{
    Matrix3f inverse;
    float det, invDet;

    inverse[0][0] = _11 * _22 - _12 * _21;
    inverse[1][0] = _12 * _20 - _10 * _22;
    inverse[2][0] = _10 * _21 - _11 * _20;

    det = _00 * inverse[0][0] + _01 * inverse[1][0] + _02 * inverse[2][0];

    if (L_abs(det) < LM_MATRIX_INVERSE_EPSILON)
    {
        return false;
    }

    invDet = 1 / det;

    inverse[0][1] = _02 * _21 - _01 * _22;
    inverse[0][2] = _01 * _12 - _02 * _11;
    inverse[1][1] = _00 * _22 - _02 * _20;
    inverse[1][2] = _02 * _10 - _00 * _12;
    inverse[2][1] = _01 * _20 - _00 * _21;
    inverse[2][2] = _00 * _11 - _01 * _10;

    _00 = inverse[0][0] * invDet;
    _01 = inverse[0][1] * invDet;
    _02 = inverse[0][2] * invDet;

    _10 = inverse[1][0] * invDet;
    _11 = inverse[1][1] * invDet;
    _12 = inverse[1][2] * invDet;

    _20 = inverse[2][0] * invDet;
    _21 = inverse[2][1] * invDet;
    _22 = inverse[2][2] * invDet;

    return true;
}
//-----------------------------------------------------------------------------
void Matrix3f::setRotation(const Quaternionf& quat)
{
    Vector3f v2=quat._v+quat._v;
    double xx=1-v2._x*quat._v._x;
    double yy=v2._y*quat._v._y;
    double xw=v2._x*quat._w;
    double xy=v2._y*quat._v._x;
    double yz=v2._z*quat._v._y;
    double yw=v2._y*quat._w;
    double xz=v2._z*quat._v._x;
    double zz=v2._z*quat._v._z;
    double zw=v2._z*quat._w;

    _00 = float(1-yy-zz);
    _01 = float(xy-zw);
    _02 = float(xz+yw);

    _10 = float(xy+zw);
    _11 = float(xx-zz);
    _12 = float(yz-xw);

    _20 = float(xz-yw);
    _21 = float(yz+xw);
    _22 = float(xx-yy);
}
//-----------------------------------------------------------------------------
void randomize(Matrix3f& mat)
{
    float* ptr = mat.data();
    for (int32 ii=0; ii<9; ii++)
        ptr[ii] = frand();
}
//-----------------------------------------------------------------------------
void randomize(Matrix3f& mat, const float min, const float max)
{
    float* ptr = mat.data();
    for (int32 ii=0; ii<9; ii++)
        ptr[ii] = frand(min, max);
}
//-----------------------------------------------------------------------------
String toString(const Matrix3f& mat)
{
    String result;
    for(int32 ii=0; ii<3; ii++)
    {
        result<<L"|";
        for(int32 jj=0; jj<3; jj++)
        {
            result << Core::toString(mat[ii][jj], 2, 3) << L", ";
        }
        result << Core::toString(mat[ii][3], 2, 3) << L"|" << L"\n";
    }
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Matrix3f& mat, int32 precision)
{
    String result;
    for(int32 ii=0; ii<3; ii++)
    {
        result<<L"|";
        for(int32 jj=0; jj<3; jj++)
        {
            result << Core::toString(mat[ii][jj], 2, precision) << L", ";
        }
        result << Core::toString(mat[ii][3], 2, precision) << L"|" << L"\n";
    }
    return result;
}

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
bool Matrix4f::isIdentity(const float epsilon) const
{
    return(L_abs(_00 - 1)<=epsilon && L_abs(_01 - 0)<=epsilon && L_abs(_02 - 0)<=epsilon && L_abs(_03 - 0)<=epsilon &&
            L_abs(_10 - 0)<=epsilon && L_abs(_11 - 1)<=epsilon && L_abs(_12 - 0)<=epsilon && L_abs(_13 - 0)<=epsilon &&
            L_abs(_20 - 0)<=epsilon && L_abs(_21 - 0)<=epsilon && L_abs(_22 - 1)<=epsilon && L_abs(_23 - 0)<=epsilon &&
            L_abs(_30 - 0)<=epsilon && L_abs(_31 - 0)<=epsilon && L_abs(_32 - 0)<=epsilon && L_abs(_33 - 1)<=epsilon);
}
//-----------------------------------------------------------------------------
bool Matrix4f::isZero(const float epsilon) const
{
    return (L_abs(_00)<=epsilon && L_abs(_01)<=epsilon && L_abs(_02)<=epsilon && L_abs(_03)<=epsilon &&
            L_abs(_10)<=epsilon && L_abs(_11)<=epsilon && L_abs(_12)<=epsilon && L_abs(_13)<=epsilon &&
            L_abs(_20)<=epsilon && L_abs(_21)<=epsilon && L_abs(_22)<=epsilon && L_abs(_23)<=epsilon &&
            L_abs(_30)<=epsilon && L_abs(_31)<=epsilon && L_abs(_32)<=epsilon && L_abs(_33)<=epsilon);
}
//-----------------------------------------------------------------------------
float Matrix4f::determinant() const
{
    // 2x2 sub-determinants
    float det2_01_01 = _00 * _11 - _01 * _10;
    float det2_01_02 = _00 * _12 - _02 * _10;
    float det2_01_03 = _00 * _13 - _03 * _10;
    float det2_01_12 = _01 * _12 - _02 * _11;
    float det2_01_13 = _01 * _13 - _03 * _11;
    float det2_01_23 = _02 * _13 - _03 * _12;

    // 3x3 sub-determinants
    float det3_201_012 = _20 * det2_01_12 - _21 * det2_01_02 + _22 * det2_01_01;
    float det3_201_013 = _20 * det2_01_13 - _21 * det2_01_03 + _23 * det2_01_01;
    float det3_201_023 = _20 * det2_01_23 - _22 * det2_01_03 + _23 * det2_01_02;
    float det3_201_123 = _21 * det2_01_23 - _22 * det2_01_13 + _23 * det2_01_12;

    return ( - det3_201_123 * _30 + det3_201_023 * _31 - det3_201_013 * _32 + det3_201_012 * _33);
}
//-----------------------------------------------------------------------------
void Matrix4f::getTransformFast(Vector3f& translation, Quaternionf& rotation, Vector3f& scale) const
{
    Matrix3f m33(*this);

    translation = getTranslation();
    scale = getScale();

    if (scale._x != 0)
    {
        m33._00 /= scale._x;
        m33._01 /= scale._x;
        m33._02 /= scale._x;
    }

    if (scale._y != 0)
    {
        m33._10 /= scale._y;
        m33._11 /= scale._y;
        m33._12 /= scale._y;
    }

    if (scale._z != 0)
    {
        m33._20 /= scale._z;
        m33._21 /= scale._z;
        m33._22 /= scale._z;
    }

    rotation = matrixToQuaternion(m33);
}
//-----------------------------------------------------------------------------
void Matrix4f::getTransformSafe(Vector3f& translation, Quaternionf& rotation,
                                                       Vector3f& scale, Quaternionf& scaleOrientation) const
{
    MathDetail::AffineParts parts;
    MathDetail::HMatrix A;
    float mul = 1;

    A[0][0] = _00;
    A[0][1] = _01;
    A[0][2] = _02;
    A[0][3] = _03;

    A[1][0] = _10;
    A[1][1] = _11;
    A[1][2] = _12;
    A[1][3] = _13;

    A[2][0] = _20;
    A[2][1] = _21;
    A[2][2] = _22;
    A[2][3] = _23;

    A[3][0] = _30;
    A[3][1] = _31;
    A[3][2] = _32;
    A[3][3] = _33;

    //on lance la decomposition
    MathDetail::decomp_affine(A, &parts);

    //translation
    if (parts.t.w != 0)
        mul = 1.0f / parts.t.w;
    translation._x = parts.t.x * mul;
    translation._y = parts.t.y * mul;
    translation._z = parts.t.z * mul;

    //rotation
    rotation._v._x = parts.q.x;
    rotation._v._y = parts.q.y;
    rotation._v._z = parts.q.z;
    rotation._w = parts.q.w;

    //scale
    mul = 1.0;
    if (parts.k.w != 0)
        mul = 1.0f / parts.k.w;
    // le signe du determinant nous permet d'eviter les valeurs de scale négative
    mul *= parts.f;
    scale._x = parts.k.x * mul;
    scale._y = parts.k.y * mul;
    scale._z = parts.k.z * mul;

    //scale orientation
    scaleOrientation._v._x = parts.u.x;
    scaleOrientation._v._y = parts.u.y;
    scaleOrientation._v._z = parts.u.z;
    scaleOrientation._w = parts.u.w;
}
//-----------------------------------------------------------------------------
void Matrix4f::setZVec(const Vector3f& zVec, const Vector3f& up)
{
    Matrix3f m3;
    m3.setZVec(zVec, up, up);
    *this = Matrix4f(m3, Vector3f(0.0f, 0.0f, 0.0f));
}
//-----------------------------------------------------------------------------
void Matrix4f::setRotation(const Quaternionf& quat)
{
    Vector3f v2=quat._v+quat._v;
    double xx=1-v2._x*quat._v._x;
    double yy=v2._y*quat._v._y;
    double xw=v2._x*quat._w;
    double xy=v2._y*quat._v._x;
    double yz=v2._z*quat._v._y;
    double yw=v2._y*quat._w;
    double xz=v2._z*quat._v._x;
    double zz=v2._z*quat._v._z;
    double zw=v2._z*quat._w;

    _00 = float(1-yy-zz);
    _01 = float(xy-zw);
    _02 = float(xz+yw);

    _10 = float(xy+zw);
    _11 = float(xx-zz);
    _12 = float(yz-xw);

    _20 = float(xz-yw);
    _21 = float(yz+xw);
    _22 = float(xx-yy);

    _03 = 0;
    _13 = 0;
    _23 = 0;

    _30 = 0;
    _31 = 0;
    _32 = 0;

    _33 = 1;
}
//-----------------------------------------------------------------------------
void Matrix4f::normalize()
{
    setXVector(getXVector().getNormalizedSafe());
    setYVector(getYVector().getNormalizedSafe());
    setZVector(getZVector().getNormalizedSafe());
}
//-----------------------------------------------------------------------------
void Matrix4f::setTransform(const Vector3f& translation, const Quaternionf& rotation,
                                                   const Vector3f& scale)
{
    Matrix4f tmp(ZERO);
    const Quaternionf identity(IDENTITY);
    const Vector3f one(ONE);
    const Vector3f zero(ZERO);

    if (scale != one)
    {
        //scale
        setScale(scale);

        //rotation
        if (rotation != identity)
        {
            tmp.setRotation(rotation);
            preMultiply(tmp);
        }
        if (translation != zero)
        {
            tmp.setTranslation(translation);
            preMultiply(tmp);
        }
    }
    else
    {
        if (rotation != identity)
        {
            setRotation(rotation);
            if (translation != zero)
            {
                tmp.setTranslation(translation);
                preMultiply(tmp);
            }
        }
        else
        {
            setTranslation(translation);
        }
    }
}
//-----------------------------------------------------------------------------
void Matrix4f::setTransform(const Vector3f& translation, const Quaternionf& rotation,
                                                   const Vector3f& scale, const Quaternionf& scaleOrientation)
{
    Matrix4f tmp(ZERO);
    const Quaternionf identity(IDENTITY);
    const Vector3f one(ONE);
    const Vector3f zero(ZERO);

    if (scale != one) //si il y a un changement d'echelle
    {
        //scale et scaleOrientation
        if (scaleOrientation != identity)
        {
            setRotation(scaleOrientation.getInverted());
            tmp.setScale(scale);
            preMultiply(tmp);
            tmp.setRotation(scaleOrientation);
            preMultiply(tmp);
        }
        else
        {
            setScale(scale);
        }

        //rotation
        if (rotation != identity)
        {
            tmp.setRotation(rotation);
            preMultiply(tmp);
        }

        //translation
        if (translation != zero)
        {
            tmp.setTranslation(translation);
            preMultiply(tmp);
        }
    }
    else
    {
        //rotation
        if (rotation != identity)
        {
            setRotation(rotation);
            //translation
            if (translation != zero)
            {
                tmp.setTranslation(translation);
                preMultiply(tmp);
            }
        }
        else
        {
            //translation
            setTranslation(translation);
        }
    }
}
//-----------------------------------------------------------------------------
void Matrix4f::setTransform(const Vector3f& translation, const Quaternionf& rotation,
                                                   const Vector3f& scale, const Quaternionf& scaleOrientation,
                                                   const Vector3f& center)
{
    Matrix4f tmp(ZERO);
    const Quaternionf identity(IDENTITY);
    const Vector3f one(ONE);
    const Vector3f zero(ZERO);

    setTranslation(-center);

    if (scale != one)
    {
        if (scaleOrientation != identity)
        {
            tmp.setRotation(scaleOrientation.getInverted());
            preMultiply(tmp);
        }

        tmp.setScale(scale);
        preMultiply(tmp);

        if (scaleOrientation != identity)
        {
            tmp.setRotation(scaleOrientation);
            preMultiply(tmp);
        }
    }

    if (rotation != identity)
    {
        tmp.setRotation(rotation);
        preMultiply(tmp);
    }

    const Vector3f sum = center + translation;
    if (sum != zero)
    {
        tmp.setTranslation(sum);
        preMultiply(tmp);
    }
}
//-----------------------------------------------------------------------------
bool Matrix4f::inverseSelf()
{
    float det, invDet;

    float det2_01_01 = _00 * _11 - _01 * _10;
    float det2_01_02 = _00 * _12 - _02 * _10;
    float det2_01_03 = _00 * _13 - _03 * _10;
    float det2_01_12 = _01 * _12 - _02 * _11;
    float det2_01_13 = _01 * _13 - _03 * _11;
    float det2_01_23 = _02 * _13 - _03 * _12;

    float det3_201_012 = _20 * det2_01_12 - _21 * det2_01_02 + _22 * det2_01_01;
    float det3_201_013 = _20 * det2_01_13 - _21 * det2_01_03 + _23 * det2_01_01;
    float det3_201_023 = _20 * det2_01_23 - _22 * det2_01_03 + _23 * det2_01_02;
    float det3_201_123 = _21 * det2_01_23 - _22 * det2_01_13 + _23 * det2_01_12;

    det = ( - det3_201_123 * _30 + det3_201_023 * _31 - det3_201_013 * _32 + det3_201_012 * _33);

    if (fabs(det) < LM_MATRIX_INVERSE_EPSILON )
    return false;

    invDet = 1.0f / det;

    // remaining 2x2 sub-determinants
    float det2_03_01 = _00 * _31 - _01 * _30;
    float det2_03_02 = _00 * _32 - _02 * _30;
    float det2_03_03 = _00 * _33 - _03 * _30;
    float det2_03_12 = _01 * _32 - _02 * _31;
    float det2_03_13 = _01 * _33 - _03 * _31;
    float det2_03_23 = _02 * _33 - _03 * _32;

    float det2_13_01 = _10 * _31 - _11 * _30;
    float det2_13_02 = _10 * _32 - _12 * _30;
    float det2_13_03 = _10 * _33 - _13 * _30;
    float det2_13_12 = _11 * _32 - _12 * _31;
    float det2_13_13 = _11 * _33 - _13 * _31;
    float det2_13_23 = _12 * _33 - _13 * _32;

    // remaining 3x3 sub-determinants
    float det3_203_012 = _20 * det2_03_12 - _21 * det2_03_02 + _22 * det2_03_01;
    float det3_203_013 = _20 * det2_03_13 - _21 * det2_03_03 + _23 * det2_03_01;
    float det3_203_023 = _20 * det2_03_23 - _22 * det2_03_03 + _23 * det2_03_02;
    float det3_203_123 = _21 * det2_03_23 - _22 * det2_03_13 + _23 * det2_03_12;

    float det3_213_012 = _20 * det2_13_12 - _21 * det2_13_02 + _22 * det2_13_01;
    float det3_213_013 = _20 * det2_13_13 - _21 * det2_13_03 + _23 * det2_13_01;
    float det3_213_023 = _20 * det2_13_23 - _22 * det2_13_03 + _23 * det2_13_02;
    float det3_213_123 = _21 * det2_13_23 - _22 * det2_13_13 + _23 * det2_13_12;

    float det3_301_012 = _30 * det2_01_12 - _31 * det2_01_02 + _32 * det2_01_01;
    float det3_301_013 = _30 * det2_01_13 - _31 * det2_01_03 + _33 * det2_01_01;
    float det3_301_023 = _30 * det2_01_23 - _32 * det2_01_03 + _33 * det2_01_02;
    float det3_301_123 = _31 * det2_01_23 - _32 * det2_01_13 + _33 * det2_01_12;

    _00 = - det3_213_123 * invDet;
    _10 = + det3_213_023 * invDet;
    _20 = - det3_213_013 * invDet;
    _30 = + det3_213_012 * invDet;

    _01 = + det3_203_123 * invDet;
    _11 = - det3_203_023 * invDet;
    _21 = + det3_203_013 * invDet;
    _31 = - det3_203_012 * invDet;

    _02 = + det3_301_123 * invDet;
    _12 = - det3_301_023 * invDet;
    _22 = + det3_301_013 * invDet;
    _32 = - det3_301_012 * invDet;

    _03 = - det3_201_123 * invDet;
    _13 = + det3_201_023 * invDet;
    _23 = - det3_201_013 * invDet;
    _33 = + det3_201_012 * invDet;

    return true;
}
//-----------------------------------------------------------------------------
void Matrix4f::preMultiply(const Matrix4f& other)
{
    float val[4];

    val[0] = other._00*_00 + other._01*_10 + other._02*_20 + other._03*_30;
    val[1] = other._10*_00 + other._11*_10 + other._12*_20 + other._13*_30;
    val[2] = other._20*_00 + other._21*_10 + other._22*_20 + other._23*_30;
    val[3] = other._30*_00 + other._31*_10 + other._32*_20 + other._33*_30;
    _00 = val[0]; _10 = val[1]; _20 = val[2]; _30 = val[3];

    val[0] = other._00*_01 + other._01*_11 + other._02*_21 + other._03*_31;
    val[1] = other._10*_01 + other._11*_11 + other._12*_21 + other._13*_31;
    val[2] = other._20*_01 + other._21*_11 + other._22*_21 + other._23*_31;
    val[3] = other._30*_01 + other._31*_11 + other._32*_21 + other._33*_31;
    _01 = val[0]; _11 = val[1]; _21 = val[2]; _31 = val[3];

    val[0] = other._00*_02 + other._01*_12 + other._02*_22 + other._03*_32;
    val[1] = other._10*_02 + other._11*_12 + other._12*_22 + other._13*_32;
    val[2] = other._20*_02 + other._21*_12 + other._22*_22 + other._23*_32;
    val[3] = other._30*_02 + other._31*_12 + other._32*_22 + other._33*_32;
    _02 = val[0]; _12 = val[1]; _22 = val[2]; _32 = val[3];

    val[0] = other._00*_03 + other._01*_13 + other._02*_23 + other._03*_33;
    val[1] = other._10*_03 + other._11*_13 + other._12*_23 + other._13*_33;
    val[2] = other._20*_03 + other._21*_13 + other._22*_23 + other._23*_33;
    val[3] = other._30*_03 + other._31*_13 + other._32*_23 + other._33*_33;
    _03 = val[0]; _13 = val[1]; _23 = val[2]; _33 = val[3];
}
//-----------------------------------------------------------------------------
void Matrix4f::postMultiply(const Matrix4f& other)
{
    float val[4];

    val[0] = _00*other._00 + _01*other._10 + _02*other._20 + _03*other._30;
    val[1] = _00*other._01 + _01*other._11 + _02*other._21 + _03*other._31;
    val[2] = _00*other._02 + _01*other._12 + _02*other._22 + _03*other._32;
    val[3] = _00*other._03 + _01*other._13 + _02*other._23 + _03*other._33;
    _00 = val[0]; _01 = val[1]; _02 = val[2]; _03 = val[3];

    val[0] = _10*other._00 + _11*other._10 + _12*other._20 + _13*other._30;
    val[1] = _10*other._01 + _11*other._11 + _12*other._21 + _13*other._31;
    val[2] = _10*other._02 + _11*other._12 + _12*other._22 + _13*other._32;
    val[3] = _10*other._03 + _11*other._13 + _12*other._23 + _13*other._33;
    _10 = val[0]; _11 = val[1]; _12 = val[2]; _13 = val[3];

    val[0] = _20*other._00 + _21*other._10 + _22*other._20 + _23*other._30;
    val[1] = _20*other._01 + _21*other._11 + _22*other._21 + _23*other._31;
    val[2] = _20*other._02 + _21*other._12 + _22*other._22 + _23*other._32;
    val[3] = _20*other._03 + _21*other._13 + _22*other._23 + _23*other._33;
    _20 = val[0]; _21 = val[1]; _22 = val[2]; _23 = val[3];

    val[0] = _30*other._00 + _31*other._10 + _32*other._20 + _33*other._30;
    val[1] = _30*other._01 + _31*other._11 + _32*other._21 + _33*other._31;
    val[2] = _30*other._02 + _31*other._12 + _32*other._22 + _33*other._32;
    val[3] = _30*other._03 + _31*other._13 + _32*other._23 + _33*other._33;
    _30 = val[0]; _31 = val[1]; _32 = val[2]; _33 = val[3];
}
//-----------------------------------------------------------------------------
void Matrix4f::setTransform(const Transformf& transf)
{
    setTransform(transf._translation, transf._rotation, transf._scale, transf._scaleOrientation);
}
//-----------------------------------------------------------------------------
void Matrix4f::getTransformSafe(Transformf& transf) const
{
    getTransformSafe(transf._translation, transf._rotation, transf._scale, transf._scaleOrientation);
}
//-----------------------------------------------------------------------------
String toString(const Matrix4f& mat)
{
    String result;
    for(int32 ii=0; ii<4; ii++)
    {
        result<<L"|";
        for(int32 jj=0; jj<3; jj++)
            result << Core::toString(mat[ii][jj], 2, 3) << L", ";
        result << Core::toString(mat[ii][3], 2, 3) << L"|" << L"\n";
    }
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Matrix4f& mat, int32 precision)
{
    String result;
    for(int32 ii=0; ii<4; ii++)
    {
        result<<L"|";
        for(int32 jj=0; jj<3; jj++)
            result << Core::toString(mat[ii][jj], 2, precision) << L", ";
        result << Core::toString(mat[ii][3], 2, precision) << L"|" << L"\n";
    }
    return result;
}
//-----------------------------------------------------------------------------
void randomize(Matrix4f& mat)
{
    float* ptr = mat.data();
    for (int32 ii=0; ii<16; ii++)
        ptr[ii] = frand();
}
//-----------------------------------------------------------------------------
void randomize(Matrix4f& mat, const float min, const float max)
{
    float* ptr = mat.data();
    for (int32 ii=0; ii<16; ii++)
        ptr[ii] = frand(min, max);
}
//-----------------------------------------------------------------------------
} //namespace Core
