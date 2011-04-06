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
#include "FacebookSession.h"
#include <Social/SocialException.h>
#include <Web/HttpRequest.h>
#include <Web/JSON.h>
#include <Core/Logger.h>
namespace Social
{
bool checkError(const String8& reponse)
{
    Web::JsonReader reader;
    Web::JsonValue  root;

    bool parsingSuccessful = reader.parse(reponse, root, false);

    if (!parsingSuccessful) return false;

    const Web::JsonValue data = root["error"];

    return data.size()==0;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FacebookAccessToken::FacebookAccessToken(const String8& accessToken, const String8& idApp, const FacebookExtendedPermission& permissions):
    _accessToken(accessToken),
    _permissions(permissions),
    _idApp(idApp)
{
}
//-----------------------------------------------------------------------------
bool FacebookAccessToken::isValid()
{
    Web::HttpRequest req;
    
    Web::Uri uri;
    uri.baseUri = "https://graph.facebook.com/me?access_token=" + getAccessToken();
                   
    req.initialize();
    req.setOption(Web::HTTP_REQUEST_SSL_NOT_VERIFY_SERVER);
    String8 content = req.makeRequest(uri);
    req.finalize();

    return checkError(content);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
FacebookSession::FacebookSession (const FacebookAccessToken& accessToken) :
    _accessToken(accessToken)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool FacebookSession::publishWallPost(const FacebookPost& post)
{
    Web::HttpRequest req;

    Web::Uri uri;
    uri.baseUri = "https://graph.facebook.com/feed";
    uri.addQueryParam("access_token", _accessToken.getAccessToken());

    if (post.message            !="")   uri.addQueryParam("message", post.message);
    if (post.picture            !="")   uri.addQueryParam("picture", post.picture);
    if (post.link               !="")   uri.addQueryParam("link", post.link);
    if (post.linkName           !="")   uri.addQueryParam("name", post.linkName);
    if (post.linkCaption        !="")   uri.addQueryParam("caption", post.linkCaption);
    if (post.linkDescription    !="")   uri.addQueryParam("description", post.linkDescription);
    if (post.linkSource         !="")   uri.addQueryParam("source", post.linkSource);
    if (post.actions.size() > 0)
    {
        
        Core::Map<String8, String8>::const_iterator it = post.actions.begin();

        String8 acts;
        acts += "{";
        acts += '"' + it->first + '"' + ": " + '"' + it->second + '"';
        ++it;

        while (it!=post.actions.end())
        {
            acts += ", ";
            acts += '"' + it->first + '"' + ": " + '"' + it->second + '"';
            ++it;
        }

        acts += "}";
        uri.addQueryParam("actions", acts);
    }

    req.initialize();
    req.setOption(Web::HTTP_REQUEST_SSL_NOT_VERIFY_SERVER);
    String8 content = req.makeRequest(uri);
    req.finalize();

    return checkError(content);
}
//-----------------------------------------------------------------------------
bool FacebookSession::publishStatus(const String8& statuts)
{
    Web::HttpRequest req;

    Web::Uri uri;
    uri.baseUri = "https://graph.facebook.com/feed";
    uri.addQueryParam("access_token", _accessToken.getAccessToken());

    uri.addQueryParam("message", statuts);

    req.initialize();
    req.setOption(Web::HTTP_REQUEST_SSL_NOT_VERIFY_SERVER);
    String8 content = req.makeRequest(uri);
    req.finalize();

    return checkError(content);
}
//-----------------------------------------------------------------------------
bool FacebookSession::getFriends(Core::List<FacebookFriend>& friends)
{
    Web::HttpRequest req;
    
    Web::Uri uri;
    uri.baseUri = "https://graph.facebook.com/me/friends?access_token=" + _accessToken.getAccessToken();

    req.initialize();
    req.setOption(Web::HTTP_REQUEST_SSL_NOT_VERIFY_SERVER);
    String8 content = req.makeRequest(uri);
    req.finalize();

    if (!checkError(content)) return false;

    Web::JsonReader reader;
    Web::JsonValue  root;
    bool parsingSuccessful = reader.parse(content, root, true);

    if (!parsingSuccessful) return false;

    friends.clear();

    Web::JsonValue data = root["data"];
    int32 nbData        = data.size();

    for (int32 ii=0; ii<nbData; ++ii)
    {
        FacebookFriend fbFriend;
            fbFriend._name = data[ii]["name"].asString();
            fbFriend._id   = data[ii]["id"].asString();

        friends.push_back(fbFriend);
    }

    return true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}