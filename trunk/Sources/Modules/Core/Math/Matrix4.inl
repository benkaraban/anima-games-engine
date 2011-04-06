#include <Core/Math/Quaternion.h>
#include <Core/Math/Operation.h>

namespace Core
{

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f()
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f()\n";
#endif
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(const Matrix4f & mat)
{
    memcpy(_val, mat._val, sizeof(Matrix4f));
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(MatAxis_t axis,
                                                const Vector3f& vx, const Vector3f& vy,
                                                const Vector3f& vz, const Vector3f& vt)
    : _Xx(vx._x), _Yx(vy._x), _Zx(vz._x), _Tx(vt._x),
      _Xy(vx._y), _Yy(vy._y), _Zy(vz._y), _Ty(vt._y),
      _Xz(vx._z), _Yz(vy._z), _Zz(vz._z), _Tz(vt._z),
      _Xw(  0.0), _Yw(  0.0), _Zw(  0.0), _Tw(  1.0)
{}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(MatAxis_t axis,
                                                const Vector4f& vx, const Vector4f& vy,
                                                const Vector4f& vz, const Vector4f& vt)
    : _Xx(vx._x), _Yx(vy._x), _Zx(vz._x), _Tx(vt._x),
      _Xy(vx._y), _Yy(vy._y), _Zy(vz._y), _Ty(vt._y),
      _Xz(vx._z), _Yz(vy._z), _Zz(vz._z), _Tz(vt._z),
      _Xw(vx._w), _Yw(vy._w), _Zw(vz._w), _Tw(vt._w)
{}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(MatLines_t tline,
                                                const Vector4f& line_0, const Vector4f& line_1,
                                                const Vector4f& line_2, const Vector4f& line_3)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f(const Vector4f& line_0, const Vector4f& line_1, const Vector4f& line_2, const Vector4f& line_3)\n";
#endif
    memcpy(_line[0], line_0.data(), sizeof(Vector4f));
    memcpy(_line[1], line_1.data(), sizeof(Vector4f));
    memcpy(_line[2], line_2.data(), sizeof(Vector4f));
    memcpy(_line[3], line_3.data(), sizeof(Vector4f));
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(const float val_00, const float val_01, const float val_02, const float val_03,
                                                const float val_10, const float val_11, const float val_12, const float val_13,
                                                const float val_20, const float val_21, const float val_22, const float val_23,
                                                const float val_30, const float val_31, const float val_32, const float val_33) :
    _00(val_00), _01(val_01), _02(val_02), _03(val_03), _10(val_10), _11(val_11), _12(val_12), _13(val_13),
            _20(val_20), _21(val_21), _22(val_22), _23(val_23), _30(val_30), _31(val_31), _32(val_32), _33(val_33)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f(const float * 16)\n";
#endif
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Identity_t) :
    _00(1), _01(0), _02(0), _03(0), _10(0), _11(1), _12(0), _13(0), _20(0), _21(0), _22(1), _23(0), _30(0), _31(0),
            _32(0), _33(1)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f(const float * 16)\n";
#endif
}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Zero_t) :
    _00(0), _01(0), _02(0), _03(0), _10(0), _11(0), _12(0), _13(0), _20(0), _21(0), _22(0), _23(0), _30(0), _31(0),
            _32(0), _33(0)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f(const float * 16)\n";
#endif
}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Translation_t, const Vector3f & trans)
    : _Xx(1.0), _Yx(0.0), _Zx(0.0), _Tx(trans._x),
      _Xy(0.0), _Yy(1.0), _Zy(0.0), _Ty(trans._y),
      _Xz(0.0), _Yz(0.0), _Zz(1.0), _Tz(trans._z),
      _Xw(0.0), _Yw(0.0), _Zw(0.0), _Tw(1.0)
{}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Translation_t, const float x, const float y, const float z)
    : _Xx(1.0), _Yx(0.0), _Zx(0.0), _Tx(  x),
      _Xy(0.0), _Yy(1.0), _Zy(0.0), _Ty(  y),
      _Xz(0.0), _Yz(0.0), _Zz(1.0), _Tz(  z),
      _Xw(0.0), _Yw(0.0), _Zw(0.0), _Tw(1.0)
{}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Scale_t, const Vector3f & s)
    : _Xx(s._x), _Yx( 0.0), _Zx( 0.0), _Tx( 0.0),
      _Xy( 0.0), _Yy(s._y), _Zy( 0.0), _Ty( 0.0),
      _Xz( 0.0), _Yz( 0.0), _Zz(s._z), _Tz( 0.0),
      _Xw( 0.0), _Yw( 0.0), _Zw( 0.0), _Tw( 1.0)
{}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Scale_t, const float x, const float y, const float z)
    : _Xx(   x), _Yx( 0.0), _Zx( 0.0), _Tx( 0.0),
      _Xy( 0.0), _Yy(   y), _Zy( 0.0), _Ty( 0.0),
      _Xz( 0.0), _Yz( 0.0), _Zz(   z), _Tz( 0.0),
      _Xw( 0.0), _Yw( 0.0), _Zw( 0.0), _Tw( 1.0)
{}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Rotation_t, const float angle, ERotationAxis axis)
{
    switch(axis)
    {
    case ROT_X_PITCH: setRotationPitch(angle); break;
    case ROT_Y_YAW:   setRotationYaw(angle);   break;
    case ROT_Z_ROLL:  setRotationRoll(angle);  break;
    }
    _Tx = _Ty = _Tz = 0.0;
    _Xw = _Yw = _Zw = 0.0;
    _Tw = 1.0;
}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Rotation_t, const float angle, const Vector3f & axis)
{
    setRotation(angle, axis._x, axis._y, axis._z);
    _Tx = _Ty = _Tz = 0.0;
    _Xw = _Yw = _Zw = 0.0;
    _Tw = 1.0;
}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(Rotation_t, const float angle, const float x, const float y, const float z)
{
    setRotation(angle, x, y, z);
    _Tx = _Ty = _Tz = 0.0;
    _Xw = _Yw = _Zw = 0.0;
    _Tw = 1.0;
}
//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(const Matrix3f& rotation, const Vector3f& translation)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Matrix4f(const Matrix3f& rotation, const Vector3f& translation)\n";
#endif
    _00 = rotation._00;
    _01 = rotation._01;
    _02 = rotation._02;
    _03 = translation[0];

    _10 = rotation._10;
    _11 = rotation._11;
    _12 = rotation._12;
    _13 = translation[1];

    _20 = rotation._20;
    _21 = rotation._21;
    _22 = rotation._22;
    _23 = translation[2];

    _30 = 0;
    _31 = 0;
    _32 = 0;
    _33 = 1;
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(MatZVec_t, const Vector3f & zvec, const Vector3f & up)
{
    setZVec(zvec, up);
}

