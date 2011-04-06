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
#include "LoginAnswerHandler.h"

#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/UserLogin.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>

#include <iostream>

namespace HOOClient
{

LoginAnswerHandler::LoginAnswerHandler()
{}

LoginAnswerHandler::~LoginAnswerHandler()
{}

void LoginAnswerHandler::handleMessage(HOOUserProtocol::EUserAnswerType ansType, Network::Message * answer)
{
    if(ansType == HOOUserProtocol::USER_CONNECT_ANSWER)
    {
        handleConnectAnswer(answer);    
    }
    else if(ansType == HOOUserProtocol::USER_LOGIN_ANSWER)
    {
        handleLoginAnswer(answer);
    }
    else if(ansType == HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE_ANSWER)
    {
        handleCheckLoginAvailableAnswer(answer);
    }
    else
    {
        Core::String message;
        message << L"LoginAnswerHandler not able to process answer of type : ("<< (int32)ansType << L") - " << HOOUserProtocol::toString(ansType);
        throw Core::Exception(message);

    }
}

void LoginAnswerHandler::handleConnectAnswer(Network::Message * answer)
{
    HOOUserProtocol::UserConnectAnswer connectAnswer;

    answer->getMessage(connectAnswer);

    HOOUserProtocol::EUserConnectAnswerType answerType = connectAnswer.getConnectAnswerType();

    std::wcout<<L"Connect : "<<HOOUserProtocol::toString(answerType) << std::endl;
}

void LoginAnswerHandler::handleLoginAnswer(Network::Message * answer)
{
    HOOUserProtocol::UserLoginAnswer loginAnswer;

    answer->getMessage(loginAnswer);

    HOOUserProtocol::EUserLoginAnswerType answerType = loginAnswer.getLoginAnswerType();

    std::wcout<<L"Login : "<<HOOUserProtocol::toString(answerType) << std::endl;
}

void LoginAnswerHandler::handleCheckLoginAvailableAnswer(Network::Message * answer)
{
    HOOUserProtocol::CheckLoginAvailableAnswer loginAvailableAnswer;

    answer->getMessage(loginAvailableAnswer);

    HOOUserProtocol::ECheckLoginAvailableAnswerType answerType = loginAvailableAnswer.getLoginAvailability();
    Core::String login = loginAvailableAnswer.getLogin();

    std::wcout<<L"Login : "<<login.c_str()<<L" "<<HOOUserProtocol::toString(answerType) << std::endl;
}

}//namespace HOOClient
