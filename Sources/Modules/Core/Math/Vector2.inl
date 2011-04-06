namespace Core
{
//-----------------------------------------------------------------------------
VECINLINE Vector2f::Vector2f(const Vector2f & v1, const Vector2f & v2, float a)
{
    setLerp(v1, v2, a);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector2f::operator == (const Vector2f & v) const
{
    return (x == v.x) && (y == v.y);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector2f::operator != (const Vector2f & v) const
{
    return (x != v.x) || (y != v.y);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE bool Vector2f::isEquivalent(const Vector2f & v, float epsilon) const
{
    return ((L_abs(x - v.x) <= epsilon) &&
            (L_abs(y - v.y) <= epsilon));
}
//-----------------------------------------------------------------------------
VECINLINE void Vector2f::setLerp(const Vector2f & from, const Vector2f & to, float amount)
{
    if( amount <= 0.0f ) (*this) = from;
    else if( amount >= 1.0f ) (*this) = to;
    else(*this) = from + (to - from) * amount;
}
//-----------------------------------------------------------------------------
VECINLINE Vector2f Vector2f::operator * (float s) const
{
    return Vector2f(s * x, s * y);
}
//-----------------------------------------------------------------------------
VECINLINE float Vector2f::getLengthFast() const
{
    return float(sqrt(x * x + y * y));
}
//-----------------------------------------------------------------------------
VECINLINE float Vector2f::getLengthSafe() const
{
    return float(sqrt(x * x + y * y));
}
//-----------------------------------------------------------------------------
VECINLINE float Vector2f::getLengthSquared() const
{
    return x * x + y * y;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector2f operator * (float s, const Vector2f & v)
{
    return Vector2f(s * v.x, s * v.y);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float Vector2f::operator[](size_t index) const
{
    if(index == 0)
        return x;
    else
        return y;
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector2f Vector2f::operator + (const Vector2f & v) const
{
    return Vector2f(x + v.x, y + v.y);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE Vector2f Vector2f::operator - (const Vector2f & v) const
{
    return Vector2f(x - v.x, y - v.y);
}
//-----------------------------------------------------------------------------
VECFUNCINLINE float & Vector2f::operator[](size_t index)
{
    if(index == 0)
        return x;
    else
        return y;
}
//-----------------------------------------------------------------------------
} //namespace Core

