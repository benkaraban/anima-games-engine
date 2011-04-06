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
#include <Core/Logger.h>
#include <Renderer/StreamBuffer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
StreamBuffer::StreamBuffer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib, int32 vertexSize, int32 vertexCount, int32 indexCount) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _vertexSize(vertexSize),
    _vertexCount(vertexCount),
    _indexSize(sizeof(uint16)),
    _indexCount(indexCount)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
StreamBuffer::~StreamBuffer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void StreamBuffer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void StreamBuffer::onDeviceReset()
{
    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, _vertexCount * _vertexSize);
    Gfx::IndexBufferDesc  ibuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, _indexCount * _indexSize);

    _pVBuffer = _pDevice->createVertexBuffer(vbuff);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    _iVertex = 0;
    _iIndex = 0;
}
//-----------------------------------------------------------------------------
void StreamBuffer::send(const void * pVertices, const void * pIndices, int32 verticeCount, int32 indicesCount)
{
    Gfx::ELockType vertexLockFlags = Gfx::LOCK_WRITE_NO_OVERWRITE;
    Gfx::ELockType indexLockFlags = Gfx::LOCK_WRITE_NO_OVERWRITE;
    
    int32 vStart = _iVertex * _vertexSize;
    int32 vSize = verticeCount * _vertexSize;

    int32 iStart = _iIndex * _indexSize;
    int32 iSize = indicesCount * _indexSize;

    if(verticeCount > (_vertexCount - _iVertex))
    {
        vertexLockFlags = Gfx::LOCK_WRITE_DISCARD;
        _iVertex = 0;
        vStart = 0;
        vSize = 0;
    }

    if(indicesCount > (_indexCount - _iIndex))
    {
        indexLockFlags = Gfx::LOCK_WRITE_DISCARD;
        _iIndex = 0;
        iStart = 0;
        iSize = 0;
    }

    void * pVerticesBuffer = _pVBuffer->lock(vertexLockFlags, vStart, vSize);
    memcpy(pVerticesBuffer, pVertices, verticeCount * _vertexSize);
    _pVBuffer->unlock();

    void * pIndicesBuffer = _pIBuffer->lock(indexLockFlags, iStart, iSize);
    memcpy(pIndicesBuffer, pIndices, indicesCount * _indexSize);
    _pIBuffer->unlock();
    
    _pDevice->setVertexBuffer(0, _pVBuffer, 32, 0);
    _pDevice->setIndexBuffer(_pIBuffer);
    _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _iVertex, 0, verticeCount, _iIndex, indicesCount / 3);

    _iVertex += verticeCount;
    _iIndex += indicesCount;
}
//-----------------------------------------------------------------------------
}
