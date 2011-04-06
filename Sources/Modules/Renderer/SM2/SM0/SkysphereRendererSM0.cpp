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
#include <Renderer/SM2/SM0/SkysphereRendererSM0.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
SkysphereRendererSM0::SkysphereRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib)
{
}
//-----------------------------------------------------------------------------
void SkysphereRendererSM0::initialise(ShaderParams & params, ERenderPass pass, ELightingModel model)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macros;

    switch(model)
    {
    case LIGHTING_MODEL_FOGGY:     macros.push_back(Gfx::ShaderMacro(L"LIGHTING_MODEL", L"0")); break;
    case LIGHTING_MODEL_CLOUDY:    macros.push_back(Gfx::ShaderMacro(L"LIGHTING_MODEL", L"1")); break;
    case LIGHTING_MODEL_CLEAR_SKY: macros.push_back(Gfx::ShaderMacro(L"LIGHTING_MODEL", L"2")); break;
    }

    switch(pass)
    {
    case PASS_WIREFRAME:
    case PASS_STATIC_SHADOW:
    case PASS_DYNAMIC_SHADOW:
    case PASS_LIGHTING:
    case PASS_REFLECTION:
    case PASS_GLOW:
    case PASS_DEBUG:
    {
        params.pVShader = _pShaderLib->getVShader(L"skysphere-fixed.vsh", Gfx::VS_V1_1, L"vs_main", macros);
        break;
    }
    }

    params.pVConst = params.pVShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idColor = params.pVConst->getConstantIndexIfExists(L"Color");
    params.idFogOpacity = params.pVConst->getConstantIndexIfExists(L"FogOpacity");

    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blend;
    Gfx::RSSamplerDesc      sampler(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_WRAP, Gfx::TEXTURE_ADDRESS_MIRROR);

    sampler.isSRGB = true;
    blend.sRGBWriteEnabled = true;

    params.state = _pDevice->createState(raster, depth, blend);
    params.state.ptSampler[0] = _pDevice->createState(sampler);
}
//-----------------------------------------------------------------------------
bool SkysphereRendererSM0::initialise()
{
    bool result = true;

    try
    {
        for(int32 iPass=0; iPass < ERenderPass_COUNT; iPass++)
            for(int32 iModel=0; iModel < ELightingModel_COUNT; iModel++)
                initialise(_params[iPass][iModel], ERenderPass(iPass), ELightingModel(iModel));
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing SkysphereRendererSM0 : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void SkysphereRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _fogSettings = context.getFog();
    _lightSettings = context.getLight();

    _commands.clear();

    const Core::List<BillboardGroup *> & bills = context.getBills();

    Command command;
    command.pass      = pass;
    command.mode      = CMD_SKYSP;
    command.pExecuter = this;
    command.pExecData = (void*)context.getSkysphere();
    command.camDist   = 0.0f;
    command.flags     = 0;

    _commands.push_back(command);
}
//-----------------------------------------------------------------------------
void SkysphereRendererSM0::endContext()
{
}
//-----------------------------------------------------------------------------
void SkysphereRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void SkysphereRendererSM0::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        const Skysphere & skysphere = *(Skysphere*)pStart->pExecData;

        const SkysphereSettings & settings = skysphere.getSettings();
        const ShaderParams & params = _params[int32(_pass)][int32(settings.model)];

        if(settings.pTexture != null)
        {
            Core::Quaternionf rotation(Core::YAW, settings.yaw);
            Core::Matrix4f rotView(_view);

            if(settings.isAtInfinity)
                rotView._Tx = rotView._Ty = rotView._Tz = 0.0;

            Core::Matrix4f world(rotation);
            Core::Matrix4f viewProj(_proj * rotView);
            Core::Matrix4f worldViewProj(viewProj * world);

            _pDevice->setState(params.state);
            _pDevice->setVertexFormat(params.pFormat);
            _pDevice->setVertexShader(params.pVShader);
            _pDevice->setPixelShader(null);

            if(_pass == PASS_GLOW)
                params.pVConst->setConstantSafe(params.idColor, settings.glow);
            else
                params.pVConst->setConstantSafe(params.idColor, settings.color);

            params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
            params.pVConst->setConstantSafe(params.idFogOpacity, _fogSettings.getSkyOpacity());
            
            Gfx::RSFixedFunc ff;
            ff.pTex0 = (LM_DEBUG_PTR_CAST<TextureMap>(settings.pTexture))->getResource();
            ff.fogEnabled = true;
            ff.fogColor = _fogSettings.getColor();

            _pDevice->setFixedFuncPipe(ff);

            skysphere.sendData(_pDevice);
        }

        pStart++;
    }
}
//-----------------------------------------------------------------------------
}
