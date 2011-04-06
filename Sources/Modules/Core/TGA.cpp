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
#include <Core/TGA.h>
#include <Core/IO/FileStream.h>
#include <Core/List.h>

namespace Core
{
Ptr<Bitmap> loadTGA(InputStream & input)
{
    int32 identSize = input.readUInt8();
    input.readUInt8(); //int32 colorMapType = input.readUInt8();
    int32 imageType = input.readUInt8();
    
    input.readUInt16();//int32 colorMapStart = input.readUInt16();
    input.readUInt16();//int32 colorMapLength = input.readUInt16();
    input.readUInt8(); //int32 colorMapBits = input.readUInt8();

    input.readUInt16();//int32 xstart = input.readUInt16();
    input.readUInt16();//int32 ystart = input.readUInt16();
    int32 width = input.readUInt16();
    int32 height = input.readUInt16();

    int32 bpp = input.readUInt8();
    int32 desc = input.readUInt8();

    if(imageType != 2)
        throw Core::Exception(L"Unsupported TGA format (palette or RLE compressed)");

    if(bpp != 32)
        throw Core::Exception(L"Unsupported TGA bits per pixel (must be 32)");

    if(identSize > 0)
    {
        Core::List<byte> ignore;
        ignore.resize(identSize);
        input.readData(ignore.data(), ignore.size());
    }

    int32 rowStep = width * 4;
    byte * pData = new byte [height * rowStep];

    if(desc & 0x0010)
    {
        for(int32 y=0; y < height; y++)
            input.readData(pData + (y * rowStep), rowStep);
    }
    else
    {
        for(int32 y=(height - 1); y >= 0; y--)
            input.readData(pData + (y * rowStep), rowStep);
    }

    return Ptr<Bitmap>(new Bitmap(pData, width, height, ARGB_U8, rowStep, true));
}

Ptr<Bitmap> loadTGA(const String & fileName)
{
    FileInputStream file;
    file.open(fileName);

    if(!file.isOpened())
        throw Core::Exception(L"Unable to open TGA file " + fileName);

    return loadTGA(file);
}

void saveTGA(const Ptr<Bitmap> & pBitmap, OutputStream & output)
{
    if(pBitmap->getFormat() != ARGB_U8)
        throw Core::Exception(L"Unsupported TGA format (must be 32 bits)");

    output.write(uint8(0)); // identSize
    output.write(uint8(0)); // colorMapType
    output.write(uint8(2)); // imageType

    output.write(uint16(0)); // colorMapStart
    output.write(uint16(0)); // colorMapLength
    output.write(uint8(0));  // colorMapBits

    output.write(uint16(0)); // xstart
    output.write(uint16(0)); // ystart
    output.write(uint16(pBitmap->getWidth()));
    output.write(uint16(pBitmap->getHeight()));

    output.write(uint8(32));
    output.write(uint8(0x0008)); // desc 0x10 = vert bit, 0x08 = alpha bits

    int32 width = pBitmap->getWidth();
    int32 height = pBitmap->getHeight();
    int32 rowStep = width * 4;
    byte * pData = (byte*)pBitmap->getData();

    for(int32 y=(height - 1); y >= 0; y--)
        output.writeData(pData + (y * rowStep), rowStep);
}

void saveTGA(const Ptr<Bitmap> & pBitmap, const String & fileName)
{
    FileOutputStream file;
    file.open(fileName);

    if(!file.isOpened())
        throw Core::Exception(L"Unable to open TGA file for save " + fileName);

    saveTGA(pBitmap, file);
}

}
