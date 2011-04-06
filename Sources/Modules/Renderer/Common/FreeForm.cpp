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
#include <Renderer/Common/FreeForm.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
FreeForm::FreeForm(int32 maxVertices, int32 maxIndices, EFreeFormMode mode, const Ptr<ITextureMap> & pTexture, 
                   const Ptr<StreamBuffer> & pBuffer) :
    _pBuffer(pBuffer),
    _isAlive(true),
    _isVisible(true),
    _glowFlag(true),
    _maxVertices(maxVertices),
    _maxIndices(maxIndices),
    _mode(mode),
    _worldMatrix(Core::IDENTITY),
    _pTexture(pTexture),
    _usePrecomputedAABox(false),
    _isWorldSpaceCoords(false)
{
}
//-----------------------------------------------------------------------------
FreeForm::FreeForm(int32 maxVertices, int32 maxIndices, EFreeFormMode mode, const Ptr<ITextureMap> & pTexture, 
                   const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _isAlive(true),
    _isVisible(true),
    _maxVertices(maxVertices),
    _maxIndices(maxIndices),
    _mode(mode),
    _worldMatrix(Core::IDENTITY),
    _pTexture(pTexture),
    _usePrecomputedAABox(false)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
FreeForm::~FreeForm()
{
    if(_pResLib != null)
        _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void FreeForm::onDeviceLost()
{
    if(_pResLib != null)
    {
        _pVBuffer = null;
        _pIBuffer = null;
    }
}
//-----------------------------------------------------------------------------
void FreeForm::onDeviceReset()
{
    if(_pResLib != null)
    {
        Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, _maxVertices * sizeof(FreeFormVertex));
        Gfx::IndexBufferDesc  ibuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, _maxIndices * sizeof(uint16));

        _pVBuffer = _pDevice->createVertexBuffer(vbuff);
        _pIBuffer = _pDevice->createIndexBuffer(ibuff);

        _isDirty = true;
    }
}
//-----------------------------------------------------------------------------
void FreeForm::setMax(int32 maxVert, int32 maxIndices)
{
    if(maxVert != _maxVertices || maxIndices != _maxIndices)
    {
        onDeviceLost();
        _maxVertices = maxVert;
        _maxIndices = maxIndices;
        _data.vertices.resize(std::min(_data.vertices.size(), _maxVertices));
        _data.indices.resize(std::min(_data.indices.size(), _maxIndices));
        _isDirty = true;
        if(_pResLib != null && !_pResLib->isDeviceLost())
            onDeviceReset();
    }
}
//-----------------------------------------------------------------------------
void FreeForm::getMax(int32 & maxVert, int32 & maxIndices)
{
    maxVert = _maxVertices;
    maxIndices = _maxIndices;
}
//-----------------------------------------------------------------------------
void FreeForm::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    if(!_usePrecomputedAABox)
        _box.set(_localBox, _worldMatrix);
}
//-----------------------------------------------------------------------------
void FreeForm::setPrecomputedAABox(const Core::AABoxf & box)
{
    _box = box;
    _usePrecomputedAABox = true;
}
//-----------------------------------------------------------------------------
void FreeForm::endUpdate()
{
    _data.vertices.resize(std::min(_data.vertices.size(), _maxVertices));
    _data.indices.resize(std::min(_data.indices.size(), _maxIndices));

    if(!_usePrecomputedAABox)
    {
        _localBox.clear();

        for(int32 ii=0; ii < _data.vertices.size(); ii++)
            _localBox.addPoint(_data.vertices[ii].position);

        if(_localBox.isEmpty())
            _localBox.addPoint(Core::Vector3f::ZERO);       

        _box.set(_localBox, _worldMatrix);
    }

    _isDirty = true;
}
//-----------------------------------------------------------------------------
void FreeForm::sendData() const
{
    if(_pResLib != null)
    {
        // Own buffer

        if(_isDirty && _data.indices.size() > 0 && _data.vertices.size() > 0)
        {
            void * pVData = _pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0);
            memcpy(pVData, _data.vertices.data(), _data.vertices.size() * sizeof(FreeFormVertex));
            _pVBuffer->unlock();

            void * pIData = _pIBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0);
            memcpy(pIData, _data.indices.data(), _data.indices.size() * sizeof(uint16));
            _pIBuffer->unlock();

            _isDirty = false;
        }

        if(_data.indices.size() > 0)
        {
            _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(FreeFormVertex), 0);
            _pDevice->setIndexBuffer(_pIBuffer);
            _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _data.vertices.size(), 0, _data.indices.size() / 3);
        }
    }
    else
    {
        // Shared buffer

        if(_data.vertices.size() > 0 && _data.indices.size() > 0)
            _pBuffer->send(_data.vertices.data(), _data.indices.data(), _data.vertices.size(), _data.indices.size());
    }
}
//-----------------------------------------------------------------------------
}
