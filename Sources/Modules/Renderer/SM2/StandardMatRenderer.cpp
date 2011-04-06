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
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/IRenderer.h>
#include <Renderer/SM2/StandardMatRenderer.h>

#include <algorithm>

namespace Renderer
{
StandardMatRenderer::ShadersParams::ShadersParams() :
    initTried(false),
    initOK(false)
{
}

StandardMatRenderer::StandardMatRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
         const Ptr<PackedVerticesFormat> & pFormat,
         const Ptr<TextureMap> & pDefaultTexWhite,
         const Ptr<TextureMap> & pDefaultTexBlack,
         const Ptr<TextureMap> & pDebugTexMip,
         const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pDefaultTexWhite(pDefaultTexWhite),
    _pDefaultTexBlack(pDefaultTexBlack),
    _pDebugTexMip(pDebugTexMip),
    _settings(settings)
{
}

void StandardMatRenderer::setStaticShadowMap(const Ptr<Gfx::ITexture2D> & pShadowMap)
{
    _pStaticShadowMap = pShadowMap;
}

void StandardMatRenderer::setDynamicShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap)
{
    switch(split)
    {
    case 0: _pDynamicShadowMap0 = pShadowMap; break;
    case 1: _pDynamicShadowMap1 = pShadowMap; break;
    default:
        LM_ASSERT(false);
        break;
    }
}

bool StandardMatRenderer::initialise()
{
    bool result = true;

#ifndef _DEBUG
    /*
    // En mode debug, on ne génère pas tous les shaders à l'initialisation mais on les génère à la demande,
    // pour limiter le temps d'initialisation pour les petits programmes de tests
    try
    {
        for(int32 iPass=0; iPass < ERenderPass_COUNT; iPass++)
            for(int32 flags=0; flags < CONFIG_COUNT; flags++)
            {
                getParams(_paramsSolid[iPass][flags], (ERenderPass)iPass, MODE_SOLID, flags);
                getParams(_paramsZOnly[iPass][flags], (ERenderPass)iPass, MODE_ZONLY, flags);
                getParams(_paramsTransBack[iPass][flags], (ERenderPass)iPass, MODE_TRANS_BACK, flags);
                getParams(_paramsTransFront[iPass][flags], (ERenderPass)iPass, MODE_TRANS_FRONT, flags);
            }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing StandardMatRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }
    */
#endif

    return result;
}

void StandardMatRenderer::compileAllShaders()
{
    ShadersParams params;

    for(int32 iPass=0; iPass < ERenderPass_COUNT; iPass++)
    {
        ERenderPass pass = ERenderPass(iPass);

        if(pass == PASS_LIGHTMAP_GENERATION || pass == PASS_WIREFRAME ||pass == PASS_DEBUG)
            continue;

        for(int32 iMode=0; iMode < EParamMode_COUNT; iMode++)
        {
            for(int32 iFlag=0; iFlag <= ALL_LIGHTING_FLAG; iFlag++)
            {
                int32 flags = iFlag;

                if(_settings.shaderLevel <= SHADER_LOW)
                {
                    // Jamais de ADV_SPECULAR_FLAG en LOW
                    flags = flags & (~ADV_SPECULAR_FLAG);

                    bool advOn = ((flags & REFLEXIONS_MAP_FLAG) != 0) || (EParamMode(iMode) == MODE_REFRAC);

                    if(advOn)
                    {
                        flags = flags & (~0x03); // maxPointight = 0
                        flags = flags & (~SPECULAR_FLAG);
                    }
                    else if((flags & SHADOW_FLAG) != 0) // Une seule lumière max => supprime le 2ième bit
                    {
                        flags = flags & (~0x02); // maxPointight = 1
                        flags = flags & (~SPECULAR_FLAG);
                    }
                    else
                    {
                        flags = flags & (~0x02); // maxPointight = 1
                    }
                }

                for(int32 iGroup=0; iGroup < ESkinLightingGroup_COUNT; iGroup++)
                {
                    getParams(params, ERenderPass(iPass), EParamMode(iMode), ESkinLightingGroup(iGroup), true, flags);
                }

                for(int32 iGroup=0; iGroup < EMeshLightingGroup_COUNT; iGroup++)
                {
                    getParams(params, ERenderPass(iPass), EParamMode(iMode), EMeshLightingGroup(iGroup), false, flags);
                }
            }
        }
    }
}

