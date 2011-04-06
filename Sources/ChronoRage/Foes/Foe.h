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
#ifndef CHRONORAGE_FOE_H_
#define CHRONORAGE_FOE_H_

#include <Core/RT/Task.h>
#include <Core/Math/Collisions.h>
#include <Universe/World.h>

#include <VoxelSprite.h>
#include <Player.h>

namespace ChronoRage
{

class GameMode; 

LM_ENUM_17(EFoeType,GRUNT,
                    KAMIKAZE,
                    TELEPORTER,
                    QUICK_TELEPORTER,
                    WASP,
                    CHARGER,
                    BUCKLER,
                    BONUS_CARRIER,
                    BONUS_CARRIER_W_SHIELD,
                    MINELAYER,
                    CRUISER,
                    TURRET,
                    LASER_TURRET,
                    BOSS,
                    TEXT,
                    UBER_BOSS,
                    UBER_BOSS_PART);

int32 powerLevel(EFoeType foeType);

LM_ENUM_7(EFoeState,
          FOE_UNINITIALIZED,
          FOE_CREATION_IN_PROGRESS,
          FOE_ACTIVE,
          FOE_CUSTOM_BEHAVIOR,
          FOE_CUSTOM_BEHAVIOR2,
          FOE_DESTRUCTION_IN_PROGRESS,
          FOE_DELETE_PENDING);

const Core::String SHOCKWAVE_REFRAC_FILE = L"/ChronoRageData/Explosions/shockwave-refrac.tex";
const float SHOCKWAVE_REFRAC_TIME = 0.4f;
const float SHOCKWAVE_REFRAC_SIZE = 20.0f;

class LM_API_CHR Foe : public Core::Object, public Core::Task
{
public:
    Foe(EFoeType type, const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);

    virtual ~Foe();

    virtual EFoeType getType() const { return _type; }

    virtual int32 foeCount() const;
    virtual bool displayWarning() const;

    virtual void initialize(const Core::Vector3f & position);

    virtual Core::Vector3f getPosition() const;
    virtual void setPosition(const Core::Vector3f & position);

    virtual void update(double elapsed);

    virtual void updateCollisionShape();
    virtual void testForCollisions();

    virtual bool collidesPlayer();
    virtual bool collides(const Core::CollisionShape & shape) const;

    virtual void damage(float d, const Core::Vector3f & direction, bool playerDestruction);
    virtual bool damageOnPlayerDeath() const { return true; }
    virtual void selfDestroy();

    virtual void playDestructionSound();

    virtual void startParallelUpdate();
    virtual void endParallelUpdate();
    virtual void process();

    EFoeState getState() { return _state; }

    bool isAlive() const;

    float getHealthRatio() const { return _healthPoint / _maxHealthPoint; }

protected:
    virtual void updateCreation(double elapsed);
    virtual void updateActive(double elapsed);
    virtual void updateCustomBehavior(double elapsed);
    virtual void updateCustomBehavior2(double elapsed);
    virtual void updateDestruction(double elapsed);

    virtual bool isCollisionActive() const;
    virtual void onDie();

    virtual Ptr<VoxelSpriteTemplate> getTemplate() const = 0;

protected:
    EFoeType                    _type;
    Ptr<Universe::World>        _pWorld;
    Ptr<Player>                 _pPlayer;
    GameMode &                  _gameMode;

    double                      _elapsed;
    EFoeState                   _state;
    Ptr<Universe::Node>         _pNodeFoe;
    Ptr<VoxelSprite>            _pVoxelSprite;
    Ptr<Core::CollisionShape>   _pCollisionShape;
    float                       _maxHealthPoint;
    float                       _healthPoint;
    Ptr<Universe::NodeDecal>    _pShockWaveRefrac;
    float                       _shockWaveRefracTimer;
    bool                        _createShockWave;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_FOE_H_*/