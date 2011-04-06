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
#include <Core/Math/Transform.h>
#include <Core/Math/Quaternion.h>
#include <Core/Math/Matrix.h>

#ifdef _DEBUG
#include <Core/Math/Transform.inl>
#endif

namespace Core
{/*
//-----------------------------------------------------------------------------
void randomize(Transformf& transf)
{
    randomize(transf._scale);
    randomize(transf._scaleOrientation);
    randomize(transf._rotation);
    randomize(transf._translation);
}
//-----------------------------------------------------------------------------
void randomize(Transformf& transf, const float min, const float max)
{
    randomize(transf._scale, min, max);
    randomize(transf._scaleOrientation, min, max);
    randomize(transf._rotation, min, max);
    randomize(transf._translation, min, max);
}*/
//-----------------------------------------------------------------------------
String toString(const Transformf& transf)
{
    String result;

    result << toString(transf._scale);
    result << toString(transf._scaleOrientation);
    result << toString(transf._rotation);
    result << toString(transf._translation);

    return result;
}
//-----------------------------------------------------------------------------
String toString(const Transformf& transf, int32 precision)
{
    String result;

    result << toString(transf._scale, precision);
    result << toString(transf._scaleOrientation, precision);
    result << toString(transf._rotation, precision);
    result << toString(transf._translation, precision);

    return result;
}
//-----------------------------------------------------------------------------
}//namespace Core
