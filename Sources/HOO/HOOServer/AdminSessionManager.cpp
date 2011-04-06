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
#include "AdminSessionManager.h"

#include <Network/Message.h>

#include <HOOServer/DisconnectException.h>
#include <HOOServer/UserSessionManager.h>
#include <HOOAdminProtocol/AdminLogin.h>
#include <HOOAdminProtocol/AdminConnect.h>

#include <HOOUserProtocol/AdminMessage.h>
#include <HOOUserProtocol/MaintenanceMessage.h>

#include <Core/Logger.h>

namespace HOOServer
{

AdminSessionManager::AdminSessionManager(ServerData & serverData, UserSessionManager & userSessionManager)
:   _serverData(serverData),
    _userSessionManager(userSessionManager)
{}
    
AdminSessionManager::~AdminSessionManager()
{}

void AdminSessionManager::openSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        Core::ScopedLock lock(_serverData.openedAdminSessionsMutex);
        if(_serverData.openedAdminSessions.size() < MAX_ADMIN_SESSIONS)
        {
            _serverData.adminSessions[idSession].open();
            _serverData.openedAdminSessions.push_back(&_serverData.adminSessions[idSession]);
        }
        else
        {
            String message;
            throw DisconnectException(L"Maximum connection reached.", MAXIMUM_CONNECTION_REACHED);
        }
    }
    else
    {
        Core::String message;
        message << L"Trying to open and admin session with an incorrect session Id : " << idSession << L".";
        //Unknown message type
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

void AdminSessionManager::closeSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        Core::ScopedLock lock(_serverData.openedAdminSessionsMutex);

        Core::List<Session *>::iterator iAdminSession = _serverData.openedAdminSessions.begin();
        while(iAdminSession != _serverData.openedAdminSessions.end() && (*iAdminSession)->getIdSession() != idSession)
        {
            ++iAdminSession;
        }

        if(iAdminSession != _serverData.openedAdminSessions.end())
        {
            (*iAdminSession)->close();

            _serverData.openedAdminSessions.erase(iAdminSession);
        }
    }
    else
    {
        Core::String message;
        message << L"Trying to close and admin session with an incorrect session Id : " << idSession << L".";
        //Unknown message type
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

Session &  AdminSessionManager::getSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        return _serverData.adminSessions[idSession];
    }
    else
    {
        String message;
        message << L"Trying to get and admin session with an incorrect session Id : " << idSession << L".";
        throw Core::Exception(message);
    }
}

bool AdminSessionManager::isIdSessionValid(int32 idSession)
{
    return (idSession >= 0 && idSession < Network::TOLERANT_SOCKET_MAX_CONNECTION);
}

int32 AdminSessionManager::connect(int32 version)
{
    if(version == _serverData.version)
        return HOOAdminProtocol::ADMIN_CONNECT_OK;
    else
        return HOOAdminProtocol::ADMIN_CONNECT_VERSION_ERROR;
}

int32 AdminSessionManager::login(int32 idSession, const String & login, const String & password, HOOUserProtocol::UserAccountInfos & infos)
{
    if(login == L"bill" && password == L"joe")
    {
        Session & session = getSession(idSession);

        Core::ScopedLock lock(session.getReadWriteMutex());
        UserAccount account;
        session.login(account);

        return HOOAdminProtocol::ADMIN_LOGIN_OK;
    }
    else
        return HOOAdminProtocol::ADMIN_LOGIN_FAILED;
}

void AdminSessionManager::logout(int32 idSession)
{
    Session & session = getSession(idSession);
    {
        Core::ScopedLock lock(session.getReadWriteMutex());
        session.logout();
    }
}

int32 AdminSessionManager::getTimeBeforeShutdown() const
{
    int32 timeBeforeShutdown = -1;
    
    if(_serverData.periodBeforeShutDown >= 0)
    {
        Core::Clock clock;
        Core::TimeValue timeValue = clock.universalTime();

        timeBeforeShutdown = (int32)((timeValue - _serverData.shutDownTime).totalSeconds());
    }

    return timeBeforeShutdown;
}

void AdminSessionManager::sendAnswer(Network::Message & networkMessage)
{
    _serverData.adminEngine.sendMessage(networkMessage);
}

int32 AdminSessionManager::countConnectedUsers()
{
    Core::ScopedLock lock(_serverData.openedUserSessionsMutex);

    return _serverData.openedUserSessions.size();
}


void AdminSessionManager::getUsersInfos(Core::List<bool> & loggedIn, Core::List<String>& logins, Core::List<EQuickMatchStatus>& qmatchStatus)
{
    Core::ScopedLock lock(_serverData.openedUserSessionsMutex);
    
    int32 nbUsers = _serverData.openedUserSessions.size();
    for(int32 ii=0; ii<nbUsers; ++ii)
    {
        loggedIn.push_back(_serverData.openedUserSessions[ii]->isLoggedIn());
        logins.push_back(_serverData.openedUserSessions[ii]->account().login);
        qmatchStatus.push_back(_serverData.openedUserSessions[ii]->getQuickMatchStatus());
    }

}

