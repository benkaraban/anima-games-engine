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
#include "GameModeLevels.h"

#include <GameTools.h>

#define USE_VORTEX 1

namespace ChronoRage
{

const Core::String GAMEMODE_WORLD_FILE = L"/ChronoRageLevels/world01.world";

GameModeLevels::GameModeLevels(AppContext & appContext)
:   GameMode(appContext),
    _totalElapsed(0.0f),
    _timerBlip(0.0f),
    _msecToClearLevel(0),
    _displayLevelLabel(true),
    _changeTimerColor(true)
{
    _gameCurrentState = GAME_ENTER_NAME;
}

void GameModeLevels::restart()
{
    initialize();
    setLevel(_levelNumber);
    
    GameMode::restart();
}

void GameModeLevels::setLevel(int32 levelNumber)
{
    if(_gameCurrentState != GAME_ENTER_NAME)
       _gameCurrentState = GAME_NOT_STARTED;

    _totalElapsed = 0;
    _timerBlip = 0;
    _powerLevel = 0;
    _musicFadedOut = false;
    _achievementBareHanded = true;
    _achievementPerfect = true;
    _achievementCreditsHunter = (levelNumber == -1);
    
    if(levelNumber == 1)
        _achievementMarathon = true;

    _pDeathPenalty->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 0.0f));
    _pDeathPenalty->setPosition(Core::Vector2f(0.0f, 100.0f));
    _pPowerUpLabel->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pPowerUpLabel->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-75.0f));
    _pPowerDownLabel->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 0.0f));
    _pPowerDownLabel->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-130.0f));
    _pTimeWarpBonusLabel->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pTimeWarpBonusLabel->setPosition(Core::Vector2f(0.0f, 75.0f));

    if(getMode() == NEW_GAME_MODE)
    {
        _appContext.pGameSettings->setCurrentLevel(levelNumber);
    }

    Core::String waveFile = GameTools::getWaveFile(levelNumber);
    Core::String levelFile = GameTools::getLevelFile(levelNumber);
    int32 secondsToClearLevel = GameTools::getSecondsToClearLevel(levelNumber);
    _levelNumber = levelNumber;

    _registeredWaves.clear();
    registerWaveFile(waveFile);
    setLevelFile(levelFile, secondsToClearLevel);

    manageGUI();

    if(_appContext.pGameSettings->getMusicVolume() >= 0.01f)
    {
        _appContext.pSoundLibrary->playLevelMusic(_levelNumber);
    }
}

void GameModeLevels::setLevelFile(const Core::String & levelFile, int32 secondsToClearLevel)
{
    _waveOrders.clear();
    _waves.clear();

    Core::XMLDocument xmlDocument;
    xmlDocument.loadDocument(*(_appContext.pWorld->getRessourcesPool()->getVFS()->readFile(levelFile)));

    Ptr<Core::XMLNode> pXMLLevel = xmlDocument.firstChildNode(L"level");
    
    Core::List<Ptr<Core::XMLNode> > pXMLWaveOrderList;
    pXMLLevel->getNodes(L"wave-order", pXMLWaveOrderList);

    for(int32 ii=0; ii<pXMLWaveOrderList.size(); ++ii)
    {
        Core::String strId;
        pXMLWaveOrderList[ii]->getAttribute(L"id", strId);
        Core::String strDuration;
        pXMLWaveOrderList[ii]->getAttribute(L"duration", strDuration);

        WaveOrder waveOrder;
        waveOrder.id = Core::toInteger(strId);
        waveOrder.duration = (float)Core::toFloat(strDuration);
        waveOrder.launched = false;

        if(_registeredWaves.find(waveOrder.id) == _registeredWaves.end())
        {
            LM_ASSERT(false);
            Core::String message;
            message << L"In level file : '" 
                    << levelFile 
                    << L"' the wave id [" 
                    << waveOrder.id 
                    << L"] is referenced but not registered!";
            throw Core::Exception(message);
        }

        _waveOrders.push_back(waveOrder);
    }

    _levelFile = levelFile;
    _msecToClearLevel = secondsToClearLevel * 1000;
    _secondsToClearLevel = secondsToClearLevel;
}

