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
#include <Core/TGA.h>
#include <Renderer/Common/Texture.h>
#include <Renderer/Common/Camera.h>
#include <Renderer/Common/Font.h>
#include <Renderer/Common/PictureBank.h>
#include <Renderer/Common/PackedVerticesFAT.h>
#include <Renderer/Common/PackedVerticesFLOAT16PN.h>
#include <Renderer/Common/Tools.h>
#include <Renderer/SM2/SM0/FreeFormRendererSM0.h>
#include <Renderer/SM2/SM0/SkysphereRendererSM0.h>
#include <Renderer/SM2/SM0/BillboardRendererSM0.h>
#include <Renderer/SM2/SM0/DecalRendererSM0.h>
#include <Renderer/SM2/SM0/GrassRendererSM0.h>
#include <Renderer/SM2/SM0/StandardMatRendererSM0.h>
#include <Renderer/SM2/SM0/WaterRendererSM0.h>
#include <Renderer/SM2/SM0/PostFXRendererSM0.h>
#include <Renderer/SM2/RendererSM2.h>
#include <Renderer/SM2/SceneSM2.h>
#include <Renderer/SM2/HUDSM2.h>
#include <Renderer/SM2/RenderToolsSM2.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
RendererSM2::RendererSM2(
        const Ptr<Gfx::IDriver> & pDriver,
        const RendererSettings & settings,
        void *  pHandle,
        const Ptr<Core::VirtualFileSystem> & pVFS,
        const String & shadersDir,
        int32 flags) :
    _pDriver(pDriver),
    _pVFS(pVFS),
    _shadersDir(shadersDir),
    _refCubeSize(256)
{
    _pResLib = Ptr<GPUResourceLib>(new GPUResourceLib());

    const Gfx::GraphicCapabilities & caps = _pDriver->getGraphicCaps(Gfx::DEFAULT_ADAPTER);

    _maxRenderSettings.textureLevel = TEXTURE_HIGH;
    _maxRenderSettings.shadowLevel = SHADOW_HIGH;
    _maxRenderSettings.reflecLevel = REFLEC_HIGH;
    _maxRenderSettings.msaaLevel = cvt(caps.maxMSAA);

    if(caps.psVersion >= Gfx::PS_V2_a)
    {
        if(caps.supportFLOAT16Tex && caps.supportFLOAT16Blend)
            _maxRenderSettings.shaderLevel = SHADER_HIGH;
        else
            _maxRenderSettings.shaderLevel = SHADER_MEDIUM;
        _maxRenderSettings.postFXOn = true;
        _maxRenderSettings.glowOn = true;
    }
    else if(caps.psVersion >= Gfx::PS_V2_0)
    {
        _maxRenderSettings.shaderLevel = SHADER_LOW;
        _maxRenderSettings.postFXOn = true;
        _maxRenderSettings.glowOn = true;
    }
    else
    {
        _maxRenderSettings.shaderLevel = SHADER_NONE;
        _maxRenderSettings.shadowLevel = SHADOW_NONE;
        _maxRenderSettings.reflecLevel = REFLEC_NONE;
        _maxRenderSettings.postFXOn = false;
        _maxRenderSettings.glowOn = false;
    }

    if(caps.psVersion >= Gfx::PS_V2_0 && caps.supportBlendingOps)
        _maxRenderSettings.refracLevel = REFRAC_HIGH;
    else
        _maxRenderSettings.refracLevel = REFRAC_NONE;

    if(caps.maxAnisotropy >= 8)
        _maxRenderSettings.filterLevel = FILTER_ANISO_8X;
    else if(caps.maxAnisotropy >= 4)
        _maxRenderSettings.filterLevel = FILTER_ANISO_4X;
    else if(caps.maxAnisotropy >= 2)
        _maxRenderSettings.filterLevel = FILTER_ANISO_2X;
    else
        _maxRenderSettings.filterLevel = FILTER_TRILINEAR;

    internalSetSettings(settings);

    Gfx::DisplayMode initialMode = _pDriver->getAdapterDisplayMode(Gfx::DEFAULT_ADAPTER);

    INF << L"Initial render settings {\n";
    dumpSettings(_renderSettings);
    INF << L"}\n";

    _scDesc.format = Gfx::DPF_X8R8G8B8;
    _scDesc.bufferCount = 2;
    _scDesc.handleWindow = pHandle;
    _scDesc.multiSamplingQuality = std::max(0, caps.maxMSAAQuality - 1);

    int32 deviceFlags = 0;

    if(flags & MULTITHREADED_FLAG) deviceFlags |= Gfx::MULTITHREADED_DEVICE_FLAG;

    _pDriver->createDevice(
            Gfx::DEFAULT_ADAPTER,
            (caps.vsVersion >= Gfx::VS_V1_1) ? Gfx::HARDWARE_PROCESSING : Gfx::SOFTWARE_PROCESSING,
            //Gfx::SOFTWARE_PROCESSING,
            _scDesc,
            _pSwapChain,
            _pDevice,
            deviceFlags);

    _totalVRAM = _pDevice->getTotalVideoMem();

    INF << L"Initial D3D VRAM     : " << Core::toStringByteSize(_pDevice->getTotalVideoMem()) << L"\n";

    filterResolutions(_pDriver->getDisplayFormats(Gfx::DEFAULT_ADAPTER), initialMode, _allResolutions, _preferredResolutions, true);

    createDefaultRenderView();
    INF << L"VRAM usage after buf : " << Core::toStringByteSize(_totalVRAM - _pDevice->getAvailableVideoMem()) << L"\n";
}
//-----------------------------------------------------------------------------
RendererSM2::~RendererSM2()
{
    _pScreenQuad = null;

    _pDefaultTextureWhite = null;
    _pDefaultTextureBlack = null;
    _pDefaultTextureBump = null;
    _pDebugTextureMip = null;

    _pLayerMapGen = null;

    _executers.clear();

    _pStdMatRenderer = null;
    _pCustomMatRenderer = null;
    _pDecalRenderer = null;
    _pBillRenderer = null;
    _pSkyRenderer = null;
    _pSkyBoxRenderer = null;
    _pGlowRenderer = null;
    _pGrassRenderer = null;
    _pWaterRenderer = null;
    _pBoxRenderer = null;
    _pSplineRenderer = null;
    _pFreeFormRenderer = null;
    _pShadowRenderer = null;
    _pPostFXRenderer = null;
    _pDebugRenderer = null;
    _pHUDRenderer = null;

    _pReflexionView = null;
    _pDefaultView = null;

    LM_ASSERT(_pVertexFormat.getCount() <= 1);
    _pVertexFormat = null;

    LM_ASSERT(_pShaderLib.getCount() <= 1);
    _pShaderLib = null;

    _pDevice = null;
    _pDriver = null;
    _pVFS = null;

    LM_ASSERT(_pResLib.getCount() <= 1);
    _pResLib = null;
}
//-----------------------------------------------------------------------------
bool RendererSM2::initialise()
{
    INF << L"Initialising RendererSM2...\n";

    // Default texture

    uint32 pixelWhite = 0xFFFFFFFF;
    uint32 pixelBlack = 0x00000000;
    uint32 pixelBump  = 0xFF8080FF;

    Assets::TextureImage imgWhite(1, 1, Assets::TEX_FORMAT_ARGB8, &pixelWhite, sizeof(pixelWhite));
    Assets::TextureImage imgBlack(1, 1, Assets::TEX_FORMAT_ARGB8, &pixelBlack, sizeof(pixelBlack));
    Assets::TextureImage imgBump(1, 1, Assets::TEX_FORMAT_ARGB8, &pixelBump, sizeof(pixelBlack));

    Core::List<Assets::TextureImage> imagesWhite;
    Core::List<Assets::TextureImage> imagesBlack;
    Core::List<Assets::TextureImage> imagesBump;

    imagesWhite.push_back(imgWhite);
    imagesBlack.push_back(imgBlack);
    imagesBump.push_back(imgBump);

    Ptr<Assets::Texture> pTexDataWhite(new Assets::Texture(imagesWhite));
    Ptr<Assets::Texture> pTexDataBlack(new Assets::Texture(imagesBlack));
    Ptr<Assets::Texture> pTexDataBump(new Assets::Texture(imagesBump));

    _pDefaultTextureWhite = LM_DEBUG_PTR_CAST<TextureMap>(uploadTexture(pTexDataWhite));
    _pDefaultTextureBlack = LM_DEBUG_PTR_CAST<TextureMap>(uploadTexture(pTexDataBlack));
    _pDefaultTextureBump  = LM_DEBUG_PTR_CAST<TextureMap>(uploadTexture(pTexDataBump));

#ifdef LM_DEVMODE
    createDebugMipTexture();
#endif

    // Composants

    _pShaderLib = Ptr<ShaderLib>(new ShaderLib(_pVFS, _shadersDir + L"/src/", _shadersDir + L"/bin/", _pDevice));
    _pScreenQuad = Ptr<ScreenQuad>(new ScreenQuad(_pDevice, _pShaderLib, _pResLib));
    _pVertexFormat = Ptr<PackedVerticesFormat>(new PackedVerticesFormatFLOAT16PN(_pResLib));
    _pVertexFormat = Ptr<PackedVerticesFormat>(new PackedVerticesFormatFAT(_pResLib));

    if(_pVertexFormat->initialise(_pDevice))
    {
        //INF << L"Picked vertex format : FLOAT16\n";
        INF << L"Picked vertex format : FAT\n";
    }
    else
    {
        ERR << L"No suitable vertex format found!\n";
        throw RendererException(L"No suitable vertex format found!");
    }

    if(_renderSettings.shaderLevel == SHADER_NONE)
    {
        _pStdMatRenderer = Ptr<IStandardMatRenderer>(new StandardMatRendererSM0(_pDevice, _pShaderLib, _pVertexFormat, _pDefaultTextureWhite, _pDefaultTextureBlack, _renderSettings));
        _pSkyRenderer = Ptr<ISkysphereRenderer>(new SkysphereRendererSM0(_pDevice, _pShaderLib));
        _pBillRenderer = Ptr<IBillboardRenderer>(new BillboardRendererSM0(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pDecalRenderer = Ptr<IDecalRenderer>(new DecalRendererSM0(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pGlowRenderer = Ptr<IGlowRenderer>(new GlowRendererDummy());
        _pGrassRenderer = Ptr<IGrassRenderer>(new GrassRendererSM0(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pDebugRenderer = Ptr<IDebugRenderer>(new DebugRendererDummy());
        _pShadowRenderer = Ptr<IShadowRenderer>(new ShadowRendererDummy());
        _pWaterRenderer = Ptr<IWaterRenderer>(new WaterRendererSM0(_pDevice, _pShaderLib, _pDefaultTextureWhite, _pResLib, _renderSettings));
        _pFreeFormRenderer = Ptr<IFreeFormRenderer>(new FreeFormRendererSM0(_pDevice, _pShaderLib, _pResLib, _pDefaultTextureWhite, _renderSettings));
        _pPostFXRenderer = Ptr<IPostFXRenderer>(new PostFXRendererSM0(_pDevice, _pScreenQuad));
    }
    else
    {
        _pStdMatRenderer = Ptr<IStandardMatRenderer>(new StandardMatRenderer(_pDevice, _pShaderLib, _pVertexFormat, _pDefaultTextureWhite, _pDefaultTextureBlack, _pDebugTextureMip, _renderSettings));
        _pCustomMatRenderer = Ptr<ICustomMatRenderer>(new CustomMatRenderer(_pDevice, _pShaderLib, _pVertexFormat, _pDefaultTextureWhite, _pDefaultTextureBump, _renderSettings));
        _pSkyRenderer = Ptr<ISkysphereRenderer>(new SkysphereRenderer(_pDevice, _pShaderLib));
        _pSkyBoxRenderer = Ptr<ISkyboxRenderer>(new SkyboxRenderer(_pDevice, _pShaderLib, _pResLib));
        _pBillRenderer = Ptr<IBillboardRenderer>(new BillboardRenderer(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pDecalRenderer = Ptr<IDecalRenderer>(new DecalRenderer(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pGlowRenderer = Ptr<IGlowRenderer>(new GlowRenderer(_pDevice, _pShaderLib, _pScreenQuad));
        _pGrassRenderer = Ptr<IGrassRenderer>(new GrassRenderer(_pDevice, _pShaderLib, _pResLib, _renderSettings));
        _pDebugRenderer = Ptr<IDebugRenderer>(new DebugRenderer(_pDevice, _pShaderLib, _pScreenQuad));
        _pShadowRenderer = Ptr<IShadowRenderer>(new ShadowRenderer(_pDevice, _pShaderLib, _pScreenQuad, _pResLib, _renderSettings.shadowLevel));
        _pWaterRenderer = Ptr<IWaterRenderer>(new WaterRenderer(_pDevice, _pShaderLib, _pDefaultTextureWhite, _pResLib, _renderSettings));
        _pFreeFormRenderer = Ptr<IFreeFormRenderer>(new FreeFormRenderer(_pDevice, _pShaderLib, _pResLib, _pDefaultTextureWhite, _renderSettings));
        _pPostFXRenderer = Ptr<IPostFXRenderer>(new PostFXRenderer(_pDevice, _pShaderLib, _pScreenQuad, _renderSettings));
    }

    _pHUDRenderer = Ptr<HUDRenderer>(new HUDRenderer(_pDevice, _pShaderLib, _pScreenQuad, _pDefaultTextureWhite));
    _pSplineRenderer = Ptr<ISplineRenderer>(new SplineRenderer(_pDevice, _pShaderLib, _pResLib));
    _pBoxRenderer = Ptr<IBoxRenderer>(new BoxRenderer(_pDevice, _pShaderLib, _pResLib));

    _executers.clear();

    _executers.push_back(_pStdMatRenderer);
    _executers.push_back(_pCustomMatRenderer);
    _executers.push_back(_pDecalRenderer);
    _executers.push_back(_pBillRenderer);
    _executers.push_back(_pFreeFormRenderer);
    _executers.push_back(_pSplineRenderer);
    _executers.push_back(_pBoxRenderer);
    _executers.push_back(_pSkyRenderer);
    _executers.push_back(_pSkyBoxRenderer);
    _executers.push_back(_pGrassRenderer);
    _executers.push_back(_pWaterRenderer);

    bool initOk = initReflexions();
    initOk = _pScreenQuad->initialise() && initOk;
    initOk = _pBoxRenderer->initialise() && initOk;
    initOk = _pSplineRenderer->initialise() && initOk;
    initOk = _pGlowRenderer->initialise() && initOk;
    initOk = _pStdMatRenderer->initialise() && initOk;
    initOk = _pCustomMatRenderer->initialise() && initOk;
    initOk = _pDecalRenderer->initialise() && initOk;
    initOk = _pBillRenderer->initialise() && initOk;
    initOk = _pSkyRenderer->initialise() && initOk;
    initOk = _pSkyBoxRenderer->initialise() && initOk;
    initOk = _pWaterRenderer->initialise() && initOk;
    initOk = _pGrassRenderer->initialise() && initOk;
    initOk = _pFreeFormRenderer->initialise() && initOk;
    initOk = _pShadowRenderer->initialise() && initOk;
    initOk = _pPostFXRenderer->initialise() && initOk;
    initOk = _pDebugRenderer->initialise() && initOk;
    initOk = _pHUDRenderer->initialise() && initOk;

    return initOk;
}
//-----------------------------------------------------------------------------
Ptr<IMesh> RendererSM2::uploadMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber)
{
    Core::ScopedLock lock(_mutex);

    Ptr<Assets::VerticeSet> pVertices = pMesh->getLOD(Assets::LOD_HIGH);

    const Assets::Material & amat = pMesh->getMaterial();
    Material mat(amat);

    mat.pDiffuseMap = texGrabber.getTexture(amat.diffuseTexName);
    mat.pSpecularMap = texGrabber.getTexture(amat.specularTexName);
    mat.pNormalMap = texGrabber.getTexture(amat.normalTexName);
    mat.pCustomMap0 = texGrabber.getTexture(amat.texture0Name);
    mat.pCustomMap1 = texGrabber.getTexture(amat.texture1Name);
    mat.pCustomMap2 = texGrabber.getTexture(amat.texture2Name);

    Ptr<IMesh> result(new Mesh(pVertices, _pVertexFormat->packStaticVerts(_pDevice, pVertices), mat));

    return result;
}
//-----------------------------------------------------------------------------
Ptr<ISkinMesh> RendererSM2::uploadSkinMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber)
{
    Core::ScopedLock lock(_mutex);

    Ptr<Assets::VerticeSet> pVertices = pMesh->getLOD(Assets::LOD_HIGH);

    const Assets::Material & amat = pMesh->getMaterial();
    Material mat(amat);

    mat.pDiffuseMap = texGrabber.getTexture(amat.diffuseTexName);
    mat.pSpecularMap = texGrabber.getTexture(amat.specularTexName);
    mat.pNormalMap = texGrabber.getTexture(amat.normalTexName);
    mat.pCustomMap0 = texGrabber.getTexture(amat.texture0Name);
    mat.pCustomMap1 = texGrabber.getTexture(amat.texture1Name);
    mat.pCustomMap2 = texGrabber.getTexture(amat.texture2Name);

    Ptr<SkinMesh> result(new SkinMesh(pVertices, _pVertexFormat->packSkinnedVerts(_pDevice, pVertices), mat));
    result->setBindShapeMatrix(pMesh->getBindShapeMatrix());

    return result;
}
//-----------------------------------------------------------------------------
Ptr<IScene> RendererSM2::createScene()
{
    Core::ScopedLock lock(_mutex);

    return Ptr<IScene>(new SceneSM2(_pDevice, _pResLib));
}
//-----------------------------------------------------------------------------
Ptr<IHUD> RendererSM2::createHUD()
{
    Core::ScopedLock lock(_mutex);

    return Ptr<IHUD>(new HUDSM2());
}
//-----------------------------------------------------------------------------
Ptr<IFont> RendererSM2::createFont(const String & name, int32 height)
{
    Core::ScopedLock lock(_mutex);

    return Ptr<IFont>(new Font(_pDevice, name, height, _pResLib));
}
//-----------------------------------------------------------------------------
Ptr<IPictureBank> RendererSM2::uploadPictureBank(const Ptr<Assets::PictureBank> & pPicBank)
{
    Core::ScopedLock lock(_mutex);

    return Ptr<IPictureBank>(new PictureBank(*this, pPicBank));
}
//-----------------------------------------------------------------------------
Ptr<IRenderView> RendererSM2::getDefaultView() const
{
    return _pDefaultView;
}
//-----------------------------------------------------------------------------
Ptr<IRenderView> RendererSM2::createView(int32 width, int32 height, int32 bufferCount, void * pHandle)
{
    Core::ScopedLock lock(_mutex);

    // Si on crée des view c'est qu'on est en fenêtré et donc que le default view n'est pas utile
    _pDefaultView = null;

    Gfx::SwapChainDesc sc(_scDesc);
    sc.width = width;
    sc.height = height;
    sc.bufferCount = bufferCount;
    sc.handleWindow = pHandle;

    return Ptr<IRenderView>(new RenderView(_pDevice, sc, null, _pResLib, _renderSettings));
}
//-----------------------------------------------------------------------------
void RendererSM2::renderScenes(const Ptr<IRenderView> & pView, const Core::List<Ptr<IScene> > & scenes, const Ptr<ICamera> & pCamera)
{
    Core::ScopedLock lock(_mutex);

    LM_START_PERF_LOG(L"render");

    Core::List<Ptr<SceneSM2> > scenesSM2;
    scenesSM2.resize(scenes.size());

    for(int32 ii=0; ii < scenes.size(); ii++)
    {
        scenesSM2[ii] = LM_DEBUG_PTR_CAST<SceneSM2>(scenes[ii]);
        scenesSM2[ii]->cleanup();
    }

    Ptr<RenderView> pViewSM2 = LM_DEBUG_PTR_CAST<RenderView>(pView);
    pViewSM2->bind(RT_LIGHT_BUFFER);

    _pDevice->beginDraw();
    internalRenderScene(pViewSM2, scenesSM2, pCamera, pView->getWidth(), pView->getHeight(), 0);
    _pDevice->endDraw();

    scenesSM2[0]->globalReflexionsUpdated();

    LM_END_PERF_LOG(L"render");
}
//-----------------------------------------------------------------------------
void RendererSM2::renderScene(const Ptr<IRenderView> & pView, const Ptr<IScene> & pScene, const Ptr<ICamera> & pCamera)
{
    Core::List<Ptr<IScene> > scenes;
    scenes.push_back(pScene);
    renderScenes(pView, scenes, pCamera);
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> RendererSM2::generateLightMap(const LightMapInfos & lm, const Ptr<IMeshInstance> & pMesh)
{
    Core::ScopedLock lock(_mutex);

    Core::List<Ptr<SceneSM2> > scenes;
    scenes.push_back(LM_DEBUG_PTR_CAST<SceneSM2>(lm.pScene));

    Gfx::SwapChainDesc sc(_scDesc);
    sc.width = lm.width;
    sc.height = lm.height;
    sc.bufferCount = 2;

    _pDevice->beginDraw();

    Ptr<ICamera> pCamera(new PerspectiveCamera());
    Ptr<RenderView> pView(new RenderView(_pDevice, sc, null, _pResLib, _renderSettings));

    Gfx::Texture2DDesc texDesc;
    texDesc.width = lm.width;
    texDesc.height = lm.height;
    texDesc.mipLevels = 1;
    texDesc.format = Gfx::TF_A8R8G8B8;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = false;

    Gfx::RenderTargetViewDesc rtDesc;
    rtDesc.texture2D.mipSlice = 0;

    Ptr<Gfx::ITexture2D> pTexLight = _pDevice->createTexture2D(texDesc);
    Ptr<Gfx::IRenderTargetView> pViewLight = _pDevice->createRenderTargetView(pTexLight, rtDesc);

    pView->changeLightBuffer(pViewLight);

    RenderContext mainContext(LIGHTMAP_GENERATION_CTX, pView, scenes, pCamera, pMesh, lm.width, lm.height, (float)scenes[0]->getTime());
    
    // Rendu de la static shadow map
    {
        RenderContext context(STATIC_SHADOW_CTX, mainContext, scenes);

        _pShadowRenderer->startStaticShadowMapRendering();

        _pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        renderContext(context, PASS_STATIC_SHADOW);
        _pShadowRenderer->endStaticShadowMapRendering();
        _pStdMatRenderer->setStaticShadowMap(_pShadowRenderer->getStaticShadowMap());
    }

    // Rendu du mesh
    {
        pView->begin();
        pView->bind(RT_LIGHT_BUFFER);
        prepareMeshes(scenes[0], mainContext, mainContext.getMeshes(), mainContext.getSkinMeshes(), 0);

        //_pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
        _pDevice->clearCurrentRenderTargetView(Core::Vector4f(0.5f, 0.5f, 0.5f, 0.0f));
        renderContext(mainContext, PASS_LIGHTMAP_GENERATION);
        pView->end();
    }

    _pDevice->endDraw();

    Ptr<Core::Bitmap> pBitmapLight(_pDevice->toBitmap(pViewLight));

    // Pour unbinder pView
    pView = null;
    pViewLight = null;
    pTexLight = null;

    for(int32 ii=0; ii < 40; ii++)
        pBitmapLight = borderDiffuse(pBitmapLight);

    static int32 cpt = 0;
    Core::saveTGA(pBitmapLight, L"Local/lm" + Core::toString(cpt++, 4) + L".tga");

    return pBitmapLight;
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> RendererSM2::generateOcclusionMap(const OcclusionMapInfos & om, const Ptr<Assets::VerticeSet> & pVSet)
{
    Core::ScopedLock lock(_mutex);
    if(_pOcclusionMapGen == null)
    {
        _pOcclusionMapGen = Ptr<OcclusionMapGenerator>(new OcclusionMapGenerator(_pDevice, _pShaderLib, _pVertexFormat, _pScreenQuad));
        _pOcclusionMapGen->initialise();
    }
    return _pOcclusionMapGen->generateOcclusionMap(om, pVSet);
}
//-----------------------------------------------------------------------------
void RendererSM2::generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox)
{
    _pSkyRenderer->generateSkyBox(settings, skybox);
}
//-----------------------------------------------------------------------------
void RendererSM2::renderContext(const RenderContext & context, ERenderPass pass)
{
    _stats << Renderer::toString(pass).slice(5) << L"{";
    LM_START_PERF_LOG(L"init");

    Core::List<Command> commands;

    initCommands(_executers, commands, context, pass);
   
    int32 counts[ECommandMode_COUNT];
    
    for(int32 ii=0; ii < ECommandMode_COUNT; ii++)
        counts[ii] = 0;
    
    for(int32 ii=0; ii < commands.size(); ii++)
        counts[commands[ii].mode] ++;
    
    bool first = true;

    for(int32 ii=0; ii < ECommandMode_COUNT; ii++)
    {
        String str;
        if(counts[ii] > 0)
        {
            if(!first)
                str << L" ";

            first = false;

            str << Renderer::toString(ECommandMode(ii)).slice(4) << L":" << Core::toStringPadSpaces(counts[ii], 4);
            _stats << str;
        }
    }

    _stats << L"} ";

    LM_END_PERF_LOG(L"init");
    execCommands(_executers, commands);
}
//-----------------------------------------------------------------------------
int32 RendererSM2::getVRAMUsage() const
{
    return int32(_totalVRAM - _pDevice->getAvailableVideoMem());
}
//-----------------------------------------------------------------------------
void RendererSM2::internalRenderScene(const Ptr<IRenderTarget> & pView, const Core::List<Ptr<SceneSM2> > & scenes, 
                                      const Ptr<ICamera> & pCamera, int32 width, int32 height, int32 flags)
{
    _stats.clear();
    _pDevice->startStats();

    const Ptr<SceneSM2> & pScene = scenes[0];
    int32 flagsCtx = 0;

    if(_renderSettings.shadowLevel >= SHADOW_LOW)
        flagsCtx |= CTX_DYNAMIC_SHADOW_ON;

    _pStdMatRenderer->setDebugMode(_debugSettings);

    RenderContext mainContext(pView, scenes, pCamera, width, height, (float)pScene->getTime(), flagsCtx);
    
    // Rendu de la static shadow map

    if(_debugSettings.renderMode == RDR_HQ_PREVIEW)
    {
        LM_START_PERF_LOG(L"SHADOW");
        RenderContext context(STATIC_SHADOW_CTX, mainContext, scenes);

        _pShadowRenderer->startStaticShadowMapRendering();

        _pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        renderContext(context, PASS_STATIC_SHADOW);
        _pShadowRenderer->endStaticShadowMapRendering();
        _pStdMatRenderer->setStaticShadowMap(_pShadowRenderer->getStaticShadowMap());
        LM_END_PERF_LOG(L"SHADOW");
    }


    // Rendu de la dynamic shadow map

    if((_renderSettings.shadowLevel >= SHADOW_LOW) && (_renderSettings.shaderLevel >= SHADER_LOW) && mainContext.isDynamicShadowMapNeeded())
    {
        LM_START_PERF_LOG(L"shadow");
        for(int32 iSplit=0; iSplit < 2; iSplit++)
        {
            RenderContext context(DYNAMIC_SHADOW_CTX, mainContext, scenes, iSplit);

            _pShadowRenderer->startDynamicShadowMapRendering(iSplit);

            _pDevice->clearCurrentRenderTargetView(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            renderContext(context, PASS_DYNAMIC_SHADOW);
            _pShadowRenderer->endDynamicShadowMapRendering();
            _pStdMatRenderer->setDynamicShadowMap(iSplit, _pShadowRenderer->getDynamicShadowMap(iSplit));
        }
        LM_END_PERF_LOG(L"shadow");
    }

    pView->begin();

    // Rendu de la scène -> réflexions pour l'eau
    if(mainContext.isReflectionMapNeeded())
    {
        LM_START_PERF_LOG(L"reflect");
        RenderContext context(pView, scenes, pCamera, width, height, (float)pScene->getTime(), flagsCtx | WATER_CONTEXT);

        prepareMeshes(pScene, context, context.getMeshes(), context.getSkinMeshes(), flags);
        pView->bind(RT_REFLECTION_BUFFER);

        _pDevice->clearCurrentRenderTargetView(context.getFog().getColor());
        renderContext(context, PASS_REFLECTION);
        LM_END_PERF_LOG(L"reflect");
    }

    // Rendu de la scène -> normal
    {
        RenderContext & context = mainContext;

        LM_START_PERF_LOG(L"prep");
        prepareMeshes(pScene, context, context.getMeshes(), context.getSkinMeshes(), flags);
        LM_END_PERF_LOG(L"prep");

        LM_START_PERF_LOG(L"light");
        pView->bind(RT_LIGHT_BUFFER);

        _pDevice->clearCurrentRenderTargetView(context.getFog().getColor());
        renderContext(context, PASS_LIGHTING);
        LM_END_PERF_LOG(L"light");

        if(_renderSettings.shaderLevel >= SHADER_LOW && _renderSettings.glowOn)
        {
            LM_START_PERF_LOG(L"glow");
            _pGlowRenderer->startGlowPass(context);
            renderContext(context, PASS_GLOW);
            _pGlowRenderer->endGlowPass();
            LM_END_PERF_LOG(L"glow");
        }
        
        LM_START_PERF_LOG(L"postfx");
        _pPostFXRenderer->postFX(pCamera->getPostFX(), context);
        LM_END_PERF_LOG(L"postfx");

        _stats << L"\n" << context.getStats();
    }

    pView->end();
    _pDevice->endStats();
    _stats << pView->getStats();

    const Gfx::GfxStats & stats = _pDevice->getStats();

    _stats << L"VtxDcl:" << Core::toStringPadSpaces(stats.setVertexDeclCount, 3)
           << L" VS:" << Core::toStringPadSpaces(stats.vsSwitchCount, 3) 
           << L" PS:" << Core::toStringPadSpaces(stats.psSwitchCount, 3) 
           << L" VSC:" << Core::toStringPadSpaces(stats.setVSConstCount, 3) 
           << L" PSC:" << Core::toStringPadSpaces(stats.setPSConstCount, 3) 
           << L" tris:" << Core::toString(float(stats.triCount) / 1000.0f, 1) << L"k";

    switch(_debugSettings.renderMode)
    {
    case RDR_DEBUG_SHADOW_MAP: 
        _pDebugRenderer->renderTexture(mainContext, _pShadowRenderer->getStaticShadowMap(), 20, 20, 128, 128);
        if((_renderSettings.shadowLevel >= SHADOW_LOW) && mainContext.isDynamicShadowMapNeeded())
        {
            _pDebugRenderer->renderTexture(mainContext, _pShadowRenderer->getDynamicShadowMap(0), 20, 20, 128, 128);
            _pDebugRenderer->renderTexture(mainContext, _pShadowRenderer->getDynamicShadowMap(1), 168, 20, 128, 128);
        }
        break;
    }
}
//-----------------------------------------------------------------------------
Ptr<Core::Bitmap> RendererSM2::generateLayerMap(const LayerMapInfos & lm, const Ptr<IMeshInstance> & pMesh)
{
    Core::ScopedLock lock(_mutex);

    if(_pLayerMapGen == null)
    {
        _pLayerMapGen = Ptr<LayerMapGenerator>(new LayerMapGenerator(_pDevice, _pShaderLib, _pVertexFormat, _pDefaultTextureBlack, _pDefaultTextureBump));
        _pLayerMapGen->initialise();
    }

    return _pLayerMapGen->generateFromModel(lm, pMesh);
}
//-----------------------------------------------------------------------------
Ptr<ITextureMap> RendererSM2::uploadTexture(const Ptr<Assets::Texture> & pTexture)
{
    Core::ScopedLock lock(_mutex);

    return Ptr<ITextureMap>(new TextureMap(pTexture, _pDevice, _pResLib, _renderSettings.textureLevel));
}
//-----------------------------------------------------------------------------
Ptr<ICubeTextureMap> RendererSM2::uploadTexture(const Ptr<Assets::CubeTexture> & pTexture)
{
    Core::ScopedLock lock(_mutex);

    return Ptr<ICubeTextureMap>(new CubeTextureMap(pTexture, _pDevice, _pResLib, _renderSettings.textureLevel));
}
//-----------------------------------------------------------------------------
bool RendererSM2::initReflexions()
{
    bool result = true;

    try
    {
        Gfx::SwapChainDesc sc(_scDesc);
        sc.width = _refCubeSize;
        sc.height = _refCubeSize;
        sc.bufferCount = 1;
        sc.handleWindow = NULL;
        sc.multiSamplingType = Gfx::MSAA_NONE;

        _pReflexionView = Ptr<RenderView>(new RenderView(_pDevice, sc, null, _pResLib, _renderSettings));
    }
    catch(Core::Exception & exception)
    {
        ERR << L"Error initialising reflexions in Renderer : " << exception.getMessage() << L"\n";
        ERR << exception.getCallStack();
        result = false;
    }

    return result;
}
//-----------------------------------------------------------------------------
struct PointLightInfluenceSorter
{
    bool operator () (const PointLightInfluence & i1, const PointLightInfluence & i2)
    {
        return i1.coeff > i2.coeff;
    };
};
//-----------------------------------------------------------------------------
void RendererSM2::prepareMeshes(
        const Ptr<SceneSM2> & pScene,
        const RenderContext & context,
        const Core::List<MeshInstanceSM2 *> & meshes,
        const Core::List<SkinMeshInstanceSM2 *> & skinMeshes,
        int32 flags)
{
    Core::List<PointLight *> lights;
    pScene->getPointLights(lights, context.getFrustum());

    for(Core::List<MeshInstanceSM2 *>::const_iterator ipMesh = meshes.begin(); 
        ipMesh != meshes.end(); 
        ++ipMesh)
    {
        prepareMesh(pScene, lights, context, **ipMesh, flags);
    }

    for(Core::List<SkinMeshInstanceSM2 *>::const_iterator ipSkinMesh = skinMeshes.begin();
        ipSkinMesh != skinMeshes.end();
        ++ipSkinMesh)
    {
        prepareMesh(pScene, lights, context, **ipSkinMesh, flags);
    }
}
//-----------------------------------------------------------------------------
void RendererSM2::prepareMesh(const Ptr<SceneSM2> & pScene,
                              Core::List<PointLight *> lights,
                              const RenderContext & context,
                              IRenderable & renderable,
                              int32 flags)
{
    const Material & mat = renderable.getRenderableMaterial();
    MaterialData & data = renderable.getMatData();
    LightingData & lighting = renderable.getLightingData();

    Core::Vector3f position(renderable.getPosition());
    const Core::AABoxf & box = renderable.getAABoundingBox();

    // Lumières
    lighting.pointLights.clear();
    
    for(Core::List<PointLight *>::const_iterator ipLight = lights.begin(); ipLight != lights.end(); ++ipLight)
    {
        PointLightInfluence influence((*ipLight)->getInfluence(box));
        if(influence.coeff > 0.0)
            lighting.pointLights.push_back(influence);
    }

    if(lighting.pointLights.size() > MAX_POINT_LIGHTS_COUNT)
    {
        PointLightInfluenceSorter sorter;
        std::sort(lighting.pointLights.begin(), lighting.pointLights.end(), sorter);
        lighting.pointLights.resize(MAX_POINT_LIGHTS_COUNT);
    }

    // Réflexions
    if(!mat.reflexions.isZero(0.0) && !((flags & IN_REFLEXION_UPDATE) != 0))
    {
        bool mustUpdate = pScene->mustUpdateReflexions();

        if(data.pReflexionsCube == null)
        {
            createRenderCube(data.pReflexionsCube, data.pRenderTarget, _refCubeSize);
            mustUpdate = true;
        }

        Ptr<ICamera>  pCubeCamera(new PerspectiveCamera(f_PI_DIV_2, 1.0, 1.0, 10000.0));
        Core::Vector3f position(renderable.getPosition());

        if(mustUpdate && !data.isHidden && !data.isUpdatingReflexion)
        {
            data.isHidden = true;
            data.isUpdatingReflexion = true;

            for(int32 face=0; face < Gfx::ECubeMapFace_COUNT; face++)
            {
                Core::Matrix4f matrix;
                Renderer::getCubeMapMatrix((Gfx::ECubeMapFace)face, matrix, position);
                pCubeCamera->setWorldMatrix(matrix);

                _pReflexionView->changeLightBuffer(data.pRenderTarget[face]);

                Core::List<Ptr<SceneSM2> > scenes;
                scenes.push_back(pScene);
                internalRenderScene(_pReflexionView, scenes, pCubeCamera, _refCubeSize, _refCubeSize, flags | IN_REFLEXION_UPDATE);
            }

            data.isUpdatingReflexion = false;
            data.isHidden = false;
        }
    }
}
//-----------------------------------------------------------------------------
void RendererSM2::createRenderCube(Ptr<Gfx::ITextureCube> & pCubeTex, Ptr<Gfx::IRenderTargetView> * ppViews, int32 size)
{
    Gfx::TextureCubeDesc texDesc;
    texDesc.edgeLength = size;
    texDesc.mipLevels = 0;
    texDesc.format = Gfx::TF_A8R8G8B8;
    texDesc.usage = Gfx::BUFFER_USAGE_DEFAULT;
    texDesc.cpuAccess = Gfx::BCA_NO_ACCESS;;
    texDesc.bind = Gfx::TB_RENDER_TARGET;
    texDesc.autoGenMipmap = true;

    pCubeTex = _pDevice->createTextureCube(texDesc);

    Gfx::RenderTargetViewDesc targetDesc;
    targetDesc.textureCube.mipSlice = 0;

    for(int32 face=0; face < Gfx::ECubeMapFace_COUNT; face++)
    {
        targetDesc.textureCube.cubeFace = (Gfx::ECubeMapFace)face;
        ppViews[face] = _pDevice->createRenderTargetView(pCubeTex, targetDesc);
    }
}
//-----------------------------------------------------------------------------
Ptr<ITextureMap> RendererSM2::getDefaultTexture() const
{
    return _pDefaultTextureWhite;
}
//-----------------------------------------------------------------------------
void RendererSM2::setRenderSettings(const RendererSettings & settings)
{
    Core::ScopedLock lock(_mutex);

    RendererSettings rs = _renderSettings;
    internalSetSettings(settings);
    if(rs != _renderSettings)
    {
        INF << L"Render settings changed to {\n";
        dumpSettings(_renderSettings);
        INF << L"}\n";

        onDeviceLost();
        _pResLib->onTextureLevelChange(_renderSettings.textureLevel);
        _pResLib->onFullscreenChange(_renderSettings.fullscreen);
        onDeviceReset();
        initialise();
    }
}
//-----------------------------------------------------------------------------
const RendererSettings & RendererSM2::getRenderSettings() const
{
    Core::ScopedLock lock(_mutex);
    return _renderSettings;
}
//-----------------------------------------------------------------------------
const RendererSettings & RendererSM2::getMaxRenderSettings() const
{
    return _maxRenderSettings;
}
//-----------------------------------------------------------------------------
void RendererSM2::internalSetSettings(const RendererSettings & settings)
{
    _renderSettings.shaderLevel = std::min(settings.shaderLevel, _maxRenderSettings.shaderLevel);
    _renderSettings.filterLevel = std::min(settings.filterLevel, _maxRenderSettings.filterLevel);
    _renderSettings.textureLevel = std::min(settings.textureLevel, _maxRenderSettings.textureLevel);
    _renderSettings.shadowLevel = std::min(settings.shadowLevel, _maxRenderSettings.shadowLevel);
    _renderSettings.reflecLevel = std::min(settings.reflecLevel, _maxRenderSettings.reflecLevel);
    _renderSettings.refracLevel = std::min(settings.refracLevel, _maxRenderSettings.refracLevel);
    _renderSettings.msaaLevel = std::min(settings.msaaLevel, _maxRenderSettings.msaaLevel);
    _renderSettings.fullscreen = settings.fullscreen;
    _renderSettings.vsync = settings.vsync;
    _renderSettings.glowOn = settings.glowOn;
    _renderSettings.postFXOn = settings.postFXOn;

    Gfx::DisplayMode dm = bestFit(_pDriver, Gfx::DEFAULT_ADAPTER, settings.width, settings.height, Gfx::DPF_X8R8G8B8);
    _renderSettings.width = dm.width;
    _renderSettings.height = dm.height;

    _scDesc.multiSamplingType = cvt(_renderSettings.msaaLevel);
    _scDesc.screenMode = _renderSettings.fullscreen ? Gfx::FULLSCREEN_MODE : Gfx::WINDOWED_MODE;
    _scDesc.vsync = _renderSettings.vsync;
    _scDesc.width = _renderSettings.width;
    _scDesc.height = _renderSettings.height;
}
//-----------------------------------------------------------------------------
void RendererSM2::setDebugSettings(const DebugSettings & settings)
{
    Core::ScopedLock lock(_mutex);
    _debugSettings = settings;
}
//-----------------------------------------------------------------------------
const DebugSettings & RendererSM2::getDebugSettings() const
{
    Core::ScopedLock lock(_mutex);
    return _debugSettings;
}
//-----------------------------------------------------------------------------
String RendererSM2::getRenderStats() const
{
    Core::ScopedLock lock(_mutex);

    return _stats;
}
//-----------------------------------------------------------------------------
void RendererSM2::renderHUD(const Ptr<IRenderView> & pView, const Ptr<IHUD> & pHUD)
{
    Core::ScopedLock lock(_mutex);

    Ptr<RenderView> pViewSM2 = LM_DEBUG_PTR_CAST<RenderView>(pView);
    pViewSM2->bind(RT_LIGHT_BUFFER);
    _pHUDRenderer->render(pHUD);
}
//-----------------------------------------------------------------------------
Gfx::EDeviceState RendererSM2::getDeviceState()
{
    return _pDevice->getDeviceState();
}
//-----------------------------------------------------------------------------
void RendererSM2::internalOnDeviceLost(bool verbose)
{
    if(verbose) INF << L"Renderer lost start\n";

    _pLayerMapGen = null;

    if(verbose) INF << L"  Free render targets...\n";
    _pStdMatRenderer->setStaticShadowMap(null);
    _pStdMatRenderer->setDynamicShadowMap(0, null);
    _pStdMatRenderer->setDynamicShadowMap(1, null);

    Ptr<Gfx::IRenderTargetView> pColorView;
    Ptr<Gfx::IDepthStencilView> pDepthView;

    _pDevice->getRenderTarget(pColorView, pDepthView);
    _pDevice->setRenderTarget(pColorView, null);

    pColorView = null;
    pDepthView = null;

    if(verbose) INF << L"  Free reflexions...\n";
    _pReflexionView = null;

    if(verbose) INF << L"  Free resource lib...\n";
    _pResLib->onDeviceLost();

    if(verbose) INF << L"  Free swap chain...\n";
    _pSwapChain = null;

    if(verbose) INF << L"Renderer lost end\n";
}
//-----------------------------------------------------------------------------
void RendererSM2::createDefaultRenderView()
{
    if(_renderSettings.fullscreen)
    {
        _pDefaultView = Ptr<RenderView>(new RenderView(_pDevice, _scDesc, _pSwapChain, _pResLib, _renderSettings));
        _pDefaultView->bind(RT_LIGHT_BUFFER);
    }
    else
    {
        _pDefaultView = LM_DEBUG_PTR_CAST<RenderView>(createView(_scDesc.width, _scDesc.height, 2, _scDesc.handleWindow));
        _pDefaultView->bind(RT_LIGHT_BUFFER);
    }
}
//-----------------------------------------------------------------------------
void RendererSM2::onDeviceLost()
{
    internalOnDeviceLost(true);
}
//-----------------------------------------------------------------------------
void RendererSM2::onDeviceReset()
{
    internalOnDeviceReset(true);
}
//-----------------------------------------------------------------------------
void RendererSM2::internalOnDeviceReset(bool verbose)
{
    if(verbose) INF << L"Renderer reset start\n";

    if(verbose) INF << L"  Device reset...\n";
    _pSwapChain = _pDevice->reset(_scDesc);

    if(verbose) INF << L"  Resource lib reset...\n";
    _pResLib->onDeviceReset();

    if(verbose) INF << L"  Create default renderview...\n";
    createDefaultRenderView();

    if(verbose) INF << L"  Init reflexions...\n";
    initReflexions();

    if(verbose) INF << L"Renderer reset end\n";
}
//-----------------------------------------------------------------------------
void RendererSM2::lock()
{
    _mutex.lock();
}
//-----------------------------------------------------------------------------
void RendererSM2::unlock()
{
    _mutex.unlock();
}
//-----------------------------------------------------------------------------
void RendererSM2::dumpSettings(const RendererSettings & settings)
{
    INF << L"  Fullscreen     : " << Core::strYesNo(settings.fullscreen) << L"\n";
    INF << L"  Resolution     : " << Core::toString(settings.width) << L"x" << Core::toString(settings.height) << L"\n";
    INF << L"  VSync on       : " << Core::strYesNo(settings.vsync) << L"\n";
    INF << L"  Shader level   : " << Renderer::toString(settings.shaderLevel) << L" / " << Renderer::toString(_maxRenderSettings.shaderLevel) << L"\n";
    INF << L"  Shadow level   : " << Renderer::toString(settings.shadowLevel) << L" / " << Renderer::toString(_maxRenderSettings.shadowLevel) << L"\n";
    INF << L"  Texture level  : " << Renderer::toString(settings.textureLevel) << L" / " << Renderer::toString(_maxRenderSettings.textureLevel) << L"\n";
    INF << L"  Reflec level   : " << Renderer::toString(settings.reflecLevel) << L" / " << Renderer::toString(_maxRenderSettings.reflecLevel) << L"\n";
    INF << L"  Filter level   : " << Renderer::toString(settings.filterLevel) << L" / " << Renderer::toString(_maxRenderSettings.filterLevel) << L"\n";
    INF << L"  MSAA level     : " << Renderer::toString(settings.msaaLevel) << L" / " << Renderer::toString(_maxRenderSettings.msaaLevel) << L"\n";
    INF << L"  Glow on        : " << Core::strYesNo(settings.glowOn) << L" / " << Core::strYesNo(settings.glowOn) << L"\n";
    INF << L"  PostFX on      : " << Core::strYesNo(settings.postFXOn) << L" / " << Core::strYesNo(settings.postFXOn) << L"\n";
}
//-----------------------------------------------------------------------------
void RendererSM2::compileAllShaders()
{
    RendererSettings settings;

    for(int32 iShaderLevel=int32(SHADER_LOW); iShaderLevel < EShaderLevel_COUNT; iShaderLevel++)
    {
        settings.shaderLevel = (EShaderLevel)iShaderLevel;

        internalSetSettings(settings);
        internalOnDeviceLost(false);
        _pResLib->onTextureLevelChange(settings.textureLevel);
        internalOnDeviceReset(false);
        initialise();
        _pStdMatRenderer->compileAllShaders();
    }
}
//-----------------------------------------------------------------------------
void RendererSM2::autoConfig(const Ptr<Gfx::IDriver> & pDriver, int32 iAdapter, RendererSettings & settings, const ConfigProfile & profile)
{
    Renderer::autoConfig(pDriver, iAdapter, settings, profile);
}
//-----------------------------------------------------------------------------
void RendererSM2::createDebugMipTexture()
{
    // ARGB
    Core::List<uint32>  colors(11);
    colors[ 0] = 0xFF008800; // 1024 - vert sombre
    colors[ 1] = 0xFF880000; //  512 - rouge sombre
    colors[ 2] = 0xFFFFFFFF; //  256 - blanc
    colors[ 3] = 0xFF888888; //  128 - gris
    colors[ 4] = 0xFF000000; //   64 - noir
    colors[ 5] = 0xFF00FFFF; //   32 - cyan
    colors[ 6] = 0xFFFF00FF; //   16 - magenta
    colors[ 7] = 0xFFFFFF00; //    8 - jaune
    colors[ 8] = 0xFF0000FF; //    4 - bleu
    colors[ 9] = 0xFF00FF00; //    2 - vert
    colors[10] = 0xFFFF0000; //    1 - rouge

    Core::List<Assets::TextureImage> images;

    for(int32 iMip=0; iMip < colors.size(); iMip++)
    {
        int32 size = 1 << (colors.size() - (iMip + 1));
        int32 pixelCount = size * size;
        int32 byteCount = 4 * pixelCount;

        Core::List<uint32> pixels(pixelCount, colors[iMip]);
        images.push_back(Assets::TextureImage(size, size, Assets::TEX_FORMAT_ARGB8, pixels.data(), byteCount));
    }

    Ptr<Assets::Texture> pDebugTexDataMip(new Assets::Texture(images));
    _pDebugTextureMip = LM_DEBUG_PTR_CAST<TextureMap>(uploadTexture(pDebugTexDataMip));
}
//-----------------------------------------------------------------------------
}