//-----------------------------------------------------------------------------
MATINLINE Matrix4f::Matrix4f(const Quaternionf& quat)
{
    *this = quat.toMatrix4();
}

//-----------------------------------------------------------------------------
MATINLINE void Matrix4f::setQT(const Quaternionf& quat, const Vector3f & trans)
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = quat._v._x + quat._v._x; y2 = quat._v._y + quat._v._y;
    z2 = quat._v._z + quat._v._z;
    xx = quat._v._x * x2; xy = quat._v._x * y2; xz = quat._v._x * z2;
    yy = quat._v._y * y2; yz = quat._v._y * z2; zz = quat._v._z * z2;
    wx = quat._w * x2; wy = quat._w * y2; wz = quat._w * z2;

    _00 = 1.0f - (yy + zz);
    _01 = xy - wz;
    _02 = xz + wy;
    _03 = trans.x;

    _10 = xy + wz;
    _11 = 1.0f - (xx + zz);
    _12 = yz - wx;
    _13 = trans.y;

    _20 = xz - wy;
    _21 = yz + wx;
    _22 = 1.0f - (xx + yy);
    _23 = trans.z;

    _30 = 0.0f;
    _31 = 0.0f;
    _32 = 0.0f;
    _33 = 1.0f;
}

//-----------------------------------------------------------------------------
MATINLINE void Matrix4f::setTransposeQT(const Quaternionf& quat, const Vector3f & trans)
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = quat._v._x + quat._v._x; y2 = quat._v._y + quat._v._y;
    z2 = quat._v._z + quat._v._z;
    xx = quat._v._x * x2; xy = quat._v._x * y2; xz = quat._v._x * z2;
    yy = quat._v._y * y2; yz = quat._v._y * z2; zz = quat._v._z * z2;
    wx = quat._w * x2; wy = quat._w * y2; wz = quat._w * z2;

    _00 = 1.0f - (yy + zz);
    _10 = xy - wz;
    _20 = xz + wy;
    _30 = trans.x;

    _01 = xy + wz;
    _11 = 1.0f - (xx + zz);
    _21 = yz - wx;
    _31 = trans.y;

    _02 = xz - wy;
    _12 = yz + wx;
    _22 = 1.0f - (xx + yy);
    _32 = trans.z;

    _03 = 0.0f;
    _13 = 0.0f;
    _23 = 0.0f;
    _33 = 1.0f;
}

//-----------------------------------------------------------------------------
MATINLINE const Vector4f& Matrix4f::operator[](const int index) const
{
    return reinterpret_cast<const Vector4f*>(_line)[index];
}

//-----------------------------------------------------------------------------

MATINLINE Vector4f &Matrix4f::operator[](const int index)
{
    return reinterpret_cast<Vector4f*>(_line)[index];
}

//-----------------------------------------------------------------------------

MATINLINE const float* Matrix4f::data() const
{
    return &_00;
}

//-----------------------------------------------------------------------------