void GameModeLevels::initialize()
{
    GameMode::initialize();

    _gameOverVisible = false;
    _totalElapsed = 0.0f;
    _timerBlip = 0.0f;
    _achievementMarathon = false;

    Ptr<Core::InputStream> pStreamWorldFile = _appContext.pVFS->readFile(GAMEMODE_WORLD_FILE);
    if(pStreamWorldFile == null)
    {
        Core::String message;
        message << L"Failed to load GameModeLevels World : "<<GAMEMODE_WORLD_FILE;
        throw Core::Exception(message);
    }
    
    Ptr<Core::XMLDocument> pXmlWorld(new Core::XMLDocument());
    pXmlWorld->loadDocument(*pStreamWorldFile);
    
    _appContext.pWorld->importWorldXML(*pXmlWorld);
    
    if(!initCamera())
    {
        Core::String message;
        message << L"Failed to get camera in GameModeLevels World : "<<GAMEMODE_WORLD_FILE;
        throw Core::Exception(message);
    }

    _pLevel = Ptr<Level>(new Level(150, 100, 10, -1));
    initPlayer(true);

    _pTimeWarp = Ptr<TimeWarp>(new TimeWarp(_appContext.pWorld, _appContext.pCameraNode, _pPlayer));

    for(int32 ii=0; ii<HELP_FIND_REMAINING_FOES; ++ii)
    {
        _foeDirections[ii].initialize(_appContext.pWorld);
    }

    initVortex();
    initGUI();

}

void GameModeLevels::initVortex()
{
    VortexSettings settings;
    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.2f;

    settings.armCount        = 6;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = true;

    settings.color0          = Core::Vector4f(0.2f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.0f, 0.1f, 0.3f, 1.0f),
    settings.colorByDepth    = true;

    settings.cubeSelfRotationSpeed = 0.2f;

    settings.start          = 10.0f;
    settings.startWidth     = 90.0f;

    settings.nearU          = 0.0f;
    settings.sharpness      = 1.0f;
    settings.sharpness2     = 1.0f;

    settings.near           = 20.0f;
    settings.nearMinWidth   = 100.0f;
    settings.nearMaxWidth   = 110.0f;

    // good
    //settings.near           = 20.0f;
    //settings.nearMinWidth   = 100.0f;
    //settings.nearMaxWidth   = 100.0f;

    settings.far            = 150.0f;
    settings.farMinWidth    = 10.0f;
    settings.farMaxWidth    = 10.0f;

    settings.spiralCount    = 1.0f;//5.0f;

    settings.suckingSpeed   = 0.01f;

#if USE_VORTEX
    _pVortex = Ptr<Vortex>(new Vortex(_appContext.pWorld, settings, _pTimeWarp));
#endif

    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.5f;

    settings.armCount        = 1;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = false;
    settings.cubeSelfRotationSpeed = 0.05f;

    settings.color0          = Core::Vector4f(0.1f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.0f, 0.05f, 0.1f, 1.0f);
    settings.colorByDepth    = false;

    settings.start          = 10.0f;
    settings.startWidth     = 90.0f;

    settings.nearU          = 0.0f;
    settings.sharpness      = 1.0f;
    settings.sharpness2     = 1.0f;

    settings.near           = 50.0f;
    settings.nearMinWidth   = 90.0f;
    settings.nearMaxWidth   = 120.0f;

    settings.far            = 130.0f;
    settings.farMinWidth    = 30.0f;
    settings.farMaxWidth    = 80.0f;

    settings.spiralCount    = 0.0f;//5.0f;

    settings.suckingSpeed   = 0.00f;

#if USE_VORTEX
    _pVortex2 = Ptr<Vortex>(new Vortex(_appContext.pWorld, settings, _pTimeWarp));
    _pVortex2->update(0.0);
#endif
}

