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
#ifndef HOOUSERPROTOCOL_USERLOGIN_H_
#define HOOUSERPROTOCOL_USERLOGIN_H_

#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/UserAccountInfos.h>

namespace HOOUserProtocol
{

class LM_API_HUP UserLogin : public Core::Object, public IUserRequest
{
public:
    UserLogin()
    {}

    UserLogin(const Core::String & login, const Core::String & password)
    :   _login(login),
        _password(password)
    {
    }

    virtual ~UserLogin(){}

    virtual EUserRequestType getType() const { return USER_LOGIN; }

    const Core::String & getLogin() const {return _login;}
    const Core::String & getPassword() const {return _password;}

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _login;
    Core::String  _password;
};

LM_ENUM_6(EUserLoginAnswerType, USER_LOGIN_OK,
                                USER_LOGIN_FAILED,
                                USER_PASSWD_FAILED,
                                USER_ALREADY_LOGGED,
                                USER_ACCOUNT_BANNED,
                                USER_ACCOUNT_LOCKED)

class LM_API_HUP UserLoginAnswer : public Core::Object, public IUserAnswer
{
public:
    UserLoginAnswer()
    {}

    UserLoginAnswer(EUserLoginAnswerType type, const UserAccountInfos & accountInfos)
    :   _answerType(type), _accountInfos(accountInfos)
    {}

    virtual ~UserLoginAnswer(){}

    virtual EUserAnswerType getType() const { return USER_LOGIN_ANSWER; }

    EUserLoginAnswerType getLoginAnswerType() const { return _answerType; }

    const UserAccountInfos & getAccountInfos() const { return _accountInfos; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EUserLoginAnswerType   _answerType;
    UserAccountInfos       _accountInfos;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_USERLOGIN_H_*/