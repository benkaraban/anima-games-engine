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
#include <Renderer/SM2/SplineRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
LM_STATIC_ASSERT(sizeof(SplineRenderer::SplineVertex) == 32);

static const int32 MAX_SPLINE_POINTS = 1000;

static const int32 VERTEX_BUFFER_SIZE = MAX_SPLINE_POINTS * sizeof(SplineRenderer::SplineVertex);
static const int32 INDEX_BUFFER_SIZE  = MAX_SPLINE_POINTS * sizeof(uint16);

//-----------------------------------------------------------------------------
SplineRenderer::SplineRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
SplineRenderer::~SplineRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool SplineRenderer::initialise()
{
    bool result = true;

    try
    {
        // Shaders

        Core::List<Gfx::ShaderMacro> macros;

        _pVShader = _pShaderLib->getVShader(L"spline.vsh", Gfx::VS_V1_1, L"vs_main", macros);

        _pVConst = _pVShader->getConstantTable();

        _idViewProj = _pVConst->getConstantIndex(L"ViewProj");
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
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing SplineRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void SplineRenderer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void SplineRenderer::onDeviceReset()
{
    // Buffers

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, VERTEX_BUFFER_SIZE);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, INDEX_BUFFER_SIZE);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pInd = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0; ii < MAX_SPLINE_POINTS; ii++)
        pInd[ii] = uint16(ii);

    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void SplineRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _viewProj = context.getViewProj();

    _commands.clear();

    if(pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        const Core::List<Spline *> & splines = context.getSplines();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_SOLID;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < splines.size(); ii++)
        {
            const Spline & spline = *splines[ii];

            command.camDist   = Core::dot((spline.getBoundingBox().getCenter() - context.getEye()), context.getEyeDir());
            command.pExecData = (void*)&spline;

            _commands.push_back(command);
        }
    }
}
//-----------------------------------------------------------------------------
void SplineRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void SplineRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void SplineRenderer::exec(Command * pStart, Command * pEnd)
{
    _pDevice->setState(_state);
    _pDevice->setVertexFormat(_pFormat);
    _pDevice->setVertexShader(_pVShader);
    _pDevice->setPixelShader(null);
    _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(SplineVertex), 0);
    _pDevice->setIndexBuffer(_pIBuffer);

    Gfx::RSFixedFunc ff;
    ff.fogEnabled = false;
    _pDevice->setFixedFuncPipe(ff);

    while(pStart != pEnd)
    {
        const Spline & spline = *(Spline*)pStart->pExecData;
        const Core::List<Core::Vector3f> & points = spline.getPoints();

        _vertices.resize(points.size());

        for(int32 ii=0; ii < points.size(); ii++)
            _vertices[ii].position = points[ii];

        void * pBufferData = _pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0);
        memcpy(pBufferData, _vertices.data(), _vertices.size() * sizeof(SplineVertex));
        _pVBuffer->unlock();

        _pVConst->setConstant(_idViewProj, _viewProj);
        _pVConst->setConstant(_idColor, spline.getColor());

        _pDevice->drawIndexed(Gfx::PT_LINESTRIP, points.size(), 0, points.size() - 1);

        pStart ++;
    }
}
//-----------------------------------------------------------------------------
}
