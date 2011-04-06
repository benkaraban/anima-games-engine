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
#ifndef CHRONORAGE_BOSS_H_
#define CHRONORAGE_BOSS_H_

#include <Foes/FoeCommon.h>
#include <Foes/Shots/Laser.h>

namespace ChronoRage
{

const Core::String BOSS_FILE = L"/ChronoRageData/Foes/voxels-boss.tga";

const Core::String BOSS_LOADING_FILE = L"/ChronoRageData/Foes/Shots/concentration.group";

const float BOSS_HORNET_TIMER = 2.0f;
const float BOSS_LASER_TIMER = 3.0f;
const float BOSS_LOADING_TIMER = 3.0f;
const float BOSS_LASER_SHOT_TIMER = 5.0f;
const float BOSS_BLINK_TIME = 0.3f;
const int32 BOSS_BLINK_NUMBER = 1;

class LM_API_CHR Boss : public FoeCommon
{
public:
    Boss(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);
    virtual ~Boss();

    virtual void initialize(const Core::Vector3f & position);

protected:
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;

    virtual void update(double elapsed);

    virtual bool isCollisionActive() const;

    virtual void updateActive(double elapsed);
    virtual void updateCustomBehavior(double elapsed);
    virtual void updateCustomBehavior2(double elapsed);
    virtual void updateDestruction(double elapsed);

private:
    float   _hornetTimer;
    float   _laserTimer;
    float   _loadingTimer;
    float   _angle1;
    float   _angle2;
    Core::Vector3f  _rearCannon1;
    Core::Vector3f  _rearCannon2;

    Ptr<Universe::Node> _pLoadingParts;
    Ptr<Laser>          _pLaser;
    bool                _hasShot;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_BOSS_H_*/