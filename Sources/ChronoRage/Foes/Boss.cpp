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
#include "Boss.h"

#include <Core/TGA.h>

#include <Foes/Shots/Hornet.h>
#include <Modes/GameMode.h>
#include <GameTools.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Boss::spTemplate;

Boss::Boss(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(BOSS, pWorld, pPlayer, gameMode),
    _hornetTimer(BOSS_HORNET_TIMER / 2.0f),
    _laserTimer(BOSS_LASER_TIMER / 2.0f),
    _angle1( 3.0f * f_PI_DIV_4),
    _angle2(-3.0f * f_PI_DIV_4),
    _rearCannon1( 1.0f, 0.0f, -2.0f),
    _rearCannon2(-1.0f, 0.0f, -2.0f),
    _pLoadingParts(null),
    _pLaser(null),
    _hasShot(false)
{
    _minRange = 0.0f;
    _maxRange = 0.0f;
    _speed = 2.0f;
    _radialSpeed = 3.0f;
    _maxHealthPoint = _healthPoint = 256.0f;
}

Boss::~Boss()
{
    if(_pLoadingParts != null)
    {
        _pLoadingParts->kill();
        _pLoadingParts = null;
    }

    if(_pLaser != null)
    {
        _pLaser->madeHit();
        _pLaser = null;
    }
}

void Boss::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 3.0f)));
}

Ptr<VoxelSpriteTemplate> Boss::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(BOSS_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Boss::update(double elapsed)
{
    if(_pLoadingParts != null && _pLoadingParts->isAlive())
    {
        _pLoadingParts->setTimeMultiplier(_gameMode.getTimeWarp()->getTimeWarpFactor(), true);
    }

    FoeCommon::update(elapsed);
}

bool Boss::isCollisionActive() const
{
    return _state == FOE_ACTIVE || _state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2;
}

void Boss::updateActive(double elapsed)
{
    _laserTimer = std::max(0.0f, _laserTimer - (float)elapsed);

    if(_laserTimer <= 0 && _gameMode.isVisible(getPosition()))
    {
        _loadingTimer = BOSS_LOADING_TIMER;
        _state = FOE_CUSTOM_BEHAVIOR;

        _pLoadingParts = _pWorld->createInstance(BOSS_LOADING_FILE);
        _pNodeFoe->addChild(_pLoadingParts);
    }

    FoeCommon::updateActive(elapsed);
}

void Boss::updateCustomBehavior(double elapsed)
{
    _loadingTimer = std::max(0.0f, _loadingTimer - (float)elapsed);
    _radialSpeed = std::max(0.1f, _loadingTimer);

    if(_loadingTimer <= 0.0f)
    {
        _hasShot = true;

        _laserTimer = BOSS_LASER_SHOT_TIMER;
        _hornetTimer = BOSS_HORNET_TIMER;
        _state = FOE_CUSTOM_BEHAVIOR2;

        _pLaser = Ptr<Laser>(new Laser(_pWorld, _pPlayer, _gameMode));
        _pLaser->initialize(Core::Vector3f(0.0f, 0.0f, 0.0f));
        _pNodeFoe->addChild(_pLaser->getNode());

        _gameMode.addFoeShot(_pLaser);
    }
    
    FoeCommon::updateActive(elapsed);
}

void Boss::updateCustomBehavior2(double elapsed)
{
    _laserTimer = std::max(0.0f, _laserTimer - (float)elapsed);
    _hornetTimer = std::max(0.0f, _hornetTimer - (float)elapsed);

    if(_hornetTimer <= 0.0f)
    {
        _pVoxelSprite->blink(BOSS_BLINK_TIME, BOSS_BLINK_NUMBER);

        Core::Matrix4f localToWorldMatrix = _pNodeFoe->getLocalToWorldMatrix();
        Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_rearCannon1);
        Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_rearCannon2);

        Ptr<Hornet> pHornet1(new Hornet(_pWorld, _pPlayer, _gameMode));
        pHornet1->initialize(shotPosition1);
        Ptr<Hornet> pHornet2(new Hornet(_pWorld, _pPlayer, _gameMode));
        pHornet2->initialize(shotPosition2);

        Core::Matrix4f rotationMatrix1(Core::ROTATION, _angle1, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Matrix4f rotationMatrix2(Core::ROTATION, _angle2, Core::Vector3f(0.0f, 0.0f, 1.0f));

        Core::Vector3f shotDirection1 = rotationMatrix1.apply(_pNodeFoe->getWorldDirection());
        Core::Vector3f shotDirection2 = rotationMatrix2.apply(_pNodeFoe->getWorldDirection());
        
        pHornet1->setDirection(shotDirection1);
        pHornet2->setDirection(shotDirection2);

        _gameMode.addFoeShot(pHornet1);
        _gameMode.addFoeShot(pHornet2);

        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _hornetTimer = BOSS_HORNET_TIMER;
    }

    if(_laserTimer <= 0.0f)
    {
        _state = FOE_ACTIVE;
        _radialSpeed = 3.0f;
        _laserTimer = BOSS_LASER_TIMER;

        if(_pLoadingParts != null && _pLoadingParts->isAlive())
        {
            _pLoadingParts->kill();
            _pLoadingParts = null;
        }

        if(_pLaser != null)
        {
            _pLaser->madeHit();
            _pLaser = null;
        }
    }
    
    FoeCommon::updateActive(elapsed);
}

void Boss::updateDestruction(double elapsed)
{
    if(_pLoadingParts != null && _pLoadingParts->isAlive())
    {
        _pLoadingParts->kill();
        _pLoadingParts = null;
    }

    if(_pLaser != null)
    {
        _pLaser->madeHit();
        _pLaser = null;
    }

    if(!_hasShot)
        _gameMode.displayAchievement(QUICK_DRAW);

    FoeCommon::updateDestruction(elapsed);
}

}//namespace ChronoRage