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
#include "GameMode.h"

#include <Universe/Tasks/PostFX.h>
#include <Universe/Tasks/Shake.h>

#include <Controls/PlayerCtrlMouse.h>
#include <Controls/PlayerCtrlKeyboard.h>
#include <Controls/PlayerCtrlJoystick.h>

namespace ChronoRage
{
static const int32 FRAME_DURATION_HISTORY_SIZE = 30;

const float DISPLAY_ACHIEVEMENT_TIMER = 3.0f;

GameMode::GameMode(AppContext & appContext)
:   IGameState(appContext),
    _gameCurrentState(GAME_NOT_STARTED),
    _foeCollisionsEnabled(true),
    _pPlayer(null),
    _pLevel(null),
    _powerLevel(0)
{}

GameMode::~GameMode()
{
    if(!_frameDurationHistory.empty())
    {
        Core::List<float> history(_frameDurationHistory.begin(), _frameDurationHistory.end());
        std::sort(history.begin(), history.end());
        float median = history[history.size() / 2];
        float fps = 1000.0f / median;
        INF << L"Gameplay median frame duration : " << Core::toString(median) << L" ms ("
            << Core::toString(fps) << L" fps) from " << Core::toString(history.size()) << L" samples\n";
    }
}

void GameMode::registerWaveFile(const Core::String & waveFile)
{
    _registeredWaves.clear();
    Core::XMLDocument xmlDocument;
    xmlDocument.loadDocument(*(_appContext.pWorld->getRessourcesPool()->getVFS()->readFile(waveFile)));

    Ptr<Core::XMLNode> pNodeWaves = xmlDocument.firstChildNode(L"waves");

    Core::List<Ptr<Core::XMLNode> > pXMLWaveList;
    pNodeWaves->getNodes(L"wave", pXMLWaveList);

    for(int32 ii=0; ii<pXMLWaveList.size(); ++ii)
    {
        Ptr<Wave> pWave(new Wave(_appContext.pWorld, _pPlayer, *this, _pLevel));
        pWave->importXML(pXMLWaveList[ii]);
        
        LM_ASSERT(_registeredWaves.find(pWave->getWaveId()) == _registeredWaves.end());
        _registeredWaves[pWave->getWaveId()] = pWave;
    }

    _waveFile = waveFile;
}

void GameMode::restart()
{
    _gameCurrentState = GAME_NOT_STARTED;
    registerWaveFile(_waveFile);

    _foes.clear();
    _foeShots.clear();
    _powerUps.clear();
    _bonusDirections.clear();
    _warningDirections.clear();

    _displayAchievement = 0.0f;
    _achievementFoeKilled = 0;
    _achievementBossKilled = 0;
    _achievementFoeKilledDuringTimeWarp = 0;
    _achievementCreditsHunter = false;
}

#if LM_DISPLAY_FPS
void GameMode::clearWaves()
{
    _registeredWaves.clear();
}
#endif

void GameMode::initPlayer(bool lookTarget)
{
    _appContext.pGameSettings->cleanupSettings(*_appContext.pInputManager);

    switch(_appContext.pGameSettings->getSelectedControls())
    {
    case SC_MOUSE_KEYBOARD:
        _pControl = Ptr<IPlayerCtrl>(new PlayerCtrlMouse(_appContext.pInputManager, _appContext.pCameraNode, _appContext.pGameSettings));
        break;
    case SC_KEYBOARD_ONLY:
        _pControl = Ptr<IPlayerCtrl>(new PlayerCtrlKeyboard(_appContext.pInputManager, _appContext.pGameSettings));
        break;
    case SC_JOYSTICK:
        {
            int32 iJoy = _appContext.pGameSettings->getSelectedJoystick();
            Ptr<Window::IJoystick> pJoy = _appContext.pInputManager->getJoysticks()[iJoy];
            _pControl = Ptr<IPlayerCtrl>(new PlayerCtrlJoystick(pJoy, _appContext.pGameSettings->getJoySettings(pJoy->getName())));
            break;
        }
    }

    if(_pPlayer != null)
        _pPlayer->kill();

    _pPlayer = Ptr<Player>(new Player(_pControl, _appContext.pWorld, _appContext.pSoundLibrary, _pLevel, lookTarget, false));
    _pPlayer->initialize();

    setReticleCursor();
}

void GameMode::initialize()
{   
    _appContext.pGUI->reset();

    _pScreenDialog = Ptr<GUI::Widget>(new GUI::Widget());
    _appContext.pGUI->setDialog(_pScreenDialog);

    _displayAchievement = 0.0f;

    _cameraDeltaX = 0.0f;
    _cameraDeltaY = 0.0f;
    _computeCameraDeltaDone = false;
    _playerDeltaX = 0.0f;
    _playerDeltaY = 0.0f;
    _computePlayerDeltaDone = false;
    _pTimeWarp = null;

    _gamePaused = false;
    _achievementFoeKilled = 0;
    _achievementBossKilled = 0;
    _achievementFoeKilledDuringTimeWarp = 0;
    _achievementCreditsHunter = false;
    
    _pGamePausedDialog = _appContext.pWidgetFactory->createDialogBase();
    _pGamePausedPanel = _appContext.pWidgetFactory->createThinPanel(0, 0);
    
    Ptr<GUI::Label> pLabelPause = _appContext.pWidgetFactory->createMediumLabel(L"game-paused", 0, 20);
    pLabelPause->setHLayout(GUI::HL_CENTER);
    Core::Vector2f panelSize = _pGamePausedPanel->getSize();
    panelSize.y = 110.0f;
    pLabelPause->setSize(panelSize);
    
    _pQuitButtonResume  = _appContext.pWidgetFactory->createSmallButton(L"resume", 14, 110);
    _pQuitButtonRetry   = _appContext.pWidgetFactory->createSmallButton(L"retry", 225, 110);
    _pQuitButtonQuit    = _appContext.pWidgetFactory->createSmallButton(L"quit", 436, 110);
    
    _pQuitButtonResume->setId(BUTTON_RESUME);
    _pQuitButtonRetry->setId(BUTTON_RETRY);
    _pQuitButtonQuit->setId(BUTTON_QUIT);

    // key nav
    _pQuitButtonResume->setKeyWidgetR(_pQuitButtonRetry.get());

    _pQuitButtonRetry->setKeyWidgetL(_pQuitButtonResume.get());
    _pQuitButtonRetry->setKeyWidgetR(_pQuitButtonQuit.get());

    _pQuitButtonQuit->setKeyWidgetL(_pQuitButtonRetry.get());
    // /key nav

    _pGamePausedPanel->addChild(_pQuitButtonResume);
    _pGamePausedPanel->addChild(_pQuitButtonRetry);
    _pGamePausedPanel->addChild(_pQuitButtonQuit);
    _pGamePausedPanel->addChild(pLabelPause);
    _pGamePausedPanel->pack();

    _appContext.pWidgetFactory->centerScreen(_pGamePausedPanel);

    _pGamePausedDialog->addChild(_pGamePausedPanel);
    _pGamePausedDialog->pack();

    _pQuitButtonResume->addEventHandler(this);
    _pQuitButtonRetry->addEventHandler(this);
    _pQuitButtonQuit->addEventHandler(this);
    
    _gameOverVisible = false;
    _pGameOverDialog = _appContext.pWidgetFactory->createDialogBase();
    _pGameOverPanel = _appContext.pWidgetFactory->createThinPanel(0, 0);

    Ptr<GUI::Label> pLabelGameOver = _appContext.pWidgetFactory->createMediumLabel(L"game-over", 0, 20);
    pLabelGameOver->setHLayout(GUI::HL_CENTER);
    panelSize = _pGameOverPanel->getSize();
    panelSize.y = 110.0f;
    pLabelGameOver->setSize(panelSize);
    
    _pGameOverButtonRetry   = _appContext.pWidgetFactory->createSmallButton(L"retry", 100, 110);
    _pGameOverButtonQuit    = _appContext.pWidgetFactory->createSmallButton(L"quit", 353, 110);
    
    // key nav
    _pGameOverButtonRetry->setKeyWidgetR(_pGameOverButtonQuit.get());
    _pGameOverButtonQuit->setKeyWidgetL(_pGameOverButtonRetry.get());
    // /key nav

    _pGameOverButtonRetry->setId(BUTTON_RETRY);
    _pGameOverButtonQuit->setId(BUTTON_QUIT);
    _pGameOverButtonRetry->addEventHandler(this);
    _pGameOverButtonQuit->addEventHandler(this);

    _pGameOverPanel->addChild(_pGameOverButtonRetry);
    _pGameOverPanel->addChild(_pGameOverButtonQuit);
    _pGameOverPanel->addChild(pLabelGameOver);
    _pGameOverPanel->pack();

    _pGameOverDialog->addChild(_pGameOverPanel);
    _pGameOverDialog->pack();

    _appContext.pWidgetFactory->centerScreen(_pGameOverPanel);

    _enterNameVisible = false;
    _hasPlayerEnteredHisName = false;
    _pEnterNameDialog = _appContext.pWidgetFactory->createDialogBase();
    _pEnterNamePanel  = _appContext.pWidgetFactory->createThinPanel(0, 0);
    _pLabelEnterName  = _appContext.pWidgetFactory->createMediumLabel(L"enter-name", 100, 50);
    _pTextEditEnterName = _appContext.pWidgetFactory->createMediumTextEdit( 400, 50);
    _pTextEditEnterName->getEdit()->setMaxLength(MAX_PLAYER_NAME_LENGTH);
    _pTextEditEnterName->getEdit()->setText(_appContext.pGameSettings->getPlayerName());
    _pButtonEnterNameOK = _appContext.pWidgetFactory->createSmallButton(L"ok", 226, 110);
    _pButtonEnterNameOK->setId(BUTTON_ENTER_NAME_OK);
    _pButtonEnterNameOK->setHotKey(VK_RETURN);
    _pButtonEnterNameOK->addEventHandler(this);

    _pEnterNamePanel->addChild(_pLabelEnterName);
    _pEnterNamePanel->addChild(_pTextEditEnterName);
    _pEnterNamePanel->addChild(_pButtonEnterNameOK);
    _pEnterNamePanel->pack();

    _pEnterNameDialog->addChild(_pEnterNamePanel);
    _pEnterNameDialog->pack();

    _appContext.pWidgetFactory->centerScreen(_pEnterNamePanel);

    _pAchievementDialog = Ptr<GUI::Widget>(new GUI::Widget());
    _pAchievementText = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pAchievementText->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pAchievementText->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 1.0f));
    _pAchievementText->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pAchievementText->setPosition(Core::Vector2f(0.0f, 200.0f));
    _pAchievementText->setHLayout(GUI::HL_CENTER);

    _pAchievementUnlocked = Ptr<GUI::BitmapLabel>(new GUI::BitmapLabel(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));
    _pAchievementUnlocked->setFont(L"/ChronoRageData/Font/in-game-font.font.xml");
    _pAchievementUnlocked->setOwnColor(Core::Vector4f(0.25f, 1.0f, 0.25f, 1.0f));
    _pAchievementUnlocked->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
    _pAchievementUnlocked->setPosition(Core::Vector2f(0.0f, 280.0f));
    _pAchievementUnlocked->setText((*_appContext.pDico)[L"achievement-unlocked"]);
    _pAchievementUnlocked->setHLayout(GUI::HL_CENTER);

    _pAchievementPicture = Ptr<GUI::Picture>(new GUI::Picture(_appContext.pGUI->getResPool(), _appContext.pGUI->getHUD()));

    _pAchievementDialog->addChild(_pAchievementText);
    _pAchievementDialog->addChild(_pAchievementUnlocked);
    _pAchievementDialog->addChild(_pAchievementPicture);
    _pAchievementDialog->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pScreenDialog->addChild(_pAchievementDialog);
    
    if(_appContext.pMenuMusic != null && _appContext.pMenuMusic->isAlive())
    {
        _appContext.pMenuMusic->kill();
        _appContext.pMenuMusic = null;
    }

    if(_appContext.pGameMusic != null && _appContext.pGameMusic->isAlive())
    {
        _appContext.pGameMusic->kill();
        _appContext.pGameMusic = null;
    }

    _powerLevel = 0;
}


