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
#ifndef IMAGEMOSAIC_H_
#define IMAGEMOSAIC_H_

#include <Core/Standard.h>
#include <Core/Map.h>
#include <Core/VFS/VirtualFilesystem.h>

#include <Images/Image.h>

namespace Images
{
/**
 * ATTENTION !! La classe ImageMosaic permet de gerer une grande image stockee en plusieurs image sur le disque dur
 * elle a ete faite pour gerer la texture du terrain de la Demo 
 * La classe a ete concu rapidement et dispose de nombreux bug/erreur de conception
 * Bref a eviter
 */



struct Rect
{
    int32 x;
    int32 y;
    int32 width;
    int32 height;
};

//Pour le moment plutot que de gerer des stream on passe par des String ...
struct SubImageStreamConvert
{
    Ptr<Images::Image> pStreamImage;
    Rect    rectInSubImage;
    Rect    rectInStream;
};


class LM_API_IMG MosaicSubImage
{
public:
    MosaicSubImage(const Core::EBitmapFormat& imgType, const int32 width, const int32 height);
    ~MosaicSubImage();
    void addSISC(const SubImageStreamConvert& sisc);
    void loadStream();
    void saveStream();
    Ptr<Images::Image> getImg() {return _pImg;}
private:
    Core::List<SubImageStreamConvert>   _SISCs;
    Ptr<Images::Image>                  _pImg;
};

class LM_API_IMG ImageMosaic
{
public:
    ImageMosaic(const Ptr<Core::VirtualFileSystem> & pVFS, const Core::String& imgName, const Core::EBitmapFormat imgType,
                const int32 nbImageX, const int32 nbImageY,
                const int32 imageWidth, const int32 imageHeight,
                const int32 cacheSize);
    ImageMosaic(const Ptr<Core::VirtualFileSystem> & pVFS, const Core::String& imgName, int32 cacheSize);
    ~ImageMosaic();

    //la largeur de l'image entiere
    int32 getMosaicWidth();

    //la hauteur de l'image entiere
    int32 getMosaicHeight();
    Core::EBitmapFormat getImgType() const;

    Ptr<MosaicSubImage> createSubImage(const int32 x, const int32 y, const int32 width, const int32 height);
    Ptr<MosaicSubImage> createSubImage(const EImageCoord_t typeCoord, const float x, const float y, const float width, const float height);

    void clearCache();
private:
    Ptr<Image> getImage(const String & name);

    struct CacheEntry
    {
        int32      age;
        int32      size;
        Ptr<Image> pImage;
    };

    Ptr<Core::VirtualFileSystem>  _pVFS;
    Core::Map<String, CacheEntry> _cache;
    int32                         _cacheSize;
    Core::String _imgName;
    EColorSpace _colorSpace;
    EChannelDepth _chnDepth;
    int32 _nbImgX;
    int32 _nbImgY;
    int32 _imageWidth;  //d'une image sur disque
    int32 _imageHeight;
};

} // namespace Images

#endif /* IMAGEMOSAIC_H_ */
