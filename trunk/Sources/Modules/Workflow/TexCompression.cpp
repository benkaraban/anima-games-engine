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

#include <Core/Logger.h>
#include <Core/IO/FileStream.h>
#include <Core/Math/MathTools.h>
#include <Images/Operator.h>
#define NVTT_SHARED 1
#include <nvtt.h>
#include "TexCompression.h"

namespace Workflow
{
const int32 INVALID_LEVEL = -1;

struct LOADEDErrorHandler : public nvtt::ErrorHandler
{
    virtual void error(nvtt::Error e)
    {
        String message(L"Error in NVTT library : ");
        ERR << String(String8(nvtt::errorString(e))) << L"\n";
    };
};

struct LOADEDOutputHandler : public nvtt::OutputHandler
{
    LOADEDOutputHandler(Core::List<Core::List<Assets::TextureImage> > & rTextures,
                        Assets::ETextureFormat format,
                        Core::IProgressObs * pProgressObs,
                        int32 totalSize) :
        _rTextures(rTextures),
        _currentLevel(INVALID_LEVEL),
        _currentWidth(0),
        _currentHeight(0),
        _currentFace(0),
        _format(format),
        _pProgressObs(pProgressObs),
        _totalWritten(0),
        _totalSize(totalSize)
    {
        _rTextures.clear();
    };

    virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
    {
        finishImage();
        _buffer.clear();
        _currentFace = face;
        int32 newFaceSize = std::max(int(_rTextures.size()), face + 1);
        _rTextures.resize(newFaceSize);
        _currentWidth = width;
        _currentHeight = height;
        _currentLevel = miplevel;
        int32 newMipSize = std::max(int(_rTextures[_currentFace].size()), miplevel + 1);
        _rTextures[_currentFace].resize(newMipSize);
    };

    virtual bool writeData(const void * data, int size)
    {
        const byte * pData = (const byte*)data;
        _buffer.insert(_buffer.end(), &pData[0], &pData[size]);

        _totalWritten = std::min(_totalSize, _totalWritten + size);
        
        if(_pProgressObs != NULL)
            _pProgressObs->progress(float(_totalWritten) / float(_totalSize));

        return true;
    };

    void finishImage() { flush(); };

    void flush()
    {
        if(_currentLevel != INVALID_LEVEL)
        {
            _rTextures[_currentFace][_currentLevel] = Assets::TextureImage(
                    _currentWidth,
                    _currentHeight,
                    _format,
                    _buffer.data(),
                    _buffer.size()
            );
            _currentLevel = INVALID_LEVEL;
        }
    };

protected:
    Core::List<Core::List<Assets::TextureImage> >  & _rTextures;
    Core::List<byte>        _buffer;
    int32                   _currentLevel;
    int32                   _currentWidth;
    int32                   _currentHeight;
    int32                   _currentFace;
    Assets::ETextureFormat  _format;
    
