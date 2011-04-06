/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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
#include "ChronoRage.h"

#include <Core/TGA.h>
#include <Core/System/System.h>
#include <Core/VFS/VFSXMLTranslator.h>
#include <Audio/AudioGlobals.h>
#include <Audio/AudioOpenAL/DriverOpenAL.h>

#include <GameSettings.h>
#include <Modes/GameModeDemo.h>
#include <Modes/GameModeLevels.h>
#include <Modes/GameModeTimeTrial.h>
#include <Modes/GameModeSurvival.h>
#include <Modes/GameModeDodge.h>
#include <Modes/GameModeRevenge.h>
#include <Modes/GameModeCredits.h>
#include <Modes/MenuMode.h>
#include <Modes/StartMode.h>
#include <Modes/GameMode1.h>

namespace ChronoRage
{
const bool TEST_MODE = false;

const String CHRONORAGE_GAME_SETTINGS_FILE = L"/User/Settings.xml";
const String CHRONORAGE_HIGH_SCORES_FILE = L"/User/HighScores.xml";
const String CHRONORAGE_LEVEL_RANKS_FILE = L"/ChronoRageLevels/level-ranks.xml";
const String CHRONORAGE_SURVIVAL_WAVE_FILE = L"/ChronoRageLevels/Waves/waves-survival.xml";
const String CHRONORAGE_DEMO_WAVE_FILE = L"/ChronoRageLevels/Waves/waves-demo.xml";
const String CHRONORAGE_DODGE_WAVE_FILE = L"/ChronoRageLevels/Waves/waves-dodge.xml";

ChronoRage::ChronoRage()
:   _pGameState(null)
{
#ifdef LM_DEVMODE
    _showRenderStats = false;
#endif
}

ChronoRage::~ChronoRage()
{
#if LM_DISPLAY_FPS
    _pTextFPS = null;
    _pTextFoesNumber = null;
    _pTextPowerLevel = null;
    _pTextTimeWarpNumber = null;
    _pFontFPS = null;
    _pHUD = null;
#endif

    _pGameState = null;

    ///
    _appContext.pSoundLibrary = null;
    _appContext.pMenuMusic = null;
    _appContext.pGameMusic = null;
    _appContext.pSoundWorld = null;
       
    _appContext.pAudioDevice = null;
    _appContext.pAudioDriver = null;

    ///
    _appContext.pCameraPivot = null;
    _appContext.pCameraNode = null;
    _appContext.pWorld = null;

    ///
    _appContext.pVoxelFont1 = null;
    _appContext.pVoxelFont2 = null;
    _appContext.pVoxelCreditFont = null;

    ///
    _appContext.pCursorArrow = null;
    _appContext.pCursorReticle = null;
    _appContext.pWidgetFactory = null;
    
    if(_appContext.pGUI != null)
    {
        _appContext.pGUI->reset();
        _appContext.pGUI = null;
    }
    
    _appContext.pRenderer = null;
    _appContext.pDriver = null;

    _appContext.pWinHandler = null;
    _appContext.pInputManager = null;
    _appContext.pRenderWindow = null;
    _appContext.pVFS = null;

    _appContext.pGameSettings = null;
    _appContext.pHighScores = null;
    _appContext.pLevelRanks = null;

    _appContext.pDico = null;
}

bool ChronoRage::initialise()
{
#ifdef LM_DEVMODE
    Core::String workDir(LM_WORKING_DIRECTORY);
    SetCurrentDirectory(Core::String8(workDir).c_str());
#endif

    INF << L"---------------------------------------------------------\n";
    INF << L"System informations\n";
    INF << L"---------------------------------------------------------\n";
    INF << L"OS Version    : " << Core::System::getOSName() << L"\n";
    INF << L"CPU type      : " << Core::System::getCPUName() << L"\n";
    INF << L"CPU count     : " << Core::toString(Core::System::getCPUCount()) << L"\n";
    INF << L"Physical RAM  : " << Core::toStringByteSize(Core::System::getTotalPhysicalRAM()) << L"\n";
    INF << L"File swap RAM : " << Core::toStringByteSize(Core::System::getTotalFileSwapRAM()) << L"\n";
    INF << L"Virtual RAM   : " << Core::toStringByteSize(Core::System::getTotalVirtualRAM()) << L"\n";
    INF << L"Current dir   : " << Core::System::getCurrentDir() << L"\n";
    INF << L"---------------------------------------------------------\n";
    INF << L"Virtual file system layout\n";
    INF << L"---------------------------------------------------------\n";

#ifdef LM_DEVMODE
    // Initialisation VFS ---------------------------------------------
    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./Data/VFS-ChronoRage.xml"))
        {
            _appContext.pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << _appContext.pVFS->toString() << "\n";
        }
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }
#else
    // Initialisation VFS ---------------------------------------------
    {
        Ptr<Core::XMLDocument> pXmlDocument(new Core::XMLDocument());

        if(pXmlDocument->loadDocument(L"./VFS-ChronoRage.xml"))
        {
            _appContext.pVFS = Core::VFSFromXMLNode(pXmlDocument->firstChildNode(L"vfs"));
            INF << _appContext.pVFS->toString() << "\n";
        }
        else
            throw Core::Exception(L"Unable to mount VFS.");

    }
#endif

