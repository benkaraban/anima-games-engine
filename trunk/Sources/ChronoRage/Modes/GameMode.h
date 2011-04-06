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
#ifndef CHRONORAGE_GAMEMODE_H_
#define CHRONORAGE_GAMEMODE_H_

#include <Core/Debug/FPSCounter.h>
#include <Core/DoubleQueue.h>

#include <GUI/BitmapLabel.h>

#include <Modes/IGameState.h>
#include <Player.h>
#include <Level.h>
#include <TimeWarp.h>
#include <PowerUp.h>
#include <BonusDirection.h>
#include <WarningDirection.h>
#include <FoeDirection.h>
#include <Foes/Foe.h>
#include <Foes/Shots/FoeShot.h>
#include <Waves/Wave.h>
#include <GameTools.h>

namespace ChronoRage
{

const int32 HELP_FIND_REMAINING_FOES = 3;

LM_ENUM_5(EGameCurrentState,    GAME_ENTER_NAME,
                                GAME_NOT_STARTED, 
                                GAME_STARTED, 
                                GAME_WON, 
                                GAME_LOST);


LM_ENUM_6(EGameMessageBoxWidgetId,  BUTTON_RESUME, 
                                    BUTTON_NEXT_LEVEL,
                                    BUTTON_RETRY, 
                                    BUTTON_QUIT,
                                    BUTTON_ENTER_NAME_OK, 
                                    BUTTON_BUY_GAME);

                              

const float START_TIMER = 5.0f;

class LM_API_CHR GameMode : public IGameState, public GUI::IEventHandler
{
public:
    GameMode(AppContext & appContext);
    virtual ~GameMode();

    virtual void registerWaveFile(const Core::String & waveFile);

    virtual void restart();

#if LM_DISPLAY_FPS
    virtual void clearWaves();
#endif

    virtual void initialize();

    virtual void update(double elapsed);

    virtual void addFoe(const Ptr<Foe> & pFoe);
    virtual void addDelayedFoe(const Ptr<Foe> & pFoe);
    virtual void addFoeShot(const Ptr<FoeShot> & pFoeShot);

    virtual void createPowerUp(const Core::Vector3f & position);

    virtual void manageTimeWarp(double elapsed);

    virtual int32 getFoesNumber();

    virtual void foeKilled(EFoeType type);
    virtual void foeKilledTimeWarpBonus();

    virtual float getTimeWarpNumber();

    virtual void powerUp();
    virtual void powerDown();
    virtual float getPowerLevel();

    const Ptr<TimeWarp> & getTimeWarp() const;

    virtual void displayPowerUp() = 0;
    virtual void displayPowerDown() = 0;
    virtual void displayTimeWarpBonus() = 0;

    virtual const Core::List<Ptr<IShot> > & getPlayerShots() const;
    virtual const Core::List<Ptr<Foe> > &   getFoes() const;

    virtual void deathPenalty() = 0;

    inline float getLevelXMin() const;
    inline float getLevelXMax() const;
    inline float getLevelYMin() const;
    inline float getLevelYMax() const;

    void movePointInPlayground(Core::Vector3f & point);
    bool isOutOfBoundaries(Core::Vector3f & point, float tolerance);

    bool isVisible(const Core::Vector3f & position);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void killAllFoes();

    virtual void displayAchievement(EAchievementType achievementType);

protected:
    virtual void initPlayer(bool lookTarget);
    virtual void manageEscapeKey();

    virtual void cameraTracking();
    virtual void computeCameraDelta();
    virtual void computePlayerDelta();
    virtual void playerDie();

    virtual void showBonusDirections();
    virtual void showWarningDirections();
    virtual void showFoeDirections();

    virtual void setReticleCursor();

    virtual void createSparks();
    virtual void moveSparks();

protected:
    Core::FPSCounter          _fps;
    Core::DoubleQueue<float>  _frameDurationHistory;

    EGameCurrentState   _gameCurrentState;
    bool                _foeCollisionsEnabled;
    Ptr<IPlayerCtrl>    _pControl;
    Ptr<Player>         _pPlayer;
    Ptr<Level>          _pLevel;

    Core::List<Ptr<Foe> >       _foes;
    Core::List<Ptr<Foe> >       _delayedFoes;
    Core::List<Ptr<IShot> >     _foeShots;

    Core::List<Ptr<PowerUp> >   _powerUps;

    float       _cameraDeltaX;
    float       _cameraDeltaY;
    bool        _computeCameraDeltaDone;

    float       _playerDeltaX;
    float       _playerDeltaY;
    bool        _computePlayerDeltaDone;

    bool        _gamePaused;
    int32       _powerLevel;

    Ptr<TimeWarp>   _pTimeWarp;

    Core::String                    _waveFile;
    Core::Map<int32, Ptr<Wave> >    _registeredWaves;

    FoeDirection                        _foeDirections[HELP_FIND_REMAINING_FOES];
    Core::List<Ptr<WarningDirection> >  _warningDirections;
    Core::List<Ptr<BonusDirection> >    _bonusDirections;

    Ptr<GUI::Widget>        _pScreenDialog;

    Ptr<GUI::Widget>        _pGamePausedDialog;
    Ptr<GUI::Widget>        _pGamePausedPanel;

    Ptr<GUI::Button>        _pQuitButtonResume;
    Ptr<GUI::Button>        _pQuitButtonRetry;
    Ptr<GUI::Button>        _pQuitButtonQuit;

    bool                    _gameOverVisible;
    Ptr<GUI::Widget>        _pGameOverDialog;
    Ptr<GUI::Widget>        _pGameOverPanel;
    Ptr<GUI::Button>        _pGameOverButtonRetry;
    Ptr<GUI::Button>        _pGameOverButtonQuit;

    bool                    _enterNameVisible;
    bool                    _hasPlayerEnteredHisName;
    Ptr<GUI::Widget>        _pEnterNameDialog;
    Ptr<GUI::Widget>        _pEnterNamePanel;
    Ptr<GUI::Label>         _pLabelEnterName;
    Ptr<TextEdit>           _pTextEditEnterName;
    Ptr<GUI::Button>        _pButtonEnterNameOK;

    float                   _displayAchievement;
    Ptr<GUI::Widget>        _pAchievementDialog;
    Ptr<GUI::BitmapLabel>   _pAchievementText;
    Ptr<GUI::BitmapLabel>   _pAchievementUnlocked;
    Ptr<GUI::Picture>       _pAchievementPicture;

    Core::List<EAchievementType>    _unlockAchievements;

    Ptr<Universe::Node>     _achievementGreenSparks;
    Ptr<Universe::Node>     _achievementOrangeSparks;
    Ptr<Universe::Node>     _achievementPinkSparks;
    Ptr<Universe::Node>     _achievementRedSparks;
    Ptr<Universe::Node>     _achievementTurquoiseSparks;

    int32                   _achievementFoeKilled;
    int32                   _achievementBossKilled;
    int32                   _achievementFoeKilledDuringTimeWarp;
    bool                    _achievementCreditsHunter;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODE_H_*/