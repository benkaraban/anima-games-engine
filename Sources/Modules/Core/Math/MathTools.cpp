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
#include "MathTools.h"

#ifdef _DEBUG
#include <Core/Math/MathTools.inl>
#endif

namespace Core
{
//-----------------------------------------------------------------------------
bool quadraticFormula(float a, float b, float c, float & r1, float & r2)
{
    bool realRoots = false;
    float det = b*b - 4*a*c;

    if(det >= 0.0f)
    {
        float sqd = L_sqrt(det);
        float d = 0.5f / a;

        r1 = d * (-b - sqd);
        r2 = d * (-b + sqd);

        realRoots = true;
    }

    return realRoots;
}
//-----------------------------------------------------------------------------
uint16 floatToHalf(float v)
{
    union { float f; uint32 i; } val;
    val.f = v;

    int32 s =  (val.i >> 16) & 0x00008000;
    int32 e = ((val.i >> 23) & 0x000000ff) - (127 - 15);
    int32 m =   val.i        & 0x007fffff;

    if (e <= 0)
    {
        if (e < -10)
        {
            return 0;
        }
        m = (m | 0x00800000) >> (1 - e);
        return s | (m >> 13);
    }
    else if (e == 0xff - (127 - 15))
    {
        if (m == 0) // Inf
        {
            return s | 0x7c00;
        } 
        else    // NAN
        {
            m >>= 13;
            return s | 0x7c00 | m | (m == 0);
        }
    }
    else
    {
        if (e > 30) // Overflow
        {
            return s | 0x7c00;
        }

        return s | (e << 10) | (m >> 13);
    }
}
//-----------------------------------------------------------------------------
float halfToFloat(uint16 v)
{
    union { float f; uint32 i; } val;
    val.i = v;
    
    int32 s = (val.i >> 15) & 0x00000001;
    int32 e = (val.i >> 10) & 0x0000001f;
    int32 m =  val.i        & 0x000003ff;

    if (e == 0)
    {
        if (m == 0) // Plus or minus zero
        {
            return (float)(s << 31);
        }
        else // Denormalized number -- renormalize it
        {
            while (!(m & 0x00000400))
            {
                m <<= 1;
                e -=  1;
            }

            e += 1;
            m &= ~0x00000400;
        }
    }
    else if (e == 31)
    {
        if (m == 0) // Inf
        {
            return (float)((s << 31) | 0x7f800000);
        }
        else // NaN
        {
            return (float)((s << 31) | 0x7f800000 | (m << 13));
        }
    }

    e = e + (127 - 15);
    m = m << 13;
    
    val.i = (s << 31) | (e << 23) | m;
    return val.f;
}
//-----------------------------------------------------------------------------
}
