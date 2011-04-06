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
#ifndef CHRONORAGE_MENUMODE_H_
#define CHRONORAGE_MENUMODE_H_

#include <Modes/IGameState.h>
#include <Vortex.h>
#include <MenuScreens/IScreen.h>
#include <MenuScreens/ScreenMainMenu.h>
#include <MenuScreens/ScreenTimeTrialMode.h>
#include <MenuScreens/ScreenSurvivalMode.h>
#include <MenuScreens/ScreenSettings.h>
#include <MenuScreens/ScreenVideoSettings.h>
#include <MenuScreens/ScreenAudioSettings.h>
#include <MenuScreens/ScreenControlsSettings.h>
#include <MenuScreens/ScreenMouseKeyboardSettings.h>
#include <MenuScreens/ScreenKeyboardOnlySettings.h>
#include <MenuScreens/ScreenJoystickSettings.h>
#include <MenuScreens/ScreenCredits.h>
#include <MenuScreens/ScreenHelp.h>
#include <MenuScreens/ScreenAchievements.h>
#ifdef CHRONORAGE_DEMO
#include <MenuScreens/ScreenBuyGame.h>
#endif

#include <MenuScreens/ScreenSelectInput.h>

namespace ChronoRage
{

LM_ENUM_5(ETitleState,
          TITLE_UNINITIALIZED,
          TITLE_CREATION_IN_PROGRESS,
          TITLE_ACTIVE,
          TITLE_DESTRUCTION_IN_PROGRESS,
          TITLE_DELETE_PENDING);

class LM_API_CHR MenuMode : public IGameState, public IScreenManager
{
public:
    MenuMode(AppContext & appContext);

    virtual ~MenuMode();

    inline virtual EGameStateMode getMode() const { return MENU_MODE; }

    virtual void initialize();
    
    virtual void initVortex();
    virtual void initGUI();

    virtual void update(double elapsed);

    virtual void setScreen(IScreen * pScreen);

    void resetScreenLabels();

#if LM_DISPLAY_FPS
    virtual int32 getFoesNumber(){ return 0;  }
    virtual float getTimeWarpNumber(){ return 0.0f;  }
    virtual float getPowerLevel(){ return 0.0f;  }


    virtual void powerUp(){}
    virtual void powerDown(){}
    virtual void createFoe(int32 foeId){}
#endif

protected:
    virtual void updateCreation(double elapsed);
    virtual void updateActive(double elapsed);
    virtual void updateDestruction(double elapsed);

protected:
    Ptr<Vortex>     _pVortex;

    ETitleState         _state;
    Ptr<Universe::Node> _pNodeChronoRage;
    Ptr<Universe::Node> _pSubNode;
    Ptr<Universe::Node> _pSourceNode;
    
    Core::List<Ptr<VoxelSprite> >   _sprites;
    Ptr<Universe::NodeDecal>        _pRageDecal;
    int32                           _iNextLetter;
    float                           _timeForNextLetter;

    IScreen *           _pCurrentScreen;
    Core::CameraQuake   _quakeCam;

    Ptr<ScreenMainMenu>         _pScreenMainMenu;
#ifndef CHRONORAGE_DEMO
    Ptr<ScreenTimeTrialMode>    _pScreenTimeTrialMode;
    Ptr<ScreenSurvivalMode>     _pScreenSurvivalMode;
    Ptr<ScreenAchievements>     _pScreenAchievements;
#endif
    Ptr<ScreenSettings>         _pScreenSettings;
    Ptr<ScreenVideoSettings>    _pScreenVideoSettings;
    Ptr<ScreenAudioSettings>    _pScreenAudioSettings;
    Ptr<ScreenControlsSettings> _pScreenControlsSettings;
    Ptr<ScreenMouseKeyboardSettings>    _pScreenMouseKeyboardSettings;
    Ptr<ScreenKeyboardOnlySettings>     _pScreenKeyboardOnlySettings;
    Ptr<ScreenJoystickSettings>         _pScreenJoystickSettings;
    Ptr<ScreenCredits>                  _pScreenCredits;
    Ptr<ScreenHelp>                     _pScreenHelp;
    Ptr<ScreenHelp>                     _pScreenTutorial;
#ifdef CHRONORAGE_DEMO
    Ptr<ScreenBuyGame>                  _pScreenBuyGame;
#endif

    Ptr<ScreenSelectInput>      _pScreenSelectInput;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODE_H_*/