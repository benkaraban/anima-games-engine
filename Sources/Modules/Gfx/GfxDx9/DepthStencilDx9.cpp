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
#include "DepthStencilDx9.h"

namespace Gfx
{
//-----------------------------------------------------------------------------
DepthStencilViewDx9::DepthStencilViewDx9(DepthStencilViewDesc desc, void* ptrD3D) :
    IDepthStencilView(desc),
    _D3Dcanvas(NULL)
{
    LPUNKNOWN pD3DUnknow = static_cast<LPUNKNOWN>(ptrD3D);
    LPDIRECT3DSURFACE9 pD3Dsurface;
    LPDIRECT3DTEXTURE9 pD3DTexture;
    HRESULT hresult;

    hresult = pD3DUnknow->QueryInterface (IID_IDirect3DSurface9, (void**)(&pD3Dsurface));
    if (hresult== S_OK)
    {
        _D3Dcanvas = pD3Dsurface;
        pD3Dsurface->AddRef();

        pD3Dsurface->Release();
    }
    else
    {
        hresult = pD3DUnknow->QueryInterface (IID_IDirect3DTexture9, (void**)(&pD3DTexture));
        if (hresult== S_OK)
        {
            hresult = pD3DTexture->GetSurfaceLevel(_desc.texture2D.mipSlice, &_D3Dcanvas);
            LM_HANDLE_D3D_ERROR(hresult, L"DepthStencilViewDx9::eventForView_RestoreDevice : GetSurfaceLevel");

            pD3DTexture->Release();
        }
        else
        {
            ERR << L"DepthStencilViewDx9::createView not a DepthStencil";
        }
    }
}
//-----------------------------------------------------------------------------
DepthStencilViewDx9::~DepthStencilViewDx9()
{
    LM_D3D_RELEASE(_D3Dcanvas);
}
//-----------------------------------------------------------------------------
} //namespace Gfx
