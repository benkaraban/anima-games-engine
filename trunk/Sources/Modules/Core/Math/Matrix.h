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
#ifndef CORE_MATRIX_H_
#define CORE_MATRIX_H_

#include <Core/Math/Vector.h>

#ifdef _DEBUG
#define MATINLINE
#define MATFUNCINLINE LM_API_COR
#else
#define MATINLINE inline
#define MATFUNCINLINE inline
#endif

#define LM_MATRIX_INVERSE_EPSILON		1e-14
#define LM_MATRIX_EPSILON				1e-6

#define LM_SET_ROW(row, val_1, val_2, val_3, val_4)    \
    _line[(row)][0] = (val_1); \
    _line[(row)][1] = (val_2); \
    _line[(row)][2] = (val_3); \
    _line[(row)][3] = (val_4);

namespace Core
{
struct Orientationf;
struct Matrix4f;
struct Quaternionf;
struct Transformf;

struct LM_API_COR Matrix3f
{
    MATINLINE Matrix3f();
    MATINLINE explicit Matrix3f(const Vector3f& mat_line_0, const Vector3f& mat_line_1, const Vector3f& mat_line_2);
    MATINLINE explicit Matrix3f(const float, const float, const float, const float, const float, const float, const float, const float, const float);
    MATINLINE explicit Matrix3f(Identity_t);
    MATINLINE explicit Matrix3f(Zero_t);
    MATINLINE Matrix3f(NAN_t);
    MATINLINE explicit Matrix3f(const Matrix4f& other);

    MATINLINE const Vector3f& operator[](const int) const;
    MATINLINE Vector3f& operator[](const int);
    MATINLINE const float* data() const;
    MATINLINE float* data();

    MATINLINE Matrix3f operator*(const float a) const;
    MATINLINE Matrix3f operator/(float a) const;
    MATINLINE Matrix3f& operator*=(const float a);
    MATINLINE Matrix3f& operator/=(const float a);

    MATINLINE bool operator==(const Matrix3f& a) const;
    MATINLINE bool operator!=(const Matrix3f& a) const;
    MATINLINE bool isEquivalent(const Matrix3f& a, const float epsilon) const;

    MATINLINE void set(const float M00, const float M01, const float M02, const float M10, const float M11, const float M12, const float M20,
                    const float M21, const float M22);
    MATINLINE void setZero();
    MATINLINE void setIdentity();

    bool isIdentity(const float epsilon = f_COMMON_EPSILON) const;
    bool isZero(const float epsilon = f_COMMON_EPSILON) const;

    float determinant() const;
    MATINLINE Matrix3f getInvertedSafe() const;
    bool inverseSafeSelf();
    Matrix3f getTransposed() const;
    void transposeSelf();

    MATINLINE Matrix4f toMatrix4() const;

    MATINLINE void setRotation(const Orientationf& angle);
    void setRotation(const Quaternionf& quat);
    MATINLINE void setRotationPitch(const float rad);
    MATINLINE void setRotationYaw(const float rad);
    MATINLINE void setRotationRoll(const float rad);

    void setZVec(const Vector3f & zVec, const Vector3f & up1, const Vector3f & up2);

    static MATINLINE Matrix3f makeRotation(const Orientationf& angle);
    static MATINLINE Matrix3f makeRotationPitch(const float rad);
    static MATINLINE Matrix3f makeRotationYaw(const float rad);
    static MATINLINE Matrix3f makeRotationRoll(const float rad);

    union
    {
        struct
        {
            float _line[3][3];
        };

