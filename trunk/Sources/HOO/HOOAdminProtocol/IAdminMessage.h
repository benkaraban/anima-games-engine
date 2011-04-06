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
#ifndef HOOADMINPROTOCOL_ADMINMESSAGE_H_
#define HOOADMINPROTOCOL_ADMINMESSAGE_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>

namespace HOOAdminProtocol
{

/**
 * cf. l'�quivalent dans HOOUserProtocol pour les explications
 */
const int32 SAFE_STRING_LENGTH  = 1024;
const int32 SAFE_LIST_SIZE      = 1024;

LM_ENUM_11(EAdminRequestType,    ADMIN_CONNECT,
                                ADMIN_LOGIN,
                                ADMIN_LOGOUT,
                                SHUTDOWN_SERVER,
                                CONNECTED_USERS_INFOS,
                                SERVER_STATS,
                                BAN_USERS,
                                UNBAN_USERS,
                                LOCK_USERS,
                                UNLOCK_USERS,
                                SEND_MSG_TO_USERS)
//client -> serveur
class LM_API_HAP IAdminRequest : public Core::Serializable
{
public:
    virtual ~IAdminRequest(){}

    virtual EAdminRequestType getType() const = 0;
};

LM_ENUM_8(EAdminAnswerType, ADMIN_CONNECT_ANSWER,
                            ADMIN_LOGIN_ANSWER,
                            CONNECTED_USERS_INFOS_ANSWER,
                            SERVER_STATS_ANSWER,
                            BAN_USERS_ANSWER,
                            UNBAN_USERS_ANSWER,
                            LOCK_USERS_ANSWER,
                            UNLOCK_USERS_ANSWER)

//serveur -> client
class LM_API_HAP IAdminAnswer : public Core::Serializable
{
public:
    virtual ~IAdminAnswer(){}

    virtual EAdminAnswerType getType() const = 0;
};

}//namespace HOOAdminProtocol

#endif/*HOOADMINPROTOCOL_ADMINMESSAGE_H_*/