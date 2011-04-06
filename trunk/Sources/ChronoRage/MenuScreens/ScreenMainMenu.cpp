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
#include "ScreenMainMenu.h"

#include <GameTools.h>

namespace ChronoRage
{

LM_ENUM_13(EMainMenuWidgetID,   NEW_GAME, 
                                CONTINUE, 
                                TIME_TRIAL,
                                PLAY_DEMO,
                                BUY_GAME,
                                HOW_TO_PLAY,
                                SURVIVAL, 
                                ACHIEVEMENTS, 
                                SETTINGS, 
                                CREDITS, 
                                EXIT, 
                                EXIT_OK, 
                                EXIT_CANCEL);

ScreenMainMenu::ScreenMainMenu(AppContext & appContext) 
:   _appContext(appContext),
    _fadeTimer(0.0f),
    _pLastKeyFocus(NULL)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    static const int32 Y_PADDING = 26;
    int padding = 2;
#ifndef CHRONORAGE_DEMO
    _pButtonNewGame         = _appContext.pWidgetFactory->createMediumButton(L"new-game", 49, padding * Y_PADDING);     padding+=2;
    _pButtonContinue        = _appContext.pWidgetFactory->createMediumButton(L"continue", 49, padding * Y_PADDING);     padding+=2;
    _pButtonTimeTrial       = _appContext.pWidgetFactory->createMediumButton(L"time-trial", 49, padding * Y_PADDING);   padding+=2;
    _pButtonSurvival        = _appContext.pWidgetFactory->createMediumButton(L"survival", 49, padding * Y_PADDING);     padding+=2;
    padding++;
    _pButtonAchievements    = _appContext.pWidgetFactory->createMediumButton(L"achievements", 49, padding * Y_PADDING); padding+=2;
#else
    padding++;
    _pButtonPlayDemo        = _appContext.pWidgetFactory->createMediumButton(L"play-demo", 49, padding * Y_PADDING);    padding+=2;
    padding+=2;
    _pButtonBuyGame         = _appContext.pWidgetFactory->createMediumButton(L"buy-game", 49, padding * Y_PADDING);     padding+=2;
    padding+=3;
#endif
    _pButtonHowToPlay       = _appContext.pWidgetFactory->createMediumButton(L"howtoplay", 49, padding * Y_PADDING);    padding+=2;
    _pButtonSettings        = _appContext.pWidgetFactory->createMediumButton(L"settings", 49, padding * Y_PADDING);     padding+=2;
    _pButtonCredits         = _appContext.pWidgetFactory->createMediumButton(L"credits", 49, padding * Y_PADDING);      padding+=2;
#ifndef CHRONORAGE_DEMO
    padding++;
#else
    padding+=2;
#endif
    _pButtonExit            = _appContext.pWidgetFactory->createMediumButton(L"exit", 49, padding * Y_PADDING);         padding+=2;

#ifndef CHRONORAGE_DEMO
    _pButtonContinue->setEnabled(GameTools::isLevelValid(_appContext.pGameSettings->getCurrentLevel()));
    _pButtonTimeTrial->setEnabled(_appContext.pHighScores->isLevelAvailable(0));

    _pButtonNewGame->setId(NEW_GAME);
    _pButtonContinue->setId(CONTINUE);
    _pButtonTimeTrial->setId(TIME_TRIAL);
    _pButtonSurvival->setId(SURVIVAL);
    _pButtonAchievements->setId(ACHIEVEMENTS);
#else
    _pButtonPlayDemo->setId(PLAY_DEMO);
    _pButtonBuyGame->setId(BUY_GAME);
#endif

    _pButtonHowToPlay->setId(HOW_TO_PLAY);
    _pButtonSettings->setId(SETTINGS);
    _pButtonCredits->setId(CREDITS);
    _pButtonExit->setId(EXIT);

    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);

#ifndef CHRONORAGE_DEMO
    _pPanelButtons->addChild(_pButtonNewGame);
    _pPanelButtons->addChild(_pButtonContinue);
    _pPanelButtons->addChild(_pButtonTimeTrial);
    _pPanelButtons->addChild(_pButtonSurvival);
    _pPanelButtons->addChild(_pButtonAchievements);
