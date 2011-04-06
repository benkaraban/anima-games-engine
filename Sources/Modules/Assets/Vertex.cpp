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

#include <Core/Math/MathTools.h>
#include "Vertex.h"

namespace Assets
{
//-----------------------------------------------------------------------------
String toString(const Vertex & v)
{
    String result("Lv ");
    result << Core::toString(v.position);
    return result;
}
//-----------------------------------------------------------------------------
uint8 packUInt8N(float v)
{
    v = std::min(1.0f, std::max(-1.0f, v));
    return uint8(Core::L_round((v + 1.0) * 0.5 * float(Core::UINT8_MAX)));
}
//-----------------------------------------------------------------------------
uint8 packUInt8UN(float v)
{
    v = std::min(1.0f, std::max(0.0f, v));
    return uint8(Core::L_round(v * float(Core::UINT8_MAX)));
}
//-----------------------------------------------------------------------------
int16 packInt16N(float v)
{
    v = std::min(1.0f, std::max(-1.0f, v));
    return int16(Core::L_round(v * float(Core::INT16_MAX)));
}
//-----------------------------------------------------------------------------
void packUInt8Vec3(const Core::Vector3f & vec, uint8 packed [])
{
    packed[0] = packUInt8N(vec._x);
    packed[1] = packUInt8N(vec._y);
    packed[2] = packUInt8N(vec._z);
}
//-----------------------------------------------------------------------------
void packUInt8Vec4(const Core::Vector4f & vec, uint8 packed [])
{
    packed[0] = packUInt8N(vec._x);
    packed[1] = packUInt8N(vec._y);
    packed[2] = packUInt8N(vec._z);
    packed[3] = packUInt8N(vec._w);
}
//-----------------------------------------------------------------------------
void packColor(const Core::Vector4f & color, uint8 packed [])
{
    packed[2] = packUInt8UN(color._x);
    packed[1] = packUInt8UN(color._y);
    packed[0] = packUInt8UN(color._z);
    packed[3] = packUInt8UN(color._w);
}
//-----------------------------------------------------------------------------
Core::Vector4f unpackColor(const uint8 c [])
{
    return Core::Vector4f(unpackUInt8UN(c[2]),
                          unpackUInt8UN(c[1]),
                          unpackUInt8UN(c[0]),
                          unpackUInt8UN(c[3]));
}
//-----------------------------------------------------------------------------
void packInt16Vec4(const Core::Vector3f & vec, int16 packed [])
{
    packed[0] = packInt16N(vec._x);
    packed[1] = packInt16N(vec._y);
    packed[2] = packInt16N(vec._z);
    packed[3] = packInt16N(0.0);
}
//-----------------------------------------------------------------------------
void packInt16Vec3(const Core::Vector3f & vec, int16 packed [])
{
    packed[0] = packInt16N(vec._x);
    packed[1] = packInt16N(vec._y);
    packed[2] = packInt16N(vec._z);
}
//-----------------------------------------------------------------------------
void packInt16Vec2(const Core::Vector3f & vec, int16 packed [])
{
    packed[0] = packInt16N(vec._x);
    packed[1] = packInt16N(vec._y);
}
//-----------------------------------------------------------------------------
void packInt16Vec2(const Core::Vector2f & vec, int16 packed [])
{
    packed[0] = packInt16N(vec.x);
    packed[1] = packInt16N(vec.y);
}
//-----------------------------------------------------------------------------
float unpackUInt8N(uint8 v)
{
    return 2.0f *((float(v) / float(Core::UINT8_MAX)) - 0.5f);
}
//-----------------------------------------------------------------------------
float unpackUInt8UN(uint8 v)
{
    return float(v) / float(Core::UINT8_MAX);
}
//-----------------------------------------------------------------------------
float unpackInt16N(int16 v)
{
    return float(v) / float(Core::INT16_MAX);
}
//-----------------------------------------------------------------------------
Core::Vector3f unpackUInt8Vec3(const uint8 v [])
{
    return Core::Vector3f(unpackUInt8N(v[0]), unpackUInt8N(v[1]), unpackUInt8N(v[2]));
}
//-----------------------------------------------------------------------------
Core::Vector3f unpackInt16Vec3(const int16 v [])
{
    return Core::Vector3f(unpackInt16N(v[0]), unpackInt16N(v[1]), unpackInt16N(v[2]));
}
//-----------------------------------------------------------------------------
Core::Vector3f unpackInt16Vec2(const int16 v [])
{
    return Core::Vector3f(unpackInt16N(v[0]), unpackInt16N(v[1]), 0.0);
}
//-----------------------------------------------------------------------------
void packFloat16Vec2(const Core::Vector2f & vec, float16 packed [])
{
    packed[0] = Core::floatToHalf(vec.x);
    packed[1] = Core::floatToHalf(vec.y);
}
//-----------------------------------------------------------------------------
Core::Vector2f unpackFloat16Vec2(const float16 packed [])
{
    return Core::Vector2f(Core::halfToFloat(packed[0]), Core::halfToFloat(packed[1]));
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Vertex::Vertex() :
    position(0.0, 0.0, 0.0),
    normale(0.0, 0.0, 1.0),
    tangent(1.0, 0.0, 0.0),
    bitangent(0.0, 1.0, 0.0),
    texCoords(0.0, 0.0),
    blendingGroup(0),
    dummy(0.0)
{
    for(int32 iBone=0; iBone < MAX_BONES_PER_VERTEX; iBone++)
    {
        boneId[iBone] = 0;
        boneWeight[iBone] = 0.0;
    }
}
//-----------------------------------------------------------------------------
bool Vertex::operator == (const Vertex & vert) const
{
    return position == vert.position && normale == vert.normale
        && tangent == vert.tangent && bitangent == vert.bitangent
        && texCoords == vert.texCoords 
        && blendingGroup == vert.blendingGroup;
}
//-----------------------------------------------------------------------------
bool Vertex::operator != (const Vertex & vert) const
{
    return !(*this == vert);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
}
