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
#include "FoeCommon.h"

#include <Core/Math/Random.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{
FoeCommon::FoeCommon(EFoeType type, const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   Foe(type, pWorld, pPlayer, gameMode),
    _minRange(0.0f),
    _maxRange(50.0f),
    _approachPoint(0.0f, 0.0f, 0.0f),
    _speed(15.0f),
    _radialSpeed(10.0f),
    _tolerance(5.0f),
    _angleWithPlayer(0.0f)
{}

FoeCommon::~FoeCommon()
{}


void FoeCommon::initialize(const Core::Vector3f & position)
{
    Foe::initialize(position);

    _approachPoint.x = Core::frand(_minRange, _maxRange);
    _approachPoint.y = 0.0f;
    _approachPoint.z = 0.0f;
    float angle = Core::frand(0.0f, f_PI_MUL_2);
    Core::Matrix4f rotationMatrix(Core::ROTATION, angle, Core::Vector3f(0.0f, 1.0f, 0.0f));
    _approachPoint = rotationMatrix.apply(_approachPoint);
}

void FoeCommon::updateActive(double elapsed)
{
    if(!_pPlayer->isDead())
    {
        Core::Vector3f playerPosition = _pPlayer->getPosition();
        
        Core::Vector3f target = _pPlayer->getApproachPoint(_approachPoint);
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
    }

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = currentPosition + (direction * _speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    Foe::updateActive(elapsed);
}

}//namespace ChronoRage