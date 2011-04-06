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
#include "GameModeDemo.h"

#include <Core/Math/Random.h>
#include <Universe/Tasks/PostFX.h>

#include <GameTools.h>
#include <Foes/BonusCarrier.h>

#define USE_VORTEX 1

namespace ChronoRage
{

const Core::String GAMEMODE_WORLD_FILE = L"/ChronoRageLevels/world01.world";
const float MIN_TIME_BETWEEN_TWO_WAVES = 0.5f;

const int32 START_POWER_LEVEL_DEMO = 0;

const int32 DEMO_TIMER = 180;

GameModeDemo::GameModeDemo(AppContext & appContext)
:   GameMode(appContext),
    _changeTimerColor(false),
    _totalElapsed(0.0f),
    _nbLives(3),
    _powerLevel(START_POWER_LEVEL_DEMO),
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
}

void GameModeDemo::registerWaveFile(const Core::String & waveFile)
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

void GameModeDemo::restart()
{
    _gameCurrentState = GAME_NOT_STARTED;
    initialize();
    
    GameMode::restart();
}

void GameModeDemo::initialize()
{
    GameMode::initialize();

    _fadeToBlack = -1.0f;

    _msecToClearDemo = DEMO_TIMER * 1000;

    _wavesToBeCreated.clear();
    _createNextWaveTimer = 0.0f;

    _gameOverVisible = false;
    _totalElapsed = 0.0f;
    _demoLabelAppearance = 0.0f;
    _timerBlip = 0.0f;
    _nbLives = 3;

    _powerLevel = 0;
    _sameLevelCreated = 0;

    _waveDuration = 0.0f;

    _createBonus = 0.0f;
    _createBonusIndex = 0;

#ifdef CHRONORAGE_DEMO
    _appContext.shipDestroyed = 0;
#endif;

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

    _appContext.pSoundLibrary->playDemoMusic();
}

void GameModeDemo::update(double elapsed)
{
    manageGUI();
    manageMusics();
    
    if(!_gamePaused)
    {
        if((_totalElapsed-START_TIMER)*1000 > _msecToClearDemo)
        {
            if(_fadeToBlack < 0.0f)
            {
                _fadeToBlack = 0.0f;
                _pDeathPenalty->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
                Ptr<Universe::ITask> pTaskFadeToBlack(new Universe::TaskFXFade( _appContext.pCameraNode,
                                                                                Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f),
                                                                                Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f),
                                                                                1.0,
                                                                                Core::INTER_LINEAR));
                
                _appContext.pWorld->registerTask(pTaskFadeToBlack);
                _pScreenDialog->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 1.0f);
            }

            _fadeToBlack += (float)elapsed;

            if(_fadeToBlack >= 1.0f)
            {
                _appContext.wantedMode = MENU_MODE;
                _appContext.wantedScreen = BUY_GAME_SCREEN;
            }
            
        }

        if(_totalElapsed <= START_TIMER)
        {
            Core::Vector2f dialogSize = _pScreenDemoDialog->getSize();
            dialogSize.x = (float)_appContext.pRenderer->getDefaultView()->getWidth();
            _pScreenDemoDialog->setSize(dialogSize);
            _pScreenDemoDialog->setPosition(Core::Vector2f(0.0f, (float)_appContext.pRenderer->getDefaultView()->getHeight()/4.0f));

            _pDemoLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), 10.0f));
            _pDemoLabel->setPosition(Core::Vector2f(0.0f, 48));
            _pDemoLabel->setHLayout(GUI::HL_CENTER);

            if(!_demoLabelVisible)
            {
                _appContext.pGUI->pushDialog(_pScreenDemoDialog);
                _demoLabelVisible = true;
                _pScreenDemoDialog->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            }

            float alpha = 1.0f;
            if(_totalElapsed - _demoLabelAppearance < 1.0f)
                alpha = (float)(_totalElapsed - _demoLabelAppearance);
            else
                alpha = Core::clamp(0.0f, 1.0f, START_TIMER - ((float)_totalElapsed - _demoLabelAppearance + 0.5f));
            _pScreenDemoDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));
        }
        else
        {
            if(_demoLabelVisible)
            {
                _appContext.pGUI->popDialog();
                _demoLabelVisible = false;
            }
        }

        if(_gameCurrentState == GAME_NOT_STARTED || _gameCurrentState == GAME_STARTED)
        {
            _totalElapsed += elapsed * _pTimeWarp->getTimeWarpFactor();
            _waveDuration -= float(elapsed * _pTimeWarp->getTimeWarpFactor());
            _createBonus  += float(elapsed * _pTimeWarp->getTimeWarpFactor());
        }

        if(_gameCurrentState != GAME_LOST && _totalElapsed > START_TIMER)
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
                    _totalElapsed -= _waveDuration;
                    
                    _timeBonusAppearance = (float)_totalElapsed;

                    _demoLabelAppearance -= _waveDuration;
                    _deathPenaltyAppearance -= _waveDuration;
                    _powerUpAppearance -= _waveDuration;
                    _powerDownAppearance -= _waveDuration;
                    _timeWarpBonusAppearance -= _waveDuration;
                    _timerBlip -= _waveDuration;

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

                if(_sameLevelCreated == 2)
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
void GameModeDemo::createFoe(int32 foeId)
{}

