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
#include <Core/Math/Splines/CatmullRom.h>

namespace Core
{
//------------------------------------------------------------------------------
CRSpline::CRSpline(const List<Vector3f> & points) :
    _points(points)
{
    LM_ASSERT(points.size() >= 4);
}
//------------------------------------------------------------------------------
Vector3f CRSpline::eval(float u) const
{
    double ind = 1.0 + u * double(_points.size() - 3);
    int32 i1 = int32(L_floor(ind));
    float t  = float(L_frac(ind));

    if(i1 > _points.size() - 3)
    {
        i1 = _points.size() - 3;
        t = 1.0;
    }

    int32 i0 = i1 - 1;
    int32 i2 = i1 + 1;
    int32 i3 = i1 + 2;

    LM_ASSERT(i0 >= 0);
    LM_ASSERT(i3 < _points.size());

    float t2 = t * t;
    float t3 = t * t2;

    float c0 = 0.5f * (       -t3 + 2.0f * t2 - t);
    float c1 = 0.5f * ( 3.0f * t3 - 5.0f * t2     + 2.0f);
    float c2 = 0.5f * (-3.0f * t3 + 4.0f * t2 + t);
    float c3 = 0.5f * (        t3 -        t2);

    return (c0 * _points[i0] +
            c1 * _points[i1] +
            c2 * _points[i2] +
            c3 * _points[i3]);
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
CRQuatSpline::CRQuatSpline(const List<Quaternionf> & quats) :
    _quats(quats)
{
    LM_ASSERT(quats.size() >= 4);
}
//------------------------------------------------------------------------------
Quaternionf CRQuatSpline::eval(float u) const
{
    double ind = 1.0 + u * double(_quats.size() - 3);
    int32 i1 = int32(L_floor(ind));
    float t = float(L_frac(ind));

    if(i1 > _quats.size() - 3)
    {
        i1 = _quats.size() - 3;
        t = 1.0;
    }

    LM_ASSERT(i1 >= 1);
    LM_ASSERT((i1 + 2) < _quats.size());

    const Quaternionf & q0 = _quats[i1 - 1];
    const Quaternionf & q1 = _quats[i1 + 0];
    const Quaternionf & q2 = _quats[i1 + 1];
    const Quaternionf & q3 = _quats[i1 + 2];
    
    Quaternionf q10(q0, q1, t + 1.0f);
    Quaternionf q11(q1, q2, t);
    Quaternionf q12(q2, q3, t - 1.0f);

    Quaternionf q20(q10, q11, 0.5f * (t + 1.0f));
    Quaternionf q21(q11, q12, 0.5f * t);

    return Quaternionf(q20, q21, t);
}
//------------------------------------------------------------------------------
}
