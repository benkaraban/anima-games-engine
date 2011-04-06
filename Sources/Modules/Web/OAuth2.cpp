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
#include "OAuth2.h"
#include <Web/HttpRequest.h>
#include <Web/WebException.h>

namespace Web
{
//-----------------------------------------------------------------------------
OAuth2UrlBuilder::OAuth2UrlBuilder(const String8& client, const String8& secret, const String8& redirectUri) :
    clientID(client),
    clientSecretKey(secret),
    redirectUri(redirectUri)
{
}
//-----------------------------------------------------------------------------
String8 OAuth2UrlBuilder::getAuthenticationURL(const String8& authServer, const String8& type, const String8& display, const String8& scope, const String8& state)
{
    Uri authenticationURL;
        authenticationURL.setBaseUri(authServer);
        authenticationURL.addQueryParam("client_id", clientID);
        if (redirectUri!="")  authenticationURL.addQueryParam("redirect_uri", redirectUri);
        if (type!="")         authenticationURL.addQueryParam("type", type);
        if (display!="")      authenticationURL.addQueryParam("display", display);
        if (scope!="")        authenticationURL.addQueryParam("scope", scope);
        if (state!="")        authenticationURL.addQueryParam("state", state);

    return authenticationURL.getUri();
}
//-----------------------------------------------------------------------------
String8  OAuth2GetAccesToken (const String8& redirectedUri)
{
	String8::size_type idxQueryParam = redirectedUri.find_first_of("?");
	String8::size_type idxAnchor     = redirectedUri.find_first_of("#");

    if (std::string::npos == idxAnchor)
    {
        String message(L"OAuth2 : Invalid Argument : '#' is last character");
        throw WebException(message);
    }

    if(std::string::npos != idxQueryParam)
	{
        if (idxQueryParam >= idxAnchor)
        {
            String message(L"OAuth2 : Invalid Argument");
            throw WebException(message);
        }
	}

    Uri redirectedParams (redirectedUri);

	Core::Map<String8, String8>::const_iterator it = redirectedParams.fragmentParams.find("access_token");

	if(it == redirectedParams.fragmentParams.end())
	{
        String message(L"Unable to find access token from redirected URI");
        throw WebException(message);
	}

    return it->second;
}
//-----------------------------------------------------------------------------
}