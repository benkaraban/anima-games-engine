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
#include "Connection.h"

#include <Network/NetworkException.h>
#include <Network/Message.h>

namespace Network
{

Connection::~Connection()
{
    if (_socket != INVALID_SOCKET)
    {
        ::closesocket(_socket);
        _socket = INVALID_SOCKET;
    }
}

void Connection::setSocket(SOCKET socket)
{
    _socket = socket;
    Core::Clock clock;
    _latestActivity = clock.universalTime();
}

void Connection::setTimeOut(int32 totalSeconds)
{
    _timeOut = totalSeconds;
}

bool Connection::hasTimedOut()
{
    if(_timeOut == 0)
        return false;

    Core::Clock clock;
    Core::TimeValue timeValue = clock.universalTime();

    uint32 nTickCount = (uint32)((timeValue - _latestActivity).totalSeconds());

    return (nTickCount > _timeOut);
}

void Connection::close()
{
    if (_socket != INVALID_SOCKET)
    {
        ::closesocket(_socket);
        _socket = INVALID_SOCKET;
    }
}

void Connection::sendMessage(Message & messageToSend)
{
    if (isClosed())
    {
        String message(L"Trying to SEND a message on a closed socket.");
        throw NetworkException(message);
    }
    else
    {
        int32 messageSize = messageToSend.getSize();
        int32 sendReturnCode = 0;
        byte buffer[NETWORK_MESSAGE_MAX_LENGTH];
        messageToSend.serialize(buffer);

        sendReturnCode = ::send(_socket, reinterpret_cast<const char *> (buffer), messageSize, 0);

        if (sendReturnCode == SOCKET_ERROR)
        {
            //On ne ferme pas la socket car elle sera fermée automatiquement au prochain read.
            //close();
            String message(L"An error as occurred while trying to send a message.");
            throw NetworkException(message);
        }
    }
}

Message * Connection::read()
{
    Message * messageReceived = NULL;

    byte buffer[NETWORK_MESSAGE_MAX_LENGTH];
    int32 dataRead = 0;
    int32 messageSize = 0;
    bool readSuccessfull = false;

    dataRead = ::recv(_socket, reinterpret_cast<char *> (buffer), sizeof(int32), 0);
    ::memcpy(&messageSize, buffer, sizeof(int32));

    if ((dataRead == sizeof(int32))
    && (messageSize <= NETWORK_MESSAGE_MAX_LENGTH)
    && (messageSize > 0))
    {
        dataRead = ::recv(_socket, reinterpret_cast<char *> (buffer) + sizeof(int32), messageSize - sizeof(int32), 0);

        if (dataRead == messageSize - sizeof(int32))
            readSuccessfull = true;
    }

    if (!readSuccessfull)
    {
         close();
        //An error as occured while trying to read a message.
        messageReceived = new Message(  NETWORK_CLOSE_SESSION_MESSAGE,
                                        _sessionId);
    }
    else
    {
        messageReceived = new Message(buffer);
        messageReceived->setSessionId(_sessionId);

        Core::Clock clock;
        _latestActivity = clock.universalTime();
    }
    
    return messageReceived;
}

} // namespace Network
