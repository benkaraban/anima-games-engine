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
#ifndef CHRONORAGE_GAMEMODEREVENGE_H_
#define CHRONORAGE_GAMEMODEREVENGE_H_

#include <GUI/BitmapLabel.h>
#include <GUI/ProgressBar.h>

#include <Vortex.h>
#include <Modes/GameMode.h>
#include <Waves/Wave.h>

namespace ChronoRage
{

class LM_API_CHR GameModeRevenge : public Core::Object, public GameMode
{
public:
    GameModeRevenge(AppContext & appContext);
    virtual ~GameModeRevenge()
    {}

    inline virtual EGameStateMode getMode() const { return REVENGE_MODE; }

    virtual void registerWaveFile(const Core::String & waveFile);

    virtual void restart();

    virtual void initialize();
    virtual void update(double elapsed);

#if LM_DISPLAY_FPS
    virtual void createFoe(int32 foeId);
    virtual float getPowerLevel();
#endif

    virtual void displayPowerUp();
    virtual void displayPowerDown();
    virtual void displayTimeWarpBonus();

    virtual void deathPenalty();

protected:
    virtual void initPlayer(bool lookTarget);
    virtual void initVortex();
    virtual void initGUI();
    virtual void manageGUI();
    virtual void manageMusics();

protected:
    Ptr<Vortex>     _pVortex;
    Ptr<Vortex>     _pVortex2;

    double          _totalElapsed;
    int32           _nbLives;

    int32           _powerLevel;
    int32           _sameLevelCreated;

    float           _waveDuration;

    Core::List<int32>                           _wavesToBeCreated;
    Core::List<Ptr<Wave> >                      _activeWaves;
    Core::Map<int32, Core::List<Ptr<Wave> > >   _registeredWavesByPowerLevel;

    float                                       _createNextWaveTimer;

    Ptr<GUI::BitmapLabel>   _pTimerLabel;

    Ptr<GUI::ProgressBar>   _pTimeBomb1;
    Ptr<GUI::ProgressBar>   _pTimeBomb2;
    Ptr<GUI::ProgressBar>   _pTimeBomb3;
    Ptr<GUI::ProgressBar>   _pTimeBomb4;

    Ptr<GUI::Picture>       _pLifePicture;
    Ptr<GUI::BitmapLabel>   _pLifeCountLabel;

    Ptr<GUI::Widget>        _pScreenBestTimeDialog;
    Ptr<GUI::BitmapLabel>   _pBestTimeLabel;
    bool                    _bestTimeLabelVisible;

    float                   _timeBonusAppearance;
    Ptr<GUI::BitmapLabel>   _pTimeBonus;

    float                   _timeWarpBonusAppearance;
    Ptr<GUI::BitmapLabel>   _pTimeWarpBonusLabel;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODEREVENGE_H_*/