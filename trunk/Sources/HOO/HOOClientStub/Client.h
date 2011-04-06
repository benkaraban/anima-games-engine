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
#ifndef HOOCLIENT_CLIENT_H_
#define HOOCLIENT_CLIENT_H_

#include <Core/Standard.h>

#include <Network/NetworkEngine.h>

#include <HOOUserProtocol/IUserMessage.h>

#include <HOOClientStub/ServerAnswerHandler/IAnswerHandler.h>

namespace HOOClient
{

class LM_API_HOC Client : public Core::Object
{
public:
    Client();
    virtual ~Client();
    
    void launch(const Core::String & serverAddress, int32 port);

    void displayActions();

    void process();

    void takeUserInput();
    void processUserInput(int32 choice);
    void processIncomingMessage();

private:
    void userConnect();
    void userLogin();
    void userLogout();
    void userCheckLoginAvailable();
    void userCreateAccount();
    void userQuickMatch();
    void userCancelQuickMatch();
    void userLaunchMatch();
    void userLoading();
    void userChatMessage();

private:
    void enterInteger(const String & label, int32 & choice);
    void enterString(const String & label, String & choice);
    void enterBool(const String & label, bool & result);

    void sendMessage(HOOUserProtocol::IUserRequest & message);

private:
    Core::ConditionMutex        _conditionMutex;
    Core::ConditionVar          _conditionVar;
    Network::NetworkEngine      _networkEngine;

    int32                       _sessionId;
    bool                        _terminateProcess;

    IAnswerHandler *            _answerHandlers[HOOUserProtocol::EUserAnswerType_COUNT];
};

}//namespace HOOClient

#endif/*HOOCLIENT_CLIENT_H_*/