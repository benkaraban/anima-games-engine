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
#include <Renderer/Common/PackedVerticesFLOAT16PN.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
LM_STATIC_ASSERT(sizeof(PackedVerticesFormatFLOAT16PN::VertexSkinGB) == 32);
LM_STATIC_ASSERT(sizeof(PackedVerticesFormatFLOAT16PN::VertexStaticGB) == 32);
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PackedStaticVerticesFLOAT16PN::PackedStaticVerticesFLOAT16PN(const Ptr<Gfx::IDevice> & pDevice, 
                                                             const Ptr<GPUResourceLib> & pResLib,
                                                             const Ptr<Assets::VerticeSet> & pVertices)
: _pDevice(pDevice),
  _pResLib(pResLib),
  _pVertices(pVertices),
  _vertexCount(0),
  _triangleCount(0)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
PackedStaticVerticesFLOAT16PN::~PackedStaticVerticesFLOAT16PN()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void PackedStaticVerticesFLOAT16PN::onDeviceLost()
{
    _pBufferGB = null;
    _pBufferIndex = null;
}
//-----------------------------------------------------------------------------
void PackedStaticVerticesFLOAT16PN::onDeviceReset()
{
    const Core::List<Assets::Vertex> & vertices = _pVertices->getVertices();
    const Core::List<uint16> & indices = _pVertices->getIndices();

    Gfx::VertexBufferDesc descGB(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vertices.size() * sizeof(PackedVerticesFormatFLOAT16PN::VertexStaticGB));
    Gfx::IndexBufferDesc  descIndex(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, indices.size() * sizeof(uint16));

    _pBufferGB = _pDevice->createVertexBuffer(descGB);
    _pBufferIndex = _pDevice->createIndexBuffer(descIndex);

    PackedVerticesFormatFLOAT16PN::VertexStaticGB * pGB = static_cast<PackedVerticesFormatFLOAT16PN::VertexStaticGB*>(_pBufferGB->lock(Gfx::LOCK_WRITE, 0, 0));
    
    for(int32 ii=0; ii < vertices.size(); ii++)
    {
        pGB[ii].position = vertices[ii].position;
        Assets::packFloat16Vec2(vertices[ii].texCoords, pGB[ii].texcoord);
        Assets::packUInt8Vec3(vertices[ii].normale, pGB[ii].normal);
        Assets::packUInt8Vec3(vertices[ii].tangent, pGB[ii].tangent);

        Core::Vector3f cmpB(Core::cross(vertices[ii].normale, vertices[ii].tangent));
        cmpB.normalizeSafe();

        if(Core::dot(cmpB, vertices[ii].bitangent) > 0.0)
            pGB[ii].tangent[3] = Assets::packUInt8N(1.0);
        else
            pGB[ii].tangent[3] = Assets::packUInt8N(-1.0);
    }

    _pBufferGB->unlock();

    uint16 * pIndex = static_cast<uint16*>(_pBufferIndex->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0; ii < indices.size(); ii++)
        pIndex[ii] = indices[ii];

    _pBufferIndex->unlock();

    _vertexCount = vertices.size();
    _triangleCount = indices.size() / 3;
}
//-----------------------------------------------------------------------------
void PackedStaticVerticesFLOAT16PN::bindData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const
{
    if(_pBufferGB != null && _pBufferIndex != null)
    {
        switch(data)
        {
        case VD_STATIC_G:
        case VD_STATIC_G_B:
            pDevice->setVertexBuffer(0, _pBufferGB, sizeof(PackedVerticesFormatFLOAT16PN::VertexStaticGB), 0);
            break;
        }
        pDevice->setIndexBuffer(_pBufferIndex);
    }
}
//-----------------------------------------------------------------------------
void PackedStaticVerticesFLOAT16PN::sendData(const Ptr<Gfx::IDevice> & pDevice, EStaticVertexStreamData data) const
{
    pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vertexCount, 0, _triangleCount);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PackedSkinnedVerticesFLOAT16PN::PackedSkinnedVerticesFLOAT16PN(const Ptr<Gfx::IDevice> & pDevice, 
                                                               const Ptr<GPUResourceLib> & pResLib,
                                                               const Ptr<Assets::VerticeSet> & pVertices) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _pVertices(pVertices),
    _vertexCount(0),
    _triangleCount(0)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
