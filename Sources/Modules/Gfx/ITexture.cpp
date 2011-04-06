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
#include "ITexture.h"

namespace Gfx
{
//*****************************************************************************
//-----------------------------------------------------------------------------
IShaderResourceView::IShaderResourceView(const ShaderResourceViewDesc& desc) :
    _desc(desc)
{
}
//-----------------------------------------------------------------------------
IShaderResourceView::~IShaderResourceView()
{
}
//*****************************************************************************
//-----------------------------------------------------------------------------
ITexture1D::ITexture1D(const Texture1DDesc& otherDesc) :
    _desc(otherDesc)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ITexture1D::~ITexture1D()
{
}
//-----------------------------------------------------------------------------
Texture1DDesc ITexture1D::getDesc() const
{
    return _desc;
}
//*****************************************************************************
//-----------------------------------------------------------------------------
ITexture2D::ITexture2D(const Texture2DDesc& otherDesc) :
    _desc(otherDesc)
{
}
//-----------------------------------------------------------------------------
ITexture2D::~ITexture2D()
{
}
//-----------------------------------------------------------------------------
Texture2DDesc ITexture2D::getDesc() const
{
    return _desc;
}
//*****************************************************************************
//-----------------------------------------------------------------------------
ITexture3D::ITexture3D(const Texture3DDesc& otherDesc) :
    _desc(otherDesc)
{
}
//-----------------------------------------------------------------------------
ITexture3D::~ITexture3D()
{
}
//-----------------------------------------------------------------------------
Texture3DDesc ITexture3D::getDesc() const
{
    return _desc;
}
//*****************************************************************************
//-----------------------------------------------------------------------------
ITextureCube::ITextureCube(const TextureCubeDesc& otherDesc):
    _desc(otherDesc)
{
}
//-----------------------------------------------------------------------------
ITextureCube::~ITextureCube()
{
}
//-----------------------------------------------------------------------------
TextureCubeDesc ITextureCube::getDesc() const
{
    return _desc;
}
//-----------------------------------------------------------------------------
} //namepsace Gfx