    // Dico load file
    const Core::String DICO_FILE = L"/ChronoRageDictionaries/main.xml";
    Ptr<Core::InputStream> pInputDic = _appContext.pVFS->readFile(DICO_FILE);
    if(pInputDic == null)
    {
        Core::String message;
        message << L"Unable to open dictionary file : " << DICO_FILE;
        throw Core::Exception(message);
    }

    _appContext.pDriver = Gfx::IDriverPtr(new Gfx::DriverDx9());
    Gfx::DisplayMode displayMode = _appContext.pDriver->getAdapterDisplayMode(0);

    //Game Settings

    bool specialFx = false;
    Renderer::RendererSettings settings;
    Renderer::ConfigProfile profile;
    profile.msaaLevel = Renderer::MSAA_4X;
    profile.minWidth = 1024;
    profile.minHeight = 720;
    profile.textureVRAMRatio = 0.7f;

    // Auto config details
    const Gfx::GraphicCapabilities & caps = _appContext.pDriver->getGraphicCaps(0);

    if(int32(caps.psVersion) >= int32(Gfx::PS_V3_0))
    {
        if(caps.videoMem >= (400 << 20))
        {
            INF << L"Special effects should be turned *on* by default, MSAA 4X by default\n";
            specialFx = true;
            profile.msaaLevel = Renderer::MSAA_4X;
        }
        else if(caps.videoMem >= (200 << 20))
        {
            INF << L"Special effects should be turned *on* by default, MSAA NONE by default\n";
            specialFx = true;
            profile.msaaLevel = Renderer::MSAA_NONE;
        }
        else
        {
            INF << L"Special effects should be turned *off* by default, MSAA NONE by default\n";
            specialFx = false;
            profile.msaaLevel = Renderer::MSAA_NONE;
        }
    }
    else if(int32(caps.psVersion) >= int32(Gfx::PS_V2_0))
    {
        INF << L"Special effects should be turned *off* by default, MSAA NONE by default\n";
        specialFx = false;
        profile.msaaLevel = Renderer::MSAA_NONE;
    }
    else
    {
        ERR << L"Video card does not support PS V2.0, exiting game with an error message\n";
        Ptr<Core::Dictionary> pDico(new Core::Dictionary(Core::System::getSystemSupportedLanguage(), *pInputDic));
        throw Core::Exception((*pDico)[L"bad-gpu"]);
    }

    Renderer::RendererSM2::autoConfig(_appContext.pDriver, 0, settings, profile);

    Ptr<GameSettings> pGameSettings(new GameSettings(settings.width, settings.height, settings.msaaLevel));
    pGameSettings->enableSpecialEffects(specialFx);
    _appContext.pGameSettings = pGameSettings;