void StandardMatRenderer::getParams(ShadersParams & params, ERenderPass pass, EParamMode mode, int32 iGroup, bool skin, int32 flags)
{
#ifndef LM_DEVMODE
    if(pass == PASS_RADIOSITY || pass == PASS_DEBUG)
        pass = PASS_LIGHTING;
#endif

    Core::List<Gfx::ShaderMacro> macrosVS;
    Core::List<Gfx::ShaderMacro> macrosPS;

    if(pass == PASS_LIGHTMAP_GENERATION)
        flags |= HQ_PREVIEW_FLAG;

    // Options générales (indépendantes des arguments, uniquement de la config/niveau de détail)

    macrosVS.push_back(Gfx::ShaderMacro(L"MAX_BONES", L"70"));

    if(_pFormat->packedNormals())
        macrosVS.push_back(Gfx::ShaderMacro(L"PACKED_NORMAL_FLAG", L"1"));

    if((flags & INTERNAL_TEX_BORDER_FLAG) != 0)
        macrosVS.push_back(Gfx::ShaderMacro(L"TEXTURE_INTERNAL_BORDER_FLAG", L"1"));

    // Valeurs par défaut

    params.skinnedStreamData = VD_SKIN_G;
    params.staticStreamData = VD_STATIC_G;

    // Options communes

    if(skin) 
        macrosVS.push_back(Gfx::ShaderMacro(L"SKIN_FLAG", L"1"));

    if(mode == MODE_REFRAC && pass == PASS_LIGHTING)
    {
        macrosVS.push_back(Gfx::ShaderMacro(L"REFRACTIONS_FLAG", L"1"));
        macrosPS.push_back(Gfx::ShaderMacro(L"REFRACTIONS_FLAG", L"1"));
    }

    // Passes

    switch(pass)
    {
    case PASS_WIREFRAME:
    {
        params.pVShader = _pShaderLib->getVShader(L"standard-wireframe.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        params.pPShader = _pShaderLib->getPShader(L"standard-wireframe.psh", Gfx::PS_V2_0, L"ps_main", macrosPS);
        break;
    }
    case PASS_STATIC_SHADOW:
    {
        params.pVShader = _pShaderLib->getVShader(L"standard-static-shadow.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        params.pPShader = _pShaderLib->getPShader(L"standard-static-shadow.psh", Gfx::PS_V2_0, L"ps_main", macrosPS);
        break;
    }
    case PASS_DYNAMIC_SHADOW:
    {
        params.pVShader = _pShaderLib->getVShader(L"standard-dynamic-shadow.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        params.pPShader = _pShaderLib->getPShader(L"standard-dynamic-shadow.psh", Gfx::PS_V2_0, L"ps_main", macrosPS);
        break;
    }
    case PASS_GLOW:
    {
        params.pVShader = _pShaderLib->getVShader(L"standard-glow.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        params.pPShader = _pShaderLib->getPShader(L"standard-glow.psh", Gfx::PS_V2_0, L"ps_main", macrosPS);
        break;
    }
    case PASS_LIGHTMAP_GENERATION:
        LM_ASSERT(mode == MODE_SOLID);
    case PASS_LIGHTING:
    case PASS_REFLECTION:
    case PASS_DEBUG:
    {
        Gfx::EVertexShaderVersion vsv = Gfx::VS_V1_1;
        Gfx::EPixelShaderVersion  psv = (_settings.shaderLevel <= SHADER_LOW) ? Gfx::PS_V2_0 : Gfx::PS_V2_a;

        if(pass == PASS_REFLECTION)
            macrosPS.push_back(Gfx::ShaderMacro(L"WATER_CLIP_FLAG", L"1"));

        if(pass == PASS_DEBUG)
        {
            macrosPS.push_back(Gfx::ShaderMacro(L"DEBUG_FLAG", L"1"));
            macrosVS.push_back(Gfx::ShaderMacro(L"DEBUG_FLAG", L"1"));
            vsv = Gfx::VS_V3_0;
            psv = Gfx::PS_V3_0;
        }

        // Gestion groupe 

        if(skin)
        {
            switch((ESkinLightingGroup)iGroup)
            {
            case LGROUP_SKIN:
                params.skinnedStreamData = VD_SKIN_G;
                break;
            case LGROUP_SKIN_TSN:
                params.skinnedStreamData = VD_SKIN_G_B;
                macrosVS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                macrosPS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                break;
            }
        }
        else
        {
            switch((EMeshLightingGroup)iGroup)
            {
            case LGROUP_STATIC:
                params.staticStreamData = VD_STATIC_G;
                macrosVS.push_back(Gfx::ShaderMacro(L"STATIC_PIXEL_LIGHTING_FLAG", L"1"));
                macrosPS.push_back(Gfx::ShaderMacro(L"STATIC_PIXEL_LIGHTING_FLAG", L"1"));
                break;
            case LGROUP_STATIC_TSN:
                params.staticStreamData = VD_STATIC_G_B;
                macrosVS.push_back(Gfx::ShaderMacro(L"STATIC_PIXEL_LIGHTING_FLAG", L"1"));
                macrosPS.push_back(Gfx::ShaderMacro(L"STATIC_PIXEL_LIGHTING_FLAG", L"1"));
                macrosVS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                macrosPS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                break;
            case LGROUP_DYNAMIC:
                params.staticStreamData = VD_STATIC_G;
                break;
            case LGROUP_DYNAMIC_TSN:
                params.staticStreamData = VD_STATIC_G_B;
                macrosVS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                macrosPS.push_back(Gfx::ShaderMacro(L"TSN_FLAG", L"1"));
                break;
            }
        }

        if(pass == PASS_DEBUG)
        {
            if(skin)
                params.skinnedStreamData = VD_SKIN_G_B;
            else
                params.staticStreamData = VD_STATIC_G_B;
        }

        // Gestion flags

        if(flags & HQ_PREVIEW_FLAG)
        {
            macrosVS.push_back(Gfx::ShaderMacro(L"HQ_PREVIEW_FLAG", L"1"));
            macrosPS.push_back(Gfx::ShaderMacro(L"HQ_PREVIEW_FLAG", L"1"));
            vsv = Gfx::VS_V3_0;
            psv = Gfx::PS_V3_0;
        }

        if(flags & SPECULAR_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"SPECULAR_FLAG", L"1"));
        if(flags & ADV_SPECULAR_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"ADV_SPECULAR_FLAG", L"1"));
        if(flags & REFLEXIONS_MAP_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"REFLEXIONS_MAP_FLAG", L"1"));
        if(flags & NORMAL_MAP_DXT5_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"NORMAL_MAP_DXT5_FLAG", L"1"));
        if((flags & SHADOW_FLAG) && (mode != MODE_REFRAC))
        {
            macrosVS.push_back(Gfx::ShaderMacro(L"SHADOW_FLAG", L"1"));
            macrosPS.push_back(Gfx::ShaderMacro(L"SHADOW_FLAG", L"1"));
        }

        int32 lightCount = flags & POINT_LIGHT_COUNT_MASK;
        macrosPS.push_back(Gfx::ShaderMacro(L"POINT_LIGHT_COUNT", Core::toString(lightCount)));

        if(pass == PASS_LIGHTMAP_GENERATION)
        {
            params.pVShader = _pShaderLib->getVShader(L"generate-lightmap.vsh", vsv, L"vs_main", macrosVS);
            params.pPShader = _pShaderLib->getPShader(L"generate-lightmap.psh", psv, L"ps_main", macrosPS);
        }
        else
        {
            params.pVShader = _pShaderLib->getVShader(L"standard.vsh", vsv, L"vs_main", macrosVS);
            params.pPShader = _pShaderLib->getPShader(L"standard.psh", psv, L"ps_main", macrosPS);
        }
        break;
    }
    } // switch pass

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    Ptr<Gfx::ShaderConstantTable> pVConstTable = params.pVConst;
    Ptr<Gfx::ShaderConstantTable> pPConstTable = params.pPConst;

    if(skin)
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.skinnedStreamData);
    else
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.staticStreamData);

    params.idWorldViewProj = pVConstTable->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorld = pVConstTable->getConstantIndexIfExists(L"World");
    params.idWorldInvTrans = pVConstTable->getConstantIndexIfExists(L"WorldInvTrans");
    params.idViewProj = pVConstTable->getConstantIndexIfExists(L"ViewProj");
    params.idStaticLightViewProj = pVConstTable->getConstantIndexIfExists(L"StaticLightViewProj");
    params.idDynamicLightViewProj0 = pVConstTable->getConstantIndexIfExists(L"DynamicLightViewProj0");
    params.idDynamicLightViewProj1 = pVConstTable->getConstantIndexIfExists(L"DynamicLightViewProj1");
    params.idWorldBone = pVConstTable->getConstantIndexIfExists(L"WorldBone");
    params.idEyePosition = pVConstTable->getConstantIndexIfExists(L"EyePosition");
    params.idFogRange = pVConstTable->getConstantIndexIfExists(L"FogRange");
    params.idGlowExtent = pVConstTable->getConstantIndexIfExists(L"GlowExtent");
    params.idDiffuseMapSize = pVConstTable->getConstantIndexIfExists(L"DiffuseMapSize");
    params.idLightingMapSize = pVConstTable->getConstantIndexIfExists(L"LightingMapSize");
    params.idZBias = pVConstTable->getConstantIndexIfExists(L"ZBias");
    params.idViewportSize = pVConstTable->getConstantIndexIfExists(L"ViewportSize");
    params.idIsTextureBorderOn = pVConstTable->getConstantIndexIfExists(L"IsTextureBorderOn");

    /////// PS
    params.idView = pPConstTable->getConstantIndexIfExists(L"View");
    
    params.idMainLightDir = pPConstTable->getConstantIndexIfExists(L"MainLightDir");
    params.idMainLightAmbient = pPConstTable->getConstantIndexIfExists(L"MainLightAmbient");
    params.idMainLightDiffuse0 = pPConstTable->getConstantIndexIfExists(L"MainLightDiffuse0");
    params.idMainLightDiffuse1 = pPConstTable->getConstantIndexIfExists(L"MainLightDiffuse1");
    params.idMainLightDiffuse2 = pPConstTable->getConstantIndexIfExists(L"MainLightDiffuse2");
    params.idMainLightSpecular = pPConstTable->getConstantIndexIfExists(L"MainLightSpecular");

    params.idOcclusion = pPConstTable->getConstantIndexIfExists(L"Occlusion");

    params.idStaticShadowDiffusion = pPConstTable->getConstantIndexIfExists(L"StaticShadowDiffusion");

    params.idLightMapReflexion = pPConstTable->getConstantIndexIfExists(L"LightMapReflexion");
    params.idFogColor = pPConstTable->getConstantIndexIfExists(L"FogColor");
    params.idWaterLevel = pPConstTable->getConstantIndexIfExists(L"WaterLevel");

    params.idDynamicShadowStart = pPConstTable->getConstantIndexIfExists(L"DynamicShadowStart");
    params.idDynamicShadowRange = pPConstTable->getConstantIndexIfExists(L"DynamicShadowRange");
    params.idDynamicShadowOpacity = pPConstTable->getConstantIndexIfExists(L"DynamicShadowOpacity");

    params.idMatEmissive = pPConstTable->getConstantIndexIfExists(L"MatEmissive");
    params.idMatDiffuse = pPConstTable->getConstantIndexIfExists(L"MatDiffuse");
    params.idMatSpecular = pPConstTable->getConstantIndexIfExists(L"MatSpecular");
    params.idMatShininess = pPConstTable->getConstantIndexIfExists(L"MatShininess");
    params.idMatReflexions = pPConstTable->getConstantIndexIfExists(L"MatReflexions");
    params.idMatRefraction = pPConstTable->getConstantIndexIfExists(L"MatRefraction");
    params.idMatRefrScale = pPConstTable->getConstantIndexIfExists(L"MatRefrScale");
    params.idMatRefrIndex = pPConstTable->getConstantIndexIfExists(L"MatRefrIndex");
    params.idMatGlow = pPConstTable->getConstantIndexIfExists(L"MatGlow");
   
    params.idSamplerDiffuse = pPConstTable->getConstantIndexIfExists(L"SamplerDiffuse");
    params.idSamplerSpecular = pPConstTable->getConstantIndexIfExists(L"SamplerSpecular");
    params.idSamplerNormal = pPConstTable->getConstantIndexIfExists(L"SamplerNormal");
    params.idSamplerLight = pPConstTable->getConstantIndexIfExists(L"SamplerLight");
    params.idSamplerOcclusion = pPConstTable->getConstantIndexIfExists(L"SamplerOcclusion");
    params.idSamplerStaticShadow = pPConstTable->getConstantIndexIfExists(L"SamplerStaticShadow");
    params.idSamplerDynamicShadow0 = pPConstTable->getConstantIndexIfExists(L"SamplerDynamicShadow0");
    params.idSamplerDynamicShadow1 = pPConstTable->getConstantIndexIfExists(L"SamplerDynamicShadow1");
    params.idSamplerReflexions = pPConstTable->getConstantIndexIfExists(L"SamplerReflexions");
    params.idSamplerRefraction = pPConstTable->getConstantIndexIfExists(L"SamplerRefraction");
    params.idSamplerDebugMIP = pPConstTable->getConstantIndexIfExists(L"SamplerDebugMIP");

    params.idIsDebugNormal = pPConstTable->getConstantIndexIfExists(L"IsDebugNormal");
    params.idIsDebugUV = pPConstTable->getConstantIndexIfExists(L"IsDebugUV");
    params.idIsDebugT = pPConstTable->getConstantIndexIfExists(L"IsDebugT");
    params.idIsDebugB = pPConstTable->getConstantIndexIfExists(L"IsDebugB");
    params.idIsDebugN = pPConstTable->getConstantIndexIfExists(L"IsDebugN");
    params.idIsDebugMIP = pPConstTable->getConstantIndexIfExists(L"IsDebugMIP");

    for(int32 iLight=0; iLight < MAX_POINT_LIGHTS_COUNT; iLight++)
    {
        String index(Core::toString(iLight));
        params.pointLights[iLight].idPosition = pPConstTable->getConstantIndexIfExists(L"PointLightPosition" + index);
        params.pointLights[iLight].idAmbient = pPConstTable->getConstantIndexIfExists(L"PointLightAmbient" + index);
        params.pointLights[iLight].idDiffuse = pPConstTable->getConstantIndexIfExists(L"PointLightDiffuse" + index);
        params.pointLights[iLight].idSpecular = pPConstTable->getConstantIndexIfExists(L"PointLightSpecular" + index);
        params.pointLights[iLight].idInvRadius = pPConstTable->getConstantIndexIfExists(L"PointLightInvRadius" + index);
    }

    // Render states

    Gfx::RSRasterizerDesc   raster(Gfx::CM_BACK, ((flags & INVERTED_CULLING_FLAG) != 0) ? false : true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, true, Gfx::COMPARISON_LESS_EQUAL);
    Gfx::RSBlendDesc        blend;

    switch(mode)
    {
    case MODE_TRANS_BACK:
    {
        raster.cullMode = Gfx::CM_FRONT;
        depth.isDepthWriteEnabled = false;
        blend = Gfx::RSBlendDesc(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
        blend.sRGBWriteEnabled = true;
        break;
    }
    case MODE_TRANS_FRONT:
    {
        raster.cullMode = Gfx::CM_BACK;
        depth.isDepthWriteEnabled = false;
        blend = Gfx::RSBlendDesc(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);
        blend.sRGBWriteEnabled = true;
        break;
    }
    case MODE_REFRAC:
    case MODE_SOLID:
    {
        switch(pass)
        {
        case PASS_WIREFRAME:
        {
            raster.fillMode = Gfx::FM_WIREFRAME;
            break;
        }
        case PASS_STATIC_SHADOW:
        {
            blend.sRGBWriteEnabled = false;
            break;
        }
        case PASS_LIGHTMAP_GENERATION:
        {
            depth = Gfx::RSDepthStencilDesc(false, false, Gfx::COMPARISON_LESS_EQUAL);
            blend.sRGBWriteEnabled = true;
            raster.cullMode = Gfx::CM_NOCM;
            break;
        }
        case PASS_DYNAMIC_SHADOW:
        {
            depth = Gfx::RSDepthStencilDesc(false, false, Gfx::COMPARISON_LESS_EQUAL);
            blend = Gfx::RSBlendDesc(Gfx::BM_ONE, Gfx::BO_MIN, Gfx::BM_ONE);
            blend.sRGBWriteEnabled = false;
            raster.cullMode = Gfx::CM_NOCM;
            break;
        }
        case PASS_DEBUG:
        {
            blend.sRGBWriteEnabled = false;
            break;
        }
        case PASS_LIGHTING:
        {
            blend.sRGBWriteEnabled = true;
            break;
        }
        case PASS_GLOW:
        {
            blend.sRGBWriteEnabled = true;
            depth.isDepthWriteEnabled = false;
            break;
        }
        case PASS_REFLECTION:
        {
            blend.sRGBWriteEnabled = true;
            raster.cullMode = Gfx::CM_NOCM;
            break;
        }
        } // switch pass
        break;
    }
    };

    params.state = _pDevice->createState(raster, depth, blend);

    Gfx::ETextureAddressMode addrMode = ((flags & CLAMP_TEX_BORDER_FLAG) != 0) ? Gfx::TEXTURE_ADDRESS_CLAMP : Gfx::TEXTURE_ADDRESS_WRAP;

    Gfx::RSSamplerDesc samplerRGB;
    samplerRGB.isSRGB = true;
    samplerRGB.addrModeU = addrMode;
    samplerRGB.addrModeV = addrMode;
    setSampler(samplerRGB, _settings.filterLevel);

    Gfx::RSSamplerDesc samplerRefrac;
    samplerRefrac.isSRGB = true;
    samplerRefrac.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerRefrac.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
    setSampler(samplerRefrac, FILTER_BILINEAR);

    if(params.idSamplerDiffuse != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerDiffuse)] = _pDevice->createState(samplerRGB);

    if(params.idSamplerSpecular != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerSpecular)] = _pDevice->createState(samplerRGB);

    if(params.idSamplerReflexions != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerReflexions)] = _pDevice->createState(samplerRGB);

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerRefrac);

    if(params.idSamplerLight != Gfx::UNDEFINED_SHADER_CONST)
    {
        Gfx::RSSamplerDesc samplerLight;
        samplerLight.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
        samplerLight.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
        samplerLight.isSRGB = true;
        setSampler(samplerLight, _settings.filterLevel);

        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerLight)] = _pDevice->createState(samplerLight);
    }

    if(params.idSamplerOcclusion != Gfx::UNDEFINED_SHADER_CONST)
    {
        Gfx::RSSamplerDesc samplerOcclusion;
        samplerOcclusion.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
        samplerOcclusion.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
        setSampler(samplerOcclusion, _settings.filterLevel);

        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerOcclusion)] = _pDevice->createState(samplerOcclusion);
    }

    if(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST)
    {
        Gfx::RSSamplerDesc samplerNormal;
        samplerNormal.addrModeU = addrMode;
        samplerNormal.addrModeV = addrMode;
        samplerNormal.isSRGB = false;
        setSampler(samplerNormal, _settings.filterLevel);

        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerNormal)] = _pDevice->createState(samplerNormal);
    }

    if(params.idSamplerDebugMIP != Gfx::UNDEFINED_SHADER_CONST)
    {
        Gfx::RSSamplerDesc samplerDebugMIP;
        setSampler(samplerDebugMIP, Renderer::FILTER_BILINEAR);

        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerDebugMIP)] = _pDevice->createState(samplerDebugMIP);
    }

    Gfx::RSSamplerDesc samplerShadow;
    samplerShadow.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerShadow.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerShadow.isSRGB = false;
    setSampler(samplerShadow, _settings.filterLevel); // TODO : peut être pas

    if(params.idSamplerDynamicShadow0 != Gfx::UNDEFINED_SHADER_CONST || params.idSamplerDynamicShadow1 != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(params.idSamplerDynamicShadow0 != Gfx::UNDEFINED_SHADER_CONST)
            params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerDynamicShadow0)] = _pDevice->createState(samplerShadow);
        
        if(params.idSamplerDynamicShadow1 != Gfx::UNDEFINED_SHADER_CONST)
            params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerDynamicShadow1)] = _pDevice->createState(samplerShadow);
    }

    Gfx::RSSamplerDesc samplerDepth;
    samplerDepth.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerDepth.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerDepth.isSRGB = false;
    samplerDepth.filter = Gfx::FILTER_MIN_MAG_MIP_POINT;

    if(params.idSamplerStaticShadow != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[pPConstTable->getSamplerId(params.idSamplerStaticShadow)] = _pDevice->createState(samplerDepth);

    params.initOK = true;
}

