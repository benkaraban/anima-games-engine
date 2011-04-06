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
#include "GameModeTimeTrial.h"

#include <GameTools.h>

namespace ChronoRage
{

const Core::String GAMEMODE_WORLD_FILE = L"/ChronoRageLevels/world01.world";

GameModeTimeTrial::GameModeTimeTrial(AppContext & appContext)
:   GameModeLevels(appContext)
{
    _gameCurrentState = GAME_NOT_STARTED;
    _scoreSaved = false;
    _playerScore = -1;
}

void GameModeTimeTrial::initialize()
{
    _gameCurrentState = GAME_NOT_STARTED;
    _scoreSaved = false;
    _playerScore = -1;

    GameModeLevels::initialize();
}

void GameModeTimeTrial::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        EGameMessageBoxWidgetId widgetId = (EGameMessageBoxWidgetId)pWidget->getId();

        if(widgetId == BUTTON_NEXT_LEVEL)
        {
            setReticleCursor();

            _appContext.pGUI->popDialog();
            _victoryVisible = false;
            //Initialiser niveau suivant
            _levelNumber++;
            if(GameTools::isLevelValid(_levelNumber))
            {
                _pPlayer->resetPowerLevelAndTimeWarp();
                setLevel(_levelNumber);
                _gameCurrentState = GAME_NOT_STARTED;
                _gamePaused = false;
                _scoreSaved = false;
                _playerScore = -1;
            }
        }
        else
        {
            GameModeLevels::onEvent(ev, pWidget);
        }
    }
}

void GameModeTimeTrial::manageEscapeKey()
{
    if(_gameCurrentState != GAME_WON)
    {
        GameModeLevels::manageEscapeKey();
    }
}

void GameModeTimeTrial::initLevelCompleteBand()
{
    //Level complete band
    _victory = 0.0f;
    _victoryVisible = false;
    _pScreenVictoryDialog = _appContext.pWidgetFactory->createLargeBand(0, 0);

    Core::String levelComplete = (*_appContext.pDico)[L"level-complete"];
    _pLevelCompleteLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLevelCompleteLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pLevelCompleteLabel->setText(levelComplete);
    _pLevelCompleteLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pYourTimeLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pYourTimeLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pYourTimeLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    
    _pBestTimeLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pBestTimeLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pBestTimeLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pRankLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pRankLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pRankLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pRankStarsL = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star0.tga", 84, 0);
    _pRankStarsR = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/star0.tga", 84, 0);

    _pButtonsPanel = Ptr<GUI::Widget>(new GUI::Widget());

    _pLevelCompleteButtonNextLevel  = _appContext.pWidgetFactory->createSmallButton(L"next-level", 14, 0);
    _pLevelCompleteButtonRetry      = _appContext.pWidgetFactory->createSmallButton(L"retry", 225, 0);
    _pLevelCompleteButtonQuit       = _appContext.pWidgetFactory->createSmallButton(L"quit", 436, 0);

    _pLevelCompleteButtonNextLevel->setId(BUTTON_NEXT_LEVEL);
    _pLevelCompleteButtonRetry->setId(BUTTON_RETRY);
    _pLevelCompleteButtonQuit->setId(BUTTON_QUIT);

    _pLevelCompleteButtonNextLevel->addEventHandler(this);
    _pLevelCompleteButtonRetry->addEventHandler(this);
    _pLevelCompleteButtonQuit->addEventHandler(this);

    _pButtonsPanel->addChild(_pLevelCompleteButtonNextLevel);
    _pButtonsPanel->addChild(_pLevelCompleteButtonRetry);
    _pButtonsPanel->addChild(_pLevelCompleteButtonQuit);

    _pButtonsPanel->pack();
    
    _pScreenVictoryDialog->addChild(_pLevelCompleteLabel);
    _pScreenVictoryDialog->addChild(_pYourTimeLabel);
    _pScreenVictoryDialog->addChild(_pBestTimeLabel);
    _pScreenVictoryDialog->addChild(_pRankLabel);
    _pScreenVictoryDialog->addChild(_pButtonsPanel);
    _pScreenVictoryDialog->addChild(_pRankStarsL);
    _pScreenVictoryDialog->addChild(_pRankStarsR);
}

void GameModeTimeTrial::manageVictory()
{
    if(_gameCurrentState == GAME_ENTER_NAME && _hasPlayerEnteredHisName)
        _gameCurrentState = GAME_WON;

    if(_playerScore == -1 && (_gameCurrentState == GAME_ENTER_NAME || _gameCurrentState == GAME_WON))
        _playerScore = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

    if(_gameCurrentState == GAME_WON)
    {
        if(!_hasPlayerEnteredHisName 
        && _appContext.pHighScores->isLevelHighScore(_levelNumber-1, _playerScore)
        && _unlockAchievements.size() <= 0)
        {   
            _gameCurrentState = GAME_ENTER_NAME;
            _scoreSaved = false;
        }

        Core::Vector2f dialogSize = _pScreenVictoryDialog->getSize();
        dialogSize.x = (float)_appContext.pRenderer->getDefaultView()->getWidth();
        _pScreenVictoryDialog->setSize(dialogSize);
        _pScreenVictoryDialog->setPosition(Core::Vector2f(0.0f,(float)_appContext.pRenderer->getDefaultView()->getHeight()/4.0f));

        _pLevelCompleteLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pLevelCompleteLabel->setPosition(Core::Vector2f(0.0f, 40));
        _pLevelCompleteLabel->setHLayout(GUI::HL_CENTER);

        _pYourTimeLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pYourTimeLabel->setPosition(Core::Vector2f(0.0f, 111));
        _pYourTimeLabel->setHLayout(GUI::HL_CENTER);

        _pBestTimeLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pBestTimeLabel->setPosition(Core::Vector2f(0.0f, 173));
        _pBestTimeLabel->setHLayout(GUI::HL_CENTER);

        _pRankLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pRankLabel->setPosition(Core::Vector2f(0.0f, 244));
        _pRankLabel->setHLayout(GUI::HL_CENTER);

        Core::Vector2f bandSize = _pScreenVictoryDialog->getSize();
        Core::Vector2f buttonSize = _pButtonsPanel->getSize();

        Core::Vector2f panelPosition;
        panelPosition.x = (bandSize.x/2) - (buttonSize.x / 2);
        panelPosition.y = bandSize.y - 65;

        _pButtonsPanel->setPosition(panelPosition);

        if(_hasPlayerEnteredHisName && !_scoreSaved && _appContext.pHighScores->isLevelHighScore(_levelNumber-1, _playerScore))
        {
            _appContext.pHighScores->addLevelHighScore(_levelNumber-1, _playerScore, _appContext.pGameSettings->getPlayerName());
            _appContext.saveScores = true;
            _scoreSaved = true;
        }

        if(!_victoryVisible)
        {
            int32 bestTimeMsec = -1;
            bestTimeMsec = _appContext.pHighScores->getLevelHighScores(_levelNumber-1)[0].score;

            if(_playerScore < bestTimeMsec)//Le score n'a pas encore ÈtÈ ajoutÈ au high score
                bestTimeMsec = _playerScore;

            Core::String yourTime = (*_appContext.pDico)[L"your-time"];
            Core::String bestTime = (*_appContext.pDico)[L"best-time"];
            Core::String rank     = (*_appContext.pDico)[L"rank"];

            int32 maxSize = Core::max(yourTime.length(), bestTime.length(), rank.length());
            while((int32)yourTime.length() < maxSize)
                yourTime << L" ";
            yourTime << L"  :  "<<GameTools::msecTimerToString(_playerScore);

            while((int32)bestTime.length() < maxSize)
                bestTime << L" ";
            bestTime << L"  :  "<<GameTools::msecTimerToString(bestTimeMsec);

            ELevelRank levelRank = _appContext.pLevelRanks->getRank(_levelNumber, _playerScore);
            String rankString = rankToString(levelRank);

            while((int32)rank.length() < maxSize)
                rank << L" ";
            rank << L"  :  " << (*_appContext.pDico)[rankString];
    
            int32 maxSize2 = Core::max(yourTime.length(), bestTime.length(), rank.length());
            while((int32)yourTime.length() < maxSize2)
                yourTime << L" ";
            while((int32)bestTime.length() < maxSize2)
                bestTime << L" ";
            while((int32)rank.length() < maxSize2)
                rank << L" ";

            _pYourTimeLabel->setText(yourTime);
            _pBestTimeLabel->setText(bestTime);
            _pRankLabel->setText(rank);

            int32 starCount = std::max(0, 3 - int32(levelRank));
            String starPic(L"/ChronoRageData/GUI/star" + Core::toString(starCount) + L".tga");

            float centerx = 0.5f * dialogSize.x;
            float textSize = std::max(_pYourTimeLabel->getTextSize().x, std::max(_pBestTimeLabel->getTextSize().x, _pRankLabel->getTextSize().x));

            _pRankStarsL->setPosition(Core::Vector2f(centerx - 0.5f * textSize - 48.0f, 244.0f));
            _pRankStarsR->setPosition(Core::Vector2f(centerx + 0.5f * textSize + 16.0f, 244.0f));

            _pRankStarsL->setPicture(starPic);
            _pRankStarsR->setPicture(starPic);

            _pLevelCompleteButtonNextLevel->setKeyWidgetR(_pLevelCompleteButtonRetry.get());
            _pLevelCompleteButtonRetry->setKeyWidgetL(_pLevelCompleteButtonNextLevel.get());
            _pLevelCompleteButtonRetry->setKeyWidgetR(_pLevelCompleteButtonQuit.get());
            _pLevelCompleteButtonQuit->setKeyWidgetL(_pLevelCompleteButtonRetry.get());

            _pLevelCompleteButtonNextLevel->setEnabled(false);
            _pLevelCompleteButtonRetry->setEnabled(false);
            _pLevelCompleteButtonQuit->setEnabled(false);

            _appContext.pGUI->pushDialog(_pScreenVictoryDialog);
            _appContext.pGUI->getKeyNavigation()->pKeyFocus = _pLevelCompleteButtonNextLevel.get();
            _pScreenVictoryDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            _victoryVisible = true;
        }
        
        float alpha = 1.0f;
        if(VICTORY_PANEL_TIMER - _victory <= 1.0f)
            alpha = VICTORY_PANEL_TIMER - _victory;
        
        _pScreenVictoryDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));

        if(alpha == 1.0f && !_gamePaused)
        {
            _appContext.pGUI->setCursor(_appContext.pCursorArrow, 0, 0);
            _gamePaused = true;

            _pLevelCompleteButtonNextLevel->setEnabled(GameTools::isLevelValid(_levelNumber+1));
            _pLevelCompleteButtonRetry->setEnabled(true);
            _pLevelCompleteButtonQuit->setEnabled(true);

        }
    }
}

}//namespace ChronoRage