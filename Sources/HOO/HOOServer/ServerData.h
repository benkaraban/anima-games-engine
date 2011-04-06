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
#ifndef HOOSERVER_SERVERDATA_H_
#define HOOSERVER_SERVERDATA_H_

#include <Core/RT/Mutex.h>
#include <Core/RT/ConditionVar.h>
#include <Core/RT/ConditionMutex.h>
#include <Core/XML/XMLDocument.h>

#include <Network/NetworkEngine.h>

#include <GameEngine/GameLibrary.h>

#include <HOOServer/Session.h>
#include <HOOServer/GamePool.h>
#include <HOOServer/Database/IDatabase.h>

namespace HOOServer
{

const int32 MAX_USERS_SESSIONS = Network::SOCKET_MAX_CONNECTION;
const int32 MAX_ADMIN_SESSIONS = 3;

struct ServerData : public Core::Object
{
    ServerData();
    virtual ~ServerData();

    int32                       version;
    int32                       verbosity;

    Core::XMLDocument           configFile;

    Core::ConditionMutex        conditionMutexUser;
    Core::ConditionVar          conditionUser;
    Network::NetworkEngine      userEngine;

    Core::ConditionMutex        conditionMutexAdmin;
    Core::ConditionVar          conditionAdmin;
    Network::NetworkEngine      adminEngine;
    
    Core::ConditionMutex        conditionMutexUpdater;
    Core::ConditionVar          conditionUpdater;
    Network::NetworkEngine      updaterEngine;

    Session                     userSessions[Network::TOLERANT_SOCKET_MAX_CONNECTION];
    Session                     adminSessions[Network::TOLERANT_SOCKET_MAX_CONNECTION];

    Core::Mutex                 openedUserSessionsMutex;
    Core::List<Session *>       openedUserSessions;

    Core::Mutex                 openedAdminSessionsMutex;
    Core::List<Session *>       openedAdminSessions;

    Core::Mutex                 lookingForOpponentUsersMutex;
    Core::List<Session *>       lookingForOpponentUsers;

    IDatabase *                 pDB;
    GameEngine::GameLibrary *   pGameLib;
    GamePool                    gamePool;

    Core::Mutex                 shutDownMutex;
    Core::TimeValue             shutDownTime;
    volatile int32              periodBeforeShutDown;

    volatile bool               isBeingKilled;
};

}//namespace HOOServer

#endif/*HOOSERVER_SERVERDATA_H_*/