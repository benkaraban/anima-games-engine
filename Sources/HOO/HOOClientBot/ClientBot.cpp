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
#include <Core/Logger.h>
#include <Core/Math/Random.h>
#include <Core/RT/Thread.h>

#include <Network/Message.h>
#include <Network/NetworkException.h>

#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/UserLogin.h>
#include <HOOUserProtocol/UserLogout.h>
#include <HOOUserProtocol/SelectCharacter.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>
#include <HOOUserProtocol/GameMessage/ChatMessage.h>
#include <HOOUserProtocol/GameMessage/GameCommand.h>

#include <HOOClientBot/ClientBot.h>

#include <limits>

namespace HOOClientBot
{
using namespace HOOUserProtocol;

BotConfig::BotConfig()
{
    serverAddress               = Network::NetworkEngine::getHostIP();
    serverPort                  = 31337;

    openSocketRetryCount        = 3;
    openSocketRetryWaitTime     = 5;

    loginBase                   = L"bot";
    loginIsIndexed              = true;
    loginIndexStart             = 0;

    connectionDuration          = 1000 * 60;
    afterGamePauseDuration      = 1;

    scriptMinDuration           = 1;
    scriptMaxDuration           = 1;

    willContestMinDuration      = 1;
    willContestMaxDuration      = 1;

    turnMinDuration             = 1;
    turnMaxDuration             = 1;
}

ClientBot::ClientBot()
:   _networkEngine(&_conditionVar, &_conditionMutex),
    _sessionId(-1),
    _terminateProcess(false)
{
}

ClientBot::~ClientBot()
{
}
    
void ClientBot::launch()
{
    if(!openSocket())
        return;

    if(!connect())
        return;

    if(!login())
        return;

    Core::TimeValue deconnectionTime(Core::Clock::localTime() + Core::TimeDuration(0, 0, _config.connectionDuration, 0));

    while(Core::Clock::localTime() < deconnectionTime)
    {
        bool played = playGame();
        INF << L"OUTGAME\n";
        
        if(played)
            Core::Thread::sleep(_config.afterGamePauseDuration * 1000);
        else
            Core::Thread::sleep(_config.afterGamePauseDuration * 100);
    }

    logout();
}

bool ClientBot::openSocket()
{
    bool result = false;
    int32 tryCount = 0;

    do
    {
        if(tryCount != 0)
        {
            INF << L"Socket open failed, will retry in " << Core::toString(_config.openSocketRetryWaitTime) << L" s\n";
            Core::Thread::sleep(_config.openSocketRetryWaitTime * 1000);
        }

        tryCount ++;
        INF << L"Opening socket " << _config.serverAddress << L":" << Core::toString(_config.serverPort)
            << L" try " << Core::toString(tryCount) << L"/" << toStringCount(_config.openSocketRetryCount) << L"...\n";
        _sessionId = _networkEngine.connect(_config.serverAddress, _config.serverPort);

    } while(tryCount < _config.openSocketRetryCount && _sessionId == -1);


    if(_sessionId != -1)
    {
        INF << L"Socket opened with sessionId " << Core::toString(_sessionId) << L"\n";
        _networkEngine.launch();
        result = true;
    }
    else
    {
        INF << L"Socket open failed, exiting...\n";
    }

    return result;
}

bool ClientBot::connect()
{
    {
        INF << L"Sending connection request\n";
        UserConnect connectMsg;
        send(connectMsg);
    }
    {
        UserConnectAnswer connectAnswer;
        receive(connectAnswer);

        INF << L"Connect answer is " << HOOUserProtocol::toString(connectAnswer.getConnectAnswerType()) << L"\n";

        return connectAnswer.getConnectAnswerType() == USER_CONNECT_OK;
    }
}

bool ClientBot::login()
{
    bool isLoggedIn = false;
    int32 curIndex(_config.loginIndexStart);

    do
    {
        String  curLogin(_config.loginBase);

        if(_config.loginIsIndexed)
            curLogin << Core::toString(curIndex, 4);

        _index = curIndex;

        INF << L"My account is '" << curLogin << L"'\n";
        INF << L"Checking if account is available\n";

        CheckLoginAvailable available(curLogin);
        send(available);

        CheckLoginAvailableAnswer availableAnswer;
        receive(availableAnswer);

        INF << L"Account status is " << HOOUserProtocol::toString(availableAnswer.getLoginAvailability()) << L"\n";

        if(availableAnswer.getLoginAvailability() == USER_LOGIN_NOT_AVAILABLE)
        {
            INF << L"Account exists\n";
            INF << L"Logging in with account '" << curLogin << L"'\n";
            UserLogin loginMsg(curLogin, curLogin);
            send(loginMsg);

            UserLoginAnswer loginAnswer;
            receive(loginAnswer);

            INF << L"Logging in result is " << HOOUserProtocol::toString(loginAnswer.getLoginAnswerType()) << L"\n";

            if(loginAnswer.getLoginAnswerType() == USER_LOGIN_OK)
            {
                isLoggedIn = true;
                _account = loginAnswer.getAccountInfos();
            }
            else if(loginAnswer.getLoginAnswerType() == USER_ALREADY_LOGGED)
            {
                INF << L"This account is already used by someone, we will try with another one\n";
            }
            else
            {
                ERR << L"Unexpected answer, exiting\n";
                break;
            }
        }
        else if(availableAnswer.getLoginAvailability() == USER_LOGIN_AVAILABLE)
        {
            INF << L"Account does not exist yet\n";
            INF << L"Creating account '" <<  curLogin << L"'\n";

            CreateUserAccount create(curLogin, curLogin,  curLogin + L"@bot-society.com", false);
            send(create);

            CreateUserAccountAnswer createAnswer;
            receive(createAnswer);

            INF << L"Account creation result is " << HOOUserProtocol::toString(createAnswer.getCreateAccountAnswerType()) << L"\n";
            
            if(createAnswer.getCreateAccountAnswerType() == USER_ACCOUNT_CREATED)
            {
                isLoggedIn = true;
            }
            else
            {
                ERR << L"Unexpected answer, exiting\n";
                break;
            }
        }
        else
        {
            ERR << L"Unexpected answer, exiting\n";
            break;
        }

        curIndex ++;
    }
    while(!isLoggedIn && _config.loginIsIndexed && curIndex <= 9999);

    if(isLoggedIn && _account.character == 0)
    {
        int32 charId = 1;
        INF << L"Selection character " << charId << L"\n";
        SelectCharacter selectChar(charId);
        send(selectChar);

        SelectCharacterAnswer selectAnswer;
        receive(selectAnswer);

        INF << L"Character selection result is " << HOOUserProtocol::toString(selectAnswer.getSelectCharacterAnswerType()) << L"\n";

        if(selectAnswer.getSelectCharacterAnswerType() == CHARACTER_SELECTED_OK)
        {
            selectAnswer.getAccountInfos();
        }
    }

    return isLoggedIn;
}

void ClientBot::logout()
{
    INF << L"Logging out\n";
    UserLogout logoutMsg;
    send(logoutMsg);
}

bool ClientBot::playGame()
{
    INF << L"Launching a quick match\n";
    
    {
        QuickMatch quick;
        send(quick);

        bool opponentFound = false;

        //Core::Thread::sleep(5000);

        //CancelQuickMatch cancel;
        //send(cancel);

        //CancelQuickMatchAnswer cancelAnswer;
        //receive(cancelAnswer);

        //INF << L"Canceling quick match answer is " << HOOUserProtocol::toString(cancelAnswer.getCancelQuickMatchAnswerType()) << L"\n";
        //Core::Thread::sleep(500);
        //removeMessages();
        //return false;

        while(!opponentFound)
        {
            QuickMatchAnswer quickAnswer;
            receive(quickAnswer);

            INF << L"Quick match status is " << HOOUserProtocol::toString(quickAnswer.getQuickMatchAnswerType()) << L"\n";

            opponentFound = (quickAnswer.getQuickMatchAnswerType() == OPPONENT_FOUND);
        }
    }

    bool ok = false;
    {
        INF << L"Opponent found, launching match\n";
        
        if(_index == 100)
        {
            Core::Thread::sleep(100);

            INF << L"Canceling quick match\n";

            CancelQuickMatch cancel;
            send(cancel);
            ok = false;

            CancelQuickMatchAnswer cancelAnswer;
            receive(cancelAnswer);

            INF << L"Canceling quick match answer is " << HOOUserProtocol::toString(cancelAnswer.getCancelQuickMatchAnswerType()) << L"\n";
            return false;
        }
        else
        {
            LaunchMatch launch;
            send(launch);

            LaunchMatchAnswer launchAnswer;
            receive(launchAnswer);

            INF << L"Launching status is " << HOOUserProtocol::toString(launchAnswer.getLaunchMatchAnswerType()) << L"\n";

            ok = (launchAnswer.getLaunchMatchAnswerType() == LAUNCH_MATCH);

            if(ok)
            {
                _playerId = launchAnswer.getPlayerId();
                _levelId = launchAnswer.getLevelId();
                const HOOUserProtocol::OpponentInfos & opponent = launchAnswer.getOpponentInfos();
                INF << L"My opponent is " << opponent.name << L" with character " << Core::toString(opponent.character) << L"\n";
            }
        }
    }

    if(ok)
    {
        INF << L"Loading level " << Core::toString(_levelId) << L"...\n";

        int32 loadingStatus = int32(USER_LOADING_0_PERCENT);
        EUserLoadingAnswerType opponentStatus = EUserLoadingAnswerType(-1);
        UserLoadingAnswer launchAnswer;

        do
        {
            UserLoading loadingMsg((EUserLoadingType)loadingStatus);
            send(loadingMsg);

            while(receive(launchAnswer, false) && opponentStatus != OPPONENT_LOADING_FINISHED)
            {
                opponentStatus = launchAnswer.getUserLoadingAnswerType();
                if(opponentStatus == OPPONENT_LOADING_FINISHED)
                {
                    INF << L"Opponent loading finished\n";
                }
            }

            //Core::Thread::sleep(Core::irand(100, 1000));
        }
        while(loadingStatus++ < USER_LOADING_FINISHED && opponentStatus != OPPONENT_DROPPED);

        INF << L"Player loading finished\n";

        while(opponentStatus != OPPONENT_LOADING_FINISHED && opponentStatus != OPPONENT_DROPPED)
        {
            receive(launchAnswer);
            opponentStatus = launchAnswer.getUserLoadingAnswerType();

            if(opponentStatus == OPPONENT_LOADING_FINISHED)
            {
                INF << L"Opponent loading finished\n";
            }
        }

        ok = (opponentStatus == OPPONENT_LOADING_FINISHED);
    }

    if(ok)
    {
        INF << L"INGAME with player ID " << Core::toString(_playerId) << L"\n";

        _gameFinished = false;

        while(!_gameFinished)
            gameLoop();
    }
    else
    {
        INF << L"Opponent dropped\n";
    }

    return ok;
}

void ClientBot::action(const Core::List<HOOUserProtocol::ActionCommand> & actions)
{
    if(actions.size() > 0)
    {
        // skip la dernière action (= pass) s'il y a d'autres possibilités
        int32 iAction = (actions.size() >= 2) ? Core::irand(0, actions.size() - 2) : 0;
        const HOOUserProtocol::ActionCommand & action = actions[iAction];

        switch(action.actionType)
        {
        case 0:
            INF << L"=> Cast spell " << Core::toString(action.actionValue) << L"\n";
            break;
        case 1:
            INF << L"=> Bid for " << Core::toString(action.actionValue) << L"\n";
            break;
        case 2:
            INF << L"=> Pass\n";
            break;
        default:
            ERR << L"Invalid action value " << Core::toString(action.actionType) << L"\n";
            break;
        }

        HOOUserProtocol::GameCommand command(action);
        send(command);
    }
    else
    {
        LM_ASSERT(false);
        INF << L"=> Pass self\n";
        HOOUserProtocol::ActionCommand action;
        action.actionType = 2;
        action.actionValue = 0;
        HOOUserProtocol::GameCommand command(action);
        send(command);
    }
}

void ClientBot::updateStatus(const HOOUserProtocol::GameStatusUpdate & status)
{
    status.applyUpdate(_status);
}

void ClientBot::displayStatus()
{
    INF << L"H:" << Core::toString(_status.player.health, 3)
        << L" W: " << Core::toString(_status.player.willpower, 2) << L" M:" 
        << Core::toString(_status.player.spheres[0].mana, 2) << L" "
        << Core::toString(_status.player.spheres[1].mana, 2) << L" "
        << Core::toString(_status.player.spheres[2].mana, 2) << L"  "
        << (_status.player.hasVantage ? L"<" : L" ") 
        << L"VS" 
        << (_status.opponent.hasVantage ? L">" : L" ") 
        << L"  H:" << Core::toString(_status.opponent.health, 3) << L"\n";

    for(int32 ii=0; ii < _status.player.cooldowns.size(); ii++)
    {
        INF << L"  - Spell " << Core::toString(_status.player.cooldowns[ii].spellId) << L" has cooldown " << Core::toString(_status.player.cooldowns[ii].cooldown) << L"\n";
    }
}

void ClientBot::synchro()
{
    INF << L"=> Synchro\n";
    HOOUserProtocol::GameCommand command(HOOUserProtocol::SYNCHRONISATION);
    send(command);
}

void ClientBot::gameLoop()
{
    GameCommandAnswer command;

    if(receive(command, false))
    {
        updateStatus(command.getStatusUpdate());

        switch(command.getCommand())
        {
        case GAME_START:
            INF << L"Game start\n";
            displayStatus();
            INF << L"{ Playing beautiful introduction script\n";
            //Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"} Playing beautiful introduction script\n";

            synchro();
            break;

        case WILL_CONTEST:
            updateStatus(command.getStatusUpdate());
            INF << L"Will contest for " << Core::toString(command.getManaCount()) << L" of " << Core::toString(command.getManaSphere()) << L"\n";
            Core::Thread::sleep(Core::irand(_config.willContestMinDuration * 1000, _config.willContestMaxDuration * 1000));
            action(command.getAvailableActions());
            _state = STATE_WAITING_SERVER;
            break;

        case WILL_RESOLVE:
            if(command.getBidWinner() == _playerId)
                INF << L"PLAYER win the will contest\n";
            else if(command.getBidWinner() == 0)
                INF << L"DRAW will contest\n";
            else
                INF << L"OPPONENT win the will contest\n";

            updateStatus(command.getStatusUpdate());

            INF << L"-{ Playing will resolve script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing will resolve script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_TURN:
            if(command.getCasterId() == _playerId)
            {
                INF << L"PLAYER turn, " << Core::toString(command.getAvailableActions().size()) << L" possibilities\n";
                Core::Thread::sleep(Core::irand(_config.turnMinDuration * 1000, _config.turnMaxDuration * 1000));
                action(command.getAvailableActions());
            }
            else
            {
                INF << L"OPPONENT turn, waiting for him\n";
                if(command.getAvailableActions().size() > 0)
                {
                    ERR << L"Weird, I do have possibilities!!!!\n";
                    for(int32 ii=0; ii < command.getAvailableActions().size(); ii++)
                    {
                        ERR << Core::toString(command.getAvailableActions()[ii].actionType) << L"  " <<
                               Core::toString(command.getAvailableActions()[ii].actionType) << L"\n";
                    }
                }
            }
            break;

        case PLAYER_CAST_SPELL:
            if(command.getCasterId() == _playerId)
            {
                INF << L"PLAYER cast spell " << Core::toString(command.getSpellId()) << L"\n";
                displayStatus();
                INF << L"-{ Playing spell incant script\n";
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing spell incant script\n";
                LM_ASSERT(command.getAvailableActions().size() == 0);
            }
            else
            {
                INF << L"OPPONENT cast spell " << Core::toString(command.getSpellId()) << L", "
                    << Core::toString(command.getAvailableActions().size()) << L" possibilities\n";
                INF << L"-{ Playing spell incant script\n";
                //Core::Thread::sleep(1000);
                Core::Thread::sleep(Core::irand(_config.turnMinDuration * 500, _config.turnMaxDuration * 500));
                action(command.getAvailableActions());
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing spell incant script\n";
            }
            break;

        case PLAYER_SPELL_HIT:
            INF << L"-{ Playing spell hit script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing spell hit script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_SPELL_HIT_AFTER_TRICK:
            if(command.getCasterId() == _playerId)
                INF << L"PLAYER cast trick " <<  Core::toString(command.getSpellId()) << L"\n";
            else
                INF << L"OPPONENT cast trick " <<  Core::toString(command.getSpellId()) << L"\n";

            INF << L"-{ Playing trick script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick script\n";
            INF << L"-{ Playing spell hit script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing spell hit script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_SPELL_COUNTERED:
            if(command.getCasterId() == _playerId)
                INF << L"PLAYER cast counter " <<  Core::toString(command.getSpellId()) << L"\n";
            else
                INF << L"OPPONENT cast counter " <<  Core::toString(command.getSpellId()) << L"\n";

            INF << L"-{ Playing spell countered script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing spell countered script\n";
            displayStatus();
            synchro();
            break;
        
        case PLAYER_CAST_TRICK:
            if(command.getCasterId() == _playerId)
            {
                INF << L"PLAYER cast trick " << Core::toString(command.getSpellId()) << L"\n";
                INF << L"-{ Playing trick incant script\n";
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing trick incant script\n";
                LM_ASSERT(command.getAvailableActions().size() == 0);
            }
            else
            {
                INF << L"OPPONENT cast trick " << Core::toString(command.getSpellId()) << L", "
                    << Core::toString(command.getAvailableActions().size()) << L" possibilities\n";
                INF << L"-{ Playing trick incant script\n";
                //Core::Thread::sleep(1000);
                Core::Thread::sleep(Core::irand(_config.turnMinDuration * 500, _config.turnMaxDuration * 500));
                action(command.getAvailableActions());
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing trick incant script\n";
            }
            break;


        case PLAYER_TRICK_HIT:
            INF << L"-{ Playing trick finish script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick finish script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_TRICK_HIT_AFTER_TRICK:
            if(command.getCasterId() == _playerId)
                INF << L"PLAYER cast trick " <<  Core::toString(command.getSpellId()) << L"\n";
            else
                INF << L"OPPONENT cast trick " <<  Core::toString(command.getSpellId()) << L"\n";

            INF << L"-{ Playing trick script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick script\n";
            INF << L"-{ Playing trick script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_START_PASS:
            if(command.getCasterId() == _playerId)
            {
                INF << L"PLAYER pass\n";
                INF << L"-{ Playing pass start script\n";
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing pass start script\n";
                LM_ASSERT(command.getAvailableActions().size() == 0);
            }
            else
            {
                INF << L"OPPONENT pass, " << Core::toString(command.getAvailableActions().size()) << L" possibilities\n";
                INF << L"-{ Playing pass start script\n";
                //Core::Thread::sleep(1000);
                Core::Thread::sleep(Core::irand(_config.turnMinDuration * 500, _config.turnMaxDuration * 500));
                action(command.getAvailableActions());
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing pass start script\n";
            }
            break;

        case PLAYER_PASS_FINISH:
            INF << L"-{ Playing pass finish script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing pass finish script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_PASS_FINISH_AFTER_TRICK:
            if(command.getCasterId() == _playerId)
                INF << L"PLAYER cast trick " <<  Core::toString(command.getSpellId()) << L"\n";
            else
                INF << L"OPPONENT cast trick " <<  Core::toString(command.getSpellId()) << L"\n";

            INF << L"-{ Playing trick script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick script\n";

            INF << L"-{ Playing pass finish script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing pass finish script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_CAST_RETRIBUTION:
            if(command.getCasterId() == _playerId)
            {
                INF << L"PLAYER cast retribution " << Core::toString(command.getSpellId()) << L"\n";
                displayStatus();
                INF << L"-{ Playing retribution incant script\n";
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing retribution incant script\n";
                LM_ASSERT(command.getAvailableActions().size() == 0);
            }
            else
            {
                INF << L"OPPONENT cast retribution " << Core::toString(command.getSpellId()) << L", "
                    << Core::toString(command.getAvailableActions().size()) << L" possibilities\n";
                INF << L"-{ Playing retribution incant script\n";
                //Core::Thread::sleep(1000);
                Core::Thread::sleep(Core::irand(_config.turnMinDuration * 500, _config.turnMaxDuration * 500));
                action(command.getAvailableActions());
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
                INF << L"}- Playing retribution incant script\n";
            }
            break;

        case PLAYER_RETRIBUTION_HIT:
            INF << L"-{ Playing retribution hit script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing retribution hit script\n";
            displayStatus();
            synchro();
            break;

        case PLAYER_RETRIBUTION_HIT_AFTER_TRICK:
            if(command.getCasterId() == _playerId)
                INF << L"PLAYER cast trick " <<  Core::toString(command.getSpellId()) << L"\n";
            else
                INF << L"OPPONENT cast trick " <<  Core::toString(command.getSpellId()) << L"\n";

            INF << L"-{ Playing trick script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing trick script\n";
            INF << L"-{ Playing retribution hit script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing retribution hit script\n";
            displayStatus();
            synchro();
            break;

        case GAME_FINISHED:
            {
            bool opponentLeft = false;
            _gameFinished = true;

            if(command.getCasterId() == _playerId)
                INF << L"PLAYER win the match\n";
            else if(command.getCasterId() == 0)
                INF << L"Game ends in a DRAW\n";
            else
                INF << L"OPPONENT win the match\n";
            displayStatus();

            INF << L"-{ Playing end script\n";
            Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            INF << L"}- Playing end script\n";
            
            if(receive(command, false))
            {
                if(command.getCommand() == HOOUserProtocol::PLAYER_LEFT)
                {
                    INF << L"OPPONENT left\n";
                    opponentLeft = true;
                }
            }

            if(!opponentLeft)
            {
                INF << L"Waiting a bit to chat with opponent\n";
                Core::Thread::sleep(Core::irand(_config.scriptMinDuration * 1000, _config.scriptMaxDuration * 1000));
            }

            {
                HOOUserProtocol::GameCommandAnswer command;

                if(receive(command, false))
                {
                    if(command.getCommand() == HOOUserProtocol::PLAYER_LEFT)
                    {
                        INF << L"OPPONENT left\n";
                        opponentLeft = true;
                    }
                    else
                        INF << L"Unexpected command " << HOOUserProtocol::toString(command.getCommand()) << L"\n";
                }
            }

            if(!opponentLeft)
            {
                INF << L"=> Leave game\n";
                HOOUserProtocol::GameCommand command(HOOUserProtocol::LEAVE_GAME);
                send(command);
            }
            break;
            }
        default:
            break;
        }
    }
    else
    {
        Core::Thread::sleep(100);
    }
}

String ClientBot::toStringCount(int32 c)
{
    if(c == -1)
        return L"INFINITE";
    else
        return Core::toString(c);
}

void ClientBot::removeMessages()
{
    bool messageToProcess = false;
    
    do
    {
        Network::Message * incomingMessage = NULL;
        {
            Core::ScopedCondLock lock(_conditionMutex);
            messageToProcess = _networkEngine.getMessage(incomingMessage);
        }

        if(incomingMessage != NULL)
        {
            Network::EMessageType messageType = incomingMessage->getMessageType();

            switch(messageType)
            {
            case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            case Network::APPLICATION_MESSAGE:
                {
                    if(incomingMessage->getDataSize() >= sizeof(HOOUserProtocol::EUserAnswerType))
                    {
                        HOOUserProtocol::EUserAnswerType userAnswerType;
                        incomingMessage->readData(&userAnswerType, sizeof(HOOUserProtocol::EUserAnswerType));

                        INF << L"Ignoring message " << HOOUserProtocol::toString(userAnswerType) << L"\n";
                    }
                    else
                    {
                        throw Network::NetworkException(L"Received invalid message!");
                    }
                }
                break;
            default:
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            }

            delete incomingMessage;
            incomingMessage = NULL;
        }
    } while(messageToProcess);
}

bool ClientBot::receive(HOOUserProtocol::IUserAnswer & message, bool blocking)
{
    bool messageToProcess = false;
    
    do
    {
        Network::Message * incomingMessage = NULL;

        if(blocking)
        {
            Core::ScopedCondLock lock(_conditionMutex);
            while(!_networkEngine.getMessage(incomingMessage))
                _conditionVar.wait(lock);

            messageToProcess = true;
        }
        else
        {
            Core::ScopedCondLock lock(_conditionMutex);
            messageToProcess = _networkEngine.getMessage(incomingMessage);
        }

        if(incomingMessage != NULL)
        {
            Network::EMessageType messageType = incomingMessage->getMessageType();

            switch(messageType)
            {
            case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            case Network::APPLICATION_MESSAGE:
                {
                    if(incomingMessage->getDataSize() >= sizeof(HOOUserProtocol::EUserAnswerType))
                    {
                        HOOUserProtocol::EUserAnswerType userAnswerType;
                        incomingMessage->readData(&userAnswerType, sizeof(HOOUserProtocol::EUserAnswerType));

                        if(userAnswerType == message.getType())
                            incomingMessage->getMessage(message);
                        else
                        {
                            INF << L"Ignoring message " << HOOUserProtocol::toString(userAnswerType) << L"\n";
                            messageToProcess = false;
                        }
                    }
                    else
                    {
                        throw Network::NetworkException(L"Received invalid message!");
                    }
                }
                break;
            default:
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            }

            delete incomingMessage;
            incomingMessage = NULL;
        }
    } while(blocking && !messageToProcess);

    return messageToProcess;
}

void ClientBot::send(HOOUserProtocol::IUserRequest & message)
{
    Network::Message messageToSend(Network::APPLICATION_MESSAGE, message, _sessionId);
    _networkEngine.sendMessage(messageToSend);
}

}
