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
#include "Session.h"

#include <HOOServer/DisconnectException.h>
#include <HOOServer/Game.h>

namespace HOOServer
{

Session::Session()
:   _idSession(-1),
    _sessionStatus(SESSION_CLOSED),
    _quickMatchStatus(QM_NOT_WAITING_FOR_QUICKMATCH),
    _game(NULL)
{
}

void Session::close()
{
    _sessionStatus      = SESSION_CLOSED;
    _quickMatchStatus   = QM_NOT_WAITING_FOR_QUICKMATCH;
    _game               = NULL;
}

void Session::open()
{
    if(isClosed())
    {
        _sessionStatus      = SESSION_CONNECTING;
        _quickMatchStatus   = QM_NOT_WAITING_FOR_QUICKMATCH;
        _game               = NULL;
    }
    else
    {
        Core::String message;
        message << L"Attempting to OPEN a session in a wrong status. Status :" << HOOServer::toString(_sessionStatus);
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

void Session::connect()
{
    if(_sessionStatus == SESSION_CONNECTING)
    {
        _sessionStatus = SESSION_CONNECTED;
    }
    else
    {
        Core::String message;
        message << L"Attempting to CONNECT a session in a wrong status. Status :" << HOOServer::toString(_sessionStatus);
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

void Session::login(const UserAccount & account)
{
    if(_sessionStatus == SESSION_CONNECTED)
    {
        _account = account;
        _sessionStatus = SESSION_LOGGED;
        _quickMatchStatus = QM_NOT_WAITING_FOR_QUICKMATCH;
    }
    else
    {
        Core::String message;
        message << L"Attempting to LOG ON a session in a wrong status. Status :" << HOOServer::toString(_sessionStatus);
        throw DisconnectException(message, WRONG_PROTOCOL);
    }    
}

void Session::logout()
{
    _sessionStatus = SESSION_CONNECTED;
    _quickMatchStatus = QM_NOT_WAITING_FOR_QUICKMATCH;
    _account.login = L"";
}

void Session::opponentFound(Game * game)
{
    _quickMatchStatus = QM_OPPONENT_FOUND;
    _game = game;
}

void Session::pushMessage(Network::Message * message)
{
    Core::ScopedLock lock(_queueMutex);

    _messages.push(message);
}

Network::Message * Session::popMessage()
{
    Core::ScopedLock lock(_queueMutex);

    Network::Message * message;
    message = _messages.front();
    _messages.pop();

    return message;
}

}//namespace HOOServer
