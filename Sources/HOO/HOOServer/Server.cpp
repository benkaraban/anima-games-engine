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
#include "Server.h"

#include <Core/Logger.h>
#include <Core/VFS/VFSDirectMapping.h>


#include <Network/Message.h>
#include <Network/NetworkException.h>

#include <HOOServer/DisconnectException.h>

#include <HOOServer/Database/DatabaseXML.h>

#include <HOOServer/AdminMessageHandler/AdminConnectHandler.h>
#include <HOOServer/AdminMessageHandler/AdminActionHandler.h>
#include <HOOServer/AdminMessageHandler/AdminLoginHandler.h>
#include <HOOServer/AdminMessageHandler/AdminLogoutHandler.h>
#include <HOOServer/AdminMessageHandler/ShutdownHandler.h>

#include <HOOServer/UserMessageHandler/UserNullHandler.h>
#include <HOOServer/UserMessageHandler/UserConnectHandler.h>
#include <HOOServer/UserMessageHandler/UserLoginHandler.h>
#include <HOOServer/UserMessageHandler/UserLogoutHandler.h>
#include <HOOServer/UserMessageHandler/CreateAccountHandler.h>
#include <HOOServer/UserMessageHandler/SendActivationCodeHandler.h>
#include <HOOServer/UserMessageHandler/LaunchMatch/QuickMatchHandler.h>
#include <HOOServer/UserMessageHandler/GameMessage/GameCommandHandler.h>

#include <HOOServer/UpdaterMessageHandler/UpdaterNullHandler.h>
#include <HOOServer/UpdaterMessageHandler/UpdaterConnectHandler.h>

