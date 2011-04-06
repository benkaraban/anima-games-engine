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
#include "Game.h"

#include <Core/Logger.h>
#include <Core/ScopedFlag.h>

#include <Network/NetworkException.h>

#include <HOOServer/GamePool.h>
#include <HOOServer/Session.h>
#include <HOOServer/UserSessionManager.h>
#include <HOOServer/UserMessageHandler/GameMessage/GameDataTranslate.h>

namespace HOOServer
{

Game::Game()
:   _levelId(0),
    _syncInProgress(false),
    _leaveGameInProgress(false),
    _gameId(-1),
    _next(NULL),
    _prev(NULL)
{}

void Game::newGame()
{
    Core::ScopedLock lock(_gameMutex);

    _player1.init(0, NULL);
    _player2.init(0, NULL);
    _levelId = 0;

    _gameState = GAME_WAITING_CHALLENGERS;
    _leaveGameInProgress = false;
}

void Game::setChallengers(Session * player1, Session * player2, int32 levelId)
{
    Core::ScopedLock lock(_gameMutex);
    
    if(_gameState != GAME_WAITING_CHALLENGERS)
        return;

    _player1.init(1, player1);
    _player2.init(2, player2);

    Ptr<GameEngine::PlayerInstance> pInst1;
    Ptr<GameEngine::PlayerInstance> pInst2;
    {
        Core::ScopedLock lock(_player1.pSession->getReadWriteMutex());
        _player1.pSession->opponentFound(this);

        const UserAccount & account = _player1.pSession->account();
        _player1.asOpponent.name = account.login;
        _player1.asOpponent.character = account.character;
        _player1.asOpponent.xp = account.xp;
        _player1.asOpponent.equipedStuff = account.equipedStuff;

        pInst1 = net2Game(account, _pGameLib);
    }
    {
        Core::ScopedLock lock(_player2.pSession->getReadWriteMutex());
        _player2.pSession->opponentFound(this);

        const UserAccount & account = _player2.pSession->account();
        _player2.asOpponent.name = account.login;
        _player2.asOpponent.character = account.character;
        _player2.asOpponent.xp = account.xp;
        _player2.asOpponent.equipedStuff = account.equipedStuff;

        pInst2 = net2Game(_player2.pSession->account(), _pGameLib);
    }

    _context.init(pInst1, pInst2);

    _levelId = levelId;
    _gameState = GAME_WAITING_ACK;
    _syncInProgress = false;
}

void Game::noChallengers()
{
    Core::ScopedLock lock(_gameMutex);
    internalReleaseGame();
}

void Game::send(const HOOUserProtocol::IUserAnswer & answer, GamePlayer & player)
{
    if(!player.sendFailed)
    {
        Network::Message message(Network::APPLICATION_MESSAGE, answer, player.pSession->getIdSession());
        try
        {
            _pSessions->sendAnswer(message);
        }
        catch(Network::NetworkException &)
        {
            // Fondamentalement il n'y a pas de problême, l'assert est juste là parce que jusqu'à maintenant
            // on n'a pas pu produire ce cas de figure alors qu'il existe. Le jour où l'assert pète c'est la fête.
            //LM_ASSERT(false);

            player.sendFailed = true;
            if(!_leaveGameInProgress)
                internalOnLeaveGame(player.pSession->getIdSession());
        }
    }
}

void Game::internalStartSynchro()
{
    if(_syncInProgress)
        throw Core::Exception(L"Trying to synchronize a game while a synchro is in progress\n");

    _player1.syncFinished = false;
    _player2.syncFinished = false;
    _syncInProgress = true;
}

void Game::onReceiveChat(const HOOUserProtocol::ChatMessage & chat, int32 sessionId)
{
    Core::ScopedLock lock(_gameMutex);

    PlayerContext context = getContext(sessionId);
    HOOUserProtocol::ChatMessageAnswer chatAnswer(chat.getMessage());
    send(chatAnswer, *context.pOpponent);
}

void Game::onReceiveCommand(const HOOUserProtocol::GameCommand & command, int32 sessionId)
{
    Core::ScopedLock lock(_gameMutex);

    switch(command.getCommand())
    {
    case HOOUserProtocol::SYNCHRONISATION:
        internalOnReceiveSynchro(sessionId);
        break;
    case HOOUserProtocol::LEAVE_GAME:
        internalOnLeaveGame(sessionId);
        break;
    case HOOUserProtocol::PLAYER_ACTION:
        internalOnAction(sessionId, command.getAction());
        break;
    }
}

void Game::onReceiveLaunch(const HOOUserProtocol::LaunchMatch & launch, int32 sessionId)
{
    Core::ScopedLock lock(_gameMutex);
    internalOnLaunch(sessionId, true);
}

void Game::onReceiveCancel(const HOOUserProtocol::CancelQuickMatch & cancel, int32 sessionId)
{
    Core::ScopedLock lock(_gameMutex);
    internalOnLaunch(sessionId, false);
}

void Game::onReceiveLoading(const HOOUserProtocol::UserLoading & loading, int32 sessionId)
{
    Core::ScopedLock lock(_gameMutex);

    if(_gameState != GAME_LOADING)
    {
        WAR << L"Received a loading message while not in a loading state. Message ignored.\n";
        return;
    }

    PlayerContext context = getContext(sessionId);

    HOOUserProtocol::EUserLoadingType loadProgress = loading.getUserLoadingType();

    if(loadProgress == HOOUserProtocol::USER_LOADING_FINISHED)
        context.pPlayer->loadingFinished = true;

    send(HOOUserProtocol::UserLoadingAnswer(HOOUserProtocol::transmitToOpponent(loadProgress)), *context.pOpponent);
 
    if(_player1.loadingFinished && _player2.loadingFinished)
    {
        _gameState = GAME_STARTED;

        // Message Game Start

        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::GAME_START);
        dispatchCommand(command, true);
    }
}

