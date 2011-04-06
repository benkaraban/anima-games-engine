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
#include "NetworkEngine.h"

#include <Network/Defines.h>
#include <Network/NetworkException.h>
#include <Network/Message.h>
#include <Network/Connection.h>

#include <Core/RT/Thread.h>
#include <Core/RT/ConditionVar.h>
#include <Core/Logger.h>

namespace Network
{
int32   NetworkEngine::_enginesCount = 0;
bool    NetworkEngine::_isWSAStarted = false;

NetworkEngine::NetworkEngine(Core::ConditionVar * pAsyncNotification, Core::ConditionMutex * pConditionMutex)
:   _isBeingStopped(false),
    _pAsyncNotification(pAsyncNotification),
    _pConditionMutex(pConditionMutex),
    _pThread(NULL),
    _networkWatcher(_clientSockets, &_serverSocket)
{
    if(_enginesCount == 0)
        networkStartUp();
    _enginesCount ++;
}

NetworkEngine::~NetworkEngine()
{
    {
        Core::ScopedLock scopedLock(_mutex);
        _isBeingStopped = true;
    }

    if(_pThread != NULL)
    {
        _pThread->join();
        delete _pThread;
        _pThread = NULL;
    }
    
    if(!_serverSocket.isClosed())
    {
        _serverSocket.close();
    }

    _enginesCount --;

    if(_enginesCount == 0)
        networkCleanUp();
}

String NetworkEngine::getHostIP()
{
    if(_enginesCount == 0)
        networkStartUp();

    static const int32 MAX_LENGTH = 1024;
    char hostName[MAX_LENGTH];

    if(gethostname(hostName, MAX_LENGTH) == SOCKET_ERROR)
        throw NetworkException(L"error retrieving host name");

    struct addrinfo * pResult   = NULL;
    struct addrinfo * pAddress  = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if(getaddrinfo(hostName, NULL, &hints, &pResult) != 0)
        throw NetworkException(L"error retrieving host address");

    String result;
    pAddress = pResult;

    while(pAddress != NULL)
    {
        if(pAddress->ai_family == AF_INET)
        {
            sockaddr_in * pIP = (struct sockaddr_in *)pAddress->ai_addr;
            result = String(String8(inet_ntoa(pIP->sin_addr)));
            break;
        }

        pAddress = pAddress->ai_next;
    }

    freeaddrinfo(pResult);

    if(_enginesCount == 0)
        networkCleanUp();

    return result;
}

void NetworkEngine::setConnectionsTimeOut(int32 totalSeconds)
{
    _clientSockets.setConnectionsTimeOut(totalSeconds);
}

void NetworkEngine::open(   const String & localAddress,
                            const int16 localPort)
{
    String8 localAddress8(localAddress);

    if(!_serverSocket.isClosed())
    {
        String message(L"Server socket already opened.");
        throw NetworkException(message);
    }

    SOCKET serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket == INVALID_SOCKET)
    {
        String message(L"Failed to create a new socket().");
        throw NetworkException(message);
    }

    sockaddr_in sin;
    ::memset(&sin, 0, sizeof sin);

    sin.sin_family = AF_INET;
    if (localAddress8.compare("") == 0)
        sin.sin_addr.s_addr = INADDR_ANY;
    else
        sin.sin_addr.s_addr = ::inet_addr(localAddress8.c_str());
    sin.sin_port = htons(localPort);

    if (::bind(serverSocket, (sockaddr *) &sin, sizeof(sin)) == SOCKET_ERROR)
    {
        String message(L"Failed to bind() on the newly created socket.");
        throw NetworkException(message);
    }

    if (::listen(serverSocket, SOCKET_MAX_CONNECTION) == SOCKET_ERROR)
    {
        String message(L"Failed to listen() on the newly created socket.");
        throw NetworkException(message);
    }
    else
    {
        Core::ScopedLock scopedLock(_mutex);
        _serverSocket.setSocket(serverSocket);
    }

    _networkWatcher.add(&_serverSocket, true);
}

long NetworkEngine::connect(const String & remoteAddress,
                            const short remotePort)
{
    String8 remoteAddress8(remoteAddress);

    long sessionId = -1;

    sockaddr_in sin;
    SOCKET connection = ::socket(AF_INET, SOCK_STREAM, 0);
    sin.sin_addr.s_addr = ::inet_addr(remoteAddress8.c_str());
    sin.sin_family = AF_INET;
    sin.sin_port = ::htons(remotePort);

    int32 connectReturnCode = 0;
    connectReturnCode = ::connect(connection, reinterpret_cast<sockaddr *> (&sin), sizeof(sin));
    if (connectReturnCode == 0)
    {
        Core::ScopedLock scopedLock(_mutex);
        Connection * pNewConnection = _clientSockets.getNewConnection();

        if(pNewConnection == NULL)
        {
            ::closesocket(connection);
            String message(L"The maximum number of simultaneous connections has been reached.");
            throw NetworkException(message);
        }

        pNewConnection->setSocket(connection);
        sessionId = pNewConnection->getSessionId();

        _networkWatcher.add(pNewConnection, false);
    }
    return sessionId;
}

bool NetworkEngine::getMessage(Message* & pMessage)
{
    bool returnCode = false;

    if(!_pendingNewMessages.empty())
    {
        pMessage = _pendingNewMessages.front();

        _pendingNewMessages.pop();
        returnCode = true;
    }

    return returnCode;
}

