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
#ifndef CHRONORAGE_POWERUP_H_
#define CHRONORAGE_POWERUP_H_

#include <Universe/World.h>

#include <Player.h>

namespace ChronoRage
{

class GameMode;

const Core::String POWERUP_FILE = L"/ChronoRageData/voxels-power-up.tga";
const float MAX_LIFETIME = 10.0f;

LM_ENUM_5(EPowerUPState,
          POWER_UP_UNINITIALIZED,
          POWER_UP_CREATION_IN_PROGRESS,
          POWER_UP_ACTIVE,
          POWER_UP_DESTRUCTION_IN_PROGRESS,
          POWER_UP_DELETE_PENDING);

class LM_API_CHR PowerUp : public Core::Object
{
public:
    PowerUp(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);
    virtual ~PowerUp();

    virtual void initialize(const Core::Vector3f & position);

    virtual Core::Vector3f getPosition() const;

    virtual void update(double elapsed);

    virtual void updateCollisionShape();

    virtual bool collidesPlayer();

    virtual void selfDestruct();
    virtual bool isKilled();

    bool isCollisionActive() const;

protected:
    virtual void updateCreation(double elapsed);
    virtual void updateActive(double elapsed);
    virtual void updateDestruction(double elapsed);

    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;
    static Ptr<VoxelSpriteTemplate> spTemplate;

protected:
    Ptr<Universe::World>        _pWorld;
    Ptr<Player>                 _pPlayer;
    GameMode &                  _gameMode;

    Ptr<Universe::Node>         _pNodePowerUp;
    Ptr<VoxelSprite>            _pVoxelSprite;
    Ptr<Core::CollisionShape>   _pCollisionShape;
    EPowerUPState               _state;

    Ptr<Universe::NodeDecal>    _pShockWaveRefrac;
    float                       _shockWaveRefracTimer;
    bool                        _createShockWave;

    Core::Vector3f              _direction;
    Core::Vector4f              _color;

    float   _width;
    float   _height;
    float   _speed;
    
};

}//namespace ChronoRage


#endif/*CHRONORAGE_POWERUP_H_*/