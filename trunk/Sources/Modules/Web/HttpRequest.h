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
#ifndef WEB_HTTP_REQUEST_H_
#define WEB_HTTP_REQUEST_H_

#include <Core/Standard.h>
#include <Core/Map.h>

typedef void CURL;

namespace Web
{
//-----------------------------------------------------------------------------
String8 doUrlEncoding(const String8& in);
String8 doUrlDecoding(const String8& in);
//-----------------------------------------------------------------------------
//Uniform Resource Identifier (URI) 
struct LM_API_WEB Uri
{
    Uri();
    Uri(const String8& strUri);  //passage de l'uri en chaine de caracter en struct uri

	String8 getUri() const;
    String8 getPost() const;     //uniquement query pour le moment, HTTP_BOUNDARY non gere

	void clear();
    void setBaseUri(const String8& base) {baseUri = base;}
    void addQueryParam(const String8& name, const String8& value);
    void addFragmentParam(const String8& name, const String8& value);

    String8                     baseUri;
    Core::Map<String8, String8> queryParams;
    Core::Map<String8, String8> fragmentParams;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_ENUM_1 (EHttpRequestOption,
           HTTP_REQUEST_SSL_NOT_VERIFY_SERVER)

class LM_API_WEB HttpRequest : public Core::Object
{
public:
    HttpRequest();

    void initialize();
    void finalize();
    void reset();

    String8 makeRequest(const Uri& uri);
    void setOption(EHttpRequestOption opt);
    

private:
    CURL*   _pCurl;
};
//-----------------------------------------------------------------------------
}

#endif //WEB_HTTP_REQUEST_H_