void GameMode::update(double elapsed)
{
    _fps.nextFrame();

    float duration = _fps.getFrameDuration();
    
    if(duration > 0.0f)
    {
        if(_frameDurationHistory.empty())
            _frameDurationHistory.push_back(duration);
        else
        {
            float lastDuration = _frameDurationHistory.back();
            if(lastDuration != duration)
            {
                _frameDurationHistory.push_back(duration);

                if(_frameDurationHistory.size() > FRAME_DURATION_HISTORY_SIZE)
                    _frameDurationHistory.pop_front();
            }
        }
    }

    manageEscapeKey();

    if(_gameCurrentState == GAME_ENTER_NAME && !_enterNameVisible)
    {
        _appContext.pGUI->setCursor(_appContext.pCursorArrow, 0, 0);
        _appContext.pGUI->pushDialog(_pEnterNameDialog);
        _enterNameVisible = true;
    }
    
    if(_gameCurrentState == GAME_LOST && !_gameOverVisible)
    {
        _appContext.pGUI->setCursor(_appContext.pCursorArrow, 0, 0);
        _appContext.pGUI->pushDialog(_pGameOverDialog);
        _appContext.pGUI->getKeyNavigation()->pKeyFocus = _pGameOverButtonQuit.get();
        _gameOverVisible = true;
        playerDie();
    }

    if(_unlockAchievements.size() > 0)
    {
        if(_displayAchievement == 0.0f)
        {
            _displayAchievement = DISPLAY_ACHIEVEMENT_TIMER;
            Core::String achievementDictKey = GameTools::getAchievementDictKey(_unlockAchievements[0]);
            Core::String achievementText = (*_appContext.pDico)[achievementDictKey];

            Core::String achievementPicture = GameTools::getAchievementPicture(_unlockAchievements[0]);
            _pAchievementText->setText(achievementText);

            _pAchievementPicture->setPicture(achievementPicture);

            createSparks();
            
        }
        
        if(!_gamePaused)
        {
            _displayAchievement = std::max(0.0f, _displayAchievement - float(elapsed));
            
            if(_displayAchievement > 0)
            {
                float alpha = 1.0f;
                if(_displayAchievement < 0.5f)
                    alpha = (float)_displayAchievement*2;
                else
                    alpha = Core::clamp(0.0f, 1.0f, (DISPLAY_ACHIEVEMENT_TIMER - _displayAchievement) *2);

                _pAchievementText->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
                _pAchievementText->setPosition(Core::Vector2f(0.0f, 200.0f));
                _pAchievementUnlocked->setSize(Core::Vector2f((float)_appContext.pRenderer->getDefaultView()->getWidth(), (float)_appContext.pRenderer->getDefaultView()->getHeight()));
                _pAchievementUnlocked->setPosition(Core::Vector2f(0.0f, 240.0f));
                _pAchievementPicture->setPosition(Core::Vector2f(((float)_appContext.pRenderer->getDefaultView()->getWidth()/2.0f) - 32.0f , 130.0f));
                _pAchievementPicture->setSize(Core::Vector2f(64.0f, 64.0f));
                _pAchievementDialog->setOwnColor(Core::Vector4f(alpha, alpha, alpha, alpha));

                moveSparks();
            }
            else
            {
                _pAchievementDialog->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
                _unlockAchievements.erase(_unlockAchievements.begin());
            }
        }
    }

    _appContext.pGUI->update(elapsed);

    float timeMultiplier = (_gamePaused) ? 0.0f : 1.0f;
    _appContext.pWorld->setTimeMultiplier(timeMultiplier);
    _appContext.pWorld->update(elapsed);
    _appContext.pSoundLibrary->update(elapsed);

    if(!_gamePaused)
    {
        _pControl->setViewDimensions(   _appContext.pRenderer->getDefaultView()->getWidth(), 
                                        _appContext.pRenderer->getDefaultView()->getHeight());
        _pControl->setPlayerPos(_pPlayer->getPosition());

        if(_pPlayer != null && _pPlayer->isInitialized() && _pLevel != null)
        {
            computeCameraDelta();
            computePlayerDelta();

            ///////////////////////////////// UPDATES /////////////////////////////////

            // Player

            bool playerKilled = false;
                        
            _pPlayer->update(elapsed, _playerDeltaX, _playerDeltaY, _pTimeWarp->getTimeWarpFactor(), _gameCurrentState == GAME_LOST || _gameCurrentState == GAME_ENTER_NAME);

            //powerDown
            if(_powerLevel > (int32)_pPlayer->getPowerLevel())
            {
                _appContext.pSoundLibrary->playSound(SOUND_PLAYER_POWER_DOWN, false);
                displayPowerDown();
            }

            _powerLevel = (int32)_pPlayer->getPowerLevel();

            cameraTracking();

            // Foes
            if(!_delayedFoes.empty())
            {
                _foes.insert(_foes.end(), _delayedFoes.begin(), _delayedFoes.end());
                _delayedFoes.clear();
            }

            Core::List<Ptr<Foe> >::iterator iFoe = _foes.begin();
            
            int32 activeFoeCount = 0;
            int32 activeBonusCount = 0;
            int32 warningFoeCount = 0;

            while(iFoe !=  _foes.end())
            {
                EFoeState state = (*iFoe)->getState();

                if(state == FOE_DELETE_PENDING)
                    iFoe = _foes.erase(iFoe);
                else
                {
                    (*iFoe)->update(elapsed);
                    if(state == FOE_CREATION_IN_PROGRESS || (*iFoe)->isAlive())
                    {
                        if((*iFoe)->getType() == BONUS_CARRIER || (*iFoe)->getType() == BONUS_CARRIER_W_SHIELD)
                        {
                            ++activeBonusCount;
                        }
                        else
                        {
                            if((*iFoe)->displayWarning())
                                warningFoeCount++;
                            else
                                activeFoeCount += (*iFoe)->foeCount();
                        }
                    }

                    ++iFoe;
                }
            }

            // Bonus directions
            if(activeBonusCount > 0 || _powerUps.size() > 0)
            {
                showBonusDirections();
            }

            for(int32 ii=0; ii<_bonusDirections.size(); ++ii)
            {
                _bonusDirections[ii]->update(elapsed);
            }

            // Warning directions
            if(warningFoeCount > 0)
            {
                showWarningDirections();
            }
            else
            {
                for(int32 ii=0; ii<_warningDirections.size(); ++ii)
                {
                    _warningDirections[ii]->setVisible(false);
                }
            }

            for(int32 ii=0; ii<_warningDirections.size(); ++ii)
            {
                _warningDirections[ii]->update(elapsed);
            }

            // Foe directions
            if(activeFoeCount <= HELP_FIND_REMAINING_FOES)
            {
                showFoeDirections();
            }
            else
            {
                for(int32 ii=0; ii<HELP_FIND_REMAINING_FOES; ++ii)
                {
                    _foeDirections[ii].setVisible(false);
                }
            }

            for(int32 ii=0; ii<HELP_FIND_REMAINING_FOES; ++ii)
            {
                _foeDirections[ii].update(elapsed);
            }

            // Foe shoots

            Core::List<Ptr<IShot> >::iterator iFoeShot = _foeShots.begin();
            while(iFoeShot !=  _foeShots.end())
            {
                Ptr<FoeShot> pFoeShot = LM_DEBUG_PTR_CAST<FoeShot>(*iFoeShot);
                if(pFoeShot->isDead())
                {
                    iFoeShot = _foeShots.erase(iFoeShot);                
                }
                else if(!pFoeShot->isDying() && (*iFoeShot)->hasHit())
                {
                    (*iFoeShot)->kill(_pLevel->isOutOfBoundaries((*iFoeShot)->getPosition(), 20.0f));
                }
                else
                {
                    (*iFoeShot)->update(elapsed);

                    if(_pLevel->isOutOfBoundaries((*iFoeShot)->getPosition(), 20.0f))
                        (*iFoeShot)->madeHit();

                    ++iFoeShot;
                }
            }

            // Power ups

            Core::List<Ptr<PowerUp> >::iterator iPowerUp = _powerUps.begin();
            while(iPowerUp != _powerUps.end())
            {
                (*iPowerUp)->update(elapsed);

                if((*iPowerUp)->isKilled())
                    iPowerUp = _powerUps.erase(iPowerUp);
                else
                    ++iPowerUp;
            }

            for(int32 ii=0; ii < _foes.size(); ii++)
                _foes[ii]->startParallelUpdate();

            for(int32 ii=0; ii < _foes.size(); ii++)
                _foes[ii]->endParallelUpdate();

            /////////////////////////// COLLISIONS SHAPES /////////////////////////////

            _pPlayer->updateCollisionShape();

            for(int32 ii=0; ii < _foes.size(); ii++)
                _foes[ii]->updateCollisionShape();

            for(int32 ii=0; ii < _foeShots.size(); ii++)
                _foeShots[ii]->updateCollisionShape();

            for(int32 ii=0; ii < _powerUps.size(); ii++)
                _powerUps[ii]->updateCollisionShape();


            /////////////////////////// COLLISIONS TESTS //////////////////////////////

            for(int32 ii=0; ii < _foes.size(); ii++)
                _foes[ii]->testForCollisions();

            if(!_pPlayer->isDead() && !_pPlayer->isInvincible())
            {
                for(int32 ii=0; ii < _foeShots.size(); ii++)
                {
                    Ptr<FoeShot> pFoeShot = LM_DEBUG_PTR_CAST<FoeShot>(_foeShots[ii]);
                    if(pFoeShot->collidesPlayer())
                    {
                        playerDie();
                        deathPenalty();
                        break;
                    }
                }
            }

            if(_foeCollisionsEnabled && !_pPlayer->isDead() && !_pPlayer->isInvincible())
            {
                for(int32 ii=0; ii < _foes.size(); ii++)
                {
                    if(_foes[ii]->collidesPlayer())
                    {
                        playerDie();
                        deathPenalty();
                        break;
                    }
                }
            }

            if(!_pPlayer->isDead())
            {
                for(int32 ii=0; ii < _powerUps.size(); ii++)
                {
                    if(_powerUps[ii]->collidesPlayer())
                    {
                        _pPlayer->powerUp();
                        _appContext.pSoundLibrary->playSound(SOUND_PLAYER_POWER_UP, false);
                        displayPowerUp();
                    }
                }
            }
        }
    }
}