namespace HOOServer
{

Server::Server(const String & configFileName)
:   _userSessionManager(_serverData),
    _adminSessionManager(_serverData, _userSessionManager),
    _playerMatcher(_serverData, _userSessionManager),
    _openAdminSessionHandler(_adminSessionManager),
    _closeAdminSessionHandler(_adminSessionManager),
    _openUserSessionHandler(_userSessionManager),
    _closeUserSessionHandler(_userSessionManager),
    _clientNbThreads(0),
    _updaterNbThreads(0)
{

    for(int32 ii = 0; ii < HOOAdminProtocol::EAdminRequestType_COUNT; ++ii)
        _adminMessageHandlers[ii] = NULL;

    for(int32 ii = 0; ii < HOOUserProtocol::EUserRequestType_COUNT; ++ii)
        _userMessageHandlers[ii] = NULL;

    for(int32 ii = 0; ii < HOOUpdaterProtocol::EUpdaterRequestType_COUNT; ++ii)
        _updaterMessageHandlers[ii] = NULL;
    

    INF << L"Initialising server with config file '" + configFileName + L"'\n";

    if(!_serverData.configFile.loadDocument(configFileName))
        throw Core::Exception(L"Unable to open config file '" + configFileName + L"'\n");

    _serverData.version             = getConfigValueInt32(L"server.version");
    _serverData.verbosity           = getConfigValueInt32(L"server.verbosity");
    _adminAddress                   = getConfigValueString(L"server.admin-address");
    _adminPort                      = getConfigValueInt32(L"server.admin-port");
    _clientAddress                  = getConfigValueString(L"server.client-address");
    _clientPort                     = getConfigValueInt32(L"server.client-port");
    _updaterAddress                 = getConfigValueString(L"server.updater-address");
    _updaterPort                    = getConfigValueInt32(L"server.updater-port");
    _connectionTimeOut              = getConfigValueInt32(L"server.connection-timeout");
    _clientNbThreads                = getConfigValueInt32(L"server.client-thread-count");
    _updaterNbThreads               = getConfigValueInt32(L"server.updater-thread-count");
    String dbMode                   = getConfigValueString(L"server.database.mode");
    String vfsMountPoint            = getConfigValueString(L"server.game-library.VFS-mountpoint");
    String gameLibDir               = getConfigValueString(L"server.game-library.lib-dir");
    String smtpServer               = getConfigValueString(L"server.smtp-server");
    String smtpLogin                = getConfigValueString(L"server.smtp-server-login");
    String smtpPassword             = getConfigValueString(L"server.smtp-server-password");
    String smtpPort                 = getConfigValueString(L"server.smtp-port");
    String senderMailAcCode         = getConfigValueString(L"server.sender-mail-ac-code");
    String mailPathAcCode_en        = getConfigValueString(L"server.mail-file-ac-code-en");
    String mailPathAcCode_fr        = getConfigValueString(L"server.mail-file-ac-code-fr");
    String mailPathAcCodeNologin_en = getConfigValueString(L"server.mail-file-ac-code-no-login-en");
    String mailPathAcCodeNoLogin_fr = getConfigValueString(L"server.mail-file-ac-code-no-login-fr");

    INF << L"-------------------------------------\n";
    INF << L"Version               : " << Core::toString(_serverData.version) << L"\n";
    INF << L"Verbosity             : " << Core::toString(_serverData.verbosity) << L"\n";
    
    if(_adminAddress == L"auto")
    {
        _adminAddress = Network::NetworkEngine::getHostIP();
        INF << L"Admin connection      : " << _adminAddress << L" (auto) : " << Core::toString(_adminPort) << L"\n";
    }
    else
    {
        INF << L"Admin connection      : " << _adminAddress << L" : " << Core::toString(_adminPort) << L"\n";
    }

    if(_clientAddress == L"auto")
    {
        _clientAddress = Network::NetworkEngine::getHostIP();
        INF << L"Client connection     : " << _clientAddress << L" (auto) : " << Core::toString(_clientPort) << L"\n";
    }
    else
    {
        INF << L"Client connection     : " << _clientAddress << L" : " << Core::toString(_clientPort) << L"\n";
    }

    if(_updaterAddress == L"auto")
    {
        _updaterAddress = Network::NetworkEngine::getHostIP();
        INF << L"Updater connection     : " << _updaterAddress << L" (auto) : " << Core::toString(_updaterPort) << L"\n";
    }
    else
    {
        INF << L"Updater connection     : " << _updaterAddress << L" : " << Core::toString(_updaterPort) << L"\n";
    }


    INF << L"Smtp server           : " << smtpServer;
    INF << L"Smtp server login     : " << smtpLogin;
    INF << L"Smtp server password  : " << smtpPassword;
    INF << L"Smtp port             : " << smtpPort; 


    INF << L"Connection timeout    : " << Core::toString(_connectionTimeOut) << L"\n";
    INF << L"Client Threads count  : " << Core::toString(_clientNbThreads) << L"\n";
    INF << L"Updater Threads count : " << Core::toString(_updaterNbThreads) << L"\n";

    INF << L"-------------------------------------\n";
    INF << L"GameLib VFS           : " << Core::toString(vfsMountPoint) << L"\n";
    INF << L"GameLib dir           : " << Core::toString(gameLibDir) << L"\n";
    {
        Ptr<Core::VFSDirectMapping> pVFS(new Core::VFSDirectMapping());
        pVFS->linkDirectoryThrow(vfsMountPoint);

        _serverData.pGameLib = new GameEngine::GameLibrary(pVFS, gameLibDir);
        _serverData.gamePool.initGames(_serverData.pGameLib, &_userSessionManager);

        INF << L"Loaded spells         : " << Core::toString(_serverData.pGameLib->spellCount()) << L"\n";
        INF << L"Loaded stuffs         : " << Core::toString(_serverData.pGameLib->stuffCount()) << L"\n";
    }

    _playerMatcher.initialise();

    INF << L"-------------------------------------\n";
    INF << L"Database mode         : " << Core::toString(dbMode) << L"\n";

    if(dbMode == L"XML")
    {
        String dbFile = getConfigValueString(L"server.database.file");
        INF << L"Database XML file     : " << Core::toString(dbFile) << L"\n";
        _serverData.pDB = new DatabaseXML(dbFile);
    }
    else
        throw Core::Exception(L"Unknown database driver");

    _serverData.pDB->connect();

    _threads.resize(_clientNbThreads + _updaterNbThreads + 2); //+ 2 : runAdminThread + PlayerMatcher::run

    for(int32 iThread = 0; iThread < _threads.size(); ++iThread)
        _threads[iThread] = NULL;

    //AdminSessionManager

    _adminMessageHandlers[HOOAdminProtocol::ADMIN_CONNECT]          = new AdminConnectHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::ADMIN_LOGIN]            = new AdminLoginHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::ADMIN_LOGOUT]           = new AdminLogoutHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::SHUTDOWN_SERVER]        = new ShutdownHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::CONNECTED_USERS_INFOS]  = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::SERVER_STATS]           = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::BAN_USERS]              = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::UNBAN_USERS]            = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::LOCK_USERS]             = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::UNLOCK_USERS]           = new AdminActionHandler(_adminSessionManager);
    _adminMessageHandlers[HOOAdminProtocol::SEND_MSG_TO_USERS]      = new AdminActionHandler(_adminSessionManager);

    for(int32 ii = 0; ii < HOOAdminProtocol::EAdminRequestType_COUNT; ++ii)
    {
        if(_adminMessageHandlers[ii] == NULL)
        {
            Core::String message;
            message << L"Handler for admin message type "<< HOOAdminProtocol::toString((HOOAdminProtocol::EAdminRequestType)ii) << L" is not set.";
            throw Core::Exception(message);
        }
    }

    //UserSessionManager

    _userMessageHandlers[HOOUserProtocol::NULL_REQUEST]                 = new UserNullHandler(_userSessionManager);
    _userMessageHandlers[HOOUserProtocol::USER_CONNECT]                 = new UserConnectHandler(_userSessionManager);
    _userMessageHandlers[HOOUserProtocol::USER_LOGIN]                   = new UserLoginHandler(_userSessionManager);
    _userMessageHandlers[HOOUserProtocol::USER_LOGOUT]                  = new UserLogoutHandler(_userSessionManager);

    _userMessageHandlers[HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE]   = new CreateAccountHandler(_userSessionManager);
    _userMessageHandlers[HOOUserProtocol::USER_CREATE_ACCOUNT]          = _userMessageHandlers[HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE];
    _userMessageHandlers[HOOUserProtocol::USER_SELECT_CHARACTER]        = _userMessageHandlers[HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE];
    _userMessageHandlers[HOOUserProtocol::USER_SEND_ACTIVATION_CODE]    = new SendActivationCodeHandler(_userSessionManager, smtpServer, smtpLogin, smtpPassword, smtpPort, senderMailAcCode, mailPathAcCode_en, mailPathAcCode_fr, mailPathAcCodeNologin_en, mailPathAcCodeNoLogin_fr);
    _userMessageHandlers[HOOUserProtocol::USER_LOGIN_WITH_ACTIVATION_CODE] = _userMessageHandlers[HOOUserProtocol::USER_SEND_ACTIVATION_CODE];

    _userMessageHandlers[HOOUserProtocol::USER_QUICK_MATCH]             = new QuickMatchHandler(_userSessionManager);

    _userMessageHandlers[HOOUserProtocol::USER_GAME_COMMAND]            = new GameCommandHandler(_userSessionManager);
    _userMessageHandlers[HOOUserProtocol::USER_LOADING]                 = _userMessageHandlers[HOOUserProtocol::USER_GAME_COMMAND];
    _userMessageHandlers[HOOUserProtocol::USER_LAUNCH_MATCH]            = _userMessageHandlers[HOOUserProtocol::USER_GAME_COMMAND];
    _userMessageHandlers[HOOUserProtocol::USER_CANCEL_QUICK_MATCH]      = _userMessageHandlers[HOOUserProtocol::USER_GAME_COMMAND];
    _userMessageHandlers[HOOUserProtocol::USER_CHAT_MESSAGE]            = _userMessageHandlers[HOOUserProtocol::USER_GAME_COMMAND];


    for(int32 ii = 0; ii < HOOUserProtocol::EUserRequestType_COUNT; ++ii)
    {
        if(_userMessageHandlers[ii] == NULL)
        {
            Core::String message;
            message << L"Handler for user message type "<< HOOUserProtocol::toString((HOOUserProtocol::EUserRequestType)ii) << L" is not set.";
            throw Core::Exception(message);
        }
    }


    //Updater
    _updaterMessageHandlers[HOOUpdaterProtocol::NULL_REQUEST]           = new UpdaterNullHandler();
    _updaterMessageHandlers[HOOUpdaterProtocol::UPDATER_CONNECT]        = new UpdaterConnectHandler(_serverData);

    for(int32 ii = 0; ii < HOOUpdaterProtocol::EUpdaterRequestType_COUNT; ++ii)
    {
        if(_updaterMessageHandlers[ii] == NULL)
        {
            Core::String message;
            message << L"Handler for updater message type "<< HOOUpdaterProtocol::toString((HOOUpdaterProtocol::EUpdaterRequestType)ii) << L" is not set.";
            throw Core::Exception(message);
        }
    }
}

