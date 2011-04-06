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
#include "RenderStateDx9.h"
#include <Gfx/GfxDx9/D3DTypeConvert.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
VertexFormatDx9::VertexFormatDx9(const VertexFormatDesc & desc, const IVertexShaderPtr & pShader, LPDIRECT3DDEVICE9 pD3Ddevice)
    : _desc(desc),
      _pShader(pShader),
      _D3DVertexElements(D3DTypeConvert(desc))
{
    HRESULT hresult = pD3Ddevice->CreateVertexDeclaration(&(_D3DVertexElements[0]), &_pD3DVertexDeclaration);
    LM_HANDLE_D3D_ERROR(hresult, L"CreateVertexDeclaration");
}
//-----------------------------------------------------------------------------
VertexFormatDx9::~VertexFormatDx9()
{
    LM_ASSERT(_pD3DVertexDeclaration != NULL);
    LM_D3D_RELEASE(_pD3DVertexDeclaration);
    _pD3DVertexDeclaration = NULL;
}
//-----------------------------------------------------------------------------
VertexFormatDesc VertexFormatDx9::getDesc() const
{
    return _desc;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
RSRasterizerDx9::RSRasterizerDx9(const RSRasterizerDesc& otherDesc) :
    IRSRasterizer(otherDesc)
{
}

RSRasterizerDx9::~RSRasterizerDx9()
{

}

RSBlendDx9::RSBlendDx9(const RSBlendDesc& otherDesc) :
    IRSBlend(otherDesc)
{
}

RSBlendDx9::~RSBlendDx9()
{

}



RSDepthStencilDx9::RSDepthStencilDx9(const RSDepthStencilDesc& otherDesc) :
    IRSDepthStencil(otherDesc)
{
}

RSDepthStencilDx9::~RSDepthStencilDx9()
{

}



RSSamplerDx9::RSSamplerDx9(const RSSamplerDesc& otherDesc) :
    IRSSampler(otherDesc)
{
}

RSSamplerDx9::~RSSamplerDx9()
{

}

} // namespace Gfx
