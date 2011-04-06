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
#ifndef RENDERERHQ_H_
#define RENDERERHQ_H_

#include <Core/RT/Mutex.h>
#include <Gfx/IDevice.h>
#include <Gfx/IDriver.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Common/Mesh.h>
#include <Renderer/Common/ScreenQuad.h>
#include <Renderer/Common/ShaderLib.h>
#include <Renderer/Common/GPUResource.h>
#include <Renderer/SM2/ICommandExecuter.h>
#include <Renderer/SM2/BillboardRenderer.h>
#include <Renderer/SM2/BoxRenderer.h>
#include <Renderer/SM2/DecalRenderer.h>
#include <Renderer/SM2/GlowRenderer.h>
#include <Renderer/SM2/StandardMatRenderer.h>
#include <Renderer/SM2/CustomMatRenderer.h>
#include <Renderer/SM2/SkysphereRenderer.h>
#include <Renderer/SM2/SkyboxRenderer.h>
#include <Renderer/SM2/RenderView.h>
#include <Renderer/SM2/IRenderTarget.h>
#include <Renderer/SM2/GrassRenderer.h>
#include <Renderer/SM2/FreeFormRenderer.h>
#include <Renderer/SM2/WaterRenderer.h>
#include <Renderer/SM2/BoxRenderer.h>
#include <Renderer/SM2/SplineRenderer.h>
#include <Renderer/SM2/ShadowRenderer.h>
#include <Renderer/SM2/DebugRenderer.h>
#include <Renderer/SM2/HUDRenderer.h>
#include <Renderer/SM2/PostFXRenderer.h>
#include <Renderer/SM2/OcclusionMapGenerator.h>
#include <Renderer/SM2/LayerMapGenerator.h>
#include <Renderer/SM2/SceneSM2.h>

namespace Renderer
{
class TextureMap;

const int32 MULTITHREADED_FLAG = 0x00000001;

class LM_API_SM2 RendererSM2 : public Core::Object, public IRenderer
{
public:
    RendererSM2(const Ptr<Gfx::IDriver> & pDriver,
                const RendererSettings & settings,
                void *  pHandle,
                const Ptr<Core::VirtualFileSystem> & pVFS,
                const String & shadersDir,
                int32 flags = 0);
    ~RendererSM2();

    virtual bool initialise();

    virtual Ptr<IMesh> uploadMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber);
    virtual Ptr<ISkinMesh> uploadSkinMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber);
    virtual Ptr<ITextureMap> uploadTexture(const Ptr<Assets::Texture> & pTexture);
    virtual Ptr<ICubeTextureMap> uploadTexture(const Ptr<Assets::CubeTexture> & pTexture);

    virtual Ptr<ITextureMap> getDefaultTexture() const;
    virtual Ptr<IFont> createFont(const String & name, int32 height);
    virtual Ptr<IPictureBank> uploadPictureBank(const Ptr<Assets::PictureBank> & pPicBank);

    virtual Ptr<IScene> createScene();
    virtual Ptr<IHUD> createHUD();

    virtual Ptr<IRenderView> getDefaultView() const;
    virtual Ptr<IRenderView> createView(int32 width, int32 height, int32 bufferCount, void * pHandle);

    virtual void renderScene(const Ptr<IRenderView> & pView, const Ptr<IScene> & pScene, const Ptr<ICamera> & pCamera);
    virtual void renderScenes(const Ptr<IRenderView> & pView, const Core::List<Ptr<IScene> > & scenes, const Ptr<ICamera> & pCamera);

    virtual void renderHUD(const Ptr<IRenderView> & pView, const Ptr<IHUD> & pHUD);

    virtual Ptr<Core::Bitmap> generateLightMap(const LightMapInfos & lm, const Ptr<IMeshInstance> & pMesh);
    virtual Ptr<Core::Bitmap> generateLayerMap(const LayerMapInfos & lm, const Ptr<IMeshInstance> & pMesh);
    virtual Ptr<Core::Bitmap> generateOcclusionMap(const OcclusionMapInfos & om, const Ptr<Assets::VerticeSet> & pVSet);
    virtual void generateSkyBox(const SkysphereSettings & settings, SkyboxInfos & skybox);

    virtual void setRenderSettings(const RendererSettings & settings);
    virtual const RendererSettings & getRenderSettings() const;
    virtual const RendererSettings & getMaxRenderSettings() const;

    virtual void setDebugSettings(const DebugSettings & settings);
    virtual const DebugSettings & getDebugSettings() const;

    virtual String getRenderStats() const;
    virtual int32 getVRAMUsage() const;

    virtual Gfx::EDeviceState getDeviceState();

    virtual const Core::List<Resolution> & getAllResolutions() const { return _allResolutions; };
    virtual const Core::List<Resolution> & getPreferredResolutions() const { return _preferredResolutions; };

    virtual void lock();
    virtual void unlock();

    virtual void onDeviceLost();
    virtual void onDeviceReset();

    void compileAllShaders();

    static void autoConfig(const Ptr<Gfx::IDriver> & pDriver, int32 iAdapter, RendererSettings & settings, const ConfigProfile & profile);

