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
#include "IndexBufferDx9.h"
#include <Gfx/gfxDx9/D3DTypeConvert.h>

namespace Gfx
{
//-----------------------------------------------------------------------------
IndexBufferDx9::IndexBufferDx9(const IndexBufferDesc& desc, LPDIRECT3DDEVICE9 pD3DDevice) :
    IIndexBuffer(desc),
    _pD3DIndexBuffer(NULL)
{
    D3DPOOL D3Dpool;
    DWORD D3DUsage;

    LM_D3D_RELEASE(_pD3DIndexBuffer);

    D3DTypeConvert(_desc.usage, _desc.cpuAccess, D3Dpool, D3DUsage);


    HRESULT hresult = pD3DDevice->CreateIndexBuffer(
                static_cast<UINT>(_desc.size),
                D3DUsage,
                D3DTypeConvert(_desc.format),
                D3Dpool,
                &_pD3DIndexBuffer,
                NULL);
    LM_HANDLE_D3D_ERROR(hresult, L"IndexBufferDx9::createResource");
}
//-----------------------------------------------------------------------------
IndexBufferDx9::~IndexBufferDx9()
{
    if(_pD3DIndexBuffer != NULL)
    {
        int32 nbRef = _pD3DIndexBuffer->Release();
        _pD3DIndexBuffer = NULL;

        if (nbRef!=0)
        {
            ERR << L"IndexBufferDx9::releaseResource() : resource not released";
        }
    }
}
//-----------------------------------------------------------------------------
void* IndexBufferDx9::lock(const ELockType lt, const int32 offset, const int32 size)
{
    void* pBuffer;
    HRESULT hresult = _pD3DIndexBuffer->Lock(offset, size, &pBuffer, D3DTypeConvert(lt, _desc.usage));
    LM_HANDLE_D3D_ERROR(hresult, L"IndexBufferLock");

    return pBuffer;
}
//-----------------------------------------------------------------------------
void IndexBufferDx9::unlock()
{
    HRESULT hresult = _pD3DIndexBuffer->Unlock();
    LM_HANDLE_D3D_ERROR(hresult, L"IndexBufferUnlock");
}
//-----------------------------------------------------------------------------
} // namespace Gfx