MATINLINE float* Matrix4f::data()
{
    return &_00;
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix4f::operator==(const Matrix4f& mat) const
{
    return (_00 == mat._00 && _01 == mat._01 && _02 == mat._02 && _03 == mat._03 &&
            _10 == mat._10 && _11 == mat._11 && _12 == mat._12 && _13 == mat._13 &&
            _20 == mat._20 && _21 == mat._21 && _22 == mat._22 && _23 == mat._23 &&
            _30 == mat._30 && _31 == mat._31 && _32 == mat._32 && _33 == mat._33);
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix4f::operator!=(const Matrix4f& mat) const
{
    return !(_00 == mat._00 && _01 == mat._01 && _02 == mat._02 && _03 == mat._03 &&
            _10 == mat._10 && _11 == mat._11 && _12 == mat._12 && _13 == mat._13 &&
            _20 == mat._20 && _21 == mat._21 && _22 == mat._22 && _23 == mat._23 &&
            _30 == mat._30 && _31 == mat._31 && _32 == mat._32 && _33 == mat._33);
}

//-----------------------------------------------------------------------------

MATINLINE void Matrix4f::set(const float M00, const float M01, const float M02, const float M03,
        const float M10, const float M11, const float M12, const float M13,
        const float M20, const float M21, const float M22, const float M23,
        const float M30, const float M31, const float M32, const float M33)
{
    _00 = M00;
    _01 = M01;
    _02 = M02;
    _03 = M03;

    _10 = M10;
    _11 = M11;
    _12 = M12;
    _13 = M13;

    _20 = M20;
    _21 = M21;
    _22 = M22;
    _23 = M23;

    _30 = M30;
    _31 = M31;
    _32 = M32;
    _33 = M33;
}

//-----------------------------------------------------------------------------

MATINLINE bool Matrix4f::isEquivalent(const Matrix4f& mat, const float epsilon) const
{
    return(
            L_abs(_00 - mat._00)<=epsilon && L_abs(_01 - mat._01)<=epsilon && L_abs(_02 - mat._02)<=epsilon && L_abs(_03 - mat._03)<=epsilon &&
            L_abs(_10 - mat._10)<=epsilon && L_abs(_11 - mat._11)<=epsilon && L_abs(_12 - mat._12)<=epsilon && L_abs(_13 - mat._13)<=epsilon &&
            L_abs(_20 - mat._20)<=epsilon && L_abs(_21 - mat._21)<=epsilon && L_abs(_22 - mat._22)<=epsilon && L_abs(_23 - mat._23)<=epsilon &&
            L_abs(_30 - mat._30)<=epsilon && L_abs(_31 - mat._31)<=epsilon && L_abs(_32 - mat._32)<=epsilon && L_abs(_33 - mat._33)<=epsilon);
}

//-----------------------------------------------------------------------------

MATINLINE void Matrix4f::setZero()
{
    memset(_line, 0, sizeof(Matrix4f));
}

//-----------------------------------------------------------------------------

MATINLINE void Matrix4f::setIdentity()
{
    _00 = 1;   _10 = 0;   _20 = 0;   _30 = 0;
    _01 = 0;   _11 = 1;   _21 = 0;   _31 = 0;
    _02 = 0;   _12 = 0;   _22 = 1;   _32 = 0;
    _03 = 0;   _13 = 0;   _23 = 0;   _33 = 1;
}


//-----------------------------------------------------------------------------

MATINLINE Matrix4f Matrix4f::getInverted() const
{
    Matrix4f inverted=*this;
    inverted.inverseSelf();
    return inverted;
}


//-----------------------------------------------------------------------------
MATINLINE Matrix4f Matrix4f::getTransposed() const
{
    Matrix4f transpose;

    transpose._00=_00; transpose._01=_10; transpose._02=_20; transpose._03=_30;
    transpose._10=_01; transpose._11=_11; transpose._12=_21; transpose._13=_31;
    transpose._20=_02; transpose._21=_12; transpose._22=_22; transpose._23=_32;
    transpose._30=_03; transpose._31=_13; transpose._32=_23; transpose._33=_33;

    return transpose;
}

//-----------------------------------------------------------------------------
MATINLINE void Matrix4f::transposeSelf()
{
    float value;

    value = _01; _01 = _10; _10 = value;
    value = _02; _02 = _20; _20 = value;
    value = _03; _03 = _30; _30 = value;

    value = _12; _12 = _21; _21 = value;
    value = _13; _13 = _31; _31 = value;

    value = _23; _23 = _32; _32 = value;
}

//-----------------------------------------------------------------------------

MATINLINE void Matrix4f::switchHand()
{
    _20 = -_20;
    _21 = -_21;
    _22 = -_22;
    _23 = -_23;
}

//-----------------------------------------------------------------------------

MATINLINE Matrix4f Matrix4f::operator*(const float a) const
{
    return Matrix4f(
            _00 * a, _01 * a, _02 * a, _03 * a,
            _10 * a, _11 * a, _12 * a, _13 * a,
            _20 * a, _21 * a, _22 * a, _23 * a,
            _30 * a, _31 * a, _32 * a, _33 * a);
}

//-----------------------------------------------------------------------------

MATINLINE Matrix4f Matrix4f::operator/(float a) const
{
    a = 1/a;
    return Matrix4f(
            _00 * a, _01 * a, _02 * a, _03 * a,
            _10 * a, _11 * a, _12 * a, _13 * a,
            _20 * a, _21 * a, _22 * a, _23 * a,
            _30 * a, _31 * a, _32 * a, _33 * a);
}

//-----------------------------------------------------------------------------

MATINLINE Matrix4f& Matrix4f::operator*=(const float a)
{
    _00 *= a; _01 *= a; _02 *= a; _03 *= a;
    _10 *= a; _11 *= a; _12 *= a; _13 *= a;
    _20 *= a; _21 *= a; _22 *= a; _23 *= a;
    _30 *= a; _31 *= a; _32 *= a; _33 *= a;

    return *this;
}

//-----------------------------------------------------------------------------

MATINLINE Matrix4f& Matrix4f::operator/=(float a)
{
    a = 1/a;

    _00 *= a; _01 *= a; _02 *= a; _03 *= a;
    _10 *= a; _11 *= a; _12 *= a; _13 *= a;
    _20 *= a; _21 *= a; _22 *= a; _23 *= a;
    _30 *= a; _31 *= a; _32 *= a; _33 *= a;

    return *this;
}

//other* *this




MATINLINE Vector3f Matrix4f::getTranslation() const
{
    return Vector3f(_03, _13, _23);
}

MATINLINE Matrix3f Matrix4f::getRotation() const
{
    return Matrix3f( _00, _01, _02,
                     _10, _11, _12,
                     _20, _21, _22);
}

MATINLINE void Matrix4f::setTranslation(const Vector3f& vec)
{
    setIdentity();

    _03 = vec._x;
    _13 = vec._y;
    _23 = vec._z;
}



MATINLINE void Matrix4f::addTranslation(const Vector3f& vec)
{
    _03 += vec._x;
    _13 += vec._y;
    _23 += vec._z;
}


MATINLINE Vector3f Matrix4f::getScale() const
{
    Vector3f x_vec(_00,_01,_02);
    Vector3f y_vec(_10,_11,_12);
    Vector3f z_vec(_20,_21,_22);

    return Vector3f(x_vec.getLengthSafe(), y_vec.getLengthSafe(), z_vec.getLengthSafe());
}



MATINLINE void Matrix4f::setScale(const Vector3f& vec)
{
    setIdentity();

    _00 = vec._x;
    _11 = vec._y;
    _22 = vec._z;
}


MATINLINE void Matrix4f::setScale(const float val)
{
    setIdentity();

    _00 = val;
    _11 = val;
    _22 = val;
}


MATINLINE void Matrix4f::setOrthoProj(const float x1, const float x2, const float y1, const float y2, const float z1, const float z2)
{
    _00 = 2/(x2-x1); _01 = 0;         _02 = 0;         _03 = (x1+x2)/(x1-x2);
    _10 = 0;         _11 = 2/(y2-y1); _12 = 0;         _13 = (y1+y2)/(y1-y2);
    _20 = 0;         _21 = 0;         _22 = 1/(z2-z1); _23 = z1/(z1-z2);
    _30 = 0;         _31 = 0;         _32 = 0;         _33 = 1;
}


MATINLINE void Matrix4f::setOrthoProj2D(const float left, const float right, const float bottom, const float top)
{
    setOrthoProj(left, right ,bottom, top, -1.0, 1.0);
}


MATINLINE void Matrix4f::setFrustumProj(const float x1, const float x2, const float y1, const float y2, const float z1, const float z2)
{
    _00 = 2.0f*z1/(x2-x1);	_01 = 0;				_02 = -(x2+x1)/(x2-x1);    _03 = 0;
    _10 = 0;				_11 = 2.0f*z1/(y2-y1);	_12 = -(y2+y1)/(y2-y1);    _13 = 0;
    _20 = 0;				_21 = 0;				_22 = (z2+z1)/(z2-z1);     _23 = -2.0f*z2*z1/(z2-z1);
    _30 = 0;				_31 = 0;				_32 = 1;                   _33 = 0;
}

//fov en radian
MATINLINE void Matrix4f::setPerspectiveProj(const float fov, const float aspectRatio, const float myNear, const float myFar)
{
    float yScale = 1.f/L_tan(fov/2);
    float xScale = yScale / aspectRatio;

    _00 = xScale;   _01 = 0;        _02 = 0;                _03 = 0;
    _10 = 0;        _11 = yScale;   _12 = 0;                _13 = 0;
    _20 = 0;        _21 = 0;        _22 = myFar/(myFar-myNear);   _23 = myNear*myFar/(myNear-myFar);
    _30 = 0;        _31 = 0;        _32 = 1;                _33 = 0;
}


MATINLINE void Matrix4f::setLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up)
{
    Vector3f zaxis = center-eye; zaxis.normalizeSafe();
    Vector3f xaxis = cross(up, zaxis); xaxis.normalizeSafe();
    Vector3f yaxis = cross(zaxis, xaxis);

    _00 = xaxis._x;   _01 = yaxis._x;   _02 = zaxis._x;  _03 = eye._x;
    _10 = xaxis._y;   _11 = yaxis._y;   _12 = zaxis._y;  _13 = eye._y;
    _20 = xaxis._z;   _21 = yaxis._z;   _22 = zaxis._z;  _23 = eye._z;
    _30 = 0.0;        _31 = 0.0;        _32 = 0.0;       _33 = 1;
}


MATINLINE void Matrix4f::setRotation(const Orientationf& angle)
{
    float sinPitch, cosPitch;
    float sinYaw, cosYaw;
    float sinRoll, cosRoll;

    L_sincos (angle._pitch, &sinPitch, &cosPitch);
    L_sincos (angle._yaw, &sinYaw, &cosYaw);
    L_sincos (angle._roll, &sinRoll, &cosRoll);

    float sinYaw_cosRoll = (sinYaw*cosRoll);
    float sintYaw_sinRoll = (sinYaw*sinRoll);

    _00=cosYaw*cosRoll; _01=sinYaw_cosRoll*sinPitch-cosPitch*sinRoll; _02=sinYaw_cosRoll*cosPitch+sinPitch*sinRoll; _03=0;
    _10=cosYaw*sinRoll; _11=sintYaw_sinRoll*sinPitch+cosPitch*cosRoll; _12=sintYaw_sinRoll*cosPitch-sinPitch*cosRoll; _13=0;
    _20=-sinYaw; _21=cosYaw*sinPitch; _22=cosYaw*cosPitch; _23=0;
    _30=0; _31=0; _32=0; _33=1;
}


MATINLINE void Matrix4f::setRotationPitch(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00 = 1; _01 =      0; _02 =       0; _03 = 0;
    _10 = 0; _11 = cosVal; _12 = -sinVal; _13 = 0;
    _20 = 0; _21 = sinVal; _22 =  cosVal; _23 = 0;
    _30 = 0; _31 =      0; _32 =       0; _33 = 1;
}


MATINLINE void Matrix4f::setRotationYaw(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00 = cosVal; _01 =  0; _02 = sinVal; _03 = 0;
    _10 =      0; _11 =  1; _12 =      0; _13 = 0;
    _20 =-sinVal; _21 =  0; _22 = cosVal; _23 = 0;
    _30 =      0; _31 =  0; _32 =      0; _33 = 1;
}


MATINLINE void Matrix4f::setRotationRoll(const float rad)
{
    float sinVal, cosVal;
    L_sincos(rad,&sinVal, &cosVal);

    _00 = cosVal; _01 = -sinVal; _02 = 0; _03 = 0;
    _10 = sinVal; _11 =  cosVal; _12 = 0; _13 = 0;
    _20 =      0; _21 =       0; _22 = 1; _23 = 0;
    _30 =      0; _31 =       0; _32 = 0; _33 = 1;
}


MATINLINE void Matrix4f::setRotation(const float rad, const Vector3f & axis)
{
    setRotation(rad, axis._x, axis._y, axis._z);
}


MATINLINE void Matrix4f::setRotation(const float rad, const float x, const float y, const float z)
{
    float s, c;
    L_sincos(rad, &s, &c);
    float c1 = 1.0f - c;
    _Xx = x*x * c1 +   c;   _Yx = x*y * c1 - z*s;   _Zx = x*z * c1 + y*s;
    _Xy = y*x * c1 + z*s;   _Yy = y*y * c1 +   c;   _Zy = y*z * c1 - x*s;
    _Xz = z*x * c1 - y*s;   _Yz = z*y * c1 + x*s;   _Zz = z*z * c1 +   c;
}


MATINLINE Matrix4f Matrix4f::makeOrthoProj(const float left, const float right, const float bottom, const float top, const float myNear, const float myFar)
{
    Matrix4f mat;
    mat.setOrthoProj(left, right, bottom, top, myNear, myFar);
    return mat;
}


MATINLINE Matrix4f Matrix4f::makeScale(const Vector3f& vec)
{
    Matrix4f mat (IDENTITY);
    mat.setScale(vec);
    return mat;
}


MATINLINE Matrix4f Matrix4f::makeTranslate(const Vector3f& vec)
{
    Matrix4f mat (IDENTITY);
    mat.setTranslation(vec);
    return mat;
}

/**
 * fonction utilisée pour créer une matrice de projection orthogonale 2D
 */

MATINLINE Matrix4f Matrix4f::makeOrthoProj2D(const float left, const float right, const float bottom, const float top)
{
    Matrix4f mat;
    mat.setOrthoProj2D(left, right, bottom, top);
    return mat;
}

/**
 * fonction utilisée pour créer une matrice de projection en perspective
 */

MATINLINE Matrix4f Matrix4f::makeFrustumProj(const float left, const float right, const float bottom, const float top, const float myNear, const float myFar)
{
    Matrix4f mat;
    mat.setFrustumProj(left, right, bottom, top, myNear, myFar);
    return mat;
}

/**
 * fonction utilisée pour créer une matrice de projection en perspective symetrique
 * aspectRatio doit etre definie par :  width/height.
 * fov :: en radian
 */

MATINLINE Matrix4f Matrix4f::makePerspectiveProj(const float fov, const float aspectRatio, const float myNear, const float myFar)
{
    Matrix4f mat;
    mat.setPerspectiveProj(fov, aspectRatio, myNear, myFar);
    return mat;
}

/**
 * fonction utilisée pour créer une matrice de projecttion en perspective à partir d'une position et d'une orientation
 */

MATINLINE Matrix4f Matrix4f::makeLookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up)
{
    Matrix4f mat;
    mat.setLookAt(eye, center, up);
    return mat;
}


