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
#ifndef NETWORK_WATCHER_H_ 
#define NETWORK_WATCHER_H_ 

#include <Network/Defines.h>

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>

#include <Network/ConnectionPool.h>

#ifndef _WIN32
#include <sys/epoll.h>
#endif

namespace Network
{

class Watcher: public Core::Object
{

public:
    Watcher(ConnectionPool & clientSockets,
            Connection * serverSocket);

    virtual ~Watcher();

    int32   init();
    int32   watch();
    bool    hasSeen(Connection * pConnection);
    void    add(Connection * pConnection, bool serverSocket = false);
    void    remove(Connection * pConnection);

    inline void setTimeOut(int32 timeOut) { _timeOut = timeOut; }

private:
    ConnectionPool &    _clientSockets;
    Connection *        _serverSocket;
    int32               _timeOut;

private:
//#ifdef _WIN32
    timeval _timeval;
    SOCKET  _highSocket;
    fd_set  _mask;
/*#else
    int32           _connectionCount;
    int32           _epfd;
    epoll_event *   _events;
    int32           _nbEvents;
#endif*/

};

} // namespace Network

#endif /*NETWORK_WATCHER_H_ */
