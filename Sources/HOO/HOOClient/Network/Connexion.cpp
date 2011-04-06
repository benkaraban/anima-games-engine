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
#include <Network/NetworkException.h>

#include <HOOUserProtocol/UserLogin.h>
#include <HOOUserProtocol/UserLogout.h>
#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/CreateUserAccount.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>
#include <HOOUserProtocol/SendActivationCode.h>
#include <HOOUserProtocol/SelectCharacter.h>

#include <HOOUserProtocol/AdminMessage.h>
#include <HOOUserProtocol/MaintenanceMessage.h>

#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>
#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>

#include <HOOUserProtocol/GameMessage/ChatMessage.h>
#include <HOOUserProtocol/GameMessage/GameCommand.h>

#include <HOOClient/Network/Connexion.h>

using namespace HOOUserProtocol;

//-----------------------------------------------------------------------------
Connexion::Connexion() :
    _networkEngine(&_conditionVar, &_conditionMutex),
    _sessionId(-1),
    _state(CONNEXION_NOT_OPENED),
    _isAppConnected(false)
{
}
//-----------------------------------------------------------------------------
void Connexion::open(const String & address, int32 port)
{
    switch(_state)
    {
    case CONNEXION_IN_PROGRESS:
    case CONNEXION_OK:
        throw Core::Exception(L"Trying to reopen an opened connexion");
        break;
    default:
        LM_ASSERT(false);
    case CONNEXION_NOT_OPENED:
    case CONNEXION_FAILED_SERVER_FULL:
    case CONNEXION_ERROR:
    case CONNEXION_LOST:
        break;
    }

    _outgoingMessages.clear();
    _incomingMessages.clear();

    _sessionId = -1;
    _connexionAddress = address;
    _connexionPort = port;
    _connexionAttemptFinished = false;
    _state = CONNEXION_IN_PROGRESS;
    _isAppConnected = false;

    _pConnexionThread = Ptr<Core::Thread>(new Core::Thread(&Connexion::threadOpen, *this));
}
//-----------------------------------------------------------------------------
void Connexion::threadOpen()
{
    EConnexionState newState = CONNEXION_OK;
    int32 sessionId = _networkEngine.connect(_connexionAddress, _connexionPort);

    //if(sessionId != -1)
    //{
    //    UserConnect userConnect;
    //    _networkEngine.launch();
    //    _appContext.pConnexion->send(userConnect);

    //    _networkEngine.sendMessage(*_outgoingMessages.front());

    //    Network::Message message(Network::APPLICATION_MESSAGE, userConnect, sessionId);
    //    
    //    //try
    //    //{
    //    //    _networkEngine.sendMessage(messageToSend);
    //    //}
    //    //catch(Network::NetworkException & ne)
    //    //{
    //    //    _terminateProcess = true;
    //    //    std::wcout<<"The network connection was remotely closed! "<< ne.getMessage() <<std::endl;
    //    //}
    //}
    //else
    //{
    //    newState = CONNEXION_FAILED;
    //}

    _connexionMutex.lock();
    {
        _sessionId = sessionId;
        _connexionAttemptFinished = true;
        _newState = newState;
    }
    _connexionMutex.unlock();
}
//-----------------------------------------------------------------------------
void Connexion::connexionInProgress()
{
    _connexionMutex.lock();

    if(_connexionAttemptFinished)
    {
        if(_sessionId != -1)
        {
            _state = CONNEXION_OK;
            INF << L"Socket to server opened\n";
            _networkEngine.launch();
        }
        else
        {
            _state = CONNEXION_FAILED;
        }
    }

    _connexionMutex.unlock();

    if(_connexionAttemptFinished)
    {
        _pConnexionThread->join();
        _pConnexionThread = null;
    }
}
//-----------------------------------------------------------------------------
Ptr<HOOUserProtocol::IUserAnswer> Connexion::translateMessage(Network::Message * pMessage)
{
    Ptr<HOOUserProtocol::IUserAnswer> pResult;

    if(pMessage->getDataSize() >= sizeof(HOOUserProtocol::EUserAnswerType))
    {
        HOOUserProtocol::EUserAnswerType userAnswerType;
        pMessage->readData(&userAnswerType, sizeof(HOOUserProtocol::EUserAnswerType));

        if(!HOOUserProtocol::isValid(userAnswerType))
        {
            ERR << L"Incorrect server answer type : " << Core::toString(int32(userAnswerType)) << L"\n";;
            LM_ASSERT(false);
        }
        else
        {
            try
            {
                switch(userAnswerType)
                {
                case USER_CONNECT_ANSWER:                   pResult = Ptr<IUserAnswer>(new UserConnectAnswer()); break;
                case USER_LOGIN_ANSWER:                     pResult = Ptr<IUserAnswer>(new UserLoginAnswer()); break;
                case USER_CHECK_LOGIN_AVAILABLE_ANSWER:     pResult = Ptr<IUserAnswer>(new CheckLoginAvailableAnswer()); break;
                case USER_CREATE_ACCOUNT_ANSWER:            pResult = Ptr<IUserAnswer>(new CreateUserAccountAnswer()); break;
                case USER_SELECT_CHARACTER_ANSWER:          pResult = Ptr<IUserAnswer>(new SelectCharacterAnswer()); break;
                case USER_QUICK_MATCH_ANSWER:               pResult = Ptr<IUserAnswer>(new QuickMatchAnswer()); break;
                case USER_CANCEL_QUICK_MATCH_ANSWER:        pResult = Ptr<IUserAnswer>(new CancelQuickMatchAnswer()); break;
                case USER_LAUNCH_MATCH_ANSWER:              pResult = Ptr<IUserAnswer>(new LaunchMatchAnswer()); break;
                case USER_LOADING_ANSWER:                   pResult = Ptr<IUserAnswer>(new UserLoadingAnswer()); break;
                case USER_ADMIN_MESSAGE_ANSWER:             pResult = Ptr<IUserAnswer>(new AdminMessageAnswer()); break;
                case USER_MAINTENANCE_MESSAGE_ANSWER:       pResult = Ptr<IUserAnswer>(new MaintenanceMessageAnswer()); break;
                case USER_CHAT_MESSAGE_ANSWER:              pResult = Ptr<IUserAnswer>(new ChatMessageAnswer()); break;
                case USER_GAME_COMMAND_ANSWER:              pResult = Ptr<IUserAnswer>(new GameCommandAnswer()); break;
                case USER_SEND_ACTIVATION_CODE_ANSWER:      pResult = Ptr<IUserAnswer>(new SendActivationCodeAnswer()); break;
                case USER_LOGIN_WITH_ACTIVATION_ANSWER:     pResult = Ptr<IUserAnswer>(new UserLoginWithAcCodeAnswer()); break;
                default:
                    throw Core::Exception(L"unsupported answer type");
                }

                pMessage->getMessage(*pResult);
            }
            catch(Core::Exception & exception)
            {
                ERR << L"Incorrect server answer data : " << exception.getMessage() << L"\n";
                pResult = null;
                LM_ASSERT(false);
            }
        }
    }
    else
    {
        ERR << L"Incorrect server answer size : " << Core::toString(pMessage->getDataSize()) << L"\n";;
        LM_ASSERT(false);
    }

    return pResult;
}
//-----------------------------------------------------------------------------
void Connexion::setDisconnected(EConnexionState state)
{
    _state = state;
    _isAppConnected = false;
}
//-----------------------------------------------------------------------------
void Connexion::connexionOK()
{
    // Remove old pending messages
    {
        Core::DoubleQueue<MessageEntry> filteredMessages;
        Core::DoubleQueue<MessageEntry>::iterator iMessage = _incomingMessages.begin();

        while(iMessage != _incomingMessages.end())
        {
            Core::TimeValue time = Core::Clock::localTime();

            if((time - iMessage->time).totalSeconds() > 10)
                WAR << L"Message " << HOOUserProtocol::toString(iMessage->pMessage->getType()) << L" has been pending for more than 10 seconds. Removing it from queue.\n";
            else
                filteredMessages.push_back(*iMessage);

            ++ iMessage;
        }

        _incomingMessages.swap(filteredMessages);
    }

    // Incoming messages
    {
        Core::ScopedCondLock lock(_conditionMutex);
        Network::Message * pIncomingMessage = NULL;

        while(_networkEngine.getMessage(pIncomingMessage))
        {
            Network::EMessageType messageType = pIncomingMessage->getMessageType();

            if(messageType == Network::NETWORK_CLOSE_SESSION_MESSAGE)
            {
                INF << L"Connexion closed by server\n";
                setDisconnected(CONNEXION_LOST);
            }
            else if(messageType == Network::NETWORK_MAX_CONNECTION_REACHED)
            {
                INF << L"Connexion refused by full server\n";
                setDisconnected(CONNEXION_FAILED_SERVER_FULL);
            }
            else if(messageType == Network::NETWORK_OPEN_SESSION_MESSAGE)
            {
                LM_ASSERT(false);
            }
            else if(messageType == Network::APPLICATION_MESSAGE)
            {
                Ptr<HOOUserProtocol::IUserAnswer> pMessage = translateMessage(pIncomingMessage);

                if(pMessage != null)
                    _incomingMessages.push_back(MessageEntry(pMessage));

                delete pIncomingMessage;
            }
            else
            {
                ERR << L"Unknown low level network message type : " << Core::toString(int32(messageType)) << L"\n";
                LM_ASSERT(false);
            }
        }
    }

    // Outgoing messages
         
    try
    {
        while(!_outgoingMessages.empty() && _state == CONNEXION_OK)
        {
            _networkEngine.sendMessage(*_outgoingMessages.front());
            _outgoingMessages.pop_front();
        }
    }
    catch(Network::NetworkException & ne)
    {
        _state = CONNEXION_ERROR;
        INF << L"Connexion with server lost : " << ne.getMessage() << L"\n";
    }
}
//-----------------------------------------------------------------------------
void Connexion::processMessages()
{
    switch(_state)
    {
    case CONNEXION_NOT_OPENED:
    case CONNEXION_FAILED:
    case CONNEXION_FAILED_SERVER_FULL:
    case CONNEXION_ERROR:
    case CONNEXION_LOST:
        // Nothing to do
        break;
    case CONNEXION_IN_PROGRESS:
        connexionInProgress();
        break;
    case CONNEXION_OK:
        connexionOK();
        break;
    default:
        LM_ASSERT(false);
        break;
    }
}
//-----------------------------------------------------------------------------
void Connexion::release()
{
    if(_pConnexionThread != null)
    {
        _pConnexionThread->join();
        _pConnexionThread = null;
    }
}
//-----------------------------------------------------------------------------
void Connexion::close()
{
}
//-----------------------------------------------------------------------------
bool Connexion::hasPendingMessage(const HOOUserProtocol::EUserAnswerType * filter)
{
    Core::DoubleQueue<MessageEntry>::const_iterator iMessage = _incomingMessages.begin();

    while(iMessage != _incomingMessages.end())
    {
        int32 iFilter = 0;

        while(filter[iFilter] != HOOUserProtocol::NULL_REQUEST)
        {
            if(iMessage->pMessage->getType() == filter[iFilter])
                return true;

            iFilter ++;
        }

        ++ iMessage;
    }

    return false;
}
//-----------------------------------------------------------------------------
Ptr<HOOUserProtocol::IUserAnswer> Connexion::popMessage(const HOOUserProtocol::EUserAnswerType * filter)
{
    Core::DoubleQueue<MessageEntry>::iterator iMessage = _incomingMessages.begin();

    while(iMessage != _incomingMessages.end())
    {
        int32 iFilter = 0;

        while(filter[iFilter] != HOOUserProtocol::NULL_REQUEST)
        {
            if(iMessage->pMessage->getType() == filter[iFilter])
            {
                Ptr<HOOUserProtocol::IUserAnswer> pResult(iMessage->pMessage);
                _incomingMessages.erase(iMessage);

                return pResult;
            }

            iFilter ++;
        }

        ++ iMessage;
    }

    return null;
}
//-----------------------------------------------------------------------------
void Connexion::resetState()
{
    _state = CONNEXION_NOT_OPENED;
}
//-----------------------------------------------------------------------------
EConnexionState Connexion::getState() const
{
    return _state;
}
//-----------------------------------------------------------------------------
void Connexion::send(const Core::Serializable & msg)
{
    Ptr<Network::Message> pMessage(new Network::Message(Network::APPLICATION_MESSAGE, msg, _sessionId));
    _outgoingMessages.push_back(pMessage);
}
//-----------------------------------------------------------------------------
