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
#ifndef HOOUSERPROTOCOL_USERCONNECT_H_
#define HOOUSERPROTOCOL_USERCONNECT_H_

#include <HOOUserProtocol/IUserMessage.h>

namespace HOOUserProtocol
{

const int32 USER_PROTOCOL_VERSION = 1;

class LM_API_HUP UserConnect : public Core::Object, public IUserRequest
{
public:
    UserConnect();

    virtual ~UserConnect(){}

    virtual EUserRequestType getType() const { return USER_CONNECT; }

    int32 getVersion() const {return _version;}
    
    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    int32   _version;
};

LM_ENUM_2(EUserConnectAnswerType,   USER_CONNECT_OK,
                                    USER_CONNECT_VERSION_ERROR)

class LM_API_HUP UserConnectAnswer : public Core::Object, public IUserAnswer
{
public:
    UserConnectAnswer()
    {}

    UserConnectAnswer(EUserConnectAnswerType type)
    :   _answerType(type)
    {}

    virtual ~UserConnectAnswer(){}

    virtual EUserAnswerType getType() const { return USER_CONNECT_ANSWER; }

    EUserConnectAnswerType getConnectAnswerType() const { return _answerType; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

protected:
    EUserConnectAnswerType   _answerType;
};

}//namespace HOOUserProtocol

#endif/*HOOUSERPROTOCOL_USERCONNECT_H_*/