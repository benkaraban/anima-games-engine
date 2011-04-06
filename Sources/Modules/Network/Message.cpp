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
#include "Message.h"
#include "NetworkException.h"

namespace Network
{

Message::Message(   EMessageType         messageType,
                    const Core::Serializable &  data,
                    int32                      sessionId)
:   _messageType(messageType),
    _sessionId(sessionId)
{
    data.write(_data);
}

Message::Message(   EMessageType         messageType,
                    int32                      sessionId)
:   _messageType(messageType),
    _sessionId(sessionId)
{
}


Message::Message( const byte buffer[NETWORK_MESSAGE_MAX_LENGTH] )
:   _sessionId(-1)
{
    int32 offset = 0;
    int32 messageSize = 0;
    ::memcpy( &messageSize, buffer + offset, sizeof(int32));
    offset += sizeof(int32);
    ::memcpy( &_messageType, buffer + offset, sizeof(EMessageType));
    offset += sizeof(EMessageType);
    messageSize -= offset;
    _data.writeData(buffer + offset, messageSize);
}

Message::~Message()
{}

void Message::readData(void * buffer, int32 size)
{
    _data.readData(buffer, size);
}

void Message::getMessage(Core::Serializable &  data)
{
    data.read(_data);
}

/*
 * L'ID de la session est un identifiant local. Il n'est pas propagé sur le réseau.
 * */
void Message::serialize(byte buffer[NETWORK_MESSAGE_MAX_LENGTH])
{
    if(getSize() > NETWORK_MESSAGE_MAX_LENGTH)
    {
        String message(L"Message too long to be serialized.");
        throw NetworkException(message);
    }
    int32 offset = 0;
    int32 messageSize = getSize();
    //Taille totale du message
    ::memcpy( buffer + offset, &messageSize, sizeof(int32));
    offset += sizeof(int32);
    //Type de message
    ::memcpy( buffer + offset, &_messageType, sizeof(EMessageType));
    offset += sizeof(EMessageType);
    //Message
    _data.readData( buffer + offset, _data.getDataSize() );
}

void Message::copy(Message & message)
{
    message._messageType = _messageType;
    message._sessionId   = _sessionId;

    byte data[NETWORK_MESSAGE_MAX_LENGTH];
    int32 dataSize = _data.getDataSize();
    _data.readData(data, dataSize);

    message._data.flush();
    message._data.writeData(data, dataSize);
}

}// namespace Network