ESkinLightingGroup StandardMatRenderer::getSkinLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    ESkinLightingGroup result = LGROUP_SKIN;

    if(mat.pNormalMap != null)
        result = LGROUP_SKIN_TSN;

    return result;
}

EMeshLightingGroup StandardMatRenderer::getMeshLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    EMeshLightingGroup result = LGROUP_DYNAMIC;
    
    if(mat.staticLighting)
    {
        if(mat.pNormalMap != null) 
            result = LGROUP_STATIC_TSN;
        else                       
            result = LGROUP_STATIC;
    }
    else
    {
        if(mat.pNormalMap != null) 
            result = LGROUP_DYNAMIC_TSN;
        else                       
            result = LGROUP_DYNAMIC;
    }

    return result;
}

int32 StandardMatRenderer::getLightingFlags(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    bool refracOn = !mat.refracColor.isZero();
    bool reflecOn = (data.pReflexionsCube != null && !data.isUpdatingReflexion);
    bool advOn = refracOn || reflecOn;
    bool isDynamicShadowOn = _isDynamicShadowOn && mat.dynamicShadowReceiver;
    int32 flags = 0;

    int32 maxPointLight = MAX_POINT_LIGHTS_COUNT;

    switch(_settings.shaderLevel)
    {
    case SHADER_NONE:
        LM_ASSERT(false);
    case SHADER_LOW:
        if(refracOn && reflecOn)
            reflecOn = false;

        if(advOn)
        {
            maxPointLight = 0;
        }
        else if(isDynamicShadowOn)
        {
            maxPointLight = 1;
        }
        else
        {
            flags |= SPECULAR_FLAG;
            maxPointLight = 1;
        }
        break;

    case SHADER_MEDIUM:
    case SHADER_HIGH:
        flags |= SPECULAR_FLAG;
        flags |= ADV_SPECULAR_FLAG;
        break;
    }

    if(isDynamicShadowOn)
        flags |= SHADOW_FLAG;

    if(reflecOn)
        flags |= REFLEXIONS_MAP_FLAG;

    if(mat.clampTextureBorder)
        flags |= CLAMP_TEX_BORDER_FLAG;

    if(mat.internalTextureBorder)
        flags |= INTERNAL_TEX_BORDER_FLAG;

    if(mat.pNormalMap != null && mat.pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
        flags |= NORMAL_MAP_DXT5_FLAG;

    if(mat.invertedCulling)
        flags |= INVERTED_CULLING_FLAG;

    if(_mode.renderMode == RDR_HQ_PREVIEW)
        flags |= HQ_PREVIEW_FLAG;

    return flags += std::min(maxPointLight, int32(lighting.pointLights.size()));
}

bool StandardMatRenderer::isTransparent(const Material & mat) const
{
    return mat.diffuse.a < 1.0 || mat.transDiffuseTex;
}

void StandardMatRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _commands.clear();

    if(pass == PASS_LIGHTING)
    {
        if(_mode.polyMode == POLY_SOLID || _mode.polyMode == POLY_SOLID_WIREFRAME)
            if(_mode.renderMode == RDR_NORMAL || _mode.renderMode == RDR_HQ_PREVIEW)
                internalStartContext(context, PASS_LIGHTING);
            else
                internalStartContext(context, PASS_DEBUG);

        if(_mode.polyMode == POLY_WIREFRAME || _mode.polyMode == POLY_SOLID_WIREFRAME)
            internalStartContext(context, PASS_WIREFRAME);
    }
    else
    {
        internalStartContext(context, pass);
    }
}

