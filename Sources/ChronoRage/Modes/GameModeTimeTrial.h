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
#ifndef CHRONORAGE_GAMEMODETIMETRIAL_H_
#define CHRONORAGE_GAMEMODETIMETRIAL_H_

#include <Modes/GameModeLevels.h>

namespace ChronoRage
{

class LM_API_CHR GameModeTimeTrial : public GameModeLevels
{
public:
    GameModeTimeTrial(AppContext & appContext);
    virtual ~GameModeTimeTrial()
    {}

    virtual void initialize();

    inline virtual EGameStateMode getMode() const { return TIME_TRIAL_MODE; }

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

protected:
    virtual void manageEscapeKey();

    virtual void initLevelCompleteBand();
    virtual void manageVictory();

protected:
    bool                    _scoreSaved;
    int32                   _playerScore;
    Ptr<GUI::Widget>        _pButtonsPanel;
    Ptr<GUI::Button>        _pLevelCompleteButtonNextLevel;
    Ptr<GUI::Button>        _pLevelCompleteButtonRetry;
    Ptr<GUI::Button>        _pLevelCompleteButtonQuit;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODELEVELS_H_*/