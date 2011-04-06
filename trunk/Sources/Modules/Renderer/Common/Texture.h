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
#ifndef COMMON_TEXTURE_H_
#define COMMON_TEXTURE_H_

#include <Gfx/IDevice.h>
#include <Renderer/ITexture.h>
#include <Renderer/Settings.h>
#include <Renderer/Common/GPUResource.h>

namespace Renderer
{
class LM_API_RDR TextureMap : public Core::Object, public ITextureMap, public IGPUResource
{
public:
    TextureMap(const Ptr<Assets::Texture> & pTexture, const Ptr<Gfx::IDevice> & pDevice, 
               const Ptr<GPUResourceLib> & pResLib, ETextureLevel texLevel);
    TextureMap(const Ptr<Gfx::ITexture2D> & pTexture);
    ~TextureMap();

    virtual void onDeviceLost();
    virtual void onDeviceReset();
    virtual void onTextureLevelChange(ETextureLevel level);

    virtual Ptr<Assets::Texture> getSourceTexture() const { return _pSrcTexture; };
    virtual bool isAnimatedTexture() const { return _textures2D.size() >= 2; };

    Ptr<Gfx::IShaderResourceView> getResource(float key = 0.0f) const;

    virtual void copyTexture(const Ptr<ITextureMap> & pITextureMap);

protected:
    Ptr<Gfx::IDevice>                           _pDevice;
    Ptr<GPUResourceLib>                         _pResLib;
    Ptr<Assets::Texture>                        _pSrcTexture;
    Core::List<Ptr<Gfx::ITexture2D> >           _textures2D;
    Core::List<Ptr<Gfx::IShaderResourceView> >  _resources;
    ETextureLevel                               _texLevel;
    bool                                        _deviceLost;
};

class LM_API_RDR VolumeTextureMap : public Core::Object, public IVolumeTextureMap
{
public:
    VolumeTextureMap(const Ptr<Assets::Texture> & pTexture, const Ptr<Gfx::IDevice> & pDevice);
    ~VolumeTextureMap() {};
    virtual Ptr<Assets::Texture> getSourceTexture() const { return _pSrcTexture; };
    virtual void copyTexture(const Ptr<IVolumeTextureMap> & pIVolumeTextureMap);

protected:
    Ptr<Assets::Texture> _pSrcTexture;
    Ptr<Gfx::ITexture3D> _pGfxTexture3D;
    Ptr<Gfx::IShaderResourceView>  _pGfxResource;
};

class LM_API_RDR CubeTextureMap : public Core::Object, public ICubeTextureMap, public IGPUResource
{
public:
    CubeTextureMap(const Ptr<Assets::CubeTexture> & pTexture, const Ptr<Gfx::IDevice> & pDevice, 
                   const Ptr<GPUResourceLib> & pResLib, ETextureLevel texLevel);

    ~CubeTextureMap();
    virtual Ptr<Assets::CubeTexture> getSourceTexture() const { return _pSrcTexture; };

    virtual void onDeviceLost();
    virtual void onDeviceReset();
    virtual void onTextureLevelChange(ETextureLevel level);

    Ptr<Gfx::IShaderResourceView> getResource() const { return _pResource; }

protected:
    Ptr<Gfx::IDevice>                           _pDevice;
    Ptr<GPUResourceLib>                         _pResLib;
    Ptr<Assets::CubeTexture>                    _pSrcTexture;
    Ptr<Gfx::ITextureCube>                      _pCube;
    Ptr<Gfx::IShaderResourceView>               _pResource;
    ETextureLevel                               _texLevel;
    bool                                        _deviceLost;
};

}

#endif /*ABSTRACT_ITEXTURE_H_*/
