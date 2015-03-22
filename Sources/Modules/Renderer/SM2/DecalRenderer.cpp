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
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/SM2/DecalRenderer.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
static const int32 VERTEX_BUFFER_SIZE = 4 * sizeof(DecalVertex);
static const int32 INDEX_BUFFER_SIZE  = 6 * sizeof(uint16);

struct DecalSorter
{
    Core::Vector3f position;
    Core::Vector3f dir;

    DecalSorter(const Core::Vector3f & position, const Core::Vector3f & dir) : position(position), dir(dir)
    {};

    bool operator () (Decal * p1, Decal * p2)
    {
        const Core::Vector3f & pos1 = p1->getPosition();
        const Core::Vector3f & pos2 = p2->getPosition();

        return Core::dot((pos1 - position), dir) > Core::dot((pos2 - position), dir);
    }
};
//-----------------------------------------------------------------------------
DecalRenderer::DecalRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                             const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
DecalRenderer::~DecalRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool DecalRenderer::initialise()
{
    bool result = true;
    int32 iMode = 0;
    int32 iFlag = 0;

    try
    {
        for(iMode=0; iMode < EDecalMode_COUNT; iMode++)
            for(iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                initialise(_params[iMode][iFlag], EDecalMode(iMode), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing DecalRenderer : '" << exception.getMessage() << L"' ("
            << Renderer::toString(EDecalMode(iMode))
            << L" 0x" << Core::toStringHex(iFlag) << L")\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void DecalRenderer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void DecalRenderer::onDeviceReset()
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
void DecalRenderer::initialise(ShaderParams & params, EDecalMode mode, int32 flags)
{
    // Shaders

    bool isSprite = (flags & SPRITE_FLAG) != 0;

    Core::List<Gfx::ShaderMacro> macros;

    if(mode == DECAL_REFRAC)
        macros.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));

    if(isSprite)
        macros.push_back(Gfx::ShaderMacro(L"SPRITE_FLAG", L"1"));

    if(flags & NORMAL_MAP_DXT5_FLAG)
        macros.push_back(Gfx::ShaderMacro(L"NORMAL_MAP_DXT5_FLAG", L"1"));

    params.pVShader = _pShaderLib->getVShader(L"decal.vsh", Gfx::VS_V1_1, L"vs_main", macros);
    params.pPShader = _pShaderLib->getPShader(L"decal.psh", Gfx::PS_V2_0, L"ps_main", macros);

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idSpriteCount = params.pVConst->getConstantIndexIfExists(L"SpriteCount");
    params.idSpriteCountU = params.pVConst->getConstantIndexIfExists(L"SpriteCountU");
    params.idSpriteUVSize = params.pVConst->getConstantIndexIfExists(L"SpriteUVSize");

    params.idSamplerColor = params.pPConst->getConstantIndexIfExists(L"SamplerColor");
    params.idSamplerNormal = params.pPConst->getConstantIndexIfExists(L"SamplerNormal");
    params.idSamplerRefraction = params.pPConst->getConstantIndexIfExists(L"SamplerRefraction");
    params.idRefrScale = params.pPConst->getConstantIndexIfExists(L"RefrScale");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT4, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    formatDesc.addAttribut(0, Gfx::VAT_COLOR,  Gfx::VAU_COLOR);

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blendADD(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_ONE);
    Gfx::RSBlendDesc        blendLERP(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendREFRAC;
    Gfx::RSSamplerDesc      samplerColor(Gfx::TEXTURE_ADDRESS_CLAMP);
    Gfx::RSSamplerDesc      samplerNormal(Gfx::TEXTURE_ADDRESS_CLAMP);

    setSampler(samplerColor, _settings.filterLevel);
    setSampler(samplerNormal, _settings.filterLevel);


    blendADD.sRGBWriteEnabled = true;
    blendLERP.sRGBWriteEnabled = true;
    blendREFRAC.sRGBWriteEnabled = true;

    samplerColor.isSRGB = true;

    switch(mode)
    {
    case DECAL_ADD:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendADD);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case DECAL_LERP:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendLERP);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case DECAL_REFRAC:
        LM_ASSERT(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST);
        LM_ASSERT(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendREFRAC);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerNormal)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerColor);
        break;
    }

}
//-----------------------------------------------------------------------------
void DecalRenderer::bind(const ShaderParams & params, const Decal & decal)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(params.pPShader);
    
    Core::Matrix4f world;
    decal.getWorldMatrix(world);
    Core::Matrix4f worldView(_view * world);
    Core::Matrix4f worldViewProj(_viewProj * world);
    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a * decal.getFogFactor());
    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
    params.pVConst->setConstantSafe(params.idWorldView, worldView);

    if(decal.hasSpriteLayout())
    {
        int32 us = std::max(1, decal.getUSpriteLayout());
        int32 vs = std::max(1, decal.getVSpriteLayout());
        Core::Vector2f spriteSize(1.0f / float(us), 1.0f / float(vs));

        params.pVConst->setConstant(params.idSpriteCount, float(us * vs));
        params.pVConst->setConstant(params.idSpriteCountU, float(us));
        params.pVConst->setConstant(params.idSpriteUVSize, spriteSize);
    }

    params.pPConst->setConstantSafe(params.idRefrScale, 10.0f * decal.getColor().a);

    if(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerColor, LM_DEBUG_PTR_CAST<TextureMap>(decal.getTexture())->getResource(decal.getTimeKey()));
    }

    if(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerNormal, LM_DEBUG_PTR_CAST<TextureMap>(decal.getTexture())->getResource(decal.getTimeKey()));
    }

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
    }
}
//-----------------------------------------------------------------------------
void DecalRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _pRenderTarget = context.getRenderTarget();

    _commands.clear();

    if(pass == PASS_GLOW || pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        const Core::List<Decal *> & decals = context.getDecals();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_TRANS;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < decals.size(); ii++)
        {
            const Decal & decal = *decals[ii];

            if((pass != PASS_GLOW || !decal.getGlow().isZero()) &&
                (decal.getMode() != DECAL_REFRAC || _settings.refracLevel >= REFRAC_LOW))
            {
                command.camDist   = Core::dot((decal.getPosition() - context.getEye()), context.getEyeDir()) + decal.getZBias();
                command.pExecData = (void*)&decal;

                _commands.push_back(command);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void DecalRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
int32 DecalRenderer::getFlags(const Decal & decal) const
{
    int32 flags = 0;

    if(decal.getTexture() != null && decal.getTexture()->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
        flags |= NORMAL_MAP_DXT5_FLAG;

    if(decal.hasSpriteLayout())
        flags |= SPRITE_FLAG;

    return flags;
}
//-----------------------------------------------------------------------------
void DecalRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void DecalRenderer::exec(Command * pStart, Command * pEnd)
{
    _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(DecalVertex), 0);
    _pDevice->setIndexBuffer(_pIBuffer);

    while(pStart != pEnd)
    {
        const Decal & decal = *(Decal*)pStart->pExecData;
        
        switch(_pass)
        {
        case PASS_LIGHTING:
        case PASS_REFLECTION:
        {
            if(decal.getMode() == DECAL_REFRAC)
                _pRenderTarget->updateRefractionBuffer();

            bind(_params[int32(decal.getMode())][getFlags(decal)], decal);
            break;
        }
        case PASS_GLOW:
        {
            bind(_params[int32(DECAL_ADD)][getFlags(decal)], decal);
            break;
        }
        }

        DecalVertex vertices[4];
        decal.getVertices(vertices, _pass == PASS_GLOW);

        void * pBufferData = _pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0);
        memcpy(pBufferData, vertices, 4 * sizeof(DecalVertex));
        _pVBuffer->unlock();

        _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, 4, 0, 2); // 4 vertices, 2 triangles

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}