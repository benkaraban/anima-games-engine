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
#ifndef ITEXTURE_H_
#define ITEXTURE_H_

#include <Core/Standard.h>

#include <Gfx/GfxType.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GFX IShaderResourceView
{
protected:
    IShaderResourceView(const ShaderResourceViewDesc& desc);

public:
    virtual ~IShaderResourceView();

protected:
    ShaderResourceViewDesc _desc;
};
typedef Core::Ptr<IShaderResourceView> IShaderResourceViewPtr;

//-----------------------------------------------------------------------------
class LM_API_GFX ITexture1D
{
protected:
    ITexture1D(const Texture1DDesc& otherDesc);
public:
    virtual ~ITexture1D();
    virtual Texture1DDesc getDesc() const;
    virtual void* lock(const ELockType lt, const int32 level) = 0;
    virtual void unlock(const int32 level) = 0;
    virtual IShaderResourceViewPtr getShaderResourceView() = 0;
    virtual void generateMipmaps() = 0;

protected:
    Texture1DDesc _desc;
};
typedef Core::Ptr<ITexture1D> ITexture1DPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX ITexture2D
{
protected:
    ITexture2D(const Texture2DDesc& otherDesc);
public:
    virtual ~ITexture2D();
    virtual Texture2DDesc getDesc() const;
    virtual MappedTexture2D lock(const ELockType lt, const int32 level) = 0;
    virtual void unlock(const int32 level) = 0;
    virtual IShaderResourceViewPtr getShaderResourceView() = 0;
    virtual void generateMipmaps() = 0;

protected:
    Texture2DDesc _desc;
};
typedef Core::Ptr<ITexture2D> ITexture2DPtr;
//-----------------------------------------------------------------------------
class LM_API_GFX ITexture3D
{
protected:
    ITexture3D(const Texture3DDesc& otherDesc);
public:
    virtual ~ITexture3D();
    virtual Texture3DDesc getDesc() const;
    virtual MappedTexture3D lock(const ELockType lt, const int32 level) = 0;
    virtual void unlock(const int32 level) = 0;
    virtual IShaderResourceViewPtr getShaderResourceView() = 0;
    virtual void generateMipmaps() = 0;

protected:
    Texture3DDesc _desc;
};
typedef Core::Ptr<ITexture3D> ITexture3DPtr;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class LM_API_GFX ITextureCube
{
protected:
    ITextureCube(const TextureCubeDesc& otherDesc);
public:
    virtual ~ITextureCube();
    TextureCubeDesc getDesc() const;
    virtual MappedTexture2D lock(const ELockType lt, const int32 level, const ECubeMapFace cubeFace) = 0;
    virtual void unlock(const int32 level, const ECubeMapFace cubeFace) = 0;
    virtual IShaderResourceViewPtr getShaderResourceView() = 0;
    virtual void generateMipmaps() = 0;

protected:
    TextureCubeDesc _desc;
};
typedef Core::Ptr<ITextureCube> ITextureCubePtr;
//-----------------------------------------------------------------------------
} //namepsace Gfx
#endif /* ITEXTURE_H_ */
