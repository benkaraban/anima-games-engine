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
#ifndef RENDERSTATEDX9_H_
#define RENDERSTATEDX9_H_

#include <Gfx/IRenderState.h>
#include <Gfx/GfxDx9/dx9Header.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
class LM_API_GX9 VertexFormatDx9 : public IVertexFormat
{
public:
    VertexFormatDx9(const VertexFormatDesc & desc, const IVertexShaderPtr & pShader, LPDIRECT3DDEVICE9 pD3Ddevice);
    virtual ~VertexFormatDx9();
    virtual VertexFormatDesc getDesc() const;

protected:
    VertexFormatDesc _desc;
    IVertexShaderPtr _pShader;
    Core::List<D3DVERTEXELEMENT9 >    _D3DVertexElements;
    IDirect3DVertexDeclaration9*      _pD3DVertexDeclaration;

    friend class DeviceDx9;
};
typedef Core::Ptr<VertexFormatDx9 > VertexFormatDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 RSRasterizerDx9 : public IRSRasterizer
{
public:
    RSRasterizerDx9(const RSRasterizerDesc& otherDesc);
    virtual ~RSRasterizerDx9();
};
typedef Core::Ptr<RSRasterizerDx9 > RSRasterizerDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 RSBlendDx9 : public IRSBlend
{
public:
    RSBlendDx9(const RSBlendDesc& otherDesc);
    virtual ~RSBlendDx9();
};
typedef Core::Ptr<RSBlendDx9 > RSBlendDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 RSDepthStencilDx9 : public IRSDepthStencil
{
public:
    RSDepthStencilDx9(const RSDepthStencilDesc& otherDesc);
    virtual ~RSDepthStencilDx9();
};
typedef Core::Ptr<RSDepthStencilDx9 > RSDepthStencilDx9Ptr;
//-----------------------------------------------------------------------------
class LM_API_GX9 RSSamplerDx9 : public IRSSampler
{
public:
    RSSamplerDx9(const RSSamplerDesc& otherDesc);
    virtual ~RSSamplerDx9();
};
typedef Core::Ptr<RSSamplerDx9 > RSSamplerDx9Ptr;
//-----------------------------------------------------------------------------

} // namespace Gfx

#endif /* RENDERSTATEDX9_H_ */
