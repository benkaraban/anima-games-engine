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

#include "FileStream.h"

namespace Core
{
//-----------------------------------------------------------------------------
FileInputStream::FileInputStream() : _file(NULL)
{}
//-----------------------------------------------------------------------------
FileInputStream::FileInputStream(const String & fileName) : _file(NULL)
{
    open(fileName);
}
//-----------------------------------------------------------------------------
FileInputStream::~FileInputStream()
{
    if(_file != NULL)
        close();
}
//-----------------------------------------------------------------------------
void FileInputStream::readData(void * buffer, int32 size)
{
    if(!isOpened())
        throw IOException(L"FileInputStream error : file not opened.");

    int32 readCount = fread(buffer, 1, size, _file);
    if(readCount != size)
    {
        String message(L"FileInputStream error : ");
        message << readCount << L" bytes read when " << size << L" were required.";
        throw IOException(message);
    }
}
//-----------------------------------------------------------------------------
int32 FileInputStream::readUpTo(void * buffer, int32 size)
{
    if(!isOpened()) throw IOException(L"FileInputStream error : file not opened.");
    if(eof()) throw IOException(L"FileInputStream error : reading a file with EOF flag.");

    return fread(buffer, 1, size, _file);
}
//-----------------------------------------------------------------------------
bool FileInputStream::eof() const
{
    return (feof(_file) != 0);
}
//-----------------------------------------------------------------------------
void FileInputStream::close()
{
    if(_file != NULL)
    {
        fclose(_file);
        _file = NULL;
    }
}
//-----------------------------------------------------------------------------
void FileInputStream::open(const String & fileName)
{
    close();
#if defined(_MSC_VER)
    _file = _wfsopen(fileName.c_str(), L"rb", _SH_DENYNO);
#else
    String8 fileName8(fileName);
    _file = fopen64(fileName8.c_str(), "rb");
#endif
}
//-----------------------------------------------------------------------------
bool FileInputStream::isOpened() const
{
    return _file != NULL;
}
//-----------------------------------------------------------------------------
int64 FileInputStream::getPosition() const
{
    #if defined(_MSC_VER)
        return _ftelli64(_file);
    #else
        return ftello64(_file);
    #endif
}
//-----------------------------------------------------------------------------
int64 FileInputStream::getLength() const
{
    #if defined(_MSC_VER)
        int64 savedPosition = _ftelli64(_file);
        _fseeki64(_file, 0, SEEK_END);
        int64 length = _ftelli64(_file);
        _fseeki64(_file, savedPosition, SEEK_SET);
    #else
        int64 savedPosition = ftello64(_file);
        fseeko64(_file, 0, SEEK_END);
        int64 length = ftello64(_file);
        fseeko64(_file, savedPosition, SEEK_SET);
    #endif
    

    return length;
}
//-----------------------------------------------------------------------------
void FileInputStream::setPosition(int64 offset, ESeekOrigin origin)
{
    switch(origin)
    {
#if defined(_MSC_VER)
    case SEEK_ORIGIN_SET : _fseeki64(_file, offset, SEEK_SET); break;
    case SEEK_ORIGIN_CUR : _fseeki64(_file, offset, SEEK_CUR); break;
    case SEEK_ORIGIN_END : _fseeki64(_file, offset, SEEK_END); break;
#else
    case SEEK_ORIGIN_SET : fseeko64(_file, offset, SEEK_SET); break;
    case SEEK_ORIGIN_CUR : fseeko64(_file, offset, SEEK_CUR); break;
    case SEEK_ORIGIN_END : fseeko64(_file, offset, SEEK_END); break;
#endif
    }
}
//-----------------------------------------------------------------------------
void FileInputStream::rewind()
{
#if defined(_MSC_VER)
    _fseeki64(_file, 0, SEEK_SET);
#else
    fseeko64(_file, 0, SEEK_SET);
#endif
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
FileOutputStream::FileOutputStream() : _file(NULL)
{}
//-----------------------------------------------------------------------------
FileOutputStream::FileOutputStream(const String & fileName, EFileAccess fileAccess)
{
    open(fileName, fileAccess);
}
//-----------------------------------------------------------------------------
FileOutputStream::~FileOutputStream()
{
    if( _file != NULL)
        close();
}
//-----------------------------------------------------------------------------
void FileOutputStream::open(const String & fileName, EFileAccess fileAccess)
{
    String8 fileName8(fileName);

    switch(fileAccess)
    {
#if defined(_MSC_VER)
    
    case FILE_TRUNCATE : _file = _wfsopen(fileName.c_str(), L"wb", _SH_DENYWR); break;
    case FILE_APPEND   : _file = _wfsopen(fileName.c_str(), L"ab", _SH_DENYWR); break;
#else
    case FILE_TRUNCATE : _file = fopen64(fileName8.c_str(), "wb"); break;
    case FILE_APPEND   : _file = fopen64(fileName8.c_str(), "ab"); break;
#endif
    }
}
//-----------------------------------------------------------------------------
bool FileOutputStream::isOpened() const
{
    return _file != NULL;
}
//-----------------------------------------------------------------------------
void FileOutputStream::writeData(const void * buffer, int32 size)
{
    if(!isOpened())
        throw IOException(L"FileOutputStream error : file not opened.");

    int32 writeCount = fwrite(buffer, 1, size, _file);
    if(writeCount != size)
    {
        String message(L"FileOutputStream error : ");
        message << writeCount << L" bytes written when " << size << L" were required.";
        throw IOException(message);
    }
}
//-----------------------------------------------------------------------------
void FileOutputStream::flush()
{
    fflush(_file);
}
//-----------------------------------------------------------------------------
bool FileOutputStream::eof() const
{
    return (feof(_file) != 0);
}
//-----------------------------------------------------------------------------
void FileOutputStream::close()
{
    if(_file != NULL)
    {
        fclose(_file);
        _file = NULL;
    }
}
//-----------------------------------------------------------------------------
}