    Ptr<Core::InputStream> pStreamSettingsFile = _appContext.pVFS->readFile(CHRONORAGE_GAME_SETTINGS_FILE);
    if(pStreamSettingsFile != null)
    {
        Ptr<Core::XMLDocument> pXmlGameSettings(new Core::XMLDocument());
        if(pXmlGameSettings->loadDocument(*pStreamSettingsFile))        
            _appContext.pGameSettings->importXML(pXmlGameSettings);
        INF << L"Configuration loaded from file\n";
    }
    else
    {
        Ptr<Core::OutputStream> pSettingsOutput(_appContext.pVFS->writeFile(CHRONORAGE_GAME_SETTINGS_FILE));
        if(pSettingsOutput != null)
        {
            Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());
            _appContext.pGameSettings->exportXML(pXMLDoc);
            pXMLDoc->saveDocument(*pSettingsOutput);
            pSettingsOutput->close();
        }
        INF << L"No configuration file found, using autoconfig setup\n";
    }

    //Ranks
    Ptr<LevelRanks> pLevelRanks(new LevelRanks());
    _appContext.pLevelRanks = pLevelRanks;
    Ptr<Core::InputStream> pStreamLevelRanksFile = _appContext.pVFS->readFile(CHRONORAGE_LEVEL_RANKS_FILE);
    if(pStreamLevelRanksFile != null)
    {
        Ptr<Core::XMLDocument> pXmlLevelRanks(new Core::XMLDocument());
        if(pXmlLevelRanks->loadDocument(*pStreamLevelRanksFile))        
            _appContext.pLevelRanks->importXML(pXmlLevelRanks);
    }

    //High Scores
#ifndef CHRONORAGE_DEMO
    Ptr<HighScores> pHighScores(new HighScores(pLevelRanks));
    _appContext.pHighScores = pHighScores;
    Ptr<Core::InputStream> pStreamHighScoresFile = _appContext.pVFS->readFile(CHRONORAGE_HIGH_SCORES_FILE);
    if(pStreamHighScoresFile != null)
    {
        Ptr<Core::XMLDocument> pXmlHighScores(new Core::XMLDocument());
        if(pXmlHighScores->loadDocument(*pStreamHighScoresFile))
            _appContext.pHighScores->importXML(pXmlHighScores);
    }
    else
    {
        Ptr<Core::OutputStream> pHighScoresOutput(_appContext.pVFS->writeFile(CHRONORAGE_HIGH_SCORES_FILE));
        if(pHighScoresOutput != null)
        {
            Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());
            _appContext.pHighScores->exportXML(pXMLDoc);
            pXMLDoc->saveDocument(*pHighScoresOutput);
            pHighScoresOutput->close();
        }
    }
#else
    _appContext.pHighScores = null;
