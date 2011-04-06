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
#ifndef WEB_JSON_H_
#define WEB_JSON_H_

#include <Core/Standard.h>
#include <json/json.h>

namespace Web
{
//-----------------------------------------------------------------------------
class JsonReader;
class LM_API_WEB JsonValue : public Core::Object, public Json::Value
{
    friend class JsonReader;

public:
    JsonValue();
    JsonValue(const Json::Value& value);

    String8 asString() const;

    JsonValue&       operator[] (ArrayIndex index)       {return *(JsonValue*)&(*(Json::Value*)(this))[index];}
    const JsonValue& operator[] (ArrayIndex index) const {return *(JsonValue*)&(*(Json::Value*)(this))[index];} 

    JsonValue&       operator[] (const char *key)        {return *(JsonValue*)&(*(Json::Value*)(this))[key];}
    const JsonValue& operator[] (const char *key) const  {return *(JsonValue*)&(*(Json::Value*)(this))[key];} 
};
//-----------------------------------------------------------------------------
class LM_API_WEB JsonReader : public Core::Object, public Json::Reader
{
public:
    JsonReader();
    bool parse(const String8& document, JsonValue& root, bool collectComments = true);
};
//-----------------------------------------------------------------------------
}

#endif /*WEB_JSON_H_*/