void StandardMatRenderer::internalStartContext(const RenderContext & context, ERenderPass pass)
{
    _viewportSize = Core::Vector2f(float(context.getViewportWidth()), float(context.getViewportHeight()));
    _view = context.getView();
    _proj = context.getProj();
    _eye = context.getEye();
    _staticLightProj = context.getStaticLightProj();
    _staticLightViewProj = context.getStaticLightViewProj();
    _dynamicLightViewProj0 = context.getLightViewProj0();
    _dynamicLightViewProj1 = context.getLightViewProj1();
    _mainLightSettings = context.getLight();
    _fogSettings = context.getFog();
    _waterLevel = context.getWaterLevel();
    _isDynamicShadowOn = context.isDynamicShadowOn();
    _pRenderTarget = context.getRenderTarget();

    Command command;
    command.pass      = pass;
    command.pExecuter = this;

    const Core::List<MeshInstanceSM2 *> & meshes = context.getMeshes();

    for(int32 ii=0; ii < meshes.size(); ii++)
    {
        if(!meshes[ii]->getMatData().isHidden)
        {
            const Material & mat = meshes[ii]->getMaterial();

            if(pass == PASS_GLOW && mat.glowColor.isZero())
                continue;

            command.mode      = (!mat.refracColor.isZero() || isTransparent(mat)) ? CMD_TRANS : CMD_SOLID;
            command.camDist   = Core::dot(meshes[ii]->getBoundingBox().getCenter() - context.getEye(), context.getEyeDir());
            command.pExecData = (void*)meshes[ii];
            command.flags     = 0;

            if(command.mode == CMD_SOLID)
            {
                const MeshInstanceSM2 & mesh    = *meshes[ii];
                const Material &        mat     = mesh.getMaterial();
                const MaterialData &    mdata   = mesh.getMatData();
                const LightingData &    ldata   = mesh.getLightingData();

                EMeshLightingGroup      group   = getMeshLightingGroup(mat, mdata, ldata);
                int32                   flags   = getLightingFlags(mat, mdata, ldata);

                ShadersParams & params = getMeshParams(MODE_SOLID, pass, group, flags);
                command.pExecData2 = (void*)&params;
            }

            if(pass != PASS_DYNAMIC_SHADOW || command.mode != CMD_TRANS)
                _commands.push_back(command);
       }
    }

    const Core::List<SkinMeshInstanceSM2 *> & skins = context.getSkinMeshes();

    for(int32 ii=0; ii < skins.size(); ii++)
    {
        if(!skins[ii]->getMatData().isHidden)
        {
            const Material & mat = skins[ii]->getMaterial();

            if(pass == PASS_GLOW && mat.glowColor.isZero())
                continue;

            command.mode      = (!mat.refracColor.isZero() || isTransparent(mat)) ? CMD_TRANS : CMD_SOLID;
            command.camDist   = Core::dot(skins[ii]->getBoundingBox().getCenter() - context.getEye(), context.getEyeDir());
            command.pExecData = (void*)skins[ii];
            command.flags     = 1;

            if(command.mode == CMD_SOLID)
            {
                SkinMeshInstanceSM2 &   skin    = *skins[ii];
                const Material &        mat     = skin.getMaterial();
                const MaterialData &    mdata   = skin.getMatData();
                const LightingData &    ldata   = skin.getLightingData();

                ESkinLightingGroup      group   = getSkinLightingGroup(mat, mdata, ldata);
                int32                   flags   = getLightingFlags(mat, mdata, ldata);

                ShadersParams & params  = getSkinParams(MODE_SOLID, pass, group, flags);
                command.pExecData2 = (void*)&params;
            }

            if(pass != PASS_DYNAMIC_SHADOW || command.mode != CMD_TRANS)
                _commands.push_back(command);
        }
    }
}

