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
#include "ScreenSurvivalMode.h"

#include <GameTools.h>

namespace ChronoRage
{

ScreenSurvivalMode::ScreenSurvivalMode(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pGlobalPanel = Ptr<GUI::Widget>(new GUI::Widget());
    _pGlobalPanel->setPosition(Core::Vector2f(0, 0));
    
    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);
    _pPanelScores = _appContext.pWidgetFactory->createMediumPanel(500, 0);

    int padding = 1;
    static const int32 Y_PADDING = 53;

    _pLabelSurvival = _appContext.pWidgetFactory->createMediumLabel(L"survival", 0, padding++ * Y_PADDING);
    Core::Vector2f labelSize;
    labelSize = _pLabelSurvival->getSize();
    labelSize.x = _pPanelButtons->getSize().x;
    _pLabelSurvival->setSize(labelSize);
    _pLabelSurvival->setHLayout(GUI::HL_CENTER);

    padding++;
    
    _pButtonNormal = _appContext.pWidgetFactory->createMediumButton(L"normal", 49, padding++ * Y_PADDING);
    _pButtonNormal->setEnabled(_appContext.pHighScores->isSurvivalModeAvailable());
    _pButtonHard   = _appContext.pWidgetFactory->createMediumButton(L"hard", 49, padding++ * Y_PADDING);
    _pButtonHard->setEnabled(_appContext.pHighScores->isSurvivalHardModeAvailable());
    _pButtonDodge  = _appContext.pWidgetFactory->createMediumButton(L"dodge", 49, padding++ * Y_PADDING);
    _pButtonDodge->setEnabled(_appContext.pHighScores->isDodgeModeAvailable());
    _pButtonRevenge= _appContext.pWidgetFactory->createMediumButton(L"revenge", 49, padding++ * Y_PADDING);
    _pButtonRevenge->setEnabled(_appContext.pHighScores->isRevengeModeAvailable());

    _pButtonNormal->addEventHandler(this);
    _pButtonHard->addEventHandler(this);
    _pButtonDodge->addEventHandler(this);
    _pButtonRevenge->addEventHandler(this);

    _pPanelButtons->addChild(_pLabelSurvival);
    _pPanelButtons->addChild(_pButtonNormal);
    _pPanelButtons->addChild(_pButtonHard);
    _pPanelButtons->addChild(_pButtonDodge);
    _pPanelButtons->addChild(_pButtonRevenge);

    padding = 1;
    _pLabelHighScores   = _appContext.pWidgetFactory->createMediumLabel(L"survival-best-times", 0, padding++ * Y_PADDING);
    _pLabelHighScores->setSize(labelSize);
    _pLabelHighScores->setHLayout(GUI::HL_CENTER);
    
    _pPanelScores->addChild(_pLabelHighScores);

    padding++;

    _modeLockedReason = _appContext.pWidgetFactory->createRichTextLabel(L"", 10, (padding+1) * Y_PADDING);
    _modeLockedReason->setSize(labelSize);
    _modeLockedReason->setDictionary(null);
    _modeLockedReason->setText(L"");
    _modeLockedReason->setSize(Core::Vector2f(446.0f, 200.0f));
        
    _pPanelScores->addChild(_modeLockedReason);

    for(int32 iScore = 0; iScore < 5; ++iScore)
    {
        _pLabelScores[iScore] = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING);
        _pLabelScores[iScore]->setHLayout(GUI::HL_CENTER);
        _pLabelScores[iScore]->setSize(labelSize);
        _pLabelScores[iScore]->setDictionary(null);
        _pLabelScores[iScore]->setText(L"");
        
        _pPanelScores->addChild(_pLabelScores[iScore]);
    }

    padding+=2;

    _pButtonBack = _appContext.pWidgetFactory->createMediumButton(L"back", 49, padding++ * Y_PADDING);
    _pButtonBack->setId(-1);
    _pButtonBack->setHotKey(VK_ESCAPE);
    _pButtonBack->addEventHandler(this);

    _pPanelScores->addChild(_pButtonBack);
    _pPanelButtons->pack();
    _pPanelScores->pack();

    _pGlobalPanel->addChild(_pPanelButtons);
    _pGlobalPanel->addChild(_pPanelScores);
    _pGlobalPanel->pack();

    _appContext.pWidgetFactory->bottomScreen(_pGlobalPanel);

    _pDialog->addChild(_pGlobalPanel);

    // Key nav
    _pLastKeyFocus = _pButtonBack.get();
    // end key nav
}

