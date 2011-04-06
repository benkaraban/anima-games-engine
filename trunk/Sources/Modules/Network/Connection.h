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
#ifndef NETWORK_CONNECTION_H_
#define NETWORK_CONNECTION_H_

#include <Core/Standard.h>
#include <Core/DateAndTime.h>
#include <Core/RT/SyncQueue.h>

#include <Network/Enum.h>
#include <Network/Defines.h>

namespace Network
{

class Message;

class LM_API_NET Connection: public Core::Object
{
friend class NetworkEngine;
friend class ConnectionPool;

public:
    Connection()
    :   _socket(INVALID_SOCKET),
        _sessionId(-1),
        _timeOut(0),
        _next(NULL),
        _prev(NULL)
    {
    }

    virtual ~Connection();

public:
    inline SOCKET   getSocket() const { return _socket; }
    void setSocket(SOCKET socket);

    void setTimeOut(int32 totalSeconds);
    bool hasTimedOut();

    inline int32    getSessionId() const { return _sessionId; }
    inline void     setSessionId(int32 sessionId) { _sessionId = sessionId; }

    inline Connection * getNext() const { return _next; }

    inline bool     isClosed() const { return (_socket == INVALID_SOCKET); }

    void sendMessage(Message & messageToSend);

private:
    void close();
    /**
     * Cette méthode est appelée par le NetworkEngine quand le NetworkWatcher
     * lui a signalé qu'il y a quelque chose à lire sur cette connection.
     */
    Message * read();

private:
    SOCKET          _socket;
    int32           _sessionId;
    uint32          _timeOut;
    Core::TimeValue _latestActivity;

    //Manipulé directement par le ConnectionPool
    Connection * _next;
    Connection * _prev;
};

} // namespace Network

#endif /* NETWORK_CONNECTION_H_ */
