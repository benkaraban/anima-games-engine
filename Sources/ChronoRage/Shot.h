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
#ifndef CHRONORAGE_SHOT_H_
#define CHRONORAGE_SHOT_H_

#include <Core/Standard.h>
#include <Core/Math/Collisions.h>
#include <Universe/World.h>

#include <IShot.h>

namespace ChronoRage
{

const Core::String SHOT_FILE_REFRAC = L"/ChronoRageData/shot-refrac.tex";
const Core::String SHOT_EXPLOSION_FILE = L"/ChronoRageData/Explosions/shot-explosion.group";

class LM_API_CHR Shot : public IShot, public Core::Object
{
public:
    Shot(const Ptr<Universe::World> & pWorld);
    virtual ~Shot();

    void initialize(const Core::Vector3f & position, 
                    const Core::Vector3f & shotDirection, 
                    float shotSpeed,
                    float width,
                    float height,
                    int32 powerLevel);

    virtual Core::Vector3f getPosition() const;
    virtual Core::Vector3f getDirection() const;

    Ptr<Universe::Node> getNode() const { return _pNodeShot; }
    Ptr<Core::CollisionShape> getCollisionShape() const { return _pCollisionShape; }

    virtual void madeHit(){ _hasHit = true; }
    virtual bool hasHit() const { return _hasHit; }
    virtual float getShotDamage() const;

    void kill(bool outOfBoundaries);

    virtual void update(double elapsed);
    virtual void updateCollisionShape();

private:
    Ptr<Universe::World>        _pWorld;
    Ptr<Universe::NodeDecal>    _pNodeShot;
    Ptr<Universe::NodeDecal>    _pNodeRefrac;
    Ptr<Core::CollisionShape>   _pCollisionShape;

    float                       _shotSpeed;
    bool                        _hasHit;
    int32                       _powerLevel;
    float                       _totalElapsed;
};

}//namespace ChronoRage


#endif/*CHRONORAGE_SHOT_H_*/