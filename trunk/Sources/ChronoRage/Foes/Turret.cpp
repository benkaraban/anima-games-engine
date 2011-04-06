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
#include "Turret.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Foes/Shots/RotatingBullet.h>
#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Turret::spTemplate;

Turret::Turret(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(TURRET, pWorld, pPlayer, gameMode),
    _fireCoolDown(TURRET_FIRE_COOL_DOWN)
{
    _minRange = 0.0f;
    _maxRange = 0.0f;
    _speed = 0.5f;
    _radialSpeed = 4.0f;
    _maxHealthPoint = _healthPoint = 64.0f;
    _tolerance = 2.0f;
}

Turret::~Turret()
{}

void Turret::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 2.5f)));
}

Ptr<VoxelSpriteTemplate> Turret::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(TURRET_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

bool Turret::isCollisionActive() const
{
    return _state == FOE_ACTIVE || _state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2;
}

void Turret::updateActive(double elapsed)
{
    _fireCoolDown = std::max(0.0f, _fireCoolDown - (float)elapsed);

    if(_fireCoolDown == 0.0)
    {
        _startDirection = _pNodeFoe->getWorldDirection();

        Core::Matrix4f rightRotMatrix(Core::ROTATION, -TURRET_FIRING_ANGLE/2, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Matrix4f leftRotMatrix(Core::ROTATION, TURRET_FIRING_ANGLE/2, Core::Vector3f(0.0f, 0.0f, 1.0f));

        _rightDirection = rightRotMatrix.apply(_startDirection);
        _leftDirection = leftRotMatrix.apply(_startDirection);

        _currentTimer = 0.0f;

        _state = FOE_CUSTOM_BEHAVIOR;
    }

    FoeCommon::updateActive(elapsed);
}

void Turret::updateCustomBehavior(double elapsed)
{
    _currentTimer = std::min(TURRET_IN_FIRE_POSITION_TIMER, _currentTimer + (float)elapsed);

    if(_currentTimer >= TURRET_IN_FIRE_POSITION_TIMER)
    {
        _pVoxelSprite->blink(TURRET_BLINK_TIME, TURRET_BLINK_NUMBER);

        _pNodeFoe->setWorldDirection(_rightDirection);
        _currentTimer = 0.0f;
        _shootTimer = 0.0f;
        _state = FOE_CUSTOM_BEHAVIOR2;
    }
    else
    {
        float ratio = _currentTimer / TURRET_IN_FIRE_POSITION_TIMER;
        Core::Matrix4f rightRotMatrix(Core::ROTATION, (-TURRET_FIRING_ANGLE * ratio)/2, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = rightRotMatrix.apply(_startDirection);

        _pNodeFoe->setWorldDirection(currentDir);
    }

    Foe::updateActive(elapsed);
}

void Turret::updateCustomBehavior2(double elapsed)
{
    if(_currentTimer < TURRET_SHOOTING_TIME1)
    {
        if(_shootTimer == 0.0f)
        {
            Ptr<RotatingBullet> pBullet(new RotatingBullet(_pWorld, _pPlayer, _gameMode));
            pBullet->initialize(getPosition());
            pBullet->setDirection(_pNodeFoe->getWorldDirection());
            _gameMode.addFoeShot(pBullet);
            _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

            _shootTimer = TURRET_SHOOTING_TIME1/TURRET_SHOT_NUMBER1;
        }
        _shootTimer = std::max(0.0f, _shootTimer - (float)elapsed);

        float ratio = _currentTimer / TURRET_SHOOTING_TIME1;
        Core::Matrix4f rightRotMatrix(Core::ROTATION, TURRET_FIRING_ANGLE * ratio, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = rightRotMatrix.apply(_rightDirection);

        _pNodeFoe->setWorldDirection(currentDir);

        _currentTimer = std::min(TURRET_SHOOTING_TIME1, _currentTimer + (float)elapsed);
    }
    else if(_currentTimer == TURRET_SHOOTING_TIME1)
    {
        _currentTimer += (float)elapsed;
        _pNodeFoe->setWorldDirection(_leftDirection);
    }
    else if(_currentTimer < TURRET_SHOOTING_TIME1 + TURRET_SHOOTING_TIME2)
    {
        if(_shootTimer == 0.0f)
        {
            Ptr<RotatingBullet> pBullet(new RotatingBullet(_pWorld, _pPlayer, _gameMode));
            pBullet->initialize(getPosition());
            pBullet->setDirection(_pNodeFoe->getWorldDirection());
            _gameMode.addFoeShot(pBullet);
            _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

            _shootTimer = TURRET_SHOOTING_TIME2/TURRET_SHOT_NUMBER2;
        }
        _shootTimer = std::max(0.0f, _shootTimer - (float)elapsed);

        float ratio = (_currentTimer - TURRET_SHOOTING_TIME1) / TURRET_SHOOTING_TIME2;
        Core::Matrix4f leftRotMatrix(Core::ROTATION, -TURRET_FIRING_ANGLE * ratio, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Vector3f currentDir(0.0f, 0.0f, 0.0f);
        currentDir = leftRotMatrix.apply(_leftDirection);

        _pNodeFoe->setWorldDirection(currentDir);
        _currentTimer += (float)elapsed;
    }
    else
    {
        _state = FOE_ACTIVE;
        _fireCoolDown = TURRET_FIRE_COOL_DOWN;
    }
    Foe::updateActive(elapsed);
}

}//namespace ChronoRage