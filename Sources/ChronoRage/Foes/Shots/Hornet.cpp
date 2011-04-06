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
#include "Hornet.h"

#include <Modes/GameMode.h>

namespace ChronoRage
{

const float ACTIVATE_COLLISION_TIMER = 0.25f;

Hornet::Hornet(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeShot(pWorld, pPlayer, gameMode),
    _headWidth(0.7f),
    _tailWidth(0.1f),
    _radialSpeed(4.0f),
    _homingTime(HORNET_TIME),
    _totalElapsed(0.0f)
{}

void Hornet::initialize(const Core::Vector3f & position)
{
    _direction = _pPlayer->getPosition() - position;
    if(_direction.isZero())
        _direction = Core::Vector3f(0.0f, 1.0f, 0.0f);
    else
        _direction.normalizeFast();

    Ptr<Universe::NodeTrail> pTrail = _pWorld->createTrail(HORNET_FILE, Renderer::FREE_FORM_ADD);
    pTrail->setHeadWidth(_headWidth);
    pTrail->setTailWidth(_tailWidth);

    pTrail->setHeadColor(Core::Vector4f(0.8f, 1.0f, 1.0f, 1.0f));
    pTrail->setTailColor(Core::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));

    pTrail->setHeadGlow(Core::Vector4f(0.8f, 1.0f, 1.0f, 1.0f));
    pTrail->setTailGlow(Core::Vector4f(0.0f, 0.0f, 1.0f, 0.0f));

    _pNodeFoeShot = pTrail;
    _pNodeFoeShot->setWorldDirection(_direction);
    _pNodeFoeShot->setWorldPosition(position);
    _pNodeFoeShot->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, -_headWidth), 2*_headWidth/3)));
    _collisionsActivated = false;
}

void Hornet::update(double elapsed)
{
    elapsed *= _gameMode.getTimeWarp()->getTimeWarpFactor();
    _totalElapsed += float(elapsed);

    if(_totalElapsed > ACTIVATE_COLLISION_TIMER)
        _collisionsActivated = true;

    Core::Vector3f currentPosition = getPosition();

    _homingTime = std::max(0.0f, _homingTime - (float)elapsed);

    if(_homingTime > 0.0f)
    {
        Core::Vector3f approachPoint = _pPlayer->getApproachPoint(Core::Vector3f(0.0f, 0.0f, 0.0f));
        Core::Vector3f distance = approachPoint - currentPosition;
        
        if(!distance.isZero())
        {
            Core::Vector3f localTarget(_pNodeFoeShot->getParentWorldToLocalMatrix().apply(approachPoint));
            Core::Vector3f localDir(localTarget - _pNodeFoeShot->getLocalPosition());

            Core::Matrix3f mat;
            mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoeShot->getUpVector(), _pNodeFoeShot->getLocalMatrix().getYVector());

            Core::Quaternionf q1(_pNodeFoeShot->getLocalOrient());
            Core::Quaternionf q2(mat);

            double rotTime = elapsed;
            float angle = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
            float maxAngle = float(_radialSpeed * rotTime);
            float alpha = 1.0;

            if(angle != 0.0)
                alpha = std::min(1.0f, maxAngle / angle);

            Core::Quaternionf newOrient(q1, q2, alpha);
            newOrient.setNlerp(q1, q2, alpha);

            _pNodeFoeShot->setLocalOrientation(newOrient);
            Core::Vector3f direction = _pNodeFoeShot->getWorldDirection();
            _direction = direction;
            _pNodeFoeShot->setWorldDirection(_direction);
        }
    }

    Core::Vector3f newPosition = currentPosition + (_direction * HORNET_SPEED * (float)elapsed);
    _pNodeFoeShot->setWorldPosition(newPosition);

    FoeShot::update(elapsed);
}

void Hornet::fade(float alpha)
{
    Ptr<Universe::NodeTrail> pTrail = LM_DEBUG_PTR_CAST<Universe::NodeTrail>(_pNodeFoeShot);
    pTrail->setHeadColor(Core::Vector4f(0.8f, 1.0f, 1.0f, 1.0f)*alpha);
    pTrail->setTailColor(Core::Vector4f(0.0f, 0.0f, 1.0f, 0.0f)*alpha);

    pTrail->setHeadGlow(Core::Vector4f(0.8f, 1.0f, 1.0f, 1.0f)*alpha);
    pTrail->setTailGlow(Core::Vector4f(0.0f, 0.0f, 1.0f, 0.0f)*alpha);
}

}//namespace ChronoRage