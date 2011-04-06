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
#ifndef CHRONORAGE_IGAMESTATE_H_
#define CHRONORAGE_IGAMESTATE_H_

#include <Core/VFS/VFSDirectMapping.h>
#include <Core/Math/CameraController.h>

#include <Gfx/IDriver.h>
#include <Gfx/IDevice.h>
#include <Gfx/GfxDx9/DriverDx9.h>

#include <Renderer/SM2/RendererSM2.h>

#include <Audio/IDriver.h>
#include <Audio/IDevice.h>

#include <Window/Application.h>
#include <Window/InputManager.h>
#include <Window/WinHandler.h>

#include <GUI/Manager.h>

#include <Universe/World.h>

#include <VoxelFont.h>
#include <GameSettings.h>
#include <HighScores.h>
#include <LevelRanks.h>
#include <SoundLibrary.h>
#include <MenuScreens/WidgetFactory.h>
#include <MenuScreens/IScreen.h>

#ifdef LM_DEVMODE
#define LM_DISPLAY_FPS 1
#else
#define LM_DISPLAY_FPS 0
#endif

namespace ChronoRage
{
LM_ENUM_11(EGameStateMode,  START_MODE,
                            MENU_MODE, 
                            DEMO_MODE,
                            NEW_GAME_MODE,
                            CONTINUE_GAME_MODE,
                            TIME_TRIAL_MODE,
                            SURVIVAL_MODE,
                            SURVIVAL_HARD_MODE,
                            DODGE_MODE,
                            REVENGE_MODE,
                            CREDITS_MODE);

LM_ENUM_3(EExitState,   DO_NOT_EXIT,
                        PLAY_EXIT_ANIM,
                        MUST_EXIT_NOW);

struct LM_API_CHR AppContext
{
    Ptr<Core::VirtualFileSystem> pVFS;

    Ptr<GameSettings>           pGameSettings;
    Ptr<HighScores>             pHighScores;
    Ptr<LevelRanks>             pLevelRanks;

    Ptr<Gfx::IDriver>           pDriver;
    
    Ptr<Window::Window>         pRenderWindow;
    Ptr<Window::WinHandler>     pWinHandler;
    Ptr<Window::InputManager>   pInputManager;

    Ptr<Renderer::IRenderer>    pRenderer;
    
    Ptr<Audio::IDriver>         pAudioDriver;
    Ptr<Audio::IDevice>         pAudioDevice;

    Ptr<Universe::World>                    pWorld;
    Ptr<Universe::Node>                     pCameraPivot;
    Ptr<Universe::NodePerspectiveCamera>    pCameraNode;

    Ptr<SoundLibrary>           pSoundLibrary;
    Ptr<Universe::World>        pSoundWorld;
    Ptr<Universe::NodeSoundSource> pMenuMusic;
    Ptr<Universe::NodeSoundSource> pGameMusic;

    Ptr<GUI::Manager>           pGUI;
    Ptr<WidgetFactory>          pWidgetFactory;
    Ptr<Core::Bitmap>           pCursorArrow;
    Ptr<Core::Bitmap>           pCursorReticle;
    Ptr<Core::Bitmap>           pCursorBlank;
    
    Ptr<VoxelFont>              pVoxelFont1;
    Ptr<VoxelFont>              pVoxelFont2;
    Ptr<VoxelFont>              pVoxelCreditFont;

    bool                        reloadDictionary;
    Ptr<Core::Dictionary>       pDico;

    EGameStateMode              wantedMode;
    int32                       wantedLevel;
    EScreenType                 wantedScreen;
    EExitState                  exitState;
    bool                        saveScores;
    bool                        fastTitle;
#ifdef CHRONORAGE_DEMO
    int32                       shipDestroyed;
#endif
};

class LM_API_CHR IGameState
{
public:
    IGameState(AppContext & appContext);
    virtual ~IGameState() {};

    virtual EGameStateMode getMode() const = 0;

    virtual void initialize() = 0;

    virtual void update(double elapsed) = 0;

    virtual void render(const Ptr<Renderer::IRenderView> & pView);
    virtual bool initCamera();

    virtual Ptr<Universe::NodeSoundSource> playSound(ESoundCode soundCode, bool loop);

    AppContext & getContext() { return _appContext; }


#if LM_DISPLAY_FPS
    virtual int32 getFoesNumber() = 0;
    virtual float getTimeWarpNumber() = 0;
    virtual float getPowerLevel() = 0;


    virtual void powerUp() = 0;
    virtual void powerDown() = 0;
    virtual void createFoe(int32 foeId) = 0;
#endif

protected:
    AppContext & _appContext;
};

}//namespace ChronoRage

#endif /* CHRONORAGE_IGAMESTATE_H_ */