MATINLINE bool Matrix4f::isValid() const
{/*
 return(
 isValid(_00) && isValid(_01) && isValid(_02) && isValid(_03) &&
 isValid(_10) && isValid(_11) && isValid(_12) && isValid(_13) &&
 isValid(_20) && isValid(_21) && isValid(_22) && isValid(_23) &&
 isValid(_30) && isValid(_31) && isValid(_32) && isValid(_33));*/
    return true;
}



MATINLINE Vector3f Matrix4f::apply(const Vector3f& vec) const
{
    Vector3f vecTransf;
    vecTransf._x = _00*vec._x + _01*vec._y + _02* vec._z + _03;
    vecTransf._y = _10*vec._x + _11*vec._y + _12* vec._z + _13;
    vecTransf._z = _20*vec._x + _21*vec._y + _22* vec._z + _23;
    return vecTransf;
}

MATINLINE Vector4f Matrix4f::apply(const Vector4f& vec) const
{
    Vector4f vecTransf;
    vecTransf._x = _00*vec._x + _01*vec._y + _02* vec._z + _03* vec._w;
    vecTransf._y = _10*vec._x + _11*vec._y + _12* vec._z + _13* vec._w;
    vecTransf._z = _20*vec._x + _21*vec._y + _22* vec._z + _23* vec._w;
    vecTransf._w = _30*vec._x + _31*vec._y + _32* vec._z + _33* vec._w;
    return vecTransf;
}