void StandardMatRenderer::endContext()
{
}

void StandardMatRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}

void StandardMatRenderer::exec(Command * pStart, Command * pEnd)
{
    LM_START_PERF_LOG(L"stdmat");

    SceneInfos infos;
    infos.eye = _eye;
    infos.viewProj = _proj * _view;
    infos.staticLightViewProj = _staticLightViewProj;
    infos.dynamicLightViewProj0 = _dynamicLightViewProj0;
    infos.dynamicLightViewProj1 = _dynamicLightViewProj1;

    _pLastParams = NULL;

    while(pStart != pEnd)
    {
        if(pStart->flags != 0)
        {
            // Skin
            SkinMeshInstanceSM2 &   skin    = *(SkinMeshInstanceSM2*)pStart->pExecData;
            const Material &        mat     = skin.getMaterial();
            const MaterialData &    mdata   = skin.getMatData();
            const LightingData &    ldata   = skin.getLightingData();

            ESkinLightingGroup      group   = getSkinLightingGroup(mat, mdata, ldata);
            int32                   flags   = getLightingFlags(mat, mdata, ldata);

            if(pStart->mode == CMD_TRANS)
            {
                if(!mat.refracColor.isZero())
                {
                    ShadersParams & params  = getSkinParams(MODE_REFRAC, pStart->pass, group, flags);

                    if(bindParams(params, pStart->pass, MODE_REFRAC, group, flags, infos))
                    {
                        _pRenderTarget->updateRefractionBuffer();
                        renderSkinMesh(params, infos, skin);
                    }
                }
                else
                {
                    ShadersParams & paramsBack  = getSkinParams(MODE_TRANS_BACK,  pStart->pass, group, flags);
                    ShadersParams & paramsFront = getSkinParams(MODE_TRANS_FRONT, pStart->pass, group, flags);

                    if(bindParams(paramsBack, pStart->pass, MODE_TRANS_BACK, group, flags, infos))
                        renderSkinMesh(paramsBack, infos, skin);
                    if(bindParams(paramsFront, pStart->pass, MODE_TRANS_FRONT, group, flags, infos))
                        renderSkinMesh(paramsFront, infos, skin);
                }
            }
            else
            {
                ShadersParams & params  = *((ShadersParams*)pStart->pExecData2);

                if(bindParams(params, pStart->pass, MODE_SOLID, group, flags, infos))
                    renderSkinMesh(params, infos, skin);
            }

            int a = 1;
        }
        else
        {
            // Mesh
            MeshInstanceSM2 &       mesh    = *(MeshInstanceSM2*)pStart->pExecData;
            const Material &        mat     = mesh.getMaterial();
            const MaterialData &    mdata   = mesh.getMatData();
            const LightingData &    ldata   = mesh.getLightingData();

            EMeshLightingGroup      group   = getMeshLightingGroup(mat, mdata, ldata);
            int32                   flags   = getLightingFlags(mat, mdata, ldata);

            if(pStart->mode == CMD_TRANS)
            {
                if(!mat.refracColor.isZero())
                {
                    ShadersParams & params  = getMeshParams(MODE_REFRAC, pStart->pass, group, flags);

                    if(bindParams(params, pStart->pass, MODE_REFRAC, group, flags, infos))
                    {
                        _pRenderTarget->updateRefractionBuffer();
                        renderMesh(params, infos, mesh);
                    }
                }
                else
                {
                    ShadersParams & paramsBack  = getMeshParams(MODE_TRANS_BACK,  pStart->pass, group, flags);
                    ShadersParams & paramsFront = getMeshParams(MODE_TRANS_FRONT, pStart->pass, group, flags);

                    if(bindParams(paramsBack, pStart->pass, MODE_TRANS_BACK, group, flags, infos))
                        renderMesh(paramsBack, infos, mesh);
                    if(bindParams(paramsFront, pStart->pass, MODE_TRANS_FRONT, group, flags, infos))
                        renderMesh(paramsFront, infos, mesh);
                }
            }
            else
            {
                ShadersParams & params  = *((ShadersParams*)pStart->pExecData2);

                if((flags & HQ_PREVIEW_FLAG) && mat.pNormalMap != null)
                    int b = 1;

                if(bindParams(params, pStart->pass, MODE_SOLID, group, flags, infos))
                    renderMesh(params, infos, mesh);
            }
            int a = 1;
        }

        pStart ++;
    }

    LM_END_PERF_LOG(L"stdmat");
}