void GameModeLevels::initGUI()
{
    _pTimerLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimerLabel->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pTimerLabel->setText(L"00:00.000");
    _pTimerLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pPowerUpLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pPowerUpLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pPowerUpLabel->setText(L"POWER UP");
    _pPowerUpLabel->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pPowerUpLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pPowerUpLabel->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-75.0f));
    _pPowerUpLabel->setHLayout(GUI::HL_CENTER);

    _pPowerDownLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pPowerDownLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pPowerDownLabel->setText(L"POWER DOWN");
    _pPowerDownLabel->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 0.0f));
    _pPowerDownLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pPowerDownLabel->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-130.0f));
    _pPowerDownLabel->setHLayout(GUI::HL_CENTER);

    _pTimeWarpBonusLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeWarpBonusLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pTimeWarpBonusLabel->setText(L"TIME WARP BONUS");
    _pTimeWarpBonusLabel->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pTimeWarpBonusLabel->setPosition(Core::Vector2f(0.0f, 75.0f));
    _pTimeWarpBonusLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimeWarpBonusLabel->setHLayout(GUI::HL_CENTER);

    _pDeathPenalty = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pDeathPenalty->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pDeathPenalty->setText(L"00:00.000");
    _pDeathPenalty->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 0.0f));
    
    _pPowerLevel = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pPowerLevel->setBarInterior(L"/ChronoRageData/GUI/powerlevel-progressbar-inside.tga");
    _pPowerLevel->setBarBorder(L"/ChronoRageData/GUI/powerlevel-progressbar-border.tga");
    _pPowerLevel->setBorderColor(Core::Vector4f(0.1f, 0.4f, 0.8f, 1.0f));
    _pPowerLevel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pTimeBomb1 = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeBomb1->setBarInterior(L"/ChronoRageData/GUI/timewarp-progress-inside.tga");
    _pTimeBomb1->setBarBorder(L"/ChronoRageData/GUI/timewarp-progress-border.tga");
    _pTimeBomb1->setBorderColor(Core::Vector4f(0.1f, 0.4f, 0.8f, 1.0f));
    _pTimeBomb2 = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeBomb2->setBarInterior(L"/ChronoRageData/GUI/timewarp-progress-inside.tga");
    _pTimeBomb2->setBarBorder(L"/ChronoRageData/GUI/timewarp-progress-border.tga");
    _pTimeBomb2->setBorderColor(Core::Vector4f(0.1f, 0.4f, 0.8f, 1.0f));
    _pTimeBomb3 = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeBomb3->setBarInterior(L"/ChronoRageData/GUI/timewarp-progress-inside.tga");
    _pTimeBomb3->setBarBorder(L"/ChronoRageData/GUI/timewarp-progress-border.tga");
    _pTimeBomb3->setBorderColor(Core::Vector4f(0.1f, 0.4f, 0.8f, 1.0f));
    _pTimeBomb4 = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeBomb4->setBarInterior(L"/ChronoRageData/GUI/timewarp-progress-inside.tga");
    _pTimeBomb4->setBarBorder(L"/ChronoRageData/GUI/timewarp-progress-border.tga");
    _pTimeBomb4->setBorderColor(Core::Vector4f(0.1f, 0.4f, 0.8f, 1.0f));

    _pScreenDialog->addChild(_pTimerLabel);
    _pScreenDialog->addChild(_pDeathPenalty);
    _pScreenDialog->addChild(_pPowerUpLabel);
    _pScreenDialog->addChild(_pPowerDownLabel);
    _pScreenDialog->addChild(_pTimeWarpBonusLabel);
    _pScreenDialog->addChild(_pPowerLevel);
    _pScreenDialog->addChild(_pTimeBomb1);
    _pScreenDialog->addChild(_pTimeBomb2);
    _pScreenDialog->addChild(_pTimeBomb3);
    _pScreenDialog->addChild(_pTimeBomb4);

    //Level start band
    _levelLabelVisible = false;
    _pScreenLevelDialog = _appContext.pWidgetFactory->createThinBand(0, 0);
    _pLevelLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLevelLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pLevelLabel->setText(L"LEVEL : 01");
    _pLevelLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    _pScreenLevelDialog->addChild(_pLevelLabel);

    initLevelCompleteBand();

}

