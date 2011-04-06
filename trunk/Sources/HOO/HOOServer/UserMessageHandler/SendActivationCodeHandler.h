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
#ifndef HOOSERVER_SENDACTIVATIONCODEHANDLER_H_
#define HOOSERVER_SENDACTIVATIONCODEHANDLER_H_

#include <Core/Standard.h>

#include <HOOServer/UserSessionManager.h>
#include <HOOServer/UserMessageHandler/IUserMessageHandler.h>
#include <HOOUserProtocol/SendActivationCode.h>

namespace HOOServer
{

LM_ENUM_2 (ESacMailType,
           SAC_MT_NO_LOGIN,
           SAC_MT_ACCODE)
class LM_API_HOS SendActivationCodeHandler : public Core::Object, public IUserMessageHandler
{
public:
    SendActivationCodeHandler(UserSessionManager & sessionManager, const String& smtpServer, const String& smtpLogin, const String& smtpPassword, const String& smtpPort, const String& senderEmail,
        const String& mailPathAcCode_en, const String& mailPathAcCode_fr, const String& mailPathAcCodeNologin_en, const String& mailPathAcCodeNoLogin_fr);
    virtual ~SendActivationCodeHandler();

    virtual void handleMessage(HOOUserProtocol::EUserRequestType reqType, int32 idSession, Network::Message * request);

private:
    String getMailBody(ESacMailType type, HOOUserProtocol::ESACLangType lang);
    
private:
    UserSessionManager & _sessionManager;
    String               _smtpServer;
    String               _smtpLogin;
    String               _smtpPassword;
    String               _smtpPort;
    String               _senderEmail;
    String               _mails[ESacMailType_COUNT][HOOUserProtocol::ESACLangType_COUNT];
};

}

#endif