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
#ifndef APPTVRENDERER_H_
#define APPTVRENDERER_H_

#include <winsafe.h>

#include <Assets/ModelMesh.h>

#include <Core/Debug/FPSCounter.h>
#include <Core/XML/XMLDocument.h>
#include <Core/DateAndTime.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Bitmap.h>

#include <Gfx/IDriver.h>
#include <Gfx/IDevice.h>
#include <Gfx/GfxDx9/DriverDx9.h>

#include <Renderer/SM2/RendererSM2.h>

#include <Window/Application.h>
#include <Window/InputManager.h>
#include <Window/WinHandler.h>

#include <Workflow/ModelImporterX.h>
#include <Workflow/TexCompression.h>

#include "ITestScene.h"

class AppTVRenderer : public Window::Application
{
public:
    AppTVRenderer();
    virtual ~AppTVRenderer();

protected:
    virtual bool initialise();
    virtual void update(double elapsed);
    virtual void render();

    void setScene(const Ptr<ITestScene> & pScene);

    virtual Renderer::IRenderer &   getRenderer() { return *_pRenderer; }
    virtual Window::InputManager &  getInputManager() { return *_pInput; }
    virtual Window::WinHandler &    getWinManager() { return *_pWinHandler; }


    Ptr<Core::VFSDirectMapping>  _pVFS;
    Ptr<Window::Window>          _pRenderWindow;
    Ptr<Gfx::IDriver>            _pDriver;

    Ptr<Window::WinHandler>     _pWinHandler;
    Ptr<Window::InputManager>   _pInput;
    Gfx::EDeviceState           _deviceState;

    Core::FPSCounter            _fps;

    Ptr<Core::Bitmap>           _pCursor;

    Ptr<Renderer::IRenderer>    _pRenderer;
    Ptr<Renderer::IHUD>         _pHUD;
    Ptr<Renderer::IFont>        _pFontFPS;
    Ptr<Renderer::IText>        _pTextFPS;

    Ptr<ITestScene>  _pSceneTeapots;
    Ptr<ITestScene>  _pSceneLights;
    Ptr<ITestScene>  _pSceneBunny;
    Ptr<ITestScene>  _pSceneAnimation;
    Ptr<ITestScene>  _pSceneDecals;
    Ptr<ITestScene>  _pSceneBillboards;
    Ptr<ITestScene>  _pSceneGrass;
    Ptr<ITestScene>  _pSceneWater;
    Ptr<ITestScene>  _pSceneLightMap;
    Ptr<ITestScene>  _pSceneLightMap2;
    Ptr<ITestScene>  _pSceneCOLLADA;
    Ptr<ITestScene>  _pSceneModelTerrain;
    Ptr<ITestScene>  _pSceneLayers;
    Ptr<ITestScene>  _pSceneCrashTest;
    Ptr<ITestScene>  _pSceneFreeForms;

    Ptr<ITestScene>  _pCurrentScene;
};

#endif /* APPTVRENDERER_H_ */
