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
#include <Core/Logger.h>
#include <Renderer/Common/ScreenQuad.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
struct QuadVertex
{
    Core::Vector4f  position;
    union
    {
        byte            color[4];
        uint32          colorWord;
    };
    Core::Vector2f  texcoords;

    byte            padding[4];
};

const int32 VERTEX_BUFFER_SIZE = 4 * sizeof(QuadVertex);
const int32 INDEX_BUFFER_SIZE  = 6 * sizeof(uint16);

LM_STATIC_ASSERT(sizeof(QuadVertex) == 32);

//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
ScreenQuad::ScreenQuad(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
ScreenQuad::~ScreenQuad()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool ScreenQuad::initialise()
{
    bool result = true;

    try
    {
        // Shaders

        Gfx::VertexFormatDesc  formatDesc;
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT4, Gfx::VAU_POSITIONT);
        formatDesc.addAttribut(0, Gfx::VAT_COLOR,  Gfx::VAU_COLOR, 0);
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

        _pFormat = _pDevice->createVertexFormat(formatDesc, Ptr<Gfx::IVertexShader>(null));
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing ScreenQuad : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void ScreenQuad::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void ScreenQuad::onDeviceReset()
{
    // Buffers

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, VERTEX_BUFFER_SIZE);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, INDEX_BUFFER_SIZE);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
    pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void ScreenQuad::send(int32 x1, int32 y1, int32 x2, int32 y2,
                      float u1, float v1, float u2, float v2, 
                      const Core::Vector4f & color) const
{
    _pDevice->setVertexFormat(_pFormat);
    _pDevice->setVertexShader(Ptr<Gfx::IVertexShader>(null));//_pVShader);

    float fx1 = float(x1) - 0.5f;
    float fx2 = float(x2) - 0.5f;
    float fy1 = float(y1) - 0.5f;
    float fy2 = float(y2) - 0.5f;

    union
    {
        byte            colorBytes[4];
        uint32          colorWord;
    };

    Assets::packColor(color, colorBytes);

    QuadVertex * pVert = reinterpret_cast<QuadVertex*>(_pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0));

    pVert[0].position = Core::Vector4f(fx1, fy1, 1.0, 1.0); pVert[0].texcoords = Core::Vector2f(u1, v1);
    pVert[1].position = Core::Vector4f(fx2, fy1, 1.0, 1.0); pVert[1].texcoords = Core::Vector2f(u2, v1);
    pVert[2].position = Core::Vector4f(fx2, fy2, 1.0, 1.0); pVert[2].texcoords = Core::Vector2f(u2, v2);
    pVert[3].position = Core::Vector4f(fx1, fy2, 1.0, 1.0); pVert[3].texcoords = Core::Vector2f(u1, v2);

    pVert[0].colorWord = pVert[1].colorWord = pVert[2].colorWord = pVert[3].colorWord = colorWord;

    _pVBuffer->unlock();

    _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(QuadVertex), 0);
    _pDevice->setIndexBuffer(_pIBuffer);
    _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, 4, 0, 2); // 4 vertices, 2 triangles
}
//-----------------------------------------------------------------------------
void ScreenQuad::send(int32 x1, int32 y1, int32 x2, int32 y2, const Core::Vector4f & color) const
{
    send(x1, y1, x2, y2, 0.0f, 0.0f, 1.0f, 1.0f, color);
}
//-----------------------------------------------------------------------------
void ScreenQuad::send(int32 width, int32 height, const Core::Vector4f & color) const
{
    send(0, 0, width, height, 0.0f, 0.0f, 1.0f, 1.0f, color);
}
//-----------------------------------------------------------------------------
}
