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
#ifndef HOOUSERPROTOCOL_CHECKLOGINAVALABLE_H_
#define HOOUSERPROTOCOL_CHECKLOGINAVALABLE_H_

#include <HOOUserProtocol/IUserMessage.h>

namespace HOOUserProtocol
{

class LM_API_HUP CheckLoginAvailable : public Core::Object, public IUserRequest
{
public:
    CheckLoginAvailable()
    {}

    CheckLoginAvailable(const Core::String & login)
    :   _login(login)
    {}

    virtual ~CheckLoginAvailable(){}

    virtual EUserRequestType getType() const { return USER_CHECK_LOGIN_AVAILABLE; }

    const Core::String & getLogin() const {return _login;}

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    Core::String  _login;
};

LM_ENUM_4(ECheckLoginAvailableAnswerType,   USER_LOGIN_AVAILABLE,
                                            USER_LOGIN_NOT_AVAILABLE,
                                            USER_LOGIN_INCORRECT,
                                            USER_LOGIN_FORBIDDEN)

class LM_API_HUP CheckLoginAvailableAnswer : public Core::Object, public IUserAnswer
{
public:
    CheckLoginAvailableAnswer()
    {}

    CheckLoginAvailableAnswer(ECheckLoginAvailableAnswerType type, const Core::String & login)
    :   _answerType(type),
        _login(login)
    {}

    virtual ~CheckLoginAvailableAnswer(){}

    virtual EUserAnswerType getType() const { return USER_CHECK_LOGIN_AVAILABLE_ANSWER; }

    ECheckLoginAvailableAnswerType getLoginAvailability() const { return _answerType; }
    const Core::String & getLogin() const { return _login; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    ECheckLoginAvailableAnswerType  _answerType;
    Core::String                    _login;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_CHECKLOGINAVALABLE_H_*/