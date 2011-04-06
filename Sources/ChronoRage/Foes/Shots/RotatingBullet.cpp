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
#include "RotatingBullet.h"

#include <Modes/GameMode.h>

namespace ChronoRage
{

const float ACTIVATE_COLLISION_TIMER = 0.4f;

RotatingBullet::RotatingBullet(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeShot(pWorld, pPlayer, gameMode),
    _width(3.0f),
    _height(3.0f),
    _speed(20.0f),
    _totalElapsed(0.0f)
{}

void RotatingBullet::initialize(const Core::Vector3f & position)
{
    _direction = _pPlayer->getPosition() - position;
    if(_direction.isZero())
        _direction = Core::Vector3f(0.0f, 1.0f, 0.0f);
    else
        _direction.normalizeFast();

    _pNodeFoeShot = _pWorld->createDecal(ROTATING_BULLET_FILE, Renderer::DECAL_ADD);
    _pNodeFoeShot->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeFoeShot->setWorldDirection(_direction);
    _pNodeFoeShot->setWorldPosition(position);

    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);
    pNodeDecal->setSize(_width, _height);
    pNodeDecal->setGlow(Core::Vector4f(1.0f, 0.0f, 1.0f, 0.5f));
    pNodeDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), _width/3)));
    _collisionsActivated = false;
}

void RotatingBullet::update(double elapsed)
{
    elapsed *= _gameMode.getTimeWarp()->getTimeWarpFactor();

    _totalElapsed += float(elapsed);

    if(_totalElapsed > ACTIVATE_COLLISION_TIMER)
        _collisionsActivated = true;

    Core::Vector3f newPosition = getPosition() + (_direction * _speed * (float)elapsed);
    _pNodeFoeShot->setWorldPosition(newPosition);

    Core::Matrix4f rotationMatrix(Core::ROTATION, f_PI_MUL_2 * ROTATION_PER_SECOND * (float)elapsed, Core::Vector3f(0.0f, 0.0f, 1.0f));
    Core::Vector3f newDir(0.0f, 0.0f, 0.0f);
    newDir = rotationMatrix.apply(_pNodeFoeShot->getWorldDirection());
    _pNodeFoeShot->setWorldDirection(newDir);

    FoeShot::update(elapsed);
}

void RotatingBullet::fade(float alpha)
{
    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);
    pNodeDecal->setColor(Core::Vector4f(alpha, alpha, alpha, alpha));
    pNodeDecal->setGlow(Core::Vector4f(1.0f, 0.0f, 1.0f, 0.5f)*alpha);
}

void RotatingBullet::setDirection(const Core::Vector3f & direction)
{
    _direction = direction;
}

float RotatingBullet::getShotDamage() const
{
    return FOE_LITTLE_DAMAGE;
}

}//namespace ChronoRage