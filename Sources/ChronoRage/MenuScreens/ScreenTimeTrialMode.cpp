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
#include "ScreenTimeTrialMode.h"

#include <GameTools.h>

namespace ChronoRage
{

ScreenTimeTrialMode::ScreenTimeTrialMode(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pGlobalPanel = Ptr<GUI::Widget>(new GUI::Widget());
    _pGlobalPanel->setPosition(Core::Vector2f(0, 0));
    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);
    _pPanelInfo = _appContext.pWidgetFactory->createMediumPanel(500, 0);

    static const int32 Y_PADDING = 53;
    static const int32 X_COL_1 = 24;
    static const int32 X_COL_2 = 245;

    //Boutons
    Core::String baseLevelString = (*_appContext.pDico)[L"time-trial-level"];

    for(int32 iButton = 0; iButton < 20; ++iButton)
    {
        Core::String levelString;
        levelString << baseLevelString << L" ";
        if(iButton+1 < 10)
            levelString << L"0";
        levelString << Core::toString(iButton+1);

        if(iButton < 10)
            _pButtonLevel[iButton] = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_1, (iButton+1) * Y_PADDING);
        else
        {
            _pButtonLevel[iButton] = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, (iButton-10+1) * Y_PADDING);
        }

        _pStarsLevelL[iButton] = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star0.tga", 2, 2);
        _pStarsLevelR[iButton] = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star0.tga", 164, 2);

        _pPriorityBooster[iButton] = Ptr<GUI::Widget>(new GUI::Widget());
        _pPriorityBooster[iButton]->setDeltaPriority(300);
        _pPriorityBooster[iButton]->addChild(_pStarsLevelL[iButton]);
        _pPriorityBooster[iButton]->addChild(_pStarsLevelR[iButton]);
        _pButtonLevel[iButton]->addChild(_pPriorityBooster[iButton]);

        //NÈcÈssaire pour qu'il ne cherche pas les textes "Niveau XX" dans le dictionnaire.
        _pButtonLevel[iButton]->setDictionary(null);
        _pButtonLevel[iButton]->setText(levelString);

        _pButtonLevel[iButton]->setId(iButton +1);
        _pButtonLevel[iButton]->addEventHandler(this);

        _pPanelButtons->addChild(_pButtonLevel[iButton]);
    }

    _pPanelButtons->pack();

    //Info
    static const int32 Y_PADDING_2 = 40;
    int padding = 1;
    Core::Vector2f labelSize;
    _pLabelLevel = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    labelSize = _pLabelLevel->getSize();
    labelSize.x = _pPanelInfo->getSize().x;
    _pLabelLevel->setSize(labelSize);
    _pLabelLevel->setHLayout(GUI::HL_CENTER);
    _pLabelLevel->setDictionary(null);
    _pLabelLevel->setText(L"");

    _pLabelLevelName = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    _pLabelLevelName->setHLayout(GUI::HL_CENTER);
    _pLabelLevelName->setSize(labelSize);
    _pLabelLevelName->setDictionary(null);
    _pLabelLevelName->setText(L"");
    
    _pLabelLevelAvailable = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    _pLabelLevelAvailable->setHLayout(GUI::HL_CENTER);
    _pLabelLevelAvailable->setSize(labelSize);
    _pLabelLevelAvailable->setDictionary(null);
    _pLabelLevelAvailable->setText(L"");
    _pLabelLevelAvailable->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

    _pPicLevelAce = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star3.tga", 40, padding * Y_PADDING_2);
    _pLabelLevelAce = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    _pLabelLevelAce->setHLayout(GUI::HL_CENTER);
    _pLabelLevelAce->setSize(labelSize);
    _pLabelLevelAce->setDictionary(null);
    _pLabelLevelAce->setText(L"");

    _pPicLevelExpert = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star2.tga", 40, padding * Y_PADDING_2);
    _pLabelLevelExpert = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    _pLabelLevelExpert->setHLayout(GUI::HL_CENTER);
    _pLabelLevelExpert->setSize(labelSize);
    _pLabelLevelExpert->setDictionary(null);
    _pLabelLevelExpert->setText(L"");

    _pPicLevelVeteran = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star1.tga", 40, padding * Y_PADDING_2);
    _pLabelLevelVeteran = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
    _pLabelLevelVeteran->setHLayout(GUI::HL_CENTER);
    _pLabelLevelVeteran->setSize(labelSize);
    _pLabelLevelVeteran->setDictionary(null);
    _pLabelLevelVeteran->setText(L"");

    _pPanelInfo->addChild(_pLabelLevel);
    _pPanelInfo->addChild(_pLabelLevelName);
    _pPanelInfo->addChild(_pLabelLevelAvailable);
    _pPanelInfo->addChild(_pLabelLevelAce);
    _pPanelInfo->addChild(_pLabelLevelExpert);
    _pPanelInfo->addChild(_pLabelLevelVeteran);
    _pPanelInfo->addChild(_pPicLevelAce);
    _pPanelInfo->addChild(_pPicLevelExpert);
    _pPanelInfo->addChild(_pPicLevelVeteran);

    padding += 1;

    for(int32 iScore = 0; iScore < 5; ++iScore)
    {
        _pStarsScores[iScore] = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star0.tga", 40, padding * Y_PADDING_2);
        _pLabelScores[iScore] = _appContext.pWidgetFactory->createMediumLabel(L"", 0, padding++ * Y_PADDING_2);
        _pLabelScores[iScore]->setHLayout(GUI::HL_CENTER);
        _pLabelScores[iScore]->setSize(labelSize);
        _pLabelScores[iScore]->setDictionary(null);
        _pLabelScores[iScore]->setText(L"");
        
        _pPanelInfo->addChild(_pLabelScores[iScore]);
        _pPanelInfo->addChild(_pStarsScores[iScore]);
    }
    //
    _pPanelInfo->pack();

    _pGlobalPanel->addChild(_pPanelButtons);
    _pGlobalPanel->addChild(_pPanelInfo);
    

    _pButtonBack = _appContext.pWidgetFactory->createSmallButton(L"back", 769, 620);
    _pButtonBack->setId(-1);
    _pButtonBack->setHotKey(VK_ESCAPE);
    _pButtonBack->addEventHandler(this);

    _pGlobalPanel->addChild(_pButtonBack);
    _pGlobalPanel->pack();

    _appContext.pWidgetFactory->centerScreen(_pGlobalPanel);

    _pDialog->addChild(_pGlobalPanel);

    // Key nav
    _pLastKeyFocus = _pButtonBack.get();
    // end key nav
}

