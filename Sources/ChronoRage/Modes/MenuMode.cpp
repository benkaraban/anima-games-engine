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
#include "MenuMode.h"

#include <Core/TGA.h>
#include <Universe/Tasks/Fade.h>
#include <Window/Tools.h>

namespace ChronoRage
{

#define QUICK_TITLE     0
#define USE_VORTEX      1
#define TEXT_MODE       0

const Core::String MENUMODE_WORLD_FILE = L"/ChronoRageLevels/menu-world.world";

#if TEXT_MODE
static const String TITLE_TEXT(L"BEND TIME");

static const float CREATE_TITLE_WAIT_TIMER  = 2.0f;

static const float TIME_BETWEEN_LETTERS     = 0.05f;
static const float LETTER_ARRIVAL_MIN_TIME  = 0.5f;
static const float LETTER_ARRIVAL_MAX_TIME  = 0.75f;

static const float TIME_BETWEEN_LETTERS_FAST    = 0.03f;
static const float LETTER_ARRIVAL_MIN_TIME_FAST = 0.3f;
static const float LETTER_ARRIVAL_MAX_TIME_FAST = 0.5f;

#else
static const String TITLE_TEXT(L"CHRONO");

static const float CREATE_TITLE_WAIT_TIMER  = 0.5f;

static const float TIME_BETWEEN_LETTERS     = 0.15f;
static const float LETTER_ARRIVAL_MIN_TIME  = 1.0f;
static const float LETTER_ARRIVAL_MAX_TIME  = 1.5f;

static const float TIME_BETWEEN_LETTERS_FAST    = 0.03f;
static const float LETTER_ARRIVAL_MIN_TIME_FAST = 0.3f;
static const float LETTER_ARRIVAL_MAX_TIME_FAST = 0.5f;
#endif


MenuMode::MenuMode(AppContext & appContext)
:   IGameState(appContext),
    _pCurrentScreen(NULL),
    _state(TITLE_UNINITIALIZED)
{
}

MenuMode::~MenuMode()
{
    if(_pScreenMainMenu != null)
    {
        _pScreenMainMenu->setTimeTrialScreen(null);
        _pScreenMainMenu->setSurvivalScreen(null);
        _pScreenMainMenu->setSettingsScreen(null);
        _pScreenMainMenu->setHowToPlayScreen(null);
        _pScreenMainMenu->setTutorialScreen(null);
        _pScreenMainMenu->setAchievementsScreen(null);
    }

    if(_pScreenHelp != null)
        _pScreenHelp->setNextScreen(null);


#ifndef CHRONORAGE_DEMO
    if(_pScreenTimeTrialMode != null)
        _pScreenTimeTrialMode->setMainMenuScreen(null);

    if(_pScreenSurvivalMode != null)
        _pScreenSurvivalMode->setMainMenuScreen(null);

    if(_pScreenAchievements != null)
        _pScreenAchievements->setNextScreen(null);

#else
    if(_pScreenBuyGame != null)
        _pScreenBuyGame->setNextScreen(null);
#endif

    if(_pScreenSettings != null)
    {
        _pScreenSettings->setMainMenuScreen(null);
        _pScreenSettings->setVideoSettingsScreen(null);
        _pScreenSettings->setAudioSettingsScreen(null);
        _pScreenSettings->setControlsSettingsScreen(null);
    }

    if(_pScreenVideoSettings != null)
        _pScreenVideoSettings->setSettingsScreen(null);

    if(_pScreenAudioSettings != null)
        _pScreenAudioSettings->setSettingsScreen(null);

    if(_pScreenControlsSettings != null)
    {
        _pScreenControlsSettings->setSettingsScreen(null);
        _pScreenControlsSettings->setMouseKeyboardSettingsScreen(null);
        _pScreenControlsSettings->setKeyboardOnlySettingsScreen(null);
        _pScreenControlsSettings->setJoystickSettingsScreen(null);
    }

    if(_pScreenMouseKeyboardSettings != null)
    {
        _pScreenMouseKeyboardSettings->setControlsSettingsScreen(null);
        _pScreenMouseKeyboardSettings->setSelectInputScreen(null);
    }

    if(_pScreenKeyboardOnlySettings != null)
    {
        _pScreenKeyboardOnlySettings->setControlsSettingsScreen(null);
        _pScreenKeyboardOnlySettings->setSelectInputScreen(null);
    }

    if(_pScreenJoystickSettings != null)
    {
        _pScreenJoystickSettings->setControlsSettingsScreen(null);
        _pScreenJoystickSettings->setSelectInputScreen(null);
    }

    if(_pScreenCredits != null)
        _pScreenCredits->setMainMenuScreen(null);
}

void MenuMode::initialize()
{
    _appContext.pGUI->reset();

    Ptr<Core::InputStream> pStreamWorldFile = _appContext.pVFS->readFile(MENUMODE_WORLD_FILE);
    if(pStreamWorldFile == null)
    {
        Core::String message;
        message << L"Failed to load GameModeLevels World : "<<MENUMODE_WORLD_FILE;
        throw Core::Exception(message);
    }
    
    Ptr<Core::XMLDocument> pXmlWorld(new Core::XMLDocument());
    pXmlWorld->loadDocument(*pStreamWorldFile);
    
    _appContext.pWorld->importWorldXML(*pXmlWorld);
    
    if(!initCamera())
    {
        Core::String message;
        message << L"Failed to get camera in GameModeLevels World : "<<MENUMODE_WORLD_FILE;
        throw Core::Exception(message);
    }

    if(_appContext.pVoxelFont1 == null)
    {
        VoxelFontInfos vfi;

        //// Version console
        vfi.pBitmap = Core::loadTGA(*_appContext.pVFS->readFile(L"/ChronoRageData/Font/title-font.tga"));
        
        vfi.charWidth = 8;
        vfi.charHeight = 12;
        vfi.charSpacing = 8;

        vfi.uRes = 75;
        vfi.vRes = 1;

        vfi.characters = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789:! ";

        vfi.cubeSize = 1.0f;
        vfi.padding = 0.1f;
        vfi.glowFactor = 0.2f;

        _appContext.pVoxelFont1 = Ptr<VoxelFont>(new VoxelFont(vfi));
        
        vfi.cubeSize = 1.5f;
        vfi.padding = 0.15f;

        _appContext.pVoxelFont2 = Ptr<VoxelFont>(new VoxelFont(vfi));
    }

    _pSourceNode = _appContext.pWorld->createNode();
    _pSourceNode->setLocalPosition(Core::Vector3f(0.0f, -8.0f, -5.0f));

    _appContext.pCameraPivot->setLocalPosition(Core::Vector3f(-160.5f, -28.3f, 31.55f));
    _appContext.pCameraPivot->setLocalOrientation(Core::Quaternionf(-0.102f, 0.360f, 0.040f, 0.926f).getNormalizedSafe());

    _pNodeChronoRage = _appContext.pWorld->createNode();
    _appContext.pCameraPivot->addChild(_pNodeChronoRage);
    _appContext.pCameraPivot->addChild(_pSourceNode);

    _pNodeChronoRage->localPitch(-d_PI_DIV_2);
    _pNodeChronoRage->setLocalPosition(Core::Vector3f(0.0f, 35.0f, 120.0f));

    _pRageDecal = _appContext.pWorld->createDecal(L"/ChronoRageData/title-rage.tex", Renderer::DECAL_LERP);
    _pNodeChronoRage->addChild(_pRageDecal);
    _pRageDecal->localPitch(d_PI_DIV_2);
    _pRageDecal->setLocalPosition(Core::Vector3f(25.0f, 5.0f, 0.0f));
    _pRageDecal->setSize(60.0f, 30.0f);
    _pRageDecal->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));
    _pNodeChronoRage->removeChild(_pRageDecal);

    _pSubNode = _appContext.pWorld->createNode();
    _pNodeChronoRage->addChild(_pSubNode);

    _iNextLetter = 0;
    if(_appContext.fastTitle)
        _timeForNextLetter = 0.0f;
    else 
        _timeForNextLetter = 1.0f;

    if(_appContext.pGameMusic != null)
    {
        _appContext.pGameMusic->killAtEnd(true);
        _appContext.pGameMusic->fadeOut(1.0f);
    }

    if(_appContext.pMenuMusic != null && _appContext.pMenuMusic->isAlive())
    {
        _appContext.pMenuMusic->kill();
        _appContext.pMenuMusic = null;
    }

    _appContext.pSoundLibrary->playMenuMusic();

    initGUI();
    initVortex();

