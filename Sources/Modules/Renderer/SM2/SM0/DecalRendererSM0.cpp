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
#include <Renderer/SM2/SM0/DecalRendererSM0.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
static const int32 VERTEX_BUFFER_SIZE = 4 * sizeof(DecalVertex);
static const int32 INDEX_BUFFER_SIZE  = 6 * sizeof(uint16);

struct DecalSorter
{
    Core::Vector3f position;

    DecalSorter(const Core::Vector3f & position) : position(position)
    {};

    bool operator () (Decal * p1, Decal * p2)
    {
        return p1->getSqDist(position) > p2->getSqDist(position);
    }
};
//-----------------------------------------------------------------------------
DecalRendererSM0::DecalRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                             const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
DecalRendererSM0::~DecalRendererSM0()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool DecalRendererSM0::initialise()
{
    bool result = true;

    try
    {
        for(int32 iMode=0; iMode < EDecalMode_COUNT; iMode++)
            for(int32 iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                initialise(_params[iMode][iFlag], EDecalMode(iMode), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing DecalRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void DecalRendererSM0::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void DecalRendererSM0::onDeviceReset()
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
void DecalRendererSM0::initialise(ShaderParams & params, EDecalMode mode, int32 flags)
{
    // Shaders

    bool isSprite = (flags & SPRITE_FLAG) != 0;

    Core::List<Gfx::ShaderMacro> macros;

    if(isSprite)
        macros.push_back(Gfx::ShaderMacro(L"SPRITE_FLAG", L"1"));

    params.pVShader = _pShaderLib->getVShader(L"decal.vsh", Gfx::VS_V1_1, L"vs_main", macros);

    params.pVConst = params.pVShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idSpriteCount = params.pVConst->getConstantIndexIfExists(L"SpriteCount");
    params.idSpriteCountU = params.pVConst->getConstantIndexIfExists(L"SpriteCountU");
    params.idSpriteUVSize = params.pVConst->getConstantIndexIfExists(L"SpriteUVSize");

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
    Gfx::RSSamplerDesc      samplerColor(Gfx::TEXTURE_ADDRESS_CLAMP);

    setSampler(samplerColor, _settings.filterLevel);

    blendADD.sRGBWriteEnabled = true;
    blendLERP.sRGBWriteEnabled = true;

    samplerColor.isSRGB = true;

    switch(mode)
    {
    case DECAL_ADD:
        params.state = _pDevice->createState(raster, depth, blendADD);
        params.state.ptSampler[0] = _pDevice->createState(samplerColor);
        break;
    case DECAL_REFRAC:
    case DECAL_LERP:
        params.state = _pDevice->createState(raster, depth, blendLERP);
        params.state.ptSampler[0] = _pDevice->createState(samplerColor);
        break;
    }

}
//-----------------------------------------------------------------------------
void DecalRendererSM0::bind(const ShaderParams & params, const Decal & decal)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(null);
    
    Core::Matrix4f world;
    decal.getWorldMatrix(world);
    Core::Matrix4f worldView(_view * world);
    Core::Matrix4f worldViewProj(_viewProj * world);
    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);
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

    Gfx::RSFixedFunc ff;
    ff.pTex0 = (LM_DEBUG_PTR_CAST<TextureMap>(decal.getTexture()))->getResource(decal.getTimeKey());
    ff.fogEnabled = false;
    _pDevice->setFixedFuncPipe(ff);
}
//-----------------------------------------------------------------------------
void DecalRendererSM0::endContext()
{
}//-----------------------------------------------------------------------------
void DecalRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
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

            if(pass != PASS_GLOW || !decal.getGlow().isZero())
            {
                command.camDist   = Core::dot((decal.getPosition() - context.getEye()), context.getEyeDir());
                command.pExecData = (void*)&decal;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void DecalRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
int32 DecalRendererSM0::getFlags(const Decal & decal) const
{
    int32 flags = 0;

    if(decal.hasSpriteLayout())
        flags |= SPRITE_FLAG;

    return flags;
}
//-----------------------------------------------------------------------------
void DecalRendererSM0::exec(Command * pStart, Command * pEnd)
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
