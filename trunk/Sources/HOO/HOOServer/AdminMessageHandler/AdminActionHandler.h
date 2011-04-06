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
#ifndef HOOSERVER_ADMINACTIONHANDLER_H_
#define HOOSERVER_ADMINACTIONHANDLER_H_

#include <Core/Standard.h>

#include <HOOServer/AdminSessionManager.h>
#include <HOOServer/AdminMessageHandler/IAdminMessageHandler.h>

namespace HOOAdminProtocol
{
    class ConnectedUsersInfos;
    class ServerStats;
    class BanUsers;
    class UnbanUsers;
    class LockUsers;
    class UnlockUsers;
    class SendMessageToUsers;
}

namespace HOOServer
{

class LM_API_HOS AdminActionHandler : public Core::Object, public IAdminMessageHandler
{
public:
    AdminActionHandler(AdminSessionManager & sessionManager);
    virtual ~AdminActionHandler();

    virtual void handleMessage(HOOAdminProtocol::EAdminRequestType reqType, int32 idSession, Network::Message * request);

private:
    void connectedUsersInfosProcessing (const HOOAdminProtocol::ConnectedUsersInfos& msg, int32 idSession);
    void serverStatsProcessing         (const HOOAdminProtocol::ServerStats& msg,         int32 idSession);
    void banUsersProcessing            (const HOOAdminProtocol::BanUsers& msg,            int32 idSession);
    void unbanUsersProcessing          (const HOOAdminProtocol::UnbanUsers& msg,          int32 idSession);
    void lockUsersProcessing           (const HOOAdminProtocol::LockUsers& msg,           int32 idSession);
    void unlockUsersProcessing         (const HOOAdminProtocol::UnlockUsers& msg,         int32 idSession);
    void sendMessageToUsersProcessing  (const HOOAdminProtocol::SendMessageToUsers& msg,  int32 idSession);

private:
    AdminSessionManager & _sessionManager;
};

}//namespace HOOServer

#endif/*HOOSERVER_ADMINACTIONHANDLER_H_*/