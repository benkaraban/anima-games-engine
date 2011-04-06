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
#include "Client.h"

#include <Core/RT/Thread.h>

#include <Network/Message.h>
#include <Network/NetworkException.h>

#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/UserLogin.h>
#include <HOOUserProtocol/UserLogout.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>
#include <HOOUserProtocol/GameMessage/ChatMessage.h>

#include <HOOClientStub/ServerAnswerHandler/LoginAnswerHandler.h>
#include <HOOClientStub/ServerAnswerHandler/CreateAccountAnswerHandler.h>
#include <HOOClientStub/ServerAnswerHandler/LaunchMatchHandler/QuickMatchAnswerHandler.h>
#include <HOOClientStub/ServerAnswerHandler/LaunchMatchHandler/LaunchMatchAnswerHandler.h>
#include <HOOClientStub/ServerAnswerHandler/GameMessageHandler/ChatMessageAnswerHandler.h>

#include <limits>
#include <iostream>

namespace HOOClient
{

Client::Client()
:   _networkEngine(&_conditionVar, &_conditionMutex),
    _sessionId(-1),
    _terminateProcess(false)
{
    for(int32 ii=0; ii<HOOUserProtocol::EUserAnswerType_COUNT; ++ii)
        _answerHandlers[ii] = NULL;

    _answerHandlers[HOOUserProtocol::USER_CONNECT_ANSWER]               = new LoginAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_LOGIN_ANSWER]                 = new LoginAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE_ANSWER] = new LoginAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_CREATE_ACCOUNT_ANSWER]        = new CreateAccountAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_QUICK_MATCH_ANSWER]           = new QuickMatchAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_CANCEL_QUICK_MATCH_ANSWER]    = new QuickMatchAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_LAUNCH_MATCH_ANSWER]          = new LaunchMatchAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_LOADING_ANSWER]               = new LaunchMatchAnswerHandler();
    _answerHandlers[HOOUserProtocol::USER_CHAT_MESSAGE_ANSWER]          = new ChatMessageAnswerHandler();

    for(int32 ii = 0; ii < HOOUserProtocol::EUserAnswerType_COUNT; ++ii)
    {
        if(_answerHandlers[ii] == NULL)
        {
            Core::String message;
            message << L"Handler for answer message type "<< HOOUserProtocol::toString((HOOUserProtocol::EUserAnswerType)ii) << L" is not set.";
            throw Core::Exception(message);
        }
    }
}

Client::~Client()
{
    for(int32 ii=0; ii<HOOUserProtocol::EUserAnswerType_COUNT; ++ii)
    {
        delete _answerHandlers[ii];
        _answerHandlers[ii] = NULL;
    }
}
    
void Client::launch(const Core::String & serverAddress, int32 port)
{
    _sessionId = _networkEngine.connect(serverAddress, port);
    if( _sessionId != -1)
    {
        _networkEngine.launch();

        process();
    }
    else
    {
        std::cout<<"Connection failed."<<std::endl;
    }
}

void Client::process()
{
    do
    {
        displayActions();

        takeUserInput();

        Core::Thread::sleep(100);

        if(!_terminateProcess)
            processIncomingMessage();
    

    }while(!_terminateProcess);
}

void Client::displayActions()
{
    std::wcout<<L"-------- Actions --------"<<std::endl;
    for(int32 iActions=0; iActions<HOOUserProtocol::EUserRequestType_COUNT; ++iActions)
    {
        std::wcout<<iActions<<L") "<<HOOUserProtocol::toString((HOOUserProtocol::EUserRequestType)iActions)<<std::endl;
    }
    std::wcout<<L"-1) Pour quitter"<<std::endl;
}

void Client::takeUserInput()
{
    int32 choice = -1;
    enterInteger(L"Choice", choice);

    if(choice == -1)
    {
        _terminateProcess = true;
    }
    else
    {
        processUserInput(choice);
    }
}

void Client::processUserInput(int32 choice)
{
    HOOUserProtocol::EUserRequestType requestType = HOOUserProtocol::EUserRequestType(choice);

    switch(requestType)
    {
    case HOOUserProtocol::USER_CONNECT:
        userConnect();
        break;
    case HOOUserProtocol::USER_LOGIN:
        userLogin();
        break;
    case HOOUserProtocol::USER_LOGOUT:
        userLogout();
        break;
    case HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE:
        userCheckLoginAvailable();
        break;
    case HOOUserProtocol::USER_CREATE_ACCOUNT:
        userCreateAccount();
        break;
    case HOOUserProtocol::USER_QUICK_MATCH:
        userQuickMatch();
        break;
    case HOOUserProtocol::USER_CANCEL_QUICK_MATCH:
        userCancelQuickMatch();
        break;
    case HOOUserProtocol::USER_LAUNCH_MATCH:
        userLaunchMatch();
        break;
    case HOOUserProtocol::USER_LOADING:
        userLoading();
        break;
    case HOOUserProtocol::USER_CHAT_MESSAGE:
        userChatMessage();
        break;
    default:
        std::wcout<<L"Unknown request : "<<choice<<std::endl;
        break;
    }
}

