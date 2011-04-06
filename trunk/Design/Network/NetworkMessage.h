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
#ifndef NETWORK_NETWORKMESSAGE_H_
#define NETWORK_NETWORKMESSAGE_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/IO/BufferStream.h>

#include "NetworkDefines.h"
#include "NetworkEnum.h"

namespace Network
{

class LM_API NetworkMessage : public Core::Object
{
public:
    NetworkMessage( ENetworkMessageType messageType,
                    ENetworkConnectionPrivilege connectionPrivilege,
                    const Core::Serializable & message,
                    int32 sessionId = -1);
    NetworkMessage( ENetworkMessageType messageType,
                    ENetworkConnectionPrivilege connectionPrivilege = NETWORK_USER_CONNECTION,
                    int32 sessionId = -1);
    NetworkMessage( const byte buffer[NETWORK_MESSAGE_MAX_LENGTH] );
    ~NetworkMessage();

public:
    inline ENetworkMessageType getMessageType() const { return _messageType; }
    inline ENetworkConnectionPrivilege getConnectionPrivilege() const { return _connectionPrivilege; }
    inline int32 getSessionId() const { return _sessionId; }
    inline void setSessionId( int32 sessionId) { _sessionId = sessionId; }
    inline int32 getMessageSize () const
    {
        return( sizeof(int32) +
                sizeof(ENetworkMessageType) +
                sizeof(ENetworkConnectionPrivilege) +
                _message.getDataSize() );

    }
    void getMessage(Core::Serializable & message);

    void serialize(byte buffer[NETWORK_MESSAGE_MAX_LENGTH]);

private:
    ENetworkMessageType         _messageType;
    ENetworkConnectionPrivilege _connectionPrivilege;
    int32                       _sessionId;
    Core::BufferStream          _message;
};

}// namespace Network

#endif /* NETWORK_NETWORKMESSAGE_H_ */
