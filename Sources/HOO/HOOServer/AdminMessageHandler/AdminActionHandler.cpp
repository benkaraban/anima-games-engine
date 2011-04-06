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
#include "AdminActionHandler.h"

#include <Core/Logger.h>
#include <HOOServer/DisconnectException.h>

#include <HOOAdminProtocol/ConnectedUsersInfos.h>
#include <HOOAdminProtocol/ServerStats.h>
#include <HOOAdminProtocol/BanUsers.h>
#include <HOOAdminProtocol/LockUsers.h>
#include <HOOAdminProtocol/SendMsgToUsers.h>

namespace HOOServer
{

AdminActionHandler::AdminActionHandler(AdminSessionManager & sessionManager)
:   _sessionManager(sessionManager)
{}

AdminActionHandler::~AdminActionHandler()
{}

void AdminActionHandler::handleMessage(HOOAdminProtocol::EAdminRequestType reqType, int32 idSession, Network::Message * request)
{
    if(_sessionManager.getSession(idSession).isLoggedIn())
    {
        switch(reqType)
        {
        case HOOAdminProtocol::CONNECTED_USERS_INFOS:
            {
            HOOAdminProtocol::ConnectedUsersInfos msg;
            request->getMessage(msg);
            connectedUsersInfosProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::SERVER_STATS:
            {
            HOOAdminProtocol::ServerStats msg;
            request->getMessage(msg);
            serverStatsProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::BAN_USERS:
            {
            HOOAdminProtocol::BanUsers msg;
            request->getMessage(msg);
            banUsersProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::UNBAN_USERS:
            {
            HOOAdminProtocol::UnbanUsers msg;
            request->getMessage(msg);
            unbanUsersProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::LOCK_USERS:
            {
            HOOAdminProtocol::LockUsers msg;
            request->getMessage(msg);
            lockUsersProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::UNLOCK_USERS:
            {
            HOOAdminProtocol::UnlockUsers msg;
            request->getMessage(msg);
            unlockUsersProcessing(msg, idSession);
            }
            break;
        case HOOAdminProtocol::SEND_MSG_TO_USERS:
            {
            HOOAdminProtocol::SendMessageToUsers msg;
            request->getMessage(msg);
            sendMessageToUsersProcessing(msg, idSession);
            }
            break;
        default:
            WAR << L"AdminActionHandler received an action while last message sent was " << HOOAdminProtocol::toString(reqType)
                << L"this message is not processing";
            break;
        }
    }
    else
    {
        throw DisconnectException(L"Sending request while not logged in.", NOT_LOGGED_IN);
    }
}

void AdminActionHandler::connectedUsersInfosProcessing(const HOOAdminProtocol::ConnectedUsersInfos& msg, int32 idSession)
{
    Core::List<bool> loggedIn;
    Core::List<String> logins;
    Core::List<EQuickMatchStatus> qmatchStatus;
    _sessionManager.getUsersInfos(loggedIn, logins, qmatchStatus);

    Core::List<HOOAdminProtocol::UserInfos>   infos;

    LM_ASSERT((logins.size() == qmatchStatus.size()) && (logins.size() == loggedIn.size()));

    infos.resize(logins.size());

    for (int32 ii=0; ii<infos.size(); ++ii)
    {
        infos[ii].loggedIn      = loggedIn[ii];
        infos[ii].login         = logins[ii];
        infos[ii].qMatchStatus  = int32(qmatchStatus[ii]);
    }

    HOOAdminProtocol::ConnectedUsersInfosAnswer answerMsg (infos);

    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::serverStatsProcessing(const HOOAdminProtocol::ServerStats& msg, int32 idSession)
{
    HOOAdminProtocol::ServerStatsAnswer answerMsg (
        Core::Clock::universalTime(),
        Core::Clock::applicationTime());

    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::banUsersProcessing(const HOOAdminProtocol::BanUsers& msg, int32 idSession)
{
    Core::List<bool> banned;
    _sessionManager.banUsers(msg.getLogins(), msg.getNbDay(), banned);

    HOOAdminProtocol::BanUsersAnswer answerMsg (msg.getLogins(), banned);
    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::unbanUsersProcessing(const HOOAdminProtocol::UnbanUsers& msg, int32 idSession)
{
    Core::List<bool> unbanned;
    _sessionManager.unbanUsers(msg.getLogins(), unbanned);

    HOOAdminProtocol::UnbanUsersAnswer answerMsg (msg.getLogins(), unbanned);
    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::lockUsersProcessing(const HOOAdminProtocol::LockUsers& msg, int32 idSession)
{
    Core::List<bool> locked;
    _sessionManager.lockUsers(msg.getLogins(), locked);

    HOOAdminProtocol::LockUsersAnswer answerMsg (msg.getLogins(), locked);
    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::unlockUsersProcessing(const HOOAdminProtocol::UnlockUsers& msg, int32 idSession)
{
    Core::List<bool> unlocked;
    _sessionManager.unlockUsers(msg.getLogins(), unlocked);

    HOOAdminProtocol::UnlockUsersAnswer answerMsg (msg.getLogins(), unlocked);
    Network::Message answer(Network::APPLICATION_MESSAGE, answerMsg, idSession);
    _sessionManager.sendAnswer(answer);
}

void AdminActionHandler::sendMessageToUsersProcessing(const HOOAdminProtocol::SendMessageToUsers& msg, int32 idSession)
{
    _sessionManager.sendMessageToUsers(msg.getTextMsg(), msg.getForAllUsers(), msg.getLogins());
}

}//namespace HOOServer
