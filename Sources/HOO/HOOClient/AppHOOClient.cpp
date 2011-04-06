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
#include <Core/XML/XMLDocument.h>
#include <Core/VFS/VFSXMLTranslator.h>
#include <Core/Strings/Tools.h>

#include <Gfx/GfxDx9/DriverDx9.h>

#include <Audio/AudioOpenAL/DriverOpenAL.h>
#include <Audio/AudioGlobals.h>

#include <Renderer/SM2/RendererSM2.h>

#include <HOOClient/AppHOOClient.h>
#include <HOOClient/StateOutGame.h>
#include <HOOClient/StateInGame.h>

//-----------------------------------------------------------------------------
AppHOOClient::AppHOOClient()
{
}
//-----------------------------------------------------------------------------
AppHOOClient::~AppHOOClient()
{
#ifdef LM_DEVMODE
    LM_PTR_DELETE(_pDebugHUD);
    LM_PTR_DELETE(_pDebugText);
#endif

    _appContext.pGUI->reset();

    _pState = null;

    for(int32 ii=0; ii < EAppState_COUNT; ii++)
        LM_PTR_DELETE(_states[ii]);

    _appContext.game.setGameLibrary(null);
    _appContext.game.freeLevel();

    LM_PTR_DELETE(_appContext.pWidgets);
    LM_PTR_DELETE(_appContext.pGUI);

    _appContext.pCameraGUI = null;
    _appContext.pAvatarGUI = null;
    LM_PTR_DELETE(_appContext.pWorldGUI);
    LM_PTR_DELETE(_appContext.pOutGameMusic);
    LM_PTR_DELETE(_appContext.pMasterResPool);
    LM_PTR_DELETE(_appContext.pConnexion);

    LM_PTR_DELETE(_appContext.pAudioDevice);
    LM_PTR_DELETE(_appContext.pAudioDriver);

    LM_PTR_DELETE(_appContext.pRenderer);
    LM_PTR_DELETE(_appContext.pGfxDriver);

    LM_PTR_DELETE(_appContext.pWindow);
    LM_PTR_DELETE(_appContext.pWinHandler);
    LM_PTR_DELETE(_appContext.pInput);
    LM_PTR_DELETE(_appContext.pGameLib);
    LM_PTR_DELETE(_appContext.pDico);
    LM_PTR_DELETE(_appContext.pVFS);


    Audio::shutdownVorbisFileDll();
}
//-----------------------------------------------------------------------------
bool AppHOOClient::initialise()
{
    const bool FULLSCREEN = false;

    Core::String workDir(LM_WORKING_DIRECTORY);
    SetCurrentDirectory(Core::String8(workDir).c_str());

    // Initialisation VFS ---------------------------------------------

    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-HOOClient.xml"))
        {
            _appContext.pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << L"VirtualFileSystem layout :\n";
            INF << _appContext.pVFS->toString() << "\n";
        }
        else
        {
            ERR << L"Unable to mount VFS\n";
            return false;
        }
    }

    _appContext.serverAddress = L"127.0.0.1"; // TODO

#ifdef LM_DEVMODE
    {
        _appContext.serverAddress = Network::NetworkEngine::getHostIP();
        _appContext.debugAutoConnect = false;
        _appContext.debugAutoMatch = false;
        _appContext.debugQuickLoad = false;
        _appContext.debugFullscreen = false;

        Core::List<String> args;
        Core::tokenize(getCommandLine(), args, L" ");

        for(int32 ii=0; ii < args.size(); ii++)
        {
            if(args[ii] == L"automatch")
            {
                _appContext.debugAutoConnect = true;
                _appContext.debugAutoMatch = true;
            }
            else if(args[ii] == L"autoconnect")
            {
                _appContext.debugAutoConnect = true;
            }
            else if(args[ii] == L"quickload")
            {
                _appContext.debugQuickLoad = true;
            }
            else if(args[ii] == L"fullscreen")
            {
                _appContext.debugFullscreen = true;
            }
            else if(args[ii].startsWith(L"server="))
            {
                _appContext.serverAddress = args[ii].slice(String(L"server=").size());
            }
        }
    }
