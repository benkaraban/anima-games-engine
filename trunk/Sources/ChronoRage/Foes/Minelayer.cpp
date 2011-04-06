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
#include "Minelayer.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Modes/GameMode.h>
#include <Foes/Shots/Mine.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Minelayer::spTemplate;

const float DROP_MINE_TIMER = 3.0f;
const float MINELAYER_BLINK_TIME = 0.3f;
const int32 MINELAYER_BLINK_NUMBER = 1;

Minelayer::Minelayer(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(MINELAYER, pWorld, pPlayer, gameMode)
{
    _minRange = 5.0f;
    _maxRange = 15.0f;
    _speed = 5.0f;
    _radialSpeed = 0.5f;
    _maxHealthPoint = _healthPoint = 4.0f;

    _pBuckler = Ptr<Buckler>(new Buckler(pWorld, pPlayer, gameMode));
    gameMode.addFoe(_pBuckler);
}

Minelayer::~Minelayer()
{}

void Minelayer::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);
    _lastShot = DROP_MINE_TIMER;

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.0f)));

    _approachPoint.x = Core::frand(_gameMode.getLevelXMin() * 0.9f, _gameMode.getLevelXMax() * 0.9f);
    _approachPoint.y = Core::frand(_gameMode.getLevelYMin() * 0.9f, _gameMode.getLevelYMax() * 0.9f);
    _approachPoint.z = _pPlayer->getPosition().z;

    _pBuckler->initialize(position);
}

Ptr<VoxelSpriteTemplate> Minelayer::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(MINELAYER_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Minelayer::update(double elapsed)
{
    FoeCommon::update(elapsed);

    if(_state == FOE_DESTRUCTION_IN_PROGRESS && _pBuckler->isAlive())
        _pBuckler->selfDestroy();
}

void Minelayer::updateActive(double elapsed)
{
    Core::Vector3f playerPosition = _pPlayer->getPosition();
    
    Core::Vector3f target = _approachPoint;
    _gameMode.movePointInPlayground(target);

    Core::Vector3f distance = target - getPosition();
    
    if(!distance.isZero() && distance.getLengthFast() > _tolerance)
    {
        Core::Vector3f localTarget(_pNodeFoe->getParentWorldToLocalMatrix().apply(target));
        Core::Vector3f localDir(localTarget - _pNodeFoe->getLocalPosition());
        Core::Matrix3f mat;
        mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoe->getUpVector(), _pNodeFoe->getLocalMatrix().getYVector());

        Core::Quaternionf q1(_pNodeFoe->getLocalOrient());
        Core::Quaternionf q2(mat);

        double rotTime = elapsed;
        _angleWithPlayer = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
        LM_ASSERT(Core::isValid(_angleWithPlayer));
        float maxAngle = float(_radialSpeed * rotTime);
        float alpha = 1.0;

        if(_angleWithPlayer != 0.0)
            alpha = std::min(1.0f, maxAngle / _angleWithPlayer);

        Core::Quaternionf newOrient(q1, q2, alpha);

        _pNodeFoe->setLocalOrientation(newOrient);
    }
    else
    {
        _approachPoint.x = Core::frand(_gameMode.getLevelXMin() * 0.9f, _gameMode.getLevelXMax() * 0.9f);
        _approachPoint.y = Core::frand(_gameMode.getLevelYMin() * 0.9f, _gameMode.getLevelYMax() * 0.9f);
        _approachPoint.z = _pPlayer->getPosition().z;
    }

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = currentPosition + (direction * _speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    Foe::updateActive(elapsed);

    _pBuckler->setPosition(getPosition());

    _lastShot = std::max(0.0f, _lastShot - float(elapsed));

    if(_lastShot <= 0.0f)
    {
        _pVoxelSprite->blink(MINELAYER_BLINK_TIME, MINELAYER_BLINK_NUMBER);

        Ptr<Mine> pMine(new Mine(_pWorld, _pPlayer, _gameMode));
        pMine->initialize(getPosition());
        _gameMode.addFoeShot(pMine);
        _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

        _lastShot = DROP_MINE_TIMER;
    }
}

}//namespace ChronoRage