void AdminSessionManager::shutdown(int32 timeBeforeShutdown)
{
    Core::ScopedLock lock(_serverData.shutDownMutex);
    Core::Clock clock;
    _serverData.shutDownTime = clock.universalTime();
    _serverData.periodBeforeShutDown = timeBeforeShutdown;

    INF << L"Shutdown message received, time before shutdown : " << timeBeforeShutdown / 60 << L" min (" << timeBeforeShutdown << L" sec).";
}

//if (nbDay == 0) => ban for ever
void AdminSessionManager::banUsers(const Core::List<String>& logins, int32 nbDays, Core::List<bool> & banned)
{
    for(int32 iLogin = 0; iLogin < logins.size(); ++iLogin)
    {
        Core::List<Session *>::iterator iSession;
        {
            Core::ScopedLock lock(_serverData.openedUserSessionsMutex);
            iSession = _serverData.openedUserSessions.begin();

            while(iSession != _serverData.openedUserSessions.end() 
                && (*iSession)->isLoggedIn() 
                && (*iSession)->account().login != logins[iLogin])
            {
                ++iSession;
            }
        }

        if(iSession == _serverData.openedUserSessions.end())//User not connected
        {
            banned.push_back(_serverData.pDB->banUser(logins[iLogin], nbDays));
        }
        else
        {

            {
                Core::ScopedLock lock((*iSession)->getReadWriteMutex());
                (*iSession)->account().accountBanned = nbDays;
            }

            int32 idSession = (*iSession)->getIdSession();
            _userSessionManager.closeSession(idSession);

            Network::Message disconnectUser(Network::NETWORK_CLOSE_SESSION_MESSAGE, idSession);
            _serverData.userEngine.sendMessage(disconnectUser);

            banned.push_back(true);
        }
    }
}

void AdminSessionManager::unbanUsers(const Core::List<String>& logins, Core::List<bool> & unbanned)
{
    for(int32 iLogin = 0; iLogin < logins.size(); ++iLogin)
    {
        unbanned.push_back(_serverData.pDB->banUser(logins[iLogin], -1));
    }
}

void AdminSessionManager::lockUsers(const Core::List<String>& logins, Core::List<bool> & locked)
{
    for(int32 iLogin = 0; iLogin < logins.size(); ++iLogin)
    {
        Core::List<Session *>::iterator iSession;
        {
            Core::ScopedLock lock(_serverData.openedUserSessionsMutex);
            iSession = _serverData.openedUserSessions.begin();

            while(iSession != _serverData.openedUserSessions.end() 
                && (*iSession)->isLoggedIn() 
                && (*iSession)->account().login != logins[iLogin])
            {
                ++iSession;
            }
        }

        if(iSession == _serverData.openedUserSessions.end())//User not connected
        {
            locked.push_back(_serverData.pDB->lockAccount(logins[iLogin]));
        }
        else
        {

            {
                Core::ScopedLock lock((*iSession)->getReadWriteMutex());
                (*iSession)->account().accountLocked = true;
            }

            int32 idSession = (*iSession)->getIdSession();
            _userSessionManager.closeSession(idSession);

            HOOUserProtocol::MaintenanceMessageAnswer accountMaintenance(HOOUserProtocol::MAINTENANCE_USER_ACCOUNT, 0);
            Network::Message maintenanceMessage(Network::APPLICATION_MESSAGE, accountMaintenance, idSession);
            _serverData.userEngine.sendMessage(maintenanceMessage);

            Network::Message disconnectUser(Network::NETWORK_CLOSE_SESSION_MESSAGE, idSession);
            _serverData.userEngine.sendMessage(disconnectUser);

            locked.push_back(true);
        }
    }
}

void AdminSessionManager::unlockUsers(const Core::List<String>& logins, Core::List<bool> & unlocked)
{
    for(int32 iLogin = 0; iLogin < logins.size(); ++iLogin)
    {
        unlocked.push_back(_serverData.pDB->unlockAccount(logins[iLogin]));
    }
}

void AdminSessionManager::sendMessageToUsers(const String& messageText, bool forAllUsers, const Core::List<String>& logins)
{
    if(forAllUsers)
    {
        INF << L"Broadcast admin message : \"" << messageText << L"\"";
    }
    else
    {
        INF << L"Send admin message : \"" << messageText << L"\" to users : " << Core::toString(logins);
    }

    Core::List<String> loginsToFind(logins);
    for(int32 iSession = 0; iSession < _serverData.openedUserSessions.size(); ++iSession)
    {
        Core::ScopedLock lock(_serverData.openedUserSessions[iSession]->getReadWriteMutex());
        bool sendMessage = false;

        if(_serverData.openedUserSessions[iSession]->isLoggedIn())
        {
            if(forAllUsers)
            {
                sendMessage = true;
            }
            else
            {
                Core::List<String>::iterator iLogin = loginsToFind.begin();
                while(iLogin != loginsToFind.end() && (*iLogin) != _serverData.openedUserSessions[iSession]->account().login)
                {
                    ++iLogin;
                }

                if(iLogin != loginsToFind.end())
                {
                    sendMessage = true;
                    loginsToFind.erase(iLogin);
                }
            }

            if(sendMessage)
            {
                HOOUserProtocol::AdminMessageAnswer adminMessage(messageText);

                Network::Message answer(Network::APPLICATION_MESSAGE, adminMessage, _serverData.openedUserSessions[iSession]->getIdSession());
                _serverData.userEngine.sendMessage(answer);
            }
        }
    }
}
}//namespace HOOServer
