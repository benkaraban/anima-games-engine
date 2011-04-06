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
#include <Core/IO/FileStream.h>
#include <Core/IO/TextStream.h>
#include <Core/IO/PipeStream.h>
#include <Core/IO/MemoryStream.h>
#include <Core/RT/Thread.h>
#include <Core/Math/MathIO.h>

class TestSerializable : public Core::Serializable, public Core::Object
{
public:
    TestSerializable() :
        _integer8(0),  _uInteger8(0),
        _integer16(0), _uInteger16(0),
        _integer32(0), _uInteger32(0),
        _integer64(0), _uInteger64(0)
    {}

    virtual void read(Core::InputStream & inputStream)
    {
        inputStream.read(_integer8);
        inputStream.read(_uInteger8);
        inputStream.read(_integer16);
        inputStream.read(_uInteger16);
        inputStream.read(_integer32);
        inputStream.read(_uInteger32);
        inputStream.read(_integer64);
        inputStream.read(_uInteger64);
    }
    virtual void read2(Core::InputStream & inputStream)
    {
        _integer8   = inputStream.readInt8();
        _uInteger8  = inputStream.readUInt8();
        _integer16  = inputStream.readInt16();
        _uInteger16 = inputStream.readUInt16();
        _integer32  = inputStream.readInt32();
        _uInteger32 = inputStream.readUInt32();
        _integer64  = inputStream.readInt64();
        _uInteger64 = inputStream.readUInt64();
    }
    virtual void write(Core::OutputStream & outputStream) const
    {
        outputStream.write(_integer8);
        outputStream.write(_uInteger8);
        outputStream.write(_integer16);
        outputStream.write(_uInteger16);
        outputStream.write(_integer32);
        outputStream.write(_uInteger32);
        outputStream.write(_integer64);
        outputStream.write(_uInteger64);
    }

    bool operator != (const TestSerializable & test) const
    {
        return  _integer8   != test._integer8 ||
                _uInteger8  != test._uInteger8 ||
                _integer16  != test._integer16 ||
                _uInteger16 != test._uInteger16 ||
                _integer32  != test._integer32 ||
                _uInteger32 != test._uInteger32 ||
                _integer64  != test._integer64 ||
                _uInteger64 != test._uInteger64;
    }

    int8    _integer8;
    uint8   _uInteger8;
    int16   _integer16;
    uint16  _uInteger16;
    int32   _integer32;
    uint32  _uInteger32;
    int64   _integer64;
    uint64  _uInteger64;
};

TestSerializable testData;
static Ptr<Core::PipeStream> spPipeStream(NULL);

void writePipeStream()
{
    for(int32 ii=0; ii < 10; ii++)
    {
        testData.write(*spPipeStream);
    }

    for(int32 ii=0; ii < 10; ii++)
    {
        byte value = ii;
        spPipeStream->writeData(&value, sizeof(value));
        Core::Thread::sleep(1);
    }

    spPipeStream->close();
}

void  readPipeStream()
{
    for(int32 ii=0; ii < 10; ii++)
    {
        TestSerializable readData;
        readData.read(*spPipeStream);
        UTEST_EQU(readData, testData);
    }

    int32 value = 0;

    while(!spPipeStream->eof())
    {
        byte buffer[10];
        int32 readBytes = spPipeStream->readUpTo(buffer, 10);

        for(int32 ii=0; ii < readBytes; ii++)
        {
            UTEST_EQU(buffer[ii], value);
            value ++;
        }

        Core::Thread::sleep(1);
    }
}