Game::PlayerContext Game::getContext(int32 sessionId)
{
    if(sessionId == _player1.pSession->getIdSession())
        return PlayerContext(&_player1, &_player2);
    else
        return PlayerContext(&_player2, &_player1);
}


void Game::internalOnLeaveGame(int32 sessionId)
{
    Core::ScopedFlag flag(_leaveGameInProgress);

    PlayerContext context = getContext(sessionId);

    switch(_gameState)
    {
    case GAME_WAITING_ACK:
        {
            // Le drop avant le ACK est traité command un message CancelQuickMatch
            // On ne passe forcément de suite en GAME_RELEASED, on attend le message de l'adversaire pour le prévenir.

            internalOnLaunch(sessionId, false); 
        }
        break;

    case GAME_LOADING:
        {
            // Le drop survenant avant la fin du chargement annule la partie sans pénalités, l'adversaire
            // est averti immédiatement avec un message UserLoadingAnswer

            send(HOOUserProtocol::UserLoadingAnswer(HOOUserProtocol::OPPONENT_DROPPED), *context.pOpponent);
            internalReleasePlayers();
            internalReleaseGame();            
        }
        break;

    case GAME_STARTED: // Le rage quit / drop de batard est considéré comme une reddition
        {
            send(HOOUserProtocol::GameCommandAnswer(HOOUserProtocol::GAME_FINISHED, context.pOpponent->index), *context.pOpponent);
        }
        // no break, on fait ensuite comme si la partie était terminée

    case GAME_FINISHED:
        {
            send(HOOUserProtocol::GameCommandAnswer(HOOUserProtocol::PLAYER_LEFT, context.pPlayer->index), *context.pOpponent);
            internalReleasePlayers();
            internalReleaseGame();            
        }
        break;

    case GAME_WAITING_CHALLENGERS:
    case GAME_RELEASED:
        break;
    }
}

