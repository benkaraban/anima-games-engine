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
#include "Shot.h"

namespace ChronoRage
{

const float APPARITION_TIMER = 0.2f;
const float ALPHA_MAX = 0.2f;

Shot::Shot(const Ptr<Universe::World> & pWorld)
:   _pWorld(pWorld),
    _pNodeShot(null),
    _pCollisionShape(null),
    _hasHit(false)
{}

Shot::~Shot()
{}

void Shot::initialize(  const Core::Vector3f & position, 
                        const Core::Vector3f & shotDirection, 
                        float shotSpeed,
                        float width,
                        float height,
                        int32 powerLevel)
{
    _totalElapsed = 0.0f;

    _hasHit = false;
    _pNodeShot = _pWorld->createDecal(L"/ChronoRageData/shot-blue.tex", Renderer::DECAL_ADD);
    _powerLevel = powerLevel;
    _shotSpeed = shotSpeed;

    _pNodeRefrac = _pWorld->createDecal(SHOT_FILE_REFRAC, Renderer::DECAL_REFRAC);
    _pNodeRefrac->setSize(width*1.5f, height*1.5f);
    _pNodeRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f));
    _pNodeRefrac->setLocalPosition(Core::Vector3f(0.0f, 5.0f, 0.0f));
    
    _pNodeRefrac->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeRefrac->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pNodeShot->addChild(_pNodeRefrac);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), width/3)));

    width *= 3.5f;
    height *= 2.25f;

    if(powerLevel == 1)
    {
        //Shoot bleu
        _pNodeShot->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        _pNodeShot->setTexture(L"/ChronoRageData/shot-blue.tex");

    }
    else if(powerLevel == 2)
    {
        //Shoot jaune
        _pNodeShot->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        _pNodeShot->setTexture(L"/ChronoRageData/shot-yellow.tex");
    }
    else
    {
        //Shoot violet
        _pNodeShot->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
        _pNodeShot->setTexture(L"/ChronoRageData/shot-purple.tex");
    }
    
    _pNodeShot->setSize(width, height);
    _pNodeShot->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeShot->setWorldDirection(shotDirection);
    _pNodeShot->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    _pNodeShot->setWorldPosition(position);
}

Core::Vector3f Shot::getPosition() const
{
    LM_ASSERT(_pNodeShot != null);

    return _pNodeShot->getWorldPosition();
}

Core::Vector3f Shot::getDirection() const
{
    LM_ASSERT(_pNodeShot != null);
    return _pNodeShot->getWorldDirection();
}

float Shot::getShotDamage() const
{
    return (float)_powerLevel;
}

void Shot::kill(bool outOfBoundaries)
{
    if(!outOfBoundaries)
    {
        Ptr<Universe::Node> pExplosionInstance = _pWorld->createInstance(SHOT_EXPLOSION_FILE);
        pExplosionInstance->setWorldPosition(_pNodeShot->getWorldPosition());
        Ptr<Universe::NodeGroup> pGroup = LM_DEBUG_PTR_CAST<Universe::NodeGroup>(pExplosionInstance);
        pGroup->killWhenChildrenAreDead(true);
        Ptr<Universe::NodeEmitter> pEmitter = LM_DEBUG_PTR_CAST<Universe::NodeEmitter>(pGroup->getChild(0));
        pEmitter->killAtEnd(true);
        pEmitter->restart();
    }
    
    _pNodeShot->kill();
}

void Shot::update(double elapsed)
{
    _totalElapsed += (float)elapsed;
    _pNodeShot->setWorldPosition(getPosition() + (getDirection() * _shotSpeed * (float)elapsed));

    const float APPARITION_TIMER = 0.2f;
    const float ALPHA_MAX = 0.2f;

    float alpha = std::min(ALPHA_MAX, ALPHA_MAX * _totalElapsed / APPARITION_TIMER);

    _pNodeRefrac->setColor(Core::Vector4f(1.0f, 1.0f, 1.0f,alpha));
}

void Shot::updateCollisionShape()
{
    _pCollisionShape->updateMatrices(_pNodeShot->getLocalToWorldMatrix(), _pNodeShot->getWorldToLocalMatrix());
}

}//namespace ChronoRage