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

#include "Texture.h"
#include <Core/Logger.h>
namespace Assets
{
struct DDSHeader
{
    static const int32 DDS_MAGIC = LM_FOURCC('D', 'D', 'S', ' ');
    static const int32 DDS_SIZE  = 124;
    
    static const int32 FLAG_CAPS        = 0x00000001;
    static const int32 FLAG_HEIGHT      = 0x00000002;
    static const int32 FLAG_WIDTH       = 0x00000004;
    static const int32 FLAG_PITCH       = 0x00000008;
    static const int32 FLAG_PIXELFORMAT = 0x00001000;
    static const int32 FLAG_MIPMAPCOUNT = 0x00020000;
    static const int32 FLAG_LINEAR      = 0x00080000;
    
    struct PixelFormat
    {
        static const int32 PF_SIZE        = 32;
        static const int32 PF_FLAG_RGB    = 0x00000040;
        static const int32 PF_FLAG_FOURCC = 0x00000004;
        static const int32 PF_FLAG_ALPHA  = 0x00000001;

        static const int32 DXT1  = LM_FOURCC('D', 'X', 'T', '1');
        static const int32 DXT3  = LM_FOURCC('D', 'X', 'T', '3');
        static const int32 DXT5  = LM_FOURCC('D', 'X', 'T', '5');
        
        static const int32 RED_MASK   = 0x00FF0000;
        static const int32 GREEN_MASK = 0x0000FF00;
        static const int32 BLUE_MASK  = 0x000000FF;
        static const int32 ALPHA_MASK = 0xFF000000;
        
        int32   size;
        int32   flags;
        int32   fourCC;
        int32   bitCount;
        int32   redMask;
        int32   greenMask;
        int32   blueMask;
        int32   alphaMask;
    };

    struct SurfCaps
    {
        static const int32 FLAG_COMPLEX   = 0x00000008;
        static const int32 FLAG_CUBEMAP   = 0x00000200;
        static const int32 FLAG_TEXTURE   = 0x00001000;
        static const int32 FLAG_MIPMAP    = 0x00400000;
        static const int32 FLAG_POSITIVEX = 0x00000400;
        static const int32 FLAG_NEGATIVEX = 0x00000800;
        static const int32 FLAG_POSITIVEY = 0x00001000;
        static const int32 FLAG_NEGATIVEY = 0x00002000;
        static const int32 FLAG_POSITIVEZ = 0x00004000;
        static const int32 FLAG_NEGATIVEZ = 0x00008000;
        static const int32 FLAG_COMPLETE_CUBEMAP = (
                FLAG_CUBEMAP | FLAG_POSITIVEX | FLAG_POSITIVEY | FLAG_POSITIVEZ |
                FLAG_NEGATIVEX | FLAG_NEGATIVEY | FLAG_NEGATIVEZ
        );
        int32   caps1;
        int32   caps2;
        int32   reserved[2];
    };
    