void GameMode::playerDie()
{
    if(_pPlayer->isDead())
        return;

    _achievementFoeKilled = 0;
    _achievementBossKilled = 0;

    _pPlayer->kill();
    Ptr<Universe::ITask> pTaskFlash(new Universe::TaskFXFade(   _appContext.pCameraNode,
                                                                Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                                                                Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f),
                                                                0.3,
                                                                Core::INTER_LINEAR));
    Ptr<Universe::ITask> pTaskShake(new Universe::TaskShake(   _appContext.pCameraNode,
                                                                5.0f, 0.1f,
                                                                5.0f, 0.25f,
                                                                1.5f,
                                                                Core::INTER_FADE_OUT));
    Ptr<Universe::ITask> pTaskBlur(new Universe::TaskFXRadialBlur(_appContext.pCameraNode, _pPlayer->getNode(), 1.0f, 0.0f, 2.0f, Core::INTER_LINEAR));

    _appContext.pWorld->registerTask(pTaskFlash);
    _appContext.pWorld->registerTask(pTaskShake);
    _appContext.pWorld->registerTask(pTaskBlur);

    _appContext.pSoundLibrary->playSound(SOUND_PLAYER_DESTRUCTION, false);

    if(_pTimeWarp->isStarted())
        _pTimeWarp->end();

}

