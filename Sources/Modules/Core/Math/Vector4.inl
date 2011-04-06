namespace Core
{
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f()
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"passage constructeur Vector4f()\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const float a, const float b, const float c, const float d) :
    _x(a), _y(b), _z(c), _w(d)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"passage constructeur Vector4f(const float a, const float b, const float c, const float d)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const Vector3f& vec) :
    _x(vec._x), _y(vec._y), _z(vec._z), _w(1.f)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"passage constructeur Vector4f(const Vector3& vec)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const Vector3f & v, float w) :
    x(v.x), y(v.y), z(v.z), w(w)
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const Vector4f& vec) :
    _x(vec._x), _y(vec._y), _z(vec._z), _w(vec._w)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"passage constructeur Vector4f(const Vector4f& vec)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const float a) :
    _x(a), _y(a), _z(a), _w(a)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"passage constructeur Vector4f(const float a)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(const float * pValues) :
    _x(pValues[0]), _y(pValues[1]), _z(pValues[2]), _w(pValues[3])
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(Zero_t) :
    _x(0), _y(0), _z(0), _w(0)
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f::Vector4f(One_t) :
    _x(1), _y(1), _z(1), _w(1)
{
}
//-----------------------------------------------------------------------------
VECINLINE Vector4f::Vector4f(const Vector4f& v1, const Vector4f& v2, float alpha)
{
    setLerp(v1, v2, alpha);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::operator[]( const size_t& index ) const
{
    return *(&_x+index);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float& Vector4f::operator[]( const size_t& index )
{
    return *(&_x+index);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float* Vector4f::data ()
{
    return &_x;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE const float* Vector4f::data () const
{
    return &_x;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::set(const float a, const float b, const float c, const float d)
{
    _x = a;
    _y = b;
    _z = c;
    _w = d;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector4f::operator== (const Vector4f& vec) const
{
    return ((_x == vec._x) && (_y == vec._y) && (_z == vec._z ) && (_w == vec._w));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector4f::operator!= (const Vector4f& vec) const
{
    return !((_x == vec._x) && (_y == vec._y) && (_z == vec._z ) && (_w == vec._w));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector4f::isEquivalent (const Vector4f& vec, const float epsilon) const
{
    return ((L_abs(_x - vec._x) <= epsilon) &&
            (L_abs(_y - vec._y) <= epsilon) &&
            (L_abs(_z - vec._z) <= epsilon) &&
            (L_abs(_w - vec._w) <= epsilon));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector4f::isZero(float epsilon) const
{
    return (L_abs(_x)<=epsilon) && (L_abs(_y)<=epsilon) && (L_abs(_z)<=epsilon) && (L_abs(_w)<=epsilon);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector4f::isUnit(float epsilon) const
{
    float val = getLengthSquared();
    return (L_abs(1 - val) <= epsilon);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::operator*(const float a) const
{
    return Vector4f(_x * a, _y * a, _z * a, _w * a);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f& Vector4f::operator*=(const float a)
{
    _x *= a;
    _y *= a;
    _z *= a;
    _w *= a;

    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::operator/(const float a) const
{
    float inverse = 1.f / a;
    return Vector4f(_x * inverse, _y * inverse, _z * inverse, _w * inverse);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f& Vector4f::operator/=(const float a)
{
    float inverse = 1.f / a;

    _x *= inverse;
    _y *= inverse;
    _z *= inverse;
    _w *= inverse;

    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::operator-() const
{
    return Vector4f( -_x, -_y, -_z, -_w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::getLengthFast() const
{
    return L_sqrt_fast(_x * _x + _y * _y + _z * _z + _w * _w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::getLengthSafe() const
{
    return L_sqrt_safe(_x * _x + _y * _y + _z * _z + _w * _w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::getLengthSquared() const
{
    return (_x*_x + _y*_y + _z*_z + _w*_w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::setLength(float newLength)
{
    float inverse = newLength * L_isqrt(getLengthSquared());
    _x*=inverse; _y*=inverse; _z*=inverse; _w*=inverse;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::normalizeFast()
{
    float invLength = L_isqrt_fast(_x*_x + _y*_y + _z*_z + _w*_w);
    _x*=invLength; _y*=invLength; _z*=invLength; _w*=invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::normalizeSafe()
{
    float invLength = L_isqrt_safe(_x*_x + _y*_y + _z*_z + _w*_w);
    _x*=invLength; _y*=invLength; _z*=invLength; _w*=invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::getNormalizedFast() const
{
    float invLength = L_isqrt_fast(_x*_x + _y*_y + _z*_z + _w*_w);
    return *this * invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::getNormalizedSafe() const
{
    float invLength = L_isqrt_safe(_x*_x + _y*_y + _z*_z + _w*_w);
    return *this * invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector4f::getXYZ() const
{
    return Vector3f(x, y, z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector4f::homogeneousDiv() const
{
    LM_ASSERT(!Core::isEquivalent(w, 0.0f, 0.00001f));
    double inv = 1.0 / w;
    return Vector3f(float(x * inv), float(y * inv), float(z * inv));
}
//-----------------------------------------------------------------------------
///interpolation linéaire
VECFUNCINLINE void Vector4f::setLerp(const Vector4f& from, const Vector4f& to, const float amount)
{
    if ( amount <= 0.0f ) (*this) = from;
    else if ( amount >= 1.0f ) (*this) = to;
    else (*this) = from + (to - from) * amount;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::setZero()
{
    _x = _y = _z = _w = 0.f;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector4f::clamp(const Vector4f& min, const Vector4f& max)
{
    if (_x < min._x)
    _x = min._x;
    else if (_x> max._x)
    _x = max._x;

    if (_y < min._y)
    _y = min._y;
    else if (_y> max._y)
    _y = max._y;

    if (_z < min._z)
    _z = min._z;
    else if (_z> max._z)
    _z = max._z;

    if (_w < min._w)
    _w = min._w;
    else if (_w> max._w)
    _w = max._w;
}

//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::getDistance(const Vector4f& other) const
{
    return L_sqrt((_x-other._x)*(_x-other._x)+(_y-other._y)*(_y-other._y)+(_z-other._z)*(_z-other._z)+(_w-other._w)*(_w-other._w));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector4f::getDistanceSquared (const Vector4f& other) const
{
    return (_x-other._x)*(_x-other._x) + (_y-other._y)*(_y-other._y) + (_z-other._z)*(_z-other._z) + (_w-other._w)*(_w-other._w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::operator + (const Vector4f& other) const
{
    return Vector4f(_x+other._x, _y+other._y, _z+other._z, _w+other._w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f Vector4f::operator - (const Vector4f& other) const
{
    return Vector4f(_x-other._x, _y-other._y, _z-other._z, _w-other._w);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f& Vector4f::operator += (const Vector4f& other)
{
    _x += other._x;
    _y += other._y;
    _z += other._z;
    _w += other._w;
    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f& Vector4f::operator -= (const Vector4f& other)
{
    _x -= other._x;
    _y -= other._y;
    _z -= other._z;
    _w -= other._w;
    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector4f operator* (const float a, const Vector4f& vec)
{
    return Vector4f(vec._x * a, vec._y * a, vec._z * a, vec._w * a);
}
//-----------------------------------------------------------------------------
} //namespace Core