void GameModeLevels::manageGUI()
{
    int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

    int32 remainingTime = std::max(0, _msecToClearLevel - totalElapsed);

    
    _pTimerLabel->setSize(Core::Vector2f(   (float)_appContext.pRenderer->getDefaultView()->getWidth(), 
                                            (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimerLabel->setPosition(Core::Vector2f(0.0f, 10.0f));
    _pTimerLabel->setHLayout(GUI::HL_CENTER);
    if(!_victoryVisible)
    {
        Core::String timerLabel = GameTools::msecTimerToString(remainingTime);
        _pTimerLabel->setText(timerLabel);
    }

    Core::Vector4f deathPenaltyColor = _pDeathPenalty->getColor();
    if(deathPenaltyColor.a > 0.0f)
    {
        _pDeathPenalty->setSize(Core::Vector2f( (float)_appContext.pRenderer->getDefaultView()->getWidth(), 
                                                (float)_appContext.pRenderer->getDefaultView()->getHeight()));
        Core::Vector2f deathPenaltyPosition;
        float alpha = 1.0f-((float)(_totalElapsed - _deathPenaltyAppearance))/1.5f;
        deathPenaltyPosition.setLerp(Core::Vector2f(0.0f, 300.0f), Core::Vector2f(0.0f, 100.0f), alpha);
        _pDeathPenalty->setPosition(deathPenaltyPosition);
        _pDeathPenalty->setHLayout(GUI::HL_CENTER);
    }

    Core::Vector4f powerUpColor = _pPowerUpLabel->getColor();
    if(powerUpColor.a > 0.0f)
    {
        _pPowerUpLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
        Core::Vector2f powerUpPosition;
        float alpha = 1.0f-((float)(_totalElapsed - _powerUpAppearance))/1.5f;
        powerUpPosition.setLerp(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-200.0f), Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-75.0f), alpha);
        _pPowerUpLabel->setPosition(powerUpPosition);
        _pPowerUpLabel->setHLayout(GUI::HL_CENTER);
        
        powerUpColor.setLerp(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), Core::Vector4f(0.25f, 1.0f, 0.25f, 1.0f), alpha);
        _pPowerUpLabel->setOwnColor(powerUpColor);
    }
    
    Core::Vector4f powerDownColor = _pPowerDownLabel->getColor();
    if(powerDownColor.a > 0.0f)
    {
        _pPowerDownLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
        Core::Vector2f powerDownPosition;
        float alpha = 1.0f-((float)(_totalElapsed - _powerDownAppearance))/1.5f;
        powerDownPosition.setLerp(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-75.0f), Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()-130.0f), alpha);
        _pPowerDownLabel->setPosition(powerDownPosition);
        _pPowerDownLabel->setHLayout(GUI::HL_CENTER);
        
        powerDownColor.setLerp(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), Core::Vector4f(1.0f, 0.25f, 0.25f, 1.0f), alpha);
        _pPowerDownLabel->setOwnColor(powerDownColor);
    }

    Core::Vector4f timeWarpBonusColor = _pTimeWarpBonusLabel->getColor();
    if(timeWarpBonusColor.a > 0.0f)
    {
        _pTimeWarpBonusLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
        _pTimeWarpBonusLabel->setHLayout(GUI::HL_CENTER);

        float alpha = 1.0f-((float)(_totalElapsed - _timeWarpBonusAppearance))/1.5f;
        timeWarpBonusColor.setLerp(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), Core::Vector4f(0.25f, 1.0f, 0.25f, 1.0f), alpha);
        _pTimeWarpBonusLabel->setOwnColor(timeWarpBonusColor);
    }

    float blinkTime = ((float)remainingTime) / 1000.0f;
    Core::Vector4f timerColor(0.4f, 0.9f, 1.0f, 1.0f);
    if(blinkTime <= 60 && _changeTimerColor)
    {
        timerColor.setLerp(Core::Vector4f(1.0f, 0.24f, 0.24f, 1.0f), timerColor, blinkTime/60);
    }
    
    if(blinkTime <= 45 && _gameCurrentState == GAME_STARTED)
    {
        float blinkFactor = 15.0f;

        if(blinkTime <= 10)
            blinkFactor = 30;

        Core::Vector4f timerLabelColor;
        float alpha = (Core::L_sin(blinkTime * blinkFactor)+1)/2;
        
        timerLabelColor.setLerp(timerColor, Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), alpha);
        _pTimerLabel->setOwnColor(timerLabelColor);

        if(_timerBlip <= _totalElapsed)
        {
            _appContext.pSoundLibrary->playSound(SOUND_CHRONO_BLIP, false);
            if(blinkTime < 1.0f)
            {
                _timerBlip = float(_totalElapsed) + 0.10f;
            }
            else if(blinkTime < 5)
            {
                _timerBlip = float(_totalElapsed) + 0.20f;
            }
            else if(blinkTime < 15)
            {
                _timerBlip = float(_totalElapsed) + 0.5f;
            }
            else
                _timerBlip = float(_totalElapsed) + 1.0f;
        }
    }
    else
        _pTimerLabel->setOwnColor(timerColor);

    float powerLevel = (_pPlayer->getPowerLevel() - 1.0f)/(PLAYER_MAXIMUM_POWER - 1.0f);
    _pPowerLevel->setPosition(Core::Vector2f((  (float)_appContext.pRenderer->getDefaultView()->getWidth()/2)-104, 
                                                (float)_appContext.pRenderer->getDefaultView()->getHeight()-30.0f));
    _pPowerLevel->setProgress(powerLevel);
        
    //  tBomb1   tBomb2   tBomb4   tBomb4
    // <--60-->4<--60-->4<--60-->4<--60-->
    // <-126 pixels>screen center<+126 pixels>
    _pTimeBomb1->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2)-126, 50.0f));
    _pTimeBomb2->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2)-62, 50.0f));
    _pTimeBomb3->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2)+2, 50.0f));
    _pTimeBomb4->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2)+66, 50.0f));

    float timeWarp = _pPlayer->getTimeWarpNumber();

    if(timeWarp >= 4.0f)
    {
        _pTimeBomb1->setProgress(1.0f);
        _pTimeBomb2->setProgress(1.0f);
        _pTimeBomb3->setProgress(1.0f);
        _pTimeBomb4->setProgress(1.0f);

        _pTimeBomb1->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb2->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb3->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb4->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    }
    else if(timeWarp >= 3.0f)
    {
        _pTimeBomb1->setProgress(1.0f);
        _pTimeBomb2->setProgress(1.0f);
        _pTimeBomb3->setProgress(1.0f);
        _pTimeBomb4->setProgress(timeWarp - 3.0f);

        _pTimeBomb1->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb2->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb3->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb4->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
    }
    else if(timeWarp >= 2.0f)
    {
        _pTimeBomb1->setProgress(1.0f);
        _pTimeBomb2->setProgress(1.0f);
        _pTimeBomb3->setProgress(timeWarp - 2.0f);
        _pTimeBomb4->setProgress(0.0f);

        _pTimeBomb1->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb2->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb3->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb4->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
    }
    else if(timeWarp >= 1.0f)
    {
        _pTimeBomb1->setProgress(1.0f);
        _pTimeBomb2->setProgress(timeWarp - 1.0f);
        _pTimeBomb3->setProgress(0.0f);
        _pTimeBomb4->setProgress(0.0f);

        _pTimeBomb1->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
        _pTimeBomb2->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb3->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb4->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
    }
    else
    {
        _pTimeBomb1->setProgress(timeWarp);
        _pTimeBomb2->setProgress(0.0f);
        _pTimeBomb3->setProgress(0.0f);
        _pTimeBomb4->setProgress(0.0f);

        _pTimeBomb1->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb2->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb3->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
        _pTimeBomb4->setOwnColor(Core::Vector4f(0.2f, 0.2f, 0.6f, 1.0f));
    }

    if(_totalElapsed <= START_TIMER && _displayLevelLabel)
    {
        Core::Vector2f dialogSize = _pScreenLevelDialog->getSize();
        dialogSize.x = (float)_appContext.pRenderer->getDefaultView()->getWidth();
        _pScreenLevelDialog->setSize(dialogSize);
        _pScreenLevelDialog->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()/4.0f));

        _pLevelLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pLevelLabel->setPosition(Core::Vector2f(0.0f, 48));
        _pLevelLabel->setHLayout(GUI::HL_CENTER);

        if(!_levelLabelVisible)
        {
            Core::String levelLabel = (*_appContext.pDico)[L"level"];
            levelLabel << L" : ";
            if(_levelNumber < 10)
                levelLabel << L"0";
            levelLabel << Core::toString(_levelNumber);
            _pLevelLabel->setText(levelLabel);
            _appContext.pGUI->pushDialog(_pScreenLevelDialog);
            _levelLabelVisible = true;
            _pScreenLevelDialog->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        }

        float alpha = 1.0f;
        if(_totalElapsed < 1.0f)
            alpha = (float)_totalElapsed;
        else
            alpha = Core::clamp(0.0f, 1.0f, START_TIMER - ((float)_totalElapsed + 0.5f));
        _pScreenLevelDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));
    }
    else
    {
        if(_levelLabelVisible)
        {
            _appContext.pGUI->popDialog();
            _levelLabelVisible = false;
        }
    }

    manageVictory();
}

