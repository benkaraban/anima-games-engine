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

#include "AppTVUniverse.h"
#include "WorldTestBaseTasks.h"
#include "WorldTestNodesTypes.h"
#include "WorldTestTerrain.h"
#include "WorldTestModels.h"
#include "WorldTestLightMaps.h"
#include "WorldTestLightning.h"
#include <Assets/GeometricPrimitive.h>

#include <Audio/AudioOpenAL/DriverOpenAL.h>

//-----------------------------------------------------------------------------
AppTVUniverse::AppTVUniverse()
{
}
//-----------------------------------------------------------------------------
AppTVUniverse::~AppTVUniverse()
{
    _pCurrentTest = null;

    _pTestNodes = null;
    _pTestTasks = null;
    _pTestTerrain = null;
    _pTestModels = null;
    _pTestLightMaps = null;
    _pTestLightning = null;

    _pRenderer = null;
    _pDriver = null;
    _pRenderWindow = null;
    _pVFS = null;

    _pWinHandler = null;
    _pInput = null;
}
//-----------------------------------------------------------------------------
bool AppTVUniverse::initialise()
{
    const bool FULLSCREEN = false;

    Core::String workDir(LM_WORKING_DIRECTORY);
    SetCurrentDirectory(Core::String8(workDir).c_str());

    // Initialisation VFS ---------------------------------------------

    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-TVUniverse.xml"))
        {
            _pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << L"VirtualFileSystem layout :\n";
            INF << _pVFS->toString() << "\n";
        }
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }

    _pInput = Ptr<Window::InputManager>(new Window::InputManager());
    _pWinHandler = Ptr<Window::WinHandler>(new Window::WinHandler(_pInput));
    _pRenderWindow = Ptr<Window::Window>(new Window::Window(
            getInstance(),
            L"TVUniverse",
            100, 100,    // position
            1600, 800,    // dimension
            FULLSCREEN,  // fullscreen
            _pWinHandler));

    _pInput->initialise(_pRenderWindow->getHandle());
    _pDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());

    Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
    rs.fullscreen = FULLSCREEN;
    rs.width = _pRenderWindow->getWidth();
    rs.height = _pRenderWindow->getHeight();
    rs.msaaLevel = Renderer::MSAA_4X;
    //rs.filterLevel = Renderer::FILTER_BILINEAR;
    rs.glowOn = true;

    _pRenderer = Ptr<Renderer::IRenderer>(new Renderer::RendererSM2(
            _pDriver,
            rs,
            _pRenderWindow->getHandle(),
            _pVFS,
            L"/Shaders"));

    _pPerfAnalyzer = Ptr<Renderer::PerfAnalyzer>(new Renderer::PerfAnalyzer(_pRenderer));
    _pAudioDriver = Ptr<Audio::IDriver>(new AudioOpenAL::DriverOpenAL());
    _pAudioDevice = _pAudioDriver->createDevice(0);

    bool result = _pRenderer->initialise();

    if(result)
    {
        _pTestNodes = Ptr<IWorldTest>(new WorldTestNodesTypes());
        _pTestTasks = Ptr<IWorldTest>(new WorldTestBaseTasks());
        _pTestTerrain = Ptr<IWorldTest>(new WorldTestTerrain());
        _pTestModels = Ptr<IWorldTest>(new WorldTestModels());
        _pTestLightMaps = Ptr<IWorldTest>(new WorldTestLightMaps());
        _pTestLightning = Ptr<IWorldTest>(new WorldTestLightning());

        setTest(_pTestLightMaps);
    }

    // Compteur FPS


    return result;
}
//-----------------------------------------------------------------------------
void AppTVUniverse::setTest(const Ptr<IWorldTest> & pTest)
{
    try
    {
        if(!pTest->isInitialised())
        {
            pTest->initalise(_pRenderer, _pVFS, _pInput, _pAudioDevice);
            pTest->setInitialised();
        }

        _pCurrentTest = pTest;
    }
    catch(Core::Exception & e)
    {
        Core::System::errorMessageBox(e.getMessage());
        ERR << L"Error initialising world : " << e.getMessage() << L"\n";
        ERR << e.getCallStack();
    }
}
//-----------------------------------------------------------------------------
void AppTVUniverse::update(double elapsed)
{
    LM_START_PERF_LOG_FRAME();

    if(_pInput->isKeyTyped(VK_ESCAPE))
        PostQuitMessage(0);

    if(!_pInput->isKeyDown(VK_LCONTROL))
    {
        if(_pInput->isKeyTyped(VK_F1)) setTest(_pTestNodes);
        else if(_pInput->isKeyTyped(VK_F2)) setTest(_pTestTasks);
        else if(_pInput->isKeyTyped(VK_F3)) setTest(_pTestTerrain);
        else if(_pInput->isKeyTyped(VK_F4)) setTest(_pTestModels);
        else if(_pInput->isKeyTyped(VK_F5)) setTest(_pTestLightMaps);
        else if(_pInput->isKeyTyped(VK_F6)) setTest(_pTestLightning);
    }

    if(_pCurrentTest != null)
    {
        _pCurrentTest->update(std::min(0.01, elapsed));
    }
}
//-----------------------------------------------------------------------------
void AppTVUniverse::render()
{
    if(_pCurrentTest != null)
    {

        Ptr<Renderer::IRenderView> pView(_pRenderer->getDefaultView());
        pView->resize(_pWinHandler->getWindowWidth(), _pWinHandler->getWindowHeight());

        _pCurrentTest->render(pView);

        _pPerfAnalyzer->display(pView);
        pView->present();

        LM_END_PERF_LOG_FRAME();
        _pPerfAnalyzer->update();
    }
}
//-----------------------------------------------------------------------------
