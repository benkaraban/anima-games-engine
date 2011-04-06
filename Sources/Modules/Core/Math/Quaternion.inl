#ifndef CORE_QUATERNION_INL_
#define CORE_QUATERNION_INL_

namespace Core
{
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf()
{
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const float x, const float y, const float z, const float w) :
    _v(x, y, z), _w(w)
{

}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const Quaternionf& other) :
    _v(other._v), _w(other._w)
{
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Identity_t) :
    _v(0, 0, 0), _w(1)
{
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Zero_t) :
    _v(0, 0, 0), _w(0)
{
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const Orientationf& Orientation)
{
    setRotation(Orientation);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const float w, const Vector3f& v) :
    _v(v), _w(w)
{
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const float pitch, const float yaw, const float roll)
{
    setRotation(pitch, yaw, roll);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Pitch_t, float angle)
{
    setRotationPitch(angle);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Yaw_t, float angle)
{
    setRotationYaw(angle);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Roll_t, float angle)
{
    setRotationRoll(angle);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(Axis_t, const Vector3f& axis, float angle)
{
    setRotation(axis, angle);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const Matrix3f& mat)
{
    (*this) = matrixToQuaternion(mat);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf::Quaternionf(const Quaternionf& from, const Quaternionf& to, const float amount)
{
    setSlerp(from, to, amount);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::set(const Vector3f& axis, const float Orientation)
{
    _v = axis;
    _w = Orientation;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::set(const float x, const float y, const float z, const float w)
{
    _v.set(x, y, z);
    _w = w;
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::operator==(const Quaternionf& other) const
{
    return (_v == other._v && _w == other._w);
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::operator!=(const Quaternionf& other) const
{
    return !(_v == other._v && _w == other._w);
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::isEquivalent(const Quaternionf& other, const float epsilon) const
{
    if (L_abs(_v._x - other._v._x)> epsilon ||
            L_abs(_v._y - other._v._y)> epsilon ||
            L_abs(_v._z - other._v._z)> epsilon ||
            L_abs(_w - other._w)> epsilon)
    return false;

    return true;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator-() const
{
    return Quaternionf(-_w, -_v);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf& Quaternionf::operator*=(const float a)
{
    _v *= a;
    _w *= a;

    return *this;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf& Quaternionf::operator/=(const float a)
{
    _v /= a;
    _w /= a;

    return *this;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator* (const float a) const
{
    return Quaternionf(_w*a, _v*a);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator/ (const float a) const
{
    return Quaternionf(_w/a, _v/a);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setIdentity ()
{
    _v._x=0; _v._y=0; _v._z=0; _w=1;
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::isIdentity (const float epsilon) const
{
    return ((L_abs(_v._x) <= epsilon) &&
            (L_abs(_v._y) <= epsilon) &&
            (L_abs(_v._z) <= epsilon) &&
            (L_abs(_w-1) <= epsilon));
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::isUnit (const float epsilon) const
{
    return L_abs(1 - dot((*this), (*this))) < epsilon;
}
//-----------------------------------------------------------------------------
QUATINLINE bool Quaternionf::isZero (const float epsilon) const
{
    return _v.isZero(epsilon) && (L_abs(_w) < epsilon);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::getInverted () const
{
    return Quaternionf (_w, -_v);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::inverseSelf ()
{
    _v._x = -_v._x;
    _v._y = -_v._y;
    _v._z = -_v._z;
}
//-----------------------------------------------------------------------------
QUATINLINE float Quaternionf::getlength () const
{
    return L_sqrt(_w*_w + _v._x*_v._x+_v._y*_v._y+_v._z*_v._z);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::normalizeSafe ()
{
    float val = L_sqrt(_w*_w + _v._x*_v._x+_v._y*_v._y+_v._z*_v._z);
    val=1/val;
    _w*=val; _v._x*=val; _v._y*=val; _v._z*=val;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::normalizeFast ()
{
    float val = L_sqrt(_w*_w + _v._x*_v._x+_v._y*_v._y+_v._z*_v._z);
    val=1/val;
    _w*=val; _v._x*=val; _v._y*=val; _v._z*=val;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::getNormalizedSafe () const
{
    float val = L_sqrt(_w*_w + _v._x*_v._x+_v._y*_v._y+_v._z*_v._z);
    val=1/val;
    return Quaternionf (_w*val, _v*val);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::getNormalizedFast () const
{
    float val = L_sqrt(_w*_w + _v._x*_v._x+_v._y*_v._y+_v._z*_v._z);
    val=1/val;
    return Quaternionf (_w*val, _v*val);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotationPitch (const float rad)
{
    float sinVal, cosVal;
    L_sincos(float(rad*0.5),&sinVal,&cosVal);
    _w=cosVal;
    _v._x=sinVal;
    _v._y=0;
    _v._z=0;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotationYaw (const float rad)
{
    float sinVal,cosVal;
    L_sincos(float(rad*0.5),&sinVal,&cosVal);
    _w=cosVal;
    _v._x=0;
    _v._y=sinVal;
    _v._z=0;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotationRoll (const float rad)
{
    float sinVal,cosVal;
    L_sincos(float(rad*0.5),&sinVal,&cosVal);
    _w=cosVal;
    _v._x=0;
    _v._y=0;
    _v._z=sinVal;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotation (const float rad_x, const float rad_y, const float rad_z)
{
    float sx,cx; L_sincos(float(rad_x*0.5),&sx,&cx);
    float sy,cy; L_sincos(float(rad_y*0.5),&sy,&cy);
    float sz,cz; L_sincos(float(rad_z*0.5),&sz,&cz);

    _w = cx*cy*cz + sx*sy*sz;

    _v._x = cz*cy*sx - sz*sy*cx;
    _v._y = cz*sy*cx + sz*cy*sx;
    _v._z = sz*cy*cx - cz*sy*sx;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotation(const Vector3f& axis, float angle)
{
    float si, co; 
    L_sincos(0.5f*angle, &si, &co);
    _w = co;
    _v._x = si * axis._x;
    _v._y = si * axis._y;
    _v._z = si * axis._z;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setRotation (const Orientationf& orientation)
{
    float sx,cx; L_sincos(float(orientation._pitch*0.5),&sx,&cx);
    float sy,cy; L_sincos(float(orientation._yaw*0.5),&sy,&cy);
    float sz,cz; L_sincos(float(orientation._roll*0.5),&sz,&cz);

    _w = cx*cy*cz + sx*sy*sz;

    _v._x = cz*cy*sx - sz*sy*cx;
    _v._y = cz*sy*cx + sz*cy*sx;
    _v._z = sz*cy*cx - cz*sy*sx;
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setSlerp(const Quaternionf& from, const Quaternionf& to, const float amount)
{
    Quaternionf tmpTo(to);
    Quaternionf tmp;

    float cosine = dot(from, tmpTo);

    if (cosine < 0 )
    {
        cosine=-cosine;
        tmpTo = -tmpTo;
    }

    if (cosine> 0.9999)
    {
        setNlerp(from, tmpTo, amount);
        return;
    }

    tmp._w = tmpTo._w- from._w*cosine;
    tmp._v._x = tmpTo._v._x-from._v._x*cosine;
    tmp._v._y = tmpTo._v._y-from._v._y*cosine;
    tmp._v._z = tmpTo._v._z-from._v._z*cosine;

    float sine = L_sqrt(dot(tmp,tmp));

    double s,c;
    L_sincos(atan2(sine,cosine)*amount,&s,&c);

    _w = float(from._w *c + tmp._w *s/sine);
    _v._x = float(from._v._x*c + tmp._v._x*s/sine);
    _v._y = float(from._v._y*c + tmp._v._y*s/sine);
    _v._z = float(from._v._z*c + tmp._v._z*s/sine);
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setNlerp(const Quaternionf& from, Quaternionf to, float amount)
{
    if( dot(from,to) < 0 )
    {
        to=-to;
    }

    _v._x = from._v._x*(1-amount) + to._v._x*amount;
    _v._y = from._v._y*(1-amount) + to._v._y*amount;
    _v._z = from._v._z*(1-amount) + to._v._z*amount;
    _w = from._w *(1-amount) + to._w*amount;

    normalizeSafe();
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setLog(const Quaternionf& q)
{
    float a = L_acosSafe(q._w);
    float sina = L_sin(a);

    if(Core::isEquivalent(sina, 0.0f))
    {
        _v.x = _v.y = _v.z = _w = 0.0;
    }
    else
    {
        _v = (a / sina)  * q._v;
        _w = 0.0;
    }
}
//-----------------------------------------------------------------------------
QUATINLINE void Quaternionf::setExp(const Quaternionf& q)
{
    float a = q._v.getLengthSafe();

    _w = L_cos(a);

    if(Core::isEquivalent(a, 0.0f))
    {
        _v.x = _v.y = _v.z = 0.0;
    }
    else
    {
        _v = (L_sin(a) / a) * q._v;
    }
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::getLog() const
{
    Quaternionf q;
    q.setLog(*this);
    return q;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::getExp() const
{
    Quaternionf q;
    q.setExp(*this);
    return q;
}
//-----------------------------------------------------------------------------
QUATINLINE Vector3f Quaternionf::apply(const Vector3f& vec) const
{
    Vector3f ret, tmp;

    tmp._x=(_v._y*vec._z-_v._z*vec._y)+_w*vec._x;
    tmp._y=(_v._z*vec._x-_v._x*vec._z)+_w*vec._y;
    tmp._z=(_v._x*vec._y-_v._y*vec._x)+_w*vec._z;

    ret._x=(tmp._z*_v._y-tmp._y*_v._z); ret._x+=ret._x+vec._x;
    ret._y=(tmp._x*_v._z-tmp._z*_v._x); ret._y+=ret._y+vec._y;
    ret._z=(tmp._y*_v._x-tmp._x*_v._y); ret._z+=ret._z+vec._z;

    return ret;
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator+ (const Quaternionf& other) const
{
    return Quaternionf(_w+other._w, _v+other._v);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator- (const Quaternionf& other) const
{
    return Quaternionf(_w-other._w, _v-other._v);
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator* (const Quaternionf& other) const
{
    float w1 = _w;
    float x1 = _v._x;
    float y1 = _v._y;
    float z1 = _v._z;
    float w2 = other._w;
    float x2 = other._v._x;
    float y2 = other._v._y;
    float z2 = other._v._z;
    return Quaternionf(
            w1*x2 + x1*w2 + y1*z2 - z1*y2,
            w1*y2 + y1*w2 + z1*x2 - x1*z2,
            w1*z2 + z1*w2 + x1*y2 - y1*x2,
            w1*w2 - x1*x2 - y1*y2 - z1*z2
    );
}
//-----------------------------------------------------------------------------
QUATINLINE Quaternionf Quaternionf::operator/ (const Quaternionf& other) const
{
    return Quaternionf(
            _v._z*other._v._y - _v._y*other._v._z - _w*other._v._x - _v._x*other._w,
            _v._x*other._v._z - _v._z*other._v._x - _w*other._v._y - _v._y*other._w,
            _v._y*other._v._x - _v._x*other._v._y - _w*other._v._z - _v._z*other._w,
            _w*other._w - _v._x*other._v._x + _v._y*other._v._y + _v._z*other._v._z);
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE Quaternionf operator* (const float a, const Quaternionf& quat)
{
    return Quaternionf(a*quat._w, a*quat._v);
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE Vector3f operator* (const Quaternionf& quat, const Vector3f& vec)
{
    Vector3f ret, tmp;

    tmp._x=(quat._v._y*vec._z-quat._v._z*vec._y)+quat._w*vec._x;
    tmp._y=(quat._v._z*vec._x-quat._v._x*vec._z)+quat._w*vec._y;
    tmp._z=(quat._v._x*vec._y-quat._v._y*vec._x)+quat._w*vec._z;

    ret._x=(tmp._z*quat._v._y-tmp._y*quat._v._z); ret._x+=ret._x+vec._x;
    ret._y=(tmp._x*quat._v._z-tmp._z*quat._v._x); ret._y+=ret._y+vec._y;
    ret._z=(tmp._y*quat._v._x-tmp._x*quat._v._y); ret._z+=ret._z+vec._z;

    return ret;
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE Vector3f operator* (const Vector3f& vec, const Quaternionf& quat)
{
    Vector3f ret, tmp;

    tmp._x=(quat._v._y*vec._z-quat._v._z*vec._y)+quat._w*vec._x;
    tmp._y=(quat._v._z*vec._x-quat._v._x*vec._z)+quat._w*vec._y;
    tmp._z=(quat._v._x*vec._y-quat._v._y*vec._x)+quat._w*vec._z;

    ret._x=(tmp._z*quat._v._y-tmp._y*quat._v._z); ret._x+=ret._x+vec._x;
    ret._y=(tmp._x*quat._v._z-tmp._z*quat._v._x); ret._y+=ret._y+vec._y;
    ret._z=(tmp._y*quat._v._x-tmp._x*quat._v._y); ret._z+=ret._z+vec._z;

    return ret;
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE Vector3f& operator*= (Vector3f& vec, const Quaternionf& quat)
{
    Vector3f ret, tmp;

    tmp._x=(quat._v._y*vec._z-quat._v._z*vec._y)+quat._w*vec._x;
    tmp._y=(quat._v._z*vec._x-quat._v._x*vec._z)+quat._w*vec._y;
    tmp._z=(quat._v._x*vec._y-quat._v._y*vec._x)+quat._w*vec._z;

    ret._x=(tmp._z*quat._v._y-tmp._y*quat._v._z); ret._x+=ret._x+vec._x;
    ret._y=(tmp._x*quat._v._z-tmp._z*quat._v._x); ret._y+=ret._y+vec._y;
    ret._z=(tmp._y*quat._v._x-tmp._x*quat._v._y); ret._z+=ret._z+vec._z;

    vec = ret;
    return vec;
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE float dot (const Quaternionf& quat1, const Quaternionf& quat2)
{
    return (quat1._v._x*quat2._v._x + quat1._v._y*quat2._v._y + quat1._v._z*quat2._v._z + quat1._w*quat2._w);
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE void Quaternionf::setSquad(const Quaternionf& q1, const Quaternionf& q2, const Quaternionf& q3, const Quaternionf& q4, float t)
{
    Quaternionf tmp1, tmp2;
    tmp1.setSlerpNoInvert(q1, q4, t);
    tmp2.setSlerpNoInvert(q2, q3, t);
    setSlerpNoInvert(tmp1, tmp2, 2.0f * t * (1.0f - t));
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE void Quaternionf::setNquad(const Quaternionf& q1, const Quaternionf& q2, const Quaternionf& q3, const Quaternionf& q4, float t)
{
    Quaternionf tmp1, tmp2;
    tmp1.setNlerp(q1, q4, t);
    tmp2.setNlerp(q2, q3, t);
    setNlerp(tmp1, tmp2, 2 * t * (1.0f - t));
}
//-----------------------------------------------------------------------------
QUATFUNCINLINE void Quaternionf::setSlerpNoInvert(const Quaternionf& from, const Quaternionf& to, float amount)
{
    Quaternionf tmpTo(to);
    Quaternionf tmp;

    float cosine = dot(from, tmpTo);

    if (cosine < -0.95 || cosine > 0.95)
    {
        setNlerp(from, tmpTo, amount);
        return;
    }

    tmp._w = tmpTo._w- from._w*cosine;
    tmp._v._x = tmpTo._v._x-from._v._x*cosine;
    tmp._v._y = tmpTo._v._y-from._v._y*cosine;
    tmp._v._z = tmpTo._v._z-from._v._z*cosine;

    float sine = L_sqrt(dot(tmp,tmp));

    double s,c;
    L_sincos(atan2(sine,cosine)*amount,&s,&c);

    _w = float(from._w *c + tmp._w *s/sine);
    _v._x = float(from._v._x*c + tmp._v._x*s/sine);
    _v._y = float(from._v._y*c + tmp._v._y*s/sine);
    _v._z = float(from._v._z*c + tmp._v._z*s/sine);
}
//-----------------------------------------------------------------------------
} //namespace Core

#endif /*CORE_QUATERNION_INL_*/
