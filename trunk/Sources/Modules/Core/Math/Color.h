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
#ifndef CORE_COLOR_H_
#define CORE_COLOR_H_

#include <Core/Standard.h>
#include <Core/Math/Vector.h>

namespace Core
{
typedef int32 ColorARGB8;

inline ColorARGB8 toARGB8(int32 red, int32 green, int32 blue, int32 alpha)
{ return (alpha << 24) | (red << 16) | (green << 8) | blue; }

inline int32 alpha(ColorARGB8 color) { return (color >> 24) & 0xFF; }
inline int32 red  (ColorARGB8 color) { return (color >> 16) & 0xFF; }
inline int32 green(ColorARGB8 color) { return (color >>  8) & 0xFF; }
inline int32 blue (ColorARGB8 color) { return (color >>  0) & 0xFF; }

inline ColorARGB8 toColorB(const Core::Vector4f & colorf)
{
    return toARGB8(int32(255.0f * colorf.x), 
                   int32(255.0f * colorf.y), 
                   int32(255.0f * colorf.z), 
                   int32(255.0f * colorf.w));
};

inline Core::Vector4f toColorF(ColorARGB8 color)
{
    return Core::Vector4f(float(red(color)) / 255.0f,
                          float(green(color)) / 255.0f,
                          float(blue(color)) / 255.0f,
                          float(alpha(color)) / 255.0f);

}

LM_API_COR float SRGB2Linear(float x);
LM_API_COR float linear2SRGB(float x);

LM_API_COR Vector4f cvtSRGB2Linear(const Vector4f & color);
LM_API_COR Vector4f cvtLinear2SRGBF(const Vector4f & color);

LM_API_COR Vector4f SRGB2Linear(ColorARGB8 color);
LM_API_COR ColorARGB8 linear2SRGB(const Vector4f & color);

}

#endif /*IMAGES_COLOR_H_*/
