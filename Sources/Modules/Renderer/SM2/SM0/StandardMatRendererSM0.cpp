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
#include <Renderer/IRenderer.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/SM2/SM0/StandardMatRendererSM0.h>

#include <algorithm>

namespace Renderer
{
StandardMatRendererSM0::ShadersParams::ShadersParams() :
    initTried(false),
    initOK(false)
{
}

StandardMatRendererSM0::StandardMatRendererSM0(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib,
         const Ptr<PackedVerticesFormat> & pFormat,
         const Ptr<TextureMap> & pDefaultTexWhite,
         const Ptr<TextureMap> & pDefaultTexBlack,
         const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pFormat(pFormat),
    _pDefaultTexWhite(pDefaultTexWhite),
    _pDefaultTexBlack(pDefaultTexBlack),
    _settings(settings)
{
}

void StandardMatRendererSM0::setStaticShadowMap(const Ptr<Gfx::ITexture2D> & pShadowMap)
{
    // Pas utilisée
}

void StandardMatRendererSM0::setDynamicShadowMap(int32 split, const Ptr<Gfx::ITexture2D> & pShadowMap)
{
    // Pas utilisée
}

bool StandardMatRendererSM0::initialise()
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

void StandardMatRendererSM0::getParams(ShadersParams & params, ERenderPass pass, EParamMode mode, int32 iGroup, bool skin, int32 flags)
{
    Core::List<Gfx::ShaderMacro> macrosVS;

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

    // Passes

    switch(pass)
    {
    case PASS_WIREFRAME:
    case PASS_STATIC_SHADOW:
    case PASS_DYNAMIC_SHADOW:
    case PASS_GLOW:
    case PASS_LIGHTING:
    case PASS_REFLECTION:
    case PASS_DEBUG:
    {
        // Gestion groupe 

        if(skin)
        {
            switch((ESkinLightingGroup)iGroup)
            {
            case LGROUP_SKIN_TSN:
                LM_ASSERT(false);
                // no break
            case LGROUP_SKIN:
                params.skinnedStreamData = VD_SKIN_G;
                break;
            }
        }
        else
        {
            switch((EMeshLightingGroup)iGroup)
            {
            case LGROUP_STATIC_TSN:
            case LGROUP_DYNAMIC_TSN:
                LM_ASSERT(false);
                // no break
            case LGROUP_DYNAMIC:
            case LGROUP_STATIC:
                params.staticStreamData = VD_STATIC_G;
                break;
            }
        }

        // Gestion flags

        params.pVShader = _pShaderLib->getVShader(L"standard-fixed.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        break;
    }
    } // switch pass

    params.pVConst = params.pVShader->getConstantTable();

    Ptr<Gfx::ShaderConstantTable> pVConstTable = params.pVConst;

    if(skin)
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.skinnedStreamData);
    else
        params.pFormat = _pFormat->getVertexFormat(_pDevice, params.pVShader, params.staticStreamData);

    params.idWorldViewProj = pVConstTable->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorld = pVConstTable->getConstantIndexIfExists(L"World");
    params.idWorldInvTrans = pVConstTable->getConstantIndexIfExists(L"WorldInvTrans");
    params.idViewProj = pVConstTable->getConstantIndexIfExists(L"ViewProj");
    params.idWorldBone = pVConstTable->getConstantIndexIfExists(L"WorldBone");
    params.idEyePosition = pVConstTable->getConstantIndexIfExists(L"EyePosition");
    params.idFogRange = pVConstTable->getConstantIndexIfExists(L"FogRange");
    params.idDiffuseMapSize = pVConstTable->getConstantIndexIfExists(L"DiffuseMapSize");
    params.idLightingMapSize = pVConstTable->getConstantIndexIfExists(L"LightingMapSize");
    params.idZBias = pVConstTable->getConstantIndexIfExists(L"ZBias");

    params.idView = pVConstTable->getConstantIndexIfExists(L"View");
    params.idMainLightDir = pVConstTable->getConstantIndexIfExists(L"MainLightDir");
    params.idMainLightAmbient = pVConstTable->getConstantIndexIfExists(L"MainLightAmbient");
    params.idMainLightDiffuse = pVConstTable->getConstantIndexIfExists(L"MainLightDiffuse");
    params.idMainLightSpecular = pVConstTable->getConstantIndexIfExists(L"MainLightSpecular");
    params.idLightMapReflexion = pVConstTable->getConstantIndexIfExists(L"LightMapReflexion");
    params.idFogColor = pVConstTable->getConstantIndexIfExists(L"FogColor");
    params.idWaterLevel = pVConstTable->getConstantIndexIfExists(L"WaterLevel");

    params.idMatEmissive = pVConstTable->getConstantIndexIfExists(L"MatEmissive");
    params.idMatDiffuse = pVConstTable->getConstantIndexIfExists(L"MatDiffuse");
    params.idMatSpecular = pVConstTable->getConstantIndexIfExists(L"MatSpecular");
    params.idMatShininess = pVConstTable->getConstantIndexIfExists(L"MatShininess");
    params.idMatReflexions = pVConstTable->getConstantIndexIfExists(L"MatReflexions");
    params.idMatRefraction = pVConstTable->getConstantIndexIfExists(L"MatRefraction");
    params.idMatRefrScale = pVConstTable->getConstantIndexIfExists(L"MatRefrScale");
    params.idMatRefrIndex = pVConstTable->getConstantIndexIfExists(L"MatRefrIndex");
    params.idMatGlow = pVConstTable->getConstantIndexIfExists(L"MatGlow");
   
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
        case PASS_STATIC_SHADOW:
        case PASS_DYNAMIC_SHADOW:
        case PASS_WIREFRAME:
        {
            raster.fillMode = Gfx::FM_WIREFRAME;
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
    } // switch mode

    params.state = _pDevice->createState(raster, depth, blend);

    Gfx::ETextureAddressMode addrMode = ((flags & CLAMP_TEX_BORDER_FLAG) != 0) ? Gfx::TEXTURE_ADDRESS_CLAMP : Gfx::TEXTURE_ADDRESS_WRAP;

    Gfx::RSSamplerDesc samplerRGB;
    samplerRGB.isSRGB = true;
    samplerRGB.addrModeU = addrMode;
    samplerRGB.addrModeV = addrMode;
    setSampler(samplerRGB, _settings.filterLevel);

    params.state.ptSampler[0] = _pDevice->createState(samplerRGB);

    Gfx::RSSamplerDesc samplerLight;
    samplerLight.addrModeU = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerLight.addrModeV = Gfx::TEXTURE_ADDRESS_CLAMP;
    samplerLight.isSRGB = true;
    setSampler(samplerLight, _settings.filterLevel);

    params.state.ptSampler[1] = _pDevice->createState(samplerLight);

    params.initOK = true;
}

ESkinLightingGroup StandardMatRendererSM0::getSkinLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    return LGROUP_SKIN;
}

EMeshLightingGroup StandardMatRendererSM0::getMeshLightingGroup(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    EMeshLightingGroup result = LGROUP_DYNAMIC;
    
    if(mat.staticLighting)
        result = LGROUP_STATIC;
    else
        result = LGROUP_DYNAMIC;

    return result;
}

int32 StandardMatRendererSM0::getLightingFlags(const Material & mat, const MaterialData & data, const LightingData & lighting) const
{
    int32 flags = 0;

    if(mat.clampTextureBorder)
        flags |= CLAMP_TEX_BORDER_FLAG;

    if(mat.internalTextureBorder)
        flags |= INTERNAL_TEX_BORDER_FLAG;

    if(mat.invertedCulling)
        flags |= INVERTED_CULLING_FLAG;

    return flags;
}

bool StandardMatRendererSM0::isTransparent(const Material & mat) const
{
    return mat.diffuse.a < 1.0 || mat.transDiffuseTex;
}

void StandardMatRendererSM0::startContext(const RenderContext & context, ERenderPass pass)
{
    _view = context.getView();
    _proj = context.getProj();
    _eye = context.getEye();
    _mainLightSettings = context.getLight();
    _fogSettings = context.getFog();
    _pRenderTarget = context.getRenderTarget();

    // TODO
}

void StandardMatRendererSM0::endContext()
{
}

void StandardMatRendererSM0::enqueueCommands(Core::List<Command> & commands)
{
}

void StandardMatRendererSM0::exec(Command * pStart, Command * pEnd)
{
}

StandardMatRendererSM0::ShadersParams & StandardMatRendererSM0::getMeshParams(EParamMode mode, ERenderPass pass, EMeshLightingGroup group, int32 flags)
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
    case PASS_STATIC_SHADOW:
    case PASS_DYNAMIC_SHADOW:
        return _paramsShadowMesh;
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

StandardMatRendererSM0::ShadersParams & StandardMatRendererSM0::getSkinParams(EParamMode mode, ERenderPass pass, ESkinLightingGroup group, int32 flags)
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
    case PASS_STATIC_SHADOW:
    case PASS_DYNAMIC_SHADOW:
        return _paramsShadowSkin;
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

bool StandardMatRendererSM0::bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, EMeshLightingGroup group, int32 flags, const SceneInfos & infos)
{
    bool result = false;

    if(!params.initOK && !params.initTried)
    {
        params.initTried = true;
        getParams(params, pass, mode, int32(group), false, flags);
    }

    if(params.initOK)
    {
        _pDevice->setState(params.state);
        _pDevice->setVertexFormat(params.pFormat);
        _pDevice->setVertexShader(params.pVShader);
        _pDevice->setPixelShader(null);

        setSceneParams(params, infos);
        setMainLightParams(params, infos);
        result = true;
    }

    return result;
}

