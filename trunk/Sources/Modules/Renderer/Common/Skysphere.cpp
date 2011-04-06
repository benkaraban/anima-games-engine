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
#include <Assets/GeometricPrimitive.h>
#include <Renderer/Common/Skysphere.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
LM_STATIC_ASSERT(sizeof(Skysphere::Vertex) == 32);
//-----------------------------------------------------------------------------
Skysphere::Skysphere(const Ptr<Gfx::IDevice> & pDevice, int32 divLevel, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pResLib(pResLib),
    _divLevel(divLevel),
    _isBufferUpdateNeeded(true)
{
    generateSphere(_settings);
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
Skysphere::~Skysphere()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void Skysphere::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void Skysphere::onDeviceReset()
{
    int32 vertexBufferSize = sizeof(Vertex) * _vertices.getVertices().size();
    int32 indexBufferSize = sizeof(uint16) * _vertices.getIndices().size();

    // Index buffer

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, indexBufferSize);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    const Core::List<uint16> & indices = _vertices.getIndices();

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    ::memcpy(pIndices, &(*indices.begin()), indexBufferSize);
    _pIBuffer->unlock();


    // Vertex buffer

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vertexBufferSize);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    _isBufferUpdateNeeded = true;
}
//-----------------------------------------------------------------------------
void Skysphere::update(const SkysphereSettings & settings)
{
    if((settings.horizon != _settings.horizon) ||
        (settings.skyRoof != _settings.skyRoof) ||
        (settings.sphericity != _settings.sphericity))
    {
        generateSphere(settings);
        _isBufferUpdateNeeded = true;
    }

    _settings = settings;
}
//-----------------------------------------------------------------------------
void Skysphere::generateSphere(const SkysphereSettings & settings)
{
    //
    //  Zenith +____
    //         |    \___
    //         |        \___
    //  Viewer +------------*  
    //         |          .  \
    //         | beta  .      \
    //         |    .          |
    //         | .    alpha    |
    //  Origin +---------------+

    float zv = std::max(0.01f, settings.sphericity);
    float ov = 1.0f - zv;

    float scale = settings.skyRoof / zv;

    float alpha = Core::L_asin(ov);
    float beta = f_PI_DIV_2 - alpha;

    Core::Matrix4f matT(Core::TRANSLATION, Core::Vector3f(0.0f, -ov * scale, 0.0f));
    Core::Matrix4f matS(Core::SCALE, Core::Vector3f(scale, scale, scale));
    Core::Matrix4f matW(matT * matS);

    float maxv = 2.0f;
    float denom = 1.0f - settings.horizon;

    if(denom != 0.0f)
        maxv = 2.0f / denom;

    Assets::createSkySphere(_vertices, _divLevel, 2.0f, 0.0f, maxv, 2.0f * beta);

    Core::List<Assets::Vertex> & verts = _vertices.getVertices();

    for(int32 ii=0; ii < verts.size(); ii++)
        verts[ii].position = matW.apply(verts[ii].position);
}
//-----------------------------------------------------------------------------
void Skysphere::sendData(const Ptr<Gfx::IDevice> & pDevice) const
{
    if(_isBufferUpdateNeeded)
    {
        const Core::List<Assets::Vertex> & verts = _vertices.getVertices();

        Vertex * pVerts = reinterpret_cast<Vertex*>(_pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

        for(int32 ii=0; ii < verts.size(); ii++)
        {
            pVerts[ii].position = verts[ii].position;
            pVerts[ii].texcoords[0] = verts[ii].texCoords.x;
            pVerts[ii].texcoords[1] = verts[ii].texCoords.y;
        }

        _pVBuffer->unlock();
        _isBufferUpdateNeeded = false;
    }

    pDevice->setVertexBuffer(0, _pVBuffer, sizeof(Vertex), 0);
    pDevice->setIndexBuffer(_pIBuffer);

    const Core::List<Assets::Vertex> & verts = _vertices.getVertices();
    const Core::List<uint16> & indices = _vertices.getIndices();

    pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, verts.size(), 0, indices.size() / 3);
}
//-----------------------------------------------------------------------------
}
