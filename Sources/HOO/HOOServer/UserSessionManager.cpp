/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include "UserSessionManager.h"

#include <Network/Message.h>

#include <HOOServer/DisconnectException.h>

#include <HOOUserProtocol/UserLogin.h>
#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>

namespace HOOServer
{
UserSessionManager::UserSessionManager(ServerData & serverData)
:   _serverData(serverData)
{}
    
UserSessionManager::~UserSessionManager()
{}

void UserSessionManager::openSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        Core::ScopedLock lock(_serverData.openedUserSessionsMutex);
        if(_serverData.openedUserSessions.size() < MAX_USERS_SESSIONS)
        {
            _serverData.userSessions[idSession].open();
            _serverData.openedUserSessions.push_back(&_serverData.userSessions[idSession]);
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
        message << L"Trying to open and User session with an incorrect session Id : " << idSession << L".";
        //Unknown message type
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

void UserSessionManager::closeSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        EQuickMatchStatus qmStatus = QM_NOT_WAITING_FOR_QUICKMATCH;
        Game * pCurrentGame = NULL;
        
        {
            Core::ScopedLock lock(_serverData.userSessions[idSession].getReadWriteMutex());

            qmStatus = _serverData.userSessions[idSession].getQuickMatchStatus();
            pCurrentGame = _serverData.userSessions[idSession].getGame();

            _serverData.userSessions[idSession].setQuickMatchStatus(QM_NOT_WAITING_FOR_QUICKMATCH);
        }

        if(pCurrentGame != NULL)
            pCurrentGame->onPlayerDeconnection(idSession);

        if(qmStatus == QM_LOOKING_FOR_OPPONENT)
            removeFromLookingForOpponent(idSession);

        // Sauvegarde les infos du compte dans la BD
        {
            Core::ScopedLock lock(_serverData.userSessions[idSession].getReadWriteMutex());

            if(_serverData.userSessions[idSession].isLoggedIn())
            {
                UserAccount & account = _serverData.userSessions[idSession].account();
                account.isLogged = false;
                _serverData.pDB->updateAccount(account);
                account.reset();
            }
        }

        {
            Core::ScopedLock lock(_serverData.openedUserSessionsMutex);

            Core::List<Session *>::iterator iUserSession = _serverData.openedUserSessions.begin();
            while(iUserSession != _serverData.openedUserSessions.end() && (*iUserSession)->getIdSession() != idSession)
            {
                ++iUserSession;
            }

            if(iUserSession != _serverData.openedUserSessions.end())
            {
                (*iUserSession)->close();

                _serverData.openedUserSessions.erase(iUserSession);
            }
        }
    }
    else
    {
        Core::String message;
        message << L"Trying to close and User session with an incorrect session Id : " << idSession << L".";
        //Unknown message type
        throw DisconnectException(message, WRONG_PROTOCOL);
    }
}

Session &  UserSessionManager::getSession(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        return _serverData.userSessions[idSession];
    }
    else
    {
        String message;
        message << L"Trying to get and User session with an incorrect session Id : " << idSession << L".";
        throw Core::Exception(message);
    }
}

bool UserSessionManager::isIdSessionValid(int32 idSession)
{
    return (idSession >= 0 && idSession < Network::TOLERANT_SOCKET_MAX_CONNECTION);
}

int32 UserSessionManager::connect(int32 version)
{
    if(version == _serverData.version)
        return HOOUserProtocol::USER_CONNECT_OK;
    else
        return HOOUserProtocol::USER_CONNECT_VERSION_ERROR;
}

int32 UserSessionManager::login(int32 idSession, const String & login, const String & password, HOOUserProtocol::UserAccountInfos & infos)
{
    UserAccount account;
    account.login = login;
    account.password = password;

    HOOUserProtocol::EUserLoginAnswerType result = _serverData.pDB->login(account);

    if(result == HOOUserProtocol::USER_LOGIN_OK)
    {
        Session & session = getSession(idSession);
        Core::ScopedLock lock(session.getReadWriteMutex());
        session.login(account);
        account.getInfos(infos);
    }

    return result;
}

int32 UserSessionManager::loginAcCode(int32 idSession, const String & login, const String & acCode, HOOUserProtocol::UserAccountInfos & infos)
{
    UserAccount account;

    HOOUserProtocol::EUserLoginAnswerType result = _serverData.pDB->loginAcCode(account, login, acCode);

    if(result == HOOUserProtocol::USER_LOGIN_OK)
    {
        Session & session = getSession(idSession);
        Core::ScopedLock lock(session.getReadWriteMutex());
        session.login(account);
        account.getInfos(infos);
    }

    return result;
}

void UserSessionManager::logout(int32 idSession)
{
    Session & session = getSession(idSession);
    {
        Core::ScopedLock lock(session.getReadWriteMutex());

        UserAccount & account = session.account();
        account.isLogged = false;

        _serverData.pDB->updateAccount(account);
        session.logout();
    }
}

int32 UserSessionManager::getTimeBeforeShutdown() const
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