Server::~Server()
{
    for(int32 ii = 0; ii<_threads.size(); ++ii)
    {
        if(_threads[ii] != NULL)
        {
            delete _threads[ii];
            _threads[ii] = NULL;
        }
    }

    for(int32 ii = 0; ii < HOOAdminProtocol::EAdminRequestType_COUNT; ++ii)
    {
        delete _adminMessageHandlers[ii];
        _adminMessageHandlers[ii] = NULL;
    }

    for(int32 ii = 0; ii < HOOUpdaterProtocol::EUpdaterRequestType_COUNT; ++ii)
    {
        delete _updaterMessageHandlers[ii];
        _updaterMessageHandlers[ii] = NULL;
    }
}

int32 Server::getConfigValueInt32(const String & path)
{
    int32 result = 0;

    if(!_serverData.configFile.getValue(path, result))
        throw Core::Exception(L"Missing '" + path + L"' entry in config file\n");

    return result;
}

String Server::getConfigValueString(const String & path)
{
    String result;

    if(!_serverData.configFile.getValue(path, result))
        throw Core::Exception(L"Missing '" + path + L"' entry in config file\n");

    return result;
}

void Server::launch()
{
    int32 iThread = 0;

    _serverData.adminEngine.setConnectionsTimeOut(_connectionTimeOut);
    _serverData.adminEngine.open(_adminAddress, _adminPort);
    _serverData.adminEngine.launch();

    _threads[iThread++] = new Core::Thread(&HOOServer::Server::runAdminThread, *this);

    _threads[iThread++] = new Core::Thread(&HOOServer::PlayerMatcher::run, _playerMatcher);


    _serverData.userEngine.setConnectionsTimeOut(_connectionTimeOut);
    _serverData.userEngine.open(_clientAddress, _clientPort);
    _serverData.userEngine.launch();

    while(iThread<_clientNbThreads+2)
    {
        _threads[iThread++] = new Core::Thread(&HOOServer::Server::runClientThread, *this);
    }

    _serverData.updaterEngine.setConnectionsTimeOut(_connectionTimeOut);
    _serverData.updaterEngine.open(_updaterAddress, _updaterPort);
    _serverData.updaterEngine.launch();

    while(iThread<_clientNbThreads+_updaterNbThreads+2)
    {
        _threads[iThread++] = new Core::Thread(&HOOServer::Server::runUpdaterThread, *this);
    }

    while(!_serverData.isBeingKilled)
    {
        Core::Thread::sleep(500);
        
        {
            Core::ScopedLock lock(_serverData.shutDownMutex);
            if(_serverData.periodBeforeShutDown >= 0)
            {
                Core::Clock clock;
                Core::TimeValue timeValue = clock.universalTime();

                if((int32)((timeValue - _serverData.shutDownTime).totalSeconds()) > _serverData.periodBeforeShutDown)
                {
                    _serverData.isBeingKilled = true;
                }
            }
        }
    }

    {
        Core::ScopedCondLock lock(_serverData.conditionMutexUser);
        _serverData.conditionUser.notifyAll();
    }

    {
        Core::ScopedCondLock lock(_serverData.conditionMutexAdmin);
        _serverData.conditionAdmin.notifyAll();
    }

    for(int32 ii = 0; ii<_threads.size(); ++ii)
    {
        _threads[ii]->join();
    }
}

