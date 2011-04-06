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
#include "FacebookApp.h"
#include <Social/SocialException.h>
#include <Web/OAuth2.h>
#include <Web/HttpRequest.h>
#include <Web/JSON.h>

namespace Social
{
bool fbAppCheckError(const String8& reponse)
{
    int32 pos = reponse.find("error");
    return (pos == String8::npos);
}
//-----------------------------------------------------------------------------
FacebookApp::FacebookApp (const String8& idApp, const String8& apiKey, const String8& secretKey) :
    _idApp(idApp), _apiKey(apiKey), _secretKey(secretKey)
{
}
//-----------------------------------------------------------------------------
String8 FacebookApp::getAuthenticationURL(const FacebookExtendedPermission& scope, const String8& type, const String8& display, const String8& state)
{
    const String8 authServer = "https://graph.facebook.com/oauth/authorize";
    const String8 redirectURI = "http://www.facebook.com/connect/login_success.html";

    Web::OAuth2UrlBuilder authUrlBuilder (_apiKey, _secretKey, redirectURI);

    return authUrlBuilder.getAuthenticationURL(authServer, type, display, scope.getPermissionsString(), state);
}
//-----------------------------------------------------------------------------
FacebookAccessToken FacebookApp::createFacebookAccessToken(const String8& redirectedUri, const FacebookExtendedPermission& scope)
{
    String8 accessToken = Web::OAuth2GetAccesToken(redirectedUri);
    FacebookAccessToken atoken (accessToken, _idApp, scope);
    return atoken;
}
//-----------------------------------------------------------------------------
void FacebookApp::analyticsAccessToken()
{
    Web::HttpRequest req;

    Web::Uri uri;
    uri.baseUri = "https://graph.facebook.com/oauth/access_token";
    
    uri.addQueryParam("grant_type", "client_credentials");
    uri.addQueryParam("client_id", _idApp);
    uri.addQueryParam("client_secret", _secretKey);

    req.initialize();
    req.setOption(Web::HTTP_REQUEST_SSL_NOT_VERIFY_SERVER);
    String8 content = req.makeRequest(uri);
    req.finalize();

    if (!fbAppCheckError(content))
    {
        String message (L"analyticsAccessToken : error when build analytics access_token");
        throw SocialException(message);
    }

    int32 pos     = content.find_first_of('=');
    int32 posLast = content.find_last_of('"');
    _analyticsAccessToken = content.substr(pos+1, posLast);
}
//-----------------------------------------------------------------------------
}  //namespace Social