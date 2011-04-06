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
#include <Renderer/IRenderer.h>
#include <Renderer/SM2/CustomMatRenderer.h>

#include <algorithm>

namespace Renderer
{
//CustomMatRenderer::ShadersParams::ShadersParams() :
//    initTried(false),
//    initOK(false)
//{
//}

CustomMatRenderer::CustomMatRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
         const Ptr<PackedVerticesFormat> & pFormat,
         const Ptr<TextureMap> & pDefaultTexWhite,
         const Ptr<TextureMap> & pDefaultTexBump,
         const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pDefaultTexWhite(pDefaultTexWhite),
    _pDefaultTexBump(pDefaultTexBump),
    _settings(settings)
{
}

void CustomMatRenderer::initialise(ShadersParams & params, Assets::ECustomMode mode, bool skin, int32 flags)
{
    // Shaders
    bool isNormal = (mode == Assets::CM_REFRAC);
    params.isNormal = isNormal;

    Core::List<Gfx::ShaderMacro> macrosPS;
    Core::List<Gfx::ShaderMacro> macrosVS;

    if(isNormal)
    {
        macrosPS.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));
        macrosVS.push_back(Gfx::ShaderMacro(L"REFRACTION_FLAG", L"1"));

        if((flags & FLAG_IS_NORMAL_DXT5_0) != 0)
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_0", L"1"));
        else
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_0", L"0"));

        if((flags & FLAG_IS_NORMAL_DXT5_1) != 0)
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_1", L"1"));
        else
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_1", L"0"));

        if((flags & FLAG_IS_NORMAL_DXT5_2) != 0)
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_2", L"1"));
        else
            macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_2", L"0"));

        params.skinnedStreamData = VD_SKIN_G_B;
        params.staticStreamData = VD_STATIC_G_B;
    }
    else
    {
        if((flags & FLAG_ADD_12) != 0) macrosPS.push_back(Gfx::ShaderMacro(L"FLAG_ADD_12", L"1"));
        if((flags & FLAG_ADD_01) != 0) macrosPS.push_back(Gfx::ShaderMacro(L"FLAG_ADD_01", L"1"));

        params.skinnedStreamData = VD_SKIN_G;
        params.staticStreamData = VD_STATIC_G;
    }

    params.pVShader = _pShaderLib->getVShader(L"custom.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
    params.pPShader = _pShaderLib->getPShader(L"custom.psh", Gfx::PS_V2_0, L"ps_main", macrosPS);

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorldView = params.pVConst->getConstantIndexIfExists(L"WorldView");
    params.idFogRange = params.pVConst->getConstantIndexIfExists(L"FogRange");
    params.idDUV0 = params.pVConst->getConstantIndexIfExists(L"DUV0");
    params.idDUV1 = params.pVConst->getConstantIndexIfExists(L"DUV1");
    params.idDUV2 = params.pVConst->getConstantIndexIfExists(L"DUV2");

    params.idCustomColor = params.pPConst->getConstantIndexIfExists(L"CustomColor");
    params.idRefrScale = params.pPConst->getConstantIndexIfExists(L"RefrScale");
    params.idSamplerTexture0 = params.pPConst->getConstantIndexIfExists(L"SamplerTexture0");
    params.idSamplerTexture1 = params.pPConst->getConstantIndexIfExists(L"SamplerTexture1");
    params.idSamplerTexture2 = params.pPConst->getConstantIndexIfExists(L"SamplerTexture2");
    params.idSamplerRefraction = params.pPConst->getConstantIndexIfExists(L"SamplerRefraction");

    // Format

    if(skin)
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.skinnedStreamData);
    else
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.staticStreamData);

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blendADD(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_ONE);
    Gfx::RSBlendDesc        blendLERP(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendADDLERP(Gfx::BM_ONE, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSBlendDesc        blendREFRAC(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
    Gfx::RSSamplerDesc      samplerColor(Gfx::TEXTURE_ADDRESS_WRAP);
    Gfx::RSSamplerDesc      samplerNormal(Gfx::TEXTURE_ADDRESS_WRAP);
    Gfx::RSSamplerDesc      samplerRefrac(Gfx::TEXTURE_ADDRESS_CLAMP);

    setSampler(samplerColor, _settings.filterLevel);
    setSampler(samplerNormal, _settings.filterLevel);

    blendADD.sRGBWriteEnabled = true;
    blendLERP.sRGBWriteEnabled = true;
    blendADDLERP.sRGBWriteEnabled = true;
    blendREFRAC.sRGBWriteEnabled = true;

    samplerColor.isSRGB = true;
    samplerRefrac.isSRGB = true;

    switch(mode)
    {
    case Assets::CM_ADD:        params.state = _pDevice->createState(raster, depth, blendADD); break;
    case Assets::CM_LERP:       params.state = _pDevice->createState(raster, depth, blendLERP); break;
    case Assets::CM_ADD_LERP:   params.state = _pDevice->createState(raster, depth, blendADDLERP); break;
    case Assets::CM_REFRAC:     params.state = _pDevice->createState(raster, depth, blendREFRAC); break;
    }

    if(isNormal)
    {
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture0)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture1)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture2)] = _pDevice->createState(samplerNormal);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerRefrac);
    }
    else
    {
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture0)] = _pDevice->createState(samplerColor);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture1)] = _pDevice->createState(samplerColor);
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerTexture2)] = _pDevice->createState(samplerColor);
    }
}

