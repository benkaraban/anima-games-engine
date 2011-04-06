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
#include "ConnectionPool.h"

#include <Network/NetworkException.h>

namespace Network
{
ConnectionPool::ConnectionPool()
:   _first(NULL)
{
    for(int32 ii=0; ii < TOLERANT_SOCKET_MAX_CONNECTION; ++ii)
    {
        _connections[ii].setSessionId(ii);
        _freeConnections.push(ii);
    }
}

ConnectionPool::~ConnectionPool()
{}

void ConnectionPool::setConnectionsTimeOut(int32 totalSeconds)
{
    for(int32 ii=0; ii < TOLERANT_SOCKET_MAX_CONNECTION; ++ii)
        _connections[ii].setTimeOut(totalSeconds);
}

Connection * ConnectionPool::getNewConnection()
{
    if(_freeConnections.size() <= 0)
        return NULL;

    int32 id = _freeConnections.front();
    _freeConnections.pop();
    
    _connections[id]._next = _first;
    _connections[id]._prev = NULL;
    
    if(_first != NULL)
        _first->_prev = &_connections[id];

    _first = &_connections[id];
    
    return _first;
}

Connection * ConnectionPool::getConnection(int32 sessionId)
{
    if(sessionId < 0 && sessionId >= TOLERANT_SOCKET_MAX_CONNECTION)
    {
        Core::String message;
        message << L"Invalid session id : "<<sessionId;
        throw Network::NetworkException(message);
    }

    return &_connections[sessionId];
}

void ConnectionPool::releaseConnection(Connection * connection)
{
    if(connection->_prev != NULL)
        connection->_prev->_next = connection->_next;

    if(connection->_next != NULL)
        connection->_next->_prev = connection->_prev;

    if(connection == _first)
    {
        _first = connection->_next;
        if(_first != NULL)
            _first->_prev = NULL;
    }

    int32 id = connection->getSessionId();
    _freeConnections.push(id);
}

}//namespace Network