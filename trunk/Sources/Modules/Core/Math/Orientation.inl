
namespace Core
{
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf()
{
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf(Zero_t) :
    _pitch(0), _yaw(0), _roll(0)
{
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf(const float pitch, const float yaw, const float roll) :
    _pitch(pitch), _yaw(yaw), _roll(roll)
{
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf(const Vector3f& vec) :
    _pitch(float(vec._x)), _yaw(float(vec._y)), _roll(float(vec._z))
{

}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf(const Quaternionf& quat)
{
    setOrientations(quat);
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf::Orientationf(const Matrix3f& mat)
{
    setOrientations(mat);
}
//-----------------------------------------------------------------------------
ORIENTINLINE bool Orientationf::operator==(const Orientationf& other) const
{
    return _yaw == other._yaw && _pitch == other._pitch && _roll == other._roll;
}
//-----------------------------------------------------------------------------
ORIENTINLINE bool Orientationf::operator!=(const Orientationf& other) const
{
    return !(*this == other);
}
//-----------------------------------------------------------------------------
ORIENTINLINE bool Orientationf::isEquivalent(const Orientationf& other, float epsilon) const
{
    return ((L_abs(_yaw-other._yaw) <= epsilon) &&
            (L_abs(_pitch-other._pitch) <= epsilon)&&
            (L_abs(_roll-other._roll) <= epsilon));
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf Orientationf::operator* (const float a) const
{
    return Orientationf(_pitch*a, _yaw*a , _roll*a);
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf Orientationf::operator/ (float a) const
{
    a = 1/a;
    return Orientationf(_pitch*a, _yaw*a , _roll*a);
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf& Orientationf::operator*= (const float a)
{
    _yaw*=a;
    _pitch*=a;
    _roll*=a;

    return *this;
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf& Orientationf::operator/= (float a)
{
    a = 1/a;

    _yaw*=a;
    _pitch*=a;
    _roll*=a;

    return *this;
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf Orientationf::operator- () const
{
    return Orientationf(-_pitch, -_yaw, -_roll);
}
//-----------------------------------------------------------------------------
ORIENTINLINE float& Orientationf::operator[] (int index)
{
    return *(&_pitch+index);
}
//-----------------------------------------------------------------------------
ORIENTINLINE float Orientationf::operator[] (int index) const
{
    return *(&_pitch+index);
}

//-----------------------------------------------------------------------------
ORIENTINLINE float* Orientationf::data ()
{
    return &_pitch;
}
//-----------------------------------------------------------------------------
ORIENTINLINE float const * Orientationf::data () const
{
    return &_pitch;
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::set(const float pitch, const float yaw, const float roll)
{
    _yaw = yaw;
    _pitch = pitch;
    _roll = roll;
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::setZero()
{
    _yaw = _pitch = _roll = 0.f;
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::normalizePIFast()
{
    if (_yaw<f_PI) _yaw+=f_PI_MUL_2;
    if (_yaw>f_PI) _yaw-=f_PI_MUL_2;

    if (_pitch<f_PI) _pitch+=f_PI_MUL_2;
    if (_pitch>f_PI) _pitch-=f_PI_MUL_2;

    if (_roll<f_PI) _roll+=f_PI_MUL_2;
    if (_roll>f_PI) _roll-=f_PI_MUL_2;
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::normalizePISafe()
{

    _pitch = atan2(sin(_pitch), cos(_pitch));
    _yaw = atan2(sin(_yaw), cos(_yaw));
    _roll = atan2(sin(_roll), cos(_roll));
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::clamp(const Orientationf& min, const Orientationf& max)
{
    if (_yaw< min._yaw) _yaw=(float)min._yaw;
    if (_yaw> max._yaw) _yaw=(float)max._yaw;

    if (_pitch< min._pitch) _pitch=(float)min._pitch;
    if (_pitch> max._pitch) _pitch=(float)max._pitch;

    if (_roll< min._roll) _roll=(float)min._roll;
    if (_roll> max._roll) _roll=(float)max._roll;
}
//-----------------------------------------------------------------------------
ORIENTINLINE void Orientationf::normalize2PI()
{
    while (_yaw >= f_PI_MUL_2)    _yaw -= f_PI_MUL_2;
    while (_yaw < 0)             _yaw += f_PI_MUL_2;

    while (_pitch >= f_PI_MUL_2)  _pitch -= f_PI_MUL_2;
    while (_pitch < 0)           _pitch += f_PI_MUL_2;

    while (_roll >= f_PI_MUL_2)   _roll -= f_PI_MUL_2;
    while (_roll < 0)            _roll += f_PI_MUL_2;
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf Orientationf::makeOrientations(const Quaternionf& quat)
{
    return Orientationf(quat);
}
//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf Orientationf::makeOrientations(const Matrix3f& mat)
{
    return Orientationf(mat);
}

//-----------------------------------------------------------------------------
ORIENTINLINE Orientationf operator+ (const Orientationf& Orientation_1, const Orientationf& Orientation_2)
{
    return Orientationf(Orientation_1._pitch+Orientation_2._pitch, Orientation_1._yaw+Orientation_2._yaw, Orientation_1._roll+Orientation_2._roll);
}
//-----------------------------------------------------------------------------
ORIENTFUNCINLINE Orientationf& operator+= (Orientationf& Orientation_1, const Orientationf& Orientation_2)
{
    Orientation_1._yaw+=Orientation_2._yaw;
    Orientation_1._pitch+=Orientation_2._pitch;
    Orientation_1._roll+=Orientation_2._roll;

    return Orientation_1;
}
//-----------------------------------------------------------------------------
ORIENTFUNCINLINE Orientationf operator- (const Orientationf& Orientation_1, const Orientationf& Orientation_2)
{
    return Orientationf(Orientation_1._pitch-Orientation_2._pitch, Orientation_1._yaw-Orientation_2._yaw, Orientation_1._roll-Orientation_2._roll);
}
//-----------------------------------------------------------------------------
ORIENTFUNCINLINE Orientationf& operator-= (Orientationf& Orientation_1, const Orientationf& Orientation_2)
{
    Orientation_1._yaw-=Orientation_2._yaw;
    Orientation_1._pitch-=Orientation_2._pitch;
    Orientation_1._roll-=Orientation_2._roll;

    return Orientation_1;
}
//-----------------------------------------------------------------------------
ORIENTFUNCINLINE Orientationf operator* (const float value, const Orientationf& Orientationf)
{
    return Orientationf * value;
}
//-----------------------------------------------------------------------------

} //namespace Core

