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
#ifndef HOOADMINPROTOCOL_CONNECTEDUSERSINFOS_H_
#define HOOADMINPROTOCOL_CONNECTEDUSERSINFOS_H_

#include <HOOAdminProtocol/IAdminMessage.h>
#include <Core/List.h>
#include <Core/IO/Streams.h>

namespace HOOAdminProtocol
{
struct LM_API_HAP UserInfos : public Core::Serializable
{
    UserInfos();
    void read(Core::InputStream & inputStream);
    void write(Core::OutputStream & outputStream) const;

    bool loggedIn;
    String login;
    int32  qMatchStatus;
};

class LM_API_HAP ConnectedUsersInfos : public Core::Object, public IAdminRequest
{
public:
    ConnectedUsersInfos()
    {}

    virtual ~ConnectedUsersInfos(){}

    virtual EAdminRequestType getType() const { return CONNECTED_USERS_INFOS; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

};

class LM_API_HAP ConnectedUsersInfosAnswer : public Core::Object, public IAdminAnswer
{
public:
    ConnectedUsersInfosAnswer()
    {}

    ConnectedUsersInfosAnswer(const Core::List<UserInfos>& info)
    :   _infos(info)
    {}

    virtual ~ConnectedUsersInfosAnswer(){}

    virtual EAdminAnswerType getType() const { return CONNECTED_USERS_INFOS_ANSWER; }

    virtual void read(Core::InputStream & inputStream);
    virtual void write(Core::OutputStream & outputStream) const;

    const Core::List<UserInfos>& getUsersInfos() const { return _infos; }

protected:
    Core::List<UserInfos>   _infos;
};

}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_CONNECTEDUSERSINFOS_H_*/