void GameMode::showBonusDirections()
{
    Core::Vector3f cameraPosition = _appContext.pCameraPivot->getWorldPosition();
    cameraPosition.z = _pLevel->playgroundDepth();

    int32 iBonusDirection = 0;
    int32 bonusDirectionsSize = _bonusDirections.size();

    for(int32 ii = 0; ii<bonusDirectionsSize; ++ii)
    {
        _bonusDirections[ii]->setVisible(false);
    }

    Core::List<Ptr<PowerUp> >::const_iterator iBonus = _powerUps.begin();
    while(iBonus != _powerUps.end())
    {
        if( (*iBonus)->isCollisionActive() && !isVisible((*iBonus)->getPosition()))
        {
            Core::Vector3f bonusDirection = (*iBonus)->getPosition() - cameraPosition;
            if(bonusDirection.isZero())
                bonusDirection = Core::Vector3f(0.0f, 1.0f, 0.0f);
            else
                bonusDirection.normalizeFast();

            if(iBonusDirection > bonusDirectionsSize-1)
            {
                Ptr<BonusDirection> pBonusDirection(new BonusDirection());
                pBonusDirection->initialize(_appContext.pWorld);
                _bonusDirections.push_back(pBonusDirection);
            }
        
            _bonusDirections[iBonusDirection]->setDirection(bonusDirection);
            _bonusDirections[iBonusDirection]->setPosition(cameraPosition + (bonusDirection*20));
            _bonusDirections[iBonusDirection]->setBonusCarrierColor(false);
            _bonusDirections[iBonusDirection]->setVisible(true);
            
            ++iBonusDirection;

        }
        ++iBonus;
    }

    Core::List<Ptr<Foe> >::iterator iFoe = _foes.begin();
    while(iFoe !=  _foes.end())
    {
        if((*iFoe)->getType() == BONUS_CARRIER || (*iFoe)->getType() == BONUS_CARRIER_W_SHIELD)
        {
            if((*iFoe)->isAlive() && !isVisible((*iFoe)->getPosition()))
            {
                Core::Vector3f bonusDirection = (*iFoe)->getPosition() - cameraPosition;
                if(bonusDirection.isZero())
                    bonusDirection = Core::Vector3f(0.0f, 1.0f, 0.0f);
                else
                    bonusDirection.normalizeFast();

                if(iBonusDirection < HELP_FIND_REMAINING_FOES)
                {
                    if(iBonusDirection > bonusDirectionsSize-1)
                    {
                        Ptr<BonusDirection> pBonusDirection(new BonusDirection());
                        pBonusDirection->initialize(_appContext.pWorld);
                        _bonusDirections.push_back(pBonusDirection);
                    }
                
                    _bonusDirections[iBonusDirection]->setDirection(bonusDirection);
                    _bonusDirections[iBonusDirection]->setPosition(cameraPosition + (bonusDirection*20));
                    _bonusDirections[iBonusDirection]->setBonusCarrierColor(true);
                    _bonusDirections[iBonusDirection]->setVisible(true);
                    ++iBonusDirection;
                }
            }
        }

        ++iFoe;
    }

    while(iBonusDirection > _bonusDirections.size())
    {
        _bonusDirections.erase(_bonusDirections.begin() + iBonusDirection);
    }
}

