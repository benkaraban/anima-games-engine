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
#ifndef ITESTSCENE_H_
#define ITESTSCENE_H_

#include <Core/Math/CameraController.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <Window/Window.h>
#include <Window/InputManager.h>
#include <Renderer/IRenderer.h>
#include <Workflow/TexCompression.h>


class TextureGrabber : public Renderer::ITextureGrabber
{
public:
    TextureGrabber(const Ptr<Core::VirtualFileSystem> & pVFS,
                   const Ptr<Renderer::IRenderer> & pRenderer);

    virtual Ptr<Renderer::ITextureMap> getTexture(const String & name);
    virtual Ptr<Renderer::ICubeTextureMap> getCubeTexture(const String & name);

protected:
    Ptr<Core::VirtualFileSystem> _pVFS;
    Ptr<Renderer::IRenderer>     _pRenderer;
    Core::Map<String, Ptr<Renderer::ITextureMap> > _textures2D;
};

struct SkeletonInfos
{
    Ptr<Assets::Model>                        pModel;
    Ptr<Renderer::ISkeletonInstance>          pSkeleton;
    Core::List<Ptr<Renderer::IMesh> >         meshes;
    Core::List<Ptr<Renderer::ISkinMesh> >     skins;
    Core::List<Ptr<Renderer::IMeshInstance> > nodes;
};

LM_ENUM_2(ECameraType,
          CAMERA_ORBITAL,
          CAMERA_QUAKE);

struct TestSceneData
{
    Ptr<Renderer::IRenderer>  pRenderer;
    Ptr<Window::InputManager> pInput;
};

class ITestScene
{
public:
    ITestScene(const TestSceneData & data);
    virtual ~ITestScene() {};

    virtual bool initialise(const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::Window> & pRenderWindow) = 0;
    virtual bool isInitialised() { return _initialised; };
    virtual void setInitialised() { _initialised = true; };

    virtual void update(double elapsed);
    virtual void render() = 0;

    virtual void setBoxesVisible(bool flag);

protected:
    Ptr<Renderer::IRenderer>  _pRenderer;
    Ptr<Renderer::IScene>     _pScene;
    Ptr<Renderer::IPerspectiveCamera> _pCamera;

    Ptr<Window::InputManager> _pInput;

    bool         _initialised;

    float   _sensibilityRot;
    float   _sensibilityXY;
    float   _sensibilityZ;
    float   _quakeCamSpeed;

    float   _sensibilityLightColor;

    // CAMERAS

    ECameraType               _camType;
    Core::ICameraController * _pCameraCtrl;
    Core::CameraOrbital       _camOrbit;
    Core::CameraQuake         _camQuake;


    Core::Matrix4f _matWorldCamera;

    Core::List<Ptr <Renderer::IAABox> > _boxes;
    bool                                _showBoxes;

    void updateCamera();
    void setCamera(ECameraType camType);

    Ptr<Renderer::ITextureMap> uploadTexture(const Ptr<Images::Image> & pImage, Workflow::ETextureType type);

    Ptr<Renderer::ITextureMap> loadTexture(const Ptr<Core::VirtualFileSystem> & pVFS, const String & fileName, Workflow::ETextureType type);
    Ptr<Renderer::ITextureMap> loadPackedTexture(const Ptr<Core::VirtualFileSystem> & pVFS, const String & fileName);

    void loadSkeleton(SkeletonInfos & infos, const Ptr<Core::VirtualFileSystem> & pVFS, 
                      const String & fileName, Renderer::ITextureGrabber & texGrabber,
                      bool skin = true);
    void animateSkeleton(SkeletonInfos & infos, int32 animId, float key);
};

#endif /* ITESTSCENE_H_ */