protected:
    Ptr<Gfx::IDriver>               _pDriver;
    Ptr<Gfx::IDevice>               _pDevice;
    Ptr<Core::VirtualFileSystem>    _pVFS;

    Gfx::SwapChainDesc              _scDesc;
    Ptr<Gfx::ISwapChain>            _pSwapChain;

    mutable Core::Mutex             _mutex;

    Core::List<Resolution>          _allResolutions;
    Core::List<Resolution>          _preferredResolutions;

    Ptr<GPUResourceLib>             _pResLib;

    Ptr<PackedVerticesFormat>       _pVertexFormat;
    Ptr<ScreenQuad>                 _pScreenQuad;

    Ptr<ShaderLib>                  _pShaderLib;
    String                          _shadersDir;

    Ptr<TextureMap>                 _pDefaultTextureWhite;
    Ptr<TextureMap>                 _pDefaultTextureBlack;
    Ptr<TextureMap>                 _pDefaultTextureBump;
    
    Ptr<TextureMap>                 _pDebugTextureMip;

    Ptr<IStandardMatRenderer>       _pStdMatRenderer;
    Ptr<ICustomMatRenderer>         _pCustomMatRenderer;
    Ptr<IDecalRenderer>             _pDecalRenderer;
    Ptr<IBillboardRenderer>         _pBillRenderer;
    Ptr<ISkysphereRenderer>         _pSkyRenderer;
    Ptr<ISkyboxRenderer>            _pSkyBoxRenderer;
    Ptr<IGlowRenderer>              _pGlowRenderer;
    Ptr<IGrassRenderer>             _pGrassRenderer;
    Ptr<IWaterRenderer>             _pWaterRenderer;
    Ptr<IBoxRenderer>               _pBoxRenderer;
    Ptr<ISplineRenderer>            _pSplineRenderer;
    Ptr<IFreeFormRenderer>          _pFreeFormRenderer;
    Ptr<IShadowRenderer>            _pShadowRenderer;
    Ptr<IDebugRenderer>             _pDebugRenderer;
    Ptr<IPostFXRenderer>            _pPostFXRenderer;
    Ptr<HUDRenderer>                _pHUDRenderer;
    Ptr<LayerMapGenerator>          _pLayerMapGen;
    Ptr<OcclusionMapGenerator>      _pOcclusionMapGen;

    Core::List<Ptr<ICommandExecuter> > _executers;

    // Réflexions
    int32                           _refCubeSize;
    Ptr<RenderView>                 _pReflexionView;
    Ptr<RenderView>                 _pDefaultView;

    RendererSettings                _renderSettings;
    RendererSettings                _maxRenderSettings;
    DebugSettings                   _debugSettings;
    String                          _stats;
    int64                           _totalVRAM;

    virtual bool initReflexions();

    void createRenderCube(Ptr<Gfx::ITextureCube> & pCubeTex, Ptr<Gfx::IRenderTargetView> * ppViews, int32 size);

    static const int32 IN_REFLEXION_UPDATE = 0x00000001;

    virtual void internalRenderScene(const Ptr<IRenderTarget> & pView, const Core::List<Ptr<SceneSM2> > & scenes, 
                                     const Ptr<ICamera> & pCamera, int32 width, int32 height, int32 flags);

    void prepareMeshes(
            const Ptr<SceneSM2> & pScene,
            const RenderContext & context,
            const Core::List<MeshInstanceSM2 *> & meshes,
            const Core::List<SkinMeshInstanceSM2 *> & skinMeshes,
            int32 flags);

    void prepareMesh(const Ptr<SceneSM2> & pScene,
                     Core::List<PointLight *> lights,
                     const RenderContext & context,
                     IRenderable & renderable,
                     int32 flags);

    void renderContext(const RenderContext & context, ERenderPass pass);

    void createDefaultRenderView();
    void createDebugMipTexture();

    void internalOnDeviceLost(bool verbose);
    void internalOnDeviceReset(bool verbose);
    void internalSetSettings(const RendererSettings & settings);
    void dumpSettings(const RendererSettings & settings);
};

}

#endif /* RENDERERHQ_H_ */