#if QUICK_TITLE
    _state = TITLE_ACTIVE;
    setScreen(_pScreenBuyGame.get());
#else
    _state = TITLE_CREATION_IN_PROGRESS;
#endif
}

void MenuMode::initVortex()
{
    VortexSettings settings;
    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.8f;

    settings.armCount        = 6;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = true;

    settings.color0          = Core::Vector4f(0.2f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.0f, 0.1f, 0.3f, 1.0f),
    settings.colorByDepth    = true;

    settings.cubeSelfRotationSpeed = 0.2f;

    settings.start          = 10.0f;
    settings.startWidth     = 160.0f;

    settings.nearU          = 0.0f;
    settings.sharpness      = 1.0f;
    settings.sharpness2     = 1.0f;

    settings.near           = 20.0f;
    settings.nearMinWidth   = 10.0f;
    settings.nearMaxWidth   = 160.0f;

    settings.far            = 550.0f;
    settings.farMinWidth    = 10.0f;
    settings.farMaxWidth    = 160.0f;

    settings.spiralCount    = 1.0f;//5.0f;

    settings.suckingSpeed   = 0.01f;

#if USE_VORTEX
    _pVortex = Ptr<Vortex>(new Vortex(_appContext.pWorld, settings, null, false));
    _pVortex->update(0.0);
#endif
}