#else
    _pPanelButtons->addChild(_pButtonPlayDemo);
    _pPanelButtons->addChild(_pButtonBuyGame);
#endif

    _pPanelButtons->addChild(_pButtonHowToPlay);
    _pPanelButtons->addChild(_pButtonSettings);
    _pPanelButtons->addChild(_pButtonCredits);
    _pPanelButtons->addChild(_pButtonExit);
    _pPanelButtons->pack();

    _appContext.pWidgetFactory->bottomScreen(_pPanelButtons);

    // Key nav
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->ignoreMouse = true;

#ifndef CHRONORAGE_DEMO
    _pLastKeyFocus = _pButtonNewGame.get();
    // end key nav
#else
    _pLastKeyFocus = _pButtonPlayDemo.get();
#endif

#ifndef CHRONORAGE_DEMO
    _pButtonNewGame->addEventHandler(this);
    _pButtonContinue->addEventHandler(this);
    _pButtonTimeTrial->addEventHandler(this);
    _pButtonSurvival->addEventHandler(this);
    _pButtonAchievements->addEventHandler(this);
#else
    _pButtonPlayDemo->addEventHandler(this);
    _pButtonBuyGame->addEventHandler(this);
#endif

    _pButtonHowToPlay->addEventHandler(this);
    _pButtonSettings->addEventHandler(this);
    _pButtonCredits->addEventHandler(this);
    _pButtonExit->addEventHandler(this);
    _pButtonExit->setHotKey(VK_ESCAPE);

#ifndef CHRONORAGE_DEMO
    _pVersionLabel = _appContext.pWidgetFactory->createMediumLabel(L"version", 10, _appContext.pRenderer->getDefaultView()->getHeight()-40);
    _pVersionLabel->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
#else
    _pVersionLabel = _appContext.pWidgetFactory->createMediumLabel(L"version", 10, _appContext.pRenderer->getDefaultView()->getHeight()-40);
    _pVersionLabel->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
#endif

    _pDialog->addChild(_pPanelButtons);
    _pGlobalDialog = Ptr<GUI::Widget>(new GUI::Widget());
    _pGlobalDialog->addChild(_pDialog);
    _pGlobalDialog->addChild(_pVersionLabel);

    // Setup de la boite modale avant de quitter
    {
        _pQuitDialog = _appContext.pWidgetFactory->createDialogBase();

        _pQuitPanel = _appContext.pWidgetFactory->createThinPanel(0, 0);
        
        _pQuitLabel = _appContext.pWidgetFactory->createMediumLabel(L"sure-exit", 0, 20);
        _pQuitLabel->setHLayout(GUI::HL_CENTER);
        Core::Vector2f panelSize = _pQuitPanel->getSize();
        panelSize.y = 110.0f;
        _pQuitLabel->setSize(panelSize);
        
        _pQuitButtonOK      = _appContext.pWidgetFactory->createSmallButton(L"ok", 100, 110);
        _pQuitButtonCancel  = _appContext.pWidgetFactory->createSmallButton(L"cancel", 353, 110);
        _pQuitButtonCancel->setHotKey(VK_ESCAPE);

        _pQuitButtonOK->setKeyWidgetR(_pQuitButtonCancel.get());
        _pQuitButtonCancel->setKeyWidgetL(_pQuitButtonOK.get());
        
        _pQuitButtonOK->setId(EXIT_OK);
        _pQuitButtonCancel->setId(EXIT_CANCEL);

        _pQuitPanel->addChild(_pQuitLabel);
        _pQuitPanel->addChild(_pQuitButtonOK);
        _pQuitPanel->addChild(_pQuitButtonCancel);
        _pQuitPanel->pack();

        _appContext.pWidgetFactory->centerScreen(_pQuitPanel);

        _pQuitDialog->addChild(_pQuitPanel);
        _pQuitDialog->pack();

        _pQuitButtonOK->addEventHandler(this);
        _pQuitButtonCancel->addEventHandler(this);
    }
}