PackedSkinnedVerticesFLOAT16PN::~PackedSkinnedVerticesFLOAT16PN()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void PackedSkinnedVerticesFLOAT16PN::onDeviceLost()
{
    _pBufferGB = null;
    _pBufferIndex = null;
}
//-----------------------------------------------------------------------------
void PackedSkinnedVerticesFLOAT16PN::onDeviceReset()
{
    const Core::List<Assets::Vertex> & vertices = _pVertices->getVertices();
    const Core::List<uint16> & indices = _pVertices->getIndices();

    Gfx::VertexBufferDesc descGB(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vertices.size() * sizeof(PackedVerticesFormatFLOAT16PN::VertexSkinGB));
    Gfx::IndexBufferDesc  descIndex(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, indices.size() * sizeof(uint16));

    _pBufferGB = _pDevice->createVertexBuffer(descGB);
    _pBufferIndex = _pDevice->createIndexBuffer(descIndex);

    PackedVerticesFormatFLOAT16PN::VertexSkinGB * pGB = static_cast<PackedVerticesFormatFLOAT16PN::VertexSkinGB*>(_pBufferGB->lock(Gfx::LOCK_WRITE, 0, 0));
    int32 maxBone = 0;
    
    for(int32 ii=0; ii < vertices.size(); ii++)
    {
        pGB[ii].position = vertices[ii].position;
        Assets::packFloat16Vec2(vertices[ii].texCoords, pGB[ii].texcoord);
        Assets::packUInt8Vec3(vertices[ii].normale, pGB[ii].normal);

        for(int32 iBone=0; iBone < 4; iBone++)
        {
            pGB[ii].bonesId[iBone] = vertices[ii].boneId[iBone];
            pGB[ii].bonesWeight[iBone] = Assets::packUInt8UN(vertices[ii].boneWeight[iBone]);

            maxBone = std::max(maxBone, vertices[ii].boneId[iBone]);
        }

        Assets::packUInt8Vec3(vertices[ii].tangent, pGB[ii].tangent);

        Core::Vector3f cmpB(Core::cross(vertices[ii].normale, vertices[ii].tangent));
        cmpB.normalizeSafe();

        if(Core::dot(cmpB, vertices[ii].bitangent) > 0.0)
            pGB[ii].tangent[3] = Assets::packUInt8N(1.0);
        else
            pGB[ii].tangent[3] = Assets::packUInt8N(-1.0);
    }

    _pBufferGB->unlock();

    uint16 * pIndex = static_cast<uint16*>(_pBufferIndex->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0; ii < indices.size(); ii++)
        pIndex[ii] = indices[ii];

    _pBufferIndex->unlock();

    _groups.clear();

    if(_pVertices->getBlendingGroups().empty())
    {
        Core::List<int32> palette;
        palette.resize(maxBone + 1);

        for(int32 ii=0; ii < palette.size(); ii++)
            palette[ii] = ii;

        _groups.push_back(Assets::BlendingGroup(0, indices.size(), palette));
    }
    else
    {
        _groups = _pVertices->getBlendingGroups();
    }

    _vertexCount = vertices.size();
    _triangleCount = indices.size() / 3;
}
//-----------------------------------------------------------------------------
void PackedSkinnedVerticesFLOAT16PN::bindData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data) const
{
    if(_pBufferGB != null && _pBufferIndex != null)
    {
        switch(data)
        {
        case VD_SKIN_G:
        case VD_SKIN_G_B:
            pDevice->setVertexBuffer(0, _pBufferGB, sizeof(PackedVerticesFormatFLOAT16PN::VertexSkinGB), 0);
            break;
        }
        pDevice->setIndexBuffer(_pBufferIndex);
    }
}
//-----------------------------------------------------------------------------
int32 PackedSkinnedVerticesFLOAT16PN::getBlendingGroupCount() const
{
    return _groups.size();
}
//-----------------------------------------------------------------------------
const Core::List<int32> PackedSkinnedVerticesFLOAT16PN::getPalette(int32 groupID) const
{
    return _groups[groupID].getMatrixPalette();
}
//-----------------------------------------------------------------------------
void PackedSkinnedVerticesFLOAT16PN::sendData(const Ptr<Gfx::IDevice> & pDevice, ESkinnedVertexStreamData data, int32 groupID) const
{
    pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vertexCount, _groups[groupID].getStartIndice(), _groups[groupID].getIndiceCount() / 3);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