void GameMode::showWarningDirections()
{
    Core::Vector3f cameraPosition = _appContext.pCameraPivot->getWorldPosition();
    cameraPosition.z = _pLevel->playgroundDepth();

    int32 iWarningDirection = 0;
    int32 warningDirectionsSize = _warningDirections.size();

    for(int32 ii = 0; ii<warningDirectionsSize; ++ii)
    {
        _warningDirections[ii]->setVisible(false);
    }

    Core::List<Ptr<Foe> >::iterator iFoe = _foes.begin();
    while(iFoe !=  _foes.end())
    {
        if((*iFoe)->displayWarning())
        {
            if((*iFoe)->isAlive() && !isVisible((*iFoe)->getPosition()))
            {
                Core::Vector3f warningDirection = (*iFoe)->getPosition() - cameraPosition;
                if(warningDirection.isZero())
                    warningDirection = Core::Vector3f(0.0f, 1.0f, 0.0f);
                else
                    warningDirection.normalizeFast();

                
                if(iWarningDirection > warningDirectionsSize-1)
                {
                    Ptr<WarningDirection> pWarningDirection(new WarningDirection());
                    pWarningDirection->initialize(_appContext.pWorld);
                    _warningDirections.push_back(pWarningDirection);
                }
            
                _warningDirections[iWarningDirection]->setDirection(warningDirection);
                _warningDirections[iWarningDirection]->setPosition(cameraPosition + (warningDirection*20));
                _warningDirections[iWarningDirection]->setVisible(true);
                ++iWarningDirection;
                
            }
        }

        ++iFoe;
    }

    while(iWarningDirection > _warningDirections.size())
    {
        _warningDirections.erase(_warningDirections.begin() + iWarningDirection);
    }
}

