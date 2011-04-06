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
#ifndef HOOCLIENTBOT_CLIENTBOT_H_
#define HOOCLIENTBOT_CLIENTBOT_H_

#include <Core/Standard.h>

#include <Network/NetworkEngine.h>

#include <HOOUserProtocol/IUserMessage.h>
#include <HOOUserProtocol/UserAccountInfos.h>
#include <HOOUserProtocol/GameMessage/GameData.h>

namespace HOOClientBot
{
struct BotConfig
{
    BotConfig();

    // Pour chaque XXX count, -1 == infini

    String          serverAddress;
    int32           serverPort;

    int32           openSocketRetryCount;
    int32           openSocketRetryWaitTime; // Attente entre les essais en SECONDES

    String          loginBase;
    bool            loginIsIndexed;
    int32           loginIndexStart;


    int32           connectionDuration; // Dur�e de la connexion en SECONDES
    int32           afterGamePauseDuration;

    int32           scriptMinDuration;
    int32           scriptMaxDuration;

    int32           willContestMinDuration;
    int32           willContestMaxDuration;

    int32           turnMinDuration;
    int32           turnMaxDuration;
};

LM_ENUM_5(EInGameState,
          STATE_PLAYING_INTRO,
          STATE_CHOICE_PLAYER,
          STATE_CHOICE_OPPONENT,
          STATE_PLAYING_SCRIPT,
          STATE_WAITING_SERVER
          );

class LM_API_HCB ClientBot : public Core::Object
{
public:
    ClientBot();
    virtual ~ClientBot();
    
    void launch();

protected:

    bool openSocket();
    bool connect();
    bool login();
    void logout();

    bool playGame();
    void gameLoop();

    void updateStatus(const HOOUserProtocol::GameStatusUpdate & status);
    void displayStatus();
    void action(const Core::List<HOOUserProtocol::ActionCommand> & action);
    void synchro();

    void removeMessages();
    bool receive(HOOUserProtocol::IUserAnswer & message, bool blocking = true);
    int32 receive(HOOUserProtocol::IUserAnswer & message1, HOOUserProtocol::IUserAnswer & message2, bool blocking);
    void send(HOOUserProtocol::IUserRequest & message);

    String toStringCount(int32 c);


    Core::ConditionMutex        _conditionMutex;
    Core::ConditionVar          _conditionVar;
    Network::NetworkEngine      _networkEngine;

    BotConfig                   _config;
    int32                       _index;

    int32                       _sessionId;
    bool                        _terminateProcess;
    int32                       _playerId;
    int32                       _levelId;
    bool                        _gameFinished;

    HOOUserProtocol::UserAccountInfos   _account;

    EInGameState                        _state;
    bool                                _syncAsked;
    int32                               _syncIndex;

    HOOUserProtocol::GameStatus         _status;
    bool                                _scriptInProgress;
    Core::TimeValue                     _scriptEndTime;
};

}//namespace HOOClient

#endif/*HOOCLIENT_CLIENT_H_*/