void ScreenSurvivalMode::resetLabels()
{
    _pLabelSurvival->setDictionary(_appContext.pDico);
    _pButtonNormal->setDictionary(_appContext.pDico);
    _pButtonHard->setDictionary(_appContext.pDico);
    _pButtonDodge->setDictionary(_appContext.pDico);
    _pButtonRevenge->setDictionary(_appContext.pDico);
    _pLabelHighScores->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelSurvival->setText(L"survival");
    _pButtonNormal->setText(L"normal");
    _pButtonHard->setText(L"hard");
    _pButtonDodge->setText(L"dodge");
    _pButtonRevenge->setText(L"revenge");
    _pLabelHighScores->setText(L"survival-best-times");
    _pButtonBack->setText(L"back");

    _modeLockedReason->setText(L"");
}

void ScreenSurvivalMode::startScreen()
{
    // Key nav
    _pButtonNormal->setKeyWidgetU(_pButtonBack.get());
    _pButtonNormal->setKeyWidgetD(_pButtonHard.get());
    _pButtonHard->setKeyWidgetU(_pButtonNormal.get());
    _pButtonHard->setKeyWidgetD(_pButtonDodge.get());
    _pButtonDodge->setKeyWidgetU(_pButtonHard.get());
    _pButtonDodge->setKeyWidgetD(_pButtonRevenge.get());
    _pButtonRevenge->setKeyWidgetU(_pButtonDodge.get());
    _pButtonRevenge->setKeyWidgetD(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pButtonRevenge.get());
    _pButtonBack->setKeyWidgetD(_pButtonNormal.get());
    // end key nav

    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenSurvivalMode::endScreen()
{
}

void ScreenSurvivalMode::update(double elapsed)
{

}

void ScreenSurvivalMode::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        int32 widgetId = pWidget->getId();
        if(widgetId == -1)
        {
            LM_ASSERT(_pScreenMainMenu != null);
            _pManager->setScreen(_pScreenMainMenu.get());
        }
        else
        {
            if(pWidget == _pButtonNormal.get())
            {
                _appContext.wantedLevel = -1;
                _appContext.wantedMode = SURVIVAL_MODE;
            }
            else if(pWidget == _pButtonHard.get())
            {
                _appContext.wantedLevel = -1;
                _appContext.wantedMode = SURVIVAL_HARD_MODE;
            }
            else if(pWidget == _pButtonDodge.get())
            {
                _appContext.wantedLevel = -1;
                _appContext.wantedMode = DODGE_MODE;
            }
            else if(pWidget == _pButtonRevenge.get())
            {
                _appContext.wantedLevel = -1;
                _appContext.wantedMode = REVENGE_MODE;
            }
        }
    }
    else if(ev == GUI::EV_OVER_BEGIN)
    {
        if(pWidget->isEnabled())
        {
            EHighScoreSurvivalType type = HSST_NORMAL;
            if(pWidget == _pButtonNormal.get())
            {
                type = HSST_NORMAL;
            }
            else if(pWidget == _pButtonHard.get())
            {
                type = HSST_HARD;
            }
            else if(pWidget == _pButtonDodge.get())
            {
                type = HSST_DODGE;
            }
            else if(pWidget == _pButtonRevenge.get())
            {
                type = HSST_REVENGE;
            }
            else
            {
                return;
            }

            _pLabelHighScores->setText(L"survival-best-times");

            _modeLockedReason->setText(L"");

            const Core::List<Score> & scores = _appContext.pHighScores->getSurvivalHighScores(type);
            LM_ASSERT(scores.size() == 5);
            for(int32 iScore = 0; iScore < scores.size(); ++iScore)
            {
                String labelScore;
                String labelName = scores[iScore].name;
                while(labelName.length() < MAX_PLAYER_NAME_LENGTH)
                    labelName << L" ";
                labelScore << GameTools::msecTimerToString(scores[iScore].score) << L" " << labelName;
                _pLabelScores[iScore]->setText(labelScore);
            }
        }
        else
        {
            _pLabelHighScores->setText(L"mode-locked");

            for(int32 iScore = 0; iScore < 5; ++iScore)
            {
                _pLabelScores[iScore]->setText(L"");
            }

            String lockedReason;

            if(pWidget == _pButtonNormal.get())
            {
                lockedReason = (*_appContext.pDico)[L"survival-mode-locked"];
            }
            else if(pWidget == _pButtonHard.get())
            {
                lockedReason = (*_appContext.pDico)[L"survival-hard-mode-locked"];
            }
            else if(pWidget == _pButtonDodge.get())
            {
                lockedReason = (*_appContext.pDico)[L"dodge-mode-locked"];
            }
            else if(pWidget == _pButtonRevenge.get())
            {
                lockedReason = (*_appContext.pDico)[L"revenge-mode-locked"];
            }

            _modeLockedReason->setText(lockedReason);
        }
    }
}

}//namespace ChronoRage