void GameMode::showFoeDirections()
{
    Core::Vector3f cameraPosition = _appContext.pCameraPivot->getWorldPosition();
    cameraPosition.z = _pLevel->playgroundDepth();

    int32 iFoeDirection = 0;
    Core::List<Ptr<Foe> >::iterator iFoe = _foes.begin();
    while(iFoe !=  _foes.end())
    {
        if((*iFoe)->getType() != BONUS_CARRIER && (*iFoe)->getType() != BONUS_CARRIER_W_SHIELD)
        {
            if((*iFoe)->isAlive() && !(*iFoe)->displayWarning() && !isVisible((*iFoe)->getPosition()) && (*iFoe)->foeCount() > 0)
            {
                Core::Vector3f foeDirection = (*iFoe)->getPosition() - cameraPosition;
                if(foeDirection.isZero())
                    foeDirection = Core::Vector3f(0.0f, 1.0f, 0.0f);
                else
                    foeDirection.normalizeFast();

                if(iFoeDirection < HELP_FIND_REMAINING_FOES)
                {
                    _foeDirections[iFoeDirection].setDirection(foeDirection);
                    _foeDirections[iFoeDirection].setPosition(cameraPosition + (foeDirection*25));
                    _foeDirections[iFoeDirection].setVisible(true);
                    ++iFoeDirection;
                }
            }
        }

        ++iFoe;
    }

    while(iFoeDirection < HELP_FIND_REMAINING_FOES)
    {
        _foeDirections[iFoeDirection].setVisible(false);
        ++iFoeDirection;
    }
}

void GameMode::manageTimeWarp(double elapsed)
{
    if(_pTimeWarp != null && !_pPlayer->isDead())
    {
        if(_gameCurrentState == GAME_STARTED && _pPlayer->toggleTimeWarp() && !_pTimeWarp->isStarted() && _pPlayer->getTimeWarpNumber() >= 1.0f)
        {
            _pPlayer->launchTimeWarp();
            _pPlayer->setInvincible(true);
            _pTimeWarp->start(_appContext.pSoundLibrary->playSound(SOUND_PLAYER_TIME_WARP, true));

            for(int32 ii=0; ii < _foeShots.size(); ii++)
            {
                if(_foeShots[ii]->getPosition().getDistance(_pPlayer->getPosition()) < BULLET_TIME_DIST)
                    displayAchievement(BULLET_TIME);
            }

            _achievementFoeKilledDuringTimeWarp = 0;
        }

        if(_pTimeWarp->isStarted())
            _pTimeWarp->update((float)elapsed);
    }
}

int32 GameMode::getFoesNumber()
{
    return _foes.size();
}

void GameMode::foeKilled(EFoeType type)
{
    _achievementFoeKilled++;
    
    if(type == BOSS)
        _achievementBossKilled++;

    if(_pTimeWarp->isStarted())
        _achievementFoeKilledDuringTimeWarp++;

    if(_achievementFoeKilled > KILLING_SPREE_NUMBER)
    {
        displayAchievement(KILLING_SPREE);
    }

    if(_achievementBossKilled > MOTHERSHIP_SLAYER_NUMBER)
    {
        displayAchievement(MOTHERSHIP_SLAYER);
    }

    if(_achievementFoeKilledDuringTimeWarp > TIME_MACHINE_NUMBER)
    {
        displayAchievement(TIME_MACHINE);
    }

    foeKilledTimeWarpBonus();
}

void GameMode::foeKilledTimeWarpBonus()
{
    int32 oldTimeWarpNumber = (int32)_pPlayer->getTimeWarpNumber();
    _pPlayer->foeKilled();

     //Time Warp increase
    if(oldTimeWarpNumber < (int32)_pPlayer->getTimeWarpNumber())
    {
        playSound(SOUND_PLAYER_TIME_WARP_INCREASE, false);
        displayTimeWarpBonus();
    }
}

float GameMode::getTimeWarpNumber()
{
    return _pPlayer->getTimeWarpNumber();
}

void GameMode::powerUp()
{
    _pPlayer->powerUp();
}

void GameMode::powerDown()
{
    _pPlayer->powerDown();
}

float GameMode::getPowerLevel()
{
    return _pPlayer->getPowerLevel();
}

const Ptr<TimeWarp> & GameMode::getTimeWarp() const
{
    return _pTimeWarp;
}

void GameMode::addFoe(const Ptr<Foe> & pFoe)
{
    _foes.push_back(pFoe);
}

void GameMode::addDelayedFoe(const Ptr<Foe> & pFoe)
{
    _delayedFoes.push_back(pFoe);
}

void GameMode::addFoeShot(const Ptr<FoeShot> & pFoeShot)
{
    _foeShots.push_back(pFoeShot);
}

void GameMode::createPowerUp(const Core::Vector3f & position)
{
    Ptr<PowerUp> pPowerUp(new PowerUp(_appContext.pWorld, _pPlayer, *this));
    pPowerUp->initialize(position);

    _powerUps.push_back(pPowerUp);
}

const Core::List<Ptr<IShot> > & GameMode::getPlayerShots() const
{
    return _pPlayer->getShots();
}

const Core::List<Ptr<Foe> > &   GameMode::getFoes() const
{
    return _foes;
}

float GameMode::getLevelXMin() const
{
    return _pLevel->xMin() + _playerDeltaX;
}

float GameMode::getLevelXMax() const
{
    return _pLevel->xMax() - _playerDeltaX;
}

float GameMode::getLevelYMin() const
{
    return _pLevel->yMin() + _playerDeltaY;
}

float GameMode::getLevelYMax() const
{
    return _pLevel->yMax() - _playerDeltaY;
}

