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
#include "Wave.h"

#include <Core/List.h>
#include <Core/IO/Tools.h>

namespace Audio
{
//-----------------------------------------------------------------------------
WaveHeader::WaveHeader()
{}
//-----------------------------------------------------------------------------
WaveHeader::~WaveHeader()
{}
//-----------------------------------------------------------------------------
void WaveHeader::read(Core::InputStream & input)
{
    input.read(riffTag[0]);input.read(riffTag[1]);input.read(riffTag[2]);input.read(riffTag[3]);
    input.read(fileSize);
    input.read(waveTag[0]);input.read(waveTag[1]);input.read(waveTag[2]);input.read(waveTag[3]);
    input.read(formatTag[0]);input.read(formatTag[1]);input.read(formatTag[2]);input.read(formatTag[3]);
    input.read(blocSize);
    input.read(audioFormat);
    input.read(nbChannels);
    input.read(frequency);
    input.read(bytesPerSec);
    input.read(bytesPerBloc);
    input.read(bitsPerSample);

    if(blocSize > 16)
    {
        int8 extraFormatBytes=0;
        for(int32 iExtraHeaderBytes=16; iExtraHeaderBytes < blocSize; ++iExtraHeaderBytes)
            input.read(extraFormatBytes);
    }

    input.read(dataTag[0]);input.read(dataTag[1]);input.read(dataTag[2]);input.read(dataTag[3]);
    input.read(dataSize);
}
//-----------------------------------------------------------------------------
void WaveHeader::write(Core::OutputStream & output) const
{
    output.write(riffTag[0]);output.write(riffTag[1]);output.write(riffTag[2]);output.write(riffTag[3]);
    output.write(fileSize);
    output.write(waveTag[0]);output.write(waveTag[1]);output.write(waveTag[2]);output.write(waveTag[3]);
    output.write(formatTag[0]);output.write(formatTag[1]);output.write(formatTag[2]);output.write(formatTag[3]);
    output.write(blocSize);
    output.write(audioFormat);
    output.write(nbChannels);
    output.write(frequency);
    output.write(bytesPerSec);
    output.write(bytesPerBloc);
    output.write(bitsPerSample);

    if(blocSize > 16)
    {
        int8 extraFormatBytes=0;
        for(int32 iExtraHeaderBytes=16; iExtraHeaderBytes < blocSize; ++iExtraHeaderBytes)
            output.write(extraFormatBytes);
    }

    output.write(dataTag[0]);output.write(dataTag[1]);output.write(dataTag[2]);output.write(dataTag[3]);
    output.write(dataSize);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
Wave::Wave()
:   _pcmData(Core::Buffer(0)),
    _offset(0)
{}
//-----------------------------------------------------------------------------
Wave::~Wave()
{}
//-----------------------------------------------------------------------------
void Wave::read(Core::InputStream & input)
{
    _waveHeader.read(input);
    _pcmData.read(input);

    //if(_waveHeader.dataSize != _pcmData.size())
    //    throw Core::Exception(L"The data size specified in the wave header is different from the actual data size.");
}
//-----------------------------------------------------------------------------
void Wave::write(Core::OutputStream & output) const
{
    output.write(_waveHeader);
    output.write(_pcmData);
}
//-----------------------------------------------------------------------------
uint32 Wave::getFrequency() const
{
    return _waveHeader.frequency;
}
//-----------------------------------------------------------------------------
uint32 Wave::getNbChannels() const
{
    return _waveHeader.nbChannels;
}
//-----------------------------------------------------------------------------
uint32 Wave::getBufferSizeFor250ms() const
{
    uint32 bufferSize = 0;

    bufferSize = _waveHeader.bytesPerSec >> 2; //<=> _waveHeader.bytesPerSec / 4; //250ms = 1sec/4
    
    // IMPORTANT : The Buffer Size must be an exact multiple of the BlockAlignment ...
    bufferSize -= (bufferSize % _waveHeader.bytesPerBloc);

    return bufferSize;
}
//-----------------------------------------------------------------------------
void Wave::rewind()
{
    _offset = 0;
}
//-----------------------------------------------------------------------------
int32 Wave::getData(Core::Buffer & buffer)
{
    int32 readSize = 0;
    if(_offset < _pcmData.size() && _offset < (int32)_waveHeader.dataSize)
    {
        byte * data = _pcmData.getData();
        data += _offset;

        int32 remainingBytesCount = _waveHeader.dataSize - _offset; 
        readSize = std::min(remainingBytesCount, buffer.size());

        memcpy(buffer.getData(), data, readSize);

        _offset += readSize;
    }

    return readSize;
}
//-----------------------------------------------------------------------------
uint16 Wave::getBitsPerSample() const
{
    return _waveHeader.bitsPerSample;
}
//-----------------------------------------------------------------------------
}//namespace Audio