    int32       size;
    int32       flags;
    int32       height;
    int32       width;
    int32       pitch;
    int32       depth;
    int32       mipmapCount;
    int32       reserved[11];
    PixelFormat format;
    SurfCaps    caps;
    int32       reserved2;
};

LM_STATIC_ASSERT(sizeof(DDSHeader) == DDSHeader::DDS_SIZE);
LM_STATIC_ASSERT(sizeof(DDSHeader::PixelFormat) == DDSHeader::PixelFormat::PF_SIZE);

static const int32 MAGIC_TEXIMAGE   = LM_FOURCC('I', 'M', 'G', '0');
static const int32 MAGIC_TEXTURE    = LM_FOURCC('T', 'E', 'X', '0');
static const int32 MAGIC_CUBETEX    = LM_FOURCC('C', 'U', 'B', '0');

//-----------------------------------------------------------------------------
static int32 getDXT1DataSize(int32 width, int32 height)
{
    return std::max(1, width / 4) * std::max(1, height / 4) * 8;
}
//-----------------------------------------------------------------------------
static int32 getDXTXDataSize(int32 width, int32 height)
{
    return std::max(1, width / 4) * std::max(1, height / 4) * 16;
}
//-----------------------------------------------------------------------------
static int32 getTextureDataSize(int32 width, int32 height, ETextureFormat format)
{
    switch(format)
    {
    case TEX_FORMAT_ARGB8: return width * height * 4;  break;
    case TEX_FORMAT_DXTC1: return getDXT1DataSize(width, height); break;
    case TEX_FORMAT_DXTC3: return getDXTXDataSize(width, height); break;
    case TEX_FORMAT_DXTC5: return getDXTXDataSize(width, height); break;
    }
    return 0;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TextureImage::TextureImage() : _width(0), _height(0), _format(TEX_FORMAT_ARGB8)
{}
//-----------------------------------------------------------------------------
TextureImage::TextureImage(const TextureImage & tex) :
    _width(tex._width), 
    _height(tex._height),
    _format(tex._format),
    _data(tex._data)
{}
//-----------------------------------------------------------------------------
TextureImage::TextureImage(int32 width, int32 height, ETextureFormat format,
                           const void * pData, int32 dataSize)
    : _width(width),
      _height(height),
      _format(format)
{   
    LM_ASSERT(dataSize == getDataSize());
    _data.resize(dataSize);
    memcpy(_data.data(), pData, dataSize);
}
//-----------------------------------------------------------------------------
TextureImage & TextureImage::operator = (const TextureImage & tex)
{
    if(&tex != this)
    {
        _width = tex._width;
        _height = tex._height;
        _format = tex._format;
        _data = tex._data;
    }
    return *this;
}
//-----------------------------------------------------------------------------
int32 TextureImage::getBitsPerPixels() const
{
    int32 result = 0;
    switch(_format)
    {
    case TEX_FORMAT_ARGB8:   result = 32;  break;
/*    case TEX_FORMAT_ARGB16:  result = 64;  break;
    case TEX_FORMAT_ARGB16F: result = 64;  break;
    case TEX_FORMAT_ARGB32F: result = 128; break;*/
    case TEX_FORMAT_DXTC1:   result = 4;   break;
    case TEX_FORMAT_DXTC3:   result = 8;   break;
    case TEX_FORMAT_DXTC5:   result = 8;   break;
    }
    return result;
}
//-----------------------------------------------------------------------------
int32 TextureImage::getDataSize() const 
{
    return getTextureDataSize(_width, _height, _format);
}
//-----------------------------------------------------------------------------
void TextureImage::read(Core::InputStream & input)
{
    int32 magic = input.readInt32();
    if(magic != MAGIC_TEXIMAGE) throw Core::IOException(L"Invalid texture image.");
    input.read(_width);
    input.read(_height);
    _format = ETextureFormat(input.readInt32());
    if(!isValid(_format)) throw Core::IOException(L"Invalid texture image format.");
    int32 size = getDataSize();
    _data.resize(size);
    input.readData(_data.data(), size);
}
//-----------------------------------------------------------------------------
void TextureImage::write(Core::OutputStream & output) const
{
    output.write(MAGIC_TEXIMAGE);
    output.write(_width);
    output.write(_height);
    output.write(int32(_format));
    output.writeData(getData(), getDataSize());
}
//-----------------------------------------------------------------------------
bool TextureImage::operator == (const TextureImage & texture) const
{
    return _width == texture._width && _height == texture._height && 
           _format == texture._format && _data == texture._data;
}
//-----------------------------------------------------------------------------
bool TextureImage::operator != (const TextureImage & texture) const
{
    return !(*this == texture);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
void Texture::readDDS(Core::InputStream & input, Core::List<Texture> & textures)
{
    int32 magic = input.readInt32();
    if(magic != DDSHeader::DDS_MAGIC) 
        throw Core::IOException(L"Invalid DDS header magic.");
    
    DDSHeader  header;
    input.readData(&header, sizeof(header));
    
    if(header.size != DDSHeader::DDS_SIZE) 
        throw Core::IOException(L"Invalid DDS header size.");
    
    ETextureFormat format;
    
    if(header.format.size != DDSHeader::PixelFormat::PF_SIZE) throw Core::IOException(L"Invalid DDS format size.");
    if(header.format.flags & DDSHeader::PixelFormat::PF_FLAG_RGB)
    {
        if(header.format.bitCount != 32) 
            throw Core::IOException(L"24 bits DDS files are not supported.");
        
        format = TEX_FORMAT_ARGB8;
        LM_ASSERT(header.pitch == (header.width * 4));
        LM_ASSERT(header.format.redMask == DDSHeader::PixelFormat::RED_MASK);
        LM_ASSERT(header.format.greenMask == DDSHeader::PixelFormat::GREEN_MASK);
        LM_ASSERT(header.format.blueMask == DDSHeader::PixelFormat::BLUE_MASK);
        LM_ASSERT(header.format.alphaMask == DDSHeader::PixelFormat::ALPHA_MASK);
    }
    else if (header.format.flags & DDSHeader::PixelFormat::PF_FLAG_FOURCC)
    {
        switch(header.format.fourCC)
        {
        case DDSHeader::PixelFormat::DXT1: format = TEX_FORMAT_DXTC1; break;
        case DDSHeader::PixelFormat::DXT3: format = TEX_FORMAT_DXTC3; break;
        case DDSHeader::PixelFormat::DXT5: format = TEX_FORMAT_DXTC5; break;
        default:
            throw Core::IOException(L"Unknown DDS pixel fourCC.");
            break;
        }
    }
    else
    {
        throw Core::IOException(L"Unknown DDS pixel format.");
    }

    if((header.caps.caps2 & DDSHeader::SurfCaps::FLAG_COMPLETE_CUBEMAP) == DDSHeader::SurfCaps::FLAG_COMPLETE_CUBEMAP)
        textures.resize(6);
    else
        textures.resize(1);
    
    for(Core::List<Texture>::iterator iTexture = textures.begin(); iTexture != textures.end(); ++iTexture)
    {
        Core::List<byte> buffer;
        iTexture->_format = format;
        int32 mipWidth = iTexture->_width = header.width;
        int32 mipHeight = iTexture->_height = header.height;
        
        if(header.flags & DDSHeader::FLAG_MIPMAPCOUNT)
            iTexture->_images.resize(header.mipmapCount);
        else
            iTexture->_images.resize(1);
        
        for(Core::List<Core::List<TextureImage> >::iterator iMipmap = iTexture->_images.begin(); 
            iMipmap != iTexture->_images.end(); 
            ++iMipmap)
        {
            for(Core::List<TextureImage>::iterator iImage= iMipmap->begin(); 
            iImage != iMipmap->end(); 
            ++iImage)
            {
                buffer.resize(getTextureDataSize(mipWidth, mipHeight, format));
                input.readData(buffer.data(), buffer.size());
                *iImage = TextureImage(mipWidth, mipHeight, format, buffer.data(), buffer.size());
                mipWidth = std::max(1, mipWidth / 2);
                mipHeight = std::max(1, mipHeight / 2);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void Texture::writeDDS(Core::OutputStream & output, const Core::List<Texture> & textures)
{
    LM_ASSERT(textures.size() >= 1);
    
    int32 width = textures[0]._width;
    int32 height = textures[0]._height;
    ETextureFormat format = textures[0]._format;
    
    DDSHeader  header;
    memset(&header, 0, sizeof(header));
    header.size = DDSHeader::DDS_SIZE;
    header.flags = DDSHeader::FLAG_CAPS | DDSHeader::FLAG_HEIGHT | DDSHeader::FLAG_WIDTH | DDSHeader::FLAG_PIXELFORMAT;
    header.width = width;
    header.height = height;
    header.depth = 0;
    header.mipmapCount = textures[0]._images.size();

    header.format.size = DDSHeader::PixelFormat::PF_SIZE;
    switch(format)
    {
    case TEX_FORMAT_ARGB8:
        header.flags |= DDSHeader::FLAG_PITCH;
        header.pitch = width * 4; 
        header.format.bitCount  = 32;
        header.format.flags     = DDSHeader::PixelFormat::PF_FLAG_RGB | DDSHeader::PixelFormat::PF_FLAG_ALPHA;
        header.format.redMask   = DDSHeader::PixelFormat::RED_MASK;
        header.format.greenMask = DDSHeader::PixelFormat::GREEN_MASK;
        header.format.blueMask  = DDSHeader::PixelFormat::BLUE_MASK;
        header.format.alphaMask = DDSHeader::PixelFormat::ALPHA_MASK;
        break;
    case TEX_FORMAT_DXTC1:  
        header.flags |= DDSHeader::FLAG_LINEAR;
        header.pitch = getDXT1DataSize(width, height);
        header.format.flags  = DDSHeader::PixelFormat::PF_FLAG_FOURCC;
        header.format.fourCC = DDSHeader::PixelFormat::DXT1;
        break;
    case TEX_FORMAT_DXTC3: 
        header.flags |= DDSHeader::FLAG_LINEAR;
        header.pitch = getDXTXDataSize(width, height); 
        header.format.flags  = DDSHeader::PixelFormat::PF_FLAG_FOURCC;
        header.format.fourCC = DDSHeader::PixelFormat::DXT3;
        break;
    case TEX_FORMAT_DXTC5:  
        header.flags |= DDSHeader::FLAG_LINEAR;
        header.pitch = getDXTXDataSize(width, height); 
        header.format.flags  = DDSHeader::PixelFormat::PF_FLAG_FOURCC;
        header.format.fourCC = DDSHeader::PixelFormat::DXT5;
        break;
    }
    
    header.caps.caps1 = DDSHeader::SurfCaps::FLAG_TEXTURE;
    if(textures[0]._images.size() > 1)
    {
        header.flags |= DDSHeader::FLAG_MIPMAPCOUNT;
        header.caps.caps1 |= DDSHeader::SurfCaps::FLAG_MIPMAP | DDSHeader::SurfCaps::FLAG_COMPLEX;
    }
    if(textures.size() == ECubeFace_SIZE)
    {
        header.caps.caps2 |= DDSHeader::SurfCaps::FLAG_COMPLETE_CUBEMAP;
    }

    output.write(DDSHeader::DDS_MAGIC);
    output.writeData(&header, sizeof(header));

    for(Core::List<Texture>::const_iterator iTexture = textures.begin(); iTexture != textures.end(); ++iTexture)
    {
        for(Core::List<Core::List<TextureImage> >::const_iterator iMipmap = iTexture->_images.begin(); 
            iMipmap != iTexture->_images.end(); 
            ++iMipmap)
        {
            for(Core::List<TextureImage>::const_iterator iImage = iMipmap->begin(); 
                iImage != iMipmap->end(); 
                ++iImage)
            {
                output.writeData(iImage->getData(), iImage->getDataSize());
            }
        }
    }
}
//-----------------------------------------------------------------------------
Texture::Texture() : _width(0), _height(0), _depth(0), _flags(0), _format(TEX_FORMAT_ARGB8)
{}
//-----------------------------------------------------------------------------
Texture::Texture(const Texture & tex) :
    _width(tex._width), 
    _height(tex._height),
    _depth(tex._depth),
    _flags(tex._flags),
    _format(tex._format),
    _images(tex._images)
{}
//-----------------------------------------------------------------------------
Texture::Texture(const Core::List<TextureImage> & images)
    : _width(0), _height(0), _depth(0), _flags(0), _format(TEX_FORMAT_ARGB8)
{
    _images.resize(images.size());
    for (int32 iMipmap=0; iMipmap<images.size(); ++iMipmap)
    {
        _images[iMipmap].resize(1);
        _images[iMipmap][0] = images[iMipmap];
    }

    LM_ASSERT(images.size() > 0);
    if(images.size() > 0)
    {
        _width  = images[0].getWidth();
        _height = images[0].getHeight();
        _depth  = 1;
        _format = images[0].getFormat();
    }
    
    int32 width = _width;
    int32 height = _height;
    
    for(Core::List<Core::List<TextureImage> >::const_iterator iMipmap =  _images.begin(); iMipmap != _images.end(); ++iMipmap)
    {
        LM_ASSERT((*iMipmap)[0].getFormat() == _format);
        LM_ASSERT((*iMipmap)[0].getWidth() == width);
        LM_ASSERT((*iMipmap)[0].getHeight() == height);
        width  = std::max(1, width  >> 1);
        height = std::max(1, height >> 1);
    }
}
//-----------------------------------------------------------------------------
Texture::Texture(const Core::List<Core::List<TextureImage> > & images)
    : _width(0), _height(0), _depth(0), _flags(0), _format(TEX_FORMAT_ARGB8), _images(images)
{
    LM_ASSERT(images.size() > 0);
    if(images.size() > 0)
    {
        _width  = images[0][0].getWidth();
        _height = images[0][0].getHeight();
        _depth  = images[0].size();
        _format = images[0][0].getFormat();
    }
    
    int32 width = _width;
    int32 height = _height;
    
    for(int32 iMipmap=0; iMipmap < images.size(); iMipmap++)
    {
        for(int32 iSlice=0; iSlice < _depth; iSlice++)
        {
            LM_ASSERT(_images[iMipmap][iSlice].getFormat() == _format);
            LM_ASSERT(_images[iMipmap][iSlice].getWidth() == width);
            LM_ASSERT(_images[iMipmap][iSlice].getHeight() == height);
        }

        width  = std::max(1, width  >> 1);
        height = std::max(1, height >> 1);
    }
}
//-----------------------------------------------------------------------------
Texture & Texture::operator = (const Texture & tex)
{
    if(&tex != this)
    {
        _width = tex._width;
        _height = tex._height;
        _depth = tex._depth;
        _flags = tex._flags;
        _format = tex._format;
        _images = tex._images;
    }
    return *this;
}
//-----------------------------------------------------------------------------
const Core::List<TextureImage> & Texture::getImage(int32 level) const
{
    LM_ASSERT(level >= 0 && level < (int32)_images.size());
    return _images[level];
}
//-----------------------------------------------------------------------------
void Texture::read(Core::InputStream & input)
{
    int32 magic = input.readInt32();
    if(magic != MAGIC_TEXTURE)
        throw Core::IOException(L"Invalid texture.");
    
    input.read(_width);
    input.read(_height);
    input.read(_depth);
    input.read(_flags);

    _format = ETextureFormat(input.readInt32());
    if(!isValid(_format)) throw Core::IOException(L"Invalid texture format.");
    int32 mipmapCount = input.readInt32();
    _images.resize(mipmapCount);

    int32 depth = _depth;

    for(int32 iMipmap=0; iMipmap < _images.size(); iMipmap++)
    {
        depth = input.readInt32();
        LM_ASSERT(depth <= _depth);

        _images[iMipmap].resize(depth);

        for(int32 iImage=0; iImage < _images[iMipmap].size(); iImage++)
            _images[iMipmap][iImage].read(input);
    }
    
    int32 width  = _width;
    int32 height = _height;

    for(int32 iMipmap=0; iMipmap < _images.size(); iMipmap++)
    {
        for(int32 iImage=0; iImage < _images[iMipmap].size(); iImage++)
        {
            if(_images[iMipmap][iImage].getFormat() != _format) throw Core::IOException(L"Incoherent texture mipmap format.");
            if(_images[iMipmap][iImage].getWidth()  != width)   throw Core::IOException(L"Incoherent texture mipmap width.");
            if(_images[iMipmap][iImage].getHeight() != height)  throw Core::IOException(L"Incoherent texture mipmap height.");
        }

        width  = std::max(1, width  >> 1);
        height = std::max(1, height >> 1);
    }
}
//-----------------------------------------------------------------------------
void Texture::readDDS(Core::InputStream & input)
{
    Core::List<Texture> textures;
    readDDS(input, textures);
    LM_ASSERT(textures.size() >= 1);
    *this = textures[0];
}
//-----------------------------------------------------------------------------
void Texture::writeDDS(Core::OutputStream & output) const
{
    Core::List<Texture> textures;
    textures.push_back(*this);
    writeDDS(output, textures);
}
//-----------------------------------------------------------------------------
void Texture::write(Core::OutputStream & output) const
{
    output.write(MAGIC_TEXTURE);
    output.write(_width);
    output.write(_height);
    output.write(_depth);
    output.write(_flags);
    output.write(int32(_format));
    output.write(getMipmapCount());
    
    for(int32 iMipmap=0; iMipmap < _images.size(); iMipmap++)
    {
        output.write(_images[iMipmap].size());
        for(int32 iImage=0; iImage < _images[iMipmap].size(); iImage++)
            _images[iMipmap][iImage].write(output);
    }
    
}
//-----------------------------------------------------------------------------
bool Texture::operator == (const Texture & texture) const
{
    return _width == texture._width && _height == texture._height && 
           _format == texture._format && _images == texture._images;
}
//-----------------------------------------------------------------------------
bool Texture::operator != (const Texture & texture) const
{
    return !(*this == texture);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
CubeTexture::CubeTexture() :  _width(0), _height(0), _flags(0), _format(TEX_FORMAT_ARGB8), 
    _mipmapCount(0)
{}
//-----------------------------------------------------------------------------
CubeTexture::CubeTexture(const CubeTexture & tex) :
    _width(tex._width), 
    _height(tex._height),
    _format(tex._format),
    _flags(tex._flags), 
    _faces(tex._faces)
{    
}
//-----------------------------------------------------------------------------
CubeTexture::CubeTexture(const Core::List<Texture> & faces) :
    _width(0), 
    _height(0), 
    _format(TEX_FORMAT_ARGB8), 
    _mipmapCount(0),
    _flags(0),
    _faces(faces)
{
    LM_ASSERT(faces.size() == ECubeFace_SIZE);
    _width = faces[0].getWidth();
    _height = faces[0].getHeight();
    _format = faces[0].getFormat();
    _mipmapCount = faces[0].getMipmapCount();
}
//-----------------------------------------------------------------------------
const Core::List<TextureImage> & CubeTexture::getImage(ECubeFace face, int32 level) const
{
    LM_ASSERT(isValid(face));
    LM_ASSERT(level < _mipmapCount);
    return _faces[int32(face)].getImage(level);
}
//-----------------------------------------------------------------------------
void CubeTexture::read(Core::InputStream & input)
{
    int32 magic = input.readInt32();
    if(magic != MAGIC_CUBETEX) throw Core::IOException(L"Invalid cube texture.");
    input.read(_width);
    input.read(_height);
    input.read(_flags);
    _format = ETextureFormat(input.readInt32());
    if(!isValid(_format)) throw Core::IOException(L"Invalid cube texture format.");
    input.read(_mipmapCount);
    
    _faces.resize(ECubeFace_COUNT);

    for(Core::List<Texture>::iterator iFace = _faces.begin(); iFace != _faces.end(); ++iFace)
    {
        iFace->read(input);
        if(iFace->getFormat() != _format) throw Core::IOException(L"Incoherent cube texture face format.");
        if(iFace->getWidth()  != _width)  throw Core::IOException(L"Incoherent cube texture face width.");
        if(iFace->getHeight() != _height) throw Core::IOException(L"Incoherent cube texture face height.");
    }
}
//-----------------------------------------------------------------------------
void CubeTexture::write(Core::OutputStream & output) const
{
    output.write(MAGIC_CUBETEX);
    output.write(_width);
    output.write(_height);
    output.write(_flags);
    output.write(int32(_format));
    output.write(_mipmapCount);
    
    for(Core::List<Texture>::const_iterator iFace = _faces.begin(); iFace != _faces.end(); ++iFace)
        iFace->write(output);
}
//-----------------------------------------------------------------------------
void CubeTexture::readDDS(Core::InputStream & input)
{
    _faces.clear();
    Texture::readDDS(input, _faces);
    if(_faces.size() != ECubeFace_SIZE) throw Core::IOException(L"Invalid cube texture face count.");
    _width = _faces[0].getWidth();
    _height = _faces[0].getHeight();
    _format = _faces[0].getFormat();
    _mipmapCount = _faces[0].getMipmapCount();
}
//-----------------------------------------------------------------------------
void CubeTexture::writeDDS(Core::OutputStream & output) const
{
    if(_faces.size() != ECubeFace_SIZE) throw Core::IOException(L"Invalid cube texture write.");
    Texture::writeDDS(output, _faces);
}
//-----------------------------------------------------------------------------
CubeTexture & CubeTexture::operator = (const CubeTexture & tex)
{
    if(this != &tex)
    {
        _width = tex._width;
        _height = tex._height;
        _flags = tex._flags;
        _format = tex._format;
        _mipmapCount = tex._mipmapCount;
        _faces = tex._faces;
    }
    return *this;
}
//-----------------------------------------------------------------------------
bool CubeTexture::operator == (const CubeTexture & texture) const
{
    return _width == texture._width && _height == texture._height && _flags == texture._flags &&
           _format == texture._format && _faces == texture._faces;        
}
//-----------------------------------------------------------------------------
bool CubeTexture::operator != (const CubeTexture & texture) const
{
    return !(*this == texture);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
}
