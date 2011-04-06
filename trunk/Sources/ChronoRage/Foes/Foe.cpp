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
#include "Foe.h"

#include <Modes/GameMode.h>

namespace ChronoRage
{

Foe::Foe(EFoeType type, const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   _type(type),
    _pWorld(pWorld),
    _pPlayer(pPlayer),
    _gameMode(gameMode),
    _state(FOE_UNINITIALIZED),
    _pNodeFoe(null),
    _pShockWaveRefrac(null),
    _pCollisionShape(null),
    _createShockWave(true)
{
}

Foe::~Foe()
{
    if(_pNodeFoe != null)
        _pNodeFoe->kill();
    if(_pShockWaveRefrac != null)
        _pShockWaveRefrac->kill();
    if(_pVoxelSprite != null)
        _pVoxelSprite->kill();
}

int32 Foe::foeCount() const
{
    return 1;
}

bool Foe::displayWarning() const
{
    return false;
}

void Foe::initialize(const Core::Vector3f & position)
{
    _pNodeFoe = _pWorld->createNode();
    _pNodeFoe->setUpVector(Core::Vector3f(0.0f, 0.0f, -1.0f));

    Core::Vector3f direction = _pPlayer->getPosition() - position;
    if(direction.isZero())
        direction = Core::Vector3f(0.0f, 1.0f, 0.0f);
    else
        direction.normalizeFast();

    _pNodeFoe->setWorldDirection(direction);
    _pNodeFoe->setWorldPosition(position);

    _pVoxelSprite = Ptr<VoxelSprite>(new VoxelSprite(_pWorld, getTemplate()));

    _pVoxelSprite->setMatrices(_pNodeFoe->getLocalToWorldMatrix(), _pNodeFoe->getWorldToLocalMatrix());

    _state = FOE_CREATION_IN_PROGRESS;

    VoxelCloud cloud;
    cloud.position      = _pNodeFoe->getWorldToLocalMatrix().apply(Core::Vector3f(0.0f, 0.0f, 150.0f));
    cloud.cosAlpha      = Core::L_cos(f_PI);
    cloud.alpha         = f_PI;
    cloud.beta          = f_PI;
    cloud.dispRadius    = 1.0f;
    _pVoxelSprite->cloudToSprite(cloud, 1.0f, 1.5f);
}

Core::Vector3f Foe::getPosition() const
{
    return _pNodeFoe->getWorldPosition();
}

void Foe::setPosition(const Core::Vector3f & position)
{
    _pNodeFoe->setWorldPosition(position);
}

bool Foe::collidesPlayer()
{
    if(isAlive())
    {
        Ptr<Core::CollisionShape> pPlayerCollisionShape = _pPlayer->getCollisionShape();
        return (_pCollisionShape->collides(*pPlayerCollisionShape));
    }
    else
    {
        return false;
    }
}

bool Foe::collides(const Core::CollisionShape & shape) const
{
    if(isAlive())
    {
        return _pCollisionShape->collides(shape);
    }
    else
    {
        return false;
    }
}

void Foe::selfDestroy()
{
    damage(_healthPoint, Core::Vector3f(0.0f, 0.0f, 1.0f), false);
}

bool Foe::isAlive() const
{
    return (_state == FOE_ACTIVE || _state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2);
}

void Foe::process()
{
    _pVoxelSprite->update(_elapsed);
}

void Foe::startParallelUpdate()
{
    Core::Task::startTask(this);
}

void Foe::endParallelUpdate()
{
    Task::waitCompletion();
}

void Foe::update(double elapsed)
{
    float timeFreeze = _gameMode.getTimeWarp()->getTimeWarpFactor();

    elapsed *= timeFreeze;

    _elapsed = elapsed;
    _pVoxelSprite->setMatrices(_pNodeFoe->getLocalToWorldMatrix(), _pNodeFoe->getWorldToLocalMatrix());

    switch(_state)
    {
    case FOE_UNINITIALIZED:
        LM_ASSERT(false);
        throw Core::Exception(L"Unitialized foe");
        break;
    case FOE_DELETE_PENDING:
        LM_ASSERT(false);
        throw Core::Exception(L"Deleted foe");
        break;
    case FOE_CREATION_IN_PROGRESS:
        updateCreation(elapsed);
        break;
    case FOE_ACTIVE:
        updateActive(elapsed);
        break;
    case FOE_CUSTOM_BEHAVIOR:
        updateCustomBehavior(elapsed);
        break;
    case FOE_CUSTOM_BEHAVIOR2:
        updateCustomBehavior2(elapsed);
        break;
    case FOE_DESTRUCTION_IN_PROGRESS:
        updateDestruction(elapsed);
        break;
    }
}

void Foe::updateCreation(double elapsed)
{
    if(_pVoxelSprite->isAnimationFinished())
        _state = FOE_ACTIVE;
}

void Foe::damage(float d, const Core::Vector3f & direction, bool playerDestruction)
{
    _healthPoint -= d;

    VoxelExplosion explo;
    explo.dir           = direction;
    explo.gravityHole   = Core::Vector3f(0.0f, 0.0f, 150.0f);
    explo.minSpeed      = 30.0f;
    explo.maxSpeed      = 60.0f;
    explo.clampSpeed    = 5000.0f;
    explo.dispAngle     = 1.5f;
    explo.selfRotSpeed  = 10.0f * f_PI;

    if(_healthPoint <= 0.0f)
    {
        playDestructionSound();
        _pVoxelSprite->explosion(explo);

        _shockWaveRefracTimer = 0.0f;
        if(_createShockWave)
        {
            _pShockWaveRefrac = _pWorld->createDecal(SHOCKWAVE_REFRAC_FILE, Renderer::DECAL_REFRAC);
            Core::Vector3f refracPosition = getPosition();
            refracPosition.z += 1;
            _pShockWaveRefrac->setWorldPosition(refracPosition);
            _pShockWaveRefrac->setSize(0.0f, 0.0f);
            _pShockWaveRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            
            _shockWaveRefracTimer   = SHOCKWAVE_REFRAC_TIME;
        }

        _state = FOE_DESTRUCTION_IN_PROGRESS;

        if(_gameMode.getContext().pHighScores != null)
        {
            HighScores & hs = *_gameMode.getContext().pHighScores;
            int32 oldCount = hs.getPixelKilledCount();

            if(oldCount < PIXEL_KILLER_NUMBER)
            {
                int32 foePixelsCount = (_pVoxelSprite != null) ? foePixelsCount = _pVoxelSprite->pixelCount() : 0;
                int32 newCount = oldCount + foePixelsCount;
                hs.setPixelKilledCount(newCount);

                if(newCount >= PIXEL_KILLER_NUMBER)
                    _gameMode.displayAchievement(PIXEL_KILLER);
            }
        }

        if(foeCount() > 0) // Pour ne pas compter les bucklers et les credits
            _gameMode.foeKilled(getType());
        
        onDie();
    }
    else
    {
        _pVoxelSprite->partialExplosion(explo, (float(_healthPoint) / float(_maxHealthPoint)) * 0.5f + 0.5f);
    }
}

void Foe::playDestructionSound()
{
    _gameMode.playSound(SOUND_FOE_DESTRUCTION, false);
}

void Foe::updateActive(double elapsed)
{
}

void Foe::onDie()
{
}

void Foe::updateCollisionShape()
{
    _pCollisionShape->updateMatrices(_pNodeFoe->getLocalToWorldMatrix(), _pNodeFoe->getWorldToLocalMatrix());
}

bool Foe::isCollisionActive() const
{
    return _state == FOE_ACTIVE;
}

void Foe::testForCollisions()
{
    if(isCollisionActive())
    {
        const Core::List<Ptr<IShot> > & shots = _gameMode.getPlayerShots();
        Core::List<Ptr<IShot> >::const_iterator iShot = shots.begin();

        while( iShot != shots.end() && isAlive())
        {
            Ptr<Core::CollisionShape> pCollisionShape = (*iShot)->getCollisionShape();
            if(!(*iShot)->hasHit() && pCollisionShape != null && _pCollisionShape->collidesOverTime(*pCollisionShape))
            {        
                (*iShot)->madeHit();
                _gameMode.playSound(SOUND_PLAYER_SHOT_IMPACT, false);
                damage((*iShot)->getShotDamage(), (*iShot)->getDirection(), false);
            }

            ++iShot;
        }
    }
}

void Foe::updateCustomBehavior(double elapsed)
{}

void Foe::updateCustomBehavior2(double elapsed)
{}

void Foe::updateDestruction(double elapsed)
{
    if(_createShockWave)
    {
        _shockWaveRefracTimer = std::max(0.0f, _shockWaveRefracTimer - (float)elapsed);
        float ratioRefrac = (_shockWaveRefracTimer / SHOCKWAVE_REFRAC_TIME);
        if(_pShockWaveRefrac != null)
        {
            if(_shockWaveRefracTimer > 0.0f)
            {
                _pShockWaveRefrac->setSize(SHOCKWAVE_REFRAC_SIZE * (1 - ratioRefrac), SHOCKWAVE_REFRAC_SIZE * (1 - ratioRefrac) );
                _pShockWaveRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, Core::interpolate(ratioRefrac, Core::INTER_FADE_OUT)));
                Core::Vector3f position = _pShockWaveRefrac->getWorldPosition();
                position.z += (0.2f*(float)elapsed);
                _pShockWaveRefrac->setWorldPosition(position);
            }
            else
            {
                _pShockWaveRefrac->kill();
                _pShockWaveRefrac = null;
            }
        }
    }

    if(_pVoxelSprite->isAnimationFinished() && _shockWaveRefracTimer <= 0.0f)
        _state = FOE_DELETE_PENDING;
}


}//namespace ChronoRage