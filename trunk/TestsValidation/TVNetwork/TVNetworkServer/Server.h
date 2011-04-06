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
#ifndef TVNETWORK_SERVER_H_
#define TVNETWORK_SERVER_H_

#include <Core/RT/ConditionVar.h>
#include <Core/RT/Thread.h>

#include <Network/NetworkEngine.h>
#include <Network/Defines.h>

#include "Session.h"

const int32 NB_THREADS = 10;

class Server: public Core::Object
{
public:
    Server();
    virtual ~Server();

    void launch();

    void manageAnswer(bool broadcast, int32 sessionId, BenchNetworkMessage & answer);

private:
    void run();

private:
    Core::Thread *          _threads[NB_THREADS];
    Core::ConditionMutex    _conditionMutex;
    Core::ConditionVar      _condition;

    Network::NetworkEngine  _engine;

    Session                 _sessions[Network::SOCKET_MAX_CONNECTION];

    Core::Mutex             _mutex;
    int32                   _nbMessageSent;
    int32                   _totalMessageSizeSent;

    volatile bool _isBeingKilled;
};

#endif/*TVNETWORK_SERVER_H_*/