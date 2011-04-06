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
#include <Core/Math/Quaternion.h>

#ifdef _DEBUG
#include <Core/Math/Quaternion.inl>
#endif
#include <Core/Math/Matrix.h>

namespace Core
{
//-----------------------------------------------------------------------------
Quaternionf matrixToQuaternion(const Matrix3f& m)
{
    float tr, s, q[4];
    int i, j, k;
    Quaternionf quat;

    int nxt[3] =
    { 1, 2, 0 };

    tr = m[0][0] + m[1][1] + m[2][2];

    // check the diagonal
    if (tr > 0.0)
    {
        s = L_sqrt(tr + 1.0f);
        quat._w = s / 2.0f;
        s = 0.5f / s;
        quat._v._x = (m[2][1] - m[1][2]) * s;
        quat._v._y = (m[0][2] - m[2][0]) * s;
        quat._v._z = (m[1][0] - m[0][1]) * s;
    }
    else
    {
        // diagonal is negative
        i = 0;
        if (m[1][1] > m[0][0])
            i = 1;
        if (m[2][2] > m[i][i])
            i = 2;
        j = nxt[i];
        k = nxt[j];

        s = L_sqrt((m[i][i] - (m[j][j] + m[k][k])) + 1.0f);

        q[i] = s * 0.5f;

        if (s != 0.0)
            s = 0.5f / s;

        q[3] = (m[k][j] - m[j][k]) * s;
        q[j] = (m[j][i] + m[i][j]) * s;
        q[k] = (m[k][i] + m[i][k]) * s;

        quat._v._x = q[0];
        quat._v._y = q[1];
        quat._v._z = q[2];
        quat._w = q[3];
    }

    return quat;
}
//-----------------------------------------------------------------------------
Matrix3f Quaternionf::toMatrix3() const
{/*
 Matrix3f mat;
 Vector3f v2=_v+_v;

 double xx=1-v2._x*_v._x; double yy=v2._y*_v._y; double xw=v2._x*_w;
 double xy=v2._y*_v._x; double yz=v2._z*_v._y; double yw=v2._y*_w;
 double xz=v2._z*_v._x; double zz=v2._z*_v._z; double zw=v2._z*_w;

 mat._00=float(1-yy-zz); mat._01=float(xy-zw); mat._02=float(xz+yw);
 mat._10=float(xy+zw); mat._11=float(xx-zz); mat._12=float(yz-xw);
 mat._20=float(xz-yw); mat._21=float(yz+xw); mat._22=float(xx-yy);

 return mat;



 */

    Matrix3f m;
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

    // calculate coefficients
    x2 = _v._x + _v._x; y2 = _v._y + _v._y;
    z2 = _v._z + _v._z;
    xx = _v._x * x2; xy = _v._x * y2; xz = _v._x * z2;
    yy = _v._y * y2; yz = _v._y * z2; zz = _v._z * z2;
    wx = _w * x2; wy = _w * y2; wz = _w * z2;

    m[0][0] = 1.0f - (yy + zz);
    m[0][1] = xy - wz;
    m[0][2] = xz + wy;

    m[1][0] = xy + wz;
    m[1][1] = 1.0f - (xx + zz);
    m[1][2] = yz - wx;

    m[2][0] = xz - wy;
    m[2][1] = yz + wx;
    m[2][2] = 1.0f - (xx + yy);

    return m;
}
//-----------------------------------------------------------------------------
void Quaternionf::setTangent(const Quaternionf& prev, const Quaternionf& cur, const Quaternionf& next)
{
    Quaternionf invCur(cur.getInverted());
    *this = cur * (-(((invCur * next).getLog() + (invCur * prev).getLog()) / 4.0)).getExp();
}
//-----------------------------------------------------------------------------
Matrix4f Quaternionf::toMatrix4() const
{
    Matrix3f mat = toMatrix3();
    return mat.toMatrix4();
}
//-----------------------------------------------------------------------------
String toString(const Quaternionf& quat)
{
    String result;

    result<<L"[";

    result << L"rotation : " << Core::toString(quat._v) << L", ";
    result << L"Orientation : " << Core::toString(quat._w);

    result<<L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Quaternionf& quat, int32 precision)
{
    String result;

    result<<L"[";

    result << L"rotation : " << Core::toString(quat._v, precision) << L", ";
    result << L"Orientation : " << Core::toString(quat._w, precision);

    result<<L"]";
    return result;
}
//-----------------------------------------------------------------------------
} //namespace Core