#endif

    INF << L"Server address : '" << _appContext.serverAddress << L"'\n";
    _appContext.config.load(_appContext.pVFS);

    {
        Ptr<Core::InputStream> pInputDic = _appContext.pVFS->readFile(L"/HOO/Dictionaries/main.xml");
        
        if(pInputDic == null)
        {
            ERR << L"Unable to open dictionary file\n";
            return false;
        }

        _appContext.language = Core::FRENCH;
        _appContext.pDico = Ptr<Core::Dictionary>(new Core::Dictionary(_appContext.language, *pInputDic));
    }

    _appContext.pGameLib = Ptr<GameEngine::GameLibrary>(new GameEngine::GameLibrary(_appContext.pVFS, L"/HOO/GameLibrary/", _appContext.language));

    int32 xpos = 100;
    int32 ypos = 100;
    int32 width = 1024;
    int32 height = 768;
    bool fullscreen = false;

    _appContext.pGfxDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());

    if(_appContext.debugFullscreen)
    {
        Gfx::DisplayMode currentMode = _appContext.pGfxDriver->getAdapterDisplayMode(0);
        
        xpos = ypos = 0;
        width = currentMode.width;
        height = currentMode.height;
        fullscreen = true;
    }

    _appContext.mustExit = false;
    _appContext.pInput = Ptr<Window::InputManager>(new Window::InputManager());
    _appContext.pWinHandler = Ptr<Window::WinHandler>(new Window::WinHandler(_appContext.pInput));
    _appContext.pWindow = Ptr<Window::Window>(new Window::Window(
            getInstance(),
            L"Heirs of Olympus",
            xpos, ypos,
            width, height,
            fullscreen,
            _appContext.pWinHandler));

    _appContext.pInput->initialise(_appContext.pWindow->getHandle());

    Renderer::RendererSettings rs(Renderer::GLOBAL_VERY_HIGH);
    rs.fullscreen = FULLSCREEN;
    rs.width = _appContext.pWindow->getWidth();
    rs.height = _appContext.pWindow->getHeight();

    _appContext.pRenderer = Ptr<Renderer::IRenderer>(new Renderer::RendererSM2(
            _appContext.pGfxDriver,
            rs,
            _appContext.pWindow->getHandle(),
            _appContext.pVFS,
            L"/Shaders"));

    if(!_appContext.pRenderer->initialise())
    {
        ERR << L"Renderer initialisation failed\n";
        return false;
    }

    Audio::initVorbisFileDll();

    _appContext.pAudioDriver = Ptr<Audio::IDriver>(new AudioOpenAL::DriverOpenAL());
    _appContext.pAudioDevice = _appContext.pAudioDriver->createDevice(0);

    _appContext.pMasterResPool = Ptr<Universe::RessourcePool>(new Universe::RessourcePool(_appContext.pVFS, _appContext.pRenderer));

    _appContext.pGUI = Ptr<GUI::Manager>(new GUI::Manager(_appContext.pVFS, 
                                                          _appContext.pWindow,
                                                          _appContext.pInput,
                                                          _appContext.pRenderer,
                                                          _appContext.pMasterResPool));

    _appContext.pWidgets = Ptr<WidgetFactory>(new WidgetFactory(_appContext.pGUI, _appContext.pInput, _appContext.pDico));

    _appContext.pWorldGUI = Ptr<Universe::World>(new Universe::World(_appContext.pRenderer, 
                                                                     _appContext.pVFS, 
                                                                     _appContext.pMasterResPool, 
                                                                     _appContext.pAudioDevice));

   




    _appContext.pConnexion = Ptr<Connexion>(new Connexion());

    try
    {
        //Ptr<Core::InputStream> pInput = _appContext.pVFS->readFile(L"/HOO/Levels/RoleplayTemple/RoleplayTemple.world");
        Ptr<Core::InputStream> pInput = _appContext.pVFS->readFile(L"/HOO/Levels/ViaOlympia/ViaOlympia-title.world");

        if(pInput == null)
            throw Core::Exception(L"file not found");

        Core::XMLDocument xmlDocument;
        xmlDocument.loadDocument(*pInput);
        _appContext.pWorldGUI->importWorldXML(xmlDocument);
        _appContext.pOutGameMusic = Ptr<OutGameMusic>(new OutGameMusic(_appContext.pWorldGUI, L"/HOO/Levels/OutGameMusic.ogg"));
        _appContext.pOutGameMusic->setGain(0.2f);
    }
    catch(Core::Exception & e)
    {
        ERR << L"Error loading GUI level : " << e.getMessage() << L"\n";
        return false;
    }

