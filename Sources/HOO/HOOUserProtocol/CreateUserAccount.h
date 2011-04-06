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
#ifndef HOOUSERPROTOCOL_CREATEUSERACCOUNT_H_
#define HOOUSERPROTOCOL_CREATEUSERACCOUNT_H_

#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/UserAccountInfos.h>

namespace HOOUserProtocol
{

class LM_API_HUP CreateUserAccount : public Core::Object, public IUserRequest
{
public:
    CreateUserAccount()
    {}

    CreateUserAccount(const Core::String & login, const Core::String & password, const String & mail, bool newsletter)
    :   _login(login),
        _password(password),
        _mail(mail),
        _newsletter(newsletter)
    {}

    virtual ~CreateUserAccount(){}

    virtual EUserRequestType getType() const { return USER_CREATE_ACCOUNT; }

    const Core::String & getLogin() const {return _login;}
    const Core::String & getPassword() const {return _password;}
    const Core::String & getMail() const {return _mail;}
    bool isOKForNewsletter() const { return _newsletter; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _login;
    Core::String  _password;
    Core::String  _mail;
    bool          _newsletter;
};

LM_ENUM_6(ECreateUserAccountAnswerType, USER_ACCOUNT_CREATED,
                                        LOGIN_ALREADY_USED,
                                        LOGIN_INCORRECT,
                                        PWD_INCORRECT,
                                        MAIL_INCORRECT,
                                        LOGIN_FORBIDDEN)

class LM_API_HUP CreateUserAccountAnswer : public Core::Object, public IUserAnswer
{
public:
    CreateUserAccountAnswer()
    {}

    CreateUserAccountAnswer(ECreateUserAccountAnswerType type, const UserAccountInfos & accountInfos)
    :   _answerType(type), _accountInfos(accountInfos)
    {}

    virtual ~CreateUserAccountAnswer(){}

    virtual EUserAnswerType getType() const { return USER_CREATE_ACCOUNT_ANSWER; }

    ECreateUserAccountAnswerType getCreateAccountAnswerType() const { return _answerType; }

    const UserAccountInfos & getAccountInfos() const { return _accountInfos; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ECreateUserAccountAnswerType    _answerType;
    UserAccountInfos                _accountInfos;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_CREATEUSERACCOUNT_H_*/