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
#include <Renderer/Common/GrassCluster.h>

namespace Renderer
{
LM_STATIC_ASSERT(sizeof(GrassVertex) == 32);

GrassCluster::GrassCluster(const Ptr<Gfx::IDevice> & pDevice,
                           const Core::List<GrassPatch> & patches,
                           const Ptr<ITextureMap> & pTexture, 
                           const Ptr<GPUResourceLib> & pResLib)
:
    _pDevice(pDevice),
    _pResLib(pResLib),
    _patches(patches),
    _isDirty(true),
    _pTexture(pTexture),
    _worldMatrix(Core::IDENTITY),
    _isAlive(true),
    _isVisible(true)
{
    _pResLib->registerResource(this);
}

GrassCluster::~GrassCluster()
{
    _pResLib->unregisterResource(this);
}

void GrassCluster::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}

void GrassCluster::onDeviceReset()
{
    int32 vertexCount = _patches.size() * VERTEX_BY_GRASS;
    int32 vertexBufferSize = vertexCount * sizeof(GrassVertex);

    int32 indiceCount = _patches.size() * 4 * 6; // 4 quads de 3 triangles (6 indices)
    int32 indiceBufferSize = indiceCount * sizeof(uint16);

    // Buffers

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vertexBufferSize);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, indiceBufferSize);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0, jj=0; ii < indiceCount; ii += 6, jj += 4)
    {
        pIndices[0] = jj + 0; pIndices[1] = jj + 1; pIndices[2] = jj + 2;
        pIndices[3] = jj + 0; pIndices[4] = jj + 2; pIndices[5] = jj + 3;

        pIndices += 6;
    }

    _pIBuffer->unlock();

    updateVertices();
}

void GrassCluster::updateVertices()
{
    _box.clear();
    _vertices.resize(_patches.size() * VERTEX_BY_GRASS);

    GrassVertex * vertices = &(*_vertices.begin());

    uint32 texCoordsUint1 = 0x0000FF00;
    uint32 texCoordsUint2 = 0x0000FFFF;
    uint32 texCoordsUint3 = 0x000000FF;
    uint32 texCoordsUint4 = 0x00000000;

    for(int32 ii=0; ii < _patches.size(); ii++)
    {
        Core::Vector3f  center(_patches[ii].centerPos);
        Core::Vector3f  vh(0.0, _patches[ii].height, 0.0);

        Core::Vector3f  v00(-_patches[ii].radius, 5.0f, 0.0f);
        Core::Vector3f  v01( _patches[ii].radius, 5.0f, 0.0f);

        Core::Matrix4f mat1(Core::ROTATION, _patches[ii].teta + (0.0f / 3.0f) * f_PI, Core::ROT_Y_YAW);
        Core::Vector3f v10(center + (mat1 * v00));
        Core::Vector3f v11(center + (mat1 * v01));

        Core::Matrix4f mat2(Core::ROTATION, _patches[ii].teta + (1.0f / 3.0f) * f_PI, Core::ROT_Y_YAW);
        Core::Vector3f v20(center + (mat2 * v00));
        Core::Vector3f v21(center + (mat2 * v01));

        Core::Matrix4f mat3(Core::ROTATION, _patches[ii].teta + (2.0f / 3.0f) * f_PI, Core::ROT_Y_YAW);
        Core::Vector3f v30(center + (mat3 * v00));
        Core::Vector3f v31(center + (mat3 * v01));

        for(int32 jj=0; jj < 12; jj += 4)
        {
            vertices[jj + 0].texCoordsUint = texCoordsUint1;
            vertices[jj + 1].texCoordsUint = texCoordsUint2;
            vertices[jj + 2].texCoordsUint = texCoordsUint3;
            vertices[jj + 3].texCoordsUint = texCoordsUint4;

            vertices[jj + 0].centerPos = center;
            vertices[jj + 1].centerPos = center;
            vertices[jj + 2].centerPos = center;
            vertices[jj + 3].centerPos = center;
        }

        vertices[ 0].position = v10;
        vertices[ 1].position = v11;
        vertices[ 2].position = v11 + vh;
        vertices[ 3].position = v10 + vh;


        vertices[ 4].position = v20;
        vertices[ 5].position = v21;
        vertices[ 6].position = v21 + vh;
        vertices[ 7].position = v20 + vh;

        vertices[ 8].position = v30;
        vertices[ 9].position = v31;
        vertices[10].position = v31 + vh;
        vertices[11].position = v30 + vh;

        vertices += VERTEX_BY_GRASS;
    }

    for(Core::List<GrassVertex>::iterator iVert = _vertices.begin(); iVert != _vertices.end(); ++iVert)
    {
        iVert->position = _worldMatrix.apply(iVert->position);
        iVert->centerPos = _worldMatrix.apply(iVert->centerPos);
        _box.addPoint(iVert->position);
    }

    LM_ASSERT(_vertices.size() <= Core::UINT16_MAX);

    _isDirty = true;
}

void GrassCluster::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    updateVertices();
}

void GrassCluster::sendData(const Ptr<Gfx::IDevice> & pDevice)
{
    if(_isDirty)
    {
        void * pData = _pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0);
        memcpy(pData, &(*_vertices.begin()), _vertices.size() * sizeof(GrassVertex));
        _pVBuffer->unlock();

        _isDirty = false;
    }

    pDevice->setVertexBuffer(0, _pVBuffer, sizeof(GrassVertex), 0);
    pDevice->setIndexBuffer(_pIBuffer);
    pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vertices.size(), 0, _vertices.size() / 2);
}
}
