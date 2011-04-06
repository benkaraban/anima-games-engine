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
#include <Assets/Vertex.h>
#include <Renderer/SM2/BillboardGroup.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
BillboardGroup::BillboardGroup(int32 maxSize, EBillboardMode mode, const Ptr<ITextureMap> & pTexture, 
                               const Ptr<Gfx::IDevice> & pDevice, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _maxSize(maxSize),
    _mode(mode),
    _align(BILLBOARD_ALIGN_FACE_CAM),
    _alignVec(0.0f, 1.0f, 0.0f),
    _worldMatrix(Core::IDENTITY),
    _worldMatrixInv(Core::IDENTITY),
    _position(Core::ZERO),
    _pTexture(pTexture),
    _size(0),
    _partScale(1.0f),
    _colorFactor(1.0f),
    _glowFactor(0.0f),
    _isAlive(true),
    _isVisible(true),
    _spriteU(1),
    _spriteV(1),
    _spriteSmooth(false),
    _refrScale(1.0f),
    _zBias(0.0f),
    _fogFactor(1.0f)
{
    LM_ASSERT(maxSize <= MAX_BILLBOARD_COUNT);
    _pResLib->registerResource(this);
    _frame = 0;
}
//-----------------------------------------------------------------------------
BillboardGroup::~BillboardGroup()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void BillboardGroup::onDeviceLost()
{
    _pVBuffer = null;
}
//-----------------------------------------------------------------------------
void BillboardGroup::onDeviceReset()
{
    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, _maxSize * 4 * sizeof(BillboardVertex));
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);
}
//-----------------------------------------------------------------------------
void BillboardGroup::setMaxSize(int32 maxSize)
{
    if(_maxSize != maxSize)
    {
        onDeviceLost();
        _maxSize = maxSize;
        if(!_pResLib->isDeviceLost())
            onDeviceReset();
    }
}
//-----------------------------------------------------------------------------
int32 BillboardGroup::getMaxSize() const
{
    return _maxSize;
}
//-----------------------------------------------------------------------------
void BillboardGroup::setPartScale(float s)
{ 
    _partScale = s;
    updateLocalBoundingBox();
}
//-----------------------------------------------------------------------------
void BillboardGroup::updateLocalBoundingBox()
{
    _localBox.clear();

    float s = 0.5f;

    for(int32 ii=0; ii < _bills.size(); ii++)
    {
        float maxSize = std::max(_bills[ii].width * s, _bills[ii].height * s);

        Core::Vector3f vs(maxSize, maxSize, maxSize);
        Core::Vector3f p1(_bills[ii].position - vs);
        Core::Vector3f p2(_bills[ii].position + vs);

        _localBox.addPoint(p1);
        _localBox.addPoint(p2);
    }

    if(_localBox.isEmpty())
        _localBox.addPoint(Core::Vector3f::ZERO);

    _box.set(_localBox, _worldMatrix);
}
//-----------------------------------------------------------------------------
void BillboardGroup::endUpdate()
{
    _bills.resize(std::min(int32(_bills.size()), _maxSize));

    updateLocalBoundingBox();
}
//-----------------------------------------------------------------------------
void BillboardGroup::update(const Ptr<Gfx::IVertexBuffer> & pVBuffer, int32 & size)
{
    size = 4 * _bills.size();
    _vertices.resize(size);

    if(size > 0)
    {
        BillboardVertex * pVertices = reinterpret_cast<BillboardVertex *>(_vertices.data());

        for(int32 ii=0; ii < _bills.size(); ii++)
        {
            pVertices[0].position = _bills[ii].position;
            pVertices[0].size[0] = _bills[ii].width;
            pVertices[0].size[1] = _bills[ii].height;
            pVertices[0].angle = _bills[ii].angle;
            Assets::packColor(_bills[ii].color, pVertices[0].color);

            pVertices[3] = pVertices[2] = pVertices[1] = pVertices[0];

            pVertices[0].texcoords[0] = 0x00; pVertices[0].texcoords[1] = 0xFF;
            pVertices[1].texcoords[0] = 0xFF; pVertices[1].texcoords[1] = 0xFF;
            pVertices[2].texcoords[0] = 0xFF; pVertices[2].texcoords[1] = 0x00;
            pVertices[3].texcoords[0] = 0x00; pVertices[3].texcoords[1] = 0x00;

            uint8 t = Assets::packUInt8UN(_bills[ii].time);
            pVertices[0].texcoords[2] = t;
            pVertices[1].texcoords[2] = t;
            pVertices[2].texcoords[2] = t;
            pVertices[3].texcoords[2] = t;

            pVertices += 4;
        }

        void * pVBData = reinterpret_cast<BillboardVertex *>(pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0));
        memcpy(pVBData, _vertices.data(), sizeof(BillboardVertex) * _vertices.size());
        pVBuffer->unlock();
    }
}
//-----------------------------------------------------------------------------
void BillboardGroup::send(const Ptr<Gfx::IVertexBuffer> & pVBuffer, const Ptr<Gfx::IDevice> & pDevice, int32 size)
{
    if(size > 0)
    {
        pDevice->setVertexBuffer(0, pVBuffer, sizeof(BillboardVertex), 0);
        pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, size, 0, size / 2);
    }
}
//-----------------------------------------------------------------------------
void BillboardGroup::sendData(const Ptr<Gfx::IDevice> & pDevice)
{
    update(_pVBuffer, _size);
    send(_pVBuffer, pDevice, _size);
}
//-----------------------------------------------------------------------------
void BillboardGroup::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    _worldMatrixInv = matrix.getInverted();
    _position = matrix.getTVector();
    _box.set(_localBox, _worldMatrix);
}
//-----------------------------------------------------------------------------
float BillboardGroup::getSqDist(const Core::Vector3f & position) const
{
    return _position.getDistanceSquared(position);
}
//-----------------------------------------------------------------------------
struct DistAndIndex
{
    float dist;
    int32 index;
};
//-----------------------------------------------------------------------------
struct DistAndIndexSorter
{
    bool operator () (const DistAndIndex & d1, const DistAndIndex & d2)
    {
        return d2.dist < d1.dist;
    }
};
//-----------------------------------------------------------------------------
void BillboardGroup::sortByDist(const Core::Vector3f & viewPos, const Core::Vector3f & viewDir)
{
    DistAndIndexSorter sorter;
    Core::List<DistAndIndex> dists;
    Core::List<Billboard> bills;

    dists.resize(_bills.size());
    bills.resize(_bills.size());

    Core::Vector3f pos(_worldMatrixInv.apply(viewPos));
    Core::Vector3f dir(_worldMatrixInv.apply3x3(viewDir));

    for(int32 ii=0; ii < _bills.size(); ii++)
    {
        dists[ii].index = ii;
        dists[ii].dist = Core::dot(_bills[ii].position - pos, dir);
    }

    std::sort(dists.begin(), dists.end(), sorter);

    for(int32 ii=0; ii < bills.size(); ii++)
        bills[ii] = _bills[dists[ii].index];

    _bills.swap(bills);
}
//-----------------------------------------------------------------------------
void BillboardGroup::setSpriteLayout(int32 uCount, int32 vCount)
{
    _spriteU = uCount;
    _spriteV = vCount;
}
//-----------------------------------------------------------------------------
bool BillboardGroup::isAnimated() const 
{ 
    return (_pTexture != null && _pTexture->isAnimatedTexture()) ||
        _spriteU > 1 ||
        _spriteV > 1;
}
//-----------------------------------------------------------------------------
}
