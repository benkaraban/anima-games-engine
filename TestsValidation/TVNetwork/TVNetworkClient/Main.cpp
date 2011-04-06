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
#include <Core/RT/ConditionVar.h>
#include <Core/RT/Thread.h>
#include <Network/NetworkEngine.h>
#include <Network/Message.h>

#include "BenchNetworkMessage.h"

#include <iostream>

void work(Network::NetworkEngine & engine, int32 sessionId, Core::ConditionMutex & mutex, Core::ConditionVar & condition);

int main()
{
    try
    {
        std::cout<<"Launching Client"<<std::endl;
        Core::ConditionMutex    mutex;
        Core::ConditionVar      condition;
        Network::NetworkEngine  engine(&condition, &mutex);

        std::cout<<"Connect 192.168.0.15:31337"<<std::endl;
        int32 sessionId = -1;
        sessionId = engine.connect(L"192.168.0.15", 31337);
        if( sessionId != -1)
        {
            std::cout<<"-->>"<<std::endl;
            engine.launch();
            std::cout<<"<-->"<<std::endl;
            work(engine, sessionId, mutex, condition);
            std::cout<<"<<--"<<std::endl;
        }
        else
        {
            std::cout<<"Connection failed."<<std::endl;
        }

    }catch(Core::Exception e)
    {
        std::wcout<<e.getMessage()<<std::endl;

    }

}

void work(Network::NetworkEngine & engine, int32 sessionId, Core::ConditionMutex & mutex, Core::ConditionVar & condition)
{
    bool loop = true;

    int32 nbMessageSent = 0;
    int32 nbMessageReceived = 0;
    int32 totalReceivedSize = 0;

    std::cout<<"1"<<std::endl;

    {
        BenchNetworkMessage benchMessage;
        benchMessage._networkMessage = L"Open session";
        benchMessage._networkMessageType = MESSAGE_BROADCAST;
        
        Network::Message message(Network::NETWORK_OPEN_SESSION_MESSAGE, benchMessage, sessionId);

        engine.sendMessage(message);
    }

    std::cout<<"2"<<std::endl;

    while(loop)
    {
        std::cout<<"3"<<std::endl;
        BenchNetworkMessage benchMessage;
        benchMessage._networkMessage = L"This is only a test!!! I repeat this is only a test!!!";
        
        benchMessage._networkMessageType = MESSAGE_UNICAST;
        
        Network::Message message(Network::APPLICATION_MESSAGE, benchMessage, sessionId);
        std::cout<<"3.0"<<std::endl;

        engine.sendMessage(message);
        std::cout<<"3.1"<<std::endl;
        nbMessageSent++;

        Network::Message * messageReceived = NULL;
        std::cout<<"3.2"<<std::endl;
        {
            Core::ScopedCondLock lock(mutex);
            while(!engine.getMessage(messageReceived))
                condition.wait(lock);
        }

        std::cout<<"4"<<std::endl;

        if(messageReceived->getMessageType() == Network::NETWORK_CLOSE_SESSION_MESSAGE)
        {
            loop = false;
            break;//Le serveur a coup� la connexion.
            std::cout<<"Break"<<std::endl;
        }
        else
        {
            nbMessageReceived++;
            BenchNetworkMessage benchMessage;
            messageReceived->getMessage(benchMessage);
            totalReceivedSize += benchMessage._networkMessage.length();
        }
        std::cout<<"5"<<std::endl;

        delete messageReceived;
        messageReceived = NULL;

        if( (nbMessageReceived % 100) == 0)
            std::cout<<"Received : "<<nbMessageReceived<<" Sent : "<<nbMessageSent<<std::endl;

        if(nbMessageReceived >= 5000)
        {
            benchMessage._networkMessageType = MESSAGE_STOP_BENCH;
            Network::Message message(Network::APPLICATION_MESSAGE, benchMessage, sessionId);
            try
            {
                engine.sendMessage(message);
                Core::Thread::sleep(100);//Pour laisser au thread le temps d'envoyer le message pour stopper le serveur
                //Sleep(100);
            }catch(Core::Exception & e)
            {
                std::cout<<String8(e.getMessage()).c_str()<<std::endl;
            }
            loop = false;
        }
    }

    engine.stop();
    
}
