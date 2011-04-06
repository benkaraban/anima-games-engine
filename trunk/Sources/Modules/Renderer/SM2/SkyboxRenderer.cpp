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
#include <Core/TGA.h>

#include <Assets/GeometricPrimitive.h>

#include <Renderer/Common/Tools.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/SM2/SkyboxRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
SkyboxRenderer::SkyboxRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
SkyboxRenderer::~SkyboxRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::initialise(ShaderParams & params, ERenderPass pass)
{

    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    switch(pass)
    {
    case PASS_WIREFRAME:
    case PASS_STATIC_SHADOW:
    case PASS_DYNAMIC_SHADOW:
    case PASS_LIGHTING:
    case PASS_REFLECTION:
    case PASS_GLOW:
    case PASS_DEBUG:
    case PASS_LIGHTMAP_GENERATION:
    {
        params.pVShader = _pShaderLib->getVShader(L"skybox.vsh", Gfx::VS_V1_1, L"vs_main", macros);
        params.pPShader = _pShaderLib->getPShader(L"skybox.psh", Gfx::PS_V2_0, L"ps_main", macros);
        break;
    }
    }

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idColor = params.pPConst->getConstantIndexIfExists(L"Color");
    params.idSampler = params.pPConst->getConstantIndexIfExists(L"SamplerColor");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blend;
    Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_WRAP, Gfx::TEXTURE_ADDRESS_MIRROR);

    if(pass == PASS_GLOW)
    {
        depth.isStencilEnabled  = true;
        depth.stencilRef        = 0x01;
        depth.stencilReadMask   = 0x01;
        depth.stencilWriteMask  = 0x01;
        depth.opStencil.function            = Gfx::COMPARISON_NOT_EQUAL;
        depth.opStencil.opStencilNOK        = Gfx::KEEP;
        depth.opStencil.opStencilOKDepthNOK = Gfx::KEEP;
        depth.opStencil.opStencilOKDepthOK  = Gfx::KEEP;
    }

    sampler.isSRGB = true;
    blend.sRGBWriteEnabled = true;

    params.state = _pDevice->createState(raster, depth, blend);

    if(params.idSampler != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSampler)] = _pDevice->createState(sampler);
}
//-----------------------------------------------------------------------------
bool SkyboxRenderer::initialise()
{
    bool result = true;

    try
    {
        for(int32 iPass=0; iPass < ERenderPass_COUNT; iPass++)
        {
            initialise(_params[iPass], ERenderPass(iPass));
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing SkysphereRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::onDeviceReset()
{
    Assets::VerticeSet vertices;
    Assets::createCube(vertices, Core::Vector3f(1000.0f));
    const Core::List<Assets::Vertex> & vert = vertices.getVertices();

    int32 vertexBufferSize = sizeof(Vertex) * vertices.getVertices().size();
    int32 indexBufferSize = sizeof(uint16) * vertices.getIndices().size();

    // Index buffer

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, indexBufferSize);
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    const Core::List<uint16> & indices = vertices.getIndices();

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));
    ::memcpy(pIndices, &(*indices.begin()), indexBufferSize);
    _pIBuffer->unlock();


    // Vertex buffer

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vertexBufferSize);
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    Vertex * pVertices = reinterpret_cast<Vertex*>(_pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0));
    for(int32 ii=0; ii < vert.size(); ii++)
        pVertices[ii].position = vert[ii].position;
    _pVBuffer->unlock();
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _lightSettings = context.getLight();

    _commands.clear();

    if((context.getSkysphere()->getSettings().pSkyBox != null) &&
       ((pass == PASS_LIGHTING || pass == PASS_REFLECTION || 
        (pass == PASS_GLOW && !context.getSkysphere()->getSettings().glow.isZero()))))
    {
        Command command;
        command.pass      = pass;
        command.mode      = CMD_SKYSP;
        command.pExecuter = this;
        command.pExecData = (void*)context.getSkysphere();
        command.camDist   = 0.0f;
        command.flags     = 0;

        _commands.push_back(command);
    }
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void SkyboxRenderer::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        const Skysphere & skysphere = *(Skysphere*)pStart->pExecData;
        const SkysphereSettings & settings = skysphere.getSettings();

        int32 iPass = int32(_pass);
        int32 iLightModel = int32(settings.model);
        const ShaderParams & params = _params[iPass];

        Core::Matrix4f view(_view);
        view._Tx = view._Ty = view._Tz = 0.0;

        Core::Matrix4f worldViewProj(_proj * view);

        _pDevice->setState(params.state);
        _pDevice->setVertexFormat(params.pFormat);
        _pDevice->setVertexShader(params.pVShader);
        _pDevice->setPixelShader(params.pPShader);

        Core::Vector4f color;

        if(_pass == PASS_GLOW)
            color = settings.glow;
        else
            color = settings.color;

        color *= _lightSettings.getGlobalFade();

        params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
        params.pPConst->setConstantSafe(params.idColor, color);
        params.pPConst->setSamplerCube(params.idSampler, LM_DEBUG_PTR_CAST<CubeTextureMap>(settings.pSkyBox)->getResource());

        _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(Vertex), 0);
        _pDevice->setIndexBuffer(_pIBuffer);
        _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, 8, 0, 12); // 8 vertices, 12 triangles

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}