PackedVerticesFormatFLOAT16PN::PackedVerticesFormatFLOAT16PN(const Ptr<GPUResourceLib> & pResLib) :
    _pResLib(pResLib)
{
    // Skinned vertices

    _descSkinG.addAttribut(0, Gfx::VAT_FLOAT3,    Gfx::VAU_POSITION);
    _descSkinG.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_NORMAL);
    _descSkinG.addAttribut(0, Gfx::VAT_FLOAT16_2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    _descSkinG.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_UNUSED);
    _descSkinG.addAttribut(0, Gfx::VAT_UBYTE4,    Gfx::VAU_TEXTURE_COORD, 6); // bones id
    _descSkinG.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_TEXTURE_COORD, 7); // bones weight

    _descSkinGB.addAttribut(0, Gfx::VAT_FLOAT3,    Gfx::VAU_POSITION);
    _descSkinGB.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_NORMAL);
    _descSkinGB.addAttribut(0, Gfx::VAT_FLOAT16_2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    _descSkinGB.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_TEXTURE_COORD, 4); // tangent
    _descSkinGB.addAttribut(0, Gfx::VAT_UBYTE4,    Gfx::VAU_TEXTURE_COORD, 6); // bones id
    _descSkinGB.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_TEXTURE_COORD, 7); // bones weight

    // Static vertices

    _descStaticG.addAttribut(0, Gfx::VAT_FLOAT3,    Gfx::VAU_POSITION);
    _descStaticG.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_NORMAL);
    _descStaticG.addAttribut(0, Gfx::VAT_FLOAT16_2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    _descStaticGB.addAttribut(0, Gfx::VAT_FLOAT3,    Gfx::VAU_POSITION);
    _descStaticGB.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_NORMAL);
    _descStaticGB.addAttribut(0, Gfx::VAT_FLOAT16_2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    _descStaticGB.addAttribut(0, Gfx::VAT_UBYTE4N,   Gfx::VAU_TEXTURE_COORD, 4); // tangent
}
//-----------------------------------------------------------------------------
bool PackedVerticesFormatFLOAT16PN::initialise(const Ptr<Gfx::IDevice> & pDevice)
{
    Gfx::GraphicCapabilities caps;
    pDevice->getCaps(caps);

    //return (caps.supportFLOAT16_2 && caps.supportSHORT4N && caps.supportUBYTE4 && caps.supportUBYTE4N);
    return true;
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IVertexFormat> PackedVerticesFormatFLOAT16PN::getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            EStaticVertexStreamData data)
{
    Ptr<Gfx::IVertexFormat> pResult;
    switch(data)
    {
    case VD_STATIC_G:     pResult = pDevice->createVertexFormat(_descStaticG, pShader); break;
    case VD_STATIC_G_B:   pResult = pDevice->createVertexFormat(_descStaticGB, pShader); break;
    }
    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IVertexFormat> PackedVerticesFormatFLOAT16PN::getVertexFormat(
            const Ptr<Gfx::IDevice> & pDevice,
            const Ptr<Gfx::IVertexShader> & pShader,
            ESkinnedVertexStreamData data)
{
    Ptr<Gfx::IVertexFormat> pResult;
    switch(data)
    {
    case VD_SKIN_G:   pResult = pDevice->createVertexFormat(_descSkinG, pShader); break;
    case VD_SKIN_G_B: pResult = pDevice->createVertexFormat(_descSkinGB, pShader); break;
    }
    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<IPackedStaticVertices> PackedVerticesFormatFLOAT16PN::packStaticVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices)
{

    return Ptr<IPackedStaticVertices>(new PackedStaticVerticesFLOAT16PN(pDevice, _pResLib, pVertices));
}
//-----------------------------------------------------------------------------
Ptr<IPackedSkinnedVertices> PackedVerticesFormatFLOAT16PN::packSkinnedVerts(const Ptr<Gfx::IDevice> & pDevice, const Ptr<Assets::VerticeSet> & pVertices)
{

    return Ptr<IPackedSkinnedVertices>(new PackedSkinnedVerticesFLOAT16PN(pDevice, _pResLib, pVertices));
}
//-----------------------------------------------------------------------------
bool PackedVerticesFormatFLOAT16PN::packedNormals() const
{
    return true;
}
//-----------------------------------------------------------------------------
}


