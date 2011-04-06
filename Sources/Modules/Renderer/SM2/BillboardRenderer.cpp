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
#include <Core/Debug/PerfLogger.h>
#include <Renderer/SM2/BillboardRenderer.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Tools.h>

#include <algorithm>

namespace Renderer
{
struct BillboardGroupSorter
{
    Core::Vector3f position;

    BillboardGroupSorter(const Core::Vector3f & position) : position(position)
    {};

    bool operator () (BillboardGroup * p1, BillboardGroup * p2)
    {
        return p1->getSqDist(position) > p2->getSqDist(position);
    }
};
//-----------------------------------------------------------------------------
BillboardRenderer::BillboardRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                                     const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
BillboardRenderer::~BillboardRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool BillboardRenderer::initialise()
{
    bool result = true;
    int32 iMode = 0;
    int32 iAlign = 0;
    int32 iFlag = 0;

    try
    {
        for(iMode=0; iMode < EBillboardMode_COUNT; iMode++)
            for(iAlign=0; iAlign < EBillboardAlign_COUNT; iAlign++)
                for(iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                    initialise(_params[iMode][iAlign][iFlag], EBillboardMode(iMode), EBillboardAlign(iAlign), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing BillboardRenderer : '" << exception.getMessage() << L"' ("
            << Renderer::toString(EBillboardMode(iMode)) << L" "
            << Renderer::toString(EBillboardAlign(iAlign))
            << L" 0x" << Core::toStringHex(iFlag) << L")\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void BillboardRenderer::initialise(ShaderParams & params, EBillboardMode mode, EBillboardAlign align, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    macros.push_back(Gfx::ShaderMacro(L"ALIGN_MODE", Core::toString(int32(align))));

    if(mode == BILLBOARD_REFRAC)
        macros.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));

    if(mode == BILLBOARD_ADD_LERP)
        macros.push_back(Gfx::ShaderMacro(L"PREMUL_ALPHA_FLAG", L"1"));

    if((flags & SPRITE_FLAG) != 0)
    {
        macros.push_back(Gfx::ShaderMacro(L"SPRITE_FLAG", L"1"));

        if((flags & SPRITE_SMOOTH_FLAG) != 0)
            macros.push_back(Gfx::ShaderMacro(L"SPRITE_SMOOTH_FLAG", L"1"));
    }

    params.pVShader = _pShaderLib->getVShader(L"billboard.vsh", Gfx::VS_V1_1, L"vs_main", macros);
    params.pPShader = _pShaderLib->getPShader(L"billboard.psh", Gfx::PS_V2_0, L"ps_main", macros);

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idWorld = params.pVConst->getConstantIndexIfExists(L"World");
    params.idViewProj = params.pVConst->getConstantIndexIfExists(L"ViewProj");
    params.idProj = params.pVConst->getConstantIndexIfExists(L"Proj");
    params.idEyePosition = params.pVConst->getConstantIndexIfExists(L"EyePosition");
    params.idAlignVector = params.pVConst->getConstantIndexIfExists(L"AlignVector");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idSpriteCount = params.pVConst->getConstantIndexIfExists(L"SpriteCount");
    params.idSpriteCountU = params.pVConst->getConstantIndexIfExists(L"SpriteCountU");
    params.idSpriteUVSize = params.pVConst->getConstantIndexIfExists(L"SpriteUVSize");
    params.idFactor = params.pVConst->getConstantIndexIfExists(L"Factor");
    params.idPartScale = params.pVConst->getConstantIndexIfExists(L"PartScale");    

    params.idSamplerColor = params.pPConst->getConstantIndexIfExists(L"SamplerColor");
    params.idSamplerNormal = params.pPConst->getConstantIndexIfExists(L"SamplerNormal");
    params.idSamplerRefraction = params.pPConst->getConstantIndexIfExists(L"SamplerRefraction");
    params.idRefrScale = params.pPConst->getConstantIndexIfExists(L"RefrScale");
    
    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_UBYTE4N, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    if(align == BILLBOARD_ALIGN_HORIZONTAL_ON_DIR || align == BILLBOARD_ALIGN_VERTICAL_ON_DIR)
    {
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT2,  Gfx::VAU_TEXTURE_COORD, 1); // size
        formatDesc.addAttribut(0, Gfx::VAT_UBYTE4N, Gfx::VAU_TEXTURE_COORD, 2); // align-vector
    }
    else
    {
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_TEXTURE_COORD, 1); // size-angle
    }
    formatDesc.addAttribut(0, Gfx::VAT_COLOR,   Gfx::VAU_COLOR);

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blendADD(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_ONE);
    Gfx::RSBlendDesc        blendLERP(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendADDLERP(Gfx::BM_ONE, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendREFRAC(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSSamplerDesc      samplerColor(Gfx::TEXTURE_ADDRESS_CLAMP);
    Gfx::RSSamplerDesc      samplerNormal(Gfx::TEXTURE_ADDRESS_CLAMP);

    blendADD.alphaTestEnabled = true;
    blendADD.alphaTestFunc = Gfx::COMPARISON_GREATER;
    blendADD.alphaTestRef = 0x00;

    blendLERP.alphaTestEnabled = true;
    blendLERP.alphaTestFunc = Gfx::COMPARISON_GREATER;
    blendLERP.alphaTestRef = 0x00;

    /*
    depth.isStencilEnabled              = true;
    depth.opStencil.function            = Gfx::COMPARISON_GREATER;
    depth.opStencil.opStencilNOK        = Gfx::KEEP;
    depth.opStencil.opStencilOKDepthOK  = Gfx::INCREMENT_SAT;
    depth.opStencil.opStencilOKDepthNOK = Gfx::KEEP;
    depth.stencilRef                    = 100;
    depth.stencilReadMask               = 0xFF;
    depth.stencilWriteMask              = 0xFF;
    */

    setSampler(samplerColor, _settings.filterLevel);
    setSampler(samplerNormal, _settings.filterLevel);

    blendADD.sRGBWriteEnabled = true;
    blendLERP.sRGBWriteEnabled = true;

    blendREFRAC.sRGBWriteEnabled = true;
    blendREFRAC.alphaTestEnabled = true;
    blendREFRAC.alphaTestFunc = Gfx::COMPARISON_GREATER;
    blendREFRAC.alphaTestRef = 0;


    samplerColor.isSRGB = true;
    samplerNormal.isSRGB = false;

    switch(mode)
    {
    case BILLBOARD_ADD:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendADD);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case BILLBOARD_LERP:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendLERP);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case BILLBOARD_ADD_LERP:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendADDLERP);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case BILLBOARD_REFRAC:
        LM_ASSERT(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST);
        LM_ASSERT(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendREFRAC);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerNormal)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerColor);
        break;
    }
}
//-----------------------------------------------------------------------------
void BillboardRenderer::onDeviceLost()
{
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void BillboardRenderer::onDeviceReset()
{
    // Buffers

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, MAX_BILLBOARD_COUNT * 6 * sizeof(uint16));
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 iBill=0, iTri=0; iBill < MAX_BILLBOARD_COUNT; iBill++)
    {
        pIndices[0] = iTri; pIndices[1] = iTri + 1; pIndices[2] = iTri + 2;
        pIndices[3] = iTri; pIndices[4] = iTri + 2; pIndices[5] = iTri + 3;

        pIndices += 6;
        iTri += 4;
    }

    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void BillboardRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _eye = context.getEye();
    _viewProj = context.getViewProj();
    _view = context.getView();
    _proj = context.getProj();
    _lightSettings = context.getLight();
    _fogSettings = context.getFog();
    _pRenderTarget = context.getRenderTarget();

    _commands.clear();

    if(pass == PASS_GLOW || pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        const Core::List<BillboardGroup *> & bills = context.getBills();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_TRANS;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < bills.size(); ii++)
        {
            BillboardGroup & bill = *bills[ii];

            if((pass == PASS_GLOW && bill.getGlowFactor() > 0.0f) ||
                (pass != PASS_GLOW && bill.getColorFactor() > 0.0f) &&
                (bill.getMode() != BILLBOARD_REFRAC || _settings.refracLevel >= REFRAC_LOW))
            {
                // TODO : ne le faire qu'une fois, pas une fois par passe !!!
                if(bill.getMode() == BILLBOARD_LERP || bill.getMode() == BILLBOARD_ADD_LERP || bill.getMode() == BILLBOARD_REFRAC)
                    bill.sortByDist(context.getEye(), context.getEyeDir());

                command.camDist   = Core::dot((bill.getBoundingBox().getCenter() - context.getEye()), context.getEyeDir()) + bill.getZBias();
                command.pExecData = (void*)&bill;

                _commands.push_back(command);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void BillboardRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
int32 BillboardRenderer::getFlags(const BillboardGroup & bill) const
{
    int32 flags = 0;

    if(bill.hasSpriteLayout())
        flags |= SPRITE_FLAG;

    if(bill.isSpriteSmooth())
        flags |= SPRITE_SMOOTH_FLAG;

    return flags;
}
//-----------------------------------------------------------------------------
void BillboardRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void BillboardRenderer::exec(Command * pStart, Command * pEnd)
{
    LM_START_PERF_LOG(L"bill");

    while(pStart != pEnd)
    {
        BillboardGroup & bill = *(BillboardGroup*)pStart->pExecData;     

        if(bill.getMode() == BILLBOARD_REFRAC && _pass != PASS_GLOW)
            _pRenderTarget->updateRefractionBuffer();

        send(_params[int32(bill.getMode())][int32(bill.getAlign())][getFlags(bill)], bill, _pass);

        pStart++;
    }

    LM_END_PERF_LOG(L"bill");
}
//-----------------------------------------------------------------------------
void BillboardRenderer::send(const ShaderParams & params, BillboardGroup & bill, ERenderPass pass)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(params.pPShader);
    _pDevice->setIndexBuffer(_pIBuffer);

    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a * bill.getFogFactor());
    params.pVConst->setConstantSafe(params.idFogRange, fogRange);

    Core::Matrix4f worldView(_view * bill.getWorldMatrix());

    params.pVConst->setConstantSafe(params.idWorld, bill.getWorldMatrix());
    params.pVConst->setConstantSafe(params.idWorldView, worldView);
    params.pVConst->setConstantSafe(params.idViewProj, _viewProj);
    params.pVConst->setConstantSafe(params.idProj, _proj);
    params.pVConst->setConstantSafe(params.idEyePosition, _eye);
    params.pVConst->setConstantSafe(params.idAlignVector, bill.getWorldMatrix().apply3x3(bill.getAlignVector()).getNormalizedSafe());
    params.pVConst->setConstantSafe(params.idPartScale, bill.getPartScale());

    if(pass == PASS_GLOW)
        params.pVConst->setConstantSafe(params.idFactor, bill.getGlowFactor());
    else
        params.pVConst->setConstantSafe(params.idFactor, bill.getColorFactor() * _lightSettings.getGlobalFade());

    if(bill.hasSpriteLayout())
    {
        int32 us = std::max(1, bill.getUSpriteLayout());
        int32 vs = std::max(1, bill.getVSpriteLayout());
        Core::Vector2f spriteSize(1.0f / float(us), 1.0f / float(vs));

        params.pVConst->setConstant(params.idSpriteCount, float(us * vs));
        params.pVConst->setConstant(params.idSpriteCountU, float(us));
        params.pVConst->setConstant(params.idSpriteUVSize, spriteSize);
    }

    params.pPConst->setConstantSafe(params.idRefrScale, 10.0f * bill.getRefractionScale());

    if(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerColor, LM_DEBUG_PTR_CAST<TextureMap>(bill.getTexture())->getResource());
    }

    if(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerNormal, LM_DEBUG_PTR_CAST<TextureMap>(bill.getTexture())->getResource());
    }

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
    }

    bill.sendData(_pDevice);
}
//-----------------------------------------------------------------------------
}
