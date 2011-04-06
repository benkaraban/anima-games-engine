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
#include <UTests.h>

#include <Core/Standard.h>
#include <Social/FacebookApp.h>

void testFacebookClient()
{
    const String8 fbIdApp     = "159780717376888";
    const String8 fbApiKey    = "57676280c75dda2f0cc752e431784e2b";
    const String8 fbSecretKey = "f513fd4404c048b9e687594bc10ed0b7";

    const String8 type        = "user_agent";
    const String8 display     = "popup";
    const String8 state       = "LOL";
    Social::FacebookExtendedPermission scope;
        scope.addPermission (Social::EP_PUBLISH_STREAM);
        scope.addPermission (Social::EP_OFFLINE_ACCESS);

    Social::FacebookApp fbApp (fbIdApp, fbApiKey, fbSecretKey);

    String8 authUrl = fbApp.getAuthenticationURL(scope, type, display, state);

    //ouvrir une session
    const String8& redirectedUri = "http://www.facebook.com/connect/login_success.html#access_token=159780717376888|a4e9f5fc5c1adac0b646c944-100001653215329|FF5RkOILk3kx1JfIjLhvvul8qX0&expires_in=0&state=LOL";
    Social::FacebookAccessToken aToken = fbApp.createFacebookAccessToken(redirectedUri, scope);
    UTEST_EQU(aToken.isValid(),  true);

    Social::FacebookSession fbSession (aToken);

    //Wall Post
    UTEST_START_TEST(L"Facebook Wall Post")
    {
        Social::FacebookPost post;
            post.message            = "TUSocial Wall Post";
            post.link               = "http://www.chronorage.com";
            post.linkName           = "chronorage";
            post.linkCaption        = "best arena shooter";
            post.linkDescription    = "buy the game";
            post.linkSource         = "";
            post.picture            = "http://agsocialor.alwaysdata.net/facebook/agSocialorApp/1001.jpg";
            post.actions["link"]    = "http://www.anima-games.fr";
            post.actions["name"]    = "View on Anima Games";
        fbSession.publishWallPost(post);
    }
    UTEST_END_TEST;

    //Statuts
    fbSession.publishStatus("TUSocial test statuts");

    //Friends info
    Core::List<Social::FacebookFriend> friends;
    fbSession.getFriends(friends);

    UTEST_EQU(friends.size()   == 2                ,  true);
    UTEST_EQU(friends[0]._name == "Ag SocialorDeux",  true);
    UTEST_EQU(friends[0]._id   == "100001820977697",  true);
    UTEST_EQU(friends[1]._name == "Ag SocialorTrois", true);
    UTEST_EQU(friends[1]._id   == "100001825897109",  true);
}