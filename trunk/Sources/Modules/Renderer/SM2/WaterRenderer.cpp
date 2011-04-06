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
#include <Renderer/SM2/WaterRenderer.h>
#include <Renderer/Common/Texture.h>
#include <Assets/GeometricPrimitive.h>

#include <Core/Logger.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
struct WaterVertex
{
    Core::Vector3f   position;
    float            texcoords[2];
    byte             padding[12];
};

LM_STATIC_ASSERT(sizeof(WaterVertex) == 32);

static const int32 VERTEX_BUFFER_SIZE = 4 * sizeof(WaterVertex);
static const int32 INDEX_BUFFER_SIZE  = 6 * sizeof(uint16);

//-----------------------------------------------------------------------------
WaterRenderer::WaterRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib, 
                             const Ptr<TextureMap> & pDefaultTexture, const Ptr<GPUResourceLib> & pResLib,
                             const RendererSettings & settings) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib),
    _pDefaultTexture(pDefaultTexture),
    _pResLib(pResLib),
    _settings(settings)
{
    _pResLib->registerResource(this);
}
//-----------------------------------------------------------------------------
WaterRenderer::~WaterRenderer()
{
    _pResLib->unregisterResource(this);
}
//-----------------------------------------------------------------------------
bool WaterRenderer::initialise()
{
    bool result = true;
    int32 flags = 0;

    try
    {
        Core::List<Gfx::ShaderMacro> macrosVS;
        _pVShaderMask = _pShaderLib->getVShader(L"water-final-mask.vsh", Gfx::VS_V1_1, L"vs_main", macrosVS);
        _idWorldViewProjMask = _pVShaderMask->getConstantTable()->getConstantIndex(L"WorldViewProj");

        Gfx::VertexFormatDesc  formatDesc;
        formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);

        _pFormatMask = _pDevice->createVertexFormat(formatDesc, _pVShaderMask);

        Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
        Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS);
        Gfx::RSBlendDesc        blend(Gfx::BM_ONE, Gfx::BO_REV_SUBTRACT, Gfx::BM_ONE);
        _stateMask = _pDevice->createState(raster, depth, blend);

        for(flags=0; flags < WATER_CONFIG_COUNT; flags++)
            initialise(_params[flags], flags);
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing WaterRenderer : '" << exception.getMessage() << L"' (0x"
            << Core::toStringHex(flags) << L")\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void WaterRenderer::onDeviceLost()
{
    _pVBuffer = null;
    _pIBuffer = null;
}
//-----------------------------------------------------------------------------
void WaterRenderer::onDeviceReset()
{
    // Vertex buffer

    const float SIZE = 0.5f;
    int32 subDiv = 20;

    Assets::VerticeSet vset;
    Assets::createPlane(vset, SIZE, SIZE, subDiv, subDiv, 1.0f, 0.0f);

    Gfx::VertexBufferDesc vbuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, vset.getVertices().size() * sizeof(WaterVertex));
    _pVBuffer = _pDevice->createVertexBuffer(vbuff);

    WaterVertex * pVertex = reinterpret_cast<WaterVertex *>(_pVBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    for(int32 ii=0; ii < vset.getVertices().size(); ii++)
    {
        pVertex[ii].position = vset.getVertices()[ii].position;
        pVertex[ii].texcoords[0] = vset.getVertices()[ii].texCoords.x;
        pVertex[ii].texcoords[1] = vset.getVertices()[ii].texCoords.y;
    }

    _pVBuffer->unlock();

    _vcount = vset.getVertices().size();
    _tcount = vset.getIndices().size() / 3;

    // Index buffer

    Gfx::IndexBufferDesc ibuff(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, vset.getIndices().size() * sizeof(uint16));
    _pIBuffer = _pDevice->createIndexBuffer(ibuff);

    uint16 * pIndices = reinterpret_cast<uint16*>(_pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));
    memcpy(pIndices, vset.getIndices().data(), vset.getIndices().size() * sizeof(uint16));
    _pIBuffer->unlock();
}
//-----------------------------------------------------------------------------
void WaterRenderer::initialise(ShaderParams & params, int32 flags)
{
    // Shaders

    Core::List<Gfx::ShaderMacro> macrosVS;
    Core::List<Gfx::ShaderMacro> macrosPS;

    if(_settings.shaderLevel >= SHADER_MEDIUM)
        macrosPS.push_back(Gfx::ShaderMacro(L"WATER_ADVANCED_FLAG", L"1"));

    Gfx::EVertexShaderVersion vsv = Gfx::VS_V1_1;
    Gfx::EPixelShaderVersion  psv = (_settings.shaderLevel >= SHADER_MEDIUM) ? Gfx::PS_V2_a : Gfx::PS_V2_0;

    if(flags & WATER_REFRAC_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"WATER_REFRAC_FLAG", L"1"));
    if(flags & WATER_REFLEC_FLAG) macrosPS.push_back(Gfx::ShaderMacro(L"WATER_REFLEC_FLAG", L"1"));

    macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_0", (flags & WATER_IS_NORMAL_DXT5_0) ? L"true" : L"false"));
    macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_1", (flags & WATER_IS_NORMAL_DXT5_1) ? L"true" : L"false"));
    macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_2", (flags & WATER_IS_NORMAL_DXT5_2) ? L"true" : L"false"));
    macrosPS.push_back(Gfx::ShaderMacro(L"IS_NORMAL_DXT5_3", (flags & WATER_IS_NORMAL_DXT5_3) ? L"true" : L"false"));

    params.pVShader = _pShaderLib->getVShader(L"water-final.vsh", vsv, L"vs_main", macrosVS);
    params.pPShader = _pShaderLib->getPShader(L"water-final.psh", psv, L"ps_main", macrosPS);

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idWorld = params.pVConst->getConstantIndexIfExists(L"World");
    params.idEyePosition = params.pVConst->getConstantIndexIfExists(L"EyePosition");
    params.idFogOpacity = params.pVConst->getConstantIndexIfExists(L"FogRange");

    params.idFogColor = params.pPConst->getConstantIndexIfExists(L"FogColor");
    params.idWaveU = params.pPConst->getConstantIndexIfExists(L"WaveU");
    params.idWaveV = params.pPConst->getConstantIndexIfExists(L"WaveV");
    params.idWaveScaleU = params.pPConst->getConstantIndexIfExists(L"WaveScaleU");
    params.idWaveScaleV = params.pPConst->getConstantIndexIfExists(L"WaveScaleV");
    params.idWaveAmplitude = params.pPConst->getConstantIndexIfExists(L"WaveAmplitude");
    params.idWaterDiffuse = params.pPConst->getConstantIndexIfExists(L"WaterDiffuse");
    params.idWaterSpecular = params.pPConst->getConstantIndexIfExists(L"WaterSpecular");
    params.idWaterShininess = params.pPConst->getConstantIndexIfExists(L"WaterShininess");
    params.idPixelAlign = params.pPConst->getConstantIndexIfExists(L"PixelAlign");
    params.idReflScale = params.pPConst->getConstantIndexIfExists(L"ReflScale");
    params.idRefrScale = params.pPConst->getConstantIndexIfExists(L"RefrScale");
    params.idFresnelClamp = params.pPConst->getConstantIndexIfExists(L"FresnelClamp");
    params.idFresnelPower = params.pPConst->getConstantIndexIfExists(L"FresnelPower");

    params.idSamplerWave0 = params.pPConst->getConstantIndexIfExists(L"SamplerWave0");
    params.idSamplerWave1 = params.pPConst->getConstantIndexIfExists(L"SamplerWave1");
    params.idSamplerWave2 = params.pPConst->getConstantIndexIfExists(L"SamplerWave2");
    params.idSamplerWave3 = params.pPConst->getConstantIndexIfExists(L"SamplerWave3");
    params.idSamplerReflection = params.pPConst->getConstantIndexIfExists(L"SamplerReflection");
    params.idSamplerRefraction = params.pPConst->getConstantIndexIfExists(L"SamplerRefraction");
    params.idSamplerDiffuse = params.pPConst->getConstantIndexIfExists(L"SamplerDiffuse");
    params.idMainLightDir = params.pPConst->getConstantIndexIfExists(L"MainLightDir");
    params.idMainLightAmbient = params.pPConst->getConstantIndexIfExists(L"MainLightAmbient");
    params.idMainLightDiffuse = params.pPConst->getConstantIndexIfExists(L"MainLightDiffuse");
    params.idMainLightSpecular = params.pPConst->getConstantIndexIfExists(L"MainLightSpecular");

    // State

    Gfx::RSRasterizerDesc   raster(Gfx::CM_NOCM, true, Gfx::FM_SOLID);
    Gfx::RSDepthStencilDesc depth(true, false, Gfx::COMPARISON_LESS);
    Gfx::RSBlendDesc        blend;
    Gfx::RSSamplerDesc      samplerRGB(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_CLAMP, Gfx::TEXTURE_ADDRESS_CLAMP);
    Gfx::RSSamplerDesc      samplerXYZ(Gfx::FILTER_MIN_MAG_MIP_LINEAR, Gfx::TEXTURE_ADDRESS_WRAP, Gfx::TEXTURE_ADDRESS_WRAP);

    depth.isStencilEnabled  = true;
    depth.stencilRef        = 0x01;
    depth.stencilReadMask   = 0x01;
    depth.stencilWriteMask  = 0x01;
    depth.opStencil.function            = Gfx::COMPARISON_ALWAYS;
    depth.opStencil.opStencilNOK        = Gfx::REPLACE;
    depth.opStencil.opStencilOKDepthNOK = Gfx::KEEP;
    depth.opStencil.opStencilOKDepthOK  = Gfx::REPLACE;

    if(!(flags & WATER_REFRAC_FLAG))
        blend = Gfx::RSBlendDesc(Gfx::BM_SRC_ALPHA, Gfx::BO_ADD, Gfx::BM_INVERT_SRC_ALPHA);

    blend.sRGBWriteEnabled = true;
    samplerRGB.isSRGB = true;

    params.state = _pDevice->createState(raster, depth, blend);
    
    if(params.idSamplerWave0 != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerWave0)] = _pDevice->createState(samplerXYZ);
    
    if(params.idSamplerWave1 != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerWave1)] = _pDevice->createState(samplerXYZ);
    
    if(params.idSamplerWave2 != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerWave2)] = _pDevice->createState(samplerXYZ);
    
    if(params.idSamplerWave3 != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerWave3)] = _pDevice->createState(samplerXYZ);

    if(params.idSamplerReflection != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerReflection)] = _pDevice->createState(samplerRGB);

    if(params.idSamplerRefraction != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerRefraction)] = _pDevice->createState(samplerRGB);

    if(params.idSamplerDiffuse != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSamplerDiffuse)] = _pDevice->createState(samplerRGB);
    
    // Format

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_POSITION);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT2, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    params.pFormat = _pDevice->createVertexFormat(formatDesc, params.pVShader);
}
//-----------------------------------------------------------------------------
void WaterRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;

    if(pass != PASS_LIGHTING)
        return;

    _view = context.getView();
    _proj = context.getProj();
    _viewProj = context.getViewProj();
    _fogSettings = context.getFog();
    _mainLightSettings = context.getLight();
    _eye = context.getEye();
    _time = context.getTime();
    _waterLevel = context.getWaterLevel();
    _pRenderTarget = context.getRenderTarget();

    for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
        _patches[flags].clear();

    const Core::List<WaterPatch *> & patches = context.getWaterPatches();
    _patchCount = patches.size();

    for(int32 iPatch=0; iPatch < patches.size(); iPatch++)
    {
        const WaterPatch & patch = *patches[iPatch];
        int32 flags = 0;

        if(patch.isRefractive() && _settings.refracLevel > REFRAC_NONE) flags |= WATER_REFRAC_FLAG;
        if(patch.isReflective() && _settings.reflecLevel >= REFLEC_LOW) flags |= WATER_REFLEC_FLAG;
                
        WaterLayer layers[WATER_LAYER_COUNT];

        for(int32 ii=0; ii < WATER_LAYER_COUNT; ii++)
            patch.getLayer(ii, layers[ii]);

        if(layers[0].pNormalMap != null && layers[0].pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
            flags |= WATER_IS_NORMAL_DXT5_0;

        if(layers[1].pNormalMap != null && layers[1].pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
            flags |= WATER_IS_NORMAL_DXT5_1;

        if(layers[2].pNormalMap != null && layers[2].pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
            flags |= WATER_IS_NORMAL_DXT5_2;

        if(layers[3].pNormalMap != null && layers[3].pNormalMap->getSourceTexture()->getFormat() == Assets::TEX_FORMAT_DXTC5)
            flags |= WATER_IS_NORMAL_DXT5_3;

        _patches[flags].push_back(patches[iPatch]);
    }
}
//-----------------------------------------------------------------------------
void WaterRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void WaterRenderer::enqueueCommands(Core::List<Command> & commands)
{
    if(_pass == PASS_LIGHTING && _patchCount > 0)
    {
        Command command;
        command.pass      = PASS_LIGHTING;
        command.mode      = CMD_WATER;
        command.pExecuter = this;
        command.pExecData = NULL;
        command.camDist   = 0.0f;
        command.flags     = 0;

        commands.push_back(command);
    }
}
//-----------------------------------------------------------------------------
void WaterRenderer::exec(Command * pStart, Command * pEnd)
{
    LM_ASSERT(pEnd == (pStart + 1));

    for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
        renderMask(_patches[flags]);

    _pRenderTarget->updateRefractionBuffer();

    for(int32 flags=0; flags < WATER_CONFIG_COUNT; flags++)
        render(_params[flags], _patches[flags]);
}
//-----------------------------------------------------------------------------
void WaterRenderer::renderMask(const Core::List<WaterPatch *> patches)
{
    _pDevice->setState(_stateMask);
    _pDevice->setVertexFormat(_pFormatMask);
    _pDevice->setVertexShader(_pVShaderMask);
    _pDevice->setPixelShader(null);

    Gfx::RSFixedFunc ff;
    ff.fogEnabled = false;
    _pDevice->setFixedFuncPipe(ff);

    _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(WaterVertex), 0);
    _pDevice->setIndexBuffer(_pIBuffer);

    for(int32 ii=0; ii < patches.size(); ii++)
    {
        const WaterPatch & patch = *patches[ii];

        Core::Matrix4f trans(Core::TRANSLATION, patch.getPosition().x, _waterLevel, patch.getPosition().y);
        Core::Matrix4f scale(Core::SCALE, patch.getWidth(), 1.0, patch.getHeight());
        Core::Matrix4f world(trans * scale);
        Core::Matrix4f worldView(_view * world);
        Core::Matrix4f worldViewProj(_viewProj * world);

        _pVShaderMask->getConstantTable()->setConstantSafe(_idWorldViewProjMask, worldViewProj);

        _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vcount, 0, _tcount);
    }
}
//-----------------------------------------------------------------------------
void WaterRenderer::render(const ShaderParams & params, const Core::List<WaterPatch *> patches)
{
    try
    {
        _pDevice->setState(params.state);
        _pDevice->setVertexFormat(params.pFormat);
        _pDevice->setVertexShader(params.pVShader);
        _pDevice->setPixelShader(params.pPShader);

        _pDevice->setVertexBuffer(0, _pVBuffer, sizeof(WaterVertex), 0);
        _pDevice->setIndexBuffer(_pIBuffer);

        params.pPConst->setConstantSafe(params.idMainLightDir, _mainLightSettings.getDirection());
        params.pPConst->setConstantSafe(params.idMainLightAmbient, _mainLightSettings.getAmbient());
        params.pPConst->setConstantSafe(params.idMainLightDiffuse, _mainLightSettings.getDiffuse0());
        params.pPConst->setConstantSafe(params.idMainLightSpecular, _mainLightSettings.getSpecular());

        for(Core::List<WaterPatch *>::const_iterator ipPatch = patches.begin(); ipPatch != patches.end(); ++ipPatch)
        {
            const WaterPatch & patch = **ipPatch;

            WaterLayer layers[WATER_LAYER_COUNT];

            for(int32 ii=0; ii < WATER_LAYER_COUNT; ii++)
                patch.getLayer(ii, layers[ii]);

            Core::Vector4f waveSpeedU(layers[0].speedU, layers[1].speedU, layers[2].speedU, layers[3].speedU);
            Core::Vector4f waveSpeedV(layers[0].speedV, layers[1].speedV, layers[2].speedV, layers[3].speedV);

            Core::Vector4f waveU((float)_time * waveSpeedU);
            Core::Vector4f waveV((float)_time * waveSpeedV);

            Core::Vector4f waveScaleU(1.0f / layers[0].scaleU, 1.0f / layers[1].scaleU, 1.0f / layers[2].scaleU, 1.0f / layers[3].scaleU);
            Core::Vector4f waveScaleV(1.0f / layers[0].scaleV, 1.0f / layers[1].scaleV, 1.0f / layers[2].scaleV, 1.0f / layers[3].scaleV);

            Core::Vector4f waveAmplitude(layers[0].amplitude, layers[1].amplitude, layers[2].amplitude, layers[3].amplitude);

            if(layers[0].pNormalMap != null)
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave0, getResource(layers[0].pNormalMap));
            }
            else
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave0, getResource(_pDefaultTexture));
                waveAmplitude.x = 0.0f;
            }

            if(layers[1].pNormalMap != null)
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave1, getResource(layers[1].pNormalMap));
            }
            else
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave1, getResource(_pDefaultTexture));
                waveAmplitude.y = 0.0f;
            }

            if(layers[2].pNormalMap != null)
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave2, getResource(layers[2].pNormalMap));
            }
            else
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave2, getResource(_pDefaultTexture));
                waveAmplitude.z = 0.0f;
            }

            if(layers[3].pNormalMap != null)
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave3, getResource(layers[3].pNormalMap));
            }
            else
            {
                params.pPConst->setSampler2DSafe(params.idSamplerWave3, getResource(_pDefaultTexture));
                waveAmplitude.w = 0.0f;
            }

            params.pPConst->setSampler2DSafe(params.idSamplerDiffuse, getResource(patch.getDiffuseTexture()));

            Core::Matrix4f trans(Core::TRANSLATION, patch.getPosition().x, _waterLevel, patch.getPosition().y);
            Core::Matrix4f scale(Core::SCALE, patch.getWidth(), 1.0, patch.getHeight());
            Core::Matrix4f world(trans * scale);
            Core::Matrix4f worldView(_view * world);
            Core::Matrix4f worldViewProj(_viewProj * world);

            Core::Vector3f fogRange(_fogSettings.getStart(), _fogSettings.getInvRange(), _fogSettings.getColor().a);

            params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);
            params.pVConst->setConstantSafe(params.idWorld, world);
            params.pVConst->setConstantSafe(params.idEyePosition, _eye);
            params.pVConst->setConstantSafe(params.idFogOpacity, fogRange);

            Core::Vector2f  pixelAlign( 0.5f / _pRenderTarget->getViewportWidth(RT_REFRACTION_BUFFER),
                                        0.5f / _pRenderTarget->getViewportHeight(RT_REFRACTION_BUFFER));

            params.pPConst->setConstantSafe(params.idPixelAlign, pixelAlign);
            
            params.pPConst->setConstantSafe(params.idFogColor, _fogSettings.getColor());
            params.pPConst->setConstantSafe(params.idWaveU, waveU);
            params.pPConst->setConstantSafe(params.idWaveV, waveV);
            params.pPConst->setConstantSafe(params.idWaveScaleU, waveScaleU);
            params.pPConst->setConstantSafe(params.idWaveScaleV, waveScaleV);
            params.pPConst->setConstantSafe(params.idWaveAmplitude, waveAmplitude);
            params.pPConst->setConstantSafe(params.idWaterDiffuse, patch.getDiffuse());
            params.pPConst->setConstantSafe(params.idWaterSpecular, patch.getSpecular());
            params.pPConst->setConstantSafe(params.idWaterShininess, patch.getShininess());
            params.pPConst->setConstantSafe(params.idReflScale, patch.getReflectionScale());
            params.pPConst->setConstantSafe(params.idRefrScale, patch.getRefractionScale());
            params.pPConst->setConstantSafe(params.idFresnelClamp, patch.getFresnelMin());
            params.pPConst->setConstantSafe(params.idFresnelPower, patch.getFresnelPower());

            params.pPConst->setSampler2DSafe(params.idSamplerReflection, _pRenderTarget->getShaderTextureView(RT_REFLECTION_BUFFER));
            params.pPConst->setSampler2DSafe(params.idSamplerRefraction, _pRenderTarget->getShaderTextureView(RT_REFRACTION_BUFFER));

            _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, _vcount, 0, _tcount);
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error rendering with WaterRenderer : " << exception.getMessage() << L"\n";
    }
}
//-----------------------------------------------------------------------------
Ptr<Gfx::IShaderResourceView> WaterRenderer::getResource(const Ptr<ITextureMap> & pTexture) const
{
    if(pTexture == null)
        return _pDefaultTexture->getResource();
    else
        return LM_DEBUG_PTR_CAST<TextureMap>(pTexture)->getResource();
}
//-----------------------------------------------------------------------------

}