void ScreenTimeTrialMode::resetLabels()
{
    //Boutons
    Core::String baseLevelString = (*_appContext.pDico)[L"time-trial-level"];

    for(int32 iButton = 0; iButton < 20; ++iButton)
    {
        Core::String levelString;
        levelString << baseLevelString << L" ";
        if(iButton+1 < 10)
            levelString << L"0";
        levelString << Core::toString(iButton+1);

        _pButtonLevel[iButton]->setText(levelString);
    }

    _pButtonBack->setDictionary(_appContext.pDico);
    _pButtonBack->setText(L"back");

    _pLabelLevelAvailable->setText(L"");
}

void ScreenTimeTrialMode::startScreen()
{
    resetLabels();
    _pPicLevelAce->setPicture(L"/ChronoRageData/GUI/star0.tga");;
    _pPicLevelExpert->setPicture(L"/ChronoRageData/GUI/star0.tga");
    _pPicLevelVeteran->setPicture(L"/ChronoRageData/GUI/star0.tga");

    for(int32 iButton = 0; iButton < 20; ++iButton)
    {
        // Ranking stars
        _pButtonLevel[iButton]->setEnabled(_appContext.pHighScores->isLevelAvailable(iButton));
        const Core::List<Score> & scores = _appContext.pHighScores->getLevelHighScores(iButton);
        int32 starCount = 0;

        if(_appContext.pHighScores->isLevelAvailable(iButton) && scores.size() >= 1)
            starCount = 3 - _appContext.pLevelRanks->getRank(iButton + 1, scores[0].score);

        String starPic(L"/ChronoRageData/GUI/star" + Core::toString(starCount) + L".tga");

        _pStarsLevelL[iButton]->setPicture(starPic);
        _pStarsLevelR[iButton]->setPicture(starPic);

        // Keyboard nav links
        if(iButton == 0)
        {
            _pButtonLevel[iButton]->setKeyWidgetU(_pButtonBack.get());
        }
        if(iButton > 0)
        {
            _pButtonLevel[iButton]->setKeyWidgetU(_pButtonLevel[iButton-1].get());
        }
        if(iButton < 19)
        {
            _pButtonLevel[iButton]->setKeyWidgetD(_pButtonLevel[iButton+1].get());
        }
        if(iButton == 19)
        {
            _pButtonLevel[iButton]->setKeyWidgetD(_pButtonBack.get());
        }

        if(iButton < 10)
        {
            _pButtonLevel[iButton]->setKeyWidgetR(_pButtonLevel[iButton+10].get());
        }
        else
        {
            _pButtonLevel[iButton]->setKeyWidgetL(_pButtonLevel[iButton-10].get());
        }
    }

    _pButtonBack->setKeyWidgetU(_pButtonLevel[19].get());
    _pButtonBack->setKeyWidgetD(_pButtonLevel[0].get());

    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenTimeTrialMode::endScreen()
{
}

void ScreenTimeTrialMode::update(double elapsed)
{
    int a = 1;
}

void ScreenTimeTrialMode::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
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
            if(GameTools::isLevelValid(widgetId))
            {
                _appContext.wantedMode = TIME_TRIAL_MODE;
                _appContext.wantedScreen = TIME_TRIAL_SCREEN;
                _appContext.wantedLevel = widgetId;
            }
        }
    }
    else if(ev == GUI::EV_OVER_BEGIN)
    {
        int32 widgetId = pWidget->getId();
        if(GameTools::isLevelValid(widgetId) && widgetId > 0)
        {
            Core::String baseLevelString = (*_appContext.pDico)[L"time-trial-level"];
            
            String labelLevel;
            labelLevel << baseLevelString << L" ";
            if(widgetId < 10)
                labelLevel << L"0";
            labelLevel << Core::toString(widgetId);
            _pLabelLevel->setText(labelLevel);

            String labelLevelName;
            labelLevelName << L"level-";
            if(widgetId < 10)
                labelLevelName << L"0";
            labelLevelName << Core::toString(widgetId)<<L"-name";

            _pLabelLevelName->setText((*_appContext.pDico)[labelLevelName]);

            Core::String ace = (*_appContext.pDico)[L"time-trial-ace"];
            Core::String expert = (*_appContext.pDico)[L"time-trial-expert"];
            Core::String veteran = (*_appContext.pDico)[L"time-trial-veteran"];
            int32 maxSize = Core::max(ace.length(), expert.length(), veteran.length());
            while(int32(ace.length()) < maxSize)
                ace << L" ";
            while(int32(expert.length()) < maxSize)
                expert << L" ";
            while(int32(veteran.length()) < maxSize)
                veteran << L" ";

            int32 aceTime       = _appContext.pLevelRanks->getTime(widgetId, RANK_ACE);
            int32 expertTime    = _appContext.pLevelRanks->getTime(widgetId, RANK_EXPERT);
            int32 veteranTime   = _appContext.pLevelRanks->getTime(widgetId, RANK_VETERAN);

            ace << L" : " << GameTools::msecTimerToString(aceTime);
            expert << L" : " << GameTools::msecTimerToString(expertTime);
            veteran << L" : " << GameTools::msecTimerToString(veteranTime);
            _pLabelLevelAce->setText(ace);
            _pLabelLevelExpert->setText(expert);
            _pLabelLevelVeteran->setText(veteran);
            
            _pPicLevelAce->setPicture(L"/ChronoRageData/GUI/star3.tga");;
            _pPicLevelExpert->setPicture(L"/ChronoRageData/GUI/star2.tga");
            _pPicLevelVeteran->setPicture(L"/ChronoRageData/GUI/star1.tga");

            const Core::List<Score> & scores = _appContext.pHighScores->getLevelHighScores(widgetId-1);
            LM_ASSERT(scores.size() == 5);
            for(int32 iScore = 0; iScore < 5; ++iScore)
            {
                String labelScore;
                String labelName = scores[iScore].name;
                while(labelName.length() < MAX_PLAYER_NAME_LENGTH)
                    labelName << L" ";
                labelScore << GameTools::msecTimerToString(scores[iScore].score) << L" " << labelName;
                _pLabelScores[iScore]->setText(labelScore);

                int32 starCount = 0;

                if(scores[iScore].score >= 0)
                    starCount = std::max(0, 3 - _appContext.pLevelRanks->getRank(widgetId, scores[iScore].score));

                String starPic(L"/ChronoRageData/GUI/star" + Core::toString(starCount) + L".tga");

                _pStarsScores[iScore]->setPicture(starPic);
            }

            if(_pButtonLevel[widgetId-1]->isEnabled())
            {
                _pLabelLevelAvailable->setText(L"");
            }
            else
            {
                _pLabelLevelAvailable->setText((*_appContext.pDico)[L"level-not-available"]);
            }
        }
    }
}

}//namespace ChronoRage