#endif

    _appContext.exitState = DO_NOT_EXIT;
    _appContext.saveScores = false;
    _appContext.fastTitle = false;

    HICON hMyIcon = LoadIcon(getInstance(), "IDI_ICON1");

    _appContext.pInputManager = Ptr<Window::InputManager>(new Window::InputManager());
    _appContext.pWinHandler = Ptr<Window::WinHandler>(new Window::WinHandler(_appContext.pInputManager));
    _appContext.pRenderWindow = Ptr<Window::Window>(new Window::Window(
            getInstance(),
            L"Chrono Rage",
            0, 0,    // position
            _appContext.pGameSettings->getScreenWidth(),    // width
            _appContext.pGameSettings->getScreenHeight(),   // height
            _appContext.pGameSettings->fullscreen(),        // fullscreen
            _appContext.pWinHandler,
            hMyIcon));

    _appContext.pInputManager->initialise(_appContext.pRenderWindow->getHandle());

    // Pour Èviter de rÈfÈrencer un joystick qui n'est plus branchÈ
    _appContext.pGameSettings->cleanupSettings(*_appContext.pInputManager);

    _appContext.pRenderer = Ptr<Renderer::IRenderer>(new Renderer::RendererSM2(
            _appContext.pDriver,
            _appContext.pGameSettings->getRendererSettings(),
            _appContext.pRenderWindow->getHandle(),
            _appContext.pVFS,
            L"/Shaders"));

    Audio::initVorbisFileDll();

    _appContext.pAudioDriver = Ptr<Audio::IDriver>(new AudioOpenAL::DriverOpenAL());
    _appContext.pAudioDevice = _appContext.pAudioDriver->createDevice(0);
    _appContext.pSoundLibrary = Ptr<SoundLibrary>(new SoundLibrary(_appContext));

    bool result = _appContext.pRenderer->initialise();

    _appContext.pWorld = Ptr<Universe::World>(new Universe::World(_appContext.pRenderer, _appContext.pVFS, null, null));

    _appContext.pSoundWorld = Ptr<Universe::World>(new Universe::World(_appContext.pRenderer, _appContext.pVFS, null, _appContext.pAudioDevice));
    _appContext.pMenuMusic = null;
    _appContext.pGameMusic = null;
    Ptr<Universe::NodeListener> pNodeListener = _appContext.pSoundWorld->getNodeListener();
    if(pNodeListener != null)
        pNodeListener->setGain(1.0f);

    _appContext.pGUI = Ptr<GUI::Manager>(new GUI::Manager(_appContext.pVFS, _appContext.pRenderWindow, _appContext.pInputManager, _appContext.pRenderer, _appContext.pWorld->getRessourcesPool()));
    _appContext.pGUI->getResPool()->mountPictureBank(L"/ChronoRageData/GUI", L"/ChronoRageData/GUI.pbk");
#ifdef CHRONORAGE_DEMO
    _appContext.pGUI->getResPool()->mountPictureBank(L"/ChronoRageData/DemoGUI", L"/ChronoRageData/DemoGUI.pbk");
#endif

    _appContext.pCursorArrow = Core::loadTGA(*_appContext.pVFS->readFile(L"/ChronoRageData/arrow.tga"));
    _appContext.pCursorReticle = Core::loadTGA(*_appContext.pVFS->readFile(L"/ChronoRageData/reticle.tga"));
    _appContext.pCursorBlank = Core::loadTGA(*_appContext.pVFS->readFile(L"/ChronoRageData/blank.tga"));
   
    _appContext.reloadDictionary = false;
    _appContext.pDico = Ptr<Core::Dictionary>(new Core::Dictionary(pGameSettings->getLanguage(), *pInputDic));

    _appContext.pWidgetFactory = Ptr<WidgetFactory>(new WidgetFactory(  _appContext.pGUI, 
                                                                        _appContext.pInputManager, 
                                                                        _appContext.pDico, 
                                                                        _appContext.pSoundLibrary));

    //_appContext.wantedMode = _currentMode = MENU_MODE;
    _appContext.wantedMode = _currentMode = START_MODE;
    _appContext.wantedScreen = MAIN_MENU_SCREEN;
    
    if(result)
    {
        if(TEST_MODE)
        {
            Ptr<GameModeTimeTrial> pGameModeTimeTrial(new GameModeTimeTrial(_appContext));
            _pGameState = pGameModeTimeTrial;
            _pGameState->initialize();
            pGameModeTimeTrial->setLevel(19);
            /*Ptr<GameModeDemo> pGameModeDemo(new GameModeDemo(_appContext));
            _pGameState = pGameModeDemo;
            _pGameState->initialize();
            pGameModeDemo->registerWaveFile(CHRONORAGE_DEMO_WAVE_FILE);*/
        }
        else
            initGameState();

    }