void MenuMode::initGUI()
{
    _pScreenMainMenu        = Ptr<ScreenMainMenu>(new ScreenMainMenu(_appContext));
#ifndef CHRONORAGE_DEMO
    _pScreenTimeTrialMode   = Ptr<ScreenTimeTrialMode>(new ScreenTimeTrialMode(_appContext));
    _pScreenSurvivalMode    = Ptr<ScreenSurvivalMode>(new ScreenSurvivalMode(_appContext));
    _pScreenAchievements    = Ptr<ScreenAchievements>(new ScreenAchievements(_appContext));
#endif
    _pScreenSettings        = Ptr<ScreenSettings>(new ScreenSettings(_appContext));
    _pScreenCredits         = Ptr<ScreenCredits>(new ScreenCredits(_appContext));
    _pScreenVideoSettings   = Ptr<ScreenVideoSettings>(new ScreenVideoSettings(_appContext));
    _pScreenAudioSettings   = Ptr<ScreenAudioSettings>(new ScreenAudioSettings(_appContext));
    _pScreenControlsSettings= Ptr<ScreenControlsSettings>(new ScreenControlsSettings(_appContext));
    _pScreenMouseKeyboardSettings = Ptr<ScreenMouseKeyboardSettings>(new ScreenMouseKeyboardSettings(_appContext));
    _pScreenKeyboardOnlySettings = Ptr<ScreenKeyboardOnlySettings>(new ScreenKeyboardOnlySettings(_appContext));
    _pScreenJoystickSettings = Ptr<ScreenJoystickSettings>(new ScreenJoystickSettings(_appContext));
    _pScreenHelp             = Ptr<ScreenHelp>(new ScreenHelp(_appContext));
    _pScreenTutorial         = Ptr<ScreenHelp>(new ScreenHelp(_appContext));
    _pScreenSelectInput      = Ptr<ScreenSelectInput>(new ScreenSelectInput(_appContext));
#ifdef CHRONORAGE_DEMO
    _pScreenBuyGame          = Ptr<ScreenBuyGame>(new ScreenBuyGame(_appContext));
#endif

#ifndef CHRONORAGE_DEMO
    _pScreenMainMenu->setTimeTrialScreen(_pScreenTimeTrialMode);
    _pScreenMainMenu->setSurvivalScreen(_pScreenSurvivalMode);
    _pScreenMainMenu->setAchievementsScreen(_pScreenAchievements);
#endif
    _pScreenMainMenu->setSettingsScreen(_pScreenSettings);
    _pScreenMainMenu->setHowToPlayScreen(_pScreenHelp);
    _pScreenMainMenu->setTutorialScreen(_pScreenTutorial);
    _pScreenMainMenu->setCreditsScreen(_pScreenCredits);

#ifndef CHRONORAGE_DEMO
    _pScreenTimeTrialMode->setMainMenuScreen(_pScreenMainMenu);

    _pScreenSurvivalMode->setMainMenuScreen(_pScreenMainMenu);

    _pScreenAchievements->setNextScreen(_pScreenMainMenu);
#endif

    _pScreenSettings->setMainMenuScreen(_pScreenMainMenu);
    _pScreenSettings->setVideoSettingsScreen(_pScreenVideoSettings);
    _pScreenSettings->setAudioSettingsScreen(_pScreenAudioSettings);
    _pScreenSettings->setControlsSettingsScreen(_pScreenControlsSettings);

    _pScreenCredits->setMainMenuScreen(_pScreenMainMenu);

    _pScreenVideoSettings->setSettingsScreen(_pScreenSettings);
    _pScreenAudioSettings->setSettingsScreen(_pScreenSettings);
    _pScreenControlsSettings->setSettingsScreen(_pScreenSettings);
    _pScreenControlsSettings->setMouseKeyboardSettingsScreen(_pScreenMouseKeyboardSettings);
    _pScreenControlsSettings->setKeyboardOnlySettingsScreen(_pScreenKeyboardOnlySettings);
    _pScreenControlsSettings->setJoystickSettingsScreen(_pScreenJoystickSettings);

    _pScreenMouseKeyboardSettings->setControlsSettingsScreen(_pScreenControlsSettings);
    _pScreenMouseKeyboardSettings->setSelectInputScreen(_pScreenSelectInput);

    _pScreenKeyboardOnlySettings->setControlsSettingsScreen(_pScreenControlsSettings);
    _pScreenKeyboardOnlySettings->setSelectInputScreen(_pScreenSelectInput);

    _pScreenJoystickSettings->setControlsSettingsScreen(_pScreenControlsSettings);
    _pScreenJoystickSettings->setSelectInputScreen(_pScreenSelectInput);

    _pScreenHelp->setNextScreen(_pScreenMainMenu);

#ifdef CHRONORAGE_DEMO
    _pScreenBuyGame->setNextScreen(_pScreenMainMenu);
#endif

    _appContext.pGUI->setCursor(_appContext.pCursorArrow, 0, 0);
}

