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
#include <Core/VFS/VFSXMLTranslator.h>
#include <Core/TGA.h>

#include "AppTVRenderer.h"
#include "SceneTeapot.h"
#include "SceneLights.h"
#include "SceneBunny.h"
#include "SceneAnimation.h"
#include "SceneDecals.h"
#include "SceneFreeForms.h"
#include "SceneBillboards.h"
#include "SceneGrass.h"
#include "SceneWater.h"
#include "SceneLightMap.h"
#include "SceneLightMap2.h"
#include "SceneCOLLADA.h"
#include "SceneModelTerrain.h"
#include "SceneLayers.h"
#include "SceneCrashTest.h"

static const bool FULLSCREEN = false;

//-----------------------------------------------------------------------------
AppTVRenderer::AppTVRenderer() :
    _deviceState(Gfx::EDeviceState(-1))
{
}
//-----------------------------------------------------------------------------
AppTVRenderer::~AppTVRenderer()
{
    _pCurrentScene = null;

    _pSceneTeapots = null;
    _pSceneLights = null;
    _pSceneBunny = null;
    _pSceneAnimation = null;
    _pSceneDecals = null;
    _pSceneBillboards = null;
    _pSceneGrass = null;
    _pSceneWater = null;
    _pSceneLightMap = null;
    _pSceneLightMap2 = null;
    _pSceneModelTerrain = null;
    _pSceneLayers= null;
    _pSceneCrashTest = null;

    _pTextFPS = null;
    _pFontFPS = null;
    _pHUD = null;

    _pRenderer = null;
    _pDriver = null;
    _pRenderWindow = null;
    _pVFS = null;

    _pWinHandler = null;
    _pInput = null;
}
//-----------------------------------------------------------------------------
bool AppTVRenderer::initialise()
{
    Core::String workDir(LM_WORKING_DIRECTORY);
    SetCurrentDirectory(Core::String8(workDir).c_str());

    // Initialisation VFS ---------------------------------------------

    {
        _pVFS = Ptr<Core::VFSDirectMapping>(new Core::VFSDirectMapping());
        if(!_pVFS->linkDirectory(L"."))
        {
            ERR << L"Unable to initialise VFS\n";
            return false;
        }
/*
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-TVRenderer.xml"))
        {
            _pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << L"VirtualFileSystem layout :\n";
            INF << _pVFS->toString() << "\n";
        }
        else
            throw Core::Exception(L"Unable to mount VFS.");
            */

    }

    _pInput = Ptr<Window::InputManager>(new Window::InputManager());
    _pWinHandler = Ptr<Window::WinHandler>(new Window::WinHandler(_pInput));
    _pRenderWindow = Ptr<Window::Window>(new Window::Window(
            getInstance(),
            L"TVRenderer",
            100, 100,    // position
            //FULLSCREEN ? 1920 : 800, 
            //FULLSCREEN ? 1080 : 600,    // dimension
            800, 
            600,    // dimension
            FULLSCREEN,  // fullscreen
            _pWinHandler));

    _pInput->initialise(_pRenderWindow->getHandle());
    _pDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());

    Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
    rs.fullscreen = FULLSCREEN;
    rs.width = _pRenderWindow->getWidth();
    rs.height = _pRenderWindow->getHeight();
    rs.advancedPostFX = true;

    _pRenderer = Ptr<Renderer::IRenderer>(new Renderer::RendererSM2(
            _pDriver,
            rs,
            _pRenderWindow->getHandle(),
            _pVFS,
            L"/Shaders",
            0));

    bool result = _pRenderer->initialise();

    if(result)
    {
        TestSceneData data;
        data.pRenderer = _pRenderer;
        data.pInput = _pInput;

        _pSceneTeapots = Ptr<ITestScene>(new SceneTeapot(data));
        _pSceneLights = Ptr<ITestScene>(new SceneLights(data));
        _pSceneBunny = Ptr<ITestScene>(new SceneBunny(data));
        _pSceneAnimation = Ptr<ITestScene>(new SceneAnimation(data));
        _pSceneDecals = Ptr<ITestScene>(new SceneDecals(data));
        _pSceneBillboards = Ptr<ITestScene>(new SceneBillboards(data));
        _pSceneGrass = Ptr<ITestScene>(new SceneGrass(data));
        _pSceneWater = Ptr<ITestScene>(new SceneWater(data));
        _pSceneLightMap = Ptr<ITestScene>(new SceneLightMap(data));
        _pSceneLightMap2 = Ptr<ITestScene>(new SceneLightMap2(data));
        _pSceneCOLLADA = Ptr<ITestScene>(new SceneCOLLADA(data));
        _pSceneModelTerrain = Ptr<ITestScene>(new SceneModelTerrain(data));
        _pSceneLayers = Ptr<ITestScene>(new SceneLayers(data));
        _pSceneCrashTest = Ptr<ITestScene>(new SceneCrashTest(data));
        _pSceneFreeForms = Ptr<ITestScene>(new SceneFreeForms(data));

        setScene(_pSceneLightMap);
    }

    // Compteur FPS

    _pHUD = _pRenderer->createHUD();
    _pFontFPS = _pRenderer->createFont(L"Arial", 10);
    _pTextFPS = _pHUD->createText(_pFontFPS, L"");
    _pTextFPS->setColor(Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
    _pTextFPS->setPosition(Core::Vector2f(5.0f, 5.0f));

    // Curseur souris

    _pCursor = Core::loadTGA(L"Data/TUData/GUI/cursor.tga");

    return result;
}
//-----------------------------------------------------------------------------
void AppTVRenderer::setScene(const Ptr<ITestScene> & pScene)
{
    try
    {
        if(!pScene->isInitialised() && pScene->initialise(_pVFS, _pRenderWindow))
            pScene->setInitialised();

        _pCurrentScene = pScene;
    }
    catch(Core::Exception & e)
    {
        ERR << L"Error initialising scene : " << e.getMessage() << L"\n";
    }
}
//-----------------------------------------------------------------------------
void AppTVRenderer::update(double elapsed)
{
    if(_pInput->isKeyTyped(VK_ESCAPE))
        PostQuitMessage(0);

    if(!_pInput->isKeyDown(VK_LCONTROL))
    {
        if(_pInput->isKeyDown(VK_LSHIFT))
        {
            if     (_pInput->isKeyTyped(VK_F1)) setScene(_pSceneBunny);
            else if(_pInput->isKeyTyped(VK_F1)) setScene(_pSceneCrashTest);
            else if(_pInput->isKeyTyped(VK_F4)) setScene(_pSceneLightMap2);
            else if(_pInput->isKeyTyped(VK_F9)) setScene(_pSceneLayers);
        }
        else
        {
            if     (_pInput->isKeyTyped(VK_F1)) setScene(_pSceneTeapots);
            else if(_pInput->isKeyTyped(VK_F2)) setScene(_pSceneLights);
            else if(_pInput->isKeyTyped(VK_F3)) setScene(_pSceneAnimation);
            else if(_pInput->isKeyTyped(VK_F4)) setScene(_pSceneLightMap);
            else if(_pInput->isKeyTyped(VK_F5)) setScene(_pSceneDecals);
            else if(_pInput->isKeyTyped(VK_F6)) setScene(_pSceneBillboards);
            else if(_pInput->isKeyTyped(VK_F7)) setScene(_pSceneGrass);
            else if(_pInput->isKeyTyped(VK_F8)) setScene(_pSceneWater);
            else if(_pInput->isKeyTyped(VK_F9)) setScene(_pSceneModelTerrain);
            else if(_pInput->isKeyTyped(VK_F11)) setScene(_pSceneCOLLADA);
        }
    }

    if(_pInput->isKeyTyped(VK_F12))
    {
        Renderer::Screenshots shots;
        _pRenderer->getDefaultView()->getDebugScreenShots(shots);

        Images::toImage(shots.pColor)->saveFile(L"Local/tvrenderer-00-color.tga");
        Images::toImage(shots.pRefrac)->saveFile(L"Local/tvrenderer-01-refrac.tga");
        Images::toImage(shots.pReflec)->saveFile(L"Local/tvrenderer-02-reflec.tga");
        Images::toImage(shots.pGlow0)->saveFile(L"Local/tvrenderer-03-glow0.tga");
        Images::toImage(shots.pGlowHalf)->saveFile(L"Local/tvrenderer-04-glowhalf.tga");
        Images::toImage(shots.pGlow1)->saveFile(L"Local/tvrenderer-05-glow1.tga");
        Images::toImage(shots.pGlow2)->saveFile(L"Local/tvrenderer-06-glow2.tga");
    }


    _pRenderWindow->setCursor(_pCursor, 2, 2);
    _pRenderWindow->toggleFullscreen(_pRenderer->getRenderSettings().fullscreen, _pRenderer->getRenderSettings().width, _pRenderer->getRenderSettings().height);

    if(_pCurrentScene != null)
    {
        _pCurrentScene->update(elapsed);
    }
}
//-----------------------------------------------------------------------------
void AppTVRenderer::render()
{
    if(_pCurrentScene != null)
    {
        Ptr<Renderer::IRenderView> pView(_pRenderer->getDefaultView());
        pView->resize(_pWinHandler->getWindowWidth(), _pWinHandler->getWindowHeight());

        _pCurrentScene->render();

        _fps.nextFrame();
        _pTextFPS->setText(_fps.getText() + L"\n" + _pRenderer->getRenderStats());

        _pRenderer->renderHUD(pView, _pHUD);
        
        pView->present();
    }
}
//-----------------------------------------------------------------------------
