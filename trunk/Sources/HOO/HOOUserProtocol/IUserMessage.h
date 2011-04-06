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
#ifndef HOOUserPROTOCOL_USERMESSAGE_H_
#define HOOUserPROTOCOL_USERMESSAGE_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>

namespace HOOUserProtocol
{

/**
 * Taille maximale "par défaut" d'une chaîne quand on faire un readSafe.
 * Ca ne veut pas dire que la chaîne sera acceptée pour autant (elle peut être trop
 * longue pour le login par exemple). C'est juste une protection pour éviter qu'un
 * message corrompu/élaboré par un hacker ne fasse exploser la consommation mémoire.
 */
const int32 SAFE_STRING_LENGTH  = 1024;
const int32 SAFE_LIST_SIZE      = 1024;

LM_ENUM_15(EUserRequestType,    NULL_REQUEST,
                                USER_CONNECT,
                                USER_LOGIN,
                                USER_LOGOUT,
                                USER_CHECK_LOGIN_AVAILABLE,
                                USER_CREATE_ACCOUNT,
                                USER_SELECT_CHARACTER,
                                USER_QUICK_MATCH,
                                USER_CANCEL_QUICK_MATCH,
                                USER_LAUNCH_MATCH,
                                USER_LOADING,
                                USER_CHAT_MESSAGE,
                                USER_GAME_COMMAND,
                                USER_SEND_ACTIVATION_CODE,
                                USER_LOGIN_WITH_ACTIVATION_CODE)

class LM_API_HUP IUserRequest : public Core::Serializable
{
public:
    virtual ~IUserRequest(){}

    virtual EUserRequestType getType() const = 0;
};

LM_ENUM_16(EUserAnswerType, NULL_ANSWER,
                            USER_CONNECT_ANSWER,
                            USER_LOGIN_ANSWER,
                            USER_CHECK_LOGIN_AVAILABLE_ANSWER,
                            USER_CREATE_ACCOUNT_ANSWER,
                            USER_SELECT_CHARACTER_ANSWER,
                            USER_QUICK_MATCH_ANSWER,
                            USER_CANCEL_QUICK_MATCH_ANSWER,
                            USER_LAUNCH_MATCH_ANSWER,
                            USER_LOADING_ANSWER,
                            USER_ADMIN_MESSAGE_ANSWER,
                            USER_MAINTENANCE_MESSAGE_ANSWER,
                            USER_CHAT_MESSAGE_ANSWER,
                            USER_GAME_COMMAND_ANSWER,
                            USER_SEND_ACTIVATION_CODE_ANSWER,
                            USER_LOGIN_WITH_ACTIVATION_ANSWER)

class LM_API_HUP IUserAnswer : public Core::Serializable
{
public:
    virtual ~IUserAnswer(){}

    virtual EUserAnswerType getType() const = 0;
};

}//namespace HOOUserProtocol

#endif/*HOOUserPROTOCOL_USERMESSAGE_H_*/