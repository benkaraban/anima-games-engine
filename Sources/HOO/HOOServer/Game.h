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
#ifndef HOOSERVER_GAME_H_
#define HOOSERVER_GAME_H_

#include <Core/Standard.h>
#include <Core/RT/Mutex.h>

#include <Network/NetworkEngine.h>

#include <GameEngine/GameContext.h>
#include <GameEngine/GameLibrary.h>

#include <HOOUserProtocol/GameMessage/GameCommand.h>
#include <HOOUserProtocol/GameMessage/ChatMessage.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>
#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>

#include <HOOServer/UserAccount.h>

namespace HOOServer
{

class Session;
class UserSessionManager;
class GamePool;

LM_ENUM_6(EGameState, 
          GAME_WAITING_CHALLENGERS,
          GAME_WAITING_ACK,
          GAME_LOADING,
          GAME_STARTED,
          GAME_FINISHED,
          GAME_RELEASED);

class LM_API_HOS Game : public Core::Object, GameEngine::IGameContextObs
{
friend class GamePool;

public:
    Game();

    // Interface

    void setChallengers(Session * player1, Session * player2, int32 levelId);
    void noChallengers();
    
    void onReceiveLaunch(const HOOUserProtocol::LaunchMatch & launch, int32 sessionId);
    void onReceiveCancel(const HOOUserProtocol::CancelQuickMatch & cancel, int32 sessionId);
    void onReceiveLoading(const HOOUserProtocol::UserLoading & loading, int32 sessionId);
    void onReceiveCommand(const HOOUserProtocol::GameCommand & command, int32 sessionId);
    void onReceiveChat(const HOOUserProtocol::ChatMessage & chat, int32 sessionId);

    void onPlayerDeconnection(int32 idSession);

    virtual void onContextUpdate(GameEngine::GameContext * pContext);

private:
    void internalStartSynchro();
    void internalOnReceiveSynchro(int32 sessionId);
    void internalOnSynchroFinished();

    void internalOnLaunch(int32 sessionId, bool ok);

    void internalOnAction(int32 sessionId, const HOOUserProtocol::ActionCommand & action);
    void internalOnLeaveGame(int32 sessionId);

    void internalReleasePlayers();
    void internalReleaseGame();

    void dispatchCommand(const HOOUserProtocol::GameCommandAnswer & master, bool requiresSynchro);

    Ptr<GameEngine::PlayerInstance> internalCvt(const HOOServer::UserAccount & account) const;

    void postLockChecks();

    HOOUserProtocol::EGameCommandAnswer  _lastAnswer;

    mutable Core::Mutex         _gameMutex;
    GameEngine::GameContext     _context;
    HOOUserProtocol::GameStatus _contextInterStatus1;
    HOOUserProtocol::GameStatus _contextInterStatus2;

    struct GamePlayer
    {
        GamePlayer() { init(0, NULL); }

        void init(int32 i, Session * pS)
        {
            pSession        = pS;
            index           = i;
            sendFailed      = false;
            launchFinished  = false;
            launchOK        = false;
            loadingFinished = false;
            syncFinished    = true;
        }

        Session *               pSession;
        int32                   index; // index dans la partie (1 ou 2)
        bool                    sendFailed;
        
        bool                    launchFinished;
        bool                    launchOK;

        bool                    loadingFinished;
        bool                    syncFinished;

        HOOUserProtocol::OpponentInfos asOpponent;
    };

    struct PlayerContext
    {
        PlayerContext(GamePlayer * p1, GamePlayer * p2) : pPlayer(p1), pOpponent(p2)
        {}

        GamePlayer *    pPlayer;
        GamePlayer *    pOpponent;
    };

    PlayerContext getContext(int32 sessionId);
    void send(const HOOUserProtocol::IUserAnswer & answer, GamePlayer & player);

    GamePlayer                      _player1;
    GamePlayer                      _player2;
    int32                           _levelId;

    EGameState                      _gameState;
    bool                            _syncInProgress;
    bool                            _leaveGameInProgress;

    // Manipul� directement par GamePool
    void newGame();

    int32                           _gameId;
    UserSessionManager *            _pSessions;
    const GameEngine::GameLibrary * _pGameLib;
    GamePool *                      _pPool;
    Game *                          _next;
    Game *                          _prev;
};

}//namespace HOOServer

#endif/*HOOSERVER_GAME_H_*/