void Server::runClientThread()
{
    while(!_serverData.isBeingKilled)
    {
        int32 sessionId = -1;
        {
            Network::Message * message = NULL;

            Core::ScopedCondLock lock(_serverData.conditionMutexUser);
            while(!_serverData.isBeingKilled && !_serverData.userEngine.getMessage(message))
            {
                _serverData.conditionUser.wait(lock);
            }

            if(!_serverData.isBeingKilled)
            {
                sessionId = message->getSessionId();
                if(_userSessionManager.isIdSessionValid(sessionId))
                {
                    _userSessionManager.getSession(sessionId).pushMessage(message);
                }
                else
                {
                    delete message;
                    message = NULL;
                }
            }
        }

        if(!_serverData.isBeingKilled)
        {
            if(_userSessionManager.isIdSessionValid(sessionId))
            {
                // LOCK SESSION
                _userSessionManager.getSession(sessionId).getGeneralMutex().lock();

                Network::Message * message = _userSessionManager.getSession(sessionId).popMessage();
                
                try
                {
                    Network::EMessageType type = message->getMessageType();
                    switch(type)
                    {
                    case Network::NETWORK_OPEN_SESSION_MESSAGE:
                        _openUserSessionHandler.handleMessage(type, sessionId);
                        break;
                    case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                        _closeUserSessionHandler.handleMessage(type, sessionId);
                        break;
                    case Network::APPLICATION_MESSAGE:
                        {
                            if(!_userSessionManager.getSession(sessionId).isClosed())
                            {
                                if(message->getDataSize() >= sizeof(HOOUserProtocol::EUserRequestType))
                                {
                                    HOOUserProtocol::EUserRequestType userRequestType;
                                    message->readData(&userRequestType, sizeof(HOOUserProtocol::EUserRequestType));
                                    if(HOOUserProtocol::isValid(userRequestType))
                                    {
                                        _userMessageHandlers[userRequestType]->handleMessage(userRequestType, sessionId, message);
                                    }
                                    else
                                    {
                                        Core::String message;
                                        message << L"Unknwown user request type : " << (int32)userRequestType;
                                        throw DisconnectException(message, WRONG_PROTOCOL);
                                    }
                                }
                                else
                                {
                                    //WTF!!! Hack tentative?!?
                                    throw DisconnectException(L"Message too short to read the reaquest type.", WRONG_PROTOCOL);
                                }
                            }
                        }
                        break;
                    default:
                        {
                            Core::String message;
                            message << L"Unknwown message type : " << type;
                            //Unknown message type
                            throw DisconnectException(message, WRONG_PROTOCOL);
                        }
                        break;
                    }
                }
                catch(DisconnectException & disconnectException)
                {
                    try
                    {
                        switch(disconnectException.getReason())
                        {
                        case(MAXIMUM_CONNECTION_REACHED):
                            {
                                Network::Message messageMaxConnReached(Network::NETWORK_MAX_CONNECTION_REACHED, sessionId);
                                _serverData.userEngine.sendMessage(messageMaxConnReached);
                            }
                            break;
                        case WRONG_PROTOCOL:
                        case SESSION_NOT_OPENED:
                        case NOT_LOGGED_IN:
                            {
                                _userSessionManager.closeSession(sessionId);
                            }
                            break;
                        }                        
                    }catch(...)
                    {
                        int i = 0;
                        ++i;
                    }

                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.userEngine.sendMessage(messageCloseConnection);
                    
                }
                catch(Network::NetworkException &)
                {
                    try
                    {
                        _userSessionManager.closeSession(sessionId);
                    }catch(...)
                    {
                        int i = 0;
                        ++i;
                    }
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.userEngine.sendMessage(messageCloseConnection);
                }
                catch(Core::IOException &)
                {
                    try
                    {
                        _userSessionManager.closeSession(sessionId);
                    }catch(...)
                    {
                        int i = 0;
                        ++i;
                    }
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.userEngine.sendMessage(messageCloseConnection);
                }
                catch(Core::Exception &)
                {
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.userEngine.sendMessage(messageCloseConnection);
                }
                catch(...)
                {
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.userEngine.sendMessage(messageCloseConnection);
                }

                delete message;
                message = NULL;
                
                // UNLOCK SESSION
                _userSessionManager.getSession(sessionId).getGeneralMutex().unlock();
            }
            else
            {
                //WTF!!!
            }
        }
    }
}