bool StandardMatRendererSM0::bindParams(ShadersParams & params, ERenderPass pass, EParamMode mode, ESkinLightingGroup group, int32 flags, const SceneInfos & infos)
{
    bool result = false;

    if(!params.initOK && !params.initTried)
    {
        params.initTried = true;
        getParams(params, pass, mode, int32(group), true, flags);
    }

    if(params.initOK)
    {
        _pDevice->setState(params.state);
        _pDevice->setVertexFormat(params.pFormat);
        _pDevice->setVertexShader(params.pVShader);
        _pDevice->setPixelShader(null);

        setSceneParams(params, infos);
        setMainLightParams(params, infos);
        result = true;
    }

    return result;
}


void StandardMatRendererSM0::setMainLightParams(ShadersParams & params, const SceneInfos & infos)
{
    Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

    params.pVConst->setConstantSafe(params.idFogRange, fogRange);
    params.pVConst->setConstantSafe(params.idMainLightDir, _mainLightSettings.getDirection().getNormalizedSafe());
    params.pVConst->setConstantSafe(params.idMainLightAmbient, _mainLightSettings.getAmbient());
    params.pVConst->setConstantSafe(params.idMainLightDiffuse, _mainLightSettings.getDiffuse0());
    params.pVConst->setConstantSafe(params.idMainLightSpecular, _mainLightSettings.getSpecular());
    // TODO params.pVConst->setConstantSafe(params.idLightMapReflexion, _mainLightSettings.getLightReflexion());

    params.pVConst->setConstantSafe(params.idFogColor, _fogSettings.getColor());
}

