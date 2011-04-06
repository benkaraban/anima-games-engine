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
#include <Renderer/Common/Texture.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
TextureMap::TextureMap(const Ptr<Assets::Texture> & pTexture, const Ptr<Gfx::IDevice> & pDevice, 
                       const Ptr<GPUResourceLib> & pResLib, ETextureLevel texLevel) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _pSrcTexture(pTexture),
    _texLevel(texLevel),
    _deviceLost(false)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
TextureMap::~TextureMap()
{
    if(_pResLib != null)
        _pResLib->unregisterResource(this);
    else
        onDeviceLost();
}
//-----------------------------------------------------------------------------
void TextureMap::onDeviceLost()
{
    _deviceLost = true;
    _resources.clear();
    _textures2D.clear();
}
//-----------------------------------------------------------------------------
void TextureMap::onTextureLevelChange(ETextureLevel level)
{
    if(_deviceLost)
    {
        _texLevel = level;
    }
    else
    {
        onDeviceLost();
        _texLevel = level;
        if(!_pResLib->isDeviceLost())
            onDeviceReset();
    }
}
//-----------------------------------------------------------------------------
void TextureMap::onDeviceReset()
{
    bool scanline = false;
    Gfx::ETextureFormat format;

    switch(_pSrcTexture->getFormat())
    {
    case Assets::TEX_FORMAT_ARGB8: format = Gfx::TF_A8R8G8B8; scanline = true; break;
    case Assets::TEX_FORMAT_DXTC1: format = Gfx::TF_DXT1; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC3: format = Gfx::TF_DXT3; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC5: format = Gfx::TF_DXT5; scanline = false; break;
    }

    int32 width = _pSrcTexture->getWidth();
    int32 height = _pSrcTexture->getHeight();
    int32 mipCount = _pSrcTexture->getMipmapCount();
    int32 deltaMip = 0;

    if(_texLevel == TEXTURE_LOW)
    {
        if(width >= 1024 && height >= 1024 && mipCount >= 3)
        {
            width = std::max(1, width / 4);
            height = std::max(1, height / 4);
            deltaMip = 2;
            mipCount = mipCount - deltaMip;
        }
        else if(width >= 256 && height >= 256 && mipCount >= 3)
        {
            width = std::max(1, width / 2);
            height = std::max(1, height / 2);
            deltaMip = 1;
            mipCount = mipCount - deltaMip;
        }
    }
    else if(_texLevel == TEXTURE_MEDIUM)
    {
        if(width >= 512 && height >= 512 && mipCount >= 3)
        {
            width = std::max(1, width / 2);
            height = std::max(1, height / 2);
            deltaMip = 1;
            mipCount = mipCount - deltaMip;
        }
    }

    // En DXTx, taille minimale = 4x4
    if(!scanline)
        mipCount = std::max(1, mipCount - 2);

    //mipCount = std::min(mipCount, 3);

    _textures2D.resize(_pSrcTexture->getDepth());
    _resources.resize(_pSrcTexture->getDepth());

    for(int32 iDepth=0; iDepth < _pSrcTexture->getDepth(); iDepth++)
    {
        Gfx::Texture2DDesc texDesc;
        texDesc.width = width;
        texDesc.height = height;
        texDesc.mipLevels = mipCount;
        texDesc.usage = Gfx::BUFFER_USAGE_IMMUTABLE;
        texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
        texDesc.bind = Gfx::TB_BUFFER_DEFAULT;
        texDesc.format = format;

        _textures2D[iDepth] = _pDevice->createTexture2D(texDesc);

        texDesc.usage = Gfx::BUFFER_USAGE_DYNAMIC;
        texDesc.cpuAccess = Gfx::BCA_READ_AND_WRITE;

        Ptr<Gfx::ITexture2D> pSystemTex(_pDevice->createTexture2D(texDesc));

        for(int32 level=0; level < mipCount; level++)
        {
            const Assets::TextureImage & img = _pSrcTexture->getImage(level + deltaMip)[iDepth];
            Gfx::MappedTexture2D mapTex = pSystemTex->lock(Gfx::LOCK_WRITE, level);

            if(scanline)
            {
                int32 srcLineOffset = (img.getBitsPerPixels() * img.getWidth()) / 8;
                int32 dstLineOffset = mapTex.rowStep;

                LM_ASSERT(srcLineOffset <= dstLineOffset);

                const byte * pSrc = reinterpret_cast<const byte*>(img.getData());
                byte * pDst = reinterpret_cast<byte*>(mapTex.pData);

                for(int32 y=0; y < img.getHeight(); y++)
                {
                    memcpy(pDst, pSrc, srcLineOffset);

                    pSrc += srcLineOffset;
                    pDst += dstLineOffset;
                }
            }
            else
            {
                const byte * pSrc = reinterpret_cast<const byte*>(img.getData());
                byte * pDst = reinterpret_cast<byte*>(mapTex.pData);

                // Blocks 4x4
                int32 srcLineOffset = (img.getBitsPerPixels() * img.getWidth() * 4) / 8;
                int32 dstLineOffset = mapTex.rowStep;

                LM_ASSERT(srcLineOffset <= dstLineOffset);

                for(int32 y=0; y < img.getHeight() / 4; y++)
                {
                    memcpy(pDst, pSrc, srcLineOffset);

                    pSrc += srcLineOffset;
                    pDst += dstLineOffset;
                }
            }

            pSystemTex->unlock(level);
        }

        _pDevice->updateTexture(pSystemTex, _textures2D[iDepth]);
        _resources[iDepth] = _textures2D[iDepth]->getShaderResourceView();
    }

    _deviceLost = false;
}
//-----------------------------------------------------------------------------
TextureMap::TextureMap(const Ptr<Gfx::ITexture2D> & pTexture)
{
    _textures2D.push_back(pTexture);
    _resources.push_back(pTexture->getShaderResourceView());
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IShaderResourceView> TextureMap::getResource(float key) const 
{ 
    int32 iKey = 0;

    if(_resources.size() >= 2)
        iKey = Core::L_round(Core::clamp(0.0f, 1.0f, key) * float(_resources.size() - 1));

    return _resources[iKey]; 
}
//-----------------------------------------------------------------------------
void TextureMap::copyTexture(const Ptr<ITextureMap> & pITextureMap)
{
    Ptr<TextureMap> pTextureMap = LM_DEBUG_PTR_CAST<TextureMap>(pITextureMap);

    _pSrcTexture = pTextureMap->_pSrcTexture;
    _textures2D  = pTextureMap->_textures2D;
    _resources   = pTextureMap->_resources;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
VolumeTextureMap::VolumeTextureMap(const Ptr<Assets::Texture> & pTexture, const Ptr<Gfx::IDevice> & pDevice) :
    _pSrcTexture(pTexture)
{
    bool scanline = false;
    Gfx::ETextureFormat format;

    switch(pTexture->getFormat())
    {
    case Assets::TEX_FORMAT_ARGB8: format = Gfx::TF_A8R8G8B8; scanline = true; break;
    case Assets::TEX_FORMAT_DXTC1: format = Gfx::TF_DXT1; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC3: format = Gfx::TF_DXT3; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC5: format = Gfx::TF_DXT5; scanline = false; break;
    }

    Gfx::Texture3DDesc tex1Desc;
    tex1Desc.width = pTexture->getWidth();
    tex1Desc.height = pTexture->getHeight();
    tex1Desc.nbSlice = pTexture->getDepth();
    tex1Desc.mipLevels = pTexture->getMipmapCount();
    tex1Desc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    tex1Desc.cpuAccess = Gfx::BCA_ONLY_WRITE;
    tex1Desc.bind = Gfx::TB_BUFFER_DEFAULT;
    tex1Desc.format = format;

    _pGfxTexture3D = pDevice->createTexture3D(tex1Desc);

    for(int32 level=0; level < pTexture->getMipmapCount(); level++)
    {
        const Core::List<Assets::TextureImage> & images = pTexture->getImage(level);
        Gfx::MappedTexture3D mapTex = _pGfxTexture3D->lock(Gfx::LOCK_WRITE, level);

        if(scanline)
        {
            for(int32 iSlice=0; iSlice < images.size(); iSlice++)
            {
                const Assets::TextureImage & img = images[iSlice];

                int32 srcLineOffset = (img.getBitsPerPixels() * img.getWidth()) / 8;
                int32 dstLineOffset = mapTex.rowStep;

                LM_ASSERT(srcLineOffset <= dstLineOffset);

                const byte * pSrc = reinterpret_cast<const byte*>(img.getData());
                byte * pDst = reinterpret_cast<byte*>(mapTex.pData);

                pDst += (mapTex.sliceStep * iSlice); // Choix slice

                for(int32 y=0; y < img.getHeight(); y++)
                {
                    memcpy(pDst, pSrc, srcLineOffset);

                    pSrc += srcLineOffset;
                    pDst += dstLineOffset;
                }
            }
        }
        else
        {
            for(int32 iSlice=0; iSlice < images.size(); iSlice++)
            {
                const Assets::TextureImage & img = images[iSlice];
                byte * pDst = reinterpret_cast<byte*>(mapTex.pData);

                pDst += (mapTex.sliceStep * iSlice); // Choix slice
                memcpy(pDst, img.getData(), img.getDataSize());
            }
        }

        _pGfxTexture3D->unlock(level);
    }

    _pGfxResource = _pGfxTexture3D->getShaderResourceView();
}
//-----------------------------------------------------------------------------
void VolumeTextureMap::copyTexture(const Ptr<IVolumeTextureMap> & pIVolumeTextureMap)
{
    Ptr<VolumeTextureMap> pTextureMap = LM_DEBUG_PTR_CAST<VolumeTextureMap>(pIVolumeTextureMap);

    _pSrcTexture   = pTextureMap->_pSrcTexture;
    _pGfxTexture3D = pTextureMap->_pGfxTexture3D;
    _pGfxResource  = pTextureMap->_pGfxResource;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
CubeTextureMap::CubeTextureMap(const Ptr<Assets::CubeTexture> & pTexture, const Ptr<Gfx::IDevice> & pDevice, 
                               const Ptr<GPUResourceLib> & pResLib, ETextureLevel texLevel) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _pSrcTexture(pTexture),
    _texLevel(texLevel),
    _deviceLost(false)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
CubeTextureMap::~CubeTextureMap()
{
    if(_pResLib != null)
        _pResLib->unregisterResource(this);
    else
        onDeviceLost();
}
//-----------------------------------------------------------------------------
void CubeTextureMap::onDeviceLost()
{
    _deviceLost = true;
    _pResource = null;
    _pCube = null;
}
//-----------------------------------------------------------------------------
void CubeTextureMap::onTextureLevelChange(ETextureLevel level)
{
    if(_deviceLost)
    {
        _texLevel = level;
    }
    else
    {
        onDeviceLost();
        _texLevel = level;
        if(!_pResLib->isDeviceLost())
            onDeviceReset();
    }
}
//-----------------------------------------------------------------------------
void CubeTextureMap::onDeviceReset()
{
    bool scanline = false;
    Gfx::ETextureFormat format;

    switch(_pSrcTexture->getFormat())
    {
    case Assets::TEX_FORMAT_ARGB8: format = Gfx::TF_A8R8G8B8; scanline = true; break;
    case Assets::TEX_FORMAT_DXTC1: format = Gfx::TF_DXT1; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC3: format = Gfx::TF_DXT3; scanline = false; break;
    case Assets::TEX_FORMAT_DXTC5: format = Gfx::TF_DXT5; scanline = false; break;
    }

    int32 width = _pSrcTexture->getWidth();
    int32 mipCount = _pSrcTexture->getMipmapCount();
    int32 deltaMip = 0;

    Gfx::TextureCubeDesc texCubeDesc;
    texCubeDesc.autoGenMipmap = false;
    texCubeDesc.usage = Gfx::BUFFER_USAGE_IMMUTABLE;
    texCubeDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texCubeDesc.bind = Gfx::TB_BUFFER_DEFAULT;
    texCubeDesc.edgeLength = width;
    texCubeDesc.mipLevels = mipCount;

    _pCube = _pDevice->createTextureCube(texCubeDesc);

    texCubeDesc.usage = Gfx::BUFFER_USAGE_DYNAMIC;
    texCubeDesc.cpuAccess = Gfx::BCA_READ_AND_WRITE;

    Ptr<Gfx::ITextureCube> pSystemTex(_pDevice->createTextureCube(texCubeDesc));

    for(int32 iFace=0; iFace < Assets::ECubeFace_COUNT; iFace++)
    {
        for(int32 level=0; level < mipCount; level++)
        {
            Gfx::MappedTexture2D mapTex = pSystemTex->lock(Gfx::LOCK_WRITE, level, (Gfx::ECubeMapFace)iFace);
            const Assets::TextureImage & img = _pSrcTexture->getImage((Assets::ECubeFace)iFace, level)[0];

            if(scanline)
            {
                int32 srcLineOffset = (img.getBitsPerPixels() * img.getWidth()) / 8;
                int32 dstLineOffset = mapTex.rowStep;

                LM_ASSERT(srcLineOffset <= dstLineOffset);

                const byte * pSrc = reinterpret_cast<const byte*>(img.getData());
                byte * pDst = reinterpret_cast<byte*>(mapTex.pData);

                for(int32 y=0; y < img.getHeight(); y++)
                {
                    memcpy(pDst, pSrc, srcLineOffset);

                    pSrc += srcLineOffset;
                    pDst += dstLineOffset;
                }
            }
            else
            {
                memcpy(mapTex.pData, img.getData(), img.getDataSize());
            }

            pSystemTex->unlock(level, (Gfx::ECubeMapFace)iFace);
        }
    }

    _pDevice->updateTexture(pSystemTex, _pCube);

    _pResource = _pCube->getShaderResourceView();
    _deviceLost = false;
}

}
