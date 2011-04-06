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
#include "LaunchMatchAnswerHandler.h"

#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>

#include <iostream>

namespace HOOClient
{

LaunchMatchAnswerHandler::LaunchMatchAnswerHandler()
{}

LaunchMatchAnswerHandler::~LaunchMatchAnswerHandler()
{}

void LaunchMatchAnswerHandler::handleMessage(HOOUserProtocol::EUserAnswerType ansType, Network::Message * answer)
{
    if(ansType == HOOUserProtocol::USER_LAUNCH_MATCH_ANSWER)
    {
        handleLaunchMatchAnswer(answer);    
    }
    else if(ansType == HOOUserProtocol::USER_LOADING_ANSWER)
    {
        handleLoadingAnswer(answer);
    }
    else
    {
        Core::String message;
        message << L"LaunchMatchAnswerHandler not able to process answer of type : ("<< (int32)ansType << L") - " << HOOUserProtocol::toString(ansType);
        throw Core::Exception(message);
    }
}

void LaunchMatchAnswerHandler::handleLaunchMatchAnswer(Network::Message * answer)
{
    HOOUserProtocol::LaunchMatchAnswer launchMatchAnswer;

    answer->getMessage(launchMatchAnswer);

    HOOUserProtocol::ELaunchMatchAnswerType answerType = launchMatchAnswer.getLaunchMatchAnswerType();

    std::wcout<<L"LaunchMatch : "<<HOOUserProtocol::toString(answerType) << std::endl;
}

void LaunchMatchAnswerHandler::handleLoadingAnswer(Network::Message * answer)
{
    HOOUserProtocol::UserLoadingAnswer userLoadingAnswer;

    answer->getMessage(userLoadingAnswer);

    HOOUserProtocol::EUserLoadingAnswerType answerType = userLoadingAnswer.getUserLoadingAnswerType();

    std::wcout<<L"Opponent Loading : "<<HOOUserProtocol::toString(answerType) << std::endl;
}


}//namespace HOOClient
