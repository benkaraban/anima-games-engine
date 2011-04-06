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
#include "TextStream.h"

namespace Core
{
//-----------------------------------------------------------------------------
TextInputStream::TextInputStream(const Ptr<InputStream> & pStream) : _pStream(pStream)
{}
//-----------------------------------------------------------------------------
TextInputStream::~TextInputStream()
{
    close();
}
//-----------------------------------------------------------------------------
void TextInputStream::getLine(String & string)
{
    String8 string8;
    char    oneChar;
    int32   readBytes = _pStream->readUpTo(&oneChar, sizeof(char));
    bool    isEndLine = (oneChar == '\n'); 
    
    while(readBytes == sizeof(char) && !isEndLine)
    {
        if(oneChar != '\r')
        {
            string8.push_back(oneChar);
        }
        
        readBytes = _pStream->readUpTo(&oneChar, sizeof(char));
        isEndLine = (oneChar == '\n');
    }
    
    string = String(string8);
}
//-----------------------------------------------------------------------------
String TextInputStream::getLine()
{
    String result;
    getLine(result);
    
    return result;
}
//-----------------------------------------------------------------------------
bool TextInputStream::eof() const
{
    return _pStream->eof();
}
//-----------------------------------------------------------------------------
void TextInputStream::close()
{
    _pStream->close();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TextOutputStream::TextOutputStream(const Ptr<OutputStream> & pStream, bool windowMode) 
    : _pStream(pStream),
      _windowMode(windowMode)
{}
//-----------------------------------------------------------------------------
TextOutputStream::~TextOutputStream()
{
    close();
}
//-----------------------------------------------------------------------------
void TextOutputStream::put(const String & string)
{
    String8 string8(string);
    
    if(_windowMode)
    {
        int32 curPos = 0;
        int32 nextCR = string8.find_first_of("\r\n", curPos);
        
        while(curPos < int32(string8.length()) && nextCR != String8::npos)
        {            
            _pStream->writeData(&string8[curPos], nextCR - curPos);
            endLine();
            
            curPos = nextCR + 1;
            nextCR = string8.find_first_of("\r\n", curPos);
        }
        
        _pStream->writeData(&string8[curPos], string8.length() - curPos);        
    }
    else
    {    
        _pStream->writeData(string8.data(), string8.length());
    }
}
//-----------------------------------------------------------------------------
void TextOutputStream::putLine(const String & string)
{
    put(string);
    endLine();
}
//-----------------------------------------------------------------------------
void TextOutputStream::endLine()
{
    String8 endLineSequence(_windowMode ? "\r\n" : "\n");
    _pStream->writeData(endLineSequence.data(), endLineSequence.length());
}
//-----------------------------------------------------------------------------
void TextOutputStream::flush()
{
    _pStream->flush();
}
//-----------------------------------------------------------------------------
void TextOutputStream::close()
{
    _pStream->close();
}
//-----------------------------------------------------------------------------
}
