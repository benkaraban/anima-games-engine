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
#ifndef CHRONORAGE_TURRET_H_
#define CHRONORAGE_TURRET_H_

#include <Foes/FoeCommon.h>

namespace ChronoRage
{

const Core::String TURRET_FILE = L"/ChronoRageData/Foes/voxels-turret.tga";

const float TURRET_FIRE_COOL_DOWN = 5.0f;
const float TURRET_IN_FIRE_POSITION_TIMER = 0.3f;
const float TURRET_FIRING_ANGLE = f_PI_DIV_2;
const float TURRET_SHOOTING_TIME1 = 1.0f;
const float TURRET_SHOOTING_TIME2 = 1.5f;
const int32 TURRET_SHOT_NUMBER1 = 8;
const int32 TURRET_SHOT_NUMBER2 = 6;
const float TURRET_BLINK_TIME = 0.3f;
const int32 TURRET_BLINK_NUMBER = 1;

class LM_API_CHR Turret : public FoeCommon
{
public:
    Turret(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);
    virtual ~Turret();

    virtual void initialize(const Core::Vector3f & position);

    virtual void updateCustomBehavior(double elapsed);
    virtual void updateCustomBehavior2(double elapsed);

protected:
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;

    virtual void updateActive(double elapsed);
    virtual bool isCollisionActive() const;

private:
    float _fireCoolDown;
    
    Core::Vector3f  _startDirection;
    Core::Vector3f  _leftDirection;
    Core::Vector3f  _rightDirection;
    float           _currentTimer;
    float           _shootTimer;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_TURRET_H_*/