#if LM_DISPLAY_FPS
    // Compteur FPS
    _pHUD = _appContext.pRenderer->createHUD();
    _pFontFPS = _appContext.pRenderer->createFont(L"Arial", 10);
    _pTextFPS = _pHUD->createText(_pFontFPS, L"");
    _pTextFPS->setColor(Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
    _pTextFPS->setPosition(Core::Vector2f(5.0f, 5.0f));

    _pTextFoesNumber = _pHUD->createText(_pFontFPS, L"");
    _pTextFoesNumber->setColor(Core::Vector4f(1.0f, 0.7f, 0.7f, 1.0f));
    _pTextFoesNumber->setPosition(Core::Vector2f(100.0f, 5.0f));

    _pTextTimeWarpNumber = _pHUD->createText(_pFontFPS, L"");
    _pTextTimeWarpNumber->setColor(Core::Vector4f(0.1f, 1.0f, 1.0f, 1.0f));
    _pTextTimeWarpNumber->setPosition(Core::Vector2f(5.0f, 40.0f));

    _pTextPowerLevel = _pHUD->createText(_pFontFPS, L"");
    _pTextPowerLevel->setColor(Core::Vector4f(0.7f, 0.1f, 1.0f, 1.0f));
    _pTextPowerLevel->setPosition(Core::Vector2f(5.0f, 60.0f));
#endif

    return result;
}

void ChronoRage::run()
{
    try
    {
        Application::run();
    }
    catch(Gfx::GfxException & exception)
    {
        ERR << L"Fatal GFX exception caught : " << exception.getMessage() << L"\n";
#ifdef _DEBUG
        ERR << exception.getCallStack() << L"\n";
#endif
        String error(L"A fatal error occurred in display module");
        if(_appContext.pDico != null)
            error = (*_appContext.pDico)[L"gfx-error"];
        throw Core::Exception(error);
    }
    catch(Renderer::RendererException & exception)
    {
        ERR << L"Fatal GFX exception caught : " << exception.getMessage() << L"\n";
#ifdef _DEBUG
        ERR << exception.getCallStack() << L"\n";
#endif
        String error(L"A fatal error occurred in display module");
        if(_appContext.pDico != null)
            error = (*_appContext.pDico)[L"gfx-error"];
        throw Core::Exception(error);
    }
}

void ChronoRage::release()
{
    //Sauvegarde settings
    Ptr<Core::OutputStream> pSettingsOutput(_appContext.pVFS->writeFile(CHRONORAGE_GAME_SETTINGS_FILE));
    if(pSettingsOutput != null)
    {
        Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());
        _appContext.pGameSettings->exportXML(pXMLDoc);
        pXMLDoc->saveDocument(*pSettingsOutput);
        pSettingsOutput->close();
    }
#ifndef CHRONORAGE_DEMO
    //Sauvegarde High Scores
    Ptr<Core::OutputStream> pHighScoresOutput(_appContext.pVFS->writeFile(CHRONORAGE_HIGH_SCORES_FILE));
    if(pHighScoresOutput != null)
    {
        Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());
        _appContext.pHighScores->exportXML(pXMLDoc);
        pXMLDoc->saveDocument(*pHighScoresOutput);
        pHighScoresOutput->close();
    }
#endif
       

    if(_appContext.pGameMusic != null)
    {
        _appContext.pGameMusic->kill();
        _appContext.pGameMusic = null;
    }

    if(_appContext.pMenuMusic != null)
    {
        _appContext.pMenuMusic->kill();
        _appContext.pMenuMusic = null;
    }

    Audio::shutdownVorbisFileDll();
}