void ScreenMainMenu::resetLabels()
{
#ifndef CHRONORAGE_DEMO
    _pButtonNewGame->setDictionary(_appContext.pDico);
    _pButtonContinue->setDictionary(_appContext.pDico);
    _pButtonTimeTrial->setDictionary(_appContext.pDico);
    _pButtonSurvival->setDictionary(_appContext.pDico);
    _pButtonAchievements->setDictionary(_appContext.pDico);
#else
    _pButtonPlayDemo->setDictionary(_appContext.pDico);
    _pButtonBuyGame->setDictionary(_appContext.pDico);
#endif
    _pButtonHowToPlay->setDictionary(_appContext.pDico);
    _pButtonSettings->setDictionary(_appContext.pDico);
    _pButtonCredits->setDictionary(_appContext.pDico);
    _pButtonExit->setDictionary(_appContext.pDico);
    _pQuitLabel->setDictionary(_appContext.pDico);
    _pQuitButtonOK->setDictionary(_appContext.pDico);
    _pQuitButtonCancel->setDictionary(_appContext.pDico);
    _pVersionLabel->setDictionary(_appContext.pDico);

#ifndef CHRONORAGE_DEMO
    _pButtonNewGame->setText(L"new-game");
    _pButtonContinue->setText(L"continue");
    _pButtonTimeTrial->setText(L"time-trial");
    _pButtonSurvival->setText(L"survival");
    _pButtonAchievements->setText(L"achievements");
#else
    _pButtonPlayDemo->setText(L"play-demo");
    _pButtonBuyGame->setText(L"buy-game");
#endif
    _pButtonHowToPlay->setText(L"howtoplay");
    _pButtonSettings->setText(L"settings");
    _pButtonCredits->setText(L"credits");
    _pButtonExit->setText(L"exit");
    _pQuitLabel->setText(L"sure-exit");
    _pQuitButtonOK->setText(L"ok");
    _pQuitButtonCancel->setText(L"cancel");
    _pVersionLabel->setText(L"version");
}

void ScreenMainMenu::startScreen()
{
#ifndef CHRONORAGE_DEMO
    // Key nav
    _pButtonNewGame->setKeyWidgetU(_pButtonExit.get());
    _pButtonNewGame->setKeyWidgetD(_pButtonContinue.get());

    _pButtonContinue->setKeyWidgetU(_pButtonNewGame.get());
    _pButtonContinue->setKeyWidgetD(_pButtonTimeTrial.get());

    _pButtonTimeTrial->setKeyWidgetU(_pButtonContinue.get());
    _pButtonTimeTrial->setKeyWidgetD(_pButtonSurvival.get());

    _pButtonSurvival->setKeyWidgetU(_pButtonTimeTrial.get());
    _pButtonSurvival->setKeyWidgetD(_pButtonAchievements.get());

    _pButtonAchievements->setKeyWidgetU(_pButtonSurvival.get());
    _pButtonAchievements->setKeyWidgetD(_pButtonHowToPlay.get());

    _pButtonHowToPlay->setKeyWidgetU(_pButtonAchievements.get());
#else
    _pButtonPlayDemo->setKeyWidgetU(_pButtonExit.get());
    _pButtonPlayDemo->setKeyWidgetD(_pButtonBuyGame.get());

    _pButtonBuyGame->setKeyWidgetU(_pButtonPlayDemo.get());
    _pButtonBuyGame->setKeyWidgetD(_pButtonHowToPlay.get());

    _pButtonHowToPlay->setKeyWidgetU(_pButtonBuyGame.get());
#endif

    _pButtonHowToPlay->setKeyWidgetD(_pButtonSettings.get());

    _pButtonSettings->setKeyWidgetU(_pButtonHowToPlay.get());
    _pButtonSettings->setKeyWidgetD(_pButtonCredits.get());

    _pButtonCredits->setKeyWidgetU(_pButtonSettings.get());
    _pButtonCredits->setKeyWidgetD(_pButtonExit.get());

    _pButtonExit->setKeyWidgetU(_pButtonCredits.get());

#ifndef CHRONORAGE_DEMO
    _pButtonExit->setKeyWidgetD(_pButtonNewGame.get());
#else
    _pButtonExit->setKeyWidgetD(_pButtonPlayDemo.get());
#endif
    // end key nav

    _appContext.pWidgetFactory->getManager()->setDialog(_pGlobalDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
    
    _pGlobalDialog->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pGlobalDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), _fadeTimer);

    _pDialog->pack();
    _pGlobalDialog->pack();
}
    
