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
#include <Core/Math/Color.h>

namespace Core
{
float SRGB2Linear(float x)
{
    if(x <= 0.04045f)
    {
        return Core::clamp(0.0f, 1.0f, x / 12.92f);
    }
    else
    {
        return Core::clamp(0.0f, 1.0f, Core::L_pow((x + 0.055f) / 1.055f, 2.4f));
    }
}

float linear2SRGB(float x)
{
    if(x <= 0.00031308f)
    {
        return Core::clamp(0.0f, 1.0f, 12.92f * x);
    }
    else
    {
        return Core::clamp(0.0f, 1.0f, 1.055f * Core::L_pow(x, (1.0f / 2.4f)) - 0.055f);
    }
}

Core::Vector4f cvtSRGB2Linear(const Core::Vector4f & color)
{
    return Core::Vector4f(SRGB2Linear(color.r),
                          SRGB2Linear(color.g),
                          SRGB2Linear(color.b),
                          color.a);
}

Core::Vector4f cvtLinear2SRGBF(const Core::Vector4f & color)
{
    return Core::Vector4f(linear2SRGB(color.r),
                          linear2SRGB(color.g),
                          linear2SRGB(color.b),
                          color.a);
}

Core::Vector4f SRGB2Linear(ColorARGB8 color)
{
    return cvtSRGB2Linear(toColorF(color));
}

ColorARGB8 linear2SRGB(const Core::Vector4f & color)
{
    return toColorB(cvtLinear2SRGBF(color));
}

}
