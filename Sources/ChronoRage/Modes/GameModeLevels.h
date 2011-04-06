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
#ifndef CHRONORAGE_GAMEMODELEVELS_H_
#define CHRONORAGE_GAMEMODELEVELS_H_

#include <GUI/BitmapLabel.h>
#include <GUI/ProgressBar.h>

#include <Vortex.h>
#include <Modes/GameMode.h>
#include <Waves/Wave.h>

namespace ChronoRage
{

const float VICTORY_PANEL_TIMER = 5.0f;

struct LM_API_CHR WaveOrder
{
    int32   id;
    float   duration;
    bool    launched;
};

class LM_API_CHR GameModeLevels : public Core::Object, public GameMode
{
public:
    GameModeLevels(AppContext & appContext);
    virtual ~GameModeLevels()
    {}

    inline virtual EGameStateMode getMode() const { return NEW_GAME_MODE; }

    virtual void restart();

    virtual void setLevel(int32 levelNumber);

    virtual void initialize();
    virtual void update(double elapsed);
#if LM_DISPLAY_FPS
    virtual void createFoe(int32 foeId);
#endif

    virtual void displayPowerUp();
    virtual void displayPowerDown();
    virtual void displayTimeWarpBonus();

    virtual void deathPenalty();

protected:
    void setLevelFile(const Core::String & levelFile, int32 secondsToClearLevel);
    virtual void initVortex();
    virtual void initGUI();
    virtual void initLevelCompleteBand();
    virtual void manageGUI();

    virtual void manageVictory();

protected:
    Ptr<Vortex>     _pVortex;
    Ptr<Vortex>     _pVortex2;

    // Config modifiable
    bool            _displayLevelLabel;
    bool            _changeTimerColor;

    double          _totalElapsed;

    int32           _levelNumber;
    Core::String    _levelFile;
    int32           _secondsToClearLevel;

    Core::LinkedList<WaveOrder> _waveOrders;
    Core::List<Ptr<Wave> >      _waves;

    Ptr<GUI::BitmapLabel>   _pTimerLabel;
    float                   _timerBlip;
    int32                   _msecToClearLevel;

    Ptr<GUI::ProgressBar>   _pPowerLevel;

    Ptr<GUI::ProgressBar>   _pTimeBomb1;
    Ptr<GUI::ProgressBar>   _pTimeBomb2;
    Ptr<GUI::ProgressBar>   _pTimeBomb3;
    Ptr<GUI::ProgressBar>   _pTimeBomb4;

    float                   _deathPenaltyAppearance;
    Ptr<GUI::BitmapLabel>   _pDeathPenalty;

    Ptr<GUI::Widget>        _pScreenLevelDialog;
    Ptr<GUI::BitmapLabel>   _pLevelLabel;
    bool                    _levelLabelVisible;

    float                   _victory;
    Ptr<GUI::Widget>        _pScreenVictoryDialog;
    Ptr<GUI::BitmapLabel>   _pLevelCompleteLabel;
    Ptr<GUI::BitmapLabel>   _pYourTimeLabel;
    Ptr<GUI::BitmapLabel>   _pBestTimeLabel;
    Ptr<GUI::BitmapLabel>   _pRankLabel;
    Ptr<GUI::Picture>       _pRankStarsL;
    Ptr<GUI::Picture>       _pRankStarsR;
    bool                    _victoryVisible;
    bool                    _musicFadedOut;

    float                   _powerUpAppearance;
    Ptr<GUI::BitmapLabel>   _pPowerUpLabel;

    float                   _powerDownAppearance;
    Ptr<GUI::BitmapLabel>   _pPowerDownLabel;

    float                   _timeWarpBonusAppearance;
    Ptr<GUI::BitmapLabel>   _pTimeWarpBonusLabel;

    bool                    _achievementPerfect;
    bool                    _achievementBareHanded;
    bool                    _achievementMarathon;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODELEVELS_H_*/