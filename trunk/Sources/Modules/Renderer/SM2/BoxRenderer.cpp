/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include <Renderer/SM2/BoxRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
struct BoxVertex
{
   Core::Vector3f  position;
   byte            padding[20];
};

LM_STATIC_ASSERT(sizeof(BoxVertex) == 32);

static const int32 VERTEX_BUFFER_SIZE = 8 * sizeof(BoxVertex);
static const int32 INDEX_BUFFER_SIZE  = 3 * 4 * 2 * sizeof(uint16); // 3 carrés, 4 seg par carré (avant/arrière + 4 de lien)

//-----------------------------------------------------------------------------
BoxRenderer::BoxRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
BoxRenderer::~BoxRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool BoxRenderer::initialise()
{
    bool result = true;

    try
    {
        // Shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pVShader = _pShaderLib->getVShader(L"box.vsh", Gfx::VS_V1_1, L"vs_main", macros);

        _pVConst = _pVShader->getConstantTable();

        _idWorldViewProj = _pVConst->getConstantIndex(L"WorldViewProj");
        _idColor = _pVConst->getConstantIndex(L"Color");

        // Format

        Gfx::VertexFormatDesc  formatDesc;
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
        _pFormat = _pDevice->createVertexFormat(formatDesc, _pVShader);

        // State

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_WIREFRAME);
        Gfx::RSDepthStencilDesc depth(true, true, Gfx::COMPARISON_LESS_EQUAL);
        Gfx::RSBlendDesc        blend;

        _state = _pDevice->createState(raster, depth, blend);

        onDeviceReset();
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing BoxRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void BoxRenderer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void BoxRenderer::onDeviceReset()
{
    // Buffers

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, VERTEX_BUFFER_SIZE);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, INDEX_BUFFER_SIZE);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    BoxVertex * pVert = reinterpret_cast<BoxVertex*>(_pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    static const float SIZE = 0.5f;

    // FRONT
    pVert[ 0].position = Core::Vector3f(-SIZE, -SIZE,  SIZE);
    pVert[ 1].position = Core::Vector3f( SIZE, -SIZE,  SIZE);
    pVert[ 2].position = Core::Vector3f( SIZE,  SIZE,  SIZE);
    pVert[ 3].position = Core::Vector3f(-SIZE,  SIZE,  SIZE);

    // BACK
    pVert[ 4].position = Core::Vector3f(-SIZE, -SIZE, -SIZE);
    pVert[ 5].position = Core::Vector3f( SIZE, -SIZE, -SIZE);
    pVert[ 6].position = Core::Vector3f( SIZE,  SIZE, -SIZE);
    pVert[ 7].position = Core::Vector3f(-SIZE,  SIZE, -SIZE);

    _pVBuffer->unlock();

    uint16 * pInd = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    int32 ii = 0;

    // FRONT
    pInd[ii++] =  0; pInd[ii++] =  1;
    pInd[ii++] =  1; pInd[ii++] =  2;
    pInd[ii++] =  2; pInd[ii++] =  3;
    pInd[ii++] =  3; pInd[ii++] =  0;

    // BACK
    pInd[ii++] =  4; pInd[ii++] =  5;
    pInd[ii++] =  5; pInd[ii++] =  6;
    pInd[ii++] =  6; pInd[ii++] =  7;
    pInd[ii++] =  7; pInd[ii++] =  4;

    // LINK
    pInd[ii++] =  4; pInd[ii++] =  0;
    pInd[ii++] =  5; pInd[ii++] =  1;
    pInd[ii++] =  6; pInd[ii++] =  2;
    pInd[ii++] =  7; pInd[ii++] =  3;

    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void BoxRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _viewProj = context.getViewProj();

    _commands.clear();

    if(pass == PASS_LIGHTING)
    {
        const Core::List<AABox *> & boxes = context.getAABoxes();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_SOLID;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < boxes.size(); ii++)
        {
            const AABox & box = *boxes[ii];

            command.camDist   = Core::dot((box.getBox().getCenter() - context.getEye()), context.getEyeDir());
            command.pExecData = (void*)&box;

            _commands.push_back(command);
        }
    }
}
//-----------------------------------------------------------------------------
void BoxRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void BoxRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void BoxRenderer::exec(Command * pStart, Command * pEnd)
{
    _pDevice->setState(_state);
    _pDevice->setVertexFormat(_pFormat);
    _pDevice->setVertexShader(_pVShader);
    _pDevice->setPixelShader(null);
    _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(BoxVertex), 0);
    _pDevice->setIndexBuffer(_pIBuffer);

    Gfx::RSFixedFunc ff;
    ff.fogEnabled = false;
    _pDevice->setFixedFuncPipe(ff);

    while(pStart != pEnd)
    {
        const AABox & aabox = *(AABox*)pStart->pExecData;
        const Core::AABoxf & box = aabox.getBox();

        if(!box.isEmpty())
        {
            Core::Vector3f scale(box.getSize());
            Core::Vector3f trans(box.getCenter());
            Core::Matrix4f world(
                    scale.x,   0.0,     0.0,     trans.x,
                    0.0,       scale.y, 0.0,     trans.y,
                    0.0,       0.0,     scale.z, trans.z,
                    0.0,       0.0,     0.0,     1.0);
            Core::Matrix4f worldViewProj(_viewProj * world);

            _pVConst->setConstant(_idWorldViewProj, worldViewProj);
            _pVConst->setConstant(_idColor, aabox.getColor());

            _pDevice->drawIndexed(Gfx::PT_LINELIST, 8, 0, 12); // 8 vertices, 12 triangles
        }

        pStart ++;
    }
}
//-----------------------------------------------------------------------------
}