void ScreenMainMenu::endScreen()
{
}

void ScreenMainMenu::update(double elapsed)
{
    _pVersionLabel->setPosition(Core::Vector2f(10.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-40.0f));
}

void ScreenMainMenu::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        _pDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        _fadeTimer = 0.0f;

        EMainMenuWidgetID widgetId = (EMainMenuWidgetID)pWidget->getId();
        switch(widgetId)
        {
#ifndef CHRONORAGE_DEMO
        case NEW_GAME:
            LM_ASSERT(_pScreenTutorial != null);
            _pManager->setScreen(_pScreenTutorial.get());
            _pLastKeyFocus = _pButtonNewGame.get();
            break;
        case CONTINUE:
            {
                if(_appContext.pGameSettings->getCurrentLevel() == -1)
                {
                    _appContext.wantedMode = CREDITS_MODE;
                }
                else
                {
                    _appContext.wantedMode = CONTINUE_GAME_MODE;
                }
                _appContext.wantedLevel = _appContext.pGameSettings->getCurrentLevel();
                _pLastKeyFocus = _pButtonContinue.get();
            }
            break;
        case TIME_TRIAL:
            LM_ASSERT(_pScreenTimeTrialMode != null);
            _pManager->setScreen(_pScreenTimeTrialMode.get());
            _pLastKeyFocus = _pButtonTimeTrial.get();
            break;
        case SURVIVAL:
            LM_ASSERT(_pScreenSurvivalMode != null);
            _pManager->setScreen(_pScreenSurvivalMode.get());
            _pLastKeyFocus = _pButtonSurvival.get();
            break;
        case ACHIEVEMENTS:
            LM_ASSERT(_pScreenAchievements != null);
            _pManager->setScreen(_pScreenAchievements.get());
            _pLastKeyFocus = _pButtonAchievements.get();
            break;
#else
        case PLAY_DEMO:
            LM_ASSERT(_pScreenTutorial != null);
            _pManager->setScreen(_pScreenTutorial.get());
            _pLastKeyFocus = _pButtonPlayDemo.get();
            break;
        case BUY_GAME:
            _pLastKeyFocus = _pButtonBuyGame.get();
            Core::System::launchDefaultInternetNavigator(L"http://www.chronorage.com/");
            break;
#endif
        case HOW_TO_PLAY:
            LM_ASSERT(_pScreenSettings != null);
            _pManager->setScreen(_pScreenHowToPlay.get());
            _pLastKeyFocus = _pButtonHowToPlay.get();
            break;
        case SETTINGS:
            LM_ASSERT(_pScreenSettings != null);
            _pManager->setScreen(_pScreenSettings.get());
            _pLastKeyFocus = _pButtonSettings.get();
            break;
        case CREDITS:
            LM_ASSERT(_pScreenCredits != null);
            _pManager->setScreen(_pScreenCredits.get());
            _pLastKeyFocus = _pButtonCredits.get();
            break;
        case EXIT:
            _appContext.pWidgetFactory->getManager()->setDialog(_pQuitDialog);
            _pLastKeyFocus = _pButtonExit.get();
            _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pQuitButtonCancel.get();
            break;
        case EXIT_OK:
            _appContext.exitState = PLAY_EXIT_ANIM;
            _appContext.pGUI->popDialog();
            break;
        case EXIT_CANCEL:
            _pGlobalDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
            _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
            break;
        }
    }
}

}//namespace ChronoRage
