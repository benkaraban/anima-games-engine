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
#ifndef HOOUSERPROTOCOL_SENDUSERACTIVATIONCODE_H_
#define HOOUSERPROTOCOL_SENDUSERACTIVATIONCODE_H_

#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/UserAccountInfos.h>
#include "UserLogin.h"

namespace HOOUserProtocol
{

LM_ENUM_2 (ESACLangType,
           SACL_ENGLISH,
           SACL_FRENCH)

class LM_API_HUP SendActivationCode : public Core::Object, public IUserRequest
{
public:
    SendActivationCode()
    {}

    SendActivationCode(const Core::String & mail, ESACLangType lang)
    :   _mail(mail), _lang(lang)
    {}

    virtual ~SendActivationCode(){}

    virtual EUserRequestType getType() const { return USER_SEND_ACTIVATION_CODE; }

    const Core::String & getMail() const {return _mail;}
    ESACLangType getLang()const {return _lang;}

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _mail;
    ESACLangType  _lang;
};

LM_ENUM_2(ESendActivationCodeAnswerType, SAC_MAIL_SENDED,
                                         SAC_CAN_NOT_SEND_MAIL)

class LM_API_HUP SendActivationCodeAnswer : public Core::Object, public IUserAnswer
{
public:
    SendActivationCodeAnswer()
    {}

    SendActivationCodeAnswer(ESendActivationCodeAnswerType type)
    :   _answerType(type)
    {}

    virtual ~SendActivationCodeAnswer(){}

    virtual EUserAnswerType getType() const { return USER_SEND_ACTIVATION_CODE_ANSWER; }

    ESendActivationCodeAnswerType getSACAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ESendActivationCodeAnswerType    _answerType;
};


class LM_API_HUP UserLoginWithAcCode : public Core::Object, public IUserRequest
{
public:
    UserLoginWithAcCode()
    {}

    UserLoginWithAcCode(const Core::String & login, const String& acCode)
    :   _login(login), _acCode(acCode)
    {}

    virtual ~UserLoginWithAcCode(){}

    virtual EUserRequestType getType() const { return USER_LOGIN_WITH_ACTIVATION_CODE; }

    const Core::String & getLogin() const   {return _login;}
    const Core::String & getAcCode() const  {return _acCode;}

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _login;
    Core::String  _acCode;
};

class LM_API_HUP UserLoginWithAcCodeAnswer : public Core::Object, public IUserAnswer
{
public:
    UserLoginWithAcCodeAnswer()
    {}

    UserLoginWithAcCodeAnswer(EUserLoginAnswerType type, const String& login, const UserAccountInfos& accountInfos)
    :   _answerType(type), _login(login), _accountInfos(accountInfos)
    {}

    virtual ~UserLoginWithAcCodeAnswer(){}

    virtual EUserAnswerType getType() const { return USER_LOGIN_WITH_ACTIVATION_ANSWER; }

    EUserLoginAnswerType getUACAnswerType() const { return _answerType; }

    const String& getLogin() const {return _login;}
    const UserAccountInfos & getAccountInfos() const { return _accountInfos; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EUserLoginAnswerType    _answerType;
    String                  _login;
    UserAccountInfos        _accountInfos;
};

}

#endif