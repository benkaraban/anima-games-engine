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
#include "ImportTextureInfo.h"

#include <EPI/Constants.h>

namespace EPI
{

void ImportTextureInfo::exportXML(const Core::Ptr<Core::XMLNode> & pXMLNode) const
{
    pXMLNode->setAttribute(L"asset", L"type", EPI::toString(TEXTURE_ASSET));

    pXMLNode->setValue(L"asset.image-name", imageName);
    pXMLNode->setValue(L"asset.texture-name", textureName);

    pXMLNode->setValue(L"asset.options.format", textureFormatToString(pTextureOptions->format));
    pXMLNode->setValue(L"asset.options.mipmap-filter", mipmapFilterToString(pTextureOptions->mipmapFilter));
    pXMLNode->setValue(L"asset.options.type", textureTypeToString(pTextureOptions->texType));
    pXMLNode->setValue(L"asset.options.kaiser-width", pTextureOptions->kaiserWidth);
    pXMLNode->setValue(L"asset.options.kaiser-alpha", pTextureOptions->kaiserAlpha);
    pXMLNode->setValue(L"asset.options.kaiser-stretch", pTextureOptions->kaiserStretch);
    pXMLNode->setValue(L"asset.options.generate-mipmap", pTextureOptions->generateMipmap);
}

void ImportTextureInfo::importXML(const Core::Ptr<Core::XMLNode> & pXMLNode)
{
    bool success = true;

    pTexture = Ptr<Assets::Texture>(new Assets::Texture());
    pTextureOptions = Ptr<Workflow::TextureOptions>(new Workflow::TextureOptions());

    Core::String textureFormat;
    Core::String mipmapFilter;
    Core::String textureType;

    success &= pXMLNode->getValue(L"asset.image-name", imageName);
    success &= pXMLNode->getValue(L"asset.texture-name", textureName);

    success &= pXMLNode->getValue(L"asset.options.format", textureFormat);
    success &= pXMLNode->getValue(L"asset.options.mipmap-filter", mipmapFilter);
    success &= pXMLNode->getValue(L"asset.options.type", textureType);
    success &= pXMLNode->getValue(L"asset.options.kaiser-width", pTextureOptions->kaiserWidth);
    success &= pXMLNode->getValue(L"asset.options.kaiser-alpha", pTextureOptions->kaiserAlpha);
    success &= pXMLNode->getValue(L"asset.options.kaiser-stretch", pTextureOptions->kaiserStretch);
    success &= pXMLNode->getValue(L"asset.options.generate-mipmap", pTextureOptions->generateMipmap);

    pTextureOptions->format       = stringToTextureFormat(textureFormat);
    pTextureOptions->mipmapFilter = stringToMipmapFilter(mipmapFilter);
    pTextureOptions->texType      = stringToTextureType(textureType);

    if(!success)
        throw Core::Exception(L"An error occured while importing the XML ImportTextureInfo values.");
}


}//namespace EPI

