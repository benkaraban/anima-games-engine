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
#include <Gfx/GfxDx9/DeviceDx9.h>
#include <winsafe.h>
#include <Core/Standard.h>
#include <Core/Logger.h>
#include <Core/Map.h>
#include <Gfx/GfxDx9/SwapChainDx9.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
SwapChainDx9::SwapChainDx9(const SwapChainDesc& desc, LPDIRECT3DSWAPCHAIN9 D3DswapChain, LPDIRECT3DDEVICE9 pD3DDevice) :
    ISwapChain(desc), _D3DswapChain(D3DswapChain)
{
}
//-----------------------------------------------------------------------------
SwapChainDx9::~SwapChainDx9()
{
    if(_D3DswapChain != NULL)
    {
        int32 nbRef = _D3DswapChain->Release();
        _D3DswapChain = NULL;

        if (nbRef!=0)
        {
            ERR << L"SwapChainDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
void SwapChainDx9::present()
{
    HRESULT hresult = _D3DswapChain->Present( NULL, NULL, static_cast<HWND>(_desc.handleWindow), NULL, 0 );

    if(hresult != D3DERR_DEVICELOST)
        LM_HANDLE_D3D_ERROR(hresult, L"present -> Present");
}
//-----------------------------------------------------------------------------
} // namespace Gfx