MATINLINE Vector3f Matrix4f::apply3x3(const Vector3f& vec) const
{
    return Vector3f(
            _00*vec._x + _01*vec._y + _02* vec._z,
            _10*vec._x + _11*vec._y + _12* vec._z,
            _20*vec._x + _21*vec._y + _22* vec._z
    );
}


MATINLINE Matrix4f Matrix4f::makeInverse(const Matrix4f& mat)
{
    Matrix4f mat_res;
    return mat_res;
}

MATINLINE Matrix4f Matrix4f::makeOrthoNormal(const Matrix4f& mat)
{
    Matrix4f mat_res;
    return mat_res;
}


MATINLINE Matrix4f Matrix4f::makeRotation(const Orientationf& angle)
{
    Matrix4f mat;
    mat.setRotation(angle);
    return mat;
}


MATINLINE Matrix4f Matrix4f::makeRotationPitch(const float rad)
{
    Matrix4f mat;
    mat.setRotationPitch(rad);
    return mat;
}


MATINLINE Matrix4f Matrix4f::makeRotationYaw(const float rad)
{
    Matrix4f mat;
    mat.setRotationYaw(rad);
    return mat;
}


MATINLINE Matrix4f Matrix4f::makeRotationRoll(const float rad)
{
    Matrix4f mat;
    mat.setRotationRoll(rad);
    return mat;
}