void Server::runUpdaterThread()
{
    while(!_serverData.isBeingKilled)
    {
        int32 sessionId = -1;
        Network::Message * message = NULL;

        Core::ScopedCondLock lock(_serverData.conditionMutexUpdater);
        while(!_serverData.isBeingKilled && !_serverData.updaterEngine.getMessage(message))
        {
            _serverData.conditionUpdater.wait(lock);
        }

        if(!_serverData.isBeingKilled)
        {
            sessionId = message->getSessionId();
            if ((sessionId >= 0 && sessionId < Network::TOLERANT_SOCKET_MAX_CONNECTION) == false)
            {
                delete message;
                message = NULL;
            }
        }

        if(!_serverData.isBeingKilled && message != NULL)
        {
            try
            {
                Network::EMessageType type = message->getMessageType();
                switch(type)
                {
                case Network::NETWORK_OPEN_SESSION_MESSAGE:
                    break;
                case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                    break;
                case Network::APPLICATION_MESSAGE:
                    {
                        if(message->getDataSize() >= sizeof(HOOUpdaterProtocol::EUpdaterRequestType))
                        {
                            HOOUpdaterProtocol::EUpdaterRequestType updaterRequestType;
                            message->readData(&updaterRequestType, sizeof(HOOUpdaterProtocol::EUpdaterRequestType));
                            if(HOOUpdaterProtocol::isValid(updaterRequestType))
                            {
                                _updaterMessageHandlers[updaterRequestType]->handleMessage(updaterRequestType, sessionId, message);
                            }
                            else
                            {
                                Core::String message;
                                message << L"Unknwown user request type : " << (int32)updaterRequestType;
                                throw DisconnectException(message, WRONG_PROTOCOL);
                            }
                        }
                        else
                        {
                            //WTF!!! Hack tentative?!?
                            throw DisconnectException(L"Message too short to read the reaquest type.", WRONG_PROTOCOL);
                        }
                    }
                    break;
                
                default:
                    {
                        Core::String message;
                        message << L"Unknwown message type : " << type;
                        //Unknown message type
                        throw DisconnectException(message, WRONG_PROTOCOL);
                    }
                    break;
                }
            }
            catch(DisconnectException & disconnectException)
            {
                try
                {
                    switch(disconnectException.getReason())
                    {
                    case(MAXIMUM_CONNECTION_REACHED):
                        {
                            Network::Message messageMaxConnReached(Network::NETWORK_MAX_CONNECTION_REACHED, sessionId);
                            _serverData.userEngine.sendMessage(messageMaxConnReached);
                        }
                        break;
                    case WRONG_PROTOCOL:
                    case SESSION_NOT_OPENED:
                    case NOT_LOGGED_IN:
                        {
                            _userSessionManager.closeSession(sessionId);
                        }
                        break;
                    }                        
                }catch(...)
                {
                    int i = 0;
                    ++i;
                }

                Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                _serverData.userEngine.sendMessage(messageCloseConnection);
                
            }
            catch(Network::NetworkException &)
            {
                try
                {
                    _userSessionManager.closeSession(sessionId);
                }catch(...)
                {
                    int i = 0;
                    ++i;
                }
                Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                _serverData.userEngine.sendMessage(messageCloseConnection);
            }
            catch(Core::IOException &)
            {
                try
                {
                    _userSessionManager.closeSession(sessionId);
                }catch(...)
                {
                    int i = 0;
                    ++i;
                }
                Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                _serverData.userEngine.sendMessage(messageCloseConnection);
            }
            catch(Core::Exception &)
            {
                Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                _serverData.userEngine.sendMessage(messageCloseConnection);
            }
            catch(...)
            {
                Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                _serverData.userEngine.sendMessage(messageCloseConnection);
            }

            delete message;
            message = NULL;
        }
    }
}

