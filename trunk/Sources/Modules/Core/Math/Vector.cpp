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
#include <Core/Math/Vector.h>

#ifdef _DEBUG
#include <Core/Math/Vector2.inl>
#include <Core/Math/Vector3.inl>
#include <Core/Math/Vector4.inl>
#endif

#include <Core/Math/Random.h>

namespace Core
{
//-----------------------------------------------------------------------------
const Vector2f Vector2f::ZERO(0.0, 0.0);
const Vector2f Vector2f::ONE(1.0, 1.0);
const Vector3f Vector3f::ZERO(0.0, 0.0, 0.0);
const Vector3f Vector3f::ONE(1.0, 1.0, 1.0);
const Vector4f Vector4f::ZERO(0.0, 0.0, 0.0, 0.0);
const Vector4f Vector4f::ONE(1.0, 1.0, 1.0, 1.0);
//-----------------------------------------------------------------------------
void genTangentBasis(const Core::Vector3f & n, Core::Vector3f & t1, Core::Vector3f & t2)
{
    float x = abs(n.x);
    float y = abs(n.y);
    float z = abs(n.z);

    if(x < y)
        if(x < z)
            t1 = Core::Vector3f(1.0f, 0.0f, 0.0f);
        else // z < x < y
            t1 = Core::Vector3f(0.0f, 0.0f, 1.0f);
    else
        if(y < z)
            t1 = Core::Vector3f(0.0f, 1.0f, 0.0f);
        else // z < y < x
            t1 = Core::Vector3f(0.0f, 0.0f, 1.0f);

    t2 = cross(n, t1).getNormalizedSafe();
    t1 = cross(t2, n).getNormalizedSafe();
}
//-----------------------------------------------------------------------------
void randomize(Vector3f& vec)
{
    vec[0] = frand();
    vec[1] = frand();
    vec[2] = frand();
}
//-----------------------------------------------------------------------------
void randomize(Vector4f& vec)
{
    vec[0] = frand();
    vec[1] = frand();
    vec[2] = frand();
    vec[3] = frand();
}
//-----------------------------------------------------------------------------
void randomize(Vector3f& vec, const float min, const float max)
{
    vec[0] = frand(min, max);
    vec[1] = frand(min, max);
    vec[2] = frand(min, max);
}
//-----------------------------------------------------------------------------
void randomize(Vector4f& vec, const float min, const float max)
{
    vec[0] = frand(min, max);
    vec[1] = frand(min, max);
    vec[2] = frand(min, max);
    vec[3] = frand(min, max);
}
//-----------------------------------------------------------------------------
String toString(const Vector2f& vec)
{
    String result(L"[");
    result << Core::toString(vec.x) << L", ";
    result << Core::toString(vec.y) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Vector2f& vec, int32 precision)
{
    String result(L"[");
    result << Core::toString(vec.x, precision) << L", ";
    result << Core::toString(vec.y, precision) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Vector3f& vec)
{
    String result(L"[");
    for(int32 ii=0; ii < 2; ii++)
    result << Core::toString(vec[ii]) << L", ";
    result << Core::toString(vec[2]) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Vector3f& vec, int32 precision)
{
    String result(L"[");
    for(int32 ii=0; ii < 2; ii++)
    result << Core::toString(vec[ii], precision) << L", ";
    result << Core::toString(vec[2], precision) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Vector4f& vec)
{
    String result(L"[");
    for(int32 ii=0; ii < 3; ii++)
    result << Core::toString(vec[ii]) << L", ";
    result << Core::toString(vec[3]) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
String toString(const Vector4f& vec, int32 precision)
{
    String result(L"[");
    for(int32 ii=0; ii < 3; ii++)
    result << Core::toString(vec[ii], precision) << L", ";
    result << Core::toString(vec[3], precision) << L"]";
    return result;
}
//-----------------------------------------------------------------------------
} //namespace Core