bool CustomMatRenderer::initialise()
{
    bool result = true;
    bool refrac = false;
    int32 ii = 0;

    try
    {
        for(ii=0; ii < FLAG_COLOR_COUNT; ii++)
        {
            initialise(_paramsAdd[ii], Assets::CM_ADD, false, ii);
            initialise(_paramsLerp[ii], Assets::CM_LERP, false, ii);
            initialise(_paramsAddLerp[ii], Assets::CM_ADD_LERP, false, ii);
        }

        refrac = true;

        for(ii=0; ii < FLAG_REFRAC_COUNT; ii++)
        {
            initialise(_paramsRefrac[ii], Assets::CM_REFRAC, false, ii);
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing CustomMatRenderer : '" << exception.getMessage() << L"' ("
            << (refrac ? L"REFRAC" : L"COLOR") << L" 0x" << Core::toStringHex(ii) << L")\n";
        result = false;
    }

    return result;
}

void CustomMatRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _pRenderTarget = context.getRenderTarget();
    _time = context.getTime();

    _commands.clear();

    if(pass == PASS_GLOW || pass == PASS_LIGHTING || pass == PASS_REFLECTION)
    {
        const Core::List<MeshInstanceSM2 *> & meshes = context.getCustomMeshes();

        Command command;
        command.pass      = pass;
        command.mode      = CMD_TRANS;
        command.pExecuter = this;
        command.flags     = 0;

        for(int32 ii=0; ii < meshes.size(); ii++)
        {
            const MeshInstanceSM2 & mesh = *meshes[ii];
            const Material & mat = mesh.getMaterial();

            if(mesh.isVisible())
            {
                if(mat.customMode == Assets::CM_REFRAC)
                {
                    if(_settings.refracLevel >= REFRAC_LOW && pass == PASS_LIGHTING)
                    {
                        command.camDist   = Core::dot((mesh.getPosition() - context.getEye()), context.getEyeDir()) + mat.zBias;
                        command.pExecData = (void*)&mesh;

                        _commands.push_back(command);
                    }
                }
                else
                {
                    if((pass == PASS_GLOW && mat.glowFactor > 0.0f) ||
                       (pass != PASS_GLOW && mat.colorFactor > 0.0f))
                    {
                        command.camDist   = Core::dot((mesh.getPosition() - context.getEye()), context.getEyeDir()) + mat.zBias;
                        command.pExecData = (void*)&mesh;

                        _commands.push_back(command);
                    }
                }
            }
        }
    }
}

void CustomMatRenderer::send(const MeshInstanceSM2 & mesh, ERenderPass pass)
{
    int32 flags = 0;

    const Material & mat = mesh.getMaterial();

    if(mat.customMode == Assets::CM_REFRAC)
    {
        if(mat.pCustomMap0 != null && (mat.pCustomMap0->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)) flags |= FLAG_IS_NORMAL_DXT5_0;
        if(mat.pCustomMap1 != null && (mat.pCustomMap1->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)) flags |= FLAG_IS_NORMAL_DXT5_1;
        if(mat.pCustomMap2 != null && (mat.pCustomMap2->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)) flags |= FLAG_IS_NORMAL_DXT5_2;

        _pRenderTarget->updateRefractionBuffer();

        send(_paramsRefrac[flags], mesh, mat, pass);
    }
    else
    {
        if(mat.opColor01 == Assets::OP_ADD) flags |= FLAG_ADD_01;
        if(mat.opColor12 == Assets::OP_ADD) flags |= FLAG_ADD_12;

        switch(mat.customMode)
        {
        case Assets::CM_ADD:        send(_paramsAdd[flags], mesh, mat, pass); break;
        case Assets::CM_LERP:       send(_paramsLerp[flags], mesh, mat, pass); break;
        case Assets::CM_ADD_LERP:   send(_paramsAddLerp[flags], mesh, mat, pass); break;
        }
    }
}

void CustomMatRenderer::send(ShadersParams & params, const MeshInstanceSM2 & mesh, const Material & mat, ERenderPass pass)
{
    _pDevice->setState(params.state);
    _pDevice->setVertexFormat(params.pFormat);
    _pDevice->setVertexShader(params.pVShader);
    _pDevice->setPixelShader(params.pPShader);

    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

    Core::Matrix4f worldView(_view * mesh.getWorldMatrix());
    Core::Matrix4f worldViewProj(_proj * worldView);

    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idWorldView, worldView);
    params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
    params.pVConst->setConstantSafe(params.idDUV0, mat.duv0 * float(_time));
    params.pVConst->setConstantSafe(params.idDUV1, mat.duv1 * float(_time));
    params.pVConst->setConstantSafe(params.idDUV2, mat.duv2 * float(_time));

    Core::Vector4f color(mat.customColor);

    if(pass == PASS_GLOW)
        color.a *= mat.glowFactor;
    else
        color.a *= mat.colorFactor;

    params.pPConst->setConstantSafe(params.idCustomColor, color);
    params.pPConst->setConstantSafe(params.idRefrScale, mat.refracScale * color.a);

    setTexture(params.pPConst, params.idSamplerTexture0, mat.pCustomMap0, params.isNormal);
    setTexture(params.pPConst, params.idSamplerTexture1, mat.pCustomMap1, params.isNormal);
    setTexture(params.pPConst, params.idSamplerTexture2, mat.pCustomMap2, params.isNormal);

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
        params.pPConst->setSampler2D(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));

    Ptr<Mesh> pInstance = LM_DEBUG_PTR_CAST<Mesh>(mesh.getMesh());
    const IPackedStaticVertices & vertices = pInstance->getVertices();

    vertices.bindData(_pDevice, params.staticStreamData);
    vertices.sendData(_pDevice, params.staticStreamData);
}

void CustomMatRenderer::setTexture(const Ptr<Gfx::ShaderConstantTable> & pPConst, int32 idSampler, const Ptr<ITextureMap> & pTex, bool isNormal)
{
    if(pTex == null)
    {
        if(isNormal)
            pPConst->setSampler2D(idSampler, _pDefaultTexBump->getResource());
        else
            pPConst->setSampler2D(idSampler, _pDefaultTexWhite->getResource());
    }
    else
    {
        pPConst->setSampler2D(idSampler, LM_DEBUG_PTR_CAST<TextureMap>(pTex)->getResource());
    }
}

void CustomMatRenderer::endContext()
{
}

void CustomMatRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}

void CustomMatRenderer::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        //if(pStart->flags != 0)
        //{
        //    // Skin
        //    SkinMeshInstanceSM2 &   skin    = *(SkinMeshInstanceSM2*)pStart->pExecData;
        //}
        //else
        {
            send(*(MeshInstanceSM2*)pStart->pExecData, pStart->pass);
        }

        pStart ++;
    }
}


}
