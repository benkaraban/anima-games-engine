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
#include "Charger.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Charger::spTemplate;

Charger::Charger(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(CHARGER, pWorld, pPlayer, gameMode)
{
    _minRange = 5.0f;
    _maxRange = 20.0f;
    _speed = 10.0f;
    _radialSpeed = 4.0f;
    _maxHealthPoint = _healthPoint = 32.0f;
    _tolerance = 0.0f;
}

Charger::~Charger()
{}

void Charger::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.3333f)));

    _approachPointSave = _approachPoint;
}

bool Charger::displayWarning() const
{
    return (_state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2);
}

Ptr<VoxelSpriteTemplate> Charger::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(CHARGER_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

bool Charger::isCollisionActive() const
{
    return _state == FOE_ACTIVE || _state == FOE_CUSTOM_BEHAVIOR || _state == FOE_CUSTOM_BEHAVIOR2;
}

void Charger::updateActive(double elapsed)
{
    if(!_gameMode.isOutOfBoundaries(getPosition(), -10) && _angleWithPlayer <= 0.001f)
    {
        _state = FOE_CUSTOM_BEHAVIOR;
        _accelerationTime = 0.0f;
        _chargeTime = 0.0f;
        _startTime = 0.0f;

        _pVoxelSprite->blink(CHARGER_START_TIME, CHARGER_BLINK_NUMBER);
    }

    FoeCommon::updateActive(elapsed);
}

void Charger::updateCustomBehavior(double elapsed)
{
    _startTime = std::min(CHARGER_START_TIME, _startTime + (float)elapsed);
    _speed = Core::lerp(10.0f, 0.0f, _startTime/CHARGER_START_TIME);

    _approachPoint.setLerp(_approachPointSave, Core::Vector3f(0.0f, 0.0f, 0.0f), _startTime/CHARGER_START_TIME);
    
    if(_startTime >= CHARGER_START_TIME)
    {
        _state = FOE_CUSTOM_BEHAVIOR2;
        _radialSpeed = 0.0f;
    }

    FoeCommon::updateActive(elapsed);
}

void Charger::updateCustomBehavior2(double elapsed)
{
    _chargeTime = std::min(CHARGER_CHARGE_TIME, _chargeTime + (float)elapsed);
    _accelerationTime = std::min(CHARGER_ACCELERATION_TIME, _accelerationTime + (float)elapsed);

    if(_chargeTime <= CHARGER_ACCELERATION_TIME)
    {
        _speed = Core::lerp(0.0f, CHARGER_MAX_SPEED, _accelerationTime/CHARGER_ACCELERATION_TIME);
    }
    else if(_chargeTime >= CHARGER_CHARGE_TIME - CHARGER_ACCELERATION_TIME && _chargeTime < CHARGER_CHARGE_TIME)
    {
        _speed = Core::lerp(CHARGER_MAX_SPEED, 10.0f, (_chargeTime - CHARGER_ACCELERATION_TIME)/(CHARGER_CHARGE_TIME - CHARGER_ACCELERATION_TIME));
    }
    else if(_chargeTime >= CHARGER_CHARGE_TIME)
    {
        _radialSpeed = 4.0f;
        _approachPoint = _approachPointSave;
        _state = FOE_ACTIVE;
    }

    FoeCommon::updateActive(elapsed);
}


}//namespace ChronoRage