        struct
        {
            float _00, _01, _02;
            float _10, _11, _12;
            float _20, _21, _22;
        };
    };
};


//Matrix3f-Vector3f
//post-multiplication
MATFUNCINLINE Vector3f operator*(const Matrix3f& mat, const Vector3f& vec);
//pre-multiplication
MATFUNCINLINE Vector3f operator*(const Vector3f& vec, const Matrix3f& mat);

//Matrix3f-Matrix3f
MATFUNCINLINE Matrix3f operator*(const Matrix3f& mat_1, const Matrix3f& mat_2);
MATFUNCINLINE Matrix3f& operator *=(Matrix3f& mat_1, const Matrix3f& mat_2);
MATFUNCINLINE Matrix3f operator+(const Matrix3f& mat_1, const Matrix3f& mat_2);
MATFUNCINLINE Matrix3f& operator+=(Matrix3f& mat_1, const Matrix3f& mat_2);
MATFUNCINLINE Matrix3f operator -(const Matrix3f& mat_1, const Matrix3f& mat_2);
MATFUNCINLINE Matrix3f& operator-=(Matrix3f& mat_1, const Matrix3f& mat_2);

struct LM_API_COR Matrix4f
{
    MATINLINE Matrix4f();
    MATINLINE Matrix4f(const Matrix4f & mat);
    MATINLINE explicit Matrix4f(MatLines_t tline,
                            const Vector4f & line0, const Vector4f & line1,
                            const Vector4f & line2, const Vector4f & line3);
    MATINLINE explicit Matrix4f(const float, const float, const float, const float, const float, const float, const float, const float, const float, const float,
                            const float, const float, const float, const float, const float, const float);

    MATINLINE explicit Matrix4f(MatAxis_t taxis,
                            const Vector3f & vx, const Vector3f & vy,
                            const Vector3f & vz, const Vector3f & vt);
    MATINLINE explicit Matrix4f(MatAxis_t taxis,
                            const Vector4f & vx, const Vector4f & vy,
                            const Vector4f & vz, const Vector4f & vt);

    MATINLINE explicit Matrix4f(const Matrix3f& rotation, const Vector3f& translation);

    MATINLINE explicit Matrix4f(Zero_t);
    MATINLINE explicit Matrix4f(Identity_t);

    MATINLINE explicit Matrix4f(Translation_t, const float x, const float y, const float z);
    MATINLINE explicit Matrix4f(Translation_t, const Vector3f & trans);

    MATINLINE explicit Matrix4f(Scale_t, const float x, const float y, const float z);
    MATINLINE explicit Matrix4f(Scale_t, const Vector3f & scale);

    MATINLINE explicit Matrix4f(Rotation_t, const float angle, ERotationAxis axis);
    MATINLINE explicit Matrix4f(Rotation_t, const float angle, const Vector3f & axis);
    MATINLINE explicit Matrix4f(Rotation_t, const float angle, const float x, const float y, const float z);

    MATINLINE explicit Matrix4f(const Quaternionf& quat);

    MATINLINE explicit Matrix4f(MatZVec_t, const Vector3f & zvec, const Vector3f & up);

    MATINLINE const Vector4f& operator[](const int) const;
    MATINLINE Vector4f& operator[](const int);
    MATINLINE const float* data() const;
    MATINLINE float* data();


    MATINLINE Matrix4f operator*(const float a) const;
    MATINLINE Matrix4f operator/(float a) const;
    MATINLINE Matrix4f& operator*=(const float a);
    MATINLINE Matrix4f& operator/=(const float a);

    MATINLINE bool operator==(const Matrix4f& a) const;
    MATINLINE bool operator!=(const Matrix4f& a) const;
    MATINLINE bool isEquivalent(const Matrix4f& a, const float epsilon = f_COMMON_EPSILON) const;

    MATINLINE void set(const float M00, const float M01, const float M02, const float M03, const float M10, const float M11, const float M12,
                    const float M13, const float M20, const float M21, const float M22, const float M23, const float M30, const float M31,
                    const float M32, const float M33);
    MATINLINE void setZero();
    MATINLINE void setIdentity();

    bool isIdentity(const float epsilon = f_COMMON_EPSILON) const;
    bool isZero(const float epsilon = f_COMMON_EPSILON) const;

    float determinant() const;

    MATINLINE Matrix4f getInverted() const;
    bool inverseSelf();

    Matrix4f getTransposed() const;
    void transposeSelf();

    void switchHand();

    void postMultiply(const Matrix4f& other);

