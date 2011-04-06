/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef HOOCLIENT_NETWORK_CONNEXION_H_
#define HOOCLIENT_NETWORK_CONNEXION_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/RT/Thread.h>
#include <Core/List.h>

#include <Network/NetworkEngine.h>
#include <Network/Message.h>

#include <HOOUserProtocol/IUserMessage.h>

LM_ENUM_7(EConnexionState,
    CONNEXION_NOT_OPENED,
    CONNEXION_IN_PROGRESS,
    CONNEXION_FAILED,
    CONNEXION_FAILED_SERVER_FULL,
    CONNEXION_OK,
    CONNEXION_ERROR,
    CONNEXION_LOST);

class Connexion : public Core::Object
{
public:
    Connexion();

    // - Vérifie l'état de la connexion
    // - Récupère la liste des messages reçus
    // - Envoie tous les messages en attente
    void processMessages();

    void open(const String & address, int32 port);
    void close();

    void release();

    bool hasPendingMessage(const HOOUserProtocol::EUserAnswerType * filter);
    Ptr<HOOUserProtocol::IUserAnswer> popMessage(const HOOUserProtocol::EUserAnswerType * filter);
    
    EConnexionState getState() const;
    void resetState();
    
    bool isAppConnected() const { return _isAppConnected; }
    void setAppConnected() { _isAppConnected = true; }

    // Enqueue le message
    void send(const Core::Serializable & msg);

protected:
    void threadOpen();
    void connexionInProgress();
    void connexionOK();
    void setDisconnected(EConnexionState state);

    Ptr<HOOUserProtocol::IUserAnswer> translateMessage(Network::Message * pMessage);

    Core::ConditionMutex            _conditionMutex;
    Core::ConditionVar              _conditionVar;
    Network::NetworkEngine          _networkEngine;
    int32                           _sessionId;
    EConnexionState                 _state;
    volatile EConnexionState        _newState;

    Ptr<Core::Thread>               _pConnexionThread;
    Core::Mutex                     _connexionMutex;
    String                          _connexionAddress;
    volatile int32                  _connexionPort;
    volatile bool                   _connexionAttemptFinished;

    struct MessageEntry
    {
        MessageEntry(const Ptr<HOOUserProtocol::IUserAnswer> & pMessage) : pMessage(pMessage), time(Core::Clock::localTime())
        {}

        Ptr<HOOUserProtocol::IUserAnswer>   pMessage;
        Core::TimeValue                     time;
    };

    Core::DoubleQueue<MessageEntry>                       _incomingMessages;
    Core::DoubleQueue<Ptr<Network::Message> >             _outgoingMessages;

    bool                            _isAppConnected;
};

#endif
