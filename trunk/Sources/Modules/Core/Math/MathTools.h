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
#ifndef CORE_MATHTOOLS_H_
#define CORE_MATHTOOLS_H_

#include <float.h>
#include <math.h>
#include <Core/Types.h>
#include <Core/Debug/Assert.h>
#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef _DEBUG
#define MTINLINE
#define MTFUNCINLINE LM_API_COR
#else
#define MTINLINE inline
#define MTFUNCINLINE inline
#endif

#include <Core/Enum.h>

#if defined(_MSC_VER)
#include <limits>
#endif

#include <winclean.h>

#ifdef M_PI
#undef M_PI
#endif

#ifdef NAN
#undef NAN
#endif

//---------------------------------------
const double d_PI         = 3.14159265358979323846264338327950288419716939937510582;
const double d_PI_DIV_2   = d_PI/2;
const double d_PI_DIV_4   = d_PI/4;
const double d_PI_MUL_2   = d_PI*2;
const double d_1_DIV_PI   = 1/d_PI;
const double d_2_DIV_PI   = 2/d_PI;

const float f_PI          = 3.14159265358979323846264338327950288419716939937510582f;
const float f_PI_DIV_2    = f_PI/2;
const float f_PI_DIV_4    = f_PI/4;
const float f_PI_MUL_2    = f_PI*2;
const float f_1_DIV_PI    = 1/f_PI;
const float f_2_DIV_PI    = 2/f_PI;
//---------------------------------------
const double d_SQRT2       = 1.4142135623730950488016887242097;
const double d_1_DIV_SQRT2 = 1/d_SQRT2;
const double d_SQRT3       = 1.7320508075688772935274463415059;
const double d_1_DIV_SQRT3 = 1/d_SQRT3;

const float f_SQRT2        = 1.4142135623730950488016887242097f;
const float f_1_DIV_SQRT2  = 1/f_SQRT2;
const float f_SQRT3        = 1.7320508075688772935274463415059f;
const float f_1_DIV_SQRT3  = 1/f_SQRT3;
//---------------------------------------
const double d_DEG2RAD_UNIT = d_PI/180;
const double d_RAD2DEG_UNIT = 180/d_PI;

const float f_DEG2RAD_UNIT = f_PI/180;
const float f_RAD2DEG_UNIT = 180/f_PI;
//---------------------------------------
const double d_COMMON_EPSILON = 0.001;
const double d_COMMON_EPSILON_FAST = 0.1;

const float f_COMMON_EPSILON = 0.001f;
const float f_COMMON_EPSILON_FAST = 0.1f;
//---------------------------------------
//utilisé pour initialiser une valeur à NAN.
//attention ne pas utiliser pour tester si la valeur est un NAN, pour cela utiliser les fonctions isValid

#if defined(_MSC_VER)
    const Core::uint32 d_INIT_TO_NAN_VALUE[] = {0xffffffff, 0x7fffffff};
    #define d_INIT_TO_NAN (*(const double *) d_INIT_TO_NAN_VALUE)
    #define f_INIT_TO_NAN ((float)d_INIT_TO_NAN) 
#else
    const double d_INIT_TO_NAN = __builtin_nan("");
    const float f_INIT_TO_NAN = __builtin_nanf("");
#endif

#if defined(_MSC_VER)
    #define INFINITY std::numeric_limits<float>::infinity()
#endif

