
#include <Core/Math/Orientation.h> 

#include <string.h>

namespace Core
{
//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f()
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix3f()\n";
#endif
}

//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(const Vector3f& line_0, const Vector3f& line_1,
                                                const Vector3f& line_2)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix3f(const Vector3f& line_0, const Vector3f& line_1, const Vector3f& line_2)\n";
#endif
    memcpy(_line[0], line_0.data(), sizeof(Vector3f));
    memcpy(_line[1], line_1.data(), sizeof(Vector3f));
    memcpy(_line[2], line_2.data(), sizeof(Vector3f));
}

//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(const float val_00, const float val_01, const float val_02, const float val_10,
                                                const float val_11, const float val_12, const float val_20, const float val_21,
                                                const float val_22) :
    _00(val_00), _01(val_01), _02(val_02), _10(val_10), _11(val_11), _12(val_12), _20(val_20), _21(val_21), _22(val_22)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix3f(const float * 9)\n";
#endif
}

//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(Identity_t) :
    _00(1), _01(0), _02(0), _10(0), _11(1), _12(0), _20(0), _21(0), _22(1)
{
}

//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(Zero_t) :
    _00(0), _01(0), _02(0), _10(0), _11(0), _12(0), _20(0), _21(0), _22(0)
{
}
//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(NAN_t) :
    _00(f_INIT_TO_NAN), _01(f_INIT_TO_NAN), _02(f_INIT_TO_NAN), _10(f_INIT_TO_NAN), _11(f_INIT_TO_NAN), _12(f_INIT_TO_NAN), _20(f_INIT_TO_NAN), _21(f_INIT_TO_NAN), _22(f_INIT_TO_NAN)
{

}
//-----------------------------------------------------------------------------
MATINLINE Matrix3f::Matrix3f(const Matrix4f& other)
{
    _00=float(other._00); _01=float(other._01); _02=float(other._02);
    _10=float(other._10); _11=float(other._11); _12=float(other._12);
    _20=float(other._20); _21=float(other._21); _22=float(other._22);
}

//-----------------------------------------------------------------------------
MATINLINE const Vector3f& Matrix3f::operator[](const int index) const
{
    return reinterpret_cast<const Vector3f*>(_line)[index];
}

//-----------------------------------------------------------------------------

MATINLINE Vector3f &Matrix3f::operator[](const int index)
{
    return reinterpret_cast<Vector3f*>(_line)[index];
}

//-----------------------------------------------------------------------------

MATINLINE const float* Matrix3f::data() const
{
    return &_00;
}

//-----------------------------------------------------------------------------