float GameModeDemo::getPowerLevel()
{
    return (float)_powerLevel;
}
#endif

void GameModeDemo::displayPowerUp()
{
    _powerUpAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerUpLabel->getColor();
    color.a = 1.0f;
    _pPowerUpLabel->setOwnColor(color);
}

void GameModeDemo::displayPowerDown()
{
    _powerDownAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pPowerDownLabel->getColor();
    color.a = 1.0f;
    _pPowerDownLabel->setOwnColor(color);
}

void GameModeDemo::displayTimeWarpBonus()
{
    _timeWarpBonusAppearance =  (float)_totalElapsed;
    Core::Vector4f color = _pTimeWarpBonusLabel->getColor();
    color.a = 1.0f;
    _pTimeWarpBonusLabel->setOwnColor(color);
}

void GameModeDemo::deathPenalty()
{
    _totalElapsed += 30;
    _deathPenaltyAppearance = (float)_totalElapsed;

    _pDeathPenalty->setText(GameTools::msecTimerToString(-30 * 1000, true));
    _pDeathPenalty->setOwnColor(Core::Vector4f(1.0f, 0.25f, 0.25f, 1.0f));
    _pDeathPenalty->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), 1.5f);
    _pDeathPenalty->setPosition(Core::Vector2f(0.0f, 100.0f));
    _pDeathPenalty->setHLayout(GUI::HL_CENTER);
}

void GameModeDemo::foeKilled(EFoeType type)
{
#ifdef CHRONORAGE_DEMO
    _appContext.shipDestroyed++;
#endif
    GameMode::foeKilled(type);
}

void GameModeDemo::initVortex()
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

void GameModeDemo::initGUI()
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

    _pDeathPenalty = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pDeathPenalty->setFont(L"/ChronoRageData/Font/in-game-digits.font.xml");
    _pDeathPenalty->setText(L"00:00.000");
    _pDeathPenalty->setSize(Core::Vector2f( (float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pDeathPenalty->setPosition(Core::Vector2f(0.0f, 100.0f));
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
    _pScreenDialog->addChild(_pTimeBonus);
    _pScreenDialog->addChild(_pPowerLevel);
    _pScreenDialog->addChild(_pPowerUpLabel);
    _pScreenDialog->addChild(_pPowerDownLabel);
    _pScreenDialog->addChild(_pTimeWarpBonusLabel);
    _pScreenDialog->addChild(_pDeathPenalty);
    _pScreenDialog->addChild(_pTimeBomb1);
    _pScreenDialog->addChild(_pTimeBomb2);
    _pScreenDialog->addChild(_pTimeBomb3);
    _pScreenDialog->addChild(_pTimeBomb4);

    //Survival start band
    _demoLabelVisible = false;
    _pScreenDemoDialog = _appContext.pWidgetFactory->createThinBand(0, 0);
    _pDemoLabel = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pDemoLabel->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pDemoLabel->setText((*_appContext.pDico)[L"demo-text"]);
    _pDemoLabel->setOwnColor(Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f));
    _pScreenDemoDialog->addChild(_pDemoLabel);
}

void GameModeDemo::manageGUI()
{
    int32 totalElapsed = std::max(0, (int32)((_totalElapsed-START_TIMER)*1000));

    int32 remainingTime = std::max(0, _msecToClearDemo - totalElapsed);

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

    Core::String timerLabel = GameTools::msecTimerToString(remainingTime);
    _pTimerLabel->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pTimerLabel->setPosition(Core::Vector2f(0.0f, 10.0f));
    _pTimerLabel->setHLayout(GUI::HL_CENTER);
    _pTimerLabel->setText(timerLabel);

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
            if(blinkTime < 5)
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

void GameModeDemo::createBonus()
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

void GameModeDemo::manageMusics()
{
    if(_appContext.pGameMusic != null && _appContext.pGameMusic->playbackCompleted())
    {
        _appContext.pSoundLibrary->playDemoMusic();
    }

    if(_appContext.pMenuMusic != null && !_appContext.pMenuMusic->isAlive())
    {
        _appContext.pMenuMusic = null;
    }
}

void GameModeDemo::manageEscapeKey()
{
    if(_fadeToBlack < 0.0f)
        GameMode::manageEscapeKey();
}

}//namespace ChronoRage