void GameMode::movePointInPlayground(Core::Vector3f & point)
{
    if(point.x > (_pLevel->xMax() - _playerDeltaX))
        point.x = (_pLevel->xMax() - _playerDeltaX);
    if(point.x < (_pLevel->xMin() + _playerDeltaX))
        point.x = (_pLevel->xMin() + _playerDeltaX);
    if(point.y > (_pLevel->yMax() - _playerDeltaY))
        point.y = (_pLevel->yMax() - _playerDeltaY);
    if(point.y < (_pLevel->yMin() + _playerDeltaY))
        point.y = (_pLevel->yMin() + _playerDeltaY);
}

bool GameMode::isOutOfBoundaries(Core::Vector3f & point, float tolerance)
{
    return _pLevel->isOutOfBoundaries(point, tolerance);
}

bool GameMode::isVisible(const Core::Vector3f & position)
{
    Core::Vector3f cameraPosition = _appContext.pCameraNode->getWorldPosition();
    float distance = _pLevel->playgroundDepth() - cameraPosition.z;

    Core::Matrix4f proj;
    _appContext.pCameraNode->getBaseCamera()->getProjectionMatrixSplit(proj, 0.5f * distance, distance);

    Core::Matrix4f invProj(proj.getInverted());

    /*Core::Vector3f p00 = invProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p10 = invProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p01 = invProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();*/
    Core::Vector3f p11 = invProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();

    float deltaX = p11.x + 1;
    float deltaY = p11.y + 1;

    return((position.x >= (cameraPosition.x - deltaX)) 
        && (position.x <= (cameraPosition.x + deltaX)) 
        && (position.y >= (cameraPosition.y - deltaY)) 
        && (position.y <= (cameraPosition.y + deltaY)));
}

void GameMode::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        EGameMessageBoxWidgetId widgetId = (EGameMessageBoxWidgetId)pWidget->getId();

        switch(widgetId)
        {
        case BUTTON_RESUME:
            setReticleCursor();
            _appContext.pGUI->popDialog();
            _gamePaused = false;
            break;
        case BUTTON_RETRY:
            _appContext.pGUI->popDialog();
            _gamePaused = false;
            restart();
            break;
        case BUTTON_QUIT:
            _appContext.wantedMode = MENU_MODE;
            break;
        case BUTTON_ENTER_NAME_OK:
            int32 length = _pTextEditEnterName->getEdit()->getText().length();
            if( length > 0 && length <= MAX_PLAYER_NAME_LENGTH)
            {
                setReticleCursor();
                _gamePaused = false;
                _hasPlayerEnteredHisName = true;
                _appContext.pGUI->popDialog();
                _appContext.pGameSettings->setPlayerName(_pTextEditEnterName->getEdit()->getText());
            }
            break;
        }
    }
}

void GameMode::manageEscapeKey()
{
    if(_appContext.pInputManager->isKeyTyped(VK_ESCAPE) && _gameCurrentState != GAME_LOST && _gameCurrentState  != GAME_ENTER_NAME)
    {
        if(!_gamePaused)
        {
            _appContext.pGUI->setCursor(_appContext.pCursorArrow, 0, 0);
            _appContext.pGUI->pushDialog(_pGamePausedDialog);
            _appContext.pGUI->getKeyNavigation()->pKeyFocus = _pQuitButtonResume.get();
        }
        else
        {
            setReticleCursor();
            _appContext.pGUI->popDialog();
        }

        _gamePaused = !_gamePaused;
    }
}

void GameMode::cameraTracking()
{
    Core::Vector3f cameraPosition = _appContext.pCameraPivot->getWorldPosition();
    Core::Vector3f playerPosition = _pPlayer->getPosition();

    float ratioX = 1.0f;
    ratioX = (playerPosition.x - ((_pLevel->xMin() + _playerDeltaX)/2) ) / (((_pLevel->xMax() - _playerDeltaX) - (_pLevel->xMin() + _playerDeltaX))/2);

    float ratioY = 1.0f;
    ratioY = (playerPosition.y - ((_pLevel->yMin() + _playerDeltaY)/2) ) / (((_pLevel->yMax() - _playerDeltaY) - (_pLevel->yMin() + _playerDeltaY))/2);

    ratioX = Core::clamp(0.0f, 1.0f, ratioX);

    ratioY = Core::clamp(0.0f, 1.0f, ratioY);
    
    cameraPosition.x = (ratioX * ((_pLevel->xMax() - _cameraDeltaX) - (_pLevel->xMin() + _cameraDeltaX))) + (_pLevel->xMin() + _cameraDeltaX);
    cameraPosition.y = (ratioY * ((_pLevel->yMax() - _cameraDeltaY) - (_pLevel->yMin() + _cameraDeltaY))) + (_pLevel->yMin() + _cameraDeltaY);

    _appContext.pCameraPivot->setWorldPosition(cameraPosition);
    
}

void GameMode::computeCameraDelta()
{
    Core::Vector3f cameraPosition = _appContext.pCameraNode->getWorldPosition();
    float distance = _pLevel->backgroundDepth() - cameraPosition.z;

    Core::Matrix4f proj;
    _appContext.pCameraNode->getBaseCamera()->getProjectionMatrixSplit(proj, 0.5f * distance, distance);

    Core::Matrix4f invProj(proj.getInverted());

    Core::Vector3f p00 = invProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p10 = invProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p01 = invProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p11 = invProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();

    float deltaX = p11.x + 1;
    float deltaY = p11.y + 1;

    if(deltaX == _cameraDeltaX && deltaY == _cameraDeltaY)
        _computeCameraDeltaDone = true;
    else
    {
        _cameraDeltaX = deltaX;
        _cameraDeltaY = deltaY;
    }
}