void Server::runAdminThread()
{
    while(!_serverData.isBeingKilled)
    {
        int32 sessionId = -1;
        {
            Network::Message * message = NULL;

            Core::ScopedCondLock lock(_serverData.conditionMutexAdmin);
            while(!_serverData.isBeingKilled && !_serverData.adminEngine.getMessage(message))
            {
                _serverData.conditionAdmin.wait(lock);
            }
    
            if(!_serverData.isBeingKilled)
            {
                sessionId = message->getSessionId();
                if(_adminSessionManager.isIdSessionValid(sessionId))
                {
                    _adminSessionManager.getSession(sessionId).pushMessage(message);
                }
                else
                {
                    delete message;
                    message = NULL;
                }
            }
        }

        if(!_serverData.isBeingKilled)
        {
            if(_adminSessionManager.isIdSessionValid(sessionId))
            {
                // LOCK SESSION
                _adminSessionManager.getSession(sessionId).getGeneralMutex().lock();

                Network::Message * message = _adminSessionManager.getSession(sessionId).popMessage();
                
                try
                {
                    Network::EMessageType type = message->getMessageType();
                    switch(type)
                    {
                    case Network::NETWORK_OPEN_SESSION_MESSAGE:
                        _openAdminSessionHandler.handleMessage(type, sessionId);
                        break;
                    case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                        _closeAdminSessionHandler.handleMessage(type, sessionId);
                        break;
                    case Network::APPLICATION_MESSAGE:
                        {
                            if(!_adminSessionManager.getSession(sessionId).isClosed())
                            {
                                if(message->getDataSize() >= sizeof(HOOAdminProtocol::EAdminRequestType))
                                {
                                    HOOAdminProtocol::EAdminRequestType adminRequestType;
                                    message->readData(&adminRequestType, sizeof(HOOAdminProtocol::EAdminRequestType));
                                    if(HOOAdminProtocol::isValid(adminRequestType))
                                    {
                                        _adminMessageHandlers[adminRequestType]->handleMessage(adminRequestType, sessionId, message);
                                    }
                                    else
                                    {
                                        Core::String message;
                                        message << L"Unknwown admin request type : " << adminRequestType;
                                        throw DisconnectException(message, WRONG_PROTOCOL);
                                    }
                                }
                                else
                                {
                                    //WTF!!! Hack tentative?!?
                                    throw DisconnectException(L"Message too short to read the reaquest type.", WRONG_PROTOCOL);
                                }
                            }
                        }
                        break;
                    default:
                        {
                            Core::String message;
                            message << L"Unknwown message type : " << type;
                            //Unknown message type
                            throw DisconnectException(message, WRONG_PROTOCOL);
                        }
                        break;
                    }
                }
                catch(DisconnectException & disconnectException)
                {
                    try
                    {
                        switch(disconnectException.getReason())
                        {
                        case(MAXIMUM_CONNECTION_REACHED):
                            {
                                Network::Message messageMaxConnReached(Network::NETWORK_MAX_CONNECTION_REACHED, sessionId);
                                _serverData.adminEngine.sendMessage(messageMaxConnReached);
                            }
                            break;
                        case WRONG_PROTOCOL:
                            {
                                _userSessionManager.closeSession(sessionId);
                            }
                            break;
                        case SESSION_NOT_OPENED:
                        case NOT_LOGGED_IN:
                            break;
                        }                        
                    }catch(...)
                    {
                        int i = 0;
                        ++i;
                    }

                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.adminEngine.sendMessage(messageCloseConnection);
                }
                catch(Core::IOException &)
                {
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.adminEngine.sendMessage(messageCloseConnection);
                }
                catch(Core::Exception &)
                {
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.adminEngine.sendMessage(messageCloseConnection);
                }
                catch(...)
                {
                    Network::Message messageCloseConnection(Network::NETWORK_CLOSE_SESSION_MESSAGE, sessionId);
                    _serverData.adminEngine.sendMessage(messageCloseConnection);
                }

                delete message;
                message = NULL;
                
                // UNLOCK SESSION
                _adminSessionManager.getSession(sessionId).getGeneralMutex().unlock();
            }
            else
            {
                //WTF!!!
            }
        }
    }
}


}//namespace HOOServer
