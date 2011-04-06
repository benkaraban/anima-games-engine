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
#include "Teleporter.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Modes/GameMode.h>
#include <Foes/Shots/HomingMissile.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Teleporter::spTemplate;

Teleporter::Teleporter(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(TELEPORTER, pWorld, pPlayer, gameMode),
    _teleportTime(TELEPORT_TIMER),
    _travelTime(0.0f),
    _shotTimer(1.0f)
{
    _minRange = 30.0f;
    _maxRange = 40.0f;
    _speed = 0.0f;
    _radialSpeed = 2.0f;
    _maxHealthPoint = _healthPoint = 2.0f;
    
}

Teleporter::~Teleporter()
{}

void Teleporter::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.3333f)));

    _approachPoint = Core::Vector3f(0.0f, 0.0f, 0.0f);
}

bool Teleporter::collidesPlayer()
{
    if(_state == FOE_ACTIVE)
    {
        Ptr<Core::CollisionShape> pPlayerCollisionShape = _pPlayer->getCollisionShape();
        return (_pCollisionShape->collides(*pPlayerCollisionShape));
    }
    else
    {
        return false;
    }
}

Ptr<VoxelSpriteTemplate> Teleporter::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(TELEPORTER_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Teleporter::updateActive(double elapsed)
{
    Core::Vector3f distanceVector = _pPlayer->getPosition() - getPosition();

    float distance = 0.0f;
    if(!distanceVector.isZero())
        distance = distanceVector.getLengthFast();

    bool isVisible = _gameMode.isVisible(getPosition());

    if( !isVisible || (distance < _minRange || distance > _maxRange) )
    {
        _teleportTime = std::max(0.0f, _teleportTime - (float)elapsed);
    }

    if( _teleportTime <= 0.0f)
    {
        _state = FOE_CUSTOM_BEHAVIOR;
        _oldPosition = getPosition();

        _newPosition.x = Core::frand(_minRange, _maxRange);
        _newPosition.y = 0.0f;
        _newPosition.z = 0.0f;
        float angle = Core::frand(0.0f, f_PI_MUL_2);
        Core::Matrix4f rotationMatrix(Core::ROTATION, angle, Core::Vector3f(0.0f, 0.0f, 1.0f));
        _newPosition = rotationMatrix.apply(_newPosition);
        _newPosition += _pPlayer->getPosition();
        _gameMode.movePointInPlayground(_newPosition);

        static const float TELEPORT_SPEED = 60.0f;

        float teleportTime = _newPosition.getDistance(_oldPosition) / TELEPORT_SPEED;

        Core::Matrix4f oldLocalToWorldMat = _pNodeFoe->getLocalToWorldMatrix();
        Core::Matrix4f oldWorldToLocalMat = _pNodeFoe->getWorldToLocalMatrix();
        Core::Matrix4f oldMat = _pNodeFoe->getLocalMatrix();

        _pNodeFoe->setWorldPosition(_newPosition);
        _pNodeFoe->worldLookAt(_pPlayer->getPosition());

        _newMat = _pNodeFoe->getLocalMatrix();

        Core::Matrix4f cvtMat = _pNodeFoe->getWorldToLocalMatrix();

        _pVoxelSprite->setMatrices(_pNodeFoe->getLocalToWorldMatrix(), _pNodeFoe->getWorldToLocalMatrix());
        _pVoxelSprite->teleport(cvtMat, 0.25f, teleportTime);
    }

    if(isVisible)
        _shotTimer = std::max(0.0f, _shotTimer - (float)elapsed);

    if(_shotTimer <= 0 && isVisible)
    {
        Ptr<HomingMissile> pHoming(new HomingMissile(_pWorld, _pPlayer, _gameMode));
        pHoming->initialize(getPosition());
        _gameMode.addFoeShot(pHoming);
        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _shotTimer = TELEPORTER_SHOT_TIMER;
        _teleportTime = 0.0f;
    }

    FoeCommon::updateActive(elapsed);
}

void Teleporter::updateCustomBehavior(double elapsed)
{
    if(_pVoxelSprite->isAnimationFinished())
    {
        _pNodeFoe->setLocalMatrix(_newMat);
        _state = FOE_ACTIVE;
        _teleportTime = TELEPORT_TIMER;
    }
}

}//namespace ChronoRage
