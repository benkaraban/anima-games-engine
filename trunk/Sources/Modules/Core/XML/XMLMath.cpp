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
#include <Core/XML/XMLMath.h>
#include <Core/Strings/Tools.h>

namespace Core
{
//------------------------------------------------------------------------------
String toXMLString(const Vector2f & vec)
{
    String result;

    result << toString(vec.x) << L"|";
    result << toString(vec.y);

    return result;
}
//------------------------------------------------------------------------------
String toXMLString(const Vector3f & vec)
{
    String result;

    result << toString(vec.x) << L"|";
    result << toString(vec.y) << L"|";
    result << toString(vec.z);

    return result;
}
//------------------------------------------------------------------------------
String toXMLString(const Vector4f & vec)
{
    String result;

    result << toString(vec.x) << L"|";
    result << toString(vec.y) << L"|";
    result << toString(vec.z) << L"|";
    result << toString(vec.w);

    return result;
}
//------------------------------------------------------------------------------
String toXMLString(const Quaternionf & quat)
{
    String result;

    result << toString(quat._v.x) << L"|";
    result << toString(quat._v.y) << L"|";
    result << toString(quat._v.z) << L"|";
    result << toString(quat._w);

    return result;
}
//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------
void fromXMLString(const String & value, bool & v)
{
    v = (Core::toInteger(value) != 0);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, int32 & v)
{
    v = toInteger(value);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, float & v)
{
    v = float(toFloat(value));
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, double & v)
{
    v = toFloat(value);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, String & v)
{
    v = value;
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, Vector2f & vec)
{
    List<String> tokens;
    tokenize(value, tokens, L"|");

    if(tokens.size() != 2)
        throw Exception(L"Invalid XML encoded value");

    vec.x = (float)toFloat(tokens[0]);
    vec.y = (float)toFloat(tokens[1]);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, Vector3f & vec)
{
    List<String> tokens;
    tokenize(value, tokens, L"|");

    if(tokens.size() != 3)
        throw Exception(L"Invalid XML encoded value");

    vec.x = (float)toFloat(tokens[0]);
    vec.y = (float)toFloat(tokens[1]);
    vec.z = (float)toFloat(tokens[2]);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, Vector4f & vec)
{
    List<String> tokens;
    tokenize(value, tokens, L"|");

    if(tokens.size() != 4)
        throw Exception(L"Invalid XML encoded value");

    vec.x = (float)toFloat(tokens[0]);
    vec.y = (float)toFloat(tokens[1]);
    vec.z = (float)toFloat(tokens[2]);
    vec.w = (float)toFloat(tokens[3]);
}
//------------------------------------------------------------------------------
void fromXMLString(const String & value, Quaternionf & quat)
{
    List<String> tokens;
    tokenize(value, tokens, L"|");

    if(tokens.size() != 4)
        throw Exception(L"Invalid XML encoded value");

    quat._v.x = (float)toFloat(tokens[0]);
    quat._v.y = (float)toFloat(tokens[1]);
    quat._v.z = (float)toFloat(tokens[2]);
    quat._w = (float)toFloat(tokens[3]);
}
//------------------------------------------------------------------------------
}


