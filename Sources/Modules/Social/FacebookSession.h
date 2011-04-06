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
#ifndef SOCIAL_FACEBOOK_SESSION_H_
#define SOCIAL_FACEBOOK_SESSION_H_

#include <Core/Standard.h>
#include <Core/Map.h>
#include <Core/List.h>
#include <Social/FacebookExtendedPermissions.h>

namespace Social
{
//decrit un message poste sur le mur
struct LM_API_SCL FacebookPost
{
    String8                         message;
    String8                         picture;
    String8                         link;
    String8                         linkName;
    String8                         linkCaption;
    String8                         linkDescription;
    String8                         linkSource;
    Core::Map<String8, String8>     actions;
};

struct LM_API_SCL FacebookFriend
{
    String8 _name;
    String8 _id;
};
//-----------------------------------------------------------------------------
class LM_API_SCL FacebookAccessToken
{
public:
    FacebookAccessToken(const String8& accessToken, const String8& idApp, const FacebookExtendedPermission& permissions);
    const String8& getAccessToken() const {return _accessToken;}
    bool isValid();

private:
    String8                     _accessToken;
    FacebookExtendedPermission  _permissions;
    String8                     _idApp;
};
//-----------------------------------------------------------------------------
class LM_API_SCL FacebookSession : public Core::Object
{
public:
    FacebookSession (const FacebookAccessToken& accessToken);

    //necessite la permission : EP_PUBLISH_STREAM
    bool publishWallPost(const FacebookPost& post);
    bool publishStatus(const String8& statuts);

    bool getFriends(Core::List<FacebookFriend>& friends);

private:
    FacebookAccessToken _accessToken;
};
//-----------------------------------------------------------------------------
}

#endif /* SOCIAL_FACEBOOK_SESSION_H_ */