void MenuMode::update(double elapsed)
{
#if 0
    Window::InputManager & input = *_appContext.pInputManager;
    static bool useCam = false;

    if(input.isKeyDown('C'))
    {
        useCam = true;
        _quakeCam.setPosition(_appContext.pCameraPivot->getWorldPosition());
    }

    if(useCam)
    {
        Window::updateQuakeCamera(_quakeCam, input, 1.0, 0.02, elapsed);
        _appContext.pCameraPivot->setLocalMatrix(_quakeCam.getWorldMatrix());
    }
#endif

    _appContext.pWorld->update(elapsed);
    _appContext.pGUI->update(elapsed);
    _appContext.pSoundLibrary->update(elapsed);

    _pVortex->update(elapsed);

    switch(_state)
    {
    case TITLE_CREATION_IN_PROGRESS:
        updateCreation(elapsed);
        break;
    case TITLE_ACTIVE:
        updateActive(elapsed);
        break;
    case TITLE_DESTRUCTION_IN_PROGRESS:
        updateDestruction(elapsed);
        break;
    }

    for(int32 ii=0; ii < _sprites.size(); ii++)
        _sprites[ii]->update(elapsed);

    if(_pCurrentScreen != NULL)
        _pCurrentScreen ->update(elapsed);

    if(_appContext.pGameMusic != null && !_appContext.pGameMusic->isAlive())
    {
        _appContext.pGameMusic = null;
    }

    if(_appContext.pInputManager->isKeyTyped(VK_ESCAPE))
        _appContext.fastTitle = true;
}

void MenuMode::updateCreation(double elapsed)
{
    if(_iNextLetter >= TITLE_TEXT.size() && _sprites[_sprites.size() - 1]->isAnimationFinished())
    {
        for(int32 ii=0; ii < _sprites.size(); ii++)
            _sprites[ii]->blink(0.25f, 1);

#if !TEXT_MODE
        _pRageDecal->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));

        Ptr<Universe::ITask> pTask1(new Universe::TaskFade(_pRageDecal, Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f), 0.25f));
        Ptr<Universe::ITask> pTask2(new Universe::TaskGlowFade2(_pRageDecal, Core::Vector4f(0.0f), Core::Vector4f(1.25f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.4f), 0.25f, 0.5f));
        _appContext.pWorld->registerTask(pTask1);
        _appContext.pWorld->registerTask(pTask2);

        _state = TITLE_ACTIVE;
        if(_appContext.fastTitle)
            _pScreenMainMenu->fadeInScreen(0.3f);
        else 
            _pScreenMainMenu->fadeInScreen(1.0f);
    
        switch(_appContext.wantedScreen)
        {
        default:
        case MAIN_MENU_SCREEN:
            setScreen(_pScreenMainMenu.get());
            break;
#ifndef CHRONORAGE_DEMO
        case TIME_TRIAL_SCREEN:
            setScreen(_pScreenTimeTrialMode.get());
            break;
#else
        case BUY_GAME_SCREEN:
            setScreen(_pScreenBuyGame.get());
        break;
#endif
        }

        _appContext.wantedScreen = MAIN_MENU_SCREEN;
        