void GameModeLevels::initLevelCompleteBand()
{
    //Level complete band
    _victory = 0.0f;
    _victoryVisible = false;
    _pScreenVictoryDialog = _appContext.pWidgetFactory->createMediumBand(0, 0);

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

    _pScreenVictoryDialog->addChild(_pLevelCompleteLabel);
    _pScreenVictoryDialog->addChild(_pYourTimeLabel);
    _pScreenVictoryDialog->addChild(_pBestTimeLabel);
    _pScreenVictoryDialog->addChild(_pRankLabel);
    _pScreenVictoryDialog->addChild(_pRankStarsL);
    _pScreenVictoryDialog->addChild(_pRankStarsR);
}

void GameModeLevels::manageVictory()
{
    if(_hasPlayerEnteredHisName && _gameCurrentState == GAME_ENTER_NAME)
        _gameCurrentState = GAME_NOT_STARTED;

    if(_victory > 0.0f)
    {
        Core::Vector2f dialogSize = _pScreenVictoryDialog->getSize();
        dialogSize.x = (float)_appContext.pRenderer->getDefaultView()->getWidth();
        _pScreenVictoryDialog->setSize(dialogSize);
        _pScreenVictoryDialog->setPosition(Core::Vector2f(0.0f,(float)_appContext.pRenderer->getDefaultView()->getHeight()/4.0f));

        _pLevelCompleteLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pLevelCompleteLabel->setPosition(Core::Vector2f(0.0f, 20));
        _pLevelCompleteLabel->setHLayout(GUI::HL_CENTER);

        _pYourTimeLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pYourTimeLabel->setPosition(Core::Vector2f(0.0f, 86));
        _pYourTimeLabel->setHLayout(GUI::HL_CENTER);

        _pBestTimeLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pBestTimeLabel->setPosition(Core::Vector2f(0.0f, 138));
        _pBestTimeLabel->setHLayout(GUI::HL_CENTER);

        _pRankLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
        _pRankLabel->setPosition(Core::Vector2f(0.0f, 205));
        _pRankLabel->setHLayout(GUI::HL_CENTER);

        if(!_victoryVisible)
        {
            int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

            int32 bestTimeMsec = -1;
            if(_appContext.pHighScores->isLevelHighScore(_levelNumber-1, totalElapsed))
            {
                _appContext.pHighScores->addLevelHighScore(_levelNumber-1, totalElapsed, _appContext.pGameSettings->getPlayerName());
                _appContext.saveScores = true;
            }

            bestTimeMsec = _appContext.pHighScores->getLevelHighScores(_levelNumber-1)[0].score;

            Core::String yourTime = (*_appContext.pDico)[L"your-time"];
            Core::String bestTime = (*_appContext.pDico)[L"best-time"];
            Core::String rank     = (*_appContext.pDico)[L"rank"];

            int32 maxSize = Core::max(yourTime.length(), bestTime.length(), rank.length());
            while((int32)yourTime.length() < maxSize)
                yourTime << L" ";
            yourTime << L"  :  "<<GameTools::msecTimerToString(totalElapsed);

            while((int32)bestTime.length() < maxSize)
                bestTime << L" ";
            bestTime << L"  :  "<<GameTools::msecTimerToString(bestTimeMsec);

            ELevelRank levelRank = _appContext.pLevelRanks->getRank(_levelNumber, totalElapsed);
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

            _pRankStarsL->setPosition(Core::Vector2f(centerx - 0.5f * textSize - 48.0f, 205.0f));
            _pRankStarsR->setPosition(Core::Vector2f(centerx + 0.5f * textSize + 16.0f, 205.0f));

            _pRankStarsL->setPicture(starPic);
            _pRankStarsR->setPicture(starPic);

            _appContext.pGUI->pushDialog(_pScreenVictoryDialog);
            _pScreenVictoryDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            _victoryVisible = true;
        }
        
        float alpha = 1.0f;
        if(VICTORY_PANEL_TIMER - _victory <= 1.0f)
            alpha = VICTORY_PANEL_TIMER - _victory;
        else
        {
            alpha = Core::clamp(0.0f, 1.0f, _victory - 0.5f);
            if(!_musicFadedOut && _victory < 1.0f)
            {
                if(_appContext.pGameMusic != null)
                {
                    _appContext.pGameMusic->fadeOut(1.0f);
                }
                _musicFadedOut = true;
            }
        }
        
        _pScreenVictoryDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));
    }
    else
    {
        if(_victoryVisible)
        {
            _appContext.pGUI->popDialog();
            _victoryVisible = false;
            //Initialiser niveau suivant
            _levelNumber++;
            if(GameTools::isLevelValid(_levelNumber))
            {
                _pPlayer->resetPowerLevelAndTimeWarp();
                setLevel(_levelNumber);
                _gameCurrentState = GAME_NOT_STARTED;
            }
            else
            {
                _pPlayer->resetPowerLevelAndTimeWarp();
                _appContext.wantedMode = CREDITS_MODE;
                _appContext.wantedLevel = -1;
                _appContext.pGameSettings->setCurrentLevel(-1);
            }
        }
    }
}

