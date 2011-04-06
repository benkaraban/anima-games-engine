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

#include "MemoryStream.h"

#include <string.h>

namespace Core
{
//-----------------------------------------------------------------------------
MemoryInputStream::MemoryInputStream(const void * pData, int32 size, bool ownData)
    : _pDataStart(reinterpret_cast<const byte*>(pData)),
      _pDataEnd(_pDataStart + size),
      _pCurData(_pDataStart),
      _ownData(ownData)
{}
//-----------------------------------------------------------------------------
MemoryInputStream::~MemoryInputStream()
{
    if(_ownData)
    {
        delete [] _pDataStart;
    }    
}
//-----------------------------------------------------------------------------
void MemoryInputStream::readData(void * buffer, int32 size)
{
    int32 readCount = readUpTo(buffer, size);
    if(readCount != size)
    {        
        String message(L"MemoryInputStream error : ");
        message << readCount << L" bytes read when " << size << L" were required.";
        throw IOException(message);
    }
}
//-----------------------------------------------------------------------------
int32 MemoryInputStream::readUpTo(void * buffer, int32 size)
{
    if(eof()) throw IOException(L"MemoryInputStream error : reading a stream with EOF flag.");
    
    int32 remainingBytesCount = _pDataEnd - _pCurData; 
    int32 readSize = std::min(remainingBytesCount, size);
    ::memcpy(buffer, _pCurData, readSize);
    _pCurData += readSize;
    
    return readSize;
}
//-----------------------------------------------------------------------------
bool MemoryInputStream::eof() const
{
    return _pCurData >= _pDataEnd;
}
//-----------------------------------------------------------------------------
void MemoryInputStream::close()
{}
//-----------------------------------------------------------------------------
int64 MemoryInputStream::getPosition() const
{
    return _pCurData - _pDataStart;
}
//-----------------------------------------------------------------------------
int64 MemoryInputStream::getLength() const
{
    return _pDataEnd - _pDataStart;
}
//-----------------------------------------------------------------------------
void MemoryInputStream::setPosition(int64 offset, ESeekOrigin origin)
{
    switch(origin)
    {
    case SEEK_ORIGIN_SET:
        _pCurData = _pDataStart + int32(offset);
        break;
    case SEEK_ORIGIN_CUR:
        _pCurData += int32(offset);
        break;
    case SEEK_ORIGIN_END:
        _pCurData = _pDataEnd + int32(offset);
        break;
    }
}
//-----------------------------------------------------------------------------
void MemoryInputStream::rewind()
{
    _pCurData = _pDataStart;
}
//-----------------------------------------------------------------------------
}
