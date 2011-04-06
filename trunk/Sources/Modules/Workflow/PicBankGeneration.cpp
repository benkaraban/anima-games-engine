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
#include <Core/System/Files.h>
#include <Workflow/PicBankGeneration.h>
#include <Workflow/ZonePacking.h>
#include <Workflow/TexCompression.h>

namespace Workflow
{
//-----------------------------------------------------------------------------
LM_API_WKF Ptr<Assets::PictureBank> generatePictureBank(const String & srcDir, int32 maxSize, Assets::ETextureFormat format, bool recursive)
{
    Core::List<String>  names;

    if(recursive)
    {
        Core::System::getRecursiveDirContent(srcDir, names);
    }
    else
    {
        Core::List<String>  dirs;
        Core::System::getDirContent(srcDir, names, dirs);
    }

    // Filtre .tga
    Core::List<String>  filteredNames;

    for(int32 ii=0; ii < names.size(); ii++)
    {
        if(names[ii].endsWith(L".tga") || 
           names[ii].endsWith(L".png") || 
           names[ii].endsWith(L".jpg") || 
           names[ii].endsWith(L".jpeg")
           )
        {
            filteredNames.push_back(names[ii]);
        }
    }

    filteredNames.swap(names);


    Core::List<Ptr<Images::Image> > images;

    images.resize(names.size());

    for(int32 ii=0; ii < images.size(); ii++)
    {
        images[ii] = Ptr<Images::Image>(new Images::Image());
        images[ii]->loadFile(srcDir + L"/" + names[ii]);
    }

    return generatePictureBank(names, maxSize, format, images);
}
//-----------------------------------------------------------------------------
Ptr<Assets::PictureBank> generatePictureBank(const Core::List<String> & names, int32 maxSize,
                                             Assets::ETextureFormat format,
                                             const Core::List<Ptr<Images::Image> > & images)
{
    LM_ASSERT(names.size() == images.size());

    Core::List<ZonePack> packs;
    Core::List<Zone> zones;
    Core::List<Assets::PictureBank::Pic> pics;

    zones.resize(images.size());
    pics.resize(images.size());

    for(int32 ii=0; ii < images.size(); ii++)
    {
        zones[ii].width = images[ii]->getWidth() + 2;
        zones[ii].height = images[ii]->getHeight() + 2;
    }

    packZones(packs, zones, maxSize, maxSize);

    Core::List<Ptr<Images::Image> > packedImages;
    packedImages.resize(packs.size());

    // Copy packed pictures in big pictures

    for(int32 ii=0; ii < packedImages.size(); ii++)
    {
        const ZonePack & pack = packs[ii];
        Ptr<Images::Image> pImagePack(new Images::Image(Core::ARGB_U8, pack.width, pack.height));
        packedImages[ii] = pImagePack;

        for(int32 iImg=0; iImg < pack.zones.size(); iImg++)
        {
            const ZonePack::PackedZone & zone = pack.zones[iImg];
            Assets::PictureBank::Pic & pic = pics[zone.id];
            Ptr<Images::Image> pSrcImg = images[zone.id];

            pic.textureIndex = ii;
            pic.width = pSrcImg->getWidth();
            pic.height = pSrcImg->getHeight();
            pic.u1 = (float(zone.x1 + 1) + 0.0f) / float(pack.width);
            pic.v1 = (float(zone.y1 + 1) + 0.0f) / float(pack.height);
            pic.u2 = (float(zone.x2 - 1) + 1.0f) / float(pack.width);
            pic.v2 = (float(zone.y2 - 1) + 1.0f) / float(pack.height);

            // Interior content
            
            {
                Images::View dstView(pImagePack, zone.x1 + 1, zone.y1 + 1, pSrcImg->getWidth(), pSrcImg->getHeight());
                Images::View dstViewLeft(pImagePack, zone.x1, zone.y1 + 1, 1, pSrcImg->getHeight());
                Images::View dstViewRight(pImagePack, zone.x2, zone.y1 + 1, 1, pSrcImg->getHeight());

                Images::View srcView(pSrcImg, 0, 0, pSrcImg->getWidth(), pSrcImg->getHeight());
                Images::View srcViewLeft(pSrcImg, 0, 0, 1, pSrcImg->getHeight());
                Images::View srcViewRight(pSrcImg, pSrcImg->getWidth() - 1, 0, 1, pSrcImg->getHeight());

                Images::it2D_argbU8 iDst, iDstLeft, iDstRight;
                Images::it2D_argbU8 iSrc, iSrcLeft, iSrcRight;

                Images::makeIterator(dstView, iDst);
                Images::makeIterator(srcView, iSrc);

                Images::makeIterator(dstViewLeft, iDstLeft);
                Images::makeIterator(srcViewLeft, iSrcLeft);

                Images::makeIterator(dstViewRight, iDstRight);
                Images::makeIterator(srcViewRight, iSrcRight);

                copy(iDst, iSrc);
                copy(iDstLeft, iSrcLeft);
                copy(iDstRight, iSrcRight);

                Images::View dstViewUp1(pImagePack, zone.x1, zone.y2 - 1, pSrcImg->getWidth() + 2, 1);
                Images::View dstViewUp2(pImagePack, zone.x1, zone.y2, pSrcImg->getWidth() + 2, 1);

                Images::View dstViewDown1(pImagePack, zone.x1, zone.y1 + 1, pSrcImg->getWidth() + 2, 1);
                Images::View dstViewDown2(pImagePack, zone.x1, zone.y1, pSrcImg->getWidth() + 2, 1);

                Images::it2D_argbU8 iDstUp1, iDstUp2, iDstDown1, iDstDown2;

                Images::makeIterator(dstViewUp1, iDstUp1);
                Images::makeIterator(dstViewUp2, iDstUp2);
                Images::makeIterator(dstViewDown1, iDstDown1);
                Images::makeIterator(dstViewDown2, iDstDown2);

                copy(iDstUp2, iDstUp1);
                copy(iDstDown2, iDstDown1);
            }
        }
    }

    // Generate textures from pictures

    Core::List<Ptr<Assets::Texture> > textures;
    textures.resize(packedImages.size());
    
    TextureOptions options;
    options.format = format;
    options.texType = TEXTURE_COLORMAP;
    options.generateMipmap = true;
    options.quality = QUALITY_HIGH;

    for(int32 ii=0; ii < packedImages.size(); ii++)
        textures[ii] = generateTexture(options, packedImages[ii]);
    
    Core::Map<String, Assets::PictureBank::Pic> picsMap;

    for(int32 ii=0; ii < pics.size(); ii++)
        picsMap[names[ii]] = pics[ii];

    return Ptr<Assets::PictureBank>(new Assets::PictureBank(textures, picsMap));
}
//-----------------------------------------------------------------------------
}