void ChronoRage::update(double elapsed)
{
    if(_appContext.reloadDictionary)
    {
        const Core::String DICO_FILE = L"/ChronoRageDictionaries/main.xml";
        Ptr<Core::InputStream> pInputDic = _appContext.pVFS->readFile(DICO_FILE);
        if(pInputDic == null)
        {
            Core::String message;
            message << L"Unable to open dictionary file : " << DICO_FILE;
            throw Core::Exception(message);
        }
        
        _appContext.reloadDictionary = false;
        _appContext.pDico = Ptr<Core::Dictionary>(new Core::Dictionary(_appContext.pGameSettings->getLanguage(), *pInputDic));

        _appContext.pWidgetFactory = Ptr<WidgetFactory>(new WidgetFactory(  _appContext.pGUI, 
                                                                            _appContext.pInputManager, 
                                                                            _appContext.pDico, 
                                                                            _appContext.pSoundLibrary));

        if(_pGameState->getMode() == MENU_MODE)
        {
            Ptr<MenuMode> pMenuMode = LM_DEBUG_PTR_CAST<MenuMode>(_pGameState);
            pMenuMode->resetScreenLabels();
        }

        _appContext.reloadDictionary = false;
    }

    if(_appContext.wantedMode != _currentMode)
    {
        _currentMode = _appContext.wantedMode;
        initGameState();
    }

#ifndef CHRONORAGE_DEMO
    if(_appContext.saveScores)
    {
        //Sauvegarde High Scores
        Ptr<Core::OutputStream> pHighScoresOutput(_appContext.pVFS->writeFile(CHRONORAGE_HIGH_SCORES_FILE));
        if(pHighScoresOutput != null)
        {
            Ptr<Core::XMLDocument> pXMLDoc(new Core::XMLDocument());
            _appContext.pHighScores->exportXML(pXMLDoc);
            pXMLDoc->saveDocument(*pHighScoresOutput);
            pHighScoresOutput->close();
        }
        _appContext.saveScores = false;
    }
#endif

    if(_appContext.exitState == MUST_EXIT_NOW)
    {
        PostQuitMessage(0);
    }

    LM_ASSERT(_appContext.pSoundWorld != null);
    _appContext.pSoundWorld->update(elapsed);

    _pGameState->update(elapsed);

#ifdef LM_DEVMODE
    if(_appContext.pInputManager->isKeyTyped(VK_ADD))
        _pGameState->powerUp();
    if(_appContext.pInputManager->isKeyTyped(VK_SUBTRACT))
        _pGameState->powerDown();

    int32 foeId = -1;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD0))
        foeId = 0;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD1))
        foeId = 1;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD2))
        foeId = 2;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD3))
        foeId = 3;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD4))
        foeId = 4;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD5))
        foeId = 5;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD6))
        foeId = 6;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD7))
        foeId = 7;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD8))
        foeId = 8;
    if(_appContext.pInputManager->isKeyTyped(VK_NUMPAD9))
        foeId = 9;

    if(foeId >= 0)
        _pGameState->createFoe(foeId);

    if(_appContext.pInputManager->isKeyTyped(VK_F11))
        _showRenderStats = !_showRenderStats;
    if(_appContext.pInputManager->isKeyTyped(VK_F12))
        Core::Allocator::getInstance().dumpMemoryUsage();

#endif/*LM_DEVMODE*/
}

void ChronoRage::render()
{
    Ptr<Renderer::IRenderView> pView(_appContext.pRenderer->getDefaultView());
    pView->resize(_appContext.pWinHandler->getWindowWidth(), _appContext.pWinHandler->getWindowHeight());

    _pGameState->render(pView);
#if LM_DISPLAY_FPS
    _fps.nextFrame();
    if(_showRenderStats)
        _pTextFPS->setText(_fps.getText() + L"\n\n\n\n\n" + _appContext.pRenderer->getRenderStats());
    else
        _pTextFPS->setText(_fps.getText());

    Core::String foesNumber = L"Foes number : ";
    foesNumber << _pGameState->getFoesNumber()
               << L"   Nodes count : "
               << Core::toString(_appContext.pWorld->getAllNodes().size());
    _pTextFoesNumber->setText(foesNumber);

    Core::String timeWarpNumber = L"TimeWarp number : ";
    timeWarpNumber << Core::toString(_pGameState->getTimeWarpNumber(), 2);
    _pTextTimeWarpNumber->setText(timeWarpNumber);

    Core::String powerLevel = L"Player Power Level : ";
    powerLevel << Core::toString(_pGameState->getPowerLevel(), 2);
    _pTextPowerLevel->setText(powerLevel);

    //_appContext.pRenderer->renderHUD(pView, _pHUD);
#endif
    pView->present();
}

