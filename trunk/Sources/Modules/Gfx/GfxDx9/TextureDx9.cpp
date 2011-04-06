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
#include "TextureDx9.h"
#include <Gfx/GfxDx9/D3DTypeConvert.h>

#if LM_WATCH_TEX_FOR_RESET
#include <Core/Debug/CallStack.h>
#endif

namespace Gfx
{
#if LM_WATCH_TEX_FOR_RESET
Core::List<Texture2DDx9 *> Texture2DDx9::sTextures;
#endif
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
ShaderResourceViewDx9::ShaderResourceViewDx9(const ShaderResourceViewDesc& desc, void* ptrD3D) :
    IShaderResourceView(desc),
    _pD3DBaseTexture(NULL)
{
    LPUNKNOWN pD3DUnknow = static_cast<LPUNKNOWN>(ptrD3D);
    LPDIRECT3DTEXTURE9 pD3DTexture;
    LPDIRECT3DCUBETEXTURE9 pD3DTextureCube;
    LPDIRECT3DVOLUMETEXTURE9 pD3DVolumeTexture;
    HRESULT hresult;

    hresult = pD3DUnknow->QueryInterface (IID_IDirect3DTexture9, (void**)(&pD3DTexture));
    if (hresult== S_OK)
    {
        _pD3DBaseTexture = pD3DTexture;
    }
    else
    {
        hresult = pD3DUnknow->QueryInterface (IID_IDirect3DCubeTexture9, (void**)(&pD3DTextureCube));
        if (hresult== S_OK)
        {
            _pD3DBaseTexture = pD3DTextureCube;
        }
        else
        {
            hresult = pD3DUnknow->QueryInterface (IID_IDirect3DVolumeTexture9, (void**)(&pD3DVolumeTexture));
            if (hresult== S_OK)
            {
                _pD3DBaseTexture = pD3DVolumeTexture;
            }
            else
            {
                ERR << L"ShaderResourceViewDx9::createView not a ShaderResource";
            }
        }
    }
}
//-----------------------------------------------------------------------------
ShaderResourceViewDx9::~ShaderResourceViewDx9()
{
    LM_D3D_RELEASE(_pD3DBaseTexture);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture1DDx9::Texture1DDx9(const Texture1DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice):
ITexture1D(otherDesc),
_pD3DTexture(NULL)
{
    D3DPOOL D3Dpool;
    DWORD D3DUsage;
    D3DFORMAT D3DFormat;
    HRESULT hresult;

    LM_D3D_RELEASE(_pD3DTexture);

    D3DTypeConvert(_desc.bind, _desc.usage, _desc.cpuAccess, D3Dpool,  D3DUsage);
    D3DFormat = D3DTypeConvert(_desc.format);

    if(_desc.autoGenMipmap)
        D3DUsage |= D3DUSAGE_AUTOGENMIPMAP;

    hresult = pD3DDevice->CreateTexture(
        static_cast<UINT>(_desc.width),
        static_cast<UINT>(1),
        static_cast<UINT>(_desc.mipLevels),
        D3DUsage,
        D3DFormat,
        D3Dpool,
        &_pD3DTexture,
        NULL);
    if(hresult == D3DERR_OUTOFVIDEOMEMORY || hresult == E_OUTOFMEMORY)
        throw OutOfVideoMemoryException(L"not enough VRAM for texture");
    LM_HANDLE_D3D_ERROR(hresult, L"Texture1DDx9::createResource CreateTexture");

    buildShaderResourceView();
}
//-----------------------------------------------------------------------------
Texture1DDx9::~Texture1DDx9()
{
    if(_pD3DTexture != NULL)
    {
        int32 nbRef = _pD3DTexture->Release();
        _pD3DTexture = NULL;

        if (nbRef!=0)
        {
            ERR << L"Texture1DDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
void* Texture1DDx9::lock(const ELockType lt, const int32 level)
{
    D3DLOCKED_RECT pLockedRect;
    DWORD lockFlags = D3DTypeConvert(lt, _desc.usage);

    HRESULT hresult = _pD3DTexture->LockRect(level, &pLockedRect, NULL, lockFlags);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture1DDx9 _pD3DTexture LockRect");

    return pLockedRect.pBits;
}
//-----------------------------------------------------------------------------
void Texture1DDx9::unlock(const int32 level)
{
    HRESULT hresult = _pD3DTexture->UnlockRect(level);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture1DDx9 _pD3DTexture UnlockRect");
}
//-----------------------------------------------------------------------------
void Texture1DDx9::buildShaderResourceView()
{
    ShaderResourceViewDesc srvDesc;
    srvDesc.texture1D.mostDetailedMip = 0;

    _pSrv = (ShaderResourceViewDx9Ptr) new ShaderResourceViewDx9(srvDesc, _pD3DTexture);
}
//-----------------------------------------------------------------------------
IShaderResourceViewPtr Texture1DDx9::getShaderResourceView()
{
    return _pSrv;
}
//-----------------------------------------------------------------------------
void Texture1DDx9::generateMipmaps()
{
    _pD3DTexture->GenerateMipSubLevels();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SurfaceDx9::SurfaceDx9(const Texture2DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice) :
    ITexture2D(otherDesc),
    _pD3DSurface(NULL)
{
    HRESULT hresult;
    D3DPOOL D3Dpool;
    DWORD D3DUsage;
    D3DFORMAT D3DFormat;

    LM_D3D_RELEASE(_pD3DSurface);

    D3DTypeConvert(_desc.bind, _desc.usage, _desc.cpuAccess, D3Dpool,  D3DUsage);
    D3DFormat = D3DTypeConvert(_desc.format);

    if(_desc.bind == TB_DEPTH_STENCIL_SURFACE)
    {
        hresult = pD3DDevice->CreateDepthStencilSurface(
            static_cast<UINT>(_desc.width),
            static_cast<UINT>(_desc.height),
            D3DFormat,
            D3DTypeConvert(_desc.multiSampleType),
            0,
            FALSE,
            &_pD3DSurface,
            NULL);
        if(hresult == D3DERR_OUTOFVIDEOMEMORY || hresult == E_OUTOFMEMORY)
            throw OutOfVideoMemoryException(L"not enough VRAM for depth/stencil surface");
        LM_HANDLE_D3D_ERROR(hresult, L"SurfaceDx9::createResource TB_DEPTH_STENCIL_SURFACE : CreateDepthStencilSurface");
    }
    else
    {
        ERR << L"SurfaceDx9::createResource : not implemented";
    }
}
//-----------------------------------------------------------------------------
SurfaceDx9::~SurfaceDx9()
{
    if(_pD3DSurface != NULL)
    {
        int32 nbRef = _pD3DSurface->Release();
        _pD3DSurface = NULL;
        if (nbRef!=0)
        {
            ERR << L"SurfaceDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
MappedTexture2D SurfaceDx9::lock(const ELockType lt, const int32 level)
{
    MappedTexture2D ret;
    D3DLOCKED_RECT pLockedRect;
    DWORD lockFlags = D3DTypeConvert(lt, _desc.usage);

    HRESULT hresult = _pD3DSurface->LockRect(&pLockedRect, NULL, lockFlags);
    LM_HANDLE_D3D_ERROR(hresult, L"Surface2DDx9 _pD3DSurface LockRect");

    ret.pData = pLockedRect.pBits;
    ret.rowStep = pLockedRect.Pitch;

    return ret;
}
//-----------------------------------------------------------------------------
void SurfaceDx9::unlock(const int32 level)
{
    HRESULT hresult = _pD3DSurface->UnlockRect();
    LM_HANDLE_D3D_ERROR(hresult, L"Surface2DDx9 _pD3DSurface UnlockRect");
}
//-----------------------------------------------------------------------------
IShaderResourceViewPtr SurfaceDx9::getShaderResourceView()
{
    return IShaderResourceViewPtr(NULL);
}
//-----------------------------------------------------------------------------
void SurfaceDx9::generateMipmaps()
{
    // Rien à générer
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture2DDx9::Texture2DDx9(const Texture2DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice):
ITexture2D(otherDesc),
_pD3DTexture(NULL)
{
#if LM_WATCH_TEX_FOR_RESET
    sTextures.push_back(this);
    Core::getCallStack(_cs);
#endif

    D3DPOOL D3Dpool;
    DWORD D3DUsage;
    D3DFORMAT D3DFormat;
    HRESULT hresult;

    // Le MSAA est uniquement là pour créer des depth buffer, il n'est pas possible de créer une
    // texture en MSAA sous D3D9. (par contre on peut créer une rendertarget).
    LM_ASSERT(otherDesc.multiSampleType == MSAA_NONE);

    D3DTypeConvert(_desc.bind, _desc.usage, _desc.cpuAccess, D3Dpool,  D3DUsage);
    D3DFormat = D3DTypeConvert(_desc.format);

    if(_desc.autoGenMipmap)
        D3DUsage |= D3DUSAGE_AUTOGENMIPMAP;

    hresult = pD3DDevice->CreateTexture(
        static_cast<UINT>(_desc.width),
        static_cast<UINT>(_desc.height),
        static_cast<UINT>(_desc.mipLevels),
        D3DUsage,
        D3DFormat,
        D3Dpool,
        &_pD3DTexture,
        NULL);
    if(hresult == D3DERR_OUTOFVIDEOMEMORY || hresult == E_OUTOFMEMORY)
        throw OutOfVideoMemoryException(L"not enough VRAM for texture");
    LM_HANDLE_D3D_ERROR(hresult, L"Texture2DDx9::createResource CreateTexture");
    buildShaderResourceView();
}
//-----------------------------------------------------------------------------
Texture2DDx9::~Texture2DDx9()
{
#if LM_WATCH_TEX_FOR_RESET
    Core::List<Texture2DDx9 *>::const_iterator iTex = std::find(sTextures.begin(), sTextures.end(), this);
    if(iTex != sTextures.end())
        sTextures.qerase(iTex - sTextures.begin());
#endif

    _pSrv = null;

    if(_pD3DTexture != NULL)
    {
        int32 nbRef = _pD3DTexture->Release();
        _pD3DTexture = NULL;

        if (nbRef!=0)
        {
            ERR << L"Texture2DDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
MappedTexture2D Texture2DDx9::lock(const ELockType lt, const int32 level)
{
    MappedTexture2D ret;
    D3DLOCKED_RECT pLockedRect;
    DWORD lockFlags = D3DTypeConvert(lt, _desc.usage);

    HRESULT hresult = _pD3DTexture->LockRect(level, &pLockedRect, NULL, lockFlags);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture2DDx9 _pD3DTexture LockRect");

    ret.pData = pLockedRect.pBits;
    ret.rowStep = pLockedRect.Pitch;

    return ret;
}
//-----------------------------------------------------------------------------
void Texture2DDx9::unlock(const int32 level)
{
    HRESULT hresult = _pD3DTexture->UnlockRect(level);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture2DDx9 _pD3DTexture UnlockRect");
}
//-----------------------------------------------------------------------------
void Texture2DDx9::buildShaderResourceView()
{
    ShaderResourceViewDesc srvDesc;
    srvDesc.texture2D.mostDetailedMip = 0;
    _pSrv = (ShaderResourceViewDx9Ptr) new ShaderResourceViewDx9 (srvDesc, _pD3DTexture);
}
//-----------------------------------------------------------------------------
IShaderResourceViewPtr Texture2DDx9::getShaderResourceView()
{
    return _pSrv;
}
//-----------------------------------------------------------------------------
void Texture2DDx9::generateMipmaps()
{
    _pD3DTexture->GenerateMipSubLevels();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Texture3DDx9::Texture3DDx9(const Texture3DDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice):
ITexture3D(otherDesc),
_pD3DVolumeTexture(NULL)
{
    D3DPOOL D3Dpool;
    DWORD D3DUsage;
    D3DFORMAT D3DFormat;
    HRESULT hresult;

    LM_D3D_RELEASE(_pD3DVolumeTexture);

    D3DTypeConvert(_desc.bind, _desc.usage, _desc.cpuAccess, D3Dpool,  D3DUsage);
    D3DFormat = D3DTypeConvert(_desc.format);

    if(_desc.autoGenMipmap)
        D3DUsage |= D3DUSAGE_AUTOGENMIPMAP;

    hresult = pD3DDevice->CreateVolumeTexture(
        static_cast<UINT>(_desc.width),
        static_cast<UINT>(_desc.height),
        static_cast<UINT>(_desc.nbSlice),
        static_cast<UINT>(_desc.mipLevels),
        D3DUsage,
        D3DFormat,
        D3Dpool,
        &_pD3DVolumeTexture,
        NULL);
    if(hresult == D3DERR_OUTOFVIDEOMEMORY || hresult == E_OUTOFMEMORY)
        throw OutOfVideoMemoryException(L"not enough VRAM for volume texture");
    LM_HANDLE_D3D_ERROR(hresult, L"Texture3DDx9::createResource CreateVolumeTexture");
    buildShaderResourceView();
}
//-----------------------------------------------------------------------------
Texture3DDx9::~Texture3DDx9()
{
    if(_pD3DVolumeTexture != NULL)
    {
        int32 nbRef = _pD3DVolumeTexture->Release();
        _pD3DVolumeTexture = NULL;

        if (nbRef!=0)
        {
            ERR << L"_pD3DVolumeTexture::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
MappedTexture3D Texture3DDx9::lock(const ELockType lt, const int32 level)
{
    MappedTexture3D ret;
    D3DLOCKED_BOX lockedBox;
    DWORD lockFlags = D3DTypeConvert(lt, _desc.usage);

    HRESULT hresult = _pD3DVolumeTexture->LockBox(level, &lockedBox, NULL, lockFlags);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture3DDx9 _pD3DVolumeTexture LockBox");

    ret.pData = lockedBox.pBits;
    ret.rowStep = lockedBox.RowPitch;
    ret.sliceStep = lockedBox.SlicePitch;

    return ret;
}
//-----------------------------------------------------------------------------
void Texture3DDx9::unlock(const int32 level)
{
    HRESULT hresult = _pD3DVolumeTexture->UnlockBox(level);
    LM_HANDLE_D3D_ERROR(hresult, L"Texture3DDx9 _pD3DVolumeTexture UnlockBox");
}
//-----------------------------------------------------------------------------
void Texture3DDx9::buildShaderResourceView()
{
    ShaderResourceViewDesc srvDesc;
    srvDesc.texture3D.mostDetailedMip = 0;
    _pSrv = (ShaderResourceViewDx9Ptr) new ShaderResourceViewDx9 (srvDesc, _pD3DVolumeTexture);
}
//-----------------------------------------------------------------------------
IShaderResourceViewPtr Texture3DDx9::getShaderResourceView()
{
    return _pSrv;
}
//-----------------------------------------------------------------------------
void Texture3DDx9::generateMipmaps()
{
    _pD3DVolumeTexture->GenerateMipSubLevels();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
TextureCubeDx9::TextureCubeDx9(const TextureCubeDesc& otherDesc, LPDIRECT3DDEVICE9 pD3DDevice) :
    ITextureCube(otherDesc),
    _pD3DTextureCube(NULL)
{
    D3DPOOL D3Dpool;
    DWORD D3DUsage;
    D3DFORMAT D3DFormat;
    HRESULT hresult;

    LM_D3D_RELEASE(_pD3DTextureCube);

    D3DTypeConvert(_desc.bind, _desc.usage, _desc.cpuAccess, D3Dpool,  D3DUsage);
    D3DFormat = D3DTypeConvert(_desc.format);

    if(_desc.autoGenMipmap)
        D3DUsage |= D3DUSAGE_AUTOGENMIPMAP;

    hresult = pD3DDevice->CreateCubeTexture(static_cast<UINT>(_desc.edgeLength),
                                            static_cast<UINT>(_desc.mipLevels),
                                            D3DUsage,
                                            D3DFormat,
                                            D3Dpool,
                                            &_pD3DTextureCube,
                                            NULL);
    if(hresult == D3DERR_OUTOFVIDEOMEMORY || hresult == E_OUTOFMEMORY)
        throw OutOfVideoMemoryException(L"not enough VRAM for cube texture");
    LM_HANDLE_D3D_ERROR(hresult, L"CreateCubeTexture");
    buildShaderResourceView();
}
//-----------------------------------------------------------------------------
TextureCubeDx9::~TextureCubeDx9()
{
    _pSrv = null;

    if(_pD3DTextureCube != NULL)
    {
        int32 nbRef = _pD3DTextureCube->Release();
        _pD3DTextureCube = NULL;

        if (nbRef!=0)
        {
            ERR << L"TextureCubeDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
MappedTexture2D TextureCubeDx9::lock(const ELockType lt, const int32 level, const ECubeMapFace cubeFace)
{
    MappedTexture2D ret;
    D3DLOCKED_RECT pLockedRect;
    DWORD lockFlags = D3DTypeConvert(lt, _desc.usage);
    D3DCUBEMAP_FACES D3DCubeFace = D3DTypeConvert(cubeFace);

    HRESULT hresult = _pD3DTextureCube->LockRect(D3DCubeFace, level, &pLockedRect, NULL, lockFlags);
    LM_HANDLE_D3D_ERROR(hresult, L"TextureCubeDx9 _pD3DTextureCube LockRect");

    ret.pData = pLockedRect.pBits;
    ret.rowStep = pLockedRect.Pitch;

    return ret;
}
//-----------------------------------------------------------------------------
void TextureCubeDx9::unlock(const int32 level, const ECubeMapFace cubeFace)
{
    D3DCUBEMAP_FACES D3DCubeFace = D3DTypeConvert(cubeFace);
    HRESULT hresult = _pD3DTextureCube->UnlockRect(D3DCubeFace, level);
    LM_HANDLE_D3D_ERROR(hresult, L"TextureCubeDx9 _pD3DTextureCube UnlockRect");
}
//-----------------------------------------------------------------------------
void TextureCubeDx9::buildShaderResourceView()
{
    ShaderResourceViewDesc srvDesc;
    srvDesc.textureCube.empty = 0;
    _pSrv = (ShaderResourceViewDx9Ptr) new ShaderResourceViewDx9(srvDesc, _pD3DTextureCube);
}
//-----------------------------------------------------------------------------
IShaderResourceViewPtr TextureCubeDx9::getShaderResourceView()
{
    return _pSrv;
}
//-----------------------------------------------------------------------------
void TextureCubeDx9::generateMipmaps()
{
    _pD3DTextureCube->GenerateMipSubLevels();
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
} //namepsace Gfx