void GameMode::computePlayerDelta()
{
    Core::Vector3f playerPosition = _pPlayer->getPosition();
    float distance = _pLevel->backgroundDepth() - playerPosition.z;

    Core::Matrix4f proj;
    _appContext.pCameraNode->getBaseCamera()->getProjectionMatrixSplit(proj, 0.5f * distance, distance);

    Core::Matrix4f invProj(proj.getInverted());

    Core::Vector3f p00 = invProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p10 = invProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p01 = invProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p11 = invProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();

    float deltaX = p11.x + _pPlayer->getWidth() + 1;
    float deltaY = p11.y + _pPlayer->getHeight() + 1;

    if(deltaX == _playerDeltaX && deltaY == _playerDeltaY)
        _computePlayerDeltaDone = true;
    else
    {
        _playerDeltaX = deltaX;
        _playerDeltaY = deltaY;
    }

}

void GameMode::killAllFoes()
{
    Core::List<Ptr<Foe> >::iterator iFoe = _foes.begin();
    while(iFoe !=  _foes.end())
    {
        if((*iFoe)->isAlive())
        {
            _achievementCreditsHunter = false;
            (*iFoe)->selfDestroy();
        }
        ++iFoe;
    }
}

void GameMode::setReticleCursor()
{
    if(_pPlayer->showCursor())
        _appContext.pGUI->setCursor(_appContext.pCursorReticle, 16, 16);
    else
        _appContext.pGUI->setCursor(_appContext.pCursorBlank, 16, 16);
}

void GameMode::displayAchievement(EAchievementType achievementType)
{
#ifndef CHRONORAGE_DEMO
    if(!_appContext.pHighScores->isAchievementUnlocked(achievementType))
    {
        _appContext.pHighScores->unlockAchievement(achievementType);
        _appContext.saveScores = true;
        _unlockAchievements.push_back(achievementType);
    }
#endif
}

void GameMode::createSparks()
{
    if(_achievementGreenSparks != null)     _achievementGreenSparks->kill();
    if(_achievementOrangeSparks != null)    _achievementOrangeSparks->kill();
    if(_achievementPinkSparks != null)      _achievementPinkSparks->kill();
    if(_achievementRedSparks != null)       _achievementRedSparks->kill();
    if(_achievementTurquoiseSparks != null) _achievementTurquoiseSparks->kill();

    _achievementGreenSparks     = _appContext.pWorld->createInstance(L"/ChronoRageData/Explosions/achievement-green.group");
    _achievementOrangeSparks    = _appContext.pWorld->createInstance(L"/ChronoRageData/Explosions/achievement-orange.group");
    _achievementPinkSparks      = _appContext.pWorld->createInstance(L"/ChronoRageData/Explosions/achievement-pink.group");
    _achievementRedSparks       = _appContext.pWorld->createInstance(L"/ChronoRageData/Explosions/achievement-red.group");
    _achievementTurquoiseSparks = _appContext.pWorld->createInstance(L"/ChronoRageData/Explosions/achievement-turquoise.group");

    _appContext.pSoundLibrary->playSound(SOUND_ACHIEVEMENT, false);

    moveSparks();

    //Restart
    _achievementGreenSparks->restart();
    _achievementOrangeSparks->restart();
    _achievementPinkSparks->restart();
    _achievementRedSparks->restart();
    _achievementTurquoiseSparks->restart();
}

void GameMode::moveSparks()
{
    float screenHeight = (float)_appContext.pRenderer->getDefaultView()->getHeight();
    float achievementTextHeight = 200.0f;

    float achievementTextHeightFromCenter = (screenHeight / 2.0f) - 200.0f;

    float ratio = achievementTextHeightFromCenter / (screenHeight / 2.0f);

    Core::Vector3f cameraPosition = _appContext.pCameraNode->getWorldPosition();
    float distance = _pLevel->playgroundDepth() - cameraPosition.z;

    Core::Matrix4f proj;
    _appContext.pCameraNode->getBaseCamera()->getProjectionMatrixSplit(proj, 0.5f * distance, distance);

    Core::Matrix4f invProj(proj.getInverted());

    /*Core::Vector3f p00 = invProj.apply(Core::Vector4f(-1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p10 = invProj.apply(Core::Vector4f( 1.0f, -1.0f,  1.0f, 1.0f)).homogeneousDiv();
    Core::Vector3f p01 = invProj.apply(Core::Vector4f(-1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();*/
    Core::Vector3f p11 = invProj.apply(Core::Vector4f( 1.0f,  1.0f,  1.0f, 1.0f)).homogeneousDiv();

    float deltaX = p11.x + 1;
    float deltaY = p11.y + 1;

    Core::Vector3f sparksPos;
    sparksPos.x = cameraPosition.x;
    sparksPos.y = cameraPosition.y + (deltaY * ratio);
    sparksPos.z = _pLevel->playgroundDepth();

    Core::Vector3f greenSparksPos = sparksPos;
    greenSparksPos.x = sparksPos.x - (deltaX * 0.3f);
    greenSparksPos.y = sparksPos.y + (deltaY * 0.2f);
    Core::Vector3f orangeSparksPos = sparksPos;
    orangeSparksPos.x = sparksPos.x + (deltaX * 0.3f);
    orangeSparksPos.y = sparksPos.y + (deltaY * 0.2f);
    Core::Vector3f pinkSparksPos = sparksPos;
    pinkSparksPos.x = sparksPos.x - (deltaX * 0.2f);
    pinkSparksPos.y = sparksPos.y - (deltaY * 0.15f);
    Core::Vector3f redSparksPos = sparksPos;
    redSparksPos.x = sparksPos.x + (deltaX * 0.2f);
    redSparksPos.y = sparksPos.y - (deltaY * 0.15f);
    Core::Vector3f turquoiseSparksPos = sparksPos;

    //Position
    _achievementGreenSparks->setWorldPosition(greenSparksPos);
    _achievementOrangeSparks->setWorldPosition(orangeSparksPos);
    _achievementPinkSparks->setWorldPosition(pinkSparksPos);
    _achievementRedSparks->setWorldPosition(redSparksPos);
    _achievementTurquoiseSparks->setWorldPosition(turquoiseSparksPos);
}

}//namespace ChronoRage