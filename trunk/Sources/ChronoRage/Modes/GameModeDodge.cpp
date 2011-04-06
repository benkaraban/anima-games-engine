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
#include "GameModeDodge.h"

#include <Core/Math/Random.h>

#include <GameTools.h>

#define USE_VORTEX 1

namespace ChronoRage
{

const Core::String GAMEMODE_WORLD_FILE = L"/ChronoRageLevels/world01.world";
const float MIN_TIME_BETWEEN_TWO_WAVES = 0.5f;

const int32 START_POWER_LEVEL_NORMAL = 0;

GameModeDodge::GameModeDodge(AppContext & appContext)
:   GameMode(appContext),
    _totalElapsed(0.0f),
    _nbLives(3),
    _powerLevel(START_POWER_LEVEL_NORMAL),
    _sameLevelCreated(0),
    _waveDuration(0.0f)
{
    _foeCollisionsEnabled = false;
}

void GameModeDodge::registerWaveFile(const Core::String & waveFile)
{
    _activeWaves.clear();
    _registeredWavesByPowerLevel.clear();
    GameMode::registerWaveFile(waveFile);

    //On trie les vagues par niveau de puissance
    Core::Map<int32, Ptr<Wave> >::const_iterator iWave = _registeredWaves.begin();
    while(iWave != _registeredWaves.end())
    {
        _registeredWavesByPowerLevel[iWave->second->getPowerLevel()].push_back(iWave->second);

        ++iWave;
    }
}

void GameModeDodge::restart()
{
    _gameCurrentState = GAME_NOT_STARTED;
    initialize();
    
    GameMode::restart();
}

void GameModeDodge::initialize()
{
    GameMode::initialize();

    _wavesToBeCreated.clear();
    _createNextWaveTimer = 0.0f;

    _gameOverVisible = false;
    _totalElapsed = 0.0f;
    _nbLives = 3;
    _timeWarpBonus = 0.0;

    _powerLevel = 0;
    _sameLevelCreated = 0;

    _waveDuration = 0.0f;

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
    initPlayer(false);

    _pTimeWarp = Ptr<TimeWarp>(new TimeWarp(_appContext.pWorld, _appContext.pCameraNode, _pPlayer));

    for(int32 ii=0; ii<HELP_FIND_REMAINING_FOES; ++ii)
    {
        _foeDirections[ii].initialize(_appContext.pWorld);
    }

    initVortex();
    initGUI();

    if(_appContext.pGameSettings->getMusicVolume() >= 0.01f)
    {
        _appContext.pSoundLibrary->playSurvivalMusic();
    }
}

void GameModeDodge::update(double elapsed)
{
    manageGUI();
    manageMusics();

    if(_hasPlayerEnteredHisName && _gameCurrentState != GAME_LOST)
    {
        _appContext.pHighScores->addSurvivalHighScore(HSST_DODGE, int32((_totalElapsed - START_TIMER)*1000), _appContext.pGameSettings->getPlayerName());
        _appContext.saveScores = true;
        _gameCurrentState = GAME_LOST;
    }

    if(!_gamePaused)
    {
        if(_totalElapsed <= START_TIMER)
        {
            Core::Vector2f dialogSize = _pScreenBestTimeDialog->getSize();
            dialogSize.x = (float)_appContext.pRenderer->getDefaultView()->getWidth();
            _pScreenBestTimeDialog->setSize(dialogSize);
            _pScreenBestTimeDialog->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()/4.0f));

            _pBestTimeLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
            _pBestTimeLabel->setPosition(Core::Vector2f(0.0f, 48));
            _pBestTimeLabel->setHLayout(GUI::HL_CENTER);

            if(!_bestTimeLabelVisible)
            {
                int32 bestTimeMsec = -1;
                
                bestTimeMsec = _appContext.pHighScores->getSurvivalHighScores(HSST_DODGE)[0].score;
                
                Core::String bestTimeLabel = (*_appContext.pDico)[L"best-time"];
                bestTimeLabel << L" : " << GameTools::msecTimerToString(bestTimeMsec);

                _pBestTimeLabel->setText(bestTimeLabel);
                _appContext.pGUI->pushDialog(_pScreenBestTimeDialog);
                _bestTimeLabelVisible = true;
                _pScreenBestTimeDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            }

            float alpha = 1.0f;
            if(_totalElapsed < 1.0f)
                alpha = (float)_totalElapsed;
            else
                alpha = Core::clamp(0.0f, 1.0f, START_TIMER - ((float)_totalElapsed + 0.5f));
            _pScreenBestTimeDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));
        }
        else
        {
            if(_bestTimeLabelVisible)
            {
                _appContext.pGUI->popDialog();
                _bestTimeLabelVisible = false;
            }
        }

        if(_gameCurrentState != GAME_ENTER_NAME)
        {
            _totalElapsed += elapsed * _pTimeWarp->getTimeWarpFactor();
        }

        if(_gameCurrentState == GAME_NOT_STARTED || _gameCurrentState == GAME_STARTED)
        {
            _waveDuration -= float(elapsed * _pTimeWarp->getTimeWarpFactor());
        }

        if(_gameCurrentState != GAME_LOST && _totalElapsed > START_TIMER)
        {
            if(_gameCurrentState == GAME_NOT_STARTED)
            {   
                _gameCurrentState = GAME_STARTED;
            }

            _timeWarpBonus += elapsed * _pTimeWarp->getTimeWarpFactor();
            if(_timeWarpBonus > 1.0 && _gameCurrentState != GAME_LOST && _gameCurrentState != GAME_ENTER_NAME)
            {
                _timeWarpBonus = 0.0;

                foeKilledTimeWarpBonus();
            }

            Core::List<Ptr<Wave> >::iterator iWave = _activeWaves.begin();
            while(iWave != _activeWaves.end())
            {
                (*iWave)->update(elapsed * _pTimeWarp->getTimeWarpFactor());
                if((*iWave)->finished())
                    iWave = _activeWaves.erase(iWave);
                else
                    ++iWave;
            }

            int32 activeFoes = 0;
            Core::List<Ptr<Foe> >::const_iterator iFoe = _foes.begin();

            while(iFoe != _foes.end())
            {
                if((*iFoe)->getState() == FOE_CREATION_IN_PROGRESS || (*iFoe)->isAlive())
                    ++activeFoes;

                ++iFoe;
            }

            if(activeFoes == 0 || _waveDuration <= 0.0f)
            {
                //Time bonus
                if(_waveDuration > 0.0f && !_pPlayer->isDead())
                {
                    _totalElapsed += _waveDuration;
                    
                    _timeBonusAppearance = (float)_totalElapsed;

                    _pTimeBonus->setText(GameTools::msecTimerToString(int32(_waveDuration * 1000)));
                    Core::Vector4f color = _pTimeBonus->getColor();
                    color.a = 1.0f;
                    _pTimeBonus->setOwnColor(color);
                }

                int32 powerLevelMax = std::min(_powerLevel, _registeredWavesByPowerLevel.size()-1);
                    
                int32 randPower = 0;
                while(randPower < _powerLevel)
                {
                    int32 randMax = std::min(powerLevelMax, _powerLevel - randPower);
                    int32 randMin = std::max(1, randMax/2);
                    int32 tempRandPower = Core::irand(randMin, randMax);
                    
                    LM_ASSERT(_registeredWavesByPowerLevel[tempRandPower].size() != 0);
                    int32 randWave = Core::irand(0, _registeredWavesByPowerLevel[tempRandPower].size() - 1);

                    _wavesToBeCreated.push_back(_registeredWavesByPowerLevel[tempRandPower][randWave]->getWaveId());
                    Ptr<Wave> pWave(new Wave(_registeredWavesByPowerLevel[tempRandPower][randWave]));
                    _waveDuration = std::max(_waveDuration, pWave->getDuration());

                    randPower += tempRandPower;
                }

                _sameLevelCreated++;

                if(_sameLevelCreated == 3)
                {
                    _powerLevel++;
                    _sameLevelCreated = 0;
                }
            }

            _createNextWaveTimer = std::max(0.0f, _createNextWaveTimer - (float)elapsed);
            if(_wavesToBeCreated.size() > 0 && _createNextWaveTimer <= 0.0f)
            {
                Ptr<Wave> pWave(new Wave(_registeredWaves[_wavesToBeCreated.front()]));

                _wavesToBeCreated.erase(_wavesToBeCreated.begin());
                
                _activeWaves.push_back(pWave);
                _createNextWaveTimer = MIN_TIME_BETWEEN_TWO_WAVES;
            }
        }

