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
#include <Renderer/SM2/FreeFormRenderer.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
FreeFormRenderer::FreeFormRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, const Ptr<GPUResourceLib> & pResLib,
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
FreeFormRenderer::~FreeFormRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool FreeFormRenderer::initialise()
{
    bool result = true;
    int32 iMode = 0;
    int32 iFlag = 0;

    try
    {
        for(iMode=0; iMode < EFreeFormMode_COUNT; iMode++)
            for(iFlag=0; iFlag < FLAG_COUNT; iFlag++)
                initialise(_params[iMode][iFlag], EFreeFormMode(iMode), iFlag);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing FreeFormRenderer : '" << exception.getMessage() << L"' ("
            << Renderer::toString(EFreeFormMode(iMode))
            << L" 0x" << Core::toStringHex(iFlag) << L")\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::onDeviceLost()
{
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::onDeviceReset()
{
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::initialise(ShaderParams & params, EFreeFormMode mode, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    if(mode == FREE_FORM_REFRAC)
        macros.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));

    if(flags & NORMAL_MAP_DXT5_FLAG)
        macros.push_back(Gfx::ShaderMacro(L"NORMAL_MAP_DXT5_FLAG", L"1"));

    if(flags & GLOW_FLAG)
        macros.push_back(Gfx::ShaderMacro(L"GLOW_FLAG", L"1"));

    if(flags & LIGHT_FLAG)
        macros.push_back(Gfx::ShaderMacro(L"LIGHT_FLAG", L"1"));

    if(flags & WORLD_SPACE_FLAG)
        macros.push_back(Gfx::ShaderMacro(L"WORLD_SPACE_FLAG", L"1"));

    params.pVShader = _pShaderLib->getVShader(L"FreeForm.vsh", Gfx::VS_V1_1, L"vs_main", macros);
    params.pPShader = _pShaderLib->getPShader(L"FreeForm.psh", Gfx::PS_V2_0, L"ps_main", macros);

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idEyePos = params.pVConst->getConstantIndexIfExists(L"EyePos");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idMainLightDir = params.pVConst->getConstantIndexIfExists(L"MainLightDir");

    params.idSamplerColor = params.pPConst->getConstantIndexIfExists(L"SamplerColor");
    params.idSamplerNormal = params.pPConst->getConstantIndexIfExists(L"SamplerNormal");
    params.idSamplerRefraction = params.pPConst->getConstantIndexIfExists(L"SamplerRefraction");
    params.idRefrScale = params.pPConst->getConstantIndexIfExists(L"RefrScale");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_COLOR,  Gfx::VAU_COLOR);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_TEXTURE_COORD, 0); // texcoord
    formatDesc.addAttribut(0, Gfx::VAT_COLOR,  Gfx::VAU_TEXTURE_COORD, 1); // glow

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSRasterizerDesc   rasterLIGHT(Gfx::CM_BACK, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depthLIGHT(true, true, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blendADD(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_ONE);
    Gfx::RSBlendDesc        blendLERP(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendREFRAC;
    Gfx::RSBlendDesc        blendLIGHT;
    Gfx::RSSamplerDesc      samplerColor(Gfx::TEXTURE_ADDRESS_WRAP);
    Gfx::RSSamplerDesc      samplerNormal(Gfx::TEXTURE_ADDRESS_CLAMP);

    setSampler(samplerColor, _settings.filterLevel);
    setSampler(samplerNormal, _settings.filterLevel);

    blendADD.sRGBWriteEnabled = true;
    blendLERP.sRGBWriteEnabled = true;
    blendREFRAC.sRGBWriteEnabled = true;
    blendLIGHT.sRGBWriteEnabled = true;

    samplerColor.isSRGB = true;

    switch(mode)
    {
    case FREE_FORM_ADD:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendADD);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case FREE_FORM_LERP:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendLERP);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    case FREE_FORM_REFRAC:
        LM_ASSERT(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST);
        LM_ASSERT(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(raster, depth, blendREFRAC);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerNormal)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerColor);
        break;
    case FREE_FORM_LIGHT_MESH:
        LM_ASSERT(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST);
        params.state = _pDevice->createState(rasterLIGHT, depthLIGHT, blendLIGHT);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerColor)] = _pDevice->createState(samplerColor);
        break;
    }

}
//-----------------------------------------------------------------------------
void FreeFormRenderer::bind(const ShaderParams & params, const FreeForm & freeForm)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(params.pPShader);
    
    Core::Matrix4f world;
    freeForm.getWorldMatrix(world);
    Core::Matrix4f worldView(_view * world);
    Core::Matrix4f worldViewProj(_viewProj * world);
    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

    if(freeForm.isWorldSpaceCoords())
        fogRange.x = _fogSettings.getEnd() + _eye.z;

    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idEyePos, _eye);
    params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
    params.pVConst->setConstantSafe(params.idWorldView, worldView);
    params.pVConst->setConstantSafe(params.idMainLightDir, -_lightSettings.getDirection());

    if(params.idSamplerColor != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(freeForm.getTexture() != null)
            params.pPConst->setSampler2D(params.idSamplerColor, LM_DEBUG_PTR_CAST<TextureMap>(freeForm.getTexture())->getResource());
        else
            params.pPConst->setSampler2D(params.idSamplerColor, _pDefaultTex->getResource());
    }

    if(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerNormal, LM_DEBUG_PTR_CAST<TextureMap>(freeForm.getTexture())->getResource());
    }

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
    }
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _eye = context.getEye();
    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _lightSettings = context.getLight();
    _pRenderTarget = context.getRenderTarget();

    _commands.clear();

    const Core::List<FreeForm *> & fforms = context.getFreeForms();

    if(pass == PASS_GLOW || pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        Command command;
        command.pass      = pass;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < fforms.size(); ii++)
        {
            const FreeForm & fform = *fforms[ii];

            if(pass != PASS_GLOW || fform.getGlowFlag())
            {
                command.mode      = (fform.getMode() == FREE_FORM_LIGHT_MESH) ? CMD_SOLID : CMD_TRANS;
                command.camDist   = Core::dot(fform.getBoundingBox().getCenter() - context.getEye(), context.getEyeDir());
                command.pExecData = (void*)&fform;

                _commands.push_back(command);
            }
        }
    }
}
//-----------------------------------------------------------------------------
int32 FreeFormRenderer::getFlags(const FreeForm & freeForm) const
{
    int32 flags = 0;

    if(freeForm.getMode() == FREE_FORM_LIGHT_MESH)
        flags |= LIGHT_FLAG;

    if(freeForm.getTexture() != null && freeForm.getTexture()->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
        flags |= NORMAL_MAP_DXT5_FLAG;

    if(freeForm.isWorldSpaceCoords())
        flags |= WORLD_SPACE_FLAG;

    return flags;
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void FreeFormRenderer::exec(Command * pStart, Command * pEnd)
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
            bind(_params[int32(FREE_FORM_ADD)][getFlags(freeForm) + GLOW_FLAG], freeForm);
            break;
        }
        }

        freeForm.sendData();

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}
