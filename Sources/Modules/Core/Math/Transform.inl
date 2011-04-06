#ifndef CORE_TRANSFORM_INL_
#define CORE_TRANSFORM_INL_

namespace Core
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MTINLINE Transformf::Transformf(const float TX, const float TY, const float TZ, const float RX, const float RY, const float RZ,
                                             const float RW, const float SX, const float SY, const float SZ, const float SOX, const float SOY,
                                             const float SOZ, const float SOW) :
    _translation(TX, TY, TZ), _rotation(RX, RY, RZ, RW), _scale(SX, SY, SZ), _scaleOrientation(SOX, SOY, SOZ, SOW)
{
}
//-----------------------------------------------------------------------------
MTINLINE Transformf::Transformf(const Vector3f& t, const Quaternionf& r, const Vector3f& s, const Quaternionf& so) :
    _translation(t), _rotation(r), _scale(s), _scaleOrientation(so)
{
}
//-----------------------------------------------------------------------------
MTINLINE Transformf::Transformf(const Matrix4f& mat)
{
    mat.getTransformSafe(_translation, _rotation, _scale, _scaleOrientation);
}
//-----------------------------------------------------------------------------
MTINLINE Transformf::Transformf(Identity_t) :
    _translation(ZERO), _rotation(IDENTITY), _scale(ONE), _scaleOrientation(IDENTITY)
{
}
//-----------------------------------------------------------------------------
MTINLINE Transformf::Transformf(const Transformf& from, const Transformf& to, const float amount)
{
    setLerp(from, to, amount);
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::setIdentity()
{
    _translation.setZero();
    _rotation.setIdentity();
    _scale.setOne();
    _scaleOrientation.setIdentity();
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::invertSelf()
{
    _scale._x = 1.0f / _scale._x;
    _scale._y = 1.0f / _scale._y;
    _scale._z = 1.0f / _scale._z;
    _rotation.inverseSelf();
    _translation = _rotation * _translation;
    _scaleOrientation.inverseSelf();
}
//-----------------------------------------------------------------------------
MTINLINE Transformf Transformf::getInverted() const
{
    Transformf inverse;
    inverse._scale._x = 1.0f / inverse._scale._x;
    inverse._scale._y = 1.0f / inverse._scale._y;
    inverse._scale._z = 1.0f / inverse._scale._z;
    inverse._rotation = _rotation.getInverted();
    inverse._translation = inverse._rotation * _translation;
    inverse._scaleOrientation = _scaleOrientation.getInverted();

    return inverse;
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::setLerp(const Transformf& from, const Transformf& to, const float amount)
{
    _translation.setLerp(from._translation, to._translation, amount);
    _scale.setLerp(from._scale, to._scale, amount);
    _rotation.setSlerp(from._rotation, to._rotation, amount);
    _scaleOrientation.setSlerp(from._scaleOrientation, to._scaleOrientation, amount);
}
//-----------------------------------------------------------------------------
MTINLINE bool Transformf::operator==(const Transformf& other) const
{
    return _translation == other._translation &&
    _rotation == other._rotation &&
    _scale == other._scale &&
    _scaleOrientation == other._scaleOrientation;
}
//-----------------------------------------------------------------------------
MTINLINE bool Transformf::operator!=(const Transformf& other) const
{
    return !(_translation == other._translation &&
            _rotation == other._rotation &&
            _scale == other._scale &&
            _scaleOrientation == other._scaleOrientation);
}
//-----------------------------------------------------------------------------
MTINLINE bool Transformf::isEquivalent(const Transformf& other, const float epsilon) const
{
    return(
            _translation.isEquivalent(other._translation, epsilon) &&
            _rotation.isEquivalent(other._rotation, epsilon) &&
            _scale.isEquivalent(other._scale, epsilon) &&
            _scaleOrientation.isEquivalent(other._scaleOrientation, epsilon));
}
//-----------------------------------------------------------------------------
MTINLINE Vector3f Transformf::apply(const Vector3f& vec) const
{
    Vector3f res (vec);

    res *= _scaleOrientation.getInverted();
    res._x *= _scale._x;
    res._y *= _scale._y;
    res._z *= _scale._z;
    res *= _scaleOrientation;

    res *= _rotation;

    res += _translation;

    return res;
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::translateInGlobalAxis(const Vector3f& offset)
{
    _translation += offset;
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::translateInTransformAxis(const Vector3f& offset)
{
    _translation += _rotation.apply(offset);
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::moveToInGlobalAxis(const Vector3f& position)
{
    _translation = position;
}
//-----------------------------------------------------------------------------
MTINLINE void Transformf::rotateInGlobalAxis(const Rotationf& rotation)
{/*
    _translation = (rotation.orientation.apply((_translation - rotation.center)) + rotation.center);
    _rotation = rotation.orientation * _rotation;*/

    _translation = rotation.rotatePoint(_translation);
    _rotation = Quaternionf(rotation.getAxisMatrix()) * _rotation;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
MTFUNCINLINE Vector3f operator* (const Transformf& te, const Vector3f& vec)
{
    Vector3f res (vec);

    res *= te._scaleOrientation.getInverted();
    res._x *= te._scale._x;
    res._y *= te._scale._y;
    res._z *= te._scale._z;
    res *= te._scaleOrientation;

    res *= te._rotation;

    res += te._translation;

    return res;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace Core


#endif /*CORE_TRANSFORM_INL_*/
