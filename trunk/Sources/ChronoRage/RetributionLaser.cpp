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
#include "RetributionLaser.h"

namespace ChronoRage
{

const Core::String RETRIBUTIONLASER_FILE = L"/ChronoRageData/retributionLaser.tex";
const Core::String RETRIBUTIONLASER_FILE_REFRAC = L"/ChronoRageData/retributionLaser-refrac.tex";

const float DAMAGE_PER_SECOND = 500.0f;

RetributionLaser::RetributionLaser(const Ptr<Universe::World> & pWorld, const Ptr<SoundLibrary> & pSoundLibrary)
:   _pWorld(pWorld),
    _pSoundLibrary(pSoundLibrary),
    _pNodeLaser(null),
    _pCollisionShape(null),
    _pSoundloadingLaser(null),
    _pSoundLaser(null),
    _width(2.0f),
    _height(150.0f),
    _hasHit(false),
    _totalElapsed(0.0f)
{}

RetributionLaser::~RetributionLaser()
{
    if(_pNodeLaser != null)
    {
        _pNodeLaser->kill();
        _pNodeLaser = null;
    }
}

void RetributionLaser::initialize(  const Core::Vector3f & position, 
                                    const Core::Vector3f & direction)
{
    _hasHit = false;
    _pNodeLaser = _pWorld->createDecal(RETRIBUTIONLASER_FILE, Renderer::DECAL_ADD);

    _pNodeRefrac = _pWorld->createDecal(RETRIBUTIONLASER_FILE_REFRAC, Renderer::DECAL_REFRAC);
    _pNodeRefrac->setSize(_width*1.5f, _height*1.5f);
    _pNodeRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f));
    _pNodeRefrac->setLocalPosition(Core::Vector3f(0.0f, 5.0f, 0.0f));
    
    _pNodeRefrac->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeRefrac->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pNodeLaser->addChild(_pNodeRefrac);

    _pNodeLaser->setSize(_width, _height);
    _pNodeLaser->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeLaser->setWorldDirection(direction);
    _pNodeLaser->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    _pNodeLaser->setWorldPosition(position + Core::Vector3f(0.0f, 0.0f, _height/2));

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());

    for(float ii=0.0f; ii < _height; ii += _width)
    {
        _pCollisionShape->addSphere(Core::Sphere(Core::Vector3f(0.0f, 0.0f, ii-(_height/2)), 0.8f));
    }

    _pSoundloadingLaser = _pSoundLibrary->playSound(SOUND_PLAYER_SHOT_LASER_LOADING, false);
    _pSoundLaser        = _pSoundLibrary->playSound(SOUND_PLAYER_SHOT_LASER, true);
}

Core::Vector3f RetributionLaser::getPosition() const
{
    LM_ASSERT(_pNodeLaser != null);

    return _pNodeLaser->getWorldPosition();
}

Core::Vector3f RetributionLaser::getDirection() const
{
    LM_ASSERT(_pNodeLaser != null);
    return _pNodeLaser->getWorldDirection();
}

void RetributionLaser::setDirectionAndPosition( const Core::Vector3f & direction, 
                                                const Core::Vector3f & position)
{
    _pNodeLaser->setWorldDirection(direction);
    _pNodeLaser->setWorldPosition(position + (direction * _height/2));
}

void RetributionLaser::madeHit()
{

}

float RetributionLaser::getShotDamage() const
{
    return _updateDamage;
}

void RetributionLaser::kill(bool outOfBoundaries)
{
    if(_pSoundloadingLaser != null)
    {
        _pSoundloadingLaser->kill();
        _pSoundloadingLaser = null;
    }

    if(_pSoundLaser != null)
    {
        _pSoundLaser->kill();
        _pSoundLaser = null;
    }
}

void RetributionLaser::update(double elapsed)
{
    _updateDamage = DAMAGE_PER_SECOND * float(elapsed);

    _totalElapsed += float(elapsed);
    float radiateFactor = 8.0f;

    float alpha = Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor));

    Core::Vector4f color = _pNodeLaser->getColor();
    color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.6f), alpha);
    _pNodeLaser->setColor(color);
    Core::Vector4f glow = _pNodeLaser->getGlow();
    glow.setLerp(Core::Vector4f(0.75f, 0.0f, 0.95f, 1.0f), Core::Vector4f(0.75f, 0.0f, 0.95f, 0.3f), alpha);
    _pNodeLaser->setGlow(glow);
    
    _pNodeRefrac->setSize(_width* (2.0f + Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor))), _height);
    Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
    colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f), alpha);
    _pNodeRefrac->setColor(colorRefrac);
}

void RetributionLaser::updateCollisionShape()
{
    _pCollisionShape->updateMatrices(_pNodeLaser->getLocalToWorldMatrix(), _pNodeLaser->getWorldToLocalMatrix());
}

}//namespace ChronoRage