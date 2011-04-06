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
#include <Renderer/SM2/SM0/FreeFormRendererSM0.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
FreeFormRendererSM0::FreeFormRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
                                   const Ptr<TextureMap> & pDefaultTex,
                                   const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pResLib(pResLib),
    _pDefaultTex(pDefaultTex),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
FreeFormRendererSM0::~FreeFormRendererSM0()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool FreeFormRendererSM0::initialise()
{
    bool result = true;

    try
    {
        for(int32 iMode=0; iMode < EFreeFormMode_COUNT; iMode++)
            for(int32 iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                initialise(_params[iMode][iFlag], EFreeFormMode(iMode), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing FreeFormRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::onDeviceLost()
{
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::onDeviceReset()
{
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::initialise(ShaderParams & params, EFreeFormMode mode, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    params.pVShader = _pShaderLib->getVShader(L"FreeForm.vsh", Gfx::VS_V1_1, L"vs_main", macros);

    params.pVConst = params.pVShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
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
    case FREE_FORM_ADD:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendADD);
        params.state.ptSampler[0] = _pDevice->createState(samplerColor);
        break;
    case FREE_FORM_REFRAC:
    case FREE_FORM_LERP:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendLERP);
        params.state.ptSampler[0] = _pDevice->createState(samplerColor);
        break;
    }

}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::bind(const ShaderParams & params, const FreeForm & freeForm)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    
    Core::Matrix4f world;
    freeForm.getWorldMatrix(world);
    Core::Matrix4f worldView(_view * world);
    Core::Matrix4f worldViewProj(_viewProj * world);
    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);
    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
    params.pVConst->setConstantSafe(params.idWorldView, worldView);

    Gfx::RSFixedFunc ff;

    if(freeForm.getTexture() != null)
        ff.pTex0 = (LM_DEBUG_PTR_CAST<TextureMap>(freeForm.getTexture()))->getResource();
    else
        ff.pTex0 = _pDefaultTex->getResource();

    ff.fogEnabled = false;
    _pDevice->setFixedFuncPipe(ff);
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _pRenderTarget = context.getRenderTarget();

    _commands.clear();

    const Core::List<FreeForm *> & fforms = context.getFreeForms();

    if(pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        Command command;
        command.pass      = pass;
        command.mode      = CMD_TRANS;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < fforms.size(); ii++)
        {
            const FreeForm & fform = *fforms[ii];

            command.mode      = (fform.getMode() == FREE_FORM_LIGHT_MESH) ? CMD_SOLID : CMD_TRANS;
            command.camDist   = Core::dot(fform.getBoundingBox().getCenter() - context.getEye(), context.getEyeDir());
            command.pExecData = (void*)&fform;

            _commands.push_back(command);
        }
    }
}
//-----------------------------------------------------------------------------
int32 FreeFormRendererSM0::getFlags(const FreeForm & FreeForm) const
{
    int32 flags = 0;
    return flags;
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::endContext()
{
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void FreeFormRendererSM0::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        const FreeForm & freeForm = *(FreeForm*)pStart->pExecData;
        
        switch(_pass)
        {
        case PASS_LIGHTING:
        case PASS_REFLECTION:
        {
            bind(_params[int32(freeForm.getMode())][getFlags(freeForm)], freeForm);
            break;
        }
        case PASS_GLOW:
        {
            LM_ASSERT(false);
            break;
        }
        }

        freeForm.sendData();

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}