void UserSessionManager::sendAnswer(Network::Message & networkMessage)
{
    _serverData.userEngine.sendMessage(networkMessage);
}

HOOUserProtocol::ECheckLoginAvailableAnswerType UserSessionManager::checkLoginAvailability(const Core::String & login)
{
    if(login == L"fuck")
        return HOOUserProtocol::USER_LOGIN_FORBIDDEN;
    else if(!isValidLogin(login))
        return HOOUserProtocol::USER_LOGIN_INCORRECT;
    else
        return _serverData.pDB->checkLoginAvailability(login);
}

HOOUserProtocol::ECreateUserAccountAnswerType UserSessionManager::createUserAccount(int32 idSession, const UserAccount & account)
{
    if(account.login == L"fuck")
        return HOOUserProtocol::LOGIN_FORBIDDEN;
    else if(!isValidLogin(account.login))
        return HOOUserProtocol::LOGIN_INCORRECT;
    else if(!isValidPassword(account.password))
        return HOOUserProtocol::PWD_INCORRECT;
    else if(!isValidMail(account.mail))
        return HOOUserProtocol::MAIL_INCORRECT;
    else
    {
        HOOUserProtocol::ECreateUserAccountAnswerType result = _serverData.pDB->createAccount(account);

        if(result == HOOUserProtocol::USER_ACCOUNT_CREATED)
        {
            Session & session = getSession(idSession);
            Core::ScopedLock lock(session.getReadWriteMutex());
            session.login(account);
        }

        return result;
    }
}

HOOUserProtocol::ESelectCharacterAnswerType UserSessionManager::selectCharacter(int32 idSession, const UserAccount & updateInfos, HOOUserProtocol::UserAccountInfos & infos)
{
    Session & session = getSession(idSession);
    {
        Core::ScopedLock lock(session.getReadWriteMutex());

        UserAccount & account = session.account();

        if(account.characterSelectionToken <= 0)
        {
            return HOOUserProtocol::CHARACTER_SELECTED_FAILED_NO_TOKEN;
        }
        else
        {
            account.character     = updateInfos.character;
            account.spells        = updateInfos.spells;
            account.stuff         = updateInfos.stuff;
            account.equipedSpells = updateInfos.equipedSpells;
            account.equipedStuff  = updateInfos.equipedStuff;

            account.characterSelectionToken --;
            account.getInfos(infos);

            _serverData.pDB->updateAccount(account);

            return HOOUserProtocol::CHARACTER_SELECTED_OK;
        }
    }
}


bool UserSessionManager::isValidLogin(const String & login)
{
    // TODO : check caractères avec une regex
    return true;
}

bool UserSessionManager::isValidPassword(const String & password)
{
    // TODO : check caractères avec une regex
    return true;
}

bool UserSessionManager::isValidMail(const String & mail)
{
    // TODO : check caractères avec une regex
    return true;
}

bool UserSessionManager::quickMatch(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        EQuickMatchStatus qmStatus = QM_NOT_WAITING_FOR_QUICKMATCH;
        {
            Core::ScopedLock lock(_serverData.userSessions[idSession].getReadWriteMutex());
            qmStatus = _serverData.userSessions[idSession].getQuickMatchStatus();
            if(qmStatus != QM_NOT_WAITING_FOR_QUICKMATCH)
            {
                return false;
            }
            _serverData.userSessions[idSession].setQuickMatchStatus(QM_LOOKING_FOR_OPPONENT);
        }

        {
            Core::ScopedLock lock(_serverData.lookingForOpponentUsersMutex);
            _serverData.lookingForOpponentUsers.push_back(&_serverData.userSessions[idSession]);

            return true;
        }
    }
    else
    {
        String message;
        message << L"Trying to launch a quick match with an incorrect session Id : " << idSession << L".";
        throw Core::Exception(message);
    }
}

void UserSessionManager::cancelQuickMatch(int32 idSession)
{
    if(isIdSessionValid(idSession))
    {
        {
            Core::ScopedLock lock(_serverData.userSessions[idSession].getReadWriteMutex());
            _serverData.userSessions[idSession].setQuickMatchStatus(QM_NOT_WAITING_FOR_QUICKMATCH);
        }
        removeFromLookingForOpponent(idSession);
    }
    else
    {
        String message;
        message << L"Trying to cancel a quick match with an incorrect session Id : " << idSession << L".";
        throw Core::Exception(message);
    }
}

void UserSessionManager::removeFromLookingForOpponent(int32 idSession)
{
    Core::ScopedLock lock(_serverData.lookingForOpponentUsersMutex);

    Core::List<Session *>::iterator iUserSession = _serverData.lookingForOpponentUsers.begin();
    while(iUserSession != _serverData.lookingForOpponentUsers.end() && (*iUserSession)->getIdSession() != idSession)
    {
        ++iUserSession;
    }

    if(iUserSession != _serverData.lookingForOpponentUsers.end())
    {
        _serverData.lookingForOpponentUsers.erase(iUserSession);
    }
}

}//namespace HOOServer
