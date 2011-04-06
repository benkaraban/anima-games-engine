
namespace Core
{
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f()
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Vector3f()\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(const float a, const float b, const float c) :
    _x(a), _y(b), _z(c)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Vector3f(const float a, const float b, const float c)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(const Vector3f& vec) :
    _x(vec._x), _y(vec._y), _z(vec._z)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Vector3f(const Vector3f& vec)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(const float a) :
    _x(a), _y(a), _z(a)
{
#ifdef LM_MATH_SHOW_CONSTRUCTOR
    std::cout<<"constructeur Vector3f(const float a)\n";
#endif
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(const float * pValues) :
    _x(pValues[0]), _y(pValues[1]), _z(pValues[2])
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(Zero_t) :
    _x(0), _y(0), _z(0)
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(One_t) :
    _x(1), _y(1), _z(1)
{
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(NAN_t) :
    _x(f_INIT_TO_NAN), _y(f_INIT_TO_NAN), _z(f_INIT_TO_NAN)
{
}
//----------------------------------------------------------------------------
VECFUNCINLINE Vector3f::Vector3f(const Vector3f & v1, const Vector3f & v2, float a)
{
    setLerp(v1, v2, a);
}
//----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::operator[]( const size_t& index ) const
{
    return *(&_x+index);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float& Vector3f::operator[]( const size_t& index )
{
    return *(&_x+index);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float* Vector3f::data ()
{
    return &_x;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE const float* Vector3f::data () const
{
    return &_x;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::set(const float a, const float b, const float c)
{
    _x = a;
    _y = b;
    _z = c;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector3f::operator== (const Vector3f& vec) const
{
    return ((_x == vec._x) && (_y == vec._y) && (_z == vec._z ));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector3f::operator!= (const Vector3f& vec) const
{
    return !((_x == vec._x) && (_y == vec._y) && (_z == vec._z ));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector3f::isEquivalent (const Vector3f& vec, const float epsilon) const
{
    return ((L_abs(_x - vec._x) <= epsilon) &&
            (L_abs(_y - vec._y) <= epsilon) &&
            (L_abs(_z - vec._z) <= epsilon));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector3f::isZero(float epsilon) const
{
    return (L_abs(_x)<=epsilon) && (L_abs(_y)<=epsilon) && (L_abs(_z)<=epsilon);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector3f::isUnit(float epsilon) const
{
    float val = getLengthSquared();
    return (L_abs(1 - val) <= epsilon);
}
//-----------------------------------------------------------------------------
VECINLINE bool Vector3f::isValid() const
{
    return Core::isValid(x) &&  Core::isValid(y) &&  Core::isValid(z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::operator*(const float a) const
{
    return Vector3f(_x * a, _y * a, _z * a);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f& Vector3f::operator*=(const float a)
{
    _x *= a;
    _y *= a;
    _z *= a;

    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::operator/(const float a) const
{
    float inverse = 1.f / a;
    return Vector3f(_x * inverse, _y * inverse, _z * inverse);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f& Vector3f::operator/=(const float a)
{
    float inverse = 1.f / a;

    _x *= inverse;
    _y *= inverse;
    _z *= inverse;

    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::operator-() const
{
    return Vector3f( -_x, -_y, -_z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::getLengthFast() const
{
    return L_sqrt_fast(_x*_x + _y*_y + _z*_z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::getLengthSafe() const
{
    return L_sqrt_safe(_x*_x + _y*_y + _z*_z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::getLengthSquared() const
{
    return (_x*_x + _y*_y + _z*_z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::setLength(float newLength)
{
    float inverse = newLength * L_isqrt(getLengthSquared());
    _x*=inverse; _y*=inverse; _z*=inverse;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::getDistance(const Vector3f& other) const
{
    return L_sqrt((_x-other._x)*(_x-other._x)+(_y-other._y)*(_y-other._y)+(_z-other._z)*(_z-other._z));
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector3f::getDistanceSquared (const Vector3f& other) const
{
    return (_x-other._x)*(_x-other._x) + (_y-other._y)*(_y-other._y) + (_z-other._z)*(_z-other._z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::normalizeFast()
{
    float invLength = L_isqrt_fast(_x*_x + _y*_y + _z*_z);
    _x*=invLength; _y*=invLength; _z*=invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::normalizeSafe()
{
    float invLength = L_isqrt_safe(_x*_x + _y*_y + _z*_z);
    _x*=invLength; _y*=invLength; _z*=invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::getNormalizedFast() const
{
    float invLength = L_isqrt_fast(_x*_x + _y*_y + _z*_z);
    return *this * invLength;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::getNormalizedSafe() const
{
    float invLength = L_isqrt_safe(_x*_x + _y*_y + _z*_z);
    return *this * invLength;
}
//-----------------------------------------------------------------------------
///interpolation linéaire
VECFUNCINLINE void Vector3f::setLerp(const Vector3f& from, const Vector3f& to, const float amount)
{
    if ( amount <= 0.0f ) (*this) = from;
    else if ( amount >= 1.0f ) (*this) = to;
    else (*this) = from + (to - from) * amount;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::setZero()
{
    _x = _y = _z = 0;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::setOne()
{
    _x = _y = _z = 1;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::clamp(const Vector3f& min, const Vector3f& max)
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
}
//-----------------------------------------------------------------------------
VECFUNCINLINE void Vector3f::snap()
{
    _x = (float)floor(_x + 0.5);
    _y = (float)floor(_y + 0.5);
    _z = (float)floor(_z + 0.5);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::operator + (const Vector3f& other) const
{
    return Vector3f(_x+other._x, _y+other._y, _z+other._z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f Vector3f::operator - (const Vector3f& other) const
{
    return Vector3f(_x-other._x, _y-other._y, _z-other._z);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f& Vector3f::operator += (const Vector3f& other)
{
    _x += other._x;
    _y += other._y;
    _z += other._z;
    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f& Vector3f::operator -= (const Vector3f& other)
{
    _x -= other._x;
    _y -= other._y;
    _z -= other._z;
    return *this;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f operator* (const float a, const Vector3f& vec)
{
    return Vector3f(vec._x * a, vec._y * a, vec._z * a);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector3f L_abs(const Vector3f& vec)
{
    return Vector3f(L_abs(vec._x), L_abs(vec._y), L_abs(vec._z));
}
//-----------------------------------------------------------------------------
} //namespace Core