void Game::onPlayerDeconnection(int32 idSession)
{
    // La déconnexion est traitée comme une sortie explicite de partie

    Core::ScopedLock lock(_gameMutex);
    internalOnLeaveGame(idSession);
}

void Game::internalOnReceiveSynchro(int32 sessionId)
{
    PlayerContext context = getContext(sessionId);

    if(_syncInProgress)
    {
        context.pPlayer->syncFinished = true;
        _syncInProgress = (!_player1.syncFinished || !_player2.syncFinished);

        if(!_syncInProgress)
            internalOnSynchroFinished();
    }
    else
    {
        LM_ASSERT(false);
    }
}

void Game::internalReleasePlayers()
{
    {
        Core::ScopedLock lock(_player1.pSession->getReadWriteMutex());
        _player1.pSession->setQuickMatchStatus(QM_NOT_WAITING_FOR_QUICKMATCH);
        _player1.pSession->resetGame();
    }
    {
        Core::ScopedLock lock(_player2.pSession->getReadWriteMutex());
        _player2.pSession->setQuickMatchStatus(QM_NOT_WAITING_FOR_QUICKMATCH);
        _player2.pSession->resetGame();
    }
}

void Game::internalReleaseGame()
{
    _gameState = GAME_RELEASED;
    _pPool->releaseGame(this);
}

void Game::internalOnLaunch(int32 sessionId, bool ok)
{
    if(_gameState == GAME_WAITING_ACK)
    {
        PlayerContext context = getContext(sessionId);

        context.pPlayer->launchFinished = true;
        context.pPlayer->launchOK = ok;

        if(_player1.launchFinished && _player2.launchFinished)
        {
            if(_player1.launchOK && _player2.launchOK) 
            {
                // Les deux joueurs ont ACK la partie, on peut commencer le chargement

                HOOUserProtocol::LaunchMatchAnswer answer1(HOOUserProtocol::LAUNCH_MATCH, 1, _levelId, _player2.asOpponent);
                HOOUserProtocol::LaunchMatchAnswer answer2(HOOUserProtocol::LAUNCH_MATCH, 2, _levelId, _player1.asOpponent);

                send(answer1, _player1);
                send(answer2, _player2);

                _gameState = GAME_LOADING;
            }
            else
            {
                // Au moins l'un des joueurs a annulé ou s'est déconnecté.
                // Il faut prévenir le joueur qui a ACK (s'il y en a) que l'autre a annulé et aquitter
                // l'annulation (si le joueur est déconnecté le send(cancel) n'aura pas d'effet).

                HOOUserProtocol::LaunchMatchAnswer answerLaunch(HOOUserProtocol::OPPONENT_CANCELLED);
                HOOUserProtocol::CancelQuickMatchAnswer answerCancel(HOOUserProtocol::QUICK_MATCH_CANCELLED);

                if(_player1.launchOK)
                {
                    send(answerLaunch, _player1);
                    send(answerCancel, _player2);
                }
                else if(_player2.launchOK)
                {
                    send(answerCancel, _player1);
                    send(answerLaunch, _player2);
                }
                else // Les deux joueurs ont annulé/se sont déconnectés
                {
                    send(answerCancel, _player1);
                    send(answerCancel, _player2);
                }

                internalReleasePlayers();
                internalReleaseGame();            
            }
        }
    }
    else
    {
        LM_ASSERT(false);
    }
}

