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
#include <Core/Bitmap.h>
#include <Core/Math/MathTools.h>

//include for memcpy
#include <string.h>

namespace Core
{
int32 getBytesPerPixel(EBitmapFormat format)
{
    switch(format)
    {
    case GRAY_U8:   return 1;
    case RGB_U8:    return 3;
    case ARGB_U8:   return 4;
    case GRAY_U16:  return 2;
    case RGB_U16:   return 6;
    case ARGB_U16:  return 8;
    case GRAY_S16:  return 2;
    case RGB_S16:   return 6;
    case ARGB_S16:  return 8;
    case GRAY_S32:  return 4;
    case RGB_S32:   return 12;
    case ARGB_S32:  return 16;
    case GRAY_F32:  return 4;
    case RGB_F32:   return 12;
    case ARGB_F32:  return 16;
    }

    return 0;
}

Bitmap::Bitmap(int32 w, int32 h, EBitmapFormat format) :
    _pData(new byte[getBytesPerPixel(format) * w *h]),
    _format(format),
    _width(w),
    _height(h),
    _ownData(true)
{
    _rowLength = getBytesPerPixel(_format) * _width;
    memset(_pData, 0, getBytesPerPixel(_format) * _width * _height);
}

Bitmap::Bitmap(byte * pData, int32 w, int32 h, EBitmapFormat format, int32 rowLength, bool ownData) :
    _pData(pData),
    _format(format),
    _width(w),
    _height(h),
    _ownData(ownData)
{
    if(rowLength == 0)
        _rowLength = getBytesPerPixel(_format) * _width;
    else
        _rowLength = rowLength;
}

Bitmap::~Bitmap()
{
    if(_ownData)
        delete [] _pData;
}

Ptr<Bitmap> toARGB32(const Ptr<Bitmap> & pSource)
{
    byte * pData = new byte [pSource->getWidth() * pSource->getHeight() * 4];
    Ptr<Bitmap> pResult(new Bitmap(pData, pSource->getWidth(), pSource->getHeight(), ARGB_U8, 0, true));

    if(pSource->getFormat() == ARGB_F32)
    {
        for(int32 y=0; y < pResult->getHeight(); y++)
        {
            byte *  pDst = pData + y * pResult->getRowLength();
            float * pSrc = (float*)(pSource->getData() + y * pSource->getRowLength());

            for(int32 x=0; x < pResult->getWidth() * 4; x += 4)
            {
                //pDst[x + 0] = (uint8)(255.0f * clamp(pSrc[x + 0], 0.0f, 1.0f));
                pDst[x + 0] = (uint8)(255.0f * clamp(0.0f, 1.0f, 0.5f * pSrc[x + 0] + 0.5f));
                pDst[x + 1] = (uint8)(255.0f * clamp(0.0f, 1.0f, 0.5f * pSrc[x + 1] + 0.5f));
                pDst[x + 2] = (uint8)(255.0f * clamp(0.0f, 1.0f, 0.5f * pSrc[x + 2] + 0.5f));
                //pDst[x + 3] = (uint8)(255.0f * clamp(0.5f * pSrc[x + 3] + 0.5f, 0.0f, 1.0f));
                pDst[x + 3] = (uint8)(255.0f * clamp(0.0f, 1.0f, pSrc[x + 3]));
            }
        }
    }
    else
    {
        throw Core::Exception(L"Conversion not supported");
    }

    return pResult;
}

void paste(const Ptr<Bitmap> & pTarget, const Ptr<Bitmap> & pSource, int32 x, int32 y)
{
    if(pSource->getFormat() != ARGB_U8 || pTarget->getFormat() != ARGB_U8)
        throw Exception(L"Unsupported format");

    const uint32 * pSrc = (uint32*)pSource->getData();
    uint32 * pDst = (uint32*)pTarget->getData();

    pDst += x + y * pTarget->getWidth();

    for(int32 ii=0; ii < pSource->getHeight(); ii++)
    {
        memcpy(pDst, pSrc, pSource->getWidth() * 4);

        pSrc += pSource->getWidth();
        pDst += pTarget->getWidth();
    }
}

Ptr<Bitmap> crop(const Ptr<Bitmap> & pSource, int32 x, int32 y, int32 w, int32 h)
{
    if(pSource->getFormat() != ARGB_U8)
        throw Exception(L"Unsupported format");

    const uint32 * pSrc = (uint32*)pSource->getData();
    uint32 * pData = new uint32[w * h];
    uint32 * pDst = pData;

    pSrc += y * pSource->getWidth();
    pSrc += x;

    for(int32 iy=0; iy < h; iy++)
    {
        memcpy(pDst, pSrc, w * 4);
        pSrc += pSource->getWidth();
        pDst += w;
    }

    return Ptr<Bitmap>(new Bitmap((byte*)pData, w, h, ARGB_U8, 0, true));    
}


}
