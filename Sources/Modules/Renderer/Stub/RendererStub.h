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
#ifndef RENDERER_RENDERERSTUB_H_
#define RENDERER_RENDERERSTUB_H_

#include <Renderer/IRenderer.h>


namespace Renderer
{
class LM_API_RDS RendererStub : public Core::Object, public IRenderer
{
public:
    RendererStub();
    virtual bool initialise();

    virtual Ptr<IMesh> uploadMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber);
    virtual Ptr<ISkinMesh> uploadSkinMesh(const Ptr<Assets::ModelMesh> & pMesh, ITextureGrabber & texGrabber);
    virtual Ptr<ITextureMap> uploadTexture(const Ptr<Assets::Texture> & pTexture);
    virtual Ptr<ICubeTextureMap> uploadTexture(const Ptr<Assets::CubeTexture> & pTexture);

    virtual Ptr<ITextureMap> getDefaultTexture() const;

    virtual Ptr<IFont> createFont(const String & name, int32 height) { return null; };
    virtual Ptr<IPictureBank> uploadPictureBank(const Ptr<Assets::PictureBank> & pPicBank) { return null; };

    virtual Ptr<IScene> createScene();
    virtual Ptr<IHUD> createHUD() { return null; };

    virtual Ptr<IRenderView> getDefaultView() const;
    virtual Ptr<IRenderView> createView(int32 width, int32 height, int32 bufferCount, void * pHandle);

    virtual void renderScene(const Ptr<IRenderView> & pView, const Ptr<IScene> & pScene, const Ptr<ICamera> & pCamera);
    virtual void renderScenes(const Ptr<IRenderView> & pView, const Core::List<Ptr<IScene> > & scenes, const Ptr<ICamera> & pCamera) {};

    virtual void renderHUD(const Ptr<IRenderView> & pView, const Ptr<IHUD> & pHUD) {};

    virtual void getRadiosity(const RadiosityQuery & radiosity, Core::List<Core::Vector4f> & result) {};

    virtual Ptr<Core::Bitmap> generateLightMap(const LightMapInfos & lm, const Ptr<IMeshInstance> & pMesh) { return null; };
    virtual Ptr<Core::Bitmap> generateLayerMap(const LayerMapInfos & lm, const Ptr<IMeshInstance> & pMesh) { return null; };

    virtual Ptr<Assets::CubeTexture> generateLightProbe(const LightProbeInfos & lp) { return null; };

    virtual String getRenderStats() const { return String::EMPTY; };

    virtual void setRenderSettings(const RendererSettings & settings) { _settings = settings; };
    virtual const RendererSettings & getRenderSettings() const { return _settings; };
    virtual const RendererSettings & getMaxRenderSettings() const { return _settings; };

    virtual Gfx::EDeviceState getDeviceState() { return Gfx::DS_DEVICE_OK; };

    virtual void setDebugSettings(const DebugSettings & settings) { _debug = settings; };
    virtual const DebugSettings & getDebugSettings() const { return _debug; };

    virtual const Core::List<Resolution> & getAllResolutions() const { return _res; };
    virtual const Core::List<Resolution> & getPreferredResolutions() const { return _res; };

    virtual void lock() {};
    virtual void unlock() {};

    virtual void onDeviceLost() {};
    virtual void onDeviceReset() {};

    Core::List<Resolution> _res;
    Ptr<IRenderView> _pDefaultView;
    Ptr<ITextureMap> _pDefaultTexture;
    RendererSettings _settings;
    DebugSettings    _debug;
};
}

#endif /*ABSTRACT_RENDERER_H_*/