void GameModeLevels::update(double elapsed)
{
    manageGUI();

    if(!_gamePaused)
    {
        if(_gameCurrentState != GAME_ENTER_NAME)
            _totalElapsed += elapsed * _pTimeWarp->getTimeWarpFactor();

        if((_totalElapsed-START_TIMER)*1000 > _msecToClearLevel && !(_gameCurrentState == GAME_WON || _gameCurrentState == GAME_ENTER_NAME))
        {
            _gameCurrentState = GAME_LOST;
            _pDeathPenalty->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        }

        if(_gameCurrentState != GAME_LOST && _totalElapsed > START_TIMER && _waveOrders.size() > 0)
        {
            if(_gameCurrentState == GAME_NOT_STARTED)
                _gameCurrentState = GAME_STARTED;

            if( !_waveOrders.front().launched)
            {
                Ptr<Wave> pWave(new Wave(_registeredWaves[_waveOrders.front().id]));
                _waves.push_back(pWave);
                _waveOrders.front().launched = true;
            }
            else
            {
                _waveOrders.front().duration -= (float)elapsed * _pTimeWarp->getTimeWarpFactor();
                if(_waveOrders.front().duration <= 0.0f)
                {
                    _waveOrders.pop_front();
                }
                else
                {
                    Core::List<Ptr<Foe> >::const_iterator iFoe =_foes.begin();
                    bool allDead = true;
                    while(allDead && iFoe != _foes.end())
                    {
                        allDead = ((*iFoe)->getState() == FOE_DESTRUCTION_IN_PROGRESS || (*iFoe)->getState() == FOE_DELETE_PENDING);
                        ++iFoe;
                    }
                    if(allDead)
                        _waveOrders.pop_front();
                }
            }

            Core::List<Ptr<Wave> >::iterator iWave = _waves.begin();
            while(iWave != _waves.end())
            {
                (*iWave)->update(elapsed * _pTimeWarp->getTimeWarpFactor());
                if((*iWave)->finished())
                    iWave = _waves.erase(iWave);
                else
                    ++iWave;
            }
        }

        if( _gameCurrentState == GAME_STARTED 
            && _waveOrders.size() == 0 
            && _foes.size() == 0 
            && _victory == 0.0f)//plus de vague et plus d'ennemi <=> VICTORY
        {
            _victory = VICTORY_PANEL_TIMER;
            _gameCurrentState = GAME_WON;

            for(int32 iShot = 0; iShot < _foeShots.size(); ++iShot)
            {
                _foeShots[iShot]->kill(false);
            }
            
            for(int32 iPowerUp = 0; iPowerUp < _powerUps.size(); ++iPowerUp)
            {
                _powerUps[iPowerUp]->selfDestruct();
            }

            //ACHIEVEMENTS

            if(_levelNumber == -1)
            {
                if(_achievementCreditsHunter)
                    displayAchievement(CREDITS_HUNTER);
            }
            else if(_levelNumber >= 1 && _levelNumber <= 20)
            {
                if(_levelNumber == 1)
                {
                    displayAchievement(FIRST_STEP);
                }
                if(_levelNumber == 20)
                {
                    displayAchievement(MISSION_COMPLETE);

                    if(_achievementMarathon)
                        displayAchievement(MARATHON);
                }

                int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));
                int32 remainingTime = std::max(0, _msecToClearLevel - totalElapsed);

                if(remainingTime < 5000)
                {
                    displayAchievement(JUST_IN_TIME);
                }

                if(_achievementBareHanded)
                {
                    displayAchievement(BARE_HANDED);
                }

                if(_achievementPerfect)
                {
                    displayAchievement(PERFECT);
                }

                ELevelRank thisGameRank = _appContext.pLevelRanks->getRank(_levelNumber, totalElapsed);
                if(isAtLeast(RANK_VETERAN, thisGameRank))
                {
                    bool allVeteran = true;
                    bool allExpert  = true;
                    bool allElite   = true;
                    for(int32 iLevel = 0; iLevel < 20; ++iLevel)
                    {
                        if(iLevel != _levelNumber-1)
                        {
                            ELevelRank rank = _appContext.pLevelRanks->getRank(iLevel+1, _appContext.pHighScores->getLevelHighScores(iLevel)[0].score);
                            if(allVeteran && !isAtLeast(RANK_VETERAN, rank))
                                allVeteran = false;
                            if(allExpert && !isAtLeast(RANK_EXPERT, rank))
                                allExpert = false;
                            if(allElite && !isAtLeast(RANK_ACE, rank))
                                allElite = false;
                        }
                    }
                    
                    if(allVeteran && isAtLeast(RANK_VETERAN, thisGameRank))
                        displayAchievement(VETERAN);
                    if(allExpert && isAtLeast(RANK_EXPERT, thisGameRank))
                        displayAchievement(EXPERT);
                    if(allElite && isAtLeast(RANK_ACE, thisGameRank))
                        displayAchievement(ELITE);
                }
            }
        }
        
        if(_victory > 0.0f && _unlockAchievements.size() <= 0)
        {
            _victory = std::max(0.0f, _victory - (float)elapsed);
        }

