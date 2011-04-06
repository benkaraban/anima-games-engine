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
#include <FreeImage.h>
#include <Core/IO/FileStream.h>
#include <Core/IO/Tools.h>
#include <Core/Math/MathTools.h>
#include "Image.h"
#include "Operator.h"
#include <math.h>

#pragma warning( push )
#pragma warning( disable : 4290 )

int32 getSaveFlags(FREE_IMAGE_FORMAT fif)
{
    int32 flags = 0;

    switch(fif)
    {
    case FIF_JPEG: flags = JPEG_QUALITYSUPERB | JPEG_ACCURATE | JPEG_SUBSAMPLING_411; break;
    case FIF_TIFF: flags = TIFF_NONE; break;
    }

    return flags;
}

//-----------------------------------------------------------------------------
struct InputHandle
{
    Core::SeekableInputStream * pInput;
};
//-----------------------------------------------------------------------------
struct OutputHandle
{
    Core::OutputStream * pOutput;
};
//-----------------------------------------------------------------------------
unsigned DLL_CALLCONV readCallback(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
    try
    {
        if(size != 0)
        {
            InputHandle * pHandle = (InputHandle *)handle;
            int32 result = pHandle->pInput->readUpTo(buffer, size * count);
            return result / size;
        }
    }
    catch(Core::Exception &)
    {
        // Vu que ce callback est appelé par FreeImage et que FreeImage ne gère pas nos exceptions,
        // on ne peut pas faire grand chose si ce n'est retourner 0 pour signaler une erreur.
    }
    return 0;
}
//-----------------------------------------------------------------------------
unsigned DLL_CALLCONV writeCallback(void *buffer, unsigned size, unsigned count, fi_handle handle)
{
    try
    {
        OutputHandle * pHandle = (OutputHandle *)handle;
        pHandle->pOutput->writeData(buffer, size * count);
    }
    catch(Core::Exception &)
    {
        // Vu que ce callback est appelé par FreeImage et que FreeImage ne gère pas nos exceptions,
        // on ne peut pas faire grand chose si ce n'est retourner 0 pour signaler une erreur.
        return 0;
    }
    return count;
}
//-----------------------------------------------------------------------------
int DLL_CALLCONV seekCallbackInput(fi_handle handle, long offset, int origin)
{
    InputHandle * pHandle = (InputHandle *)handle;
    int32 oldPosition = (int32)pHandle->pInput->getPosition();
    switch(origin)
    {
        case SEEK_SET: pHandle->pInput->setPosition(offset, Core::SEEK_ORIGIN_SET); break;
        case SEEK_CUR: pHandle->pInput->setPosition(offset, Core::SEEK_ORIGIN_CUR); break;
        case SEEK_END: pHandle->pInput->setPosition(offset, Core::SEEK_ORIGIN_END); break;
    }
    return oldPosition;
}
//-----------------------------------------------------------------------------
long DLL_CALLCONV tellCallbackInput(fi_handle handle)
{
    InputHandle * pHandle = (InputHandle *)handle;
    return (int32)pHandle->pInput->getPosition();
}
//-----------------------------------------------------------------------------
static FREE_IMAGE_FORMAT getFreeImageFormat(Images::EImageFileFormat format)
{
    switch(format)
    {
        case Images::IMAGE_FILE_JPG: return FIF_JPEG;
        case Images::IMAGE_FILE_PNG: return FIF_PNG;
        case Images::IMAGE_FILE_BMP: return FIF_BMP;
        case Images::IMAGE_FILE_TGA: return FIF_TARGA;
        case Images::IMAGE_FILE_PPM: return FIF_PPM;
    }
    return FIF_UNKNOWN;
}
//-----------------------------------------------------------------------------
void FIB2Image(FIBITMAP * fib, Images::Image& img) throw(Core::IOException)
{
    if(fib == NULL) throw Core::IOException(L"Error loading image : invalid data");

    img.erase();
    switch(FreeImage_GetImageType(fib))
    {
        case FIT_BITMAP:
        {
            switch(FreeImage_GetBPP(fib))
            {
                case 8:
                {
                    img.erase();
                    img._width = FreeImage_GetWidth(fib);
                    img._height = FreeImage_GetHeight(fib);
                    img._rowStep = FreeImage_GetPitch(fib);
                    img._colorSpace = Images::GRAY;
                    img._chnDepth = Images::U8;

                    img._data = new byte [img._height*img._rowStep];
                    // convert the bitmap to raw bits (top-left pixel first)
                    FreeImage_ConvertToRawBits(img._data, fib, img._rowStep, 8,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
                }
                break;

                case 24:
                {
                    img.erase();
                    img._width = FreeImage_GetWidth(fib);
                    img._height = FreeImage_GetHeight(fib);
                    img._rowStep = FreeImage_GetPitch(fib);
                    img._colorSpace = Images::RGB;
                    img._chnDepth = Images::U8;

                    img._data = new byte [img._height*img._rowStep];
                    // convert the bitmap to raw bits (top-left pixel first)
                    FreeImage_ConvertToRawBits(img._data, fib, img._rowStep, 24,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
                }
                break;

                case 32:
                {
                    img.erase();
                    img._width = FreeImage_GetWidth(fib);
                    img._height = FreeImage_GetHeight(fib);
                    img._rowStep = FreeImage_GetPitch(fib);
                    img._colorSpace = Images::ARGB;
                    img._chnDepth = Images::U8;

                    img._data = new byte [img._height*img._rowStep];
                    // convert the bitmap to raw bits (top-left pixel first)
                    FreeImage_ConvertToRawBits(img._data, fib, img._rowStep, 32,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
                }
                break;

                default:
                    throw Core::IOException(L"Error loading image : BITMAP unsupported bits per pixel format");
                break;
            }
        }
        break;

        case FIT_FLOAT:
        {
            img.erase();
            img._width = FreeImage_GetWidth(fib);
            img._height = FreeImage_GetHeight(fib);
            img._rowStep = FreeImage_GetPitch(fib);
            img._colorSpace = Images::GRAY;
            img._chnDepth = Images::F32;

            img._data = new byte [img._height*img._rowStep];
            Images::it2D_grayF32 itPix;
            Images::makeIterator(img, itPix);
            foreach_line_i(itPix, iY)
            {
                byte *bits = (byte *)FreeImage_GetScanLine(fib, img._height-iY-1);
                memcpy((byte *) itPix.rowBegin(), bits, img._width * img.getBytePerPixel());
            }
        }
        break;

        case FIT_RGBF:
        {
            img.erase();
            img._width = FreeImage_GetWidth(fib);
            img._height = FreeImage_GetHeight(fib);
            img._rowStep = FreeImage_GetPitch(fib);
            img._colorSpace = Images::RGB;
            img._chnDepth = Images::F32;

            img._data = new byte [img._height*img._rowStep];
            Images::it2D_rgbF32 itPix;
            Images::makeIterator(img, itPix);
            foreach_line_i(itPix, iY)
            {
                byte *bits = (byte *)FreeImage_GetScanLine(fib, img._height-iY-1);
                memcpy((byte *) itPix.rowBegin(), bits, img._width * img.getBytePerPixel());
            }
        }
        break;

        case FIT_RGBAF:
        {
            img.erase();
            img._width = FreeImage_GetWidth(fib);
            img._height = FreeImage_GetHeight(fib);
            img._rowStep = FreeImage_GetPitch(fib);
            img._colorSpace = Images::ARGB;
            img._chnDepth = Images::F32;

            img._data = new byte [img._height*img._rowStep];
            Images::it2D_argbF32 itPix;
            Images::makeIterator(img, itPix);
            foreach_line_i(itPix, iY)
            {
                byte *bits = (byte *)FreeImage_GetScanLine(fib, img._height-iY-1);
                memcpy((byte *) itPix.rowBegin(), bits, img._width * img.getBytePerPixel());
            }
        }
        break;

        case FIT_UINT16:
        {
            img.erase();
            img._width = FreeImage_GetWidth(fib);
            img._height = FreeImage_GetHeight(fib);
            img._rowStep = FreeImage_GetPitch(fib);
            img._colorSpace = Images::GRAY;
            img._chnDepth = Images::U16;

            img._data = new byte [img._height*img._rowStep];
            Images::it2D_grayU16 itPix;
            Images::makeIterator(img, itPix);
            foreach_line_i(itPix, iY)
            {
                byte *bits = (byte *)FreeImage_GetScanLine(fib, img._height-iY-1);
                memcpy((byte *) itPix.rowBegin(), bits, img._width * img.getBytePerPixel());
            }
        }
        break;


        default:
            throw Core::IOException(L"Error loading image : unsupported pixel format");
        break;
    }
}

FIBITMAP* ImageFIB(const Images::Image& img) throw(Core::IOException)
{
    FIBITMAP* fib = NULL;
    Core::EBitmapFormat imgType;
    colorAndDepthToimgType(img._colorSpace, img._chnDepth, imgType);

    switch(imgType)
    {
        case Core::GRAY_U8:
            fib = FreeImage_ConvertFromRawBits(img._data, img._width, img._height, img._rowStep, 8,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
        break;

        case Core::RGB_U8:
            fib = FreeImage_ConvertFromRawBits(img._data, img._width, img._height, img._rowStep, 24,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
        break;

        case Core::ARGB_U8:
            fib = FreeImage_ConvertFromRawBits(img._data, img._width, img._height, img._rowStep, 32,
                    FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, TRUE);
        break;

        case Core::GRAY_F32:
            fib = FreeImage_AllocateT(FIT_FLOAT, img._width, img._height);
            Images::it2D_grayF32 itPix;
            Images::makeIterator(const_cast<Images::Image&>(img), itPix);
            foreach_line_i(itPix, iY)
            {
                byte *bits = (byte *)FreeImage_GetScanLine(fib, img._height-iY-1);
                memcpy(bits, (byte *) itPix.rowBegin(), img._width * img.getBytePerPixel());
            }
        break;

        case Core::GRAY_U16:
        case Core::RGB_U16:
        case Core::ARGB_U16:
        case Core::GRAY_S16:
        case Core::RGB_S16:
        case Core::ARGB_S16:
        case Core::GRAY_S32:
        case Core::RGB_S32:
        case Core::ARGB_S32:
        case Core::RGB_F32:
        case Core::ARGB_F32:
            throw Core::IOException(L"Error convert image to freeImage (ImageFIB) : type d'image non geree");
        break;
    }

    return fib;
}

namespace Images
{
//-----------------------------------------------------------------------------
LM_API_IMG void imgTypeToColorAndDepth(const Core::EBitmapFormat type, EColorSpace& space, EChannelDepth& depth)
{
    switch(type)
    {
    case Core::GRAY_U8: space = GRAY; depth = U8; break;
    case Core::RGB_U8: space = RGB; depth = U8; break;
    case Core::ARGB_U8: space = ARGB; depth = U8; break;
    case Core::GRAY_U16: space = GRAY; depth = U16; break;
    case Core::RGB_U16: space = RGB; depth = U16; break;
    case Core::ARGB_U16: space = ARGB; depth = U16; break;
    case Core::GRAY_S16: space = GRAY; depth = S16; break;
    case Core::RGB_S16: space = RGB; depth = S16; break;
    case Core::ARGB_S16: space = ARGB; depth = S16; break;
    case Core::GRAY_S32: space = GRAY; depth = S32; break;
    case Core::RGB_S32: space = RGB; depth = S32; break;
    case Core::ARGB_S32: space = ARGB; depth = S32; break;
    case Core::GRAY_F32: space = GRAY; depth = F32; break;
    case Core::RGB_F32: space = RGB; depth = F32; break;
    case Core::ARGB_F32: space = ARGB; depth = F32; break;
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void colorAndDepthToimgType(const EColorSpace space, const EChannelDepth depth, Core::EBitmapFormat& type)
{
    switch(space)
    {
        case GRAY:
            switch(depth)
            {
                case U8: type = Core::GRAY_U8; break;
                case U16: type = Core::GRAY_U16; break;
                case S16: type = Core::GRAY_S16; break;
                case S32: type = Core::GRAY_S32; break;
                case F32: type = Core::GRAY_F32; break;
            }
        break;

        case RGB:
            switch(depth)
            {
                case U8: type = Core::RGB_U8; break;
                case U16: type = Core::RGB_U16; break;
                case S16: type = Core::RGB_S16; break;
                case S32: type = Core::RGB_S32; break;
                case F32: type = Core::RGB_F32; break;
            }
        break;

        case ARGB:
            switch(depth)
            {
                case U8: type = Core::ARGB_U8; break;
                case U16: type = Core::ARGB_U16; break;
                case S16: type = Core::ARGB_S16; break;
                case S32: type = Core::ARGB_S32; break;
                case F32: type = Core::ARGB_F32; break;
            }
        break;

    }
}
//-----------------------------------------------------------------------------
const grayU8 grayU8::MIN_VALUE = {Core::UINT8_MIN};
const grayU8 grayU8::MAX_VALUE = {Core::UINT8_MAX};
const rgbU8 rgbU8::MIN_VALUE = {Core::UINT8_MIN, Core::UINT8_MIN, Core::UINT8_MIN};
const rgbU8 rgbU8::MAX_VALUE = {Core::UINT8_MAX, Core::UINT8_MAX, Core::UINT8_MAX};
const argbU8 argbU8::MIN_VALUE = {Core::UINT8_MIN, Core::UINT8_MIN, Core::UINT8_MIN, Core::UINT8_MIN};
const argbU8 argbU8::MAX_VALUE = {Core::UINT8_MAX, Core::UINT8_MAX, Core::UINT8_MAX, Core::UINT8_MAX};

const grayU16 grayU16::MIN_VALUE = {Core::UINT16_MIN};
const grayU16 grayU16::MAX_VALUE = {Core::UINT16_MAX};
const rgbU16 rgbU16::MIN_VALUE = {Core::UINT16_MIN, Core::UINT16_MIN, Core::UINT16_MIN};
const rgbU16 rgbU16::MAX_VALUE = {Core::UINT16_MAX, Core::UINT16_MAX, Core::UINT16_MAX};
const argbU16 argbU16::MIN_VALUE = {Core::UINT16_MIN, Core::UINT16_MIN, Core::UINT16_MIN, Core::UINT16_MIN};
const argbU16 argbU16::MAX_VALUE = {Core::UINT16_MAX, Core::UINT16_MAX, Core::UINT16_MAX, Core::UINT16_MAX};

const grayS16 grayS16::MIN_VALUE = {Core::INT16_MIN};
const grayS16 grayS16::MAX_VALUE = {Core::INT16_MAX};
const rgbS16 rgbS16::MIN_VALUE = {Core::INT16_MIN, Core::INT16_MIN, Core::INT16_MIN};
const rgbS16 rgbS16::MAX_VALUE = {Core::INT16_MAX, Core::INT16_MAX, Core::INT16_MAX};
const argbS16 argbS16::MIN_VALUE = {Core::INT16_MIN, Core::INT16_MIN, Core::INT16_MIN, Core::INT16_MIN};
const argbS16 argbS16::MAX_VALUE = {Core::INT16_MAX, Core::INT16_MAX, Core::INT16_MAX, Core::INT16_MAX};

const grayS32 grayS32::MIN_VALUE = {Core::INT32_MIN};
const grayS32 grayS32::MAX_VALUE = {Core::INT32_MAX};
const rgbS32 rgbS32::MIN_VALUE = {Core::INT32_MIN, Core::INT32_MIN, Core::INT32_MIN};
const rgbS32 rgbS32::MAX_VALUE = {Core::INT32_MAX, Core::INT32_MAX, Core::INT32_MAX};
const argbS32 argbS32::MIN_VALUE = {Core::INT32_MIN, Core::INT32_MIN, Core::INT32_MIN, Core::INT32_MIN};
const argbS32 argbS32::MAX_VALUE = {Core::INT32_MAX, Core::INT32_MAX, Core::INT32_MAX, Core::INT32_MAX};

const grayF32 grayF32::MIN_VALUE = {FLT_MIN};
const grayF32 grayF32::MAX_VALUE = {FLT_MAX};
const rgbF32 rgbF32::MIN_VALUE = {FLT_MIN, FLT_MIN, FLT_MIN};
const rgbF32 rgbF32::MAX_VALUE = {FLT_MAX, FLT_MAX, FLT_MAX};
const argbF32 argbF32::MIN_VALUE = {FLT_MIN, FLT_MIN, FLT_MIN, FLT_MIN};
const argbF32 argbF32::MAX_VALUE = {FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX};
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// La classe image
//-----------------------------------------------------------------------------
Image::Image():
    _width(1),
    _height(1),
    _rowStep(4),
    _colorSpace(ARGB),
    _chnDepth(U8)
{
    _rowStep = getBytePerPixel();
    _data = new byte [_height*_rowStep];
    memset(_data, 0, _height*_rowStep);
}
//-----------------------------------------------------------------------------
Image::~Image()
{
    delete [] _data;
}
//-----------------------------------------------------------------------------
Image::Image(const Core::EBitmapFormat type, const int32 width, const int32 height):
    _data(NULL),
    _width(width),
    _height(height),
    _rowStep(0)
{
    imgTypeToColorAndDepth(type, _colorSpace, _chnDepth);
    int32 widthInByte = width*getBytePerPixel();
    _rowStep = widthInByte + widthInByte%16;
    _data = new byte [_height*_rowStep];
    memset(_data, 0, _height*_rowStep);
}
//-----------------------------------------------------------------------------
Image::Image(const Ptr<Core::Bitmap> & pBitmap) :
    _width(pBitmap->getWidth()),
    _height(pBitmap->getHeight()),
    _rowStep(pBitmap->getRowLength())
{
    imgTypeToColorAndDepth(pBitmap->getFormat(), _colorSpace, _chnDepth);
    _data = new byte [_height * _rowStep];
    memcpy(_data, pBitmap->getData(), _height * _rowStep);
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> Image::toBitmap()
{
    Core::EBitmapFormat format;
    colorAndDepthToimgType(_colorSpace, _chnDepth, format);

    byte * pData = new byte [_height * _rowStep];
    memcpy(pData, _data, _height * _rowStep);

    return Ptr<Core::Bitmap>(new Core::Bitmap(pData, _width, _height, format, _rowStep, true));
}
//-----------------------------------------------------------------------------
Image::Image(void* data, int32 width, int32 height, int32 rowStep, int32 bitsPerPixel, uint32 redMask, uint32 greenMask, uint32 blueMask, bool topDown):
    _data(NULL),
    _width(0),
    _height(0),
    _rowStep(0),
    _colorSpace(GRAY),
    _chnDepth(U8)
{
    FIBITMAP * fib = FreeImage_ConvertFromRawBits((BYTE*)data, width, height, rowStep, bitsPerPixel, redMask, greenMask, blueMask, topDown);
    FIBITMAP * fib2 = FreeImage_ConvertTo32Bits(fib);
    FIB2Image(fib2, *this);
    FreeImage_Unload(fib);
    FreeImage_Unload(fib2);
}
//-----------------------------------------------------------------------------
void Image::erase()
{
    delete [] _data;
    _data = NULL;
    _width = 0;
    _height = 0;
    _rowStep = 0;
    _colorSpace = GRAY;
    _chnDepth = U8;
}
//-----------------------------------------------------------------------------
void Image::loadFile(const String & fileName) throw(Core::IOException)
{
    Core::FileInputStream file;
    file.open(fileName);
    if(!file.isOpened())
    {
        String message(L"Error loading image : unable to open file ");
        message << fileName;
        throw Core::IOException(message);
    }

    InputHandle handle =
    {   &file};
    FreeImageIO imageReadIO =
    {
        readCallback,
        NULL,
        seekCallbackInput,
        tellCallbackInput
    };
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(fileName.c_str());
    FIBITMAP * fib = FreeImage_LoadFromHandle(fif, &imageReadIO, &handle);

    //TODO temporaire on convertie en 32 bits
    FIBITMAP * fib2 = FreeImage_ConvertTo32Bits(fib);
    FIB2Image(fib2, *this);
    FreeImage_Unload(fib);
    FreeImage_Unload(fib2);
}
//-----------------------------------------------------------------------------
void Image::saveFile(const String & fileName) const
{
    Core::FileOutputStream file;
    file.open(fileName, Core::FileOutputStream::FILE_TRUNCATE);
    if(!file.isOpened())
    {
        String message(L"Error saving image : unable to open file ");
        message << fileName;
        throw Core::IOException(message);
    }

    OutputHandle handle =
    {   &file};
    FreeImageIO imageIO =
    {
        NULL,
        writeCallback,
        NULL,
        NULL
    };

    FIBITMAP * fib = ImageFIB(*this);
    FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilenameU(fileName.c_str());
    BOOL ok = FreeImage_SaveToHandle(fif, fib, &imageIO, &handle, getSaveFlags(fif));
    FreeImage_Unload(fib);
    if(!ok) throw Core::IOException(L"Error saving image : internal encoding error");
}
//-----------------------------------------------------------------------------
void Image::load(Core::SeekableInputStream & input) throw(Core::IOException)
{
    InputHandle handle =
    {   &input};
    FreeImageIO imageReadIO =
    {
        readCallback,
        NULL,
        seekCallbackInput,
        tellCallbackInput
    };
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&imageReadIO, &handle);

    /* Le format TGA n'a pas de header particulier mais c'est le seul format
     * connu qui n'en n'ait pas donc si on n'a pas réussi à trouver un format en
     * analysant le header il y a 99.9% de chances qu'il s'agisse d'un TGA et
     * dans le cas contraire il faut arrêter d'utiliser des formats bidons.
     */
    if(fif == FIF_UNKNOWN)
    fif = FIF_TARGA;

    FIBITMAP * fib = FreeImage_LoadFromHandle(fif, &imageReadIO, &handle);

    //TODO temporaire : on convertie en 32 bits
    FIBITMAP * fib2 = FreeImage_ConvertTo32Bits(fib);
    FIB2Image(fib2, *this);
    FreeImage_Unload(fib);
    FreeImage_Unload(fib2);
}
//-----------------------------------------------------------------------------
void Image::load2(Core::SeekableInputStream & input) throw(Core::IOException)
{
    InputHandle handle =
    {   &input};
    FreeImageIO imageReadIO =
    {
        readCallback,
        NULL,
        seekCallbackInput,
        tellCallbackInput
    };
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromHandle(&imageReadIO, &handle);

    /* Le format TGA n'a pas de header particulier mais c'est le seul format
     * connu qui n'en n'ait pas donc si on n'a pas réussi à trouver un format en
     * analysant le header il y a 99.9% de chances qu'il s'agisse d'un TGA et
     * dans le cas contraire il faut arrêter d'utiliser des formats bidons.
     */
    if(fif == FIF_UNKNOWN)
    fif = FIF_TARGA;

    FIBITMAP * fib = FreeImage_LoadFromHandle(fif, &imageReadIO, &handle);
    FIB2Image(fib, *this);
    FreeImage_Unload(fib);
}
//-----------------------------------------------------------------------------
void Image::save(Core::OutputStream & output, EImageFileFormat format) const
{
    OutputHandle handle =
    {   &output};
    FreeImageIO imageIO =
    {
        NULL,
        writeCallback,
        NULL,
        NULL
    };

    FIBITMAP * fib = ImageFIB(*this);
    FREE_IMAGE_FORMAT fif = getFreeImageFormat(format);
    BOOL ok = FreeImage_SaveToHandle(fif, fib, &imageIO, &handle, getSaveFlags(fif));
    FreeImage_Unload(fib);
    if(!ok) throw Core::IOException(L"Error saving image : internal encoding error");
}
//-----------------------------------------------------------------------------
void Image::resize(const int32 width, const int32 height, const EInterpolationType inter)
{
    Image tmp (this->getImgType(), width,  height);
    Images::resize(*this, tmp, inter);
    erase();

    _data = tmp._data;
    _width = tmp._width;
    _height = tmp._height;
    _rowStep = tmp._rowStep;
    _colorSpace = tmp._colorSpace;
    _chnDepth = tmp._chnDepth;

    tmp._data = NULL;
}
//-----------------------------------------------------------------------------
int32 Image::getBytePerPixel() const
{
    int32 bpp = 0;
    int32 bytePerChannel = 0;

    switch(_chnDepth)
    {
        case U8: bytePerChannel = 1; break;
        case U16: bytePerChannel = 2; break;
        case S16: bytePerChannel = 2; break;
        case S32: bytePerChannel = 4; break;
        case F32: bytePerChannel = 4; break;
    }

    switch(_colorSpace)
    {
        case GRAY: bpp = bytePerChannel; break;
        case RGB: bpp = 3*bytePerChannel; break;
        case ARGB: bpp = 4*bytePerChannel; break;
    }

    return bpp;
}
//-----------------------------------------------------------------------------
void Image::cloneTo(Image& dst) const
{
    dst.erase();


    dst._width = this->_width;
    dst._height = this->_height;
    dst._rowStep = this->_rowStep;
    dst._colorSpace = this->_colorSpace;
    dst._chnDepth = this->_chnDepth;

    dst._data = new byte [_height*_rowStep];
    memcpy(dst._data, _data, _height*_rowStep);
}
//-----------------------------------------------------------------------------
Ptr<Image> Image::clone() const
{
    Ptr<Image> pClone(new Image());
    cloneTo(*pClone);
    return pClone;
}
//-----------------------------------------------------------------------------
Core::EBitmapFormat Image::getImgType() const
{
    Core::EBitmapFormat imgType;
    colorAndDepthToimgType(_colorSpace, _chnDepth, imgType);

    return imgType;
}
//-----------------------------------------------------------------------------
template <class T>
inline T getSubPixel_T(float x, float y,
                       byte* data, const int32 width, const int32 height, const int32 rowStep, const int32 bytePerPixel)
{
    int32 xRound, yRound;
    float rowFrac, colFrac;
    T row1;
    T row2;




    if (y > height - 1)
      y = (float)(height - 1);

    if (x > width - 1)
      x = (float)(width - 1);

    xRound = static_cast<int32>(x);
    yRound = static_cast<int32>(y);

    rowFrac = 1.f - (y - (float) yRound);
    colFrac = 1.f - (x - (float) xRound);

    const int32 offset_pix = (xRound) * bytePerPixel + (yRound) * rowStep;
    const int32 offset_pix_E = (xRound+1) * bytePerPixel + (yRound) * rowStep;   //voisin est
    const int32 offset_pix_S = (xRound) * bytePerPixel + (yRound+1) * rowStep;   //voisin sud
    const int32 offset_pix_SE = (xRound+1) * bytePerPixel + (yRound+1) * rowStep;   //voisin sud est

    const T& pix = *((T*)(&data[offset_pix]));
    const T& pix_E = *((T*)(&data[offset_pix_E]));
    const T& pix_S = *((T*)(&data[offset_pix_S]));
    const T& pix_SE = *((T*)(&data[offset_pix_SE]));

    if (colFrac < 1)
    {
        row1 = pix * colFrac + pix_E * (1.0f - colFrac);
    }
    else
    {
      row1 = pix;
    }

    if (rowFrac < 1)
    {
      if (colFrac < 1)
      {
        row2 = pix_S * colFrac + pix_SE * (1.f - colFrac);
      }
      else
      {
        row2 = pix_S;
      }
    }

    return (T)( row1 * rowFrac +  row2 * (1.0f - rowFrac));
}

const Pixel Image::getSubPixel(const EImageCoord_t coord, const float x, const float y) const
{
    Pixel ret;
    float posX, posY;

    switch(coord)
    {
        case COORD_PIXEL:
            posX = x;
            posY = y;
        break;
        case COORD_NORM:

            posX = Core::clamp(0.f, 1.f, x) * _width;
            posY = Core::clamp(0.f, 1.f, y) * _height;
        break;
    }

    switch(getImgType())
    {
        case Core::GRAY_F32:
            ret.as_grayF32 = getSubPixel_T<grayF32>(posX, posY,
                            _data, _width, _height, _rowStep, getBytePerPixel());
        break;

        case Core::RGB_F32:
            ret.as_rgbF32 = getSubPixel_T<rgbF32>(posX, posY,
                            _data, _width, _height, _rowStep, getBytePerPixel());
        break;

        case Core::RGB_U8:
            ret.as_rgbU8 = getSubPixel_T<rgbU8>(posX, posY,
                            _data, _width, _height, _rowStep, getBytePerPixel());
        break;

        case Core::ARGB_U8:
            ret.as_argbU8 = getSubPixel_T<argbU8>(posX, posY,
                            _data, _width, _height, _rowStep, getBytePerPixel());
        break;

        case Core::GRAY_U8:
        case Core::GRAY_U16:
        case Core::RGB_U16:
        case Core::ARGB_U16:
        case Core::GRAY_S16:
        case Core::RGB_S16:
        case Core::ARGB_S16:
        case Core::GRAY_S32:
        case Core::RGB_S32:
        case Core::ARGB_S32:
        case Core::ARGB_F32:
            //ERR <<L"getSubPixel not implemented";
        break;
    }

    return ret;
}
//-----------------------------------------------------------------------------
template <class T>
inline void setPixel_T(int32 x, int32 y, const T color,
                       byte* data, const int32 width, const int32 height, const int32 rowStep, const int32 bytePerPixel)
{
    const int32 offset_pix = x * bytePerPixel + y * rowStep;
    T& pix = *((T*)(&data[offset_pix]));
    pix = color;
}

void Image::setPixel(const EImageCoord_t coord, const float x, const float y, const Pixel pix)
{
    int32 posX, posY;

    switch(coord)
    {
        case COORD_PIXEL:
            posX = Core::L_round(x);
            posY = Core::L_round(y);
        break;
        case COORD_NORM:
            posX = Core::L_round(Core::clamp(0.f, 1.f, x) * (_width-1));
            posY = Core::L_round(Core::clamp(0.f, 1.f, y) * (_height-1));
        break;
    }

    switch(getImgType())
    {
        case Core::GRAY_U8:
             setPixel_T<grayU8>(posX, posY, pix.as_grayU8,
                            _data, _width, _height, _rowStep, getBytePerPixel());
        break;


        case Core::RGB_U8:
        case Core::ARGB_U8:
        case Core::GRAY_U16:
        case Core::RGB_U16:
        case Core::ARGB_U16:
        case Core::GRAY_S16:
        case Core::RGB_S16:
        case Core::ARGB_S16:
        case Core::GRAY_S32:
        case Core::RGB_S32:
        case Core::ARGB_S32:
        case Core::GRAY_F32:
        case Core::RGB_F32:
        case Core::ARGB_F32:
            //ERR <<L"setPixel not implemented";
        break;
    }
}
//-----------------------------------------------------------------------------
bool Image::operator == (const Image & image) const
{
    if (getWidth() != image.getWidth() ||
        getHeight() != image.getHeight() ||
        getImgType() != image.getImgType())
    {
        return false;
    }

    const byte* buf1 = getData();
    const byte* buf2 = image.getData();
    for (int iY=0; iY<getHeight(); iY++)
    {
        if (memcmp(&buf1[iY*_rowStep], &buf2[iY*image._rowStep], getWidth()*getBytePerPixel()) != 0)
            return false;
    }

    return true;
}
//-----------------------------------------------------------------------------
bool Image::operator != (const Image & image) const
{
    return !(*this == image);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// La classe View
//-----------------------------------------------------------------------------
View::View(const Ptr<Image>& pImage):
x(0),
y(0),
width(pImage->getWidth()),
height(pImage->getHeight()),
pImg(pImage)
{
}
//-----------------------------------------------------------------------------
View::View(const Ptr<Image>& pImage, const int32 x, const int32 y, const int32 width, const int32 height):
    x(x),
    y(y),
    width(width),
    height(height),
    pImg(pImage)
{

}
//-----------------------------------------------------------------------------
View::View(const Ptr<Image>& pImage, const EImageCoord_t typeCoord, const float other_x, const float other_y, const float other_width, float other_height)
{
    switch(typeCoord)
    {
        case COORD_PIXEL:
            x = (int32)other_x;
            y = (int32)other_y;
            width = (int32)other_width;
            height = (int32)other_height;
            pImg = pImage;
        break;

        case COORD_NORM:
            /*x = (int32)roundf(Core::clamp(0.f, 1.f, other_x) * (pImage->getWidth()-1));
            y = (int32)roundf(Core::clamp(0.f, 1.f, other_y) * (pImage->getHeight()-1));
            width = (int32)roundf(Core::clamp(0.f, 1.f, other_width) * pImage->getWidth());
            height = (int32)roundf(Core::clamp(0.f, 1.f, other_height) * pImage->getHeight());*/

            x = Core::L_round(other_x * (pImage->getWidth()));
            y = Core::L_round(other_y * (pImage->getHeight()));
            width = Core::L_round(other_width * pImage->getWidth());
            height = Core::L_round(other_height * pImage->getHeight());
            pImg = pImage;
        break;
    }
}
//-----------------------------------------------------------------------------
void View::move(const int32 offsetX, const int32 offsetY)
{
    if (x+offsetX<0 || x+offsetX>pImg->getWidth() ||
        y+offsetY<0 || y+offsetY>pImg->getHeight() )
    {
        //TODO gerer l'erreur
    }

    x += offsetX;
    y += offsetY;
}
//-----------------------------------------------------------------------------
void View::moveTo(const int32 posX, const int32 posY)
{
   /* if (x+offsetX<0 || x+offsetX>img->getWidth() ||
        y+offsetY<0 || y+offsetY>img->getHeight() )
    {
        //TODO gerer l'erreur
    }*/

    x = posX;
    y = posY;
}
//-----------------------------------------------------------------------------
const Pixel View::getSubPixel(const EImageCoord_t coord, const float other_x, const float other_y) const
{
    float posX, posY;

    switch(coord)
    {
        case COORD_PIXEL:
            posX = other_x + x;
            posY = other_y + y;
        break;
        case COORD_NORM:

            posX = x + other_x*width;
            posY = y + other_y*height;
        break;
    }

    return pImg->getSubPixel(COORD_PIXEL, posX, posY);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Les fonctions makeIterator
//-----------------------------------------------------------------------------
template <class Pixel_t>
void makeIterator_T(View& view, Iterator2D<Pixel_t >& itPix)
{
    itPix._firstRow = (Pixel_t*)(view.pImg->getData() + view.x*view.getBytePerPixel() + view.y*view.getRowStep());
    itPix._byteRowStep = view.getRowStep();
    itPix._lastRow = (Pixel_t*)((byte*)(itPix._firstRow) + view.height*view.getRowStep());

    itPix._itPixel = itPix._firstRow;
    itPix._rowBegin = itPix._firstRow;
    itPix._rowEnd = (Pixel_t*)((byte*)(itPix._rowBegin) + view.width*view.getBytePerPixel());
}
//-----------------------------------------------------------------------------
template <class Pixel_t>
void makeIterator_T(const Image& img, Iterator2D<Pixel_t >& itPix)
{
    itPix._firstRow = (Pixel_t*)(img.getData());
    itPix._byteRowStep = img.getRowStep();
    itPix._lastRow = (Pixel_t*)((byte*)(itPix._firstRow) + img.getHeight()*img.getRowStep());

    itPix._itPixel = itPix._firstRow;
    itPix._rowBegin = itPix._firstRow;
    itPix._rowEnd = (Pixel_t*)((byte*)(itPix._rowBegin) + img.getWidth()*img.getBytePerPixel());
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayU8& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_rgbU8& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_argbU8& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayU8& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbU8& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbU8& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_grayU8& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_rgbU8& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_argbU8& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayU16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_rgbU16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_argbU16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayU16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbU16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbU16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(const Image& img, it2D_grayU16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_rgbU16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_argbU16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayS16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_rgbS16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_argbS16& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayS16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbS16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbS16& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_grayS16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_rgbS16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_argbS16& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayS32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_rgbS32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_argbS32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayS32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbS32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbS32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_grayS32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_rgbS32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_argbS32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_grayF32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_rgbF32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D_argbF32& itPix)
{
    makeIterator_T(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_grayF32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_rgbF32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D_argbF32& itPix)
{
    View view(pImg);
    makeIterator(view, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_grayF32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_rgbF32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D_argbF32& itPix)
{
    makeIterator_T(img, itPix);
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(View& view, it2D& itPix)
{
    Core::EBitmapFormat type = view.getImgType();

    switch(type)
    {
        case Core::GRAY_U8: makeIterator(view, itPix.as_grayU8); break;
        case Core::RGB_U8: makeIterator(view, itPix.as_rgbU8); break;
        case Core::ARGB_U8: makeIterator(view, itPix.as_argbU8); break;
        case Core::GRAY_U16: makeIterator(view, itPix.as_grayU16); break;
        case Core::RGB_U16: makeIterator(view, itPix.as_rgbU16); break;
        case Core::ARGB_U16: makeIterator(view, itPix.as_argbU16); break;
        case Core::GRAY_S16: makeIterator(view, itPix.as_grayS16); break;
        case Core::RGB_S16: makeIterator(view, itPix.as_rgbS16); break;
        case Core::ARGB_S16: makeIterator(view, itPix.as_argbS16); break;
        case Core::GRAY_S32: makeIterator(view, itPix.as_grayS32); break;
        case Core::RGB_S32: makeIterator(view, itPix.as_rgbS32); break;
        case Core::ARGB_S32: makeIterator(view, itPix.as_argbS32); break;
        case Core::GRAY_F32: makeIterator(view, itPix.as_grayF32); break;
        case Core::RGB_F32: makeIterator(view, itPix.as_rgbF32); break;
        case Core::ARGB_F32: makeIterator(view, itPix.as_argbF32); break;
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Ptr<Image>& pImg, it2D& itPix)
{
    Core::EBitmapFormat type = pImg->getImgType();

    switch(type)
    {
        case Core::GRAY_U8: makeIterator(pImg, itPix.as_grayU8); break;
        case Core::RGB_U8: makeIterator(pImg, itPix.as_rgbU8); break;
        case Core::ARGB_U8: makeIterator(pImg, itPix.as_argbU8); break;
        case Core::GRAY_U16: makeIterator(pImg, itPix.as_grayU16); break;
        case Core::RGB_U16: makeIterator(pImg, itPix.as_rgbU16); break;
        case Core::ARGB_U16: makeIterator(pImg, itPix.as_argbU16); break;
        case Core::GRAY_S16: makeIterator(pImg, itPix.as_grayS16); break;
        case Core::RGB_S16: makeIterator(pImg, itPix.as_rgbS16); break;
        case Core::ARGB_S16: makeIterator(pImg, itPix.as_argbS16); break;
        case Core::GRAY_S32: makeIterator(pImg, itPix.as_grayS32); break;
        case Core::RGB_S32: makeIterator(pImg, itPix.as_rgbS32); break;
        case Core::ARGB_S32: makeIterator(pImg, itPix.as_argbS32); break;
        case Core::GRAY_F32: makeIterator(pImg, itPix.as_grayF32); break;
        case Core::RGB_F32: makeIterator(pImg, itPix.as_rgbF32); break;
        case Core::ARGB_F32: makeIterator(pImg, itPix.as_argbF32); break;
    }
}
//-----------------------------------------------------------------------------
LM_API_IMG void makeIterator(Image& img, it2D& itPix)
{
    Core::EBitmapFormat type = img.getImgType();

    switch(type)
    {
        case Core::GRAY_U8: makeIterator(img, itPix.as_grayU8); break;
        case Core::RGB_U8: makeIterator(img, itPix.as_rgbU8); break;
        case Core::ARGB_U8: makeIterator(img, itPix.as_argbU8); break;
        case Core::GRAY_U16: makeIterator(img, itPix.as_grayU16); break;
        case Core::RGB_U16: makeIterator(img, itPix.as_rgbU16); break;
        case Core::ARGB_U16: makeIterator(img, itPix.as_argbU16); break;
        case Core::GRAY_S16: makeIterator(img, itPix.as_grayS16); break;
        case Core::RGB_S16: makeIterator(img, itPix.as_rgbS16); break;
        case Core::ARGB_S16: makeIterator(img, itPix.as_argbS16); break;
        case Core::GRAY_S32: makeIterator(img, itPix.as_grayS32); break;
        case Core::RGB_S32: makeIterator(img, itPix.as_rgbS32); break;
        case Core::ARGB_S32: makeIterator(img, itPix.as_argbS32); break;
        case Core::GRAY_F32: makeIterator(img, itPix.as_grayF32); break;
        case Core::RGB_F32: makeIterator(img, itPix.as_rgbF32); break;
        case Core::ARGB_F32: makeIterator(img, itPix.as_argbF32); break;
    }
}
//-----------------------------------------------------------------------------
} // namespace Images

#pragma warning( pop )

