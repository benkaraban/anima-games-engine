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
#ifndef CHRONORAGE_GAMEMODE1_H_
#define CHRONORAGE_GAMEMODE1_H_

#include <Vortex.h>
#include <Modes/GameMode.h>
#include <Controls/IPlayerCtrl.h>
#include <Waves/Wave.h>

namespace ChronoRage
{

class LM_API_CHR GameMode1 : public Core::Object, public GameMode
{
public:
    GameMode1(AppContext & appContext)
    :   GameMode(appContext),
        _createWave(0.0)
    {}

    virtual ~GameMode1()
    {}

    inline virtual EGameStateMode getMode() const { return NEW_GAME_MODE; }

    virtual void initialize();

    virtual void update(double elapsed);

    virtual void createFoe(int32 foeId);

    virtual void displayPowerUp();
    virtual void displayPowerDown();
    virtual void displayTimeWarpBonus();

    virtual void deathPenalty();

private:
    void loadWave();

protected:
    float           _createWave;
    Ptr<Vortex>     _pVortex;
    Ptr<Vortex>     _pVortex2;

    Core::List<Ptr<Wave> > _waves;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GAMEMODE1_H_*/