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
#ifndef NETWORK_NETWORKENGINE_H_
#define NETWORK_NETWORKENGINE_H_

#include <Core/Standard.h>
#include <Core/Map.h>
#include <Core/RT/SyncQueue.h>
#include <Core/RT/Mutex.h>

#include "NetworkWatcher.h"
#include "NetworkIPBlacklist.h"

namespace Core
{
class Thread;
class ConditionVar;
}

namespace Network
{

class NetworkMessage;
class NetworkConnection;

class LM_API NetworkEngine: public Core::Object
{

public:
    NetworkEngine(uint32 socketMaxConnections = 10000);
    /**
     * Ce constructeur du NetworkEngine permet d'être notifié
     * via la ConditionVar quand il y a un évênement à traiter.
     */
    NetworkEngine(  Ptr<Core::ConditionVar> pAsyncNotification,
                    uint32 socketMaxConnections = 10000);
    virtual ~NetworkEngine();

    void launch();
    inline void stop() { _isBeingStopped = true; }

    void open(  const String8 & localAddress,
                const int16 localPort,
                ENetworkConnectionPrivilege connectionPrivilege);

    bool connect(const String8 & remoteAddress, const int16 remotePort);

    Ptr<NetworkMessage> getMessage();
    void sendMessage(Ptr<NetworkMessage> pNetworkMessage);

private:
    bool    _isBeingStopped;
    uint32  _socketMaxConnections;
    bool    _doAsyncNotification;
    int32   _nextSessionId;

    Ptr<Core::ConditionVar> _pAsyncNotification;

    Core::Thread * _pThread;
    NetworkIPBlacklist      _ipBlacklist;

    NetworkWatcher _networkWatcher;

    Core::Map<int32, Ptr<NetworkConnection> >   _clientSockets;
    Core::List<Ptr<NetworkConnection> >         _serverSockets;
    Core::SyncQueue<Ptr<NetworkMessage> >       _pendingNewMessages;

    Core::Mutex _mutex;

private:
    void _run();
    void _networkStartUp();
    void _networkCleanUp();
    void _deleteClosedConnection();
    void _manageActiveClientConnection();
    void _manageActiveServerConnection();
};

} // namespace Network

#endif /* NETWORK_NETWORKENGINE_H_ */