StandardMatRenderer::ShadersParams & StandardMatRenderer::getMeshParams(EParamMode mode, ERenderPass pass, EMeshLightingGroup group, int32 flags)
{
    switch(pass)
    {
    case PASS_LIGHTING:
        return _paramsLightingMesh[int32(mode)][int32(group)][flags];
        break;
    case PASS_REFLECTION:
        return _paramsReflecMesh[int32(mode)][int32(group)][flags];
        break;
    case PASS_DEBUG:
        return _paramsDebugMesh[int32(group)][flags];
        break;
    case PASS_LIGHTMAP_GENERATION:
        return _paramsLightMapMesh[int32(group)][flags];
        break;
    case PASS_STATIC_SHADOW:
        return _paramsStaticShadowMesh;
        break;
    case PASS_DYNAMIC_SHADOW:
        return _paramsDynamicShadowMesh;
        break;
    case PASS_WIREFRAME:
        return _paramsWireframeMesh;
        break;
    case PASS_GLOW:
        return _paramsGlowMesh;
        break;
    }

    LM_ASSERT(false);
    return _paramsGlowMesh;
}

StandardMatRenderer::ShadersParams & StandardMatRenderer::getSkinParams(EParamMode mode, ERenderPass pass, ESkinLightingGroup group, int32 flags)
{
    switch(pass)
    {
    case PASS_LIGHTING:
        return _paramsLightingSkin[int32(mode)][int32(group)][flags];
        break;
    case PASS_REFLECTION:
        return _paramsReflecSkin[int32(mode)][int32(group)][flags];
        break;
    case PASS_DEBUG:
        return _paramsDebugSkin[int32(group)][flags];
        break;
    case PASS_LIGHTMAP_GENERATION:
    case PASS_STATIC_SHADOW:
        LM_ASSERT(false);
        break;
    case PASS_DYNAMIC_SHADOW:
        return _paramsDynamicShadowSkin;
        break;
    case PASS_WIREFRAME:
        return _paramsWireframeSkin;
        break;
    case PASS_GLOW:
        return _paramsGlowSkin;
        break;
    }

    LM_ASSERT(false);
    return _paramsGlowMesh;
}

void StandardMatRenderer::renderMesh(ShadersParams & params, const SceneInfos & sceneInfos, const MeshInstanceSM2 & mesh)
{
    SceneInfos  infos(sceneInfos);

    Ptr<Mesh> pInstance = LM_DEBUG_PTR_CAST<Mesh>(mesh.getMesh());
    const IPackedStaticVertices & vertices = pInstance->getVertices();

    infos.world = mesh.getWorldMatrix();
    infos.worldViewProj = _proj * (_view * mesh.getWorldMatrix());
    infos.worldInvTrans = infos.world.getInverted().getTransposed();

    setObjectParams(params, infos);
    setMatParams(params, mesh.getMaterial(), mesh.getMatData());
    setLightingParams(params, mesh.getLightingData());

    params.pVConst->flushBurst();
    params.pPConst->flushBurst();

    vertices.bindData(_pDevice, params.staticStreamData);
    vertices.sendData(_pDevice, params.staticStreamData);
}

void StandardMatRenderer::renderSkinMesh(ShadersParams & params, const SceneInfos & sceneInfos, const SkinMeshInstanceSM2 & mesh)
{
    SceneInfos  infos(sceneInfos);

    Ptr<SkinMesh> pInstance = LM_DEBUG_PTR_CAST<SkinMesh>(mesh.getMesh());
    const IPackedSkinnedVertices & vertices = pInstance->getVertices();

    infos.world = mesh.getSkeleton()->getWorldMatrix();
    infos.worldViewProj = infos.viewProj * infos.world;
    infos.worldInvTrans = infos.world.getInverted().getTransposed();

    setObjectParams(params, infos);
    setMatParams(params, mesh.getMaterial(), mesh.getMatData());
    setLightingParams(params, mesh.getLightingData());

    params.pVConst->flushBurst();
    params.pPConst->flushBurst();

    vertices.bindData(_pDevice, params.skinnedStreamData);

    for(int32 groupID=0; groupID < vertices.getBlendingGroupCount(); groupID++)
    {
        setSkeltonParams(params, mesh.getPose(), vertices.getPalette(groupID));
        vertices.sendData(_pDevice, params.skinnedStreamData, groupID);
    }
}

bool StandardMatRenderer::bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, EMeshLightingGroup group, int32 flags, const SceneInfos & infos)
{
    bool result = false;

    if(!params.initOK && !params.initTried)
    {
        params.initTried = true;
        getParams(params, pass, mode, int32(group), false, flags);
    }

    if(params.initOK)
    {
        if(_pLastParams != &params)
        {
            _pDevice->setState(params.state);
            _pDevice->setVertexFormat(params.pFormat);
            _pDevice->setVertexShader(params.pVShader);
            _pDevice->setPixelShader(params.pPShader);

            params.pVConst->setBurstMode(true);
            params.pPConst->setBurstMode(true);

            setSceneParams(params, infos);
            setMainLightParams(params, infos);
            _pLastParams = &params;
        }
        result = true;
    }

    return result;
}

