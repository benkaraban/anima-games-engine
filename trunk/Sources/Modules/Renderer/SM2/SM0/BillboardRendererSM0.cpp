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
#include <Renderer/SM2/SM0/BillboardRendererSM0.h>
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
BillboardRendererSM0::BillboardRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                                     const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
BillboardRendererSM0::~BillboardRendererSM0()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool BillboardRendererSM0::initialise()
{
    bool result = true;

    try
    {
        for(int32 iMode=0; iMode < EBillboardMode_COUNT; iMode++)
            for(int32 iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                initialise(_params[iMode][iFlag], EBillboardMode(iMode), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing BillboardRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::initialise(ShaderParams & params, EBillboardMode mode, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    if(mode == BILLBOARD_REFRAC)
        macros.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));

    if((flags & SPRITE_FLAG) != 0)
        macros.push_back(Gfx::ShaderMacro(L"SPRITE_FLAG", L"1"));

    params.pVShader = _pShaderLib->getVShader(L"billboard.vsh", Gfx::VS_V1_1, L"vs_main", macros);

    params.pVConst = params.pVShader->getConstantTable();

    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idProj = params.pVConst->getConstantIndexIfExists(L"Proj");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idSpriteCount = params.pVConst->getConstantIndexIfExists(L"SpriteCount");
    params.idSpriteCountU = params.pVConst->getConstantIndexIfExists(L"SpriteCountU");
    params.idSpriteUVSize = params.pVConst->getConstantIndexIfExists(L"SpriteUVSize");
    params.idFactor = params.pVConst->getConstantIndexIfExists(L"Factor");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_UBYTE4N, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3,  Gfx::VAU_TEXTURE_COORD, 1); // size-angle
    formatDesc.addAttribut(0, Gfx::VAT_COLOR,   Gfx::VAU_COLOR);

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
void BillboardRendererSM0::onDeviceLost()
{
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::onDeviceReset()
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
void BillboardRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _eye = context.getEye();
    _view = context.getView();
    _proj = context.getProj();
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
                (pass != PASS_GLOW && bill.getColorFactor() > 0.0f))
            {
                // TODO : ne le faire qu'une fois, pas une fois par passe !!!
                if(bill.getMode() == BILLBOARD_LERP || bill.getMode() == BILLBOARD_ADD_LERP)
                    bill.sortByDist(context.getEye(), context.getEyeDir());

                command.camDist   = Core::dot((bill.getWorldMatrix().getTVector() - context.getEye()), context.getEyeDir());
                command.pExecData = (void*)&bill;

                _commands.push_back(command);
            }
        }
    }
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::endContext()
{
}
//-----------------------------------------------------------------------------
int32 BillboardRendererSM0::getFlags(const BillboardGroup & bill) const
{
    int32 flags = 0;

    if(bill.hasSpriteLayout())
        flags |= SPRITE_FLAG;

    return flags;
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        BillboardGroup & bill = *(BillboardGroup*)pStart->pExecData;       
        send(_params[int32(bill.getMode())][getFlags(bill)], bill, _pass);

        pStart++;
    }
}
//-----------------------------------------------------------------------------
void BillboardRendererSM0::send(const ShaderParams & params, BillboardGroup & bill, ERenderPass pass)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(null);
    _pDevice->setIndexBuffer(_pIBuffer);

    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);
    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idProj, _proj);

    Core::Matrix4f worldView(_view * bill.getWorldMatrix());

    params.pVConst->setConstantSafe(params.idWorldView, worldView);

    if(pass == PASS_GLOW)
        params.pVConst->setConstantSafe(params.idFactor, bill.getGlowFactor());
    else
        params.pVConst->setConstantSafe(params.idFactor, bill.getColorFactor());

    if(bill.hasSpriteLayout())
    {
        int32 us = std::max(1, bill.getUSpriteLayout());
        int32 vs = std::max(1, bill.getVSpriteLayout());
        Core::Vector2f spriteSize(1.0f / float(us), 1.0f / float(vs));

        params.pVConst->setConstant(params.idSpriteCount, float(us * vs));
        params.pVConst->setConstant(params.idSpriteCountU, float(us));
        params.pVConst->setConstant(params.idSpriteUVSize, spriteSize);
    }

    Gfx::RSFixedFunc ff;
    ff.pTex0 = (LM_DEBUG_PTR_CAST<TextureMap>(bill.getTexture()))->getResource();
    ff.fogEnabled = false;

    _pDevice->setFixedFuncPipe(ff);

    bill.sendData(_pDevice);
}
//-----------------------------------------------------------------------------
}
