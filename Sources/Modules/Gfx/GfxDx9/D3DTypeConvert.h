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
#ifndef D3DTYPECONVERT_H_
#define D3DTYPECONVERT_H_

#include <Core/Bitmap.h>
#include <Gfx/GfxDx9/dx9Header.h>
#include <Gfx/GfxType.h>
#include <Gfx/ISwapChain.h>

namespace Gfx
{

inline DWORD F2DW( FLOAT f ) { return *((DWORD*)&f); }

//-----------------------------------------------------------------------------
D3DTEXTUREADDRESS   D3DTypeConvert(const ETextureAddressMode adressMode, const D3DCAPS9& caps);
D3DSTENCILOP        D3DTypeConvert(const EStencilOperation stencilOp);
D3DCMPFUNC          D3DTypeConvert(const EComparisonFunc func);
D3DBLENDOP          D3DTypeConvert(const EBlendOperation blendOp);
D3DBLEND            D3DTypeConvert(const EBlendMode blendMode);
D3DCULL             D3DTypeConvert(const ECullMode cullMode, const bool frontFaceIsCounterClockwise);
D3DFILLMODE         D3DTypeConvert(const EFillMode fillMode);
D3DPRIMITIVETYPE    D3DTypeConvert(const EPrimitiveType pt);
D3DDECLTYPE         D3DTypeConvert(const EVertexAttributType vat);
D3DDECLUSAGE        D3DTypeConvert(const EVertexAttributUsage vau);

Core::List<D3DVERTEXELEMENT9> D3DTypeConvert(const VertexFormatDesc desc);
Core::EBitmapFormat D3DTypeConvert(const D3DFORMAT D3Dformat);

D3DFORMAT           D3DTypeConvert(const EIndexBufferFormat ibf);
DWORD               D3DTypeConvert(const ELockType lt, const EBufferUsage& vbu);
D3DVIEWPORT9        D3DTypeConvert(const Viewport& vp);
D3DFORMAT           D3DTypeConvert(const ETextureFormat& tf);
D3DCUBEMAP_FACES    D3DTypeConvert(const ECubeMapFace& cubeMapFace);
D3DFORMAT           D3DTypeConvert(EDisplayPixelFormat format);
D3DMULTISAMPLE_TYPE D3DTypeConvert(EMultiSamplingType ms);

void D3DTypeConvert(const EBufferUsage usage, EBufferCpuAccess cpuAccess, D3DPOOL& D3Dpool, DWORD& D3DUsage);
void D3DTypeConvert(const ETextureBind& bind, const EBufferUsage& usage, EBufferCpuAccess cpuAccess, D3DPOOL& D3Dpool, DWORD& D3DUsage);

void D3DTypeConvert(D3DPRESENT_PARAMETERS & presentParam, const SwapChainDesc & desc);

//-----------------------------------------------------------------------------
} // namespace Gfx

#endif /* D3DTYPECONVERT_H_ */
