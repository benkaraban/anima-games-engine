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
#include "VertexBufferDx9.h"
#include <Gfx/GfxDx9/D3DTypeConvert.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
VertexBufferDx9::VertexBufferDx9(const VertexBufferDesc& desc, LPDIRECT3DDEVICE9 pD3DDevice) :
    IVertexBuffer(desc),
    _pD3DBuffer(NULL)
{
    D3DPOOL D3Dpool;
    DWORD D3DUsage;

    LM_D3D_RELEASE(_pD3DBuffer);

    D3DTypeConvert(_desc.usage, _desc.cpuAccess, D3Dpool, D3DUsage);

    HRESULT hresult = pD3DDevice->CreateVertexBuffer(
                    static_cast<UINT>(_desc.size),
                    D3DUsage,
                    0,
                    D3Dpool,
                    &_pD3DBuffer,
                    NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"VertexBufferDx9::createResource CreateVertexBuffer");
}
//-----------------------------------------------------------------------------
VertexBufferDx9::~VertexBufferDx9()
{
    if(_pD3DBuffer != NULL)
    {
        int32 nbRef = _pD3DBuffer->Release();
        _pD3DBuffer = NULL;

        if(nbRef != 0)
        {
            ERR << L"VertexBufferDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
void* VertexBufferDx9::lock(const ELockType lt, const int32 offset, const int32 size)
{
    void* pBuffer;
    HRESULT hresult = _pD3DBuffer->Lock(offset, size, &pBuffer, D3DTypeConvert(lt, _desc.usage));
    LM_HANDLE_D3D_ERROR(hresult, L"VertexBufferLock");

    return pBuffer;
}
//-----------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9 VertexBufferDx9::getD3DVertexBuffer()
{
    return _pD3DBuffer;
}
//-----------------------------------------------------------------------------
void VertexBufferDx9::unlock()
{
    HRESULT hresult = _pD3DBuffer->Unlock();
    LM_HANDLE_D3D_ERROR(hresult, L"VertexBufferUnlock");
}
//-----------------------------------------------------------------------------
} //namespace Gfx