void NetworkEngine::sendMessage(Message & message)
{
    Core::ScopedLock scopedLock(_mutex);

    int32 sessionId = message.getSessionId();
    Connection * pConnection = _clientSockets.getConnection(sessionId);

    if(pConnection != NULL)
    {
        if(message.getMessageType() == NETWORK_CLOSE_SESSION_MESSAGE)
        {
            pConnection->close();
            _clientSockets.releaseConnection(pConnection);
        }
        else
        {
            pConnection->sendMessage(message);
        }
    }
}

void NetworkEngine::launch()
{
    _isBeingStopped = false;
    _pendingNewMessages.clear();

    _pThread = new Core::Thread(&NetworkEngine::run, *this);
}

void NetworkEngine::run()
{
    int32 minTimeOut = 500;// 500 millisecondes

    _networkWatcher.setTimeOut(minTimeOut);

    int32 seenNumber = 0;

    while(!_isBeingStopped)
    {
        // On supprime les connections fermée
        deleteClosedConnection();

        if(_networkWatcher.init() == 0)
        {
            _isBeingStopped = true;
        }
        else
        {
            seenNumber = _networkWatcher.watch();

            if(seenNumber > 0)
            {
                manageActiveClientConnection();
                manageActiveServerConnection();
            }
        }
    }
}

void NetworkEngine::deleteClosedConnection()
{
    if(_serverSocket.isClosed())
    {
        Core::ScopedLock scopedLock(_mutex);
        _networkWatcher.remove(&_serverSocket);
    }

    Connection * pConnection = _clientSockets.getFirst();

    while(pConnection != NULL)
    {
        Connection * pNext = pConnection->getNext();

        if(pConnection->isClosed())
        {
            Core::ScopedLock scopedLock(_mutex);

            _networkWatcher.remove(pConnection);
            _clientSockets.releaseConnection(pConnection);
        }
        else if(pConnection->hasTimedOut())
        {
            int32 sessionId = pConnection->getSessionId();

            {
                Core::ScopedLock scopedLock(_mutex);

                pConnection->close();
                _networkWatcher.remove(pConnection);
                _clientSockets.releaseConnection(pConnection);
            }
            {
                Core::ScopedCondLock lock(*_pConditionMutex);

                _pendingNewMessages.push(new Network::Message(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId));
                _pAsyncNotification->notifyAll();
            }
        }

        pConnection = pNext;
    }
}

void NetworkEngine::manageActiveClientConnection()
{
    Connection * pConnection = _clientSockets.getFirst();

    while(pConnection != NULL)
    {
        if( (pConnection->getSocket() != INVALID_SOCKET)
        &&  (_networkWatcher.hasSeen(pConnection)) )
        {
            Core::ScopedCondLock lock(*_pConditionMutex);
            _pendingNewMessages.push(pConnection->read());

            _pAsyncNotification->notifyAll();
        }

        pConnection = pConnection->getNext();
    }
}

void NetworkEngine::manageActiveServerConnection()
{
    sockaddr_in csin;

    if( !_serverSocket.isClosed() )
    {
        if( ( _serverSocket.getSocket() != INVALID_SOCKET )
        && _networkWatcher.hasSeen(&_serverSocket) )
        {
            SOCKADDR_SIZE sockaddrSize = sizeof(csin);
            SOCKET incomingConnection = ::accept(_serverSocket.getSocket(), reinterpret_cast<sockaddr *>(&csin), &sockaddrSize);

            uint32 ipAdress = csin.sin_addr.s_addr;
            ENetworkIPStatus blacklisted = _ipBlacklist.isClientBlacklisted( ipAdress );

            Connection * pConnection = _clientSockets.getNewConnection();

            if(pConnection == NULL)//Max connection reached
            {
                Connection conn;
                conn.setSocket(incomingConnection);
                conn.setSessionId(-1);
                
                Message messageToSend(NETWORK_MAX_CONNECTION_REACHED);
                conn.sendMessage(messageToSend);
                conn.close();
            }
            else if( blacklisted != NETWORK_IP_AUTHORISED )
            {
                pConnection->setSocket(incomingConnection);
                pConnection->close();
                _clientSockets.releaseConnection(pConnection);
            }
            else
            {
                pConnection->setSocket(incomingConnection);
                _networkWatcher.add(pConnection);

                {
                    Message * pOpenSessionMessage = new Message(NETWORK_OPEN_SESSION_MESSAGE, pConnection->getSessionId());
                    Core::ScopedCondLock lock(*_pConditionMutex);
                    _pendingNewMessages.push(pOpenSessionMessage);

                    _pAsyncNotification->notifyAll();
                }
            }
        }
    }
}

void NetworkEngine::networkStartUp()
{
#ifdef _WIN32
    if(_isWSAStarted)
        return;

    WSADATA wsaData;
    WORD version;
    int error;

    version = MAKEWORD( 2, 0 );
    error = WSAStartup( version, &wsaData );

    //error occured
    if ( error != 0 )
    {
        String message(L"Error during the call of WSAStartup in order to initialize socket support");
        throw NetworkException(message);
    }

    // check for correct version
    if ( LOBYTE( wsaData.wVersion ) != 2 ||
            HIBYTE( wsaData.wVersion ) != 0 )
    {
        // incorrect WinSock version
        WSACleanup();
        String message(L"Incorrect Winsock version. Expecting v2.0.\nFound v");
        message << Core::toString(LOBYTE( wsaData.wVersion ) );
        message << L".";
        message << Core::toString(LOBYTE( wsaData.wVersion ) );
        message << L".";
        throw NetworkException(message);
    }

    _isWSAStarted = true;
#endif
}

void NetworkEngine::networkCleanUp()
{
#ifdef _WIN32
    WSACleanup();
    _isWSAStarted = false;
#endif
}

} // namespace Network