bool StandardMatRenderer::bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, ESkinLightingGroup group, int32 flags, const SceneInfos & infos)
{
    bool result = false;

    if(!params.initOK && !params.initTried)
    {
        params.initTried = true;
        getParams(params, pass, mode, int32(group), true, flags);
    }

    if(params.initOK)
    {
        if(_pLastParams != &params)
        {
            _pDevice->setState(params.state);
            _pDevice->setVertexFormat(params.pFormat);
            _pDevice->setVertexShader(params.pVShader);
            _pDevice->setPixelShader(params.pPShader);

            setSceneParams(params, infos);
            setMainLightParams(params, infos);
            _pLastParams = &params;
        }

        result = true;
    }

    return result;
}


void StandardMatRenderer::setMainLightParams(ShadersParams & params, const SceneInfos & infos)
{
    // Vertex shader

    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idStaticLightViewProj, infos.staticLightViewProj);
    params.pVConst->setConstantSafe(params.idDynamicLightViewProj0, infos.dynamicLightViewProj0);
    params.pVConst->setConstantSafe(params.idDynamicLightViewProj1, infos.dynamicLightViewProj1);

    // Pixel shader

    float shadowStart0 = _mainLightSettings.getDynamicShadowMiddle() - _mainLightSettings.getDynamicShadowMiddleBlend();
    float shadowStart1 = _mainLightSettings.getDynamicShadowEnd() - _mainLightSettings.getDynamicShadowEndBlend();
    float shadowZ4 = _mainLightSettings.getDynamicShadowEndBlend();

    Core::Vector2f shadowStart(shadowStart0, shadowStart1);
    Core::Vector2f shadowRange(_mainLightSettings.getDynamicShadowMiddleBlend(), _mainLightSettings.getDynamicShadowEndBlend());

    params.pPConst->setConstantSafe(params.idDynamicShadowStart, shadowStart);
    params.pPConst->setConstantSafe(params.idDynamicShadowRange, shadowRange);

    Core::Vector4f ambient(_mainLightSettings.getAmbient());
    ambient.a = _mainLightSettings.getMulIntensity() * _mainLightSettings.getGlobalFade();
    params.pPConst->setConstantSafe(params.idMainLightDir, _mainLightSettings.getDirection().getNormalizedSafe());
    params.pPConst->setConstantSafe(params.idMainLightAmbient, ambient);
    params.pPConst->setConstantSafe(params.idMainLightDiffuse0, _mainLightSettings.getDiffuse0());
    params.pPConst->setConstantSafe(params.idMainLightDiffuse1, _mainLightSettings.getDiffuse1());
    params.pPConst->setConstantSafe(params.idMainLightDiffuse2, _mainLightSettings.getDiffuse2() * (1.0f - _mainLightSettings.getStaticShadowOpacity()));
    params.pPConst->setConstantSafe(params.idMainLightSpecular, _mainLightSettings.getSpecular());

    Core::Vector2f occlusion(_mainLightSettings.getOcclusionContrast(), _mainLightSettings.getOcclusionLuminosity());
    params.pPConst->setConstantSafe(params.idOcclusion, occlusion);

    Core::Vector3f p1(_staticLightProj.apply(Core::Vector3f(0.0f, 0.0f, 0.0f)));
    Core::Vector3f p2(_staticLightProj.apply(Core::Vector3f(1.0f, 1.0f, 1.0f)));

    Core::Vector2f ssd(Core::Vector2f(p2.x - p1.x, p2.y - p1.y) * _mainLightSettings.getStaticShadowDiffusion());
    Core::Vector4f staticShadowDiffusion(ssd.x, ssd.y, _mainLightSettings.getStaticShadowOpacity(), _mainLightSettings.getStaticShadowBias());

    params.pPConst->setConstantSafe(params.idStaticShadowDiffusion, staticShadowDiffusion);

    params.pPConst->setConstantSafe(params.idFogColor, _fogSettings.getColor());
    params.pPConst->setConstantSafe(params.idWaterLevel, _waterLevel);
}

void StandardMatRenderer::setMatParams(ShadersParams & params, const Renderer::Material & mat, const Renderer::MaterialData & data)
{
    // VS

    params.pVConst->setConstantSafe(params.idGlowExtent, mat.glowExtent);
    params.pVConst->setConstantSafe(params.idZBias, 2.0f * mat.zBias);
    params.pVConst->setConstantSafe(params.idIsTextureBorderOn, mat.internalTextureBorder);

    if(mat.pDiffuseMap != null)
    {
        int32 w = mat.pDiffuseMap->getSourceTexture()->getWidth();
        int32 h = mat.pDiffuseMap->getSourceTexture()->getHeight();
        params.pVConst->setConstantSafe(params.idDiffuseMapSize, Core::Vector2f(float(w), float(h)));
    }
    else if(mat.pNormalMap != null)
    {
        int32 w = mat.pNormalMap->getSourceTexture()->getWidth();
        int32 h = mat.pNormalMap->getSourceTexture()->getHeight();
        params.pVConst->setConstantSafe(params.idDiffuseMapSize, Core::Vector2f(float(w), float(h)));
    }
    else
    {
        params.pVConst->setConstantSafe(params.idDiffuseMapSize, Core::Vector2f(1.0f, 1.0f));
    }

    if(mat.pLightMap != null)
    {
        int32 w = mat.pLightMap->getSourceTexture()->getWidth();
        int32 h = mat.pLightMap->getSourceTexture()->getHeight();
        params.pVConst->setConstantSafe(params.idLightingMapSize, Core::Vector2f(float(w), float(h)));
    }
    else
    {
        params.pVConst->setConstantSafe(params.idLightingMapSize, Core::Vector2f(1.0f, 1.0f));
    }

    // PS

    params.pPConst->setConstantSafe(params.idMatRefraction, mat.refracColor);
    params.pPConst->setConstantSafe(params.idMatRefrScale, 10.0f * mat.refracScale);
    params.pPConst->setConstantSafe(params.idMatRefrIndex, mat.refracIndex);
    params.pPConst->setConstantSafe(params.idMatEmissive, mat.emissive);
    params.pPConst->setConstantSafe(params.idMatDiffuse, mat.diffuse);
    params.pPConst->setConstantSafe(params.idMatSpecular, mat.specular);
    params.pPConst->setConstantSafe(params.idMatShininess, mat.shininess);
    params.pPConst->setConstantSafe(params.idMatReflexions, mat.reflexions);
    params.pPConst->setConstantSafe(params.idMatGlow, mat.glowColor);

    float shadowOpacity = 1.0f - _mainLightSettings.getDynamicShadowOpacity() * mat.dynamicShadowOpacity;
    params.pPConst->setConstantSafe(params.idDynamicShadowOpacity, shadowOpacity);

    if(params.idSamplerDiffuse != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(mat.pDiffuseMap == null)
            params.pPConst->setSampler2D(params.idSamplerDiffuse, _pDefaultTexWhite->getResource());
        else
            params.pPConst->setSampler2D(params.idSamplerDiffuse, LM_DEBUG_PTR_CAST<TextureMap>(mat.pDiffuseMap)->getResource());
    }

    if(params.idSamplerSpecular != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(mat.pSpecularMap == null)
            params.pPConst->setSampler2D(params.idSamplerSpecular, _pDefaultTexWhite->getResource());
        else
            params.pPConst->setSampler2D(params.idSamplerSpecular, LM_DEBUG_PTR_CAST<TextureMap>(mat.pSpecularMap)->getResource());
    }

    if(params.idSamplerNormal != Gfx::UNDEFINED_SHADER_CONST && mat.pNormalMap != null)
    {
        Ptr<TextureMap> pNormalMap = LM_DEBUG_PTR_CAST<TextureMap>(mat.pNormalMap);
        params.pPConst->setSampler2D(params.idSamplerNormal, pNormalMap->getResource());
    }

    if(params.idSamplerLight != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(mat.pLightMap == null)
            params.pPConst->setSampler2D(params.idSamplerLight, _pDefaultTexBlack->getResource());
        else
            params.pPConst->setSampler2D(params.idSamplerLight, LM_DEBUG_PTR_CAST<TextureMap>(mat.pLightMap)->getResource());
    }

    if(params.idSamplerOcclusion != Gfx::UNDEFINED_SHADER_CONST)
    {
        if(mat.pOcclusionMap == null)
            params.pPConst->setSampler2D(params.idSamplerOcclusion, _pDefaultTexWhite->getResource());
        else
            params.pPConst->setSampler2D(params.idSamplerOcclusion, LM_DEBUG_PTR_CAST<TextureMap>(mat.pOcclusionMap)->getResource());
    }

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
    {
        params.pPConst->setSampler2D(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));
    }

    if(params.idSamplerReflexions != Gfx::UNDEFINED_SHADER_CONST)
    {
        LM_ASSERT(!data.isUpdatingReflexion);
        LM_ASSERT(data.pReflexionsCube != null);
        params.pPConst->setSamplerCube(params.idSamplerReflexions, data.pReflexionsCube->getShaderResourceView());
    }
}