void ChronoRage::initGameState()
{
    try
    {
        switch(_currentMode)
        {
        case START_MODE:
            _pGameState = Ptr<StartMode>(new StartMode(_appContext));
            _pGameState->initialize();
            break;
        case MENU_MODE:
            _pGameState = Ptr<MenuMode>(new MenuMode(_appContext));
            _pGameState->initialize();
            break;
#ifndef CHRONORAGE_DEMO
        case NEW_GAME_MODE:
            {
                Ptr<GameModeLevels> pGameModeLevels(new GameModeLevels(_appContext));
                _pGameState = pGameModeLevels;
                _pGameState->initialize();
                pGameModeLevels->setLevel(1);
            }
            break;
        case CONTINUE_GAME_MODE:
            {
                Ptr<GameModeLevels> pGameModeLevels(new GameModeLevels(_appContext));
                _pGameState = pGameModeLevels;
                _pGameState->initialize();
                pGameModeLevels->setLevel(_appContext.wantedLevel);
            }
            break;
        case TIME_TRIAL_MODE:
            {
                Ptr<GameModeTimeTrial> pGameModeTimeTrial(new GameModeTimeTrial(_appContext));
                _pGameState = pGameModeTimeTrial;
                _pGameState->initialize();
                pGameModeTimeTrial->setLevel(_appContext.wantedLevel);
            }
            break;
        case SURVIVAL_MODE:
            {
                Ptr<GameModeSurvival> pGameModeSurvival(new GameModeSurvival(_appContext, false));
                _pGameState = pGameModeSurvival;
                _pGameState->initialize();
                pGameModeSurvival->registerWaveFile(CHRONORAGE_SURVIVAL_WAVE_FILE);
            }
            break;
        case SURVIVAL_HARD_MODE:
            {
                Ptr<GameModeSurvival> pGameModeSurvival(new GameModeSurvival(_appContext, true));
                _pGameState = pGameModeSurvival;
                _pGameState->initialize();
                pGameModeSurvival->registerWaveFile(CHRONORAGE_SURVIVAL_WAVE_FILE);
            }
            break;
        case DODGE_MODE:
            {
                Ptr<GameModeDodge> pGameModeDodge(new GameModeDodge(_appContext));
                _pGameState = pGameModeDodge;
                _pGameState->initialize();
                pGameModeDodge->registerWaveFile(CHRONORAGE_DODGE_WAVE_FILE);
            }
            break;
        case REVENGE_MODE:
            {
                Ptr<GameModeRevenge> pGameModeRevenge(new GameModeRevenge(_appContext));
                _pGameState = pGameModeRevenge;
                _pGameState->initialize();
                pGameModeRevenge->registerWaveFile(CHRONORAGE_SURVIVAL_WAVE_FILE);
            }
            break;
#endif
        case DEMO_MODE:
            {
                Ptr<GameModeDemo> pGameModeDemo(new GameModeDemo(_appContext));
                _pGameState = pGameModeDemo;
                _pGameState->initialize();
                pGameModeDemo->registerWaveFile(CHRONORAGE_DEMO_WAVE_FILE);
            }
            break;
#ifndef CHRONORAGE_DEMO
        case CREDITS_MODE:
            {
                Ptr<GameModeCredits> pGameModeCredits(new GameModeCredits(_appContext));
                _pGameState = pGameModeCredits;
                _pGameState->initialize();
                pGameModeCredits->setLevel(_appContext.wantedLevel);
            }
            break;
#endif
        }
    }
    catch(Core::Exception & e)
    {
        Core::System::errorMessageBox(e.getMessage());
        ERR << L"Error initialising world : " << e.getMessage() << L"\n";
        ERR << e.getCallStack();
    }
}

}//namespace ChronoRage