    Core::IProgressObs *    _pProgressObs;
    int32                   _totalWritten;
    int32                   _totalSize;
};

//-----------------------------------------------------------------------------
void generateTexture(const TextureOptions & options,
                            const Core::List<Ptr<Images::Image> > & images,
                            Core::List<Core::List<Assets::TextureImage> > & texImages)
{
    LM_ASSERT(images.size() == 1 || images.size() == 6);
    int32 width = Core::nextPowerOfTwo(images[0]->getWidth());
    int32 height = Core::nextPowerOfTwo(images[0]->getHeight());
    nvtt::TextureType  textureType = (images.size() == 6) ? nvtt::TextureType_Cube : nvtt::TextureType_2D;
    nvtt::InputOptions inputOptions;
    Ptr<Images::Image> pImageCopy;

    inputOptions.setTextureLayout(textureType, width, height);

    for(int32 face=0; face < images.size(); face++)
    {
        const Ptr<Images::Image> & pImage = images[face];
        LM_ASSERT(pImage->getImgType() == Core::ARGB_U8);

        if(pImage->getWidth() == width && pImage->getHeight() == height)
        {
            inputOptions.setMipmapData(pImage->getData(), width, height, 1, face);
        }
        else
        {
            if(pImageCopy == null)
            {
                pImageCopy = Ptr<Images::Image>(new Images::Image(pImage->getImgType(), width, height));
            }
            Images::resize(pImage, pImageCopy);
            
            inputOptions.setMipmapData(pImageCopy->getData(), width, height, 1, face);
        }
    }

    inputOptions.setWrapMode(nvtt::WrapMode_Clamp);
    inputOptions.setFormat(nvtt::InputFormat_BGRA_8UB);
    inputOptions.setMipmapGeneration(options.generateMipmap);

    switch(options.mipmapFilter)
    {
    case MIPMAP_FILTER_BOX:      inputOptions.setMipmapFilter(nvtt::MipmapFilter_Box); break;
    case MIPMAP_FILTER_TRIANGLE: inputOptions.setMipmapFilter(nvtt::MipmapFilter_Triangle); break;
    case MIPMAP_FILTER_KAISER:   inputOptions.setMipmapFilter(nvtt::MipmapFilter_Kaiser);
        inputOptions.setKaiserParameters(options.kaiserWidth, options.kaiserAlpha, options.kaiserStretch);
        break;
    }

    switch(options.texType)
    {
    case TEXTURE_COLORMAP:
        inputOptions.setGamma(2.2f, 2.2f);
        inputOptions.setNormalMap(false);
        inputOptions.setConvertToNormalMap(false);
        break;
    case TEXTURE_LIGHTMAP:
        inputOptions.setGamma(1.0f, 1.0f);
        inputOptions.setNormalMap(false);
        inputOptions.setConvertToNormalMap(false);
        break;
    case TEXTURE_HEIGHTMAP:
        inputOptions.setGamma(1.0f, 1.0f);
        inputOptions.setHeightEvaluation(0.0f, 1.0f, 0.0f, 0.0f);
        inputOptions.setNormalMap(true);
        inputOptions.setConvertToNormalMap(true);
        break;
    case TEXTURE_NORMALMAP:
        inputOptions.setGamma(1.0f, 1.0f);
        inputOptions.setNormalMap(true);
        inputOptions.setConvertToNormalMap(false);
        break;
    }

    nvtt::CompressionOptions compOptions;

    switch(options.quality)
    {
    case QUALITY_LOW:  compOptions.setQuality(nvtt::Quality_Fastest); break;
    case QUALITY_HIGH: compOptions.setQuality(nvtt::Quality_Highest); break;
    }


    switch(options.format)
    {
    case Assets::TEX_FORMAT_ARGB8: compOptions.setFormat(nvtt::Format_RGBA); break;
    case Assets::TEX_FORMAT_DXTC1: compOptions.setFormat(nvtt::Format_DXT1); break;
    case Assets::TEX_FORMAT_DXTC3: compOptions.setFormat(nvtt::Format_DXT3); break;
    case Assets::TEX_FORMAT_DXTC5:
        if(options.texType == TEXTURE_HEIGHTMAP || options.texType == TEXTURE_NORMALMAP)
            compOptions.setFormat(nvtt::Format_DXT5n);
        else
            compOptions.setFormat(nvtt::Format_DXT5);
        break;
    /*
    case Assets::TEX_FORMAT_ARGB16:
    case Assets::TEX_FORMAT_ARGB16F:
    case Assets::TEX_FORMAT_ARGB32F:
        LM_ASSERT(false); // Not implemented
        break;
        */
    }

    texImages.clear();

    nvtt::Compressor    compressor;
    nvtt::OutputOptions outputOptions;
    LOADEDErrorHandler  errorHandler;
    LOADEDOutputHandler outputHandler(texImages, options.format, options.pProgressObs, 
                                      compressor.estimateSize(inputOptions, compOptions));

    outputOptions.setOutputHeader(true);
    outputOptions.setOutputHandler(&outputHandler);
    outputOptions.setErrorHandler(&errorHandler);

    compressor.enableCudaAcceleration(false);
    compressor.process(inputOptions, compOptions, outputOptions);
    outputHandler.flush();
}
//-----------------------------------------------------------------------------
Ptr<Assets::Texture> generateTexture(const TextureOptions & options, const Ptr<Images::Image> & pImage)
{
    Core::List<Core::List<Assets::TextureImage> > texImages;
    Core::List<Ptr<Images::Image> > images;
    images.push_back(pImage);
    generateTexture(options, images, texImages);
    LM_ASSERT(texImages.size() == 1);

    Ptr<Assets::Texture> result(new Assets::Texture(texImages[0]));
    return result;
}
//-----------------------------------------------------------------------------
Ptr<Assets::Texture> generateTexture3D(const TextureOptions & options, const Core::List<Ptr<Images::Image> > & images)
{
    Core::List<Ptr<Assets::Texture> > textures2D;
    textures2D.resize(images.size());

    for(int32 iImage=0; iImage < images.size(); iImage++)
        textures2D[iImage] = generateTexture(options, images[iImage]);

    Core::List<Core::List<Assets::TextureImage> > texImages; // [mip][depth]
    texImages.resize(textures2D[0]->getMipmapCount());

    for(int32 iMip=0; iMip < texImages.size(); iMip++)
        texImages[iMip].resize(textures2D.size());

    for(int32 iSlice=0; iSlice < textures2D.size(); iSlice++)
    {
        if(textures2D[iSlice]->getWidth() != textures2D[iSlice]->getWidth() || textures2D[iSlice]->getHeight() != textures2D[iSlice]->getHeight())
            throw Core::Exception(L"Incoherent texture sizes for volume texture generation.");

        for(int32 iMip=0; iMip < textures2D[iSlice]->getMipmapCount(); iMip++)
        {
            const Core::List<Assets::TextureImage> & tex2DImages = textures2D[iSlice]->getImage(iMip);
            LM_ASSERT(tex2DImages.size() == 1);

            texImages[iMip][iSlice] = tex2DImages[0];   
        }
    }

    return Ptr<Assets::Texture>(new Assets::Texture(texImages));
}
//-----------------------------------------------------------------------------
Ptr<Assets::CubeTexture> generateTextureCube(const TextureOptions & options, const Core::List<Ptr<Images::Image> > & images)
{
    Core::List<Core::List<Assets::TextureImage> > texImages;
    texImages.reserve(6);
    generateTexture(options, images, texImages);
    LM_ASSERT(texImages.size() == 6);

    Core::List<Assets::Texture> faces;
    faces.reserve(texImages.size());
    for(Core::List<Core::List<Assets::TextureImage> >::const_iterator iTexture = texImages.begin();
        iTexture != texImages.end();
        ++iTexture)
    {
        faces.push_back(Assets::Texture(*iTexture));
    }

    Ptr<Assets::CubeTexture> result(new Assets::CubeTexture(faces));
    return result;
}
//-----------------------------------------------------------------------------
}
