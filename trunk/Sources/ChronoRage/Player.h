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
#ifndef CHRONORAGE_PLAYER_H
#define CHRONORAGE_PLAYER_H

#include <Core/Standard.h>
#include <Window/WinHandler.h>
#include <Universe/World.h>

#include <Controls/IPlayerCtrl.h>
#include <SoundLibrary.h>
#include <VoxelSprite.h>
#include <Level.h>
#include <RetributionLaser.h>

namespace ChronoRage
{

const Core::String PLAYER_SHIP_FILE = L"/ChronoRageData/playerShip.tga";
const float PLAYER_SPEED = 30.0f;
const float PLAYER_SPEED_MODIFIER = 1.25f;
const float PLAYER_MAXIMUM_POWER = 5.0f;
const float DEATH_TIMEOUT = 2.0f;
const float TIME_BOMB_NUMBER_MAX = 4.0f;
const float TIME_BOMB_BONUS_PER_KILL = 0.01f;
const float INVINCIBLE_TIME_FOR_DEATH = 2.0f;
const float INVINCIBLE_TIME_FOR_TIMEWARP = 5.0f;

const float POWER5_TIME = 1.0f/20.0f;//20''
const float POWER4_TIME = 1.0f/20.0f;//20''
const float POWER3_TIME = 1.0f/20.0f;//20''
const float POWER2_TIME = 1.0f/40.0f;//40''

LM_ENUM_5(EPlayerState,
          PLAYER_UNINITIALIZED,
          PLAYER_APPARITION,
          PLAYER_CREATION_IN_PROGRESS,
          PLAYER_ACTIVE,
          PLAYER_DESTRUCTION_IN_PROGRESS);

class LM_API_CHR Player : public Core::Object
{
public:
    Player( const Ptr<IPlayerCtrl> & pControl,
            const Ptr<Universe::World> & pWorld,
            const Ptr<SoundLibrary> & pSoundLibrary,
            const Ptr<Level> & pLevel,
            bool lookTarget,
            bool revengeMode);

    virtual ~Player();
    void release();

    void initialize();
    inline bool isInitialized() const { return _initialized; }

    void update(double elapsed, float deltaX, float deltaY, float timeWarpFactor, bool gameLost);

    inline Core::Vector3f getPosition() const;

    Core::Vector3f getApproachPoint(const Core::Vector3f & localApproachPoint) const;

    Core::Vector3f getDirection() const;

    void kill();
    inline bool isDead(){ return _isDead; }

    bool showCursor() const;

    void setInvincible(bool timeWarp);
    inline bool isInvincible(){ return (_isInvincible > 0.0f); }

    bool isShooting();
    bool isCannonReady(double elapsed);
    void fire();
    void stopFire();

    void resetTimeWarp();
    void resetPowerLevelAndTimeWarp();

    void powerUp();
    void powerDown();
    inline float getPowerLevel() const { return _shipPower; };

    inline bool toggleTimeWarp();
    inline float getTimeWarpNumber() const { return _timeWarpNumber; }
    void launchTimeWarp();
    void foeKilled();

    const Ptr<Core::CollisionShape> & getCollisionShape() const { return _pCollisionShape; }

    virtual void updateCollisionShape();

    inline float getWidth() const { return _width; }
    inline float getHeight() const { return _height; }
    inline float getSpeed() const { return _speed; }

    const Core::List<Ptr<IShot> > & getShots() const { return _shots; }
    void killDeadShots();
    
    Ptr<Universe::Node> getNode() const { return _pNodeShip; }

private:
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;

    void updateDirection(double elapsed);
    void updateCreation(double elapsed);
    void updateActive(double elapsed, float deltaX, float deltaY, float timeWarpFactor);
    void updateDestruction(double elapsed, bool gameLost);

    void updateShipPowerLevel(float elapsed);

    void fireLaser();

    void firePowerLevel1();
    void firePowerLevel2();
    void firePowerLevel3();
    void firePowerLevel4();
    void firePowerLevel5();

private:
    bool                        _initialized;

    Ptr<IPlayerCtrl>            _pControl;
    Ptr<Universe::World>        _pWorld;
    Ptr<SoundLibrary>           _pSoundLibrary;
    Ptr<Level>                  _pLevel;

    bool                        _lookTarget;

    bool                        _revengeMode;
    bool                        _laserFired;
    Ptr<RetributionLaser>       _pLaser;

    Core::Vector3f              _direction;

    Ptr<Universe::Node>         _pNodeShip;
    Ptr<VoxelSprite>            _pVoxelSprite;
    Ptr<Core::CollisionShape>   _pCollisionShape;
    EPlayerState                _state;

    Ptr<Universe::NodeDecal>    _pForceShield1;
    Ptr<Universe::NodeDecal>    _pForceShield2;

    float                       _width;
    float                       _height;
    float                       _speed;

    double                      _lastShot;
    float                       _shipPower;
    float                       _timeWarpNumber;

    bool                        _isDead;
    float                       _isInvincible;
    float                       _invincibleByDeath;

    Core::Vector3f              _cannon1Position;
    Core::Vector3f              _cannon2Position;
    Core::Vector3f              _laserPosition;

    Core::List<Ptr<IShot> >      _shots;
};

}//Namespace ChronoRage

#endif/*CHRONORAGE_PLAYER_H*/
