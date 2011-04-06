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
#include "Server.h"
#include "BenchNetworkMessage.h"

#include <Network/Message.h>

#include <iostream>

Server::Server()
:   _engine(Network::NetworkEngine(&_condition, &_conditionMutex)),
    _nbMessageSent(0),
    _totalMessageSizeSent(0),
    _isBeingKilled(false)
{    
    for(int32 iThread = 0; iThread<NB_THREADS; ++iThread)
        _threads[iThread] = NULL;

    for(int32 iSession = 0; iSession<Network::SOCKET_MAX_CONNECTION; ++iSession)
        _sessions[iSession].setIdSession(iSession);
}

Server::~Server()
{
    for(int32 ii = 0; ii<NB_THREADS; ++ii)
    {
        if(_threads[ii] != NULL)
        {
            delete _threads[ii];
            _threads[ii] = NULL;
        }
    }
}

void Server::launch()
{
    std::cout<<"Listening on 192.168.0.19:31337"<<std::endl;
    _engine.open(L"192.168.0.19", 31337);

    _engine.launch();

    for(int32 ii = 0; ii<NB_THREADS; ++ii)
    {
        _threads[ii] = new Core::Thread(&Server::run, *this);
    }

    while(!_isBeingKilled)
    {
        Core::Thread::sleep(20);
    }

    {
        Core::ScopedCondLock lock(_conditionMutex);
        _condition.notifyAll();
    }

    for(int32 ii = 0; ii<NB_THREADS; ++ii)
    {
        _threads[ii]->join();
    }
}

void Server::run()
{
    while(!_isBeingKilled)
    {
        Network::Message * message = NULL;
        {
            Core::ScopedCondLock lock(_conditionMutex);
            while(!_isBeingKilled && !_engine.getMessage(message))
                _condition.wait(lock);
        }

        if(!_isBeingKilled)
        {
            int32 sessionId = message->getSessionId();

            Network::EMessageType type = message->getMessageType();
            if(type == Network::NETWORK_OPEN_SESSION_MESSAGE)
            {
                if(!_sessions[sessionId].isOpened())
                    _sessions[sessionId].open();
            }
            else if(type == Network::NETWORK_CLOSE_SESSION_MESSAGE)//La connexion r�seau a �t� ferm�e.
            {
                if(_sessions[sessionId].isOpened())
                    _sessions[sessionId].close();
            }
            else if(type == Network::APPLICATION_MESSAGE)
            {
                BenchNetworkMessage benchMessage;
                message->getMessage(benchMessage);

                bool broadcast = false;
                BenchNetworkMessage answer;
                try
                {
                    _sessions[sessionId].processMessage(benchMessage, answer, broadcast);
                    if(_sessions[sessionId].isOpened())
                        manageAnswer(broadcast, sessionId, answer);

                }catch(Core::Exception & e)
                {
                    std::cout<<"ProcessMessage : "<<String8(e.getMessage()).c_str()<<std::endl;
                    _isBeingKilled = true;
                }
            }
            else//Message non g�r�!
            {
                std::cout<<"Unknown message type : "<<type<<std::endl;
            }

            delete message;
            message = NULL;
        }
    }
}

void Server::manageAnswer(bool broadcast, int32 sessionId, BenchNetworkMessage & answer)
{
    if(broadcast)
    {
        for(int32 iSession = 0; iSession<Network::SOCKET_MAX_CONNECTION; ++iSession)
        {
            if(_sessions[iSession].isOpened())
            {
                Network::Message messageToSend(Network::APPLICATION_MESSAGE, answer, iSession);
                int32 messageSize = messageToSend.getSize();
                try
                {
                    _engine.sendMessage(messageToSend);

                    //On arrive ici que si le message a bien �t� envoy�
                    {
                        Core::ScopedLock lock(_mutex);
                        _nbMessageSent++;
                        _totalMessageSizeSent += messageSize;
                    }
                }catch(Core::Exception & e)
                {
                    std::cout<<"Broadcast : "<<String8(e.getMessage()).c_str()<< " Session : "<< iSession <<std::endl;
                    _sessions[iSession].close();
                }
            }
        }
    }
    else//On envoie � la premi�re session ouverte
    {
        int32 iSession = sessionId;
        bool notFound = true;
        do
        {
            iSession++;
            iSession = iSession % Network::SOCKET_MAX_CONNECTION;

            if(iSession != sessionId && _sessions[iSession].isOpened())
            {
                notFound = false;

                Network::Message messageToSend(Network::APPLICATION_MESSAGE, answer, iSession);
                int32 messageSize = messageToSend.getSize();
                try
                {
                    _engine.sendMessage(messageToSend);
                    
                    //On arrive ici que si le message a bien �t� envoy�
                    {
                        Core::ScopedLock lock(_mutex);
                        _nbMessageSent++;
                        _totalMessageSizeSent += messageSize;
                    }
                }catch(Core::Exception & e)
                {
                    std::cout<<"Unicast : "<<String8(e.getMessage()).c_str()<< " Session : "<< iSession <<std::endl;
                    _sessions[iSession].close();
                }
            }
        }while(notFound && iSession != sessionId);
    }
}
