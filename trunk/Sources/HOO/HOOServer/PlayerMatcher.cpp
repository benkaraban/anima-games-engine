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
#include "PlayerMatcher.h"

#include <Core/Logger.h>
#include <Core/RT/Thread.h>
#include <Core/Math/Random.h>

#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>

namespace HOOServer
{

void PlayerMatcher::initialise()
{
    _levels.clear();
    const Core::XMLDocument & conf = _serverData.configFile;

    String msg(L"Available levels      : ");
    Core::List<Ptr<Core::XMLNode> > levels;
    conf.getNodes(L"server.levels", L"level", levels);

    for(int32 ii=0; ii < levels.size(); ii++)
    {
        int32 level = 0;
        levels[ii]->getValue(L"", level);
        _levels.push_back(level);

        msg << Core::toString(level) << L" ";
    }
    INF << msg << L"\n";

    if(_levels.empty())
        throw Core::Exception(L"No levels available for game");
}

void PlayerMatcher::run()
{
    while(!_serverData.isBeingKilled)
    {
        Core::List<Session *> lookingForOpponentCopy;
        {
            Core::ScopedLock lock(_serverData.lookingForOpponentUsersMutex);

            lookingForOpponentCopy.resize(_serverData.lookingForOpponentUsers.size());
            std::copy(  _serverData.lookingForOpponentUsers.begin(), 
                        _serverData.lookingForOpponentUsers.end(),
                        lookingForOpponentCopy.begin());
            
        }

        if(lookingForOpponentCopy.size() > 1)
        {
            for(int32 iOpponents = 0; (iOpponents + 1) < lookingForOpponentCopy.size(); iOpponents += 2)
            {
                Game * pGame = _serverData.gamePool.getNewGame();
                if(pGame != NULL)
                {
                    Session * player1 = lookingForOpponentCopy[iOpponents];
                    Session * player2 = lookingForOpponentCopy[iOpponents + 1];

                    //On retrouve les sessions pour les sortir de la liste des joueurs en attente
                    if(removeChallengersFromLookingForOpponents(player1, player2))
                    {
                        int32 levelId = _levels[Core::irand(0, _levels.size() - 1)];
                        pGame->setChallengers(player1, player2, levelId);

                        HOOUserProtocol::QuickMatchAnswer quickMatchAnswer(HOOUserProtocol::OPPONENT_FOUND);

                        Network::Message answerMessagePlayer1(Network::APPLICATION_MESSAGE, quickMatchAnswer, player1->getIdSession());
                        Network::Message answerMessagePlayer2(Network::APPLICATION_MESSAGE, quickMatchAnswer, player2->getIdSession());
                        _userSessionManager.sendAnswer(answerMessagePlayer1);
                        _userSessionManager.sendAnswer(answerMessagePlayer2);
                    }
                    else
                    {
                        pGame->noChallengers();
                    }
                }
                else
                {
                    throw Core::Exception(L"No More game available!?!");
                }
            }
        }

        Core::Thread::sleep(50);
    }
}

bool PlayerMatcher::removeChallengersFromLookingForOpponents(Session * player1, Session * player2)
{
    Core::ScopedLock lock(_serverData.lookingForOpponentUsersMutex);

    bool player1Found = false;
    bool player2Found = false;

    for(int32 ii = 0; (ii < _serverData.lookingForOpponentUsers.size()) && (!player1Found || !player2Found); ++ii)
    {
        if(_serverData.lookingForOpponentUsers[ii] == player1)
            player1Found = true;
        if(_serverData.lookingForOpponentUsers[ii] == player2)
            player2Found = true;
    }

    if(player1Found && player2Found)
    {
        Core::List<Session *>::iterator iPlayer1 = _serverData.lookingForOpponentUsers.begin();

        while(iPlayer1 != _serverData.lookingForOpponentUsers.end() && *iPlayer1 != player1)
        {
            iPlayer1++;
        }

        if(iPlayer1 != _serverData.lookingForOpponentUsers.end())
        {
            _serverData.lookingForOpponentUsers.erase(iPlayer1);
        }

        Core::List<Session *>::iterator iPlayer2 = _serverData.lookingForOpponentUsers.begin();

        while(iPlayer2 != _serverData.lookingForOpponentUsers.end() && *iPlayer2 != player2)
        {
            iPlayer2++;
        }

        if(iPlayer2 != _serverData.lookingForOpponentUsers.end())
        {
            _serverData.lookingForOpponentUsers.erase(iPlayer2);
        }
    }
    return (player1Found && player2Found);
}

}//namespace HOOServer
