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
#ifndef SOCIAL_FACEBOOK_APP_H_
#define SOCIAL_FACEBOOK_APP_H_

#include <Core/Standard.h>
#include <Social/FacebookSession.h>

namespace Social
{
//-----------------------------------------------------------------------------
class LM_API_SCL FacebookApp : public Core::Object
{
public:
    FacebookApp (const String8& idApp, const String8& apiKey, const String8& secretKey);

    String8 getAuthenticationURL(const FacebookExtendedPermission& scope, const String8& type, const String8& display, const String8& state);
    FacebookAccessToken createFacebookAccessToken(const String8& redirectedUri, const FacebookExtendedPermission& scope);

private:
    void analyticsAccessToken();

private:
    String8 _idApp;
    String8 _apiKey;
	String8 _secretKey;
    String8 _analyticsAccessToken;
};
//-----------------------------------------------------------------------------
}

#endif /* SOCIAL_FACEBOOK_APP_H_ */