#ifdef LM_DEVMODE
    _pDebugHUD = _appContext.pRenderer->createHUD();
    _pDebugText = _pDebugHUD->createText(_appContext.pRenderer->createFont(L"Consolas", 8), L"<debug label>");
    _pDebugText->setVisible(false);
    _pDebugText->setColor(Core::Vector4f(1.0f, 0.0f, 1.0f, 1.0f));
    _pDebugRectUpdate = _pDebugHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pDebugRectRender = _pDebugHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pDebugRectFlip = _pDebugHUD->createRectangle(Renderer::PictureInfos::PLAIN_COLOR);
    _pDebugRectUpdate->setColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    _pDebugRectRender->setColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    _pDebugRectFlip->setColor(Core::Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
#endif

    // Init states

    _states[APPSTATE_OUTGAME]       = Ptr<IAppState>(new StateOutGame(_appContext));
    _states[APPSTATE_INGAME]        = Ptr<IAppState>(new StateInGame(_appContext));

    setState(_appContext.nextState = APPSTATE_OUTGAME);

    return true;
}
//-----------------------------------------------------------------------------
void AppHOOClient::release()
{
    _appContext.pConnexion->release();
}
//-----------------------------------------------------------------------------
bool AppHOOClient::finished()
{ 
    return _appContext.mustExit; 
}
//-----------------------------------------------------------------------------
void AppHOOClient::update(double elapsed)
{
#ifdef LM_DEVMODE
    Core::StopWatch sw;
    _debugFPS.nextFrame();

    if(_appContext.pInput->isKeyTyped(VK_F11))
        _pDebugText->setVisible(!_pDebugText->isVisible());

    _pDebugRectUpdate->setVisible(_pDebugText->isVisible());
    _pDebugRectRender->setVisible(_pDebugText->isVisible());
    _pDebugRectFlip->setVisible(_pDebugText->isVisible());

    if(_pDebugText->isVisible())
    {
        String debugText(_debugFPS.getText());
        debugText << L"\n" << _appContext.pRenderer->getRenderStats() << L"\n";
        _pDebugText->setText(debugText);

        double _debugTime = 1.0 / 50.0;//_debugTimeUpdate + _debugTimeRender + _debugTimeFlip;

        if(_debugTime > 0.0)
        {
            float ratioUpdate = float(_debugTimeUpdate / _debugTime);
            float ratioRender = float(_debugTimeRender / _debugTime);
            float ratioFlip = float(_debugTimeFlip / _debugTime);

            static const float TOTAL_WIDTH  = 600.0f;
            static const float TOTAL_HEIGHT = 16.0f;
            static const float START_Y      = 550.0f;

            _pDebugRectUpdate->setPosition(Core::Vector2f(0.0f, START_Y));
            _pDebugRectUpdate->setSize(Core::Vector2f(ratioUpdate * TOTAL_WIDTH, TOTAL_HEIGHT));

            _pDebugRectRender->setPosition(Core::Vector2f(_pDebugRectUpdate->getSize().x, START_Y));
            _pDebugRectRender->setSize(Core::Vector2f(ratioRender * TOTAL_WIDTH, TOTAL_HEIGHT));

            _pDebugRectFlip->setPosition(Core::Vector2f(_pDebugRectRender->getPosition().x + _pDebugRectRender->getSize().x, START_Y));
            _pDebugRectFlip->setSize(Core::Vector2f(ratioFlip * TOTAL_WIDTH, TOTAL_HEIGHT));
        }

    }
        
#endif

    _appContext.pConnexion->processMessages();
    _pState->update(elapsed);
    setState(_appContext.nextState);

#ifdef LM_DEVMODE
    _debugTimeUpdate = sw.elapsed().ftotalSeconds();
#endif
}
//-----------------------------------------------------------------------------
void AppHOOClient::setState(EAppState state)
{
    if(_pState == null || state != _pState->getState())
    {
        if(_pState != null)
            _pState->endState();

        if(int32(state) < 0 || int32(state) >= EAppState_COUNT)
            throw Core::Exception(L"undefined app state");

        _pState = _states[int32(state)];
        _pState->startState();
    }
}
//-----------------------------------------------------------------------------
void AppHOOClient::render()
{
#ifdef LM_DEVMODE
    Core::StopWatch sw1;
#endif

    Ptr<Renderer::IRenderView> pView(_appContext.pRenderer->getDefaultView());
    pView->resize(_appContext.pWinHandler->getWindowWidth(), _appContext.pWinHandler->getWindowHeight());

    _pState->render();

#ifdef LM_DEVMODE
    _debugTimeRender = sw1.elapsed().ftotalSeconds();
    _appContext.pRenderer->renderHUD(pView, _pDebugHUD);
    Core::StopWatch sw2;
#endif

    pView->present();

#ifdef LM_DEVMODE
    _debugTimeFlip = sw2.elapsed().ftotalSeconds();
#endif
}
//-----------------------------------------------------------------------------
