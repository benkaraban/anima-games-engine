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
#include "LaserTurret.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Foes/Shots/RotatingBullet.h>
#include <Modes/GameMode.h>

namespace ChronoRage
{
const Core::String TURRET_FILE = L"/ChronoRageData/Foes/voxels-turret-laser.tga";

const float LASER_TURRET_FIRE_COOL_DOWN = 5.0f;
const float LASER_TURRET_IN_FIRE_POSITION_TIMER = 0.3f;
const float LASER_TURRET_FIRING_ANGLE = f_PI_DIV_2 * 0.5f;
const float LASER_TURRET_SHOOTING_TIME1 = 4.0f;
const float LASER_TURRET_SHOOTING_TIME2 = 8.0f;
const int32 LASER_TURRET_SHOT_NUMBER1 = 8;
const int32 LASER_TURRET_SHOT_NUMBER2 = 6;
const float LASER_TURRET_BLINK_TIME = 0.3f;
const int32 LASER_TURRET_BLINK_NUMBER = 1;

Ptr<VoxelSpriteTemplate> LaserTurret::spTemplate;

LaserTurret::LaserTurret(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(LASER_TURRET, pWorld, pPlayer, gameMode),
    _fireCoolDown(LASER_TURRET_FIRE_COOL_DOWN)
{
    _minRange = 0.0f;
    _maxRange = 0.0f;
    _speed = 0.5f;
    _radialSpeed = 4.0f;
    _maxHealthPoint = _healthPoint = 64.0f;
    _tolerance = 2.0f;
}

LaserTurret::~LaserTurret()
{
    if(_pLaser != null)
    {
        _pLaser->madeHit();
        _pLaser = null;
    }
}

void LaserTurret::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 2.5f)));
}

Ptr<VoxelSpriteTemplate> LaserTurret::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(TURRET_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

bool LaserTurret::isCollisionActive() const
{
    return _state == FOE_ACTIVE || _state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2;
}

void LaserTurret::updateActive(double elapsed)
{
    _fireCoolDown = std::max(0.0f, _fireCoolDown - (float)elapsed);

    if(_fireCoolDown == 0.0)
    {
        _startDirection = _pNodeFoe->getWorldDirection();

        Core::Matrix4f rightRotMatrix(Core::ROTATION, -LASER_TURRET_FIRING_ANGLE/2, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Matrix4f leftRotMatrix(Core::ROTATION, LASER_TURRET_FIRING_ANGLE/2, Core::Vector3f(0.0f, 0.0f, 1.0f));

        _rightDirection = rightRotMatrix.apply(_startDirection);
        _leftDirection = leftRotMatrix.apply(_startDirection);

        _currentTimer = 0.0f;

        _state = FOE_CUSTOM_BEHAVIOR;
    }

    FoeCommon::updateActive(elapsed);
}

void LaserTurret::updateCustomBehavior(double elapsed)
{
    _currentTimer = std::min(LASER_TURRET_IN_FIRE_POSITION_TIMER, _currentTimer + (float)elapsed);

    if(_currentTimer >= LASER_TURRET_IN_FIRE_POSITION_TIMER)
    {
        _pVoxelSprite->blink(LASER_TURRET_BLINK_TIME, LASER_TURRET_BLINK_NUMBER);

        _pNodeFoe->setWorldDirection(_rightDirection);
        _currentTimer = 0.0f;
        _state = FOE_CUSTOM_BEHAVIOR2;

        _pLaser = Ptr<Laser>(new Laser(_pWorld, _pPlayer, _gameMode));
        _pLaser->initialize(Core::Vector3f(0.0f, 0.0f, 0.0f));
        _pNodeFoe->addChild(_pLaser->getNode());
        _gameMode.addFoeShot(_pLaser);
        _gameMode.playSound(SOUND_FOE_SHOT_LASER, false);
    }
    else
    {
        float ratio = _currentTimer / LASER_TURRET_IN_FIRE_POSITION_TIMER;
        Core::Matrix4f rightRotMatrix(Core::ROTATION, (-LASER_TURRET_FIRING_ANGLE * ratio)/2, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = rightRotMatrix.apply(_startDirection);

        _pNodeFoe->setWorldDirection(currentDir);
    }

    Foe::updateActive(elapsed);
}

void LaserTurret::updateCustomBehavior2(double elapsed)
{
    if(_currentTimer < LASER_TURRET_SHOOTING_TIME1)
    {
        float ratio = _currentTimer / LASER_TURRET_SHOOTING_TIME1;
        Core::Matrix4f rightRotMatrix(Core::ROTATION, LASER_TURRET_FIRING_ANGLE * ratio, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = rightRotMatrix.apply(_rightDirection);

        _pNodeFoe->setWorldDirection(currentDir);

        _currentTimer = std::min(LASER_TURRET_SHOOTING_TIME1, _currentTimer + (float)elapsed);
    }
    else if(_currentTimer == LASER_TURRET_SHOOTING_TIME1)
    {
        _currentTimer += (float)elapsed;
        _pNodeFoe->setWorldDirection(_leftDirection);
    }
    else if(_currentTimer < LASER_TURRET_SHOOTING_TIME1 + LASER_TURRET_SHOOTING_TIME2)
    {
        float ratio = (_currentTimer - LASER_TURRET_SHOOTING_TIME1) / LASER_TURRET_SHOOTING_TIME2;
        Core::Matrix4f leftRotMatrix(Core::ROTATION, -LASER_TURRET_FIRING_ANGLE * ratio, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = leftRotMatrix.apply(_leftDirection);

        _pNodeFoe->setWorldDirection(currentDir);
        _currentTimer += (float)elapsed;
    }
    else
    {
        if(_pLaser != null)
        {
            _pLaser->madeHit();
            _pLaser = null;
        }

        _state = FOE_ACTIVE;
        _fireCoolDown = LASER_TURRET_FIRE_COOL_DOWN;
    }

    Foe::updateActive(elapsed);
}

void LaserTurret::updateDestruction(double elapsed)
{
    //if(_pLoadingParts != null && _pLoadingParts->isAlive())
    //{
    //    _pLoadingParts->kill();
    //    _pLoadingParts = null;
    //}

    if(_pLaser != null)
    {
        _pLaser->madeHit();
        _pLaser = null;
    }

    FoeCommon::updateDestruction(elapsed);
}


}//namespace ChronoRage