void Game::internalOnSynchroFinished()
{
    switch(_context.getState())
    {
    case GameEngine::WAITING_FOR_P1_BID:
    case GameEngine::WAITING_FOR_P2_BID:
        LM_ASSERT(false);
        // no break;
    case GameEngine::WAITING_FOR_BIDS:
    {
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::WILL_CONTEST);
        command.setManaCount(_context.getManaOnTable());
        command.setManaSphere(int32(_context.getSphereOnTable()));
        dispatchCommand(command, false);
        break;
    }

    case GameEngine::P1_ATTACK:
    {
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_TURN);
        command.setCaster(1);
        dispatchCommand(command, false);
        break;
    }
    case GameEngine::P2_ATTACK:
    {
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_TURN);
        command.setCaster(2);
        dispatchCommand(command, false);
        break;
    }

    case GameEngine::P1_WIN:
    {
        _gameState = GAME_FINISHED;
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::GAME_FINISHED);
        command.setCaster(1);
        dispatchCommand(command, false);
        break;
    }
    case GameEngine::P2_WIN:
    {
        _gameState = GAME_FINISHED;
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::GAME_FINISHED);
        command.setCaster(2);
        dispatchCommand(command, false);
        break;
    }
    case GameEngine::DRAW:
    {
        _gameState = GAME_FINISHED;
        HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::GAME_FINISHED);
        command.setCaster(0);
        dispatchCommand(command, false);
        break;
    }

    case GameEngine::P2_DEFEND:
    case GameEngine::P1_DEFEND:
        LM_ASSERT(false);
        break;
    }
}

void Game::internalOnAction(int32 sessionId, const HOOUserProtocol::ActionCommand & action)
{
    if(_gameState != GAME_STARTED)
    {
        WAR << L"Player with session ID " << Core::toString(sessionId) << L" trying to make an action when state is " 
            << HOOServer::toString(_gameState) << L", action ignored\n";
        return;
    }
    else if(_syncInProgress)
    {
        WAR << L"Player with session ID " << Core::toString(sessionId) 
            << L" trying to make an action when a synchronisation is in progress, action ignored\n";
        return;
    }

    Ptr<GameEngine::PlayerInstance> pPlayerInstance;

    if(sessionId == _player1.pSession->getIdSession())
        pPlayerInstance = _context.getP1();
    else if(sessionId == _player2.pSession->getIdSession())
        pPlayerInstance = _context.getP2();
    else
    {
        ERR << L"Game received an action from session ID " << Core::toString(sessionId) << L" but this is not a player ID. Action ignored.\n";
        return;
    }

    GameEngine::EPlayerActionType actionType(GameEngine::PASS);
    Ptr<GameEngine::SpellInstance> pSpellInstance = null;
    int32 quantity = 0;

    switch(action.actionType)
    {
    case 0:
        actionType = GameEngine::CAST;
        pSpellInstance = pPlayerInstance->getSpellInDeck(action.actionValue);

        if(pSpellInstance == null)
        {
            WAR << L"Game received an CAST action from unknown spell " << Core::toString(action.actionValue) << L". PASS action used instead.\n";
            actionType = GameEngine::PASS;
        }
        break;
    case 1:
        actionType = GameEngine::BID;
        quantity = action.actionValue;
        break;
    case 2:
        actionType = GameEngine::PASS;
        break;
    }

    GameEngine::PlayerAction playerAction(pPlayerInstance, actionType, pSpellInstance, quantity);

    if(!_context.verifyValidity(playerAction))
    {
        WAR << L"Game received an invalid action. Action ignored.\n";
        return;
    }

    switch(_lastAnswer)
    {
    case HOOUserProtocol::WILL_CONTEST:

        if(_context.applyPlayerAction(playerAction, this))
        {
            if(!_context.isBidInProgress())
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::WILL_RESOLVE);
                command.setBidWinner(_context.getLastBidWinnerIndex());
                dispatchCommand(command, true);
            }
        }
        break;

    case HOOUserProtocol::PLAYER_TURN:
        if(_context.applyPlayerAction(playerAction, this))
        {
            if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_TRICK)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_CAST_TRICK);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, false);
            }
            else if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_RETRIBUTION)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_CAST_RETRIBUTION);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, false);
            }
            else if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_SPELL)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_CAST_SPELL);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, false);
            }
            else
            {
                LM_ASSERT(playerAction._type == GameEngine::PASS);

                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_START_PASS);
                command.setCaster(pPlayerInstance->getPlayerIndex());

                dispatchCommand(command, false);
            }
        }
        break;

    case HOOUserProtocol::PLAYER_CAST_SPELL:
        if(_context.applyPlayerAction(playerAction, this))
        {
            if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_COUNTER)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_SPELL_COUNTERED);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, true);
            }
            else if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_TRICK)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_SPELL_HIT_AFTER_TRICK);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, true);
            }
            else
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_SPELL_HIT);
                dispatchCommand(command, true);
            }
        }
        break;

    case HOOUserProtocol::PLAYER_START_PASS:
        if(_context.applyPlayerAction(playerAction, this))
        {
            if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_TRICK)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_PASS_FINISH_AFTER_TRICK);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, true);
            }
            else
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_PASS_FINISH);
                dispatchCommand(command, true);
            }
        }
        break;

    case HOOUserProtocol::PLAYER_CAST_RETRIBUTION:
        if(_context.applyPlayerAction(playerAction, this))
        {
            if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_TRICK)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_RETRIBUTION_HIT_AFTER_TRICK);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, true);
            }
            else
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_RETRIBUTION_HIT);
                dispatchCommand(command, true);
            }
        }
        break;

    case HOOUserProtocol::PLAYER_CAST_TRICK:
        if(_context.applyPlayerAction(playerAction, this))
        {
            if(pSpellInstance != null && pSpellInstance->getType() == GameEngine::SPELLTYPE_TRICK)
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_TRICK_HIT_AFTER_TRICK);
                command.setCaster(pPlayerInstance->getPlayerIndex());
                command.setSpell(pSpellInstance->getId());

                dispatchCommand(command, true);
            }
            else
            {
                HOOUserProtocol::GameCommandAnswer command(HOOUserProtocol::PLAYER_TRICK_HIT);
                dispatchCommand(command, true);
            }
        }
        break;

    default:
        WAR << L"Game received an action while last message sent was " << HOOUserProtocol::toString(_lastAnswer)
            << L", action ignored.\n";
        break;
    }
}

