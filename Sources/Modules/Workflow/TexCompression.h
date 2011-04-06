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
#ifndef WORKFLOW_TEXTURES_H_
#define WORKFLOW_TEXTURES_H_

#include <Core/Standard.h>
#include <Core/IProgressObs.h>
#include <Assets/Texture.h>
#include <Images/Image.h>

namespace Workflow
{
LM_ENUM_3(EMipmapFilter,
          MIPMAP_FILTER_BOX,
          MIPMAP_FILTER_TRIANGLE,
          MIPMAP_FILTER_KAISER);

LM_ENUM_4(ETextureType,
          TEXTURE_COLORMAP,
          TEXTURE_LIGHTMAP,
          TEXTURE_HEIGHTMAP,
          TEXTURE_NORMALMAP);

LM_ENUM_2(ECompressionQuality,
        QUALITY_LOW,
        QUALITY_HIGH);

struct TextureOptions
{
    TextureOptions() :
        format(Assets::TEX_FORMAT_ARGB8),
        mipmapFilter(MIPMAP_FILTER_BOX),
        texType(TEXTURE_COLORMAP),
        kaiserWidth(3.0f),
        kaiserAlpha(4.0f),
        kaiserStretch(1.0f),
        generateMipmap(true),
        quality(QUALITY_LOW),
        pProgressObs(NULL)
    {}

    Assets::ETextureFormat  format;
    EMipmapFilter           mipmapFilter;
    ETextureType            texType;
    float                   kaiserWidth;
    float                   kaiserAlpha;
    float                   kaiserStretch;
    bool                    generateMipmap;
    ECompressionQuality     quality;
    Core::IProgressObs *    pProgressObs;
};

LM_API_WKF Ptr<Assets::Texture> generateTexture(const TextureOptions & options, const Ptr<Images::Image> & pImage);
LM_API_WKF Ptr<Assets::Texture> generateTexture3D(const TextureOptions & options, const Core::List<Ptr<Images::Image> > & images);
LM_API_WKF Ptr<Assets::CubeTexture> generateTextureCube(const TextureOptions & options, const Core::List<Ptr<Images::Image> > & images);
}

#endif /*WORKFLOW_TEXTURES_H_*/