    void preMultiply(const Matrix4f& other);

    // Matrice translation
    MATINLINE void setTranslation(const Vector3f& vec);
    MATINLINE void addTranslation(const Vector3f& vec);
    MATINLINE Vector3f getTranslation() const;
    MATINLINE Matrix3f getRotation() const;

    // Matrice scale
    MATINLINE void setScale(const Vector3f& vec);
    MATINLINE void setScale(const float val);
    MATINLINE Vector3f getScale() const;

    // Matrices projection
    MATINLINE void setOrthoProj(const float left, const float right, const float bottom, const float top, const float near, const float far);
    MATINLINE void setOrthoProj2D(const float left, const float right, const float bottom, const float top);
    MATINLINE void setFrustumProj(const float left, const float right, const float bottom, const float top, const float near, const float far);
    MATINLINE void setPerspectiveProj(const float fov, const float aspectRatio, const float near, const float far);

    // Matrices diverses
    MATINLINE void setLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);

    void setZVec(const Vector3f & zVec, const Vector3f & up);

    MATINLINE bool isValid() const;

    void getTransformSafe(Transformf& transf) const;
    void getTransformFast(Transformf& transf) const;
    void getTransformSafe(Vector3f& translation, Quaternionf& rotation, Vector3f& scale,
                          Quaternionf& scaleOrientation) const;
    void getTransformFast(Vector3f& translation, Quaternionf& rotation, Vector3f& scale) const;

    void setTransform(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale,
                      const Quaternionf& scaleOrientation);
    void setTransform(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale);
    void setTransform(const Vector3f& translation, const Quaternionf& rotation, const Vector3f& scale,
                      const Quaternionf& scaleOrientation, const Vector3f& center);

    void setTransform(const Transformf& transf);

    MATINLINE void setQT(const Quaternionf& quat, const Vector3f & trans);
    MATINLINE void setTransposeQT(const Quaternionf& quat, const Vector3f & trans);


    // Modifie la rotation sans affecter le reste de la matrice, pour créer une matrice de rotation
    // de toute pièce, il faut utiliser makeXXX
    void setRotation(const Quaternionf& quat);

    void normalize();

    MATINLINE void setRotation(const Orientationf& angle);
    MATINLINE void setRotationPitch(const float rad);
    MATINLINE void setRotationYaw(const float rad);
    MATINLINE void setRotationRoll(const float rad);
    MATINLINE void setRotation(const float rad, const Vector3f & axis);
    MATINLINE void setRotation(const float rad, const float x, const float y, const float z);

    static MATINLINE Matrix4f makeScale(const Vector3f& vec);

    static MATINLINE Matrix4f makeTranslate(const Vector3f& vec);

    static MATINLINE Matrix4f makeOrthoProj(const float left, const float right, const float bottom, const float top, const float near,
                                          const float far);

    static MATINLINE Matrix4f makeOrthoProj2D(const float left, const float right, const float bottom, const float top);

    static MATINLINE Matrix4f makeFrustumProj(const float left, const float right, const float bottom,
                                             const float top, const float near, const float far);

    static MATINLINE Matrix4f makePerspectiveProj(const float fov, const float aspectRatio, const float near, const float far);