void Client::processIncomingMessage()
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
                _terminateProcess = true;
                std::wcout<<"The network connection was remotely closed!"<<std::endl;
                break;
            case Network::APPLICATION_MESSAGE:
                {
                    if(incomingMessage->getDataSize() >= sizeof(HOOUserProtocol::EUserAnswerType))
                    {
                        HOOUserProtocol::EUserAnswerType userAnswerType;
                        incomingMessage->readData(&userAnswerType, sizeof(HOOUserProtocol::EUserAnswerType));

                        if(HOOUserProtocol::isValid(userAnswerType))
                        {
                            _answerHandlers[userAnswerType]->handleMessage(userAnswerType, incomingMessage);
                        }
                    }
                }
                break;
            default:
                {
                    std::wcout << L"Unknwown message type : " << int32(messageType) <<std::endl;
                }
                break;
            }

            delete incomingMessage;
            incomingMessage = NULL;
        }

    }while(messageToProcess);
}

void Client::userConnect()
{
    HOOUserProtocol::UserConnect userConnect;
    sendMessage(userConnect);
}

void Client::userLogin()
{
    String login;
    String password;
    enterString(L"Login", login);
    enterString(L"Password", password);

    HOOUserProtocol::UserLogin userLogin(login, password);
    sendMessage(userLogin);
}

void Client::userLogout()
{
    HOOUserProtocol::UserLogout userLogout;
    sendMessage(userLogout);
}

void Client::userCheckLoginAvailable()
{
    String login;
    enterString(L"Login", login);

    HOOUserProtocol::CheckLoginAvailable loginAvailable(login);
    sendMessage(loginAvailable);
}

void Client::userCreateAccount()
{
    String login;
    String password;
    String mail;
    int32 character;
    bool newsletter;
    enterString(L"Login", login);
    enterString(L"Password", password);
    enterString(L"Mail", mail);
    enterInteger(L"Character", character);
    enterBool(L"Newsletter", newsletter);

    HOOUserProtocol::CreateUserAccount userAccount(login, password, mail, newsletter);
    sendMessage(userAccount);
}

void Client::userQuickMatch()
{
    HOOUserProtocol::QuickMatch quickMatch;

    sendMessage(quickMatch);
}

void Client::userCancelQuickMatch()
{
    HOOUserProtocol::CancelQuickMatch cancelQuickMatch;

    sendMessage(cancelQuickMatch);
}

void Client::userLaunchMatch()
{
    HOOUserProtocol::LaunchMatch launchMatch;

    sendMessage(launchMatch);
}

void Client::userLoading()
{
    std::wcout<<L"Enter Loading %"<<std::endl;
    std::wcout<<L"0)   0%"<<std::endl;
    std::wcout<<L"1)  25%"<<std::endl;
    std::wcout<<L"2)  50%"<<std::endl;
    std::wcout<<L"3)  75%"<<std::endl;
    std::wcout<<L"4) 100%"<<std::endl;

    int32 choice = -1;
    do
    {
        enterInteger(L"Choice", choice);
    }while(choice < 0 && choice > 4);

    HOOUserProtocol::UserLoading loading((HOOUserProtocol::EUserLoadingType)choice);

    sendMessage(loading);
}

void Client::userChatMessage()
{
    String message;
    enterString(L"Message", message);

    HOOUserProtocol::ChatMessage chatMessage(message);
    sendMessage(chatMessage);
}

void Client::enterInteger(const String & label, int32 & choice)
{
    std::wcout<<L"Enter "<<label.c_str() << L" : "<< std::endl;
    while(!(std::cin>>choice))
    {
        std::wcout<<L"Invalid input"<<std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
    }
}

void Client::enterBool(const String & label, bool & result)
{
    int32 choice = 0;

    std::wcout<<L"Enter "<<label.c_str() << L" : "<< std::endl;
    while(!(std::cin>>choice))
    {
        std::wcout<<L"Invalid input"<<std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
    }

    result = (choice != 0);
}

void Client::enterString(const String & label, String & choice)
{
    std::wcout<<L"Enter "<<label.c_str()<< L" : "<<std::endl;
    while(!(std::wcin>>choice))
    {
        std::wcout<<L"Invalid input"<<std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits <std::streamsize>::max(), '\n');
    }
}

void Client::sendMessage(HOOUserProtocol::IUserRequest & message)
{
    Network::Message messageToSend(Network::APPLICATION_MESSAGE, message, _sessionId);
    {

        try
        {
            _networkEngine.sendMessage(messageToSend);
        }catch(Network::NetworkException & ne)
        {
            _terminateProcess = true;
            std::wcout<<"The network connection was remotely closed! "<< ne.getMessage() <<std::endl;
        }
    }
}

}
