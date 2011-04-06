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
#ifndef HOOSERVER_SERVER_H_
#define HOOSERVER_SERVER_H_

#include <Core/Standard.h>
#include <Core/RT/Thread.h>

#include <HOOServer/ServerData.h>
#include <HOOServer/AdminSessionManager.h>
#include <HOOServer/UserSessionManager.h>
#include <HOOServer/PlayerMatcher.h>

#include <HOOServer/NetworkMessageHandler/OpenSessionHandler.h>
#include <HOOServer/NetworkMessageHandler/CloseSessionHandler.h>

#include <HOOServer/AdminMessageHandler/IAdminMessageHandler.h>
#include <HOOServer/UserMessageHandler/IUserMessageHandler.h>
#include <HOOServer/UpdaterMessageHandler/IUpdaterMessageHandler.h>

namespace HOOServer
{

const int32 NB_THREADS = 12;

class LM_API_HOS Server : public Core::Object
{
public:
    Server(const String & configFileName);
    virtual ~Server();

    void launch();

private:
    void runClientThread();
    void runAdminThread();
    void runUpdaterThread();
    int32 getConfigValueInt32(const String & path);
    String getConfigValueString(const String & path);
    
private:
    String              _adminAddress;
    int32               _adminPort;
    String              _clientAddress;
    int32               _clientPort;
    String              _updaterAddress;
    int32               _updaterPort;
    int32               _connectionTimeOut;
    int32               _clientNbThreads;
    int32               _updaterNbThreads;

    Core::List<Core::Thread *>  _threads;

    ServerData          _serverData;

    UserSessionManager  _userSessionManager;
    AdminSessionManager _adminSessionManager;

    PlayerMatcher       _playerMatcher;

    OpenSessionHandler  _openAdminSessionHandler;
    CloseSessionHandler _closeAdminSessionHandler;

    OpenSessionHandler  _openUserSessionHandler;
    CloseSessionHandler _closeUserSessionHandler;

    IAdminMessageHandler *    _adminMessageHandlers[HOOAdminProtocol::EAdminRequestType_COUNT];
    IUserMessageHandler *     _userMessageHandlers[HOOUserProtocol::EUserRequestType_COUNT];
    IUpdaterMessageHandler *  _updaterMessageHandlers[HOOUpdaterProtocol::EUpdaterRequestType_COUNT];
};

}//namespace HOOServer

#endif/*HOOSERVER_SERVER_H_*/