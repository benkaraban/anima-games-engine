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
#ifndef HOOADMINPROTOCOL_ADMINLOGIN_H_
#define HOOADMINPROTOCOL_ADMINLOGIN_H_

#include <HOOAdminProtocol/IAdminMessage.h>

namespace HOOAdminProtocol
{

class LM_API_HAP AdminLogin : public Core::Object, public IAdminRequest
{
public:
    AdminLogin()
    {}

    AdminLogin(const Core::String & login, const Core::String & password);

    virtual ~AdminLogin(){}

    virtual EAdminRequestType getType() const { return ADMIN_LOGIN; }

    const Core::String & getLogin() const {return _login;}
    const Core::String & getPassword() const {return _password;}

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _login;
    Core::String  _password;
};

LM_ENUM_4(EAdminLoginAnswerType,    ADMIN_LOGIN_OK,
                                    ADMIN_LOGIN_FAILED,
                                    ADMIN_PASSWD_FAILED,
                                    NOT_AN_ADMIN_ACCOUNT)

class LM_API_HAP AdminLoginAnswer : public Core::Object, public IAdminAnswer
{
public:
    AdminLoginAnswer()
    {}

    AdminLoginAnswer(EAdminLoginAnswerType type)
    :   _loginAnswerType(type)
    {}

    virtual ~AdminLoginAnswer(){}

    virtual EAdminAnswerType getType() const { return ADMIN_LOGIN_ANSWER; }

    EAdminLoginAnswerType getLoginAnswerType() const { return _loginAnswerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EAdminLoginAnswerType   _loginAnswerType;
};

}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_ADMINLOGIN_H_*/