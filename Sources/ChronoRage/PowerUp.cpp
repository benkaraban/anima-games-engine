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
#include "PowerUp.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{

Ptr<VoxelSpriteTemplate> PowerUp::spTemplate;

PowerUp::PowerUp(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   _pWorld(pWorld),
    _pPlayer(pPlayer),
    _gameMode(gameMode),
    _pNodePowerUp(null),
    _pVoxelSprite(null),
    _state(POWER_UP_UNINITIALIZED),
    _createShockWave(true),
    _direction(0.0f, 1.0f, 0.0f),
    _color(0.23f, 1.0f, 0.0f, 1.0f),
    _width(2.0f),
    _height(2.0f),
    _speed(5.0f)
{}

PowerUp::~PowerUp()
{
    if(_pShockWaveRefrac != null)
        _pShockWaveRefrac->kill();
    _pNodePowerUp->kill();
}

void PowerUp::initialize(const Core::Vector3f & position)
{
    _pNodePowerUp = _pWorld->createNode();
    _pNodePowerUp->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));

    _pNodePowerUp->setWorldPosition(position);
    _pNodePowerUp->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 4*_width/5)));

    _pVoxelSprite = Ptr<VoxelSprite>(new VoxelSprite(_pWorld, getTemplate()));
    _pVoxelSprite->setMatrices(_pNodePowerUp->getLocalToWorldMatrix(), _pNodePowerUp->getWorldToLocalMatrix());

    _state = POWER_UP_CREATION_IN_PROGRESS;

    _pVoxelSprite->defaultSprite();
}

Core::Vector3f PowerUp::getPosition() const
{
    return _pNodePowerUp->getWorldPosition();
}

void PowerUp::updateCollisionShape()
{
    _pCollisionShape->updateMatrices(_pNodePowerUp->getLocalToWorldMatrix(), _pNodePowerUp->getWorldToLocalMatrix());
}

bool PowerUp::isCollisionActive() const
{
    return _state == FOE_ACTIVE;
}


void PowerUp::update(double elapsed)
{
    float timeFreeze = _gameMode.getTimeWarp()->getTimeWarpFactor();
    elapsed *= timeFreeze;

    _pVoxelSprite->setMatrices(_pNodePowerUp->getLocalToWorldMatrix(), _pNodePowerUp->getWorldToLocalMatrix());
    _pVoxelSprite->update(elapsed);

    switch(_state)
    {
    case POWER_UP_UNINITIALIZED:
        LM_ASSERT(false);
        throw Core::Exception(L"Unitialized PowerUp");
        break;
    case POWER_UP_DELETE_PENDING:
        LM_ASSERT(false);
        throw Core::Exception(L"Deleted PowerUp");
        break;
    case POWER_UP_CREATION_IN_PROGRESS:
        updateCreation(elapsed);
        break;
    case POWER_UP_ACTIVE:
        updateActive(elapsed);
        break;
    case POWER_UP_DESTRUCTION_IN_PROGRESS:
        updateDestruction(elapsed);
        break;
    }
}

void PowerUp::updateCreation(double elapsed)
{
    if(_pVoxelSprite->isAnimationFinished())
        _state = POWER_UP_ACTIVE;
}

void PowerUp::updateActive(double elapsed)
{
    Core::Matrix4f rotationMatrix(Core::ROTATION, (float)elapsed, Core::Vector3f(0.0f, 0.0f, 1.0f));

    _direction = rotationMatrix.apply(_direction);

    Core::Vector3f position = _pNodePowerUp->getWorldPosition();

    Core::Vector3f newPosition = position + (_direction * _speed * (float)elapsed);
    _gameMode.movePointInPlayground(newPosition);
    _pNodePowerUp->setWorldPosition(newPosition);
}

void PowerUp::updateDestruction(double elapsed)
{
    if(_createShockWave)
    {
        _shockWaveRefracTimer = std::max(0.0f, _shockWaveRefracTimer - (float)elapsed);
        float ratioRefrac = (_shockWaveRefracTimer / SHOCKWAVE_REFRAC_TIME);
        _pShockWaveRefrac->setSize(SHOCKWAVE_REFRAC_SIZE * (1 - ratioRefrac), SHOCKWAVE_REFRAC_SIZE * (1 - ratioRefrac) );
        _pShockWaveRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, Core::interpolate(ratioRefrac, Core::INTER_FADE_OUT)));
        Core::Vector3f position = _pShockWaveRefrac->getWorldPosition();
        position.z += (0.2f*(float)elapsed);
        _pShockWaveRefrac->setWorldPosition(position);
    }

    if(_pVoxelSprite->isAnimationFinished() && _shockWaveRefracTimer <= 0.0f)
        _state = POWER_UP_DELETE_PENDING;
}

Ptr<VoxelSpriteTemplate> PowerUp::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(POWERUP_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}


bool PowerUp::collidesPlayer()
{
    bool returnCode = false;

    Ptr<Core::CollisionShape> pPlayerCollisionShape = _pPlayer->getCollisionShape();

    if(_state == POWER_UP_ACTIVE && _pCollisionShape->collides(*pPlayerCollisionShape))
    {
        returnCode = true;

        selfDestruct();
    }

    return returnCode;
}

void PowerUp::selfDestruct()
{
    VoxelExplosion explo;
    explo.dir           = Core::Vector3f(0.0f, 0.0f, 1.0f);
    explo.gravityHole   = Core::Vector3f(0.0f, 0.0f, 150.0f);
    explo.minSpeed      = 30.0f;
    explo.maxSpeed      = 60.0f;
    explo.clampSpeed    = 5000.0f;
    explo.dispAngle     = 1.5f;
    explo.selfRotSpeed  = 10.0f * f_PI;

    _pVoxelSprite->explosion(explo);

    _shockWaveRefracTimer = 0.0f;
    if(_createShockWave && _pShockWaveRefrac == null)
    {
        _pShockWaveRefrac = _pWorld->createDecal(SHOCKWAVE_REFRAC_FILE, Renderer::DECAL_REFRAC);
        Core::Vector3f refracPosition = getPosition();
        refracPosition.z += 1;
        _pShockWaveRefrac->setWorldPosition(refracPosition);
        _pShockWaveRefrac->setSize(0.0f, 0.0f);
        _pShockWaveRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        
        _shockWaveRefracTimer   = SHOCKWAVE_REFRAC_TIME;
    }

    _state = POWER_UP_DESTRUCTION_IN_PROGRESS;
}

bool PowerUp::isKilled()
{
    return (_state == POWER_UP_DELETE_PENDING);
}

}//namespace ChronoRage