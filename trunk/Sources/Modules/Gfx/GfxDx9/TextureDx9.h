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
#ifndef TEXTUREDX9_H_
#define TEXTUREDX9_H_

#include <Gfx/ITexture.h>
#include <Gfx/GfxDx9/dx9Header.h>

#ifdef _DEBUG
#define LM_WATCH_TEX_FOR_RESET 1
#else
#define LM_WATCH_TEX_FOR_RESET 0
#endif

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GX9 ShaderResourceViewDx9 : public Core::Object, public IShaderResourceView
{
public:
    ShaderResourceViewDx9(const ShaderResourceViewDesc& desc, void* ptrD3D);
    virtual ~ShaderResourceViewDx9();

public:
    LPDIRECT3DBASETEXTURE9 _pD3DBaseTexture;
};
typedef Core::Ptr<ShaderResourceViewDx9> ShaderResourceViewDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 Texture1DDx9 : public Core::Object, public ITexture1D
{
    friend class DeviceDx9;

public:
    Texture1DDx9(const Texture1DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice);
    ~Texture1DDx9();

    virtual void* lock(const ELockType lt, const int32 level);
    virtual void unlock(const int32 level);
    void buildShaderResourceView();
    virtual IShaderResourceViewPtr getShaderResourceView();
    virtual void generateMipmaps();

protected:
    LPDIRECT3DTEXTURE9 _pD3DTexture;
    ShaderResourceViewDx9Ptr _pSrv;
};
typedef Core::Ptr<Texture1DDx9> Texture1DDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 SurfaceDx9 : public Core::Object, public ITexture2D
{
    friend class DeviceDx9;
public:
    SurfaceDx9(const Texture2DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice);
    virtual ~SurfaceDx9();

    virtual MappedTexture2D lock(const ELockType lt, const int32 level);
    virtual void unlock(const int32 level);
    virtual IShaderResourceViewPtr getShaderResourceView();
    virtual void generateMipmaps();

protected:
    LPDIRECT3DSURFACE9 _pD3DSurface;
};
typedef Core::Ptr<SurfaceDx9> SurfaceDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 Texture2DDx9 : public Core::Object, public ITexture2D
{
    friend class DeviceDx9;

public:
    Texture2DDx9(const Texture2DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice);
    ~Texture2DDx9();

    virtual MappedTexture2D lock(const ELockType lt, const int32 level);
    virtual void unlock(const int32 level);
    virtual void buildShaderResourceView();
    virtual IShaderResourceViewPtr getShaderResourceView();
    virtual void generateMipmaps();

#if LM_WATCH_TEX_FOR_RESET
    String          _cs;
    static Core::List<Texture2DDx9 *> sTextures;
#endif

protected:
    LPDIRECT3DTEXTURE9 _pD3DTexture;
    ShaderResourceViewDx9Ptr _pSrv;
};
typedef Core::Ptr<Texture2DDx9> Texture2DDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 Texture3DDx9 : public Core::Object, public ITexture3D
{
    friend class DeviceDx9;

public:
    Texture3DDx9(const Texture3DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice);
    ~Texture3DDx9();

    virtual MappedTexture3D lock(const ELockType lt, const int32 level);
    virtual void unlock(const int32 level);
    virtual void buildShaderResourceView();
    virtual IShaderResourceViewPtr getShaderResourceView();
    virtual void generateMipmaps();

protected:
    LPDIRECT3DVOLUMETEXTURE9 _pD3DVolumeTexture;
    ShaderResourceViewDx9Ptr _pSrv;
};
typedef Core::Ptr<Texture3DDx9> Texture3DDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 TextureCubeDx9 : public Core::Object, public ITextureCube
{
    friend class DeviceDx9;

public:
    TextureCubeDx9(const TextureCubeDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice);
    virtual ~TextureCubeDx9();
    virtual MappedTexture2D lock(const ELockType lt, const int32 level, const ECubeMapFace cubeFace);
    virtual void unlock(const int32 level, const ECubeMapFace cubeFace);
    virtual void buildShaderResourceView();
    virtual IShaderResourceViewPtr getShaderResourceView();
    virtual void generateMipmaps();

protected:
    LPDIRECT3DCUBETEXTURE9 _pD3DTextureCube;
    ShaderResourceViewDx9Ptr _pSrv;
};
typedef Core::Ptr<TextureCubeDx9> TextureCubeDx9Ptr;
//-----------------------------------------------------------------------------
} //namepsace Gfx
#endif /* TEXTUREDX9_H_ */