void StandardMatRendererSM0::setMatParams(ShadersParams & params, const Renderer::Material & mat, const Renderer::MaterialData & data, const SceneInfos & infos)
{
    params.pVConst->setConstantSafe(params.idZBias, 2.0f * mat.zBias);
    params.pVConst->setConstantSafe(params.idWorldInvTrans, infos.worldInvTrans);
    params.pVConst->setConstantSafe(params.idWorldViewProj, infos.worldViewProj);
    params.pVConst->setConstantSafe(params.idViewProj, infos.viewProj);
    params.pVConst->setConstantSafe(params.idWorld, infos.world);

    if(mat.pDiffuseMap != null)
    {
        int32 w = mat.pDiffuseMap->getSourceTexture()->getWidth();
        int32 h = mat.pDiffuseMap->getSourceTexture()->getHeight();
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

    Gfx::RSFixedFunc ff;
    ff.world = infos.world;
    ff.view = _view;
    ff.proj = _proj;

    if(mat.pDiffuseMap == null)
        ff.pTex0 = _pDefaultTexWhite->getResource();
    else
        ff.pTex0 = LM_DEBUG_PTR_CAST<TextureMap>(mat.pDiffuseMap)->getResource();

    if(mat.pLightMap != null)
        ff.pTex1 = LM_DEBUG_PTR_CAST<TextureMap>(mat.pLightMap)->getResource();

    ff.fogEnabled = true;
    ff.fogColor = _fogSettings.getColor();

    _pDevice->setFixedFuncPipe(ff);

    // PS

    Core::Vector4f diffuse(mat.diffuse);
    diffuse.r = std::max(diffuse.r, std::max(mat.glowColor.r, mat.reflexions.r));
    diffuse.g = std::max(diffuse.g, std::max(mat.glowColor.g, mat.reflexions.g));
    diffuse.b = std::max(diffuse.b, std::max(mat.glowColor.b, mat.reflexions.b));
    diffuse.a = std::min(diffuse.a, 1.0f - 0.8f * mat.refracColor.a);

    params.pVConst->setConstantSafe(params.idMatRefraction, mat.refracColor);
    params.pVConst->setConstantSafe(params.idMatRefrScale, 10.0f * mat.refracScale);
    params.pVConst->setConstantSafe(params.idMatRefrIndex, mat.refracIndex);
    params.pVConst->setConstantSafe(params.idMatEmissive, mat.emissive);
    params.pVConst->setConstantSafe(params.idMatDiffuse, diffuse);
    params.pVConst->setConstantSafe(params.idMatSpecular, mat.specular);
    params.pVConst->setConstantSafe(params.idMatShininess, mat.shininess);
    params.pVConst->setConstantSafe(params.idMatReflexions, mat.reflexions);
    params.pVConst->setConstantSafe(params.idMatGlow, mat.glowColor);
}

void StandardMatRendererSM0::setSceneParams(ShadersParams & params, const SceneInfos & infos)
{
    params.pVConst->setConstantSafe(params.idEyePosition, infos.eye);
    params.pVConst->setConstantSafe(params.idView, _view);
}

void StandardMatRendererSM0::setSkeltonParams(ShadersParams & params, const Assets::SkeletonPose & pose, const Core::List<int32> & palette)
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

void StandardMatRendererSM0::setDebugMode(const DebugSettings & mode)
{
    _mode = mode;
}

}
