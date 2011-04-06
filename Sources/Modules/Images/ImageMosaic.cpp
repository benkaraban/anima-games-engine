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
#include "ImageMosaic.h"

#include <math.h>

#include <Core/Standard.h>
#include <Core/Strings/Tools.h>
#include <Core/Logger.h>
#include <Core/XML/XMLDocument.h>
#include <Core/Math/MathTools.h>
#include <Images/Operator.h>

namespace Images
{
//-----------------------------------------------------------------------------
MosaicSubImage::MosaicSubImage(const Core::EBitmapFormat& imgType, const int32 width, const int32 height)
{
    _pImg = Ptr<Images::Image>(new Images::Image(imgType, width, height));
}
//-----------------------------------------------------------------------------
MosaicSubImage::~MosaicSubImage()
{
}
//-----------------------------------------------------------------------------
void MosaicSubImage::addSISC(const SubImageStreamConvert& sisc)
{
    _SISCs.push_back(sisc);
}
//-----------------------------------------------------------------------------
void MosaicSubImage::loadStream()
{
    for(Core::List<SubImageStreamConvert>::iterator it = _SISCs.begin(); it != _SISCs.end(); ++it)
    {
        LM_ASSERT(it->pStreamImage != null);
        LM_ASSERT(it->rectInStream.width > 0);
        LM_ASSERT(it->rectInStream.height > 0);
        LM_ASSERT(it->rectInSubImage.width > 0);
        LM_ASSERT(it->rectInSubImage.height > 0);

        Images::View viewImgStream(it->pStreamImage, it->rectInStream.x, it->rectInStream.y, it->rectInStream.width, it->rectInStream.height);
        Images::View viewSubImg(_pImg, it->rectInSubImage.x, it->rectInSubImage.y, it->rectInSubImage.width, it->rectInSubImage.height);

        copy(viewImgStream, viewSubImg);
    }
}
//-----------------------------------------------------------------------------
void MosaicSubImage::saveStream()
{
    LM_ASSERT(false);
    /*
    Ptr<Images::Image> pImgStream (new Images::Image);

    foreach(it, _SISCs)
    {
        pImgStream->loadFile(it->stream);

        Images::View viewImgStream(pImgStream, it->rectInStream.x, it->rectInStream.y, it->rectInStream.width, it->rectInStream.height);
        Images::View viewSubImg(_pImg, it->rectInSubImage.x, it->rectInSubImage.y, it->rectInSubImage.width, it->rectInSubImage.height);

        copy(viewSubImg, viewImgStream);

        pImgStream->saveFile(it->stream);
    }*/

}
//-----------------------------------------------------------------------------
ImageMosaic::ImageMosaic(const Ptr<Core::VirtualFileSystem> & pVFS, const Core::String& imgName, const Core::EBitmapFormat imgType,
                         const int32 nbImageX, const int32 nbImageY,
                         const int32 imageWidth, const int32 imageHeight,
                         const int32 cacheSize):
    _pVFS(pVFS),
    _cacheSize(cacheSize),
    _imgName(imgName),
    _nbImgX(nbImageX),
    _nbImgY(nbImageY),
    _imageWidth(imageWidth),
    _imageHeight(imageHeight)
{
    LM_ASSERT(_nbImgX > 0);
    LM_ASSERT(_nbImgY > 0);
    LM_ASSERT(_imageWidth > 0);
    LM_ASSERT(_imageHeight > 0);

    imgTypeToColorAndDepth(imgType, _colorSpace, _chnDepth);

    Core::String configFileName = _imgName + L".xml";

    if (pVFS->fileExists(_imgName+L"/"+configFileName) == false)
    {
        Core::XMLDocument document;
        document.setValue(L"config-mosaic.image-name", imgName);
        document.setValue(L"config-mosaic.image-color-space", _colorSpace);
        document.setValue(L"config-mosaic.image-channel-depth", _chnDepth);
        document.setValue(L"config-mosaic.image-size.width", _imageWidth);
        document.setValue(L"config-mosaic.image-size.height", _imageHeight);
        document.setValue(L"config-mosaic.nb-image.width", _nbImgX);
        document.setValue(L"config-mosaic.nb-image.height", _nbImgY);

        Ptr<Core::OutputStream> pOutputXML(pVFS->writeFile(_imgName+L"/"+configFileName));
        document.saveDocument(*pOutputXML);

        Ptr<Images::Image> pImg (new Images::Image(imgType, imageWidth, imageHeight));

        for (int32 iY=0; iY<_nbImgY; iY++)
            for (int32 iX=0; iX<_nbImgX; iX++)
            {
                Core::String imgNameFile = _imgName + L"_" + Core::toString(iX) + L"_" + Core::toString(iY) + L".jpg";
                if (pVFS->fileExists(_imgName+L"/"+imgNameFile) == false)
                {
                    Ptr<Core::OutputStream> pOutputImg(pVFS->writeFile(_imgName + L"/" + imgNameFile));
                    pImg->save(*pOutputImg);
                }
            }
    }
    else
    {
        ERR << L"ImageMosaic image existante cas non gere";
    }
}
//-----------------------------------------------------------------------------
ImageMosaic::ImageMosaic(const Ptr<Core::VirtualFileSystem> & pVFS, const Core::String& imgName, int32 cacheSize)
:   _pVFS(pVFS),
    _cacheSize(cacheSize),
    _imgName(imgName)
{
    Core::String configFileName = imgName + L".xml";

    if(!pVFS->fileExists(configFileName))
    {
        String message(L"Unable to configure mosaic image because file doesn't exist : '");
        message << configFileName << L"'";
        throw Core::Exception(message);
    }

    Core::XMLDocument document;

    Ptr<Core::InputStream> pInputXML(pVFS->readFile(configFileName));
    document.loadDocument(*pInputXML);

    int32 tmp;
    document.getValue(L"config-mosaic.image-name", _imgName);
    document.getValue(L"config-mosaic.image-color-space", tmp);
    _colorSpace = static_cast<EColorSpace>(tmp);
    document.getValue(L"config-mosaic.image-channel-depth", tmp);
    _chnDepth = static_cast<EChannelDepth>(tmp);
    document.getValue(L"config-mosaic.image-size.width", _imageWidth);
    document.getValue(L"config-mosaic.image-size.height", _imageHeight);
    document.getValue(L"config-mosaic.nb-image.width", _nbImgX);
    document.getValue(L"config-mosaic.nb-image.height", _nbImgY);

    if(!pVFS->dirExists(_imgName))
    {
        String message(L"Unable to configure mosaic image because directory doesn't exist : '");
        message << _imgName << L"'";
        throw Core::Exception(message);
    }

    LM_VERIFY(_imageWidth > 0, L"Invalid mosaic image width");
    LM_VERIFY(_imageHeight > 0, L"Invalid mosaic image height");
    LM_VERIFY(_nbImgX > 0, L"Invalid mosaic image X");
    LM_VERIFY(_nbImgY > 0, L"Invalid mosaic image Y");
}
//-----------------------------------------------------------------------------
ImageMosaic::~ImageMosaic()
{

}
//-----------------------------------------------------------------------------
int32 ImageMosaic::getMosaicWidth()
{
    return _imageWidth*_nbImgX;
}
//-----------------------------------------------------------------------------
int32 ImageMosaic::getMosaicHeight()
{
    return _imageHeight*_nbImgY;
}
//-----------------------------------------------------------------------------
Core::EBitmapFormat ImageMosaic::getImgType() const
{
    Core::EBitmapFormat imgType;
    colorAndDepthToimgType(_colorSpace, _chnDepth, imgType);
    return imgType;
}
//-----------------------------------------------------------------------------
Ptr<MosaicSubImage> ImageMosaic::createSubImage(const EImageCoord_t typeCoord, const float x, const float y, const float width, const float height)
{
    Ptr<MosaicSubImage> ret;

    int32 intX, intY, intW, intH;

    switch(typeCoord)
    {
        case COORD_PIXEL:
            intX = Core::L_round(x);
            intY = Core::L_round(y);
            intW = Core::L_round(width);
            intH = Core::L_round(height);
        break;

        case COORD_NORM:
           /* intX = Core::L_round(Core::clamp(0.f, 1.f, x) * (getMosaicWidth()-1));
            intY = Core::L_round(Core::clamp(0.f, 1.f, y) * (getMosaicHeight()-1));
            intW = Core::L_round(Core::clamp(0.f, 1.f, width) * getMosaicWidth());
            intH = Core::L_round(Core::clamp(0.f, 1.f, height) * getMosaicHeight());*/

            intX = Core::L_round(x * (getMosaicWidth()));
            intY = Core::L_round(y * (getMosaicHeight()));
            intW = Core::L_round(width * getMosaicWidth());

            intH = Core::L_round(height * getMosaicHeight());
        break;
    }

    ret = createSubImage(intX, intY, intW, intH);
    return ret;
}

Ptr<MosaicSubImage> ImageMosaic::createSubImage(const int32 x, const int32 y, const int32 width, const int32 height)
{
    Ptr<MosaicSubImage> sub (new MosaicSubImage(getImgType(), width, height));

    int32 ind_firstImgX = x/_imageWidth; // =0 => première image
    int32 ind_lastImgX = (x+width-1)/_imageWidth;
    int32 ind_firstImgY = y/_imageHeight; // =0 => première image
    int32 ind_lastImgY = (y+height-1)/_imageHeight;

    int32 totalHeight=0;

    for (int32 iY=ind_firstImgY; iY<=ind_lastImgY; iY++)
    {
        int32 totalWidth = 0;
        SubImageStreamConvert sisc;
        for (int32 iX=ind_firstImgX; iX<=ind_lastImgX; iX++)
        {


            //on cherche le point de depart dans l'image stream
            if (iX==ind_firstImgX)  sisc.rectInStream.x = x - (ind_firstImgX)*_imageWidth;
            else                    sisc.rectInStream.x = 0;
            if (iY==ind_firstImgY)  sisc.rectInStream.y = y - (ind_firstImgY)*_imageHeight;
            else                    sisc.rectInStream.y = 0;


            //on cherche le point de depart dans la subImage
            sisc.rectInSubImage.x = totalWidth;
            sisc.rectInSubImage.y = totalHeight;

            //on cherche la taille
            if (iX==ind_firstImgX)
            {

                sisc.rectInStream.width = _imageWidth - sisc.rectInStream.x;
                if (width < sisc.rectInStream.width )
                {
                    sisc.rectInStream.width = width;
                }
            }
            else if (iX != ind_lastImgX) {sisc.rectInStream.width = _imageWidth;}
            else                     {sisc.rectInStream.width = width - totalWidth;}


            if (iY==ind_firstImgY)
            {
                sisc.rectInStream.height = _imageHeight - sisc.rectInStream.y;
                if (height < sisc.rectInStream.height )
                {
                    sisc.rectInStream.height = height;
                }
            }
            else if (iY !=ind_lastImgY) sisc.rectInStream.height = _imageHeight;
            else                     sisc.rectInStream.height = height - totalHeight;

            sisc.rectInSubImage.width = sisc.rectInStream.width;
            sisc.rectInSubImage.height = sisc.rectInStream.height;
            sisc.pStreamImage = getImage(_imgName + L"/" + Core::System::getFileBaseName(_imgName) + L"_" + Core::toString(iY) + L"_" + Core::toString(iX) + L".jpg");

            totalWidth += sisc.rectInStream.width;
            sub->addSISC(sisc);

        }
        totalHeight += sisc.rectInStream.height;
    }

    sub->loadStream();
    return sub;
}
//-----------------------------------------------------------------------------
Ptr<Image> ImageMosaic::getImage(const String & name)
{
    if(!_pVFS->fileExists(name))
    {
        String message;
        message << L"File does not exist : " << name;
        throw Core::Exception(message);
    }

    Ptr<Image> result;
    Core::Map<String, CacheEntry>::iterator iEntry = _cache.find(name);

    if(iEntry != _cache.end())
    {
        //DBG << L"ImgMosaic get  OK " << Core::System::getFileName(name) << L"\n";
        iEntry->second.age = 0;
        result = iEntry->second.pImage;
    }
    else
    {
        int32 cacheSize = 0;
        for(Core::Map<String, CacheEntry>::iterator iEntry = _cache.begin(); iEntry != _cache.end(); ++iEntry)
            cacheSize += iEntry->second.size;

        //DBG << L"ImgMosaic get NOK " << Core::System::getFileName(name) << L" (" << Core::toString(_cache.size())
        //    << L" : " << Core::toStringByteSize(cacheSize) << L"/" << Core::toStringByteSize(_cacheSize) << L")\n";

        while(cacheSize > _cacheSize)
        {
            int32 oldest = 0;
            Core::Map<String, CacheEntry>::iterator iOldest = _cache.begin();

            for(Core::Map<String, CacheEntry>::iterator iEntry = _cache.begin(); iEntry != _cache.end(); ++iEntry)
            {
                if(iEntry->second.age > oldest)
                {
                    oldest = iEntry->second.age;
                    iOldest = iEntry;
                }
            }

            if(iOldest != _cache.end())
                _cache.erase(iOldest);

            cacheSize = 0;
            for(Core::Map<String, CacheEntry>::iterator iEntry = _cache.begin(); iEntry != _cache.end(); ++iEntry)
                cacheSize += iEntry->second.size;
        }

        result = Ptr<Image>(new Image);
        Ptr<Core::InputStream> pInput = _pVFS->readFile(name);
        result->load((Core::SeekableInputStream &)*pInput);

        CacheEntry entry;
        entry.age = 0;
        entry.size = result->getHeight() * result->getRowStep();
        entry.pImage = result;
        _cache[name] = entry;
    }

    for(Core::Map<String, CacheEntry>::iterator iEntry = _cache.begin(); iEntry != _cache.end(); ++iEntry)
        iEntry->second.age++;

    return result;
}
//-----------------------------------------------------------------------------
void ImageMosaic::clearCache()
{
    _cache.clear();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} //namespace Images

