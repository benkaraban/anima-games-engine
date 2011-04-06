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
#include "Watcher.h"

#include <Network/NetworkException.h>
#include <Network/Connection.h>

#ifndef _WIN32
#include <stdlib.h>
#endif

namespace Network
{
#ifdef _WIN32
Watcher::Watcher(   ConnectionPool & clientSockets,
                    Connection * serverSocket)
:   _clientSockets(clientSockets),
    _serverSocket(serverSocket),
    _timeOut(1000)
{}

Watcher::~Watcher()
{}

int32 Watcher::init()
{
    int32 connectionsCount = 0;

    FD_ZERO(&_mask );
    _highSocket = INVALID_SOCKET;

    Connection * pConnection = _clientSockets.getFirst();

    while(pConnection != NULL)
    {
        if(pConnection->getSocket() != INVALID_SOCKET)
        {
            
            FD_SET( pConnection->getSocket(), &_mask);
            if (_highSocket < pConnection->getSocket())
                _highSocket = pConnection->getSocket();

            ++connectionsCount;
        }

        pConnection = pConnection->getNext();
    }

    if(!_serverSocket->isClosed())
    {
        ++connectionsCount;
        FD_SET( _serverSocket->getSocket(), &_mask);
        if (_highSocket < _serverSocket->getSocket())
            _highSocket = _serverSocket->getSocket();
    }

    return connectionsCount;
}

int32 Watcher::watch()
{
    int32 selectReturnCode = 0;
    _timeval.tv_sec = _timeOut / 1000;
    _timeval.tv_usec = (_timeOut % 1000) * 1000;
    selectReturnCode = ::select(_highSocket + 1, &_mask, NULL, NULL, &_timeval);

    return selectReturnCode;
}

bool Watcher::hasSeen(Connection * pConnection)
{
    return ((pConnection->getSocket() != INVALID_SOCKET)
            && FD_ISSET( pConnection->getSocket(), &_mask ));
}

void Watcher::add(Connection * pConnection, bool serverSocket)
{
}

void Watcher::remove(Connection * pConnection)
{
}
#else
Watcher::Watcher(   ConnectionPool & clientSockets,
                    Connection * serverSocket)
:   _clientSockets(clientSockets),
    _serverSocket(serverSocket),
    _timeOut(1000)
{}

Watcher::~Watcher()
{}

int32 Watcher::init()
{
    int32 connectionsCount = 0;

    FD_ZERO(&_mask );
    _highSocket = INVALID_SOCKET;

    Connection * pConnection = _clientSockets.getFirst();

    while(pConnection != NULL)
    {
        if(pConnection->getSocket() != INVALID_SOCKET)
        {

            FD_SET( pConnection->getSocket(), &_mask);
            if (_highSocket < pConnection->getSocket())
                _highSocket = pConnection->getSocket();

            ++connectionsCount;
        }

        pConnection = pConnection->getNext();
    }

    if(!_serverSocket->isClosed())
    {
        ++connectionsCount;
        FD_SET( _serverSocket->getSocket(), &_mask);
        if (_highSocket < _serverSocket->getSocket())
            _highSocket = _serverSocket->getSocket();
    }

    return connectionsCount;
}

int32 Watcher::watch()
{
    int32 selectReturnCode = 0;
    _timeval.tv_sec = _timeOut / 1000;
    _timeval.tv_usec = (_timeOut % 1000) * 1000;
    selectReturnCode = ::select(_highSocket + 1, &_mask, NULL, NULL, &_timeval);

    return selectReturnCode;
}

bool Watcher::hasSeen(Connection * pConnection)
{
    return ((pConnection->getSocket() != INVALID_SOCKET)
            && FD_ISSET( pConnection->getSocket(), &_mask ));
}

void Watcher::add(Connection * pConnection, bool serverSocket)
{
}

void Watcher::remove(Connection * pConnection)
{
}
/*
Watcher::Watcher(   ConnectionPool & clientSockets,
                    Connection * serverSocket)
:   _clientSockets(clientSockets),
    _serverSocket(serverSocket),
    _timeOut(1000),
    _connectionCount(0),
    _events(NULL),
    _nbEvents(0)
{
    _epfd = epoll_create(SOCKET_MAX_CONNECTION);
    if(_epfd == 0)
    {
        String message(L"Error during the call of epoll_create in order to initialize epoll");
        throw NetworkException(message);
    }

    _events = new epoll_event[SOCKET_MAX_CONNECTION];//calloc(SOCKET_MAX_CONNECTION, sizeof(struct epoll_event));

    if(!serverSocket->isClosed())
    {
        std::cout<<"Add server socket"<<std::endl;

        epoll_event ev;
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
        ev.data.fd = serverSocket->getSocket();
        std::cout <<serverSocket->isClosed() << "  " << ev.data.fd <<std::endl;
        if(epoll_ctl(_epfd, EPOLL_CTL_ADD, serverSocket->getSocket(), &ev) < 0)
        {
            String message(L"Error during the call of epoll_ctl, adding server listen FD.");
            throw NetworkException(message);
        }
        _connectionCount++;
    }
}

Watcher::~Watcher()
{
    if(_events != NULL)
    {
        delete [] _events;
        _events = NULL;
    }
}

int32 Watcher::init()
{
    return _connectionCount;
}

int32 Watcher::watch()
{
    _nbEvents = epoll_wait(_epfd, _events, SOCKET_MAX_CONNECTION, _timeOut);

    return _nbEvents;
}

bool Watcher::hasSeen(Connection * pConnection)
{
    int32 iSeenEvent = 0;
    bool connectionSeen = false;

    while(iSeenEvent < _nbEvents && !connectionSeen)
    {
        if(pConnection->getSocket() == _events[iSeenEvent].data.fd)
            connectionSeen = true;

        iSeenEvent++;
    }

    return connectionSeen;
}

void Watcher::add(Connection * pConnection, bool serverSocket)
{
    epoll_event ev;
    if(serverSocket)
    {
        ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
    }
    else
    {
        ev.events = EPOLLIN | EPOLLET;
    }

    ev.data.fd = pConnection->getSocket();
    if(epoll_ctl(_epfd, EPOLL_CTL_ADD, pConnection->getSocket(), &ev) < 0)
    {
        String message(L"Error during the call of epoll_ctl ADD in order to add a new socket in epoll.");
        if(serverSocket)
            message << L" Fail while adding server listen FD";
        throw NetworkException(message);
    }

    _connectionCount++;
}

void Watcher::remove(Connection * pConnection)
{
    epoll_ctl(_epfd, EPOLL_CTL_DEL, pConnection->getSocket(), NULL);
    _connectionCount--;
}
*/
#endif

} // namespace Network