#if USE_VORTEX
        _pVortex->startParallelUpdate(elapsed);
        //_pVortex2->update(elapsed);
#endif

        if(_pPlayer != null && _pPlayer->isInitialized() && _pLevel != null)
        {
            manageTimeWarp(elapsed);
        }
    }

    GameMode::update(elapsed);

#if USE_VORTEX
    _pVortex->endParallelUpdate();
#endif
}

#if LM_DISPLAY_FPS
void GameModeDodge::createFoe(int32 foeId)
{}

float GameModeDodge::getPowerLevel()
{
    return (float)_powerLevel;
}
#endif

void GameModeDodge::displayPowerUp()
{}

void GameModeDodge::displayPowerDown()
{}

void GameModeDodge::displayTimeWarpBonus()
{
    _timeWarpBonusAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pTimeWarpBonusLabel->getColor();
    color.a = 1.0f;
    _pTimeWarpBonusLabel->setOwnColor(color);
}

void GameModeDodge::deathPenalty()
{
    _nbLives--;
    if(_nbLives == 0)
    {
        if(_appContext.pHighScores->isSurvivalHighScore(HSST_DODGE, int32(_totalElapsed - START_TIMER)*1000))
            _gameCurrentState = GAME_ENTER_NAME;
        else
            _gameCurrentState = GAME_LOST;
    }
    else
    {
        _pPlayer->resetTimeWarp();
        _timeWarpBonus = 0.0;
    }
}

const Core::List<Ptr<IShot> > & GameModeDodge::getPlayerShots() const
{
    return _foeShots;
}

