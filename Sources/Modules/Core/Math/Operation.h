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
#ifndef CORE_OPERATION_H_
#define CORE_OPERATION_H_

#include <Core/Math/Quaternion.h>

namespace Core
{
inline float dot(const Vector3f& src1, const Vector3f& src2)
{
    return src1._x*src2._x + src1._y*src2._y + src1._z*src2._z;
}

inline Vector3f cross (const Vector3f& src1, const Vector3f& src2)
{
    return Vector3f(
            src1._y * src2._z - src1._z * src2._y,
            src1._z * src2._x - src1._x * src2._z,
            src1._x * src2._y - src1._y * src2._x);
}

inline Vector3f mul(const Vector3f& src1, const Vector3f& src2)
{
    return Vector3f(
            src1._x * src2._x,
            src1._y * src2._y,
            src1._z * src2._z);
}

inline Vector3f div(const Vector3f& src1, const Vector3f& src2)
{
    return Vector3f(
            src1._x / src2._x,
            src1._y / src2._y,
            src1._z / src2._z);
}

inline Vector4f mul(const Vector4f & v1, const Vector4f & v2)
{
    return Vector4f(v1.r * v2.r, 
                    v1.g * v2.g, 
                    v1.b * v2.b, 
                    v1.a * v2.a);
}

} //namespace Core

#endif /*MATHOPERATION_H_*/
