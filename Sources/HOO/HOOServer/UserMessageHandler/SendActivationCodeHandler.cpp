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
#include "SendActivationCodeHandler.h"

#include <Core/Logger.h>
#include <Core/Math/Random.h>
#include <Core/IO/Tools.h>
#include <Web/WebException.h>
#include <Web/Mail.h>
#include <HOOServer/DisconnectException.h>



namespace HOOServer
{
String generateAcCode()
{
    return Core::toString(Core::irand(0, Core::INT32_MAX));
}


SendActivationCodeHandler::SendActivationCodeHandler(UserSessionManager & sessionManager, const String& smtpServer, const String& smtpLogin, const String& smtpPassword, const String& smtpPort, const String& senderEmail,
                                                     const String& mailPathAcCode_en, const String& mailPathAcCode_fr, const String& mailPathAcCodeNologin_en, const String& mailPathAcCodeNoLogin_fr)
:   _sessionManager(sessionManager),
    _smtpServer(smtpServer),
    _smtpLogin(smtpLogin),
    _smtpPassword(smtpPassword),
    _smtpPort(smtpPort),
    _senderEmail(senderEmail)
{
    Ptr<Core::InputStream> pStream;
        pStream = Ptr<Core::InputStream> (new Core::FileInputStream(mailPathAcCode_en));
        String mailAcCode_en;
        Core::readAllStream(mailAcCode_en, *pStream);

        pStream = Ptr<Core::InputStream> (new Core::FileInputStream(mailPathAcCode_fr));
        String mailAcCode_fr;
        Core::readAllStream(mailAcCode_fr, *pStream);

        pStream = Ptr<Core::InputStream> (new Core::FileInputStream(mailPathAcCodeNologin_en));
        String mailAcCodeNologin_en;
        Core::readAllStream(mailAcCodeNologin_en, *pStream);

        pStream = Ptr<Core::InputStream> (new Core::FileInputStream(mailPathAcCodeNoLogin_fr));
        String mailAcCodeNologin_fr;
        Core::readAllStream(mailAcCodeNologin_fr, *pStream);
    pStream = null;


    _mails[SAC_MT_NO_LOGIN][HOOUserProtocol::SACL_ENGLISH] = mailAcCodeNologin_en;
    _mails[SAC_MT_NO_LOGIN][HOOUserProtocol::SACL_FRENCH]  = mailAcCodeNologin_fr;
    _mails[SAC_MT_ACCODE][HOOUserProtocol::SACL_ENGLISH]   = mailAcCode_en;
    _mails[SAC_MT_ACCODE][HOOUserProtocol::SACL_FRENCH]    = mailAcCode_fr;
}

SendActivationCodeHandler::~SendActivationCodeHandler()
{}

void SendActivationCodeHandler::handleMessage(HOOUserProtocol::EUserRequestType reqType, int32 idSession, Network::Message * request)
{ 
    if(!_sessionManager.getSession(idSession).isConnected())
        throw DisconnectException(L"Trying to log on a not connected session.", SESSION_NOT_OPENED);

    if(reqType == HOOUserProtocol::USER_SEND_ACTIVATION_CODE)
    {
        HOOUserProtocol::SendActivationCode sendAcCode;
        request->getMessage(sendAcCode);

        Core::List<String> logins;
        _sessionManager.getServerData().pDB->getLogins(sendAcCode.getMail(), logins);

        Core::List<String> activationsCodes;
        for (int32 ii=0; ii<logins.size(); ++ii)
            activationsCodes.push_back(generateAcCode());


        Core::TimeValue date = Core::Clock::universalTime() + Core::TimeDuration(48, 0, 0, 0);

        _sessionManager.getServerData().pDB->setActivationcode(logins, activationsCodes, date);
       

        Web::MailSender sender (_smtpServer, _smtpLogin, _smtpPassword, _smtpPort);

        String body;
        if (logins.size() == 0)
            body = getMailBody(SAC_MT_NO_LOGIN, sendAcCode.getLang());
        else
        {
            body = getMailBody(SAC_MT_ACCODE, sendAcCode.getLang());
            body += L"\n";
            for (int32 ii=0; ii<logins.size(); ++ii)
                body += L"login _ " + logins[ii] + L" Ac Code _ " + activationsCodes[ii] + L"\n";
        }

        Web::Mail mail;
            mail.from    = L"Heirs of Olympus - Anima Games";
            mail.replyTo = L"noreply";
            mail.subject = L"Heirs of Olympus - activation Code";
            mail.body    = body;

        try
        {
            sender.sendMail(mail, _senderEmail, sendAcCode.getMail());
            HOOUserProtocol::SendActivationCodeAnswer sendAcCodeAnswer (HOOUserProtocol::SAC_MAIL_SENDED);
            Network::Message answerMessage(Network::APPLICATION_MESSAGE, sendAcCodeAnswer, idSession);
            _sessionManager.sendAnswer(answerMessage);
        }
        catch(Web::WebException e)
        {
            HOOUserProtocol::SendActivationCodeAnswer sendAcCodeAnswer (HOOUserProtocol::SAC_CAN_NOT_SEND_MAIL);
            Network::Message answerMessage(Network::APPLICATION_MESSAGE, sendAcCodeAnswer, idSession);
            _sessionManager.sendAnswer(answerMessage);
        }
    }
    else if (reqType == HOOUserProtocol::USER_LOGIN_WITH_ACTIVATION_CODE)
    {
        HOOUserProtocol::UserLoginWithAcCode userLoginAcCode;
        request->getMessage(userLoginAcCode);

        HOOUserProtocol::UserAccountInfos accountInfos;

        HOOUserProtocol::EUserLoginAnswerType loginAnswer 
            = (HOOUserProtocol::EUserLoginAnswerType)_sessionManager.loginAcCode(idSession, userLoginAcCode.getLogin(), userLoginAcCode.getAcCode(), accountInfos);

        HOOUserProtocol::UserLoginWithAcCodeAnswer ulacAnswer (loginAnswer, userLoginAcCode.getLogin(), accountInfos);
        Network::Message answerMessage(Network::APPLICATION_MESSAGE, ulacAnswer, idSession);
        _sessionManager.sendAnswer(answerMessage);
    }


   
}


String  SendActivationCodeHandler::getMailBody(ESacMailType type, HOOUserProtocol::ESACLangType lang)
{
    return _mails[type][lang];
}

}//namespace HOOServer