MATINLINE float* Matrix3f::data()
{
    return &_00;
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix3f::operator==(const Matrix3f& mat) const
{
    if(_00 != mat._00 || _01 != mat._01 || _02 != mat._02 ||
            _10 != mat._10 || _11 != mat._11 || _12 != mat._12 ||
            _20 != mat._20 || _21 != mat._21 || _22 != mat._22 )
    return false;

    return true;
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix3f::operator!=(const Matrix3f& mat) const
{
    if(_00 != mat._00 || _01 != mat._01 || _02 != mat._02 ||
            _10 != mat._10 || _11 != mat._11 || _12 != mat._12 ||
            _20 != mat._20 || _21 != mat._21 || _22 != mat._22 )
    return true;

    return false;
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix3f::isEquivalent(const Matrix3f& mat, const float epsilon) const
{
    return(L_abs(_00 - mat._00)<=epsilon && L_abs(_01 - mat._01)<=epsilon && L_abs(_02 - mat._02)<=epsilon &&
            L_abs(_10 - mat._10)<=epsilon && L_abs(_11 - mat._11)<=epsilon && L_abs(_12 - mat._12)<=epsilon &&
            L_abs(_20 - mat._20)<=epsilon && L_abs(_21 - mat._21)<=epsilon && L_abs(_22 - mat._22)<=epsilon);

}

MATINLINE void Matrix3f::set(const float M00, const float M01, const float M02,
        const float M10, const float M11, const float M12,
        const float M20, const float M21, const float M22)
{
    _00 = M00; _01 = M01; _02 = M02;
    _10 = M10; _11 = M11; _12 = M12;
    _20 = M20; _21 = M21; _22 = M22;
}
//-----------------------------------------------------------------------------

MATINLINE void Matrix3f::setZero()
{
    memset(_line, 0, sizeof(Matrix3f));
}

//-----------------------------------------------------------------------------

MATINLINE void Matrix3f::setIdentity()
{
    _00 = 1; _01 = 0; _02 = 0;
    _10 = 0; _11 = 1; _12 = 0;
    _20 = 0; _21 = 0; _22 = 1;
}


//-----------------------------------------------------------------------------

MATINLINE bool Matrix3f::isZero(const float epsilon) const
{
    return(L_abs(_00)<=epsilon && L_abs(_01)<=epsilon && L_abs(_02)<=epsilon &&
            L_abs(_10)<=epsilon && L_abs(_11)<=epsilon && L_abs(_12)<=epsilon &&
            L_abs(_20)<=epsilon && L_abs(_21)<=epsilon && L_abs(_22)<=epsilon);

}

//-----------------------------------------------------------------------------
MATINLINE float Matrix3f::determinant() const
{
    float det2_12_01 = _10 * _21 - _11 * _20;
    float det2_12_02 = _10 * _22 - _12 * _20;
    float det2_12_12 = _11 * _22 - _12 * _21;

    return _00 * det2_12_12 - _01 * det2_12_02 + _02 * det2_12_01;
}

//-----------------------------------------------------------------------------

MATINLINE Matrix3f Matrix3f::getInvertedSafe() const
{
    Matrix3f inverted=*this;
    inverted.inverseSafeSelf();
    return inverted;
}


//-----------------------------------------------------------------------------

MATINLINE Matrix3f Matrix3f::getTransposed() const
{
    Matrix3f transpose;

    transpose._00=_00; transpose._01=_10; transpose._02=_20;
    transpose._10=_01; transpose._11=_11; transpose._12=_21;
    transpose._20=_02; transpose._21=_12; transpose._22=_22;

    return transpose;
}

//-----------------------------------------------------------------------------
MATINLINE void Matrix3f::transposeSelf()
{
    float value;

    value = _01; _01 = _10; _10 = value;
    value = _02; _02 = _20; _20 = value;
    value = _12; _12 = _21; _21 = value;
}


MATINLINE Matrix3f Matrix3f::operator*(const float a) const
{
    return Matrix3f(
            _00 * a, _01 * a, _02 * a,
            _10 * a, _11 * a, _12 * a,
            _20 * a, _21 * a, _22 * a);
}

//-----------------------------------------------------------------------------

MATINLINE Matrix3f Matrix3f::operator/(float a) const
{
    a = 1/a;
    return Matrix3f(
            _00 * a, _01 * a, _02 * a,
            _10 * a, _11 * a, _12 * a,
            _20 * a, _21 * a, _22 * a);
}

//-----------------------------------------------------------------------------

MATINLINE Matrix3f& Matrix3f::operator*=(const float a)
{
    _00 *= a; _01 *= a; _02 *= a;
    _10 *= a; _11 *= a; _12 *= a;
    _20 *= a; _21 *= a; _22 *= a;

    return *this;
}

//-----------------------------------------------------------------------------

MATINLINE Matrix3f& Matrix3f::operator/=(float a)
{
    a = 1/a;

    _00 *= a; _01 *= a; _02 *= a;
    _10 *= a; _11 *= a; _12 *= a;
    _20 *= a; _21 *= a; _22 *= a;

    return *this;
}


MATINLINE void Matrix3f::setRotation(const Orientationf& angle)
{
    float sinPitch, cosPitch;
    float sinYaw, cosYaw;
    float sinRoll, cosRoll;

    L_sincos (angle._pitch, &sinPitch, &cosPitch);
    L_sincos (angle._yaw, &sinYaw, &cosYaw);
    L_sincos (angle._roll, &sinRoll, &cosRoll);

    float sinYaw_cosRoll = (sinYaw*cosRoll);
    float sintYaw_sinRoll = (sinYaw*sinRoll);

    _00=cosYaw*cosRoll; _01=sinYaw_cosRoll*sinPitch-cosPitch*sinRoll; _02=sinYaw_cosRoll*cosPitch+sinPitch*sinRoll;
    _10=cosYaw*sinRoll; _11=sintYaw_sinRoll*sinPitch+cosPitch*cosRoll; _12=sintYaw_sinRoll*cosPitch-sinPitch*cosRoll;
    _20=-sinYaw; _21=cosYaw*sinPitch; _22=cosYaw*cosPitch;
}


MATINLINE void Matrix3f::setRotationPitch(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00=1; _01=0; _02=0;
    _10=0; _11=cosVal; _12=-sinVal;
    _20=0; _21=sinVal; _22=cosVal;
}


MATINLINE void Matrix3f::setRotationYaw(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00=cosVal; _01=0; _02=sinVal;
    _10=0; _11=1; _12=0;
    _20=-sinVal; _21=0; _22=cosVal;
}


MATINLINE void Matrix3f::setRotationRoll(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00=cosVal; _01=-sinVal; _02=0;
    _10=sinVal; _11=cosVal; _12=0;
    _20=0; _21=0; _22=1;
}


MATINLINE Matrix3f Matrix3f::makeRotation(const Orientationf& angle)
{
    Matrix3f mat;
    mat.setRotation(angle);
    return mat;
}


MATINLINE Matrix3f Matrix3f::makeRotationPitch(const float rad)
{
    Matrix3f mat;
    mat.setRotationPitch(rad);
    return mat;
}


MATINLINE Matrix3f Matrix3f::makeRotationYaw(const float rad)
{
    Matrix3f mat;
    mat.setRotationYaw(rad);
    return mat;
}


MATINLINE Matrix3f Matrix3f::makeRotationRoll(const float rad)
{
    Matrix3f mat;
    mat.setRotationRoll(rad);
    return mat;
}

//Matrix3f-Matrix3f
//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f operator*(const Matrix3f& mat_1, const Matrix3f& mat_2)
{
    return Matrix3f(
            mat_1._00*mat_2._00 + mat_1._01*mat_2._10 + mat_1._02*mat_2._20,
            mat_1._00*mat_2._01 + mat_1._01*mat_2._11 + mat_1._02*mat_2._21,
            mat_1._00*mat_2._02 + mat_1._01*mat_2._12 + mat_1._02*mat_2._22,

            mat_1._10*mat_2._00 + mat_1._11*mat_2._10 + mat_1._12*mat_2._20,
            mat_1._10*mat_2._01 + mat_1._11*mat_2._11 + mat_1._12*mat_2._21,
            mat_1._10*mat_2._02 + mat_1._11*mat_2._12 + mat_1._12*mat_2._22,

            mat_1._20*mat_2._00 + mat_1._21*mat_2._10 + mat_1._22*mat_2._20,
            mat_1._20*mat_2._01 + mat_1._21*mat_2._11 + mat_1._22*mat_2._21,
            mat_1._20*mat_2._02 + mat_1._21*mat_2._12 + mat_1._22*mat_2._22);
}

//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f& operator *= (Matrix3f& mat_1, const Matrix3f& mat_2)
{
    float val0, val1, val2;

    val0 = mat_1._00*mat_2._00 + mat_1._01*mat_2._10 + mat_1._02*mat_2._20;
    val1 = mat_1._00*mat_2._01 + mat_1._01*mat_2._11 + mat_1._02*mat_2._21;
    val2 = mat_1._00*mat_2._02 + mat_1._01*mat_2._12 + mat_1._02*mat_2._22;
    mat_1._00 = val0; mat_1._01 = val1; mat_1._02 = val2;

    val0 = mat_1._10*mat_2._00 + mat_1._11*mat_2._10 + mat_1._12*mat_2._20;
    val1 = mat_1._10*mat_2._01 + mat_1._11*mat_2._11 + mat_1._12*mat_2._21;
    val2 = mat_1._10*mat_2._02 + mat_1._11*mat_2._12 + mat_1._12*mat_2._22;
    mat_1._10 = val0; mat_1._11 = val1; mat_1._12 = val2;

    val0 = mat_1._20*mat_2._00 + mat_1._21*mat_2._10 + mat_1._22*mat_2._20;
    val1 = mat_1._20*mat_2._01 + mat_1._21*mat_2._11 + mat_1._22*mat_2._21;
    val2 = mat_1._20*mat_2._02 + mat_1._21*mat_2._12 + mat_1._22*mat_2._22;
    mat_1._20 = val0; mat_1._21 = val1; mat_1._22 = val2;

    return mat_1;
}

//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f operator+(const Matrix3f& mat_1, const Matrix3f& mat_2)
{
    return Matrix3f(
            mat_1._00 + mat_2._00, mat_1._01 + mat_2._01, mat_1._02 + mat_2._02,
            mat_1._10 + mat_2._10, mat_1._11 + mat_2._11, mat_1._12 + mat_2._12,
            mat_1._20 + mat_2._20, mat_1._21 + mat_2._21, mat_1._22 + mat_2._22);
}

//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f& operator+=(Matrix3f& mat_1, const Matrix3f& mat_2)
{
    mat_1._00 += mat_2._00; mat_1._01 += mat_2._01; mat_1._02 += mat_2._02;
    mat_1._10 += mat_2._10; mat_1._11 += mat_2._11; mat_1._12 += mat_2._12;
    mat_1._20 += mat_2._20; mat_1._21 += mat_2._21; mat_1._22 += mat_2._22;

    return mat_1;
}

//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f operator - (const Matrix3f& mat_1, const Matrix3f& mat_2)
{
    return Matrix3f(
            mat_1._00 - mat_2._00, mat_1._01 - mat_2._01, mat_1._02 - mat_2._02,
            mat_1._10 - mat_2._10, mat_1._11 - mat_2._11, mat_1._12 - mat_2._12,
            mat_1._20 - mat_2._20, mat_1._21 - mat_2._21, mat_1._22 - mat_2._22);
}

//-----------------------------------------------------------------------------

MATFUNCINLINE Matrix3f& operator-=(Matrix3f& mat_1, const Matrix3f& mat_2)
{
    mat_1._00 -= mat_2._00; mat_1._01 -= mat_2._01; mat_1._02 -= mat_2._02;
    mat_1._10 -= mat_2._10; mat_1._11 -= mat_2._11; mat_1._12 -= mat_2._12;
    mat_1._20 -= mat_2._20; mat_1._21 -= mat_2._21; mat_1._22 -= mat_2._22;

    return mat_1;
}

//Matrix3f-Scalar
//-----------------------------------------------------------------------------
/*
inline Matrix3f operator*(const Matrix3f& mat, const float a)
{
    return Matrix3f(
            mat._00 * a, mat._01 * a, mat._02 * a,
            mat._10 * a, mat._11 * a, mat._12 * a,
            mat._20 * a, mat._21 * a, mat._22 * a);
}

//-----------------------------------------------------------------------------

inline Matrix3f operator/(const Matrix3f& mat, float a)
{
    a = 1/a;
    return Matrix3f(
            mat._00 * a, mat._01 * a, mat._02 * a,
            mat._10 * a, mat._11 * a, mat._12 * a,
            mat._20 * a, mat._21 * a, mat._22 * a);
}*/

//Matrix3f-Vector3f
//-----------------------------------------------------------------------------
//post-multiplication

MATFUNCINLINE Vector3f operator*(const Matrix3f& mat, const Vector3f& vec)
{
    return Vector3f(
            mat._00 * vec._x + mat._01 * vec._y + mat._02 * vec._z,
            mat._10 * vec._x + mat._11 * vec._y + mat._12 * vec._z,
            mat._20 * vec._x + mat._21 * vec._y + mat._22 * vec._z);
}

//-----------------------------------------------------------------------------
//pre-multiplication

MATFUNCINLINE Vector3f operator*(const Vector3f& vec, const Matrix3f& mat)
{
    return Vector3f(vec._x*mat._00 + vec._y*mat._10 + vec._z*mat._20,
            vec._x*mat._01 + vec._y*mat._11 + vec._z*mat._21,
            vec._x*mat._02 + vec._y*mat._12 + vec._z*mat._22);
}

//-----------------------------------------------------------------------------

MATINLINE Matrix4f Matrix3f::toMatrix4() const
{
    return Matrix4f(
            _00, _01, _02, 0,
            _10, _11, _12, 0,
            _20, _21, _22, 0,
            0, 0, 0, 1);
}


} //namespace Core

