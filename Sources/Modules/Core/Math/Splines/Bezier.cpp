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
#include <Core/Math/Splines/Bezier.h>

namespace Core
{
//------------------------------------------------------------------------------
BezierCurve::BezierCurve(BezierPoints_t b, 
                         const Vector3f & p1, 
                         const Vector3f & p2, 
                         const Vector3f & p3, 
                         const Vector3f & p4) :
    p1(p1), p2(p2), p3(p3), p4(p4)
{
}
//------------------------------------------------------------------------------
BezierCurve::BezierCurve(BezierTangents_t b, 
                         const Vector3f & p1, 
                         const Vector3f & t1, 
                         const Vector3f & p2, 
                         const Vector3f & t2) :
    p1(p1), p2(p1 + t1), p3(p2 - t2), p4(p2)
{
}
//------------------------------------------------------------------------------
Vector3f BezierCurve::eval(float t) const
{
    float t2 = t * t;
    float t3 = t * t2;
    float u  = 1.0f - t;
    float u2 = u * u;
    float u3 = u * u2;
    
    return p1 * u3 + 
           p2 * (3.0f * t * u2) + 
           p3 * (3.0f * t2 * u) + 
           p4 * t3;
}
//------------------------------------------------------------------------------
}
