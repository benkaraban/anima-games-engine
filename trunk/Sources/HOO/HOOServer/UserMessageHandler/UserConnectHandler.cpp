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
#include "UserConnectHandler.h"

#include <HOOServer/DisconnectException.h>
#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/MaintenanceMessage.h>

namespace HOOServer
{

UserConnectHandler::UserConnectHandler(ISessionManager & sessionManager)
:   _sessionManager(sessionManager)
{}

UserConnectHandler::~UserConnectHandler()
{}

void UserConnectHandler::handleMessage(HOOUserProtocol::EUserRequestType reqType, int32 idSession, Network::Message * request)
{
    if(_sessionManager.getSession(idSession).isConnecting())
    {
        HOOUserProtocol::UserConnect userConnect;
        request->getMessage(userConnect);

        HOOUserProtocol::EUserConnectAnswerType connectAnswer 
            = (HOOUserProtocol::EUserConnectAnswerType)_sessionManager.connect(userConnect.getVersion());

        if( connectAnswer == HOOUserProtocol::USER_CONNECT_OK)
        {
            _sessionManager.getSession(idSession).connect();
        }
        
        HOOUserProtocol::UserConnectAnswer userConnectAnswer(connectAnswer);
        Network::Message answerMessage(Network::APPLICATION_MESSAGE, userConnectAnswer, idSession);
        _sessionManager.sendAnswer(answerMessage);
        
        int32 timeBeforeShutdown = _sessionManager.getTimeBeforeShutdown();
        if(timeBeforeShutdown >= 0)
        {
            HOOUserProtocol::MaintenanceMessageAnswer maintenanceMessage(HOOUserProtocol::MAINTENANCE_SERVER, timeBeforeShutdown);
            Network::Message maintenanceAnswerMessage(Network::APPLICATION_MESSAGE, maintenanceMessage, idSession);
            _sessionManager.sendAnswer(maintenanceAnswerMessage);
        }
        
    }
    else
    {
        throw DisconnectException(L"Trying to connect on a session in a Wrong status.", SESSION_NOT_OPENED);
    }
}

}//namespace HOOServer