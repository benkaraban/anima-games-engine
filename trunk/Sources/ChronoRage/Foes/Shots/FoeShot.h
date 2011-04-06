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
#ifndef CHRONORAGE_FOESHOT_H_
#define CHRONORAGE_FOESHOT_H_

#include <Core/Math/Collisions.h>
#include <Universe/World.h>

#include <Player.h>

namespace ChronoRage
{

class GameMode;

const float FOE_LITTLE_DAMAGE   = 10.0f;
const float FOE_MEDIUM_DAMAGE   = 25.0f;
const float FOE_HUGE_DAMAGE     = 100.0f;

const float FADE_TIMER = 0.25f;

class LM_API_CHR FoeShot : public IShot
{
public:
    FoeShot(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);

    virtual ~FoeShot();

    virtual void initialize(const Core::Vector3f & position) = 0;

    virtual Core::Vector3f getPosition() const;
    virtual Core::Vector3f getDirection() const;
    virtual void setDirection(const Core::Vector3f & direction);

    virtual void update(double elapsed);

    virtual void fade(float alpha) = 0;

    virtual void updateCollisionShape();
    virtual bool collidesPlayer();

    virtual bool isDead();
    virtual bool isDying();
    virtual void kill(bool outOfBoundaries);

    virtual Ptr<Universe::Node> getNode() { return _pNodeFoeShot; }

    virtual void madeHit();
    virtual bool hasHit() const;

    virtual float getShotDamage() const;
    
    virtual Ptr<Core::CollisionShape> getCollisionShape() const;

protected:
    Ptr<Universe::World>        _pWorld;
    Ptr<Player>                 _pPlayer;
    GameMode &                  _gameMode;
    Ptr<Universe::Node>         _pNodeFoeShot;
    Ptr<Core::CollisionShape>   _pCollisionShape;

    bool                        _hasHit;
    bool                        _collisionsActivated;
    float                       _fadeTimer;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_FOESHOT_H_*/