//Matrix4f-Matrix4f

MATFUNCINLINE Matrix4f operator * (const Matrix4f& mat_1, const Matrix4f& mat_2)
{
    return Matrix4f(
            mat_1._00*mat_2._00 + mat_1._01*mat_2._10 + mat_1._02*mat_2._20 + mat_1._03*mat_2._30,
            mat_1._00*mat_2._01 + mat_1._01*mat_2._11 + mat_1._02*mat_2._21 + mat_1._03*mat_2._31,
            mat_1._00*mat_2._02 + mat_1._01*mat_2._12 + mat_1._02*mat_2._22 + mat_1._03*mat_2._32,
            mat_1._00*mat_2._03 + mat_1._01*mat_2._13 + mat_1._02*mat_2._23 + mat_1._03*mat_2._33,

            mat_1._10*mat_2._00 + mat_1._11*mat_2._10 + mat_1._12*mat_2._20 + mat_1._13*mat_2._30,
            mat_1._10*mat_2._01 + mat_1._11*mat_2._11 + mat_1._12*mat_2._21 + mat_1._13*mat_2._31,
            mat_1._10*mat_2._02 + mat_1._11*mat_2._12 + mat_1._12*mat_2._22 + mat_1._13*mat_2._32,
            mat_1._10*mat_2._03 + mat_1._11*mat_2._13 + mat_1._12*mat_2._23 + mat_1._13*mat_2._33,

            mat_1._20*mat_2._00 + mat_1._21*mat_2._10 + mat_1._22*mat_2._20 + mat_1._23*mat_2._30,
            mat_1._20*mat_2._01 + mat_1._21*mat_2._11 + mat_1._22*mat_2._21 + mat_1._23*mat_2._31,
            mat_1._20*mat_2._02 + mat_1._21*mat_2._12 + mat_1._22*mat_2._22 + mat_1._23*mat_2._32,
            mat_1._20*mat_2._03 + mat_1._21*mat_2._13 + mat_1._22*mat_2._23 + mat_1._23*mat_2._33,

            mat_1._30*mat_2._00 + mat_1._31*mat_2._10 + mat_1._32*mat_2._20 + mat_1._33*mat_2._30,
            mat_1._30*mat_2._01 + mat_1._31*mat_2._11 + mat_1._32*mat_2._21 + mat_1._33*mat_2._31,
            mat_1._30*mat_2._02 + mat_1._31*mat_2._12 + mat_1._32*mat_2._22 + mat_1._33*mat_2._32,
            mat_1._30*mat_2._03 + mat_1._31*mat_2._13 + mat_1._32*mat_2._23 + mat_1._33*mat_2._33);
}


