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
#ifndef TVNETWORK_BENCHNETWORKMESSAGE_H
#define TVNETWORK_BENCHNETWORKMESSAGE_H

#include <Core/Standard.h>
#include <Core/Enum.h>
#include <Core/IO/Streams.h>

LM_ENUM_3(EBenchNetworkMessage, MESSAGE_UNICAST,
                                MESSAGE_BROADCAST,
                                MESSAGE_STOP_BENCH)

class BenchNetworkMessage : public Core::Serializable
{
public:
    BenchNetworkMessage(EBenchNetworkMessage networkMessageType, String networkMessage)
    :   _networkMessageType(networkMessageType),
        _networkMessage(networkMessage)
    {}

    BenchNetworkMessage()
    :   _networkMessageType(MESSAGE_UNICAST),
        _networkMessage(L"")
    {}

    virtual ~BenchNetworkMessage(){}

    virtual void read(Core::InputStream & inputStream)
    {
        inputStream.readData(&_networkMessageType, sizeof(EBenchNetworkMessage));
        _networkMessage = inputStream.readString();
    }
    virtual void write(Core::OutputStream & outputStream) const
    {
        outputStream.write(_networkMessageType);
        outputStream.write(_networkMessage);
    }

    EBenchNetworkMessage    _networkMessageType;
    String                  _networkMessage;
};

#endif /*TVNETWORK_BENCHNETWORKMESSAGE_H*/