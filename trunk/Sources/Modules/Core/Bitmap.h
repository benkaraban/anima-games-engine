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
#ifndef CORE_BITMAP_H_
#define CORE_BITMAP_H_

#include <Core/Standard.h>

namespace Core
{
LM_ENUM_15(EBitmapFormat,
    GRAY_U8,  RGB_U8,  ARGB_U8,
    GRAY_U16, RGB_U16, ARGB_U16,
    GRAY_S16, RGB_S16, ARGB_S16,
    GRAY_S32, RGB_S32, ARGB_S32,
    GRAY_F32, RGB_F32, ARGB_F32)

LM_API_COR int32 getBytesPerPixel(EBitmapFormat format);

struct LM_API_COR PixelARGB8
{
    byte    b;
    byte    g;
    byte    r;
    byte    a;
};

class LM_API_COR Bitmap : public Object
{
public:
    Bitmap(int32 w, int32 h, EBitmapFormat format);
    Bitmap(byte * pData, int32 w, int32 h, EBitmapFormat format, int32 rowLength = 0, bool ownData = true);
    ~Bitmap();

          byte * getData()       { return _pData; }
    const byte * getData() const { return _pData; }
    int32 getWidth() const { return _width; }
    int32 getHeight() const { return _height; }
    int32 getRowLength() const { return _rowLength; }
    EBitmapFormat getFormat() const { return _format; }
    bool ownData() const { return _ownData; }

protected:
    byte *          _pData;
    EBitmapFormat   _format;
    int32           _width;
    int32           _height;
    int32           _rowLength;
    bool            _ownData;
};

LM_API_COR Ptr<Bitmap>  toARGB32(const Ptr<Bitmap> & pSource);
LM_API_COR Ptr<Bitmap>  crop(const Ptr<Bitmap> & pSource, int32 x, int32 y, int32 w, int32 h);
LM_API_COR void         paste(const Ptr<Bitmap> & pTarget, const Ptr<Bitmap> & pSource, int32 x, int32 y);
}
#endif