void StandardMatRenderer::setLightingParams(ShadersParams & params, const LightingData & lighting)
{
    LM_ASSERT(lighting.pointLights.size() <= MAX_POINT_LIGHTS_COUNT);

    for(int32 iLight=0; iLight < lighting.pointLights.size(); iLight++)
    {
        const PointLight & light = *lighting.pointLights[iLight].pLight;
        const ShadersParams::PointLightParams & lparams = params.pointLights[iLight];

        params.pPConst->setConstantSafe(lparams.idPosition, light.getPosition());
        params.pPConst->setConstantSafe(lparams.idAmbient, light.getAmbientColor());
        params.pPConst->setConstantSafe(lparams.idDiffuse, light.getDiffuseColor());
        params.pPConst->setConstantSafe(lparams.idSpecular, light.getSpecularColor());
        params.pPConst->setConstantSafe(lparams.idInvRadius, light.getInvRadius());
    }
}

void StandardMatRenderer::setSceneParams(ShadersParams & params, const SceneInfos & infos)
{
    params.pVConst->setConstantSafe(params.idViewportSize, _viewportSize);
    params.pVConst->setConstantSafe(params.idEyePosition, infos.eye);

    params.pPConst->setConstantSafe(params.idView, _view);

    params.pPConst->setConstantSafe(params.idIsDebugNormal, (_mode.renderMode == RDR_DEBUG_NORMAL));
    params.pPConst->setConstantSafe(params.idIsDebugUV, (_mode.renderMode == RDR_DEBUG_UV));
    params.pPConst->setConstantSafe(params.idIsDebugT, (_mode.renderMode == RDR_DEBUG_T));
    params.pPConst->setConstantSafe(params.idIsDebugB, (_mode.renderMode == RDR_DEBUG_B));
    params.pPConst->setConstantSafe(params.idIsDebugN, (_mode.renderMode == RDR_DEBUG_N));
    params.pPConst->setConstantSafe(params.idIsDebugMIP, (_mode.renderMode == RDR_DEBUG_MIP));

    if(params.idSamplerDebugMIP != Gfx::UNDEFINED_SHADER_CONST)
        params.pPConst->setSampler2D(params.idSamplerDebugMIP, _pDebugTexMip->getResource());

    if(params.idSamplerStaticShadow != Gfx::UNDEFINED_SHADER_CONST && _pStaticShadowMap != null)
        params.pPConst->setSampler2D(params.idSamplerStaticShadow, _pStaticShadowMap->getShaderResourceView());

    if(params.idSamplerDynamicShadow0 != Gfx::UNDEFINED_SHADER_CONST && _pDynamicShadowMap0 != null)
        params.pPConst->setSampler2D(params.idSamplerDynamicShadow0, _pDynamicShadowMap0->getShaderResourceView());

    if(params.idSamplerDynamicShadow1 != Gfx::UNDEFINED_SHADER_CONST && _pDynamicShadowMap1 != null)
        params.pPConst->setSampler2D(params.idSamplerDynamicShadow1, _pDynamicShadowMap1->getShaderResourceView());
}

void StandardMatRenderer::setObjectParams(ShadersParams & params, const SceneInfos & infos)
{
    params.pVConst->setConstantSafe(params.idWorldInvTrans, infos.worldInvTrans);
    params.pVConst->setConstantSafe(params.idWorldViewProj, infos.worldViewProj);
    params.pVConst->setConstantSafe(params.idViewProj, infos.viewProj);
    params.pVConst->setConstantSafe(params.idWorld, infos.world);
}

void StandardMatRenderer::setSkeltonParams(ShadersParams & params, const Assets::SkeletonPose & pose, const Core::List<int32> & palette)
{
    if(params.idWorldBone == Gfx::UNDEFINED_SHADER_CONST)
        throw RendererException(L"Skinned vertex shader doesn't have a WorldBone constant.");

    const Gfx::ShaderConstantDesc & desc = params.pVConst->getConstantDesc(params.idWorldBone);

    Core::List<Core::Matrix4f> bonesMat;
    bonesMat.resize(palette.size());

    for(int32 ii=0; ii < palette.size(); ii++)
        bonesMat[ii] = pose.getBoneMatrix(palette[ii]);

    if(desc.nbElements < palette.size())
    {
        String msg(L"Too many bones for skinning vertex shader. ");
        msg << Core::toString(pose.getBoneCount()) << L" required, "
            << Core::toString(desc.nbElements) << L" available.";
        throw RendererException(msg);
    }

    params.pVConst->setConstantMat3x4Array(params.idWorldBone, &bonesMat[0], bonesMat.size());
}

void StandardMatRenderer::setDebugMode(const DebugSettings & mode)
{
    _mode = mode;
}

}