void GameModeDodge::initVortex()
{
    VortexSettings settings;
    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2000;
    settings.cubeGlow       = 0.5f;

    settings.armCount        = 6;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = true;

    settings.color0          = Core::Vector4f(0.5f, 0.3f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.5f, 0.0f, 0.0f, 1.0f);
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

    settings.color0          = Core::Vector4f(0.10f, 0.0f, 0.0f, 1.0f);
    settings.color1          = Core::Vector4f(0.05f, 0.f, 0.0f, 1.0f);
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

void GameModeDodge::initGUI()
{
    _pTimerLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimerLabel->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pTimerLabel->setText(L"00:00.000");
    _pTimerLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pTimeBonus = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeBonus->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pTimeBonus->setText(L"00:00.000");
    _pTimeBonus->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pTimeBonus->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimeBonus->setPosition(Core::Vector2f(0.0f, 200.0f));
    _pTimeBonus->setHLayout(GUI::HL_CENTER);

    _pTimeWarpBonusLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pTimeWarpBonusLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pTimeWarpBonusLabel->setText(L"TIME WARP BONUS");
    _pTimeWarpBonusLabel->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 0.0f));
    _pTimeWarpBonusLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimeWarpBonusLabel->setPosition(Core::Vector2f(0.0f, 75.0f));
    _pTimeWarpBonusLabel->setHLayout(GUI::HL_CENTER);

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

    _pLifePicture = Ptr<GUI::Picture>(new GUI::Picture(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLifePicture->setPicture(L"/ChronoRageData/GUI/life-icon.tga");
    _pLifePicture->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    _pLifeCountLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLifeCountLabel->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pLifeCountLabel->setText(L"*3");
    _pLifeCountLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pScreenDialog->addChild(_pTimerLabel);
    _pScreenDialog->addChild(_pTimeWarpBonusLabel);
    _pScreenDialog->addChild(_pTimeBonus);
    _pScreenDialog->addChild(_pTimeBomb1);
    _pScreenDialog->addChild(_pTimeBomb2);
    _pScreenDialog->addChild(_pTimeBomb3);
    _pScreenDialog->addChild(_pTimeBomb4);
    _pScreenDialog->addChild(_pLifePicture);
    _pScreenDialog->addChild(_pLifeCountLabel);

    //Survival start band
    _bestTimeLabelVisible = false;
    _pScreenBestTimeDialog = _appContext.pWidgetFactory->createThinBand(0, 0);
    _pBestTimeLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pBestTimeLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    String bestTime;
    bestTime << (*_appContext.pDico)[L"best-time"] << L" : " << GameTools::msecTimerToString(-1);
    _pBestTimeLabel->setText(bestTime);
    _pBestTimeLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    _pScreenBestTimeDialog->addChild(_pBestTimeLabel);
}

void GameModeDodge::manageGUI()
{
    int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

    if(_gameCurrentState == GAME_STARTED && totalElapsed > BULLET_DANCER_TIME)
        displayAchievement(BULLET_DANCER);

    _pTimerLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimerLabel->setPosition(Core::Vector2f(0.0f, 10.0f));
    _pTimerLabel->setHLayout(GUI::HL_CENTER);

    if(_gameCurrentState == GAME_NOT_STARTED || _gameCurrentState == GAME_STARTED)
    {
        Core::String timerLabel = GameTools::msecTimerToString(totalElapsed);
        _pTimerLabel->setText(timerLabel);
    }

    Core::Vector4f timeBonusColor = _pTimeBonus->getColor();
    if(timeBonusColor.a > 0.0f)
    {
        _pTimeBonus->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
        Core::Vector2f timeBonusPosition;
        float alpha = 1.0f-((float)(_totalElapsed - _timeBonusAppearance))/1.5f;
        timeBonusPosition.setLerp(Core::Vector2f(0.0f, 75.0f), Core::Vector2f(0.0f, 200.0f), alpha);
        _pTimeBonus->setPosition(timeBonusPosition);
        _pTimeBonus->setHLayout(GUI::HL_CENTER);
        
        timeBonusColor.setLerp(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), Core::Vector4f(0.25f, 1.0f, 0.25f, 1.0f), alpha);
        _pTimeBonus->setOwnColor(timeBonusColor);
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

    _pLifePicture->setPosition(Core::Vector2f((float(_appContext.pRenderer->getDefaultView()->getWidth()) / 2.0f) - 300.0f, 15.0f));
    _pLifeCountLabel->setPosition(Core::Vector2f((float(_appContext.pRenderer->getDefaultView()->getWidth()) / 2.0f) - 250.0f, 20.0f));

    String lifeCountLabel;
    lifeCountLabel << L"*" << Core::toString(_nbLives);
    _pLifeCountLabel->setText(lifeCountLabel);


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
}

void GameModeDodge::manageMusics()
{
    if(_appContext.pGameMusic != null && _appContext.pGameMusic->playbackCompleted())
    {
        _appContext.pSoundLibrary->playSurvivalMusic();
    }

    if(_appContext.pMenuMusic != null && !_appContext.pMenuMusic->isAlive())
    {
        _appContext.pMenuMusic = null;
    }
}

}//namespace ChronoRage