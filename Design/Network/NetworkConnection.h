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
#ifndef NETWORK_NETWORKCONNECTION_H_
#define NETWORK_NETWORKCONNECTION_H_

#include <Core/Standard.h>
#include <Core/RT/SyncQueue.h>

#include "NetworkEnum.h"

namespace Network
{

class NetworkMessage;

class NetworkConnection: public Core::Object
{

public:
    NetworkConnection(  SOCKET socketClient,
                        int32 sessionId,
                        ENetworkConnectionPrivilege connectionPrivilege = NETWORK_USER_CONNECTION)
    :   _socket(socketClient),
        _sessionId(sessionId),
        _connectionPrivilege(connectionPrivilege)
    {
    }
    virtual ~NetworkConnection();

public:
    inline ENetworkConnectionPrivilege getConnectionPrivilege() const { return _connectionPrivilege; }
    inline SOCKET   getSocket() const { return _socket; }
    inline int32    getSessionId() const { return _sessionId; }
    inline bool     isClosed() const { return (_socket == INVALID_SOCKET); }
    inline bool     isAdminConnection() const { return _connectionPrivilege == NETWORK_ADMIN_CONNECTION; }

    void sendMessage(Ptr<NetworkMessage> pMessageToSend);

private:
    friend class NetworkEngine;
    void _close();
    /**
     * Cette méthode est appelée par le NetworkEngine quand le NetworkWatcher
     * lui a signalé qu'il y a quelque chose à lire sur cette connection.
     */
    Ptr<NetworkMessage> _read();

private:
    SOCKET  _socket;
    int32   _sessionId;
    ENetworkConnectionPrivilege _connectionPrivilege;

};

} // namespace Network

#endif /* NETWORK_NETWORKCONNECTION_H_ */
