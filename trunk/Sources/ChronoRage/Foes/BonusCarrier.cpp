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
#include "BonusCarrier.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> BonusCarrier::spTemplate;

BonusCarrier::BonusCarrier(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, bool withShield)
:   FoeCommon(BONUS_CARRIER, pWorld, pPlayer, gameMode),
    _pBuckler(null),
    _createBonus(true)
{
    _minRange = 5.0f;
    _maxRange = 15.0f;
    _speed = 5.0f;
    _radialSpeed = 0.5f;
    _maxHealthPoint = _healthPoint = 4.0f;

    if(withShield)
    {
        _pBuckler = Ptr<Buckler>(new Buckler(pWorld, pPlayer, gameMode));
        gameMode.addFoe(_pBuckler);
    }
}

BonusCarrier::~BonusCarrier()
{}

void BonusCarrier::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.0f)));
  
    _approachPoint.x = Core::frand(_gameMode.getLevelXMin(), _gameMode.getLevelXMax());
    _approachPoint.y = Core::frand(_gameMode.getLevelYMin(), _gameMode.getLevelYMax());
    _approachPoint.z = _pPlayer->getPosition().z;

    if(_pBuckler != null)
    {
        _pBuckler->initialize(position);
    }
}

void BonusCarrier::updateActive(double elapsed)
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
        _approachPoint.x = Core::frand(_gameMode.getLevelXMin(), _gameMode.getLevelXMax());
        _approachPoint.y = Core::frand(_gameMode.getLevelYMin(), _gameMode.getLevelYMax());
        _approachPoint.z = _pPlayer->getPosition().z;
    }

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = currentPosition + (direction * _speed * (float)elapsed);

    _gameMode.movePointInPlayground(newPosition);
    _pNodeFoe->setWorldPosition(newPosition);

    Foe::updateActive(elapsed);

    if(_pBuckler != null)
    {
        _pBuckler->setPosition(getPosition());
    }
}

void BonusCarrier::updateDestruction(double elapsed)
{
    if(_createBonus)
    {
        _createBonus = false;

        _gameMode.createPowerUp(getPosition());
    }

    FoeCommon::updateDestruction(elapsed);
}

Ptr<VoxelSpriteTemplate> BonusCarrier::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(BONUS_CARRIER_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void BonusCarrier::update(double elapsed)
{
    FoeCommon::update(elapsed);

    if(_state == FOE_DESTRUCTION_IN_PROGRESS && _pBuckler != null && _pBuckler->isAlive())
    {
        _pBuckler->selfDestroy();
    }
}

}//namespace ChronoRage