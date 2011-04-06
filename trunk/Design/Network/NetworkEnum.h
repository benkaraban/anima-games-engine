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
#ifndef NETWORK_NETWORKENUM_H_
#define NETWORK_NETWORKENUM_H_

#include <Core/Enum.h>
#include <Core/Standard.h>

namespace Network
{

LM_ENUM_7(ENetworkMessageType,  NETWORK_PING,
                                NETWORK_PONG,
                                NETWORK_MAX_CONNECTION_REACHED,
                                NETWORK_IP_BLACKLISTED_MESSAGE,
                                NETWORK_IP_TEMPORARY_BLACKLISTED_MESSAGE,
                                NETWORK_CLOSE_SESSION_MESSAGE,
                                APPLICATION_MESSAGE)

LM_ENUM_3(ENetworkIPStatus, NETWORK_IP_AUTHORISED,
                            NETWORK_IP_BLACKLISTED,
                            NETWORK_IP_TEMPORARY_BLACKLISTED)

LM_ENUM_3(ENetworkConnectionPrivilege,  NETWORK_USER_CONNECTION,
                                        NETWORK_ADMIN_CONNECTION,
                                        NETWORK_MODERATOR_CONNECTION)

} // namespace Network

#endif /*NETWORK_NETWORKENUM_H_*/
