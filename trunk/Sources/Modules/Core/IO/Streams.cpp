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

#include "Streams.h"

namespace Core
{
//-----------------------------------------------------------------------------
byte InputStream::readByte()
{
    byte result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
bool InputStream::readBool()
{
    bool result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
int8 InputStream::readInt8()
{
    int8 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
uint8 InputStream::readUInt8()
{
    uint8 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
int16 InputStream::readInt16()
{
    int16 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
uint16 InputStream::readUInt16()
{
    uint16 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
int32 InputStream::readInt32()
{
    int32 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
uint32 InputStream::readUInt32()
{
    uint32 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
int64 InputStream::readInt64()
{
    int64 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
uint64 InputStream::readUInt64()
{
    uint64 result;
    readData(&result, sizeof(result));
    return result;
}
//-----------------------------------------------------------------------------
String InputStream::readString()
{
    String result;
    read(result);
    return result;
}
//-----------------------------------------------------------------------------
String InputStream::readStringSafe(int32 maxSize)
{
    String result;
    readSafe(result, maxSize);
    return result;
}
//-----------------------------------------------------------------------------
void InputStream::read(bool & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(int8 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(uint8 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(int16 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(uint16 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(int32 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(uint32 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(int64 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(uint64 & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(float & value)
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(double & value)    
{
    readData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void InputStream::read(String & value)    
{
    int32    size = readInt32();
    String8  string8;
    string8.resize(size);
    readData(const_cast<char*>(string8.data()), size);
    value = String(string8);
}
//-----------------------------------------------------------------------------
void InputStream::readSafe(String & value, int32 maxSize)
{
    int32   size = readInt32();
    if(size > maxSize)
    {
        String message;
        message << L"String size in stream is greater than expected MAX size. (read size : "
                <<size
                <<L", max size : "
                << maxSize<<L")";
        throw Core::IOException(message);
    }
    String8  string8;
    string8.resize(size);
    readData(const_cast<char*>(string8.data()), size);
    value = String(string8);
}
//-----------------------------------------------------------------------------
void InputStream::read(Serializable & value)    
{
    value.read(*this);
}
//-----------------------------------------------------------------------------
void OutputStream::write(bool value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(int8 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(uint8 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(int16 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(uint16 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(int32 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(uint32 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(int64 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(uint64 value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(float value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(double value)
{
    writeData(&value, sizeof(value));
}
//-----------------------------------------------------------------------------
void OutputStream::write(const String & value)
{
    String8 string8(value);    
    write(int32(string8.size()));
    writeData(string8.data(), string8.size());
}
//-----------------------------------------------------------------------------
void OutputStream::write(const Serializable & value)
{
    value.write(*this);
}
//-----------------------------------------------------------------------------
}