    static MATINLINE Matrix4f makeLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);

    static MATINLINE Matrix4f makeInverse(const Matrix4f& mat);

    static MATINLINE Matrix4f makeOrthoNormal(const Matrix4f& mat);


    static MATINLINE Matrix4f makeRotation(const Orientationf& angle);
    static MATINLINE Matrix4f makeRotationPitch(const float rad);
    static MATINLINE Matrix4f makeRotationYaw(const float rad);
    static MATINLINE Matrix4f makeRotationRoll(const float rad);

    MATINLINE Vector3f apply(const Vector3f& vec) const;
    MATINLINE Vector4f apply(const Vector4f& vec) const;
    MATINLINE Vector3f apply3x3(const Vector3f& vec) const;

    MATINLINE Vector3f getXVector() const { return Vector3f(_Xx, _Xy, _Xz); };
    MATINLINE Vector3f getYVector() const { return Vector3f(_Yx, _Yy, _Yz); };
    MATINLINE Vector3f getZVector() const { return Vector3f(_Zx, _Zy, _Zz); };
    MATINLINE Vector3f getTVector() const { return Vector3f(_Tx, _Ty, _Tz); };

    MATINLINE void setXVector(const Vector3f & vec) { _Xx = vec.x; _Xy = vec.y; _Xz = vec.z; };
    MATINLINE void setYVector(const Vector3f & vec) { _Yx = vec.x; _Yy = vec.y; _Yz = vec.z; };
    MATINLINE void setZVector(const Vector3f & vec) { _Zx = vec.x; _Zy = vec.y; _Zz = vec.z; };
    MATINLINE void setTVector(const Vector3f & vec) { _Tx = vec.x; _Ty = vec.y; _Tz = vec.z; };

    union
    {
        struct
        {
            float _line[4][4];
        };

        struct
        {
            float _val[16];
        };

        struct
        {
            float _00, _01, _02, _03;
            float _10, _11, _12, _13;
            float _20, _21, _22, _23;
            float _30, _31, _32, _33;
        };

        struct
        {
            float _Xx, _Yx, _Zx, _Tx;
            float _Xy, _Yy, _Zy, _Ty;
            float _Xz, _Yz, _Zz, _Tz;
            float _Xw, _Yw, _Zw, _Tw;
        };
    };

    const static Matrix4f IDENTITY_MATRIX;
};

//Matrix4f-Scalar
/*
inline Matrix4f operator*(const Matrix4f& mat, const float a);
inline Matrix4f operator/(const Matrix4f& mat, float a);
*/
//Matrix4f-Vector4f
//post-multiplication
MATFUNCINLINE Vector4f operator*(const Matrix4f& mat, const Vector4f& vec);
MATFUNCINLINE Vector3f operator*(const Matrix4f& mat, const Vector3f& vec);
//pre-multiplication
MATFUNCINLINE Vector4f operator*(const Vector4f& vec, const Matrix4f& mat);
MATFUNCINLINE Vector3f operator*(const Vector3f& vec, const Matrix4f& mat);

//Matrix4f-Matrix4f
MATFUNCINLINE Matrix4f operator *(const Matrix4f& mat_1, const Matrix4f& mat_2);
MATFUNCINLINE Matrix4f& operator *=(Matrix4f& mat_1, const Matrix4f& mat_2);
MATFUNCINLINE Matrix4f operator+(const Matrix4f& mat_1, const Matrix4f& mat_2);
MATFUNCINLINE Matrix4f& operator+=(Matrix4f& mat_1, const Matrix4f& mat_2);
MATFUNCINLINE Matrix4f operator -(const Matrix4f& mat_1, const Matrix4f& mat_2);
MATFUNCINLINE Matrix4f& operator-=(Matrix4f& mat_1, const Matrix4f& mat_2);

//-----------------------------------------------------------------------------
//toString
LM_API_COR String toString(const Matrix3f& mat);
LM_API_COR String toString(const Matrix3f& mat, int32 precision);
LM_API_COR String toString(const Matrix4f& mat);
LM_API_COR String toString(const Matrix4f& mat, int32 precision);

LM_API_COR void randomize(Matrix3f& mat);
//LM_API_COR void randomize(Matrix3d& mat);

LM_API_COR void randomize(Matrix3f& mat, const float min, const float max);
//LM_API_COR void randomize(Matrix3d& mat, const double min, const double max);

LM_API_COR void randomize(Matrix4f& mat);
//LM_API_COR void randomize(Matrix4d& mat);

LM_API_COR void randomize(Matrix4f& mat, const float min, const float max);
//LM_API_COR void randomize(Matrix4d& mat, const double min, const double max);
} //namespace Core


#ifndef _DEBUG
#include <Core/Math/Matrix3.inl>
#include <Core/Math/Matrix4.inl>
#endif

#endif /*MATRIX_H_*/

