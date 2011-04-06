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
#include "FoeShot.h"

namespace ChronoRage
{

const Core::String FOE_SHOT_EXPLOSION_FILE = L"/ChronoRageData/Explosions/shot-explosion.group";

FoeShot::FoeShot(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   _pWorld(pWorld),
    _pPlayer(pPlayer),
    _gameMode(gameMode),
    _pNodeFoeShot(null),
    _pCollisionShape(null),
    _hasHit(false),
    _collisionsActivated(true),
    _fadeTimer(-1.0f)
{}

FoeShot::~FoeShot()
{
    _pNodeFoeShot->kill();
    _pNodeFoeShot = null;
}

void FoeShot::updateCollisionShape()
{
    if(_pCollisionShape != null)
        _pCollisionShape->updateMatrices(_pNodeFoeShot->getLocalToWorldMatrix(), _pNodeFoeShot->getWorldToLocalMatrix());
}

bool FoeShot::collidesPlayer()
{
    if(isDying())
        return false;

    if(_pCollisionShape == null)
        return false;

    Ptr<Core::CollisionShape> pPlayerCollisionShape = _pPlayer->getCollisionShape();
    return (_pCollisionShape->collides(*pPlayerCollisionShape));
}

Core::Vector3f FoeShot::getPosition() const
{
    return _pNodeFoeShot->getWorldPosition();
}

Core::Vector3f FoeShot::getDirection() const
{
    return _pNodeFoeShot->getWorldDirection();
}

void FoeShot::setDirection(const Core::Vector3f & direction)
{
    _pNodeFoeShot->setWorldDirection(direction);
}

void FoeShot::update(double elapsed)
{
    if(_fadeTimer >= 0.0)
    {
        _fadeTimer += float(elapsed);

        float alpha = 1 - (_fadeTimer / FADE_TIMER);

        fade(alpha);
    }
}

bool FoeShot::isDead()
{
    return (_hasHit && _fadeTimer >= FADE_TIMER);
}

bool FoeShot::isDying()
{
    return (_hasHit && _fadeTimer >= 0.0f);
}

void FoeShot::kill(bool outOfBoundaries)
{
    if(!outOfBoundaries)
    {
        Ptr<Universe::Node> pExplosionInstance = _pWorld->createInstance(FOE_SHOT_EXPLOSION_FILE);
        pExplosionInstance->setWorldPosition(_pNodeFoeShot->getWorldPosition());
        Ptr<Universe::NodeGroup> pGroup = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pExplosionInstance);
        pGroup->killWhenChildrenAreDead(true);
        Ptr<Universe::NodeEmitter> pEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pGroup->getChild(0));
        pEmitter->killAtEnd(true);
        pEmitter->restart();

        _fadeTimer = 0.0f;
    }
    else
        _fadeTimer = FADE_TIMER;
}


void FoeShot::madeHit()
{
    _hasHit = true;
}

float FoeShot::getShotDamage() const
{
    return FOE_MEDIUM_DAMAGE;
}

bool FoeShot::hasHit() const
{
    return _hasHit;
}

Ptr<Core::CollisionShape> FoeShot::getCollisionShape() const
{
    if(_collisionsActivated)
        return _pCollisionShape;
    else 
        return null;
}

}//namespace ChronoRage