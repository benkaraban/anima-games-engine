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
#include <Core/TGA.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/SM2/SkysphereRenderer.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
SkysphereRenderer::SkysphereRenderer(const Ptr<Gfx::IDevice> & pDevice, const Ptr<ShaderLib> & pShaderLib) :
    _pDevice(pDevice),
    _pShaderLib(pShaderLib)
{
}
//-----------------------------------------------------------------------------
void SkysphereRenderer::initialise(ShaderParams & params, ERenderPass pass, ELightingModel model, bool isProcedural)
{
#ifndef LM_DEVMODE
    if(pass == PASS_RADIOSITY || pass == PASS_DEBUG)
        pass = PASS_LIGHTING;

    isProcedural = false;
#endif

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
    case PASS_LIGHTMAP_GENERATION:
    {
        if(isProcedural)
        {
            params.pVShader = _pShaderLib->getVShader(L"skysphere-procedural.vsh", Gfx::VS_V3_0, L"vs_main", macros);
            params.pPShader = _pShaderLib->getPShader(L"skysphere-procedural.psh", Gfx::PS_V3_0, L"ps_main", macros);
        }
        else
        {
            params.pVShader = _pShaderLib->getVShader(L"skysphere.vsh", Gfx::VS_V1_1, L"vs_main", macros);
            params.pPShader = _pShaderLib->getPShader(L"skysphere.psh", Gfx::PS_V2_0, L"ps_main", macros);
        }
        break;
    }
    }

    params.pVConst = params.pVShader->getConstantTable();
    params.pPConst = params.pPShader->getConstantTable();

    params.idWorldViewProj = params.pVConst->getConstantIndexIfExists(L"WorldViewProj");
    params.idColor = params.pPConst->getConstantIndexIfExists(L"Color");
    params.idSunColor = params.pPConst->getConstantIndexIfExists(L"SunColor");
    params.idSkyColor = params.pPConst->getConstantIndexIfExists(L"SkyColor");
    params.idFogColor = params.pPConst->getConstantIndexIfExists(L"FogColor");
    params.idFogOpacity = params.pPConst->getConstantIndexIfExists(L"FogOpacity");
    params.idMainLightDir = params.pPConst->getConstantIndexIfExists(L"MainLightDir");
    params.idMainLightDiffuse = params.pPConst->getConstantIndexIfExists(L"MainLightDiffuse");
    params.idSampler = params.pPConst->getConstantIndexIfExists(L"SamplerColor");

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

    if(pass == PASS_GLOW)
    {
        depth.isStencilEnabled  = true;
        depth.stencilRef        = 0x01;
        depth.stencilReadMask   = 0x01;
        depth.stencilWriteMask  = 0x01;
        depth.opStencil.function            = Gfx::COMPARISON_NOT_EQUAL;
        depth.opStencil.opStencilNOK        = Gfx::KEEP;
        depth.opStencil.opStencilOKDepthNOK = Gfx::KEEP;
        depth.opStencil.opStencilOKDepthOK  = Gfx::KEEP;
    }

    sampler.isSRGB = true;
    blend.sRGBWriteEnabled = true;

    params.state = _pDevice->createState(raster, depth, blend);

    if(params.idSampler != Gfx::UNDEFINED_SHADER_CONST)
        params.state.ptSampler[params.pPConst->getSamplerId(params.idSampler)] = _pDevice->createState(sampler);
}
//-----------------------------------------------------------------------------
bool SkysphereRenderer::initialise()
{
    bool result = true;

    try
    {
        for(int32 iPass=0; iPass < ERenderPass_COUNT; iPass++)
        {
            for(int32 iModel=0; iModel < ELightingModel_COUNT; iModel++)
            {
                initialise(_paramsTexture[iPass][iModel], ERenderPass(iPass), ELightingModel(iModel), false);
                initialise(_paramsProcedural[iPass][iModel], ERenderPass(iPass), ELightingModel(iModel), true);
            }
        }
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initializing SkysphereRenderer : " << exception.getMessage() << L"\n";
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
void SkysphereRenderer::startContext(const RenderContext & context, ERenderPass pass)
{
    _pass = pass;
    _view = context.getView();
    _proj = context.getProj();
    _fogSettings = context.getFog();
    _lightSettings = context.getLight();

    _commands.clear();

    if((context.getSkysphere()->getSettings().pSkyBox == null) &&
       (pass == PASS_LIGHTING || pass == PASS_REFLECTION || 
       (pass == PASS_GLOW && !context.getSkysphere()->getSettings().glow.isZero())))
    {
        Command command;
        command.pass      = pass;
        command.mode      = CMD_SKYSP;
        command.pExecuter = this;
        command.pExecData = (void*)context.getSkysphere();
        command.camDist   = 0.0f;
        command.flags     = 0;

        _commands.push_back(command);
    }
}
//-----------------------------------------------------------------------------
void SkysphereRenderer::endContext()
{
}
//-----------------------------------------------------------------------------
void SkysphereRenderer::enqueueCommands(Core::List<Command> & commands)
{
    commands.insert(commands.end(), _commands.begin(), _commands.end());
}
//-----------------------------------------------------------------------------
void SkysphereRenderer::exec(Command * pStart, Command * pEnd)
{
    while(pStart != pEnd)
    {
        const Skysphere & skysphere = *(Skysphere*)pStart->pExecData;
        const SkysphereSettings & settings = skysphere.getSettings();

        if(settings.pTexture != null || settings.isProcedural)
        {
            int32 iPass = int32(_pass);
            int32 iLightModel = int32(settings.model);
            const ShaderParams & params = (settings.pTexture != null) ? _paramsTexture[iPass][iLightModel] : _paramsProcedural[iPass][iLightModel];

            Core::Quaternionf rotation(Core::YAW, (settings.pTexture != null) ? settings.yaw : 0.0f);
            Core::Matrix4f rotView(_view);

            if(settings.isAtInfinity)
                rotView._Tx = rotView._Ty = rotView._Tz = 0.0;

            Core::Matrix4f world(rotation);
            Core::Matrix4f viewProj(_proj * rotView);
            Core::Matrix4f worldViewProj(viewProj * world);

            _pDevice->setState(params.state);
            _pDevice->setVertexFormat(params.pFormat);
            _pDevice->setVertexShader(params.pVShader);
            _pDevice->setPixelShader(params.pPShader);

            Core::Vector4f color;

            if(_pass == PASS_GLOW)
                color = settings.glow;
            else
                color = settings.color;

            color *= _lightSettings.getGlobalFade();

            params.pVConst->setConstantSafe(params.idWorldViewProj, worldViewProj);

            params.pPConst->setConstantSafe(params.idColor, color);
            params.pPConst->setConstantSafe(params.idSunColor, settings.intensity * Core::mul(settings.sunColor, color));
            params.pPConst->setConstantSafe(params.idSkyColor, Core::mul(settings.skyColor, color));

            params.pPConst->setConstantSafe(params.idFogColor, _fogSettings.getColor());
            params.pPConst->setConstantSafe(params.idFogOpacity, _fogSettings.getSkyOpacity());
            
            params.pPConst->setConstantSafe(params.idMainLightDir, _lightSettings.getDirection());

            if(params.idSampler != Gfx::UNDEFINED_SHADER_CONST)
                params.pPConst->setSampler2D(params.idSampler, LM_DEBUG_PTR_CAST<TextureMap>(settings.pTexture)->getResource());

            skysphere.sendData(_pDevice);
        }
        pStart++;
    }
}
//-----------------------------------------------------------------------------
int32 floorLog2(int32 n)
{
    if(n == 0)
        return 0;

    int32 pos = 0;

    if (n >= 1<<16) { n >>= 16; pos += 16; }
    if (n >= 1<< 8) { n >>=  8; pos +=  8; }
    if (n >= 1<< 4) { n >>=  4; pos +=  4; }
    if (n >= 1<< 2) { n >>=  2; pos +=  2; }
    if (n >= 1<< 1) {           pos +=  1; }

    return pos;
}

struct FaceVertex
{
    Core::Vector4f  position;
    Core::Vector3f  normal;
    byte            padding[4];
};

LM_STATIC_ASSERT(sizeof(FaceVertex) == 32);

static const int32 VERTEX_BUFFER_SIZE   = 4 * sizeof(FaceVertex);
static const int32 INDEX_BUFFER_SIZE    = 6 * sizeof(uint16);

void SkysphereRenderer::generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox)
{
    _fogSettings = InternalFogSettings(skybox.pScene->getFogSettings(), 10.0f, 1000.0f);
    _lightSettings = InternalMainLightSettings(skybox.pScene->getLightSettings());

    int32 mipCount = floorLog2(skybox.resolution);
    Core::List<Ptr<Gfx::ITexture2D> > renderTex(mipCount);
    Core::List<Ptr<Gfx::IRenderTargetView> > renderView(mipCount);

    for(int32 iFace=0; iFace < Assets::ECubeFace_COUNT; iFace++)
        skybox.bitmaps[iFace].resize(mipCount);

    LM_ASSERT(settings.isProcedural);

    Gfx::Texture2DDesc texDepthDesc;
    texDepthDesc.width = skybox.resolution;
    texDepthDesc.height = skybox.resolution;
    texDepthDesc.mipLevels = 1;
    texDepthDesc.format = Gfx::TF_D24S8;
    texDepthDesc.bind = Gfx::TB_DEPTH_STENCIL_SURFACE;
    texDepthDesc.multiSampleType = Gfx::MSAA_NONE;

    Gfx::DepthStencilViewDesc dsvDepthDesc;
    dsvDepthDesc.texture2D.mipSlice = 0;

    Ptr<Gfx::ITexture2D> pDepthTex = _pDevice->createTexture2D(texDepthDesc);
    Ptr<Gfx::IDepthStencilView> pDepthView = _pDevice->createDepthStencilView(pDepthTex, dsvDepthDesc);


    // Format brut

    Gfx::VertexFormatDesc  formatDesc;
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT4, Gfx::VAU_POSITIONT);
    formatDesc.addAttribut(0, Gfx::VAT_FLOAT3, Gfx::VAU_TEXTURE_COORD, 0); // texcoord

    Ptr<Gfx::IVertexFormat> pFormat = _pDevice->createVertexFormat(formatDesc, Ptr<Gfx::IVertexShader>(null));

    // Buffers

    Ptr<Gfx::IVertexBuffer> pVBuffer = _pDevice->createVertexBuffer(Gfx::VertexBufferDesc(Gfx::BUFFER_USAGE_DEFAULT, Gfx::BCA_ONLY_WRITE, VERTEX_BUFFER_SIZE));
    Ptr<Gfx::IIndexBuffer> pIBuffer = _pDevice->createIndexBuffer(Gfx::IndexBufferDesc(Gfx::BUFFER_USAGE_IMMUTABLE, Gfx::BCA_ONLY_WRITE, Gfx::INDEX_BUFFFER_FMT_16, INDEX_BUFFER_SIZE));

    uint16 * pIndices = reinterpret_cast<uint16*>(pIBuffer->lock(Gfx::LOCK_WRITE, 0, 0));

    pIndices[0] = 0; pIndices[1] = 1; pIndices[2] = 2;
    pIndices[3] = 0; pIndices[4] = 2; pIndices[5] = 3;

    pIBuffer->unlock();

    for(int32 iMip=0; iMip < mipCount; iMip++)
    {
        Gfx::Texture2DDesc texDesc;
        texDesc.width = texDesc.height = (skybox.resolution >> iMip);
        texDesc.mipLevels = 1;
        texDesc.format = Gfx::TF_A8R8G8B8;
        texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
        texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
        texDesc.bind = Gfx::TB_RENDER_TARGET;
        texDesc.autoGenMipmap = false;

        Gfx::RenderTargetViewDesc rtDesc;
        rtDesc.texture2D.mipSlice = 0;

        Ptr<Gfx::ITexture2D> pTexture = _pDevice->createTexture2D(texDesc);
        Ptr<Gfx::IRenderTargetView> pView = _pDevice->createRenderTargetView(pTexture, rtDesc);

        int32 iLightModel = int32(settings.model);
        ShaderParams & params = _paramsProcedural[PASS_LIGHTING][iLightModel];

        float fx1 = float(0) - 0.5f;
        float fx2 = float(texDesc.width) - 0.5f;
        float fy1 = float(0) - 0.5f;
        float fy2 = float(texDesc.height) - 0.5f;

        for(int32 iFace=0; iFace < Assets::ECubeFace_COUNT; iFace++)
        {
            Core::Matrix4f world;
            getCubeMapMatrix((Gfx::ECubeMapFace)iFace, world, Core::Vector3f(0.0f, 0.0f, 0.0f));

            FaceVertex * pVert = reinterpret_cast<FaceVertex*>(pVBuffer->lock(Gfx::LOCK_WRITE_DISCARD, 0, 0));

            pVert[0].position = Core::Vector4f(fx1, fy1, 1.0, 1.0); pVert[0].normal = world.apply3x3(Core::Vector3f(-1.0f,  1.0f, 1.0f).getNormalizedSafe());
            pVert[1].position = Core::Vector4f(fx2, fy1, 1.0, 1.0); pVert[1].normal = world.apply3x3(Core::Vector3f( 1.0f,  1.0f, 1.0f).getNormalizedSafe());
            pVert[2].position = Core::Vector4f(fx2, fy2, 1.0, 1.0); pVert[2].normal = world.apply3x3(Core::Vector3f( 1.0f, -1.0f, 1.0f).getNormalizedSafe());
            pVert[3].position = Core::Vector4f(fx1, fy2, 1.0, 1.0); pVert[3].normal = world.apply3x3(Core::Vector3f(-1.0f, -1.0f, 1.0f).getNormalizedSafe());

            pVBuffer->unlock();

            _pDevice->beginDraw();
            _pDevice->setRenderTarget(pView, null);
            _pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1.0, 0, Gfx::CLEAR_COLOR_BUFFER);

            _pDevice->setState(params.state);
            _pDevice->setPixelShader(params.pPShader);

            params.pPConst->setConstantSafe(params.idSunColor, settings.intensity * settings.sunColor);
            params.pPConst->setConstantSafe(params.idSkyColor, settings.skyColor);

            params.pPConst->setConstantSafe(params.idFogColor, _fogSettings.getColor());
            params.pPConst->setConstantSafe(params.idFogOpacity, _fogSettings.getSkyOpacity());
            
            params.pPConst->setConstantSafe(params.idMainLightDir, _lightSettings.getDirection());


            _pDevice->setVertexFormat(pFormat);
            _pDevice->setVertexShader(null);
            _pDevice->setVertexBuffer(0, pVBuffer, sizeof(FaceVertex), 0);
            _pDevice->setIndexBuffer(pIBuffer);
            _pDevice->drawIndexed(Gfx::PT_TRIANGLE_LIST, 4, 0, 2); // 4 vertices, 2 triangles
            _pDevice->endDraw();

            skybox.bitmaps[iFace][iMip] = _pDevice->toBitmap(pView);
            Core::saveTGA(skybox.bitmaps[iFace][iMip], L"Local/sky-" + Core::toString(iMip) + L"-" + Core::toString(iFace) + L".tga");
        }

        pView = null;
        pTexture = null;
    }
}
//-----------------------------------------------------------------------------
}