MATFUNCINLINE Matrix4f& operator *= (Matrix4f& mat_1, const Matrix4f& mat_2)
{
    float val0, val1, val2, val3;

    val0 = mat_1._00*mat_2._00 + mat_1._01*mat_2._10 + mat_1._02*mat_2._20 + mat_1._03*mat_2._30;
    val1 = mat_1._00*mat_2._01 + mat_1._01*mat_2._11 + mat_1._02*mat_2._21 + mat_1._03*mat_2._31;
    val2 = mat_1._00*mat_2._02 + mat_1._01*mat_2._12 + mat_1._02*mat_2._22 + mat_1._03*mat_2._32;
    val3 = mat_1._00*mat_2._03 + mat_1._01*mat_2._13 + mat_1._02*mat_2._23 + mat_1._03*mat_2._33;
    mat_1._00 = val0; mat_1._01 = val1; mat_1._02 = val2; mat_1._03 = val3;

    val0 = mat_1._10*mat_2._00 + mat_1._11*mat_2._10 + mat_1._12*mat_2._20 + mat_1._13*mat_2._30;
    val1 = mat_1._10*mat_2._01 + mat_1._11*mat_2._11 + mat_1._12*mat_2._21 + mat_1._13*mat_2._31;
    val2 = mat_1._10*mat_2._02 + mat_1._11*mat_2._12 + mat_1._12*mat_2._22 + mat_1._13*mat_2._32;
    val3 = mat_1._10*mat_2._03 + mat_1._11*mat_2._13 + mat_1._12*mat_2._23 + mat_1._13*mat_2._33;
    mat_1._10 = val0; mat_1._11 = val1; mat_1._12 = val2; mat_1._13 = val3;

    val0 = mat_1._20*mat_2._00 + mat_1._21*mat_2._10 + mat_1._22*mat_2._20 + mat_1._23*mat_2._30;
    val1 = mat_1._20*mat_2._01 + mat_1._21*mat_2._11 + mat_1._22*mat_2._21 + mat_1._23*mat_2._31;
    val2 = mat_1._20*mat_2._02 + mat_1._21*mat_2._12 + mat_1._22*mat_2._22 + mat_1._23*mat_2._32;
    val3 = mat_1._20*mat_2._03 + mat_1._21*mat_2._13 + mat_1._22*mat_2._23 + mat_1._23*mat_2._33;
    mat_1._20 = val0; mat_1._21 = val1; mat_1._22 = val2; mat_1._23 = val3;

    val0 = mat_1._30*mat_2._00 + mat_1._31*mat_2._10 + mat_1._32*mat_2._20 + mat_1._33*mat_2._30;
    val1 = mat_1._30*mat_2._01 + mat_1._31*mat_2._11 + mat_1._32*mat_2._21 + mat_1._33*mat_2._31;
    val2 = mat_1._30*mat_2._02 + mat_1._31*mat_2._12 + mat_1._32*mat_2._22 + mat_1._33*mat_2._32;
    val3 = mat_1._30*mat_2._03 + mat_1._31*mat_2._13 + mat_1._32*mat_2._23 + mat_1._33*mat_2._33;
    mat_1._30 = val0; mat_1._31 = val1; mat_1._32 = val2; mat_1._33 = val3;

    return mat_1;
}


MATFUNCINLINE Matrix4f operator+(const Matrix4f& mat_1, const Matrix4f& mat_2)
{
    return Matrix4f(
            mat_1._00 + mat_2._00, mat_1._01 + mat_2._01, mat_1._02 + mat_2._02, mat_1._03 + mat_2._03,
            mat_1._10 + mat_2._10, mat_1._11 + mat_2._11, mat_1._12 + mat_2._12, mat_1._13 + mat_2._13,
            mat_1._20 + mat_2._20, mat_1._21 + mat_2._21, mat_1._22 + mat_2._22, mat_1._23 + mat_2._23,
            mat_1._30 + mat_2._30, mat_1._31 + mat_2._31, mat_1._32 + mat_2._32, mat_1._33 + mat_2._33);
}