void Game::dispatchCommand(const HOOUserProtocol::GameCommandAnswer & master, bool requiresSynchro)
{
    if(requiresSynchro)
        internalStartSynchro();

    HOOUserProtocol::GameCommandAnswer command1 = master;
    HOOUserProtocol::GameCommandAnswer command2 = master;

    // Status update

    HOOUserProtocol::GameStatus status1;
    HOOUserProtocol::GameStatus status2;

    game2Net(1, _context, status1);
    game2Net(2, _context, status2);

    command1.setStatusUpdate(HOOUserProtocol::GameStatusUpdate(status1));
    command2.setStatusUpdate(HOOUserProtocol::GameStatusUpdate(status2));

    command1.setStatusUpdateAfterOpponentTrick(HOOUserProtocol::GameStatusUpdate(_contextInterStatus1));
    command2.setStatusUpdateAfterOpponentTrick(HOOUserProtocol::GameStatusUpdate(_contextInterStatus2));

    // Available actions

    Core::List<HOOUserProtocol::ActionCommand> actions1;
    Core::List<HOOUserProtocol::ActionCommand> actions2;

    Ptr<Core::List<Ptr<GameEngine::PlayerAction> > > pGameActions1 = _context.getAvailableActions(_context.getP1());
    Ptr<Core::List<Ptr<GameEngine::PlayerAction> > > pGameActions2 = _context.getAvailableActions(_context.getP2());

    game2Net(*pGameActions1, actions1);
    game2Net(*pGameActions2, actions2);

    command1.setAvailableActions(actions1);
    command2.setAvailableActions(actions2);

    send(command1, _player1);
    send(command2, _player2);

    _lastAnswer = master.getCommand();
}

void Game::onContextUpdate(GameEngine::GameContext * pContext)
{
    game2Net(1, *pContext, _contextInterStatus1);
    game2Net(2, *pContext, _contextInterStatus2);
}

}//namespace HOOServer
