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
#include <Renderer/SM2/SM0/WaterRendererSM0.h>
#include <Renderer/Common/Texture.h>
#include <Assets/GeometricPrimitive.h>

#include <Core/Logger.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
struct WaterVertex
{
    Core::Vector3f   position;
    float            texcoords[2];
    byte             padding[12];
};

LM_STATIC_ASSERT(sizeof(WaterVertex) == 32);

static const int32 VERTEX_BUFFER_SIZE = 4 * sizeof(WaterVertex);
static const int32 INDEX_BUFFER_SIZE  = 6 * sizeof(uint16);

//-----------------------------------------------------------------------------
WaterRendererSM0::WaterRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                             const Ptr<TextureMap> & pDefaultTexture, const Ptr<GPUResourceLib> & pResLib,
                             const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pDefaultTexture(pDefaultTexture),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
WaterRendererSM0::~WaterRendererSM0()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool WaterRendererSM0::initialise()
{
    bool result = true;

    try
    {
        for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
            initialise(_params[flags], flags);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing WaterRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::onDeviceReset()
{
    // Vertex buffer

    const float SIZE = 0.5f;
    int32 subDiv = 20;

    Assets::VerticeSet vset;
    Assets::createPlane(vset, SIZE, SIZE, subDiv, subDiv, 1.0f, 0.0f);

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vset.getVertices().size() * sizeof(WaterVertex));
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    WaterVertex * pVertex = reinterpret_cast<WaterVertex *>(_pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0; ii < vset.getVertices().size(); ii++)
    {
        pVertex[ii].position = vset.getVertices()[ii].position;
        pVertex[ii].texcoords[0] = vset.getVertices()[ii].texCoords.x;
        pVertex[ii].texcoords[1] = vset.getVertices()[ii].texCoords.y;
    }

    _pVBuffer->unlock();

    _vcount = vset.getVertices().size();
    _tcount = vset.getIndices().size() / 3;

    // Index buffer

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, vset.getIndices().size() * sizeof(uint16));
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));
    memcpy(pIndices, vset.getIndices().data(), vset.getIndices().size() * sizeof(uint16));
    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::initialise(ShaderParams & params, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macrosVS;

    params.pVShader = _pShaderLib->getVShader(L"water-final-fixed.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);

    params.pVConst = params.pVShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorld = params.pVConst->getConstantIndexIfExists(L"World");
    params.idEyePosition = params.pVConst->getConstantIndexIfExists(L"EyePosition");
    params.idFogOpacity = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idWaterDiffuse = params.pVConst->getConstantIndexIfExists(L"WaterDiffuse");

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS);
    Gfx::RSBlendDesc        blend(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSSamplerDesc      samplerRGB(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);

    blend.sRGBWriteEnabled = true;
    samplerRGB.isSRGB = true;

    params.state = _pDevice->createState(raster, depth, blend);
    
    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    if(pass != PASS_LIGHTING)
        return;

    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _mainLightSettings = context.getLight();
    _eye = context.getEye();
    _time = context.getTime();
    _waterLevel = context.getWaterLevel();
    _pRenderTarget = context.getRenderTarget();

    for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
        _patches[flags].clear();

    const Core::List<WaterPatch *> & patches = context.getWaterPatches();
    for(int32 iPatch=0; iPatch < patches.size(); iPatch++)
    {
        const WaterPatch & patch = *patches[iPatch];
        int32 flags = 0;

        _patches[flags].push_back(patches[iPatch]);
    }
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::endContext()
{
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    Command command;
    command.pass      = PASS_LIGHTING;
    command.mode      = CMD_WATER;
    command.pExecuter = this;
    command.pExecData = NULL;
    command.camDist   = 0.0f;
    command.flags     = 0;

    commands.push_back(command);
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::exec(Command * pStart, Command * pEnd)
{
    LM_ASSERT(pEnd == (pStart + 1));

    for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
        render(_params[flags], _patches[flags]);
}
//-----------------------------------------------------------------------------
void WaterRendererSM0::render(const ShaderParams & params, const Core::List<WaterPatch *> patches)
{
    try
    {
        _pDevice->setState(params.state);
        _pDevice->setVertexFormat(params.pFormat);
        _pDevice->setVertexShader(params.pVShader);
        _pDevice->setPixelShader(null);

        _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(WaterVertex), 0);
        _pDevice->setIndexBuffer(_pIBuffer);

        for(Core::List<WaterPatch *>::const_iterator ipPatch = patches.begin(); ipPatch != patches.end(); ++ipPatch)
        {
            const WaterPatch & patch = **ipPatch;

            Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

            Core::Matrix4f trans(Core::TRANSLATION, patch.getPosition().x, _waterLevel, patch.getPosition().y);
            Core::Matrix4f scale(Core::SCALE, patch.getWidth(), 1.0, patch.getHeight());
            Core::Matrix4f world(trans * scale);
            Core::Matrix4f worldViewProj(_viewProj * world);

            Core::Vector4f diffuse(patch.getDiffuse());

            params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
            params.pVConst->setConstantSafe(params.idWorld, world);
            params.pVConst->setConstantSafe(params.idEyePosition, _eye);
            params.pVConst->setConstantSafe(params.idFogOpacity, fogRange);
            params.pVConst->setConstantSafe(params.idWaterDiffuse, diffuse);

            Gfx::RSFixedFunc ff;
            ff.fogEnabled = true;
            ff.fogColor = _fogSettings.getColor();
            _pDevice->setFixedFuncPipe(ff);

            _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vcount, 0, _tcount);
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error rendering with WaterRendererSM0 : " << exception.getMessage() << L"\n";
    }
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IShaderResourceView> WaterRendererSM0::getResource(const Ptr<ITextureMap> & pTexture) const
{
    if(pTexture == null)
        return _pDefaultTexture->getResource();
    else
        return LM_DEBUG_PTR_CAST<TextureMap>(pTexture)->getResource();
}
//-----------------------------------------------------------------------------

}


