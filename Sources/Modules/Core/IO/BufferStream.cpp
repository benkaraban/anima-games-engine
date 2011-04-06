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
#include <algorithm>
#include "BufferStream.h"

namespace Core
{
//-----------------------------------------------------------------------------
BufferStream::BufferStream() : _isOpened(true)
{}
//-----------------------------------------------------------------------------
void BufferStream::internalRead(void * buffer, int32 size)
{
    if(size > 0)
    {
        byte * pBuffer = static_cast<byte*>(buffer);

        for(int32 ii=0; ii < size; ii++)
            pBuffer[ii] = _data[ii];

        // Attention : même avec une size de 0, cet appel écrase l'élément
        // _data.begin()
        _data.erase(_data.begin(), _data.begin() + size);
    }
}
//-----------------------------------------------------------------------------
void BufferStream::readData(void * buffer, int32 size)
{
    if(!_isOpened)
        throw IOException(L"BufferStream read error : trying to read a closed stream.");

    if(size > _data.size())
    {
        String message(L"BufferStream error : ");
        message << _data.size() << L" bytes in buffer, " << size << L" expected.";
        throw IOException(message);
    }

    internalRead(buffer, size);
}
//-----------------------------------------------------------------------------
int32 BufferStream::readUpTo(void * buffer, int32 size)
{
    int32 readSize = std::min(size, int32(_data.size()));
    internalRead(buffer, readSize);

    return readSize;
}
//-----------------------------------------------------------------------------
void BufferStream::writeData(const void * buffer, int32 size)
{
    if(!_isOpened)
        throw IOException(L"BufferStream write error : trying to write in a closed stream.");

    const byte * pDataStart = static_cast<const byte*>(buffer);
    const byte * pDataEnd = pDataStart + size;
    _data.insert(_data.end(), pDataStart, pDataEnd);
}
//-----------------------------------------------------------------------------
bool BufferStream::eof() const
{
    return !_isOpened || _data.empty();
}
//-----------------------------------------------------------------------------
void BufferStream::flush()
{
    _data.clear();
}
//-----------------------------------------------------------------------------
void BufferStream::close()
{
    _isOpened = false;
    _data.clear();
}
//-----------------------------------------------------------------------------
void BufferStream::getDataCopy(List<byte> & data) const
{
    data.clear();
    data.insert(data.end(), _data.begin(), _data.end());
}
//-----------------------------------------------------------------------------
int32 BufferStream::getDataSize() const
{
    return _data.size();
}
//-----------------------------------------------------------------------------
}