namespace Core
{


LM_ENUM_1(Identity_t, IDENTITY);
LM_ENUM_1(Zero_t, ZERO);
LM_ENUM_1(One_t, ONE);
LM_ENUM_1(NAN_t, INIT_TO_NAN);
LM_ENUM_1(Translation_t, TRANSLATION);
LM_ENUM_1(Rotation_t, ROTATION);
LM_ENUM_1(Scale_t, SCALE);
LM_ENUM_1(MatLines_t, MAT_LINES);
LM_ENUM_1(MatAxis_t, MAT_AXIS);
LM_ENUM_1(MatZVec_t, MAT_ZVEC);

LM_ENUM_1(Pitch_t, PITCH);
LM_ENUM_1(Yaw_t, YAW);
LM_ENUM_1(Roll_t, ROLL);
LM_ENUM_1(Axis_t, AXIS);

LM_ENUM_3(ERotationAxis, ROT_X_PITCH,
                         ROT_Y_YAW,
                         ROT_Z_ROLL);




template <class T>
inline void L_swap(T &a,T &b)
{
        T tmp = a;
        a=b;
        b=tmp;
}

MTFUNCINLINE bool isValid(const float val);
MTFUNCINLINE bool isValid(const double val);

template <typename T> inline bool isValid(const T val)
{
    return true;
}

/**
 * ax² + bx + c
 * retourne true s'il y des solutions réelles, false dans le cas contraire
 * en sortie, r1 et r2 sont les racines réelles si elles existent
 */
LM_API_COR bool quadraticFormula(float a, float b, float c, float & r1, float & r2);

LM_API_COR uint16 floatToHalf(float v);
LM_API_COR float halfToFloat(uint16 v);

//-----------------------------------------------------------------------------
inline float L_cos(float val)
{
    return cosf(val);
}
inline double L_cos(double val)
{
    return cos(val);
}
//-----------------------------------------------------------------------------
inline float L_sin(float val)
{
    return sinf(val);
}
inline double L_sin(double val)
{
    return sin(val);
}
//-----------------------------------------------------------------------------
inline double L_tan(double val)
{
    return tan(val);
}
inline float L_tan(float val)
{
    return tanf(val);
}
//-----------------------------------------------------------------------------
inline float L_acos(float val)
{
    return acosf(val);
}
inline double L_acos(double val)
{
    return acos(val);
}
//-----------------------------------------------------------------------------
inline float L_acosSafe(float val)
{
    return acosf(std::min(1.0f, std::max(-1.0f, val)));
}
inline double L_acosSafe(double val)
{
    return acos(std::min(1.0, std::max(-1.0, val)));
}
//-----------------------------------------------------------------------------
inline float L_asin(float val)
{
    return asinf(val);
}
inline double L_asin(double val)
{
    return asin(val);
}
//-----------------------------------------------------------------------------
inline float L_atan(float val)
{
    return atanf(val);
}
inline double L_atan(double val)
{
    return atan(val);
}
//-----------------------------------------------------------------------------
inline void L_sincos(float angle, float* sinVal, float* cosVal)
{
    *sinVal=sinf(angle);
    *cosVal=cosf(angle);
}
inline void L_sincos(double angle, double* sinVal, double* cosVal)
{
    *sinVal=sin(angle);
    *cosVal=cos(angle);
}
//-----------------------------------------------------------------------------
inline float L_atan2(float val1, float val2)
{
    return atan2f(val1, val2);
}
inline double L_atan2(double val1, double val2)
{
    return atan2(val1, val2);
}
//-----------------------------------------------------------------------------
inline int32 L_abs(int32 val)
{
    int32 mask=val>>31;
    return (val+mask)^mask;
}
inline float L_abs(float val)
{
    return fabsf(val);
}
inline double L_abs(double val)
{
    return fabs(val);
}
//-----------------------------------------------------------------------------
inline float L_exp(float val)
{
    return expf(val);
}
inline double L_exp(double val)
{
    return exp(val);
}
//-----------------------------------------------------------------------------
inline float L_log(float val)
{
    return logf(val);
}
inline double L_log(double val)
{
    return log(val);
}
//-----------------------------------------------------------------------------
inline float L_sqrt(float val)
{
    return sqrtf(val);
}
inline double L_sqrt(double val)
{
    return sqrt(val);
}
inline float L_sqrt_safe(float val)
{
    return sqrtf(val + FLT_MIN);
}
inline double L_sqrt_safe(double val)
{
    return sqrt(val + DBL_MIN);
}
inline float L_sqrt_fast(float val)
{
    float res;
    __m128 s = _mm_rcp_ss(_mm_rsqrt_ss(_mm_set_ss(val)));
    _mm_store_ss(&res, s);
    return res;
}
inline double L_sqrt_fast(double val)
{
    return sqrt(val);
}
//-----------------------------------------------------------------------------
inline float L_isqrt(float val)
{
    return 1.0f/sqrtf(val);
}
inline double L_isqrt(double val)
{
    return 1.0/sqrt(val);
}
inline float L_isqrt_safe(float val)
{
    return 1.0f/sqrtf(val + FLT_MIN);
}
inline double L_isqrt_safe(double val)
{
    return 1.0/sqrt(val + DBL_MIN);
}
inline float L_isqrt_fast(float val)
{
    float res;
    __m128 s = _mm_rsqrt_ss(_mm_set_ss(val));
    _mm_store_ss(&res, s);
    return res;
}
inline double L_isqrt_fast(double val)
{
    return 1.0/sqrt(val);
}
//-----------------------------------------------------------------------------
inline int32 L_pow(int32 x, int32 y)
{
    return static_cast<int32>(powf((float)x, (float)y));
}
inline float L_pow(float x, float y)
{
    return powf(x, y);
}
inline double L_pow(double x, double y)
{
    return pow(x, y);
}
//-----------------------------------------------------------------------------
inline int32 L_floor(int32 val)
{
    return val;
}
inline float L_floor(float val)
{
    return floorf(val);
}
inline double L_floor(double val)
{
    return floor(val);
}
//-----------------------------------------------------------------------------
inline float L_frac(float val)
{
    return val - floorf(val);
}
inline double L_frac(double val)
{
    return val - floor(val);
}
//-----------------------------------------------------------------------------
inline float L_truncate(float val, int32 nbDecimal)
{
    float power = (float)L_pow(10, nbDecimal);
    return ((float)((int32)(val*power)))/power;
}
//-----------------------------------------------------------------------------
/*
//retounre la valeur entiere la plus proche de val qui n'est pas inferieur a val
ceil de 2.3 = 3.0
ceil de 3.8 = 4.0
ceil de -2.3 = -2.0
ceil de -3.8 = -3.0
*/
inline int32 L_ceil(int32 val)
{
    return val;
}
inline float L_ceil(float val)
{
    return ceilf(val);
}
inline double L_ceil(double val)
{
    return ceil(val);
}
//-----------------------------------------------------------------------------
/*
// retourne la valeur entiere la plus proche de val qui n'est pas superieur a val
round de 2.3 = 2.0
round de 3.8 = 3.0
round de -2.3 = -3.0
round de -3.8 = -4.0
*/
inline int32 L_round(int32 val)
{
    return val;
}
inline int32 L_round(float val)
{
    return int32(L_floor(val + 0.5));
}
inline int32 L_round(double val)
{
    return int32(L_floor(val + 0.5));
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
inline int32 nextPowerOfTwo(int32 n)
{
    LM_ASSERT(n >= 0);
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

//-----------------------------------------------------------------------------
inline int32 previousPowerOfTwo(int32 n)
{
    return nextPowerOfTwo(n + 1) >> 1;
}

//-----------------------------------------------------------------------------
inline int32 nearestPowerOfTwo(int32 n)
{
    const int32 next = nextPowerOfTwo(n);
    const int32 prev = previousPowerOfTwo(n);

    if (next - n <= n - prev)
        return next;
    else
        return prev;
}

//-----------------------------------------------------------------------------
inline bool isPowerOfTwo(int32 n)
{
    return nextPowerOfTwo(n) == n;
}

//-----------------------------------------------------------------------------
inline float deg2rad(const float& deg)
{
    return deg * f_DEG2RAD_UNIT;
}

//-----------------------------------------------------------------------------
inline float rad2deg(const float& rad)
{
    return rad * f_RAD2DEG_UNIT;
}

//-----------------------------------------------------------------------------
template <class T> inline T clamp(T min, T max, T value)
{
    if (value < min)
        return min;
    if (value> max)
        return max;

    return value;
}

//-----------------------------------------------------------------------------
template <class T> inline T clampByte(T value)
{
    if (value < 0)
        return 0;
    if (value> Core::BYTE_MAX)
        return Core::BYTE_MAX;

    return value;
}

//-----------------------------------------------------------------------------
template <class T> inline T clampInt8(T value)
{
    if (value < Core::INT8_MIN)
        return Core::INT8_MIN;
    if (value > Core::INT8_MAX)
        return Core::INT8_MAX;

    return value;
}

//-----------------------------------------------------------------------------
template <class T> inline T clampUInt8(T value)
{
    if (value < 0)
        return 0;
    if (value > Core::UINT8_MAX)
        return Core::UINT8_MAX;

    return value;
}

//-----------------------------------------------------------------------------
template <class T> inline T clampInt16(T value)
{
    if (value < Core::UINT16_MIN)
        return Core::UINT16_MIN;
    if (value > Core::UINT16_MAX)
        return Core::UINT16_MAX;

    return value;
}

//-----------------------------------------------------------------------------
template <class T> inline T clampUInt16(T value)
{
    if (value < 0)
        return 0;
    if (value > Core::UINT16_MAX)
        return Core::UINT16_MAX;

    return value;
}

inline bool isEquivalent(const float val_1, const float val_2, const float epsilon = f_COMMON_EPSILON)
{
    return (L_abs(val_1-val_2)<epsilon);
}

inline bool isEquivalent(const double val_1, const double val_2, const double epsilon = d_COMMON_EPSILON)
{
    return (L_abs(val_1-val_2)<epsilon);
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& min(const T& A, const T& B)
{
    return (A <= B) ? (A) : (B);
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& max(const T& A, const T& B)
{
    return (A >= B) ? (A) : (B);
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& max(const T& A, const T& B, const T& C)
{
    return (A >= B) ? (max(A,C)) : (max(B,C));
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& max(const T& A, const T& B, const T& C, const T& D)
{
    return (A >= B) ? (max(A,C,D)) : (max(B,C,D));
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& max(const T& A, const T& B, const T& C, const T& D, const T& E)
{
    return (A >= B) ? (max(A,C,D,E)) : (max(B,C,D,E));
}
//-----------------------------------------------------------------------------
template<typename T>
MTINLINE const T& max(const T& A, const T& B, const T& C, const T& D, const T& E, const T F)
{
    return (A >= B) ? (max(A,C,D,E,F)) : (max(B,C,D,E,F));
}
//-----------------------------------------------------------------------------

} //namespace Core

#ifndef _DEBUG
#include <Core/Math/MathTools.inl>
#endif

#endif /*MATHTOOLS_H_*/