#endif
    }
    else
    {
        _timeForNextLetter -= float(elapsed);

        if(_timeForNextLetter <= 0.0f && _iNextLetter < TITLE_TEXT.size())
        {
            Ptr<VoxelSpriteTemplate> pTemplate;

            if(_iNextLetter == 0)
                pTemplate = _appContext.pVoxelFont2->getTemplate(TITLE_TEXT[_iNextLetter]);
            else
                pTemplate = _appContext.pVoxelFont1->getTemplate(TITLE_TEXT[_iNextLetter]);

            if(pTemplate != null)
            {
                Ptr<VoxelSprite> pSprite(new VoxelSprite(_appContext.pWorld, pTemplate, Core::Vector4f::ONE, false));
#if TEXT_MODE
                _pSubNode->setLocalPosition(Core::Vector3f((_iNextLetter - (TITLE_TEXT.size() / 2)) * 8.0f, 0.0f, -35.0f));
                //_pSubNode->setLocalPosition(Core::Vector3f((_iNextLetter - (TITLE_TEXT.length() / 2)) * 8.0f, -35.0f, 0.0f));
#else
                _pSubNode->setLocalPosition(Core::Vector3f((_iNextLetter - 5) * 8.0f, 0.0f, 0.0f));
#endif

                VoxelCloud cloud;
                cloud.position      = _pSubNode->getWorldToLocalMatrix().apply(_pSourceNode->getWorldPosition());
                cloud.cosAlpha      = Core::L_cos(f_PI);
                cloud.alpha         = f_PI;
                cloud.beta          = f_PI;
                cloud.dispRadius    = 4.0f;

                pSprite->setMatrices(_pSubNode->getLocalToWorldMatrix(), _pSubNode->getWorldToLocalMatrix());
                if(_appContext.fastTitle)
                    pSprite->cloudToSprite(cloud, LETTER_ARRIVAL_MIN_TIME_FAST, LETTER_ARRIVAL_MAX_TIME_FAST);
                else
                    pSprite->cloudToSprite(cloud, LETTER_ARRIVAL_MIN_TIME, LETTER_ARRIVAL_MAX_TIME);

                pSprite->setGlowFlag(true);

                _sprites.push_back(pSprite);
            }

            _iNextLetter++;
            if(_appContext.fastTitle)
                _timeForNextLetter = TIME_BETWEEN_LETTERS_FAST;
            else
                _timeForNextLetter = TIME_BETWEEN_LETTERS;
        }
    }
}

void MenuMode::updateActive(double elapsed)
{
    if(!_appContext.fastTitle)
        _appContext.fastTitle = true;

    if(_appContext.exitState == PLAY_EXIT_ANIM)
    {
        _state = TITLE_DESTRUCTION_IN_PROGRESS;
        _timeForNextLetter = 0.0f;
        _iNextLetter = 0;
        if(_appContext.pMenuMusic != null)
        {
            _appContext.pMenuMusic->killAtEnd(true);
            _appContext.pMenuMusic->fadeOut(0.3f);
        }
    }
}

void MenuMode::updateDestruction(double elapsed)
{
    _state = TITLE_DELETE_PENDING;
    _appContext.exitState = MUST_EXIT_NOW;
}

void MenuMode::setScreen(IScreen * pScreen)
{
    if(_pCurrentScreen != NULL)
        _pCurrentScreen->endScreen();

    pScreen->setScreenManager(this);
    pScreen->startScreen();
    _pCurrentScreen = pScreen;
}

void MenuMode::resetScreenLabels()
{
    _pScreenMainMenu->resetLabels();
#ifndef CHRONORAGE_DEMO
    _pScreenTimeTrialMode->resetLabels();
    _pScreenSurvivalMode->resetLabels();
    _pScreenAchievements->resetLabels();
#endif
    _pScreenSettings->resetLabels();
    _pScreenCredits->resetLabels();
    _pScreenVideoSettings->resetLabels();
    _pScreenAudioSettings->resetLabels();
    _pScreenControlsSettings->resetLabels();
    _pScreenMouseKeyboardSettings->resetLabels();
    _pScreenKeyboardOnlySettings->resetLabels();
    _pScreenJoystickSettings->resetLabels();
    _pScreenHelp->resetLabels();
    _pScreenTutorial->resetLabels();
#ifdef CHRONORAGE_DEMO
    _pScreenBuyGame->resetLabels();
#endif
}

}