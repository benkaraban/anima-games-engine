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
#include <UTests.h>

#include <Network/Enum.h>
#include <Network/Message.h>
#include <Network/IPBlacklist.h>
#include <Network/Connection.h>
#include <Network/NetworkException.h>
#include <Core/RT/Thread.h>

class MyData : public Core::Serializable
{
public:
    MyData(int32 myData)
    :   _myData(myData)
    {}

    virtual ~MyData(){}

    virtual void read(Core::InputStream & inputStream)
    {
        _myData = inputStream.readInt32();
    }
    virtual void write(Core::OutputStream & outputStream) const
    {
        outputStream.write(_myData);
    }
    int32 _myData;
};

LM_ENUM_3(EBenchMessage, BENCH_NETWORK_MESSAGE_1,
                                BENCH_NETWORK_MESSAGE_2,
                                BENCH_NETWORK_MESSAGE_STOP_BENCH)

class MyData2 : public Core::Serializable
{
public:
    MyData2(EBenchMessage MessageType, String Message)
    :   _MessageType(MessageType),
        _Message(Message)
    {}

    MyData2()
    :   _MessageType(BENCH_NETWORK_MESSAGE_1),
        _Message(L"")
    {}

    virtual ~MyData2(){}

    virtual void read(Core::InputStream & inputStream)
    {
        inputStream.readData(&_MessageType, sizeof(EBenchMessage));
        _Message = inputStream.readString();
    }
    virtual void write(Core::OutputStream & outputStream) const
    {
        outputStream.write(_MessageType);
        outputStream.write(_Message);
    }

    EBenchMessage   _MessageType;
    String          _Message;
};


void testNetworkMessages()
{
    MyData myData(1337);
    MyData myData2(31337);
    Network::Message message(Network::APPLICATION_MESSAGE, myData, 1234);

    UTEST_START_TEST(L"Network Messages");

    UTEST_EQU(message.getMessageType(), Network::APPLICATION_MESSAGE);
    UTEST_EQU(message.getSessionId(), 1234);
    message.getMessage(myData2);
    UTEST_EQU(myData2._myData, 1337);

    byte buffer[Network::NETWORK_MESSAGE_MAX_LENGTH];
    MyData myData3(3133337);
    Network::Message message2(Network::APPLICATION_MESSAGE, myData3, 4321);
    message2.serialize(buffer);
    Network::Message message3(buffer);
    UTEST_EQU(message3.getMessageType(), Network::APPLICATION_MESSAGE);
    UTEST_EQU(message3.getSessionId(), -1);
    message3.getMessage(myData2);
    UTEST_EQU(myData2._myData, 3133337);
    message3.setSessionId(1887);
    UTEST_EQU(message3.getSessionId(), 1887);


    MyData2 myData4(BENCH_NETWORK_MESSAGE_2, L"This is only a test!!!");
    MyData2 myData5;

    Network::Message message4(Network::APPLICATION_MESSAGE, myData4, 1234);
    int32 message4Size = message4.getSize();
    message4.serialize(buffer);
    Network::Message message5(buffer);
    int32 message5Size = message5.getSize();
    message5.getMessage(myData5);
    UTEST_EQU(myData5._MessageType,  BENCH_NETWORK_MESSAGE_2);
    UTEST_EQU(myData5._Message,  L"This is only a test!!!");
    UTEST_EQU(message4Size, message5Size);

    UTEST_END_TEST;
}

void testIPBlackList()
{
    UTEST_START_TEST(L"Network IP Blacklist");
    Network::IPBlacklist IPBlacklist;
    Network::ENetworkIPStatus status;
    status = IPBlacklist.isClientBlacklisted(1);
    UTEST_EQU(status, Network::NETWORK_IP_AUTHORISED);
    Core::Thread::sleep(1000);
    status = IPBlacklist.isClientBlacklisted(1);
    UTEST_EQU(status, Network::NETWORK_IP_TEMPORARY_BLACKLISTED);
    Core::Thread::sleep(4000);
    status = IPBlacklist.isClientBlacklisted(1);
    UTEST_EQU(status, Network::NETWORK_IP_AUTHORISED);

    for( int32 i=0; i<97; i++)
    {
        status = IPBlacklist.isClientBlacklisted(1);
    }
    UTEST_EQU(status, Network::NETWORK_IP_BLACKLISTED);

    UTEST_END_TEST;
}

void testNetworkConnection()
{
    UTEST_START_TEST(L"Network Connection");

    Network::Connection networkConnection1;
    networkConnection1.setSocket(1234);
    networkConnection1.setSessionId(4321);
    UTEST_EQU(networkConnection1.getSocket(), 1234);
    UTEST_EQU(networkConnection1.getSessionId(), 4321);
    UTEST_EQU(networkConnection1.isClosed(), false);


    Network::Connection networkConnection2;
    networkConnection2.setSocket(INVALID_SOCKET); 
    networkConnection2.setSessionId(6547);
    UTEST_EQU(networkConnection2.isClosed(), true);

    Network::Message Message( Network::APPLICATION_MESSAGE);

    UTEST_CATCH_EXCEPTION(  Network::NetworkException,
                            networkConnection2.sendMessage(Message));

    UTEST_END_TEST;
}
