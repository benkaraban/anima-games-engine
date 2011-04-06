/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#include "GameModeSurvival.h"

#include <Core/Math/Random.h>

#include <GameTools.h>
#include <Foes/BonusCarrier.h>

#define USE_VORTEX 1

namespace ChronoRage
{

const Core::String GAMEMODE_WORLD_FILE = L"/ChronoRageLevels/world01.world";
const float MIN_TIME_BETWEEN_TWO_WAVES = 0.5f;

const int32 START_POWER_LEVEL_NORMAL = 0;
const int32 START_POWER_LEVEL_HARD = START_POWER_LEVEL_NORMAL;

GameModeSurvival::GameModeSurvival(AppContext & appContext, bool hard)
:   GameMode(appContext),
    _hard(hard),
    _totalElapsed(0.0f),
    _nbLives(3),
    _powerLevel(START_POWER_LEVEL_NORMAL),
    _sameLevelCreated(0),
    _waveDuration(0.0f),
    _createBonus(0.0f),
    _createBonusIndex(0)
{
    //Première salve de bonus
    _bonusTimeLine.push_back(10.0f);
    _bonusTimeLine.push_back(20.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(65.0f);
    //Deuxième salve
    _bonusTimeLine.push_back(10.0f);
    _bonusTimeLine.push_back(20.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(15.0f);
    _bonusTimeLine.push_back(65.0f);

    if(_hard)
        _powerLevel = START_POWER_LEVEL_HARD;
}

void GameModeSurvival::registerWaveFile(const Core::String & waveFile)
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

void GameModeSurvival::restart()
{
    _gameCurrentState = GAME_NOT_STARTED;
    initialize();
    
    GameMode::restart();
}

void GameModeSurvival::initialize()
{
    GameMode::initialize();

    _wavesToBeCreated.clear();
    _createNextWaveTimer = 0.0f;

    _gameOverVisible = false;
    _totalElapsed = 0.0f;
    _nbLives = 3;

    _powerLevel = 0;
    _sameLevelCreated = 0;

    _waveDuration = 0.0f;

    _createBonus = 0.0f;
    _createBonusIndex = 0;

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

    _appContext.pSoundLibrary->playSurvivalMusic();
}

void GameModeSurvival::update(double elapsed)
{
    manageGUI();
    manageMusics();
    
    EHighScoreSurvivalType type = HSST_NORMAL;
    if(_hard)
        type = HSST_HARD;
    else
        type = HSST_NORMAL;

    if(_hasPlayerEnteredHisName && _gameCurrentState != GAME_LOST)
    {
        _appContext.pHighScores->addSurvivalHighScore(type, int32((_totalElapsed - START_TIMER)*1000), _appContext.pGameSettings->getPlayerName());
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
                
                bestTimeMsec = _appContext.pHighScores->getSurvivalHighScores(type)[0].score;
                
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
        if(_gameCurrentState != GAME_NOT_STARTED || _gameCurrentState == GAME_STARTED)
        {
            _waveDuration -= float(elapsed * _pTimeWarp->getTimeWarpFactor());
            _createBonus  += float(elapsed * _pTimeWarp->getTimeWarpFactor());
        }

        if(_gameCurrentState != GAME_ENTER_NAME && _gameCurrentState != GAME_LOST && _totalElapsed > START_TIMER)
        {
            if(_gameCurrentState == GAME_NOT_STARTED)
            {   
                _gameCurrentState = GAME_STARTED;
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

                if(_powerLevel == 0)
                {
                    LM_ASSERT(_registeredWavesByPowerLevel[_powerLevel].size() != 0);
                    
                    int32 randWave = Core::irand(0, _registeredWavesByPowerLevel[_powerLevel].size() - 1);
                    _wavesToBeCreated.push_back(_registeredWavesByPowerLevel[_powerLevel][randWave]->getWaveId());
                    Ptr<Wave> pWave(new Wave(_registeredWavesByPowerLevel[_powerLevel][randWave]));
                    _waveDuration = std::max(_waveDuration, pWave->getDuration());
                }
                else
                {
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
                }

                _sameLevelCreated++;

                if(_hard)
                {
                    if(_sameLevelCreated == 2)
                    {
                        _powerLevel++;
                        _sameLevelCreated = 0;
                    }
                }
                else
                {
                    if(_sameLevelCreated == 3)
                    {
                        _powerLevel++;
                        _sameLevelCreated = 0;
                    }
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

            //Creation de bonus
            if(_createBonus > _bonusTimeLine[_createBonusIndex])
            {
                createBonus();
            }
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
void GameModeSurvival::createFoe(int32 foeId)
{}

float GameModeSurvival::getPowerLevel()
{
    return (float)_powerLevel;
}
#endif

void GameModeSurvival::displayPowerUp()
{
    _powerUpAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerUpLabel->getColor();
    color.a = 1.0f;
    _pPowerUpLabel->setOwnColor(color);
}

void GameModeSurvival::displayPowerDown()
{
    _powerDownAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerDownLabel->getColor();
    color.a = 1.0f;
    _pPowerDownLabel->setOwnColor(color);
}

void GameModeSurvival::displayTimeWarpBonus()
{
    _timeWarpBonusAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pTimeWarpBonusLabel->getColor();
    color.a = 1.0f;
    _pTimeWarpBonusLabel->setOwnColor(color);
}

void GameModeSurvival::deathPenalty()
{
    _nbLives--;
    if(_nbLives == 0)
    {
        EHighScoreSurvivalType type = HSST_NORMAL;
        if(_hard)
            type = HSST_HARD;
        else
            type = HSST_NORMAL;

        if(_appContext.pHighScores->isSurvivalHighScore(type, int32(_totalElapsed - START_TIMER)*1000))
            _gameCurrentState = GAME_ENTER_NAME;
        else
            _gameCurrentState = GAME_LOST;
    }
    else
    {
        _pPlayer->resetTimeWarp();
    }
}

void GameModeSurvival::initVortex()
{
    VortexSettings settings;
    settings.cubeSize       = 2.0;
    settings.cubeCount      = 2500;
    settings.cubeGlow       = 0.0f;

    settings.armCount        = 6;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = true;

    if(_hard)
    {
        settings.color0          = Core::Vector4f(0.8f, 0.0f, 0.0f, 1.0f);
        settings.color1          = Core::Vector4f(0.1f, 0.05f, 0.05f, 1.0f),
        settings.colorByDepth    = true;
    }
    else
    {
        settings.color0          = Core::Vector4f(0.0f, 0.2f, 0.0f, 1.0f);
        settings.color1          = Core::Vector4f(0.0f, 0.1f, 0.0f, 1.0f),
        settings.colorByDepth    = true;
    }

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
    settings.cubeGlow       = 0.0f;

    settings.armCount        = 1;
    settings.armRelativeSize = 1.0f;

    settings.gaussianDensity = false;
    settings.cubeSelfRotationSpeed = 0.05f;

    if(_hard)
    {
        settings.color0          = Core::Vector4f(0.2f, 0.025f, 0.0f, 1.0f);
        settings.color1          = Core::Vector4f(0.2f, 0.025f, 0.0f, 1.0f);
    }
    else
    {
        settings.color0          = Core::Vector4f(0.0f, 0.05f, 0.025f, 1.0f);
        settings.color1          = Core::Vector4f(0.025f, 0.05f, 0.025f, 1.0f);
    }

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

void GameModeSurvival::initGUI()
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
    _pTimeWarpBonusLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimeWarpBonusLabel->setPosition(Core::Vector2f(0.0f, 75.0f));
    _pTimeWarpBonusLabel->setHLayout(GUI::HL_CENTER);

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

    _pLifePicture = Ptr<GUI::Picture>(new GUI::Picture(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLifePicture->setPicture(L"/ChronoRageData/GUI/life-icon.tga");
    _pLifePicture->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    _pLifeCountLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pLifeCountLabel->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pLifeCountLabel->setText(L"*3");
    _pLifeCountLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));

    _pScreenDialog->addChild(_pTimerLabel);
    _pScreenDialog->addChild(_pPowerLevel);
    _pScreenDialog->addChild(_pTimeBonus);
    _pScreenDialog->addChild(_pPowerUpLabel);
    _pScreenDialog->addChild(_pPowerDownLabel);
    _pScreenDialog->addChild(_pTimeWarpBonusLabel);
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

void GameModeSurvival::manageGUI()
{
    int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

    if(_gameCurrentState == GAME_STARTED)
    {
        if(_hard)
        {
            if(totalElapsed > IMMORTAL_TIME)
                displayAchievement(IMMORTAL);
        }
        else
        {
            if(totalElapsed > DIE_HARD_TIME)
                displayAchievement(DIE_HARD);
        }
    }

    _pTimerLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimerLabel->setPosition(Core::Vector2f(0.0f, 10.0f));
    _pTimerLabel->setHLayout(GUI::HL_CENTER);
    if(_gameCurrentState == GAME_STARTED)
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

    _pLifePicture->setPosition(Core::Vector2f((float(_appContext.pRenderer->getDefaultView()->getWidth()) / 2.0f) - 300.0f, 15.0f));
    _pLifeCountLabel->setPosition(Core::Vector2f((float(_appContext.pRenderer->getDefaultView()->getWidth()) / 2.0f) - 250.0f, 20.0f));

    String lifeCountLabel;
    lifeCountLabel << L"*" << Core::toString(_nbLives);
    _pLifeCountLabel->setText(lifeCountLabel);


    float powerLevel = (_pPlayer->getPowerLevel() - 1.0f)/(PLAYER_MAXIMUM_POWER - 1.0f);
    _pPowerLevel->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2)-104, (float)_appContext.pRenderer->getDefaultView()->getHeight()-30.0f));
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
}

void GameModeSurvival::createBonus()
{
    Ptr<Foe> pBonus = null;
    if(_pPlayer->getPowerLevel() >= 3.0f)
    {
        pBonus= Ptr<Foe>(new BonusCarrier(_appContext.pWorld, _pPlayer, *this, true));
    }
    else
    {
        pBonus= Ptr<Foe>(new BonusCarrier(_appContext.pWorld, _pPlayer, *this, false));
    }

    Core::Vector3f foePosition;
    foePosition.x = Core::frand(_pLevel->xMin(), _pLevel->xMax());
    foePosition.y = Core::frand(_pLevel->yMin(), _pLevel->yMax());
    foePosition.z = _pLevel->playgroundDepth();

    movePointInPlayground(foePosition);

    pBonus->initialize(foePosition);

    addFoe(pBonus);
    _createBonus -= _bonusTimeLine[_createBonusIndex];
    _createBonusIndex = (++_createBonusIndex) % _bonusTimeLine.size();
}

void GameModeSurvival::manageMusics()
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