#if USE_VORTEX
        _pVortex->startParallelUpdate(elapsed);
        //_pVortex2->update(elapsed);
#endif

        if(_pPlayer != null && _pPlayer->isInitialized() && _pLevel != null)
        {
            if(     _gameCurrentState != GAME_LOST 
                &&  _gameCurrentState != GAME_ENTER_NAME 
                &&  _pPlayer->isCannonReady(elapsed) 
                &&  _pPlayer->isShooting() 
                && !_pPlayer->isDead())
            {
                _pPlayer->fire();
            }

            manageTimeWarp(elapsed);
            
            _pPlayer->killDeadShots();
        }
    }

    GameMode::update(elapsed);

#if USE_VORTEX
    _pVortex->endParallelUpdate();
#endif
}

#if LM_DISPLAY_FPS
void GameModeLevels::createFoe(int32 foeId)
{
    clearWaves();
    setLevel(foeId);
}
#endif

void GameModeLevels::displayPowerUp()
{
    _achievementBareHanded = false;
    _powerUpAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerUpLabel->getColor();
    color.a = 1.0f;
    _pPowerUpLabel->setOwnColor(color);
}

void GameModeLevels::displayPowerDown()
{
    _powerDownAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerDownLabel->getColor();
    color.a = 1.0f;
    _pPowerDownLabel->setOwnColor(color);
}

void GameModeLevels::displayTimeWarpBonus()
{
    _timeWarpBonusAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pTimeWarpBonusLabel->getColor();
    color.a = 1.0f;
    _pTimeWarpBonusLabel->setOwnColor(color);
}

void GameModeLevels::deathPenalty()
{
    _achievementPerfect = false;

    _totalElapsed += 30;
    _deathPenaltyAppearance = (float)_totalElapsed;

    _pDeathPenalty->setText(GameTools::msecTimerToString(-30 * 1000, true));
    _pDeathPenalty->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));
    _pDeathPenalty->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 1.5f);
    _pDeathPenalty->setPosition(Core::Vector2f(0.0f, 100.0f));
    _pDeathPenalty->setHLayout(GUI::HL_CENTER);
}

}//namespace ChronoRage