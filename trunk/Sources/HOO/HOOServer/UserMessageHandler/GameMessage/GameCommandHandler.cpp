/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "GameCommandHandler.h"

#include <HOOServer/DisconnectException.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>

#include <Network/NetworkException.h>

namespace HOOServer
{

GameCommandHandler::GameCommandHandler(UserSessionManager & sessionManager)
:   _sessionManager(sessionManager)
{}


void GameCommandHandler::handleMessage(HOOUserProtocol::EUserRequestType reqType, int32 idSession, Network::Message * request)
{
    EQuickMatchStatus qmStatus = QM_NOT_WAITING_FOR_QUICKMATCH;
    Game * pGame = NULL;
    {
        Core::ScopedLock lock(_sessionManager.getSession(idSession).getReadWriteMutex());
        pGame = _sessionManager.getSession(idSession).getGame();
        qmStatus = _sessionManager.getSession(idSession).getQuickMatchStatus();
    }

    if(pGame == NULL)
    {
        if(reqType == HOOUserProtocol::USER_CANCEL_QUICK_MATCH)
        {
            // 

            if(qmStatus == QM_LOOKING_FOR_OPPONENT)
            {
                HOOUserProtocol::CancelQuickMatch cancel;
                request->getMessage(cancel);

                _sessionManager.cancelQuickMatch(idSession);

                HOOUserProtocol::CancelQuickMatchAnswer cancelQuickMatchAnswer(HOOUserProtocol::QUICK_MATCH_CANCELLED);

                Network::Message answerMessage(Network::APPLICATION_MESSAGE, cancelQuickMatchAnswer, idSession);
                _sessionManager.sendAnswer(answerMessage);
            }
            else
            {
                Core::String message;
                message << L"Trying to cancel a quick match with a wrong QuickMatchStatus : "<<HOOServer::toString(qmStatus);
                throw DisconnectException(message, WRONG_PROTOCOL);
            }
        }

        // La partie a ÈtÈ dÈtruite aprËs l'envoi du message cÙtÈ client, c'est normal si l'adversaire a droppÈ.
        // Il ne faut pas pÈnaliser l'expÈditeur => le message est ignorÈ.
    }
    else
    {
        if(reqType == HOOUserProtocol::USER_GAME_COMMAND)
        {
            HOOUserProtocol::GameCommand command;
            request->getMessage(command);
            pGame->onReceiveCommand(command, idSession);
        }
        else if(reqType == HOOUserProtocol::USER_LOADING)
        {
            HOOUserProtocol::UserLoading loading;
            request->getMessage(loading);
            pGame->onReceiveLoading(loading, idSession);
        }
        else if(reqType == HOOUserProtocol::USER_LAUNCH_MATCH)
        {
            HOOUserProtocol::LaunchMatch launch;
            request->getMessage(launch);
            pGame->onReceiveLaunch(launch, idSession);
        }
        else if(reqType == HOOUserProtocol::USER_CANCEL_QUICK_MATCH)
        {
            HOOUserProtocol::CancelQuickMatch cancel;
            request->getMessage(cancel);
            pGame->onReceiveCancel(cancel, idSession);
        }
        else
        {
            throw Core::Exception(L"Unexpected message");
        }
    }
}

}//namespace HOOServer