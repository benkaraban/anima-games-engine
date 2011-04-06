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
#ifndef CHRONORAGE_GUI_SCREENMAINMENU_H_
#define CHRONORAGE_GUI_SCREENMAINMENU_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

class ScreenMainMenu : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenMainMenu(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    void fadeInScreen(float fadeTimer) { _fadeTimer = fadeTimer; }

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setTimeTrialScreen(const Ptr<IScreen> & pScreen) { _pScreenTimeTrialMode = pScreen; }
    void setSurvivalScreen(const Ptr<IScreen> & pScreen) { _pScreenSurvivalMode = pScreen; }
    void setSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenSettings = pScreen; }
    void setHowToPlayScreen(const Ptr<IScreen> & pScreen) { _pScreenHowToPlay = pScreen; }
    void setTutorialScreen(const Ptr<IScreen> & pScreen) { _pScreenTutorial = pScreen; }
    void setCreditsScreen(const Ptr<IScreen> & pScreen) { _pScreenCredits = pScreen; }
    void setAchievementsScreen(const Ptr<IScreen> & pScreen) { _pScreenAchievements = pScreen; }

protected:
    AppContext &            _appContext;
    
    float                   _fadeTimer;

    Ptr<GUI::Widget>        _pGlobalDialog;
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pPanelButtons;
#ifndef CHRONORAGE_DEMO
    Ptr<GUI::Button>        _pButtonNewGame;
    Ptr<GUI::Button>        _pButtonContinue;
    Ptr<GUI::Button>        _pButtonTimeTrial;
    Ptr<GUI::Button>        _pButtonSurvival;

    Ptr<GUI::Button>        _pButtonAchievements;
#else
    Ptr<GUI::Button>        _pButtonPlayDemo;
    Ptr<GUI::Button>        _pButtonBuyGame;
#endif

    Ptr<GUI::Button>        _pButtonHowToPlay;
    Ptr<GUI::Button>        _pButtonSettings;
    Ptr<GUI::Button>        _pButtonCredits;
    Ptr<GUI::Button>        _pButtonExit;

    Ptr<GUI::Widget>        _pQuitDialog;
    Ptr<GUI::Widget>        _pQuitPanel;
    Ptr<GUI::Label>         _pQuitLabel;

    Ptr<GUI::Button>        _pQuitButtonOK;
    Ptr<GUI::Button>        _pQuitButtonCancel;

    Ptr<GUI::Label>         _pVersionLabel;

    Ptr<IScreen>            _pScreenTimeTrialMode;
    Ptr<IScreen>            _pScreenSurvivalMode;
    Ptr<IScreen>            _pScreenHowToPlay;
    Ptr<IScreen>            _pScreenSettings;
    Ptr<IScreen>            _pScreenTutorial;
    Ptr<IScreen>            _pScreenCredits;
    Ptr<IScreen>            _pScreenAchievements;

    GUI::Widget *           _pLastKeyFocus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_SCREENMAINMENU_H_*/