MATFUNCINLINE Matrix4f& operator+=(Matrix4f& mat_1, const Matrix4f& mat_2)
{
    mat_1._00 += mat_2._00; mat_1._01 += mat_2._01; mat_1._02 += mat_2._02; mat_1._03 += mat_2._03;
    mat_1._10 += mat_2._10; mat_1._11 += mat_2._11; mat_1._12 += mat_2._12; mat_1._13 += mat_2._13;
    mat_1._20 += mat_2._20; mat_1._21 += mat_2._21; mat_1._22 += mat_2._22; mat_1._23 += mat_2._23;
    mat_1._30 += mat_2._30; mat_1._31 += mat_2._31; mat_1._32 += mat_2._32; mat_1._33 += mat_2._33;

    return mat_1;
}


MATFUNCINLINE Matrix4f operator - (const Matrix4f& mat_1, const Matrix4f& mat_2)
{
    return Matrix4f(
            mat_1._00 - mat_2._00, mat_1._01 - mat_2._01, mat_1._02 - mat_2._02, mat_1._03 - mat_2._03,
            mat_1._10 - mat_2._10, mat_1._11 - mat_2._11, mat_1._12 - mat_2._12, mat_1._13 - mat_2._13,
            mat_1._20 - mat_2._20, mat_1._21 - mat_2._21, mat_1._22 - mat_2._22, mat_1._23 - mat_2._23,
            mat_1._30 - mat_2._30, mat_1._31 - mat_2._31, mat_1._32 - mat_2._32, mat_1._33 - mat_2._33);
}


MATFUNCINLINE Matrix4f& operator-=(Matrix4f& mat_1, const Matrix4f& mat_2)
{
    mat_1._00 -= mat_2._00; mat_1._01 -= mat_2._01; mat_1._02 -= mat_2._02; mat_1._03 -= mat_2._03;
    mat_1._10 -= mat_2._10; mat_1._11 -= mat_2._11; mat_1._12 -= mat_2._12; mat_1._13 -= mat_2._13;
    mat_1._20 -= mat_2._20; mat_1._21 -= mat_2._21; mat_1._22 -= mat_2._22; mat_1._23 -= mat_2._23;
    mat_1._30 -= mat_2._30; mat_1._31 -= mat_2._31; mat_1._32 -= mat_2._32; mat_1._33 -= mat_2._33;

    return mat_1;
}

//Matrix4f-Scalar
/* KILLME

inline Matrix4f operator*(const Matrix4f& mat, const float a)
{
    return Matrix4f(
            mat._00 * a, mat._01 * a, mat._02 * a, mat._03 * a,
            mat._10 * a, mat._11 * a, mat._12 * a, mat._13 * a,
            mat._20 * a, mat._21 * a, mat._22 * a, mat._23 * a,
            mat._30 * a, mat._31 * a, mat._32 * a, mat._33 * a);
}


inline Matrix4f operator/(const Matrix4f& mat, float a)
{
    a = 1/a;
    return Matrix4f(
            mat._00 * a, mat._01 * a, mat._02 * a, mat._03 * a,
            mat._10 * a, mat._11 * a, mat._12 * a, mat._13 * a,
            mat._20 * a, mat._21 * a, mat._22 * a, mat._23 * a,
            mat._30 * a, mat._31 * a, mat._32 * a, mat._33 * a);
}
*/
//Matrix4f-Vector4f

MATFUNCINLINE Vector4f operator*(const Matrix4f& mat, const Vector4f& vec)
{
    return Vector4f(
            mat._00 * vec._x + mat._01 * vec._y + mat._02 * vec._z + mat._03 * vec._w,
            mat._10 * vec._x + mat._11 * vec._y + mat._12 * vec._z + mat._13 * vec._w,
            mat._20 * vec._x + mat._21 * vec._y + mat._22 * vec._z + mat._23 * vec._w,
            mat._30 * vec._x + mat._31 * vec._y + mat._32 * vec._z + mat._33 * vec._w);
}

MATFUNCINLINE Vector3f operator*(const Matrix4f& mat, const Vector3f& vec)
{
    return Vector3f(
            mat._00 * vec._x + mat._10 * vec._y + mat._20 * vec._z + mat._30,
            mat._01 * vec._x + mat._11 * vec._y + mat._21 * vec._z + mat._31,
            mat._02 * vec._x + mat._12 * vec._y + mat._22 * vec._z + mat._32);
}


MATFUNCINLINE Vector4f operator*(const Vector4f& vec, const Matrix4f& mat)
{
    return Vector4f(
            mat._00 * vec._x + mat._10 * vec._y + mat._20 * vec._z + mat._30 * vec._w,
            mat._01 * vec._x + mat._11 * vec._y + mat._21 * vec._z + mat._31 * vec._w,
            mat._02 * vec._x + mat._12 * vec._y + mat._22 * vec._z + mat._32 * vec._w,
            mat._03 * vec._x + mat._13 * vec._y + mat._23 * vec._z + mat._33 * vec._w);
}

MATFUNCINLINE Vector3f operator*(const Vector3f& vec, const Matrix4f& mat)
{
    return Vector3f(
            mat._00 * vec._x + mat._10 * vec._y + mat._20 * vec._z + mat._30,
            mat._01 * vec._x + mat._11 * vec._y + mat._21 * vec._z + mat._31,
            mat._02 * vec._x + mat._12 * vec._y + mat._22 * vec._z + mat._32);
}

} //namespace Core