void testStreams()
{
    UTEST_PACKAGE("Streams");

    testData._integer8 = -127;
    testData._uInteger8 = 255;
    testData._integer16 = -32767;
    testData._uInteger16 = 65535;
    testData._integer32 = -2147483647;
    testData._uInteger32 = 4294967295u;
    testData._integer64 = 0x12345678;
    testData._integer64 = (testData._integer64 << 32) + 0x9ABCDEF0;
    testData._uInteger64 = 0xFDECBA98;
    testData._uInteger64 = (testData._uInteger64 << 32) + 0x76543210;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"FileStream");

    {
        Core::FileOutputStream  outStream;

        UTEST_EQU(outStream.isOpened(), false);

        outStream.open(L"Data/TUData/Tmp/test.dat", Core::FileOutputStream::FILE_TRUNCATE);
        bool val = outStream.isOpened();
        UTEST_EQU(val, true);

        testData.write(outStream);

        outStream.close();
        UTEST_EQU(outStream.isOpened(), false);

        Core::FileInputStream  inStream;
        UTEST_EQU(outStream.isOpened(), false);
        inStream.open(L"Data/TUData/Tmp/test.dat");
        UTEST_EQU(inStream.isOpened(), true);

        TestSerializable readData;
        TestSerializable readData2;

        UTEST_EQU(inStream.eof(), false);
        readData.read(inStream);
        UTEST_EQU(testData, readData);

        UTEST_CATCH_EXCEPTION(Core::IOException, readData2.read(inStream));

        inStream.rewind();

        byte data[20];
        UTEST_EQU(inStream.readUpTo(data, 20), 20);
        UTEST_EQU(inStream.readUpTo(data, 20), 10);
        UTEST_CATCH_EXCEPTION(Core::IOException, inStream.readUpTo(data, 20));

        UTEST_EQU(inStream.eof(), true);
    }

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"TextStream");

    {
        Ptr<Core::FileOutputStream>  pFileOutStream(new Core::FileOutputStream(L"Data/TUData/Tmp/test.txt", Core::FileOutputStream::FILE_TRUNCATE));
        Core::TextOutputStream textOut(pFileOutStream, true);

        textOut.putLine(L"AAA");
        textOut.putLine(L"AB\nCD");
        textOut.putLine(L"Hello les amis je suis le capitaine Iglo !");
        textOut.putLine(L"Je suis allé loin pour pêcher des colins pour Iglo !");
        textOut.put(L"ABC");
        textOut.put(L"DEF");
        textOut.endLine();
        textOut.put(L"The End");
    }

    {
        Ptr<Core::FileInputStream>  pFileInStream(new Core::FileInputStream(L"Data/TUData/Tmp/test.txt"));
        Core::TextInputStream textIn(pFileInStream);

        UTEST_EQU(textIn.eof(), false);
        UTEST_EQU(textIn.getLine(), L"AAA");
        UTEST_EQU(textIn.getLine(), L"AB");
        UTEST_EQU(textIn.getLine(), L"CD");
        UTEST_EQU(textIn.getLine(), L"Hello les amis je suis le capitaine Iglo !");
        UTEST_EQU(textIn.getLine(), L"Je suis allé loin pour pêcher des colins pour Iglo !");
        UTEST_EQU(textIn.getLine(), L"ABCDEF");
        UTEST_EQU(textIn.eof(), false);
        UTEST_EQU(textIn.getLine(), L"The End");
        UTEST_EQU(textIn.eof(), true);
    }

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"MemoryStream");

    {
        byte bufferDoNotDelete[100];
        Core::MemoryInputStream  memStream(bufferDoNotDelete, 100);
    }

    const int BUFFER_SIZE = 20;
    byte * pData = new byte[BUFFER_SIZE];

    for(int32 ii=0; ii < BUFFER_SIZE; ii++)
        pData[ii] = ii;

    Core::MemoryInputStream  memStream(pData, BUFFER_SIZE, true);

    byte readData[2 * BUFFER_SIZE];

    memStream.readData(readData, 10);

    for(int32 ii=0; ii < 10; ii++)
        UTEST_EQU(readData[ii], ii);

    UTEST_EQU(memStream.eof(), false);

    memStream.setPosition(-10, Core::SEEK_ORIGIN_CUR);
    memStream.readData(readData, 10);

    for(int32 ii=0; ii < 10; ii++)
        UTEST_EQU(readData[ii], ii);

    memStream.setPosition(5, Core::SEEK_ORIGIN_SET);
    UTEST_EQU(memStream.readUpTo(readData, 10), 10);

    for(int32 ii=0; ii < 10; ii++)
        UTEST_EQU(readData[ii], 5 + ii);

    memStream.setPosition(-15, Core::SEEK_ORIGIN_END);
    memStream.readData(readData, 10);

    for(int32 ii=0; ii < 10; ii++)
        UTEST_EQU(readData[ii], 5 + ii);

    UTEST_EQU(memStream.eof(), false);
    UTEST_EQU(memStream.readUpTo(readData, 10), 5);
    UTEST_EQU(memStream.eof(), true);

    for(int32 ii=0; ii < 5; ii++)
        UTEST_EQU(readData[ii], 15 + ii);

    UTEST_CATCH_EXCEPTION(Core::IOException, memStream.readUpTo(readData, 2 * BUFFER_SIZE));

    memStream.rewind();
    UTEST_EQU(memStream.eof(), false);
    memStream.readData(readData, 10);

    for(int32 ii=0; ii < 10; ii++)
        UTEST_EQU(readData[ii], ii);

    UTEST_CATCH_EXCEPTION(Core::IOException, memStream.readData(readData, 2 * BUFFER_SIZE));

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"BufferStream");
    {
        byte byteBuffer[10];
        Core::BufferStream  buffer;

        UTEST_EQU(buffer.eof(), true);
        UTEST_EQU(buffer.getDataSize(), 0);

        UTEST_EQU(buffer.readUpTo(byteBuffer, 10), 0);
        UTEST_CATCH_EXCEPTION(Core::IOException, buffer.readData(byteBuffer, 1));

        testData.write(buffer);
        testData.write(buffer);

        TestSerializable readData;
        readData.read(buffer);
        UTEST_EQU(testData, readData);
        readData.read(buffer);
        UTEST_EQU(testData, readData);
        UTEST_CATCH_EXCEPTION(Core::IOException, readData.read(buffer));

        buffer.writeData(byteBuffer, 10);
        UTEST_EQU(buffer.getDataSize(), 10);
        UTEST_EQU(buffer.readUpTo(byteBuffer, 7), 7);
        UTEST_EQU(buffer.getDataSize(), 3);
        UTEST_EQU(buffer.readUpTo(byteBuffer, 7), 3);
        UTEST_EQU(buffer.getDataSize(), 0);
        UTEST_EQU(buffer.readUpTo(byteBuffer, 7), 0);
        UTEST_EQU(buffer.eof(), true);

        testData.write(buffer);
        {
            Core::List<byte> data;
            buffer.getDataCopy(data);
            Core::MemoryInputStream  memStream(data.data(), buffer.getDataSize());

            TestSerializable readData;
            readData.read(memStream);
            UTEST_EQU(testData, readData);
        }

        readData.read(buffer);
        UTEST_EQU(testData, readData);

        UTEST_EQU(buffer.eof(), true);
        buffer.writeData(byteBuffer, 10);
        UTEST_EQU(buffer.getDataSize(), 10);

        buffer.close();
        UTEST_EQU(buffer.eof(), true);
        UTEST_EQU(buffer.getDataSize(), 0);

        UTEST_CATCH_EXCEPTION(Core::IOException, buffer.readData(byteBuffer, 1));
    }
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"String streaming");
    {
        Core::BufferStream  buffer;
        String string1(L"Il était ambigüe à l'extrême : 平仮名指事文字");
        buffer.write(string1);
        String string2;
        buffer.read(string2);
        UTEST_EQU(string1, string2);

        buffer.write(string1);
        String string3 = buffer.readString();
        UTEST_EQU(string1, string3);
    }
    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"PipeStream");

    spPipeStream = Ptr<Core::PipeStream>(new Core::PipeStream());

    Core::Thread  threadRead(&readPipeStream);
    Core::Thread  threadWrite(&writePipeStream);

    threadRead.join();
    threadWrite.join();

    UTEST_END_TEST;

    //-------------------------------------------------------------------------
    UTEST_START_TEST(L"Math IO");

    Core::BufferStream buffer;
    Core::Vector3f vec3fa(3.1415f, 0.1235456f, 9876543.0f);
    Core::Vector3f vec3fb;
    Core::write(buffer, vec3fa);
    Core::read(buffer, vec3fb);
    UTEST_EQU(vec3fa, vec3fb);

    Core::Vector4f vec4fa(3.1415f, 0.1235456f, 9876543.0f, 85245.23f);
    Core::Vector4f vec4fb;
    Core::write(buffer, vec4fa);
    Core::read(buffer, vec4fb);
    UTEST_EQU(vec4fa, vec4fb);

    Core::Vector4f vec4da(3.1415f, 0.1235456f, 9876543.0f, 85245.23f);
    Core::Vector4f vec4db;
    Core::write(buffer, vec4da);
    Core::read(buffer, vec4db);
    UTEST_EQU(vec4da, vec4db);

    Core::Quaternionf quatfa(1.00001f, 2.00002f, 3.00003f, 4.00004f);
    Core::Quaternionf quatfb;
    Core::write(buffer, quatfa);
    Core::read(buffer, quatfb);
    UTEST_EQU(quatfa, quatfb);

    UTEST_END_TEST;
}
