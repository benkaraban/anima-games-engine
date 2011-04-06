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
#include "Player.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

#include <Shot.h>

namespace ChronoRage
{

Ptr<VoxelSpriteTemplate> Player::spTemplate;

const String PLAYER_FORCE_FIELD1 = L"/ChronoRageData/force-shield1.tex";
const String PLAYER_FORCE_FIELD2 = L"/ChronoRageData/force-shield2.tex";
const float PLAYER_FORCE_FIELD_RPS = 0.25f;
const String PLAYER_FORCE_FIELD_REFRAC = L"/ChronoRageData/force-shield.group";

Player::Player( const Ptr<IPlayerCtrl> & pControl,
                const Ptr<Universe::World> & pWorld,
                const Ptr<SoundLibrary> & pSoundLibrary,
                const Ptr<Level> & pLevel,
                bool lookTarget,
                bool revengeMode)
:   _initialized(false),
    _pControl(pControl),
    _pWorld(pWorld),
    _pSoundLibrary(pSoundLibrary),
    _pLevel(pLevel),
    _lookTarget(lookTarget),
    _revengeMode(revengeMode),
    _laserFired(false),
    _pLaser(null),
    _pNodeShip(null),
    _pCollisionShape(null),
    _state(PLAYER_UNINITIALIZED),
    _pForceShield1(null),
    _pForceShield2(null),
    _width(2.0f),
    _height(2.0f),
    _speed(PLAYER_SPEED),
    _lastShot(0.0),
    _shipPower(1.0f),
    _timeWarpNumber(3.0f),
    _isDead(true),
    _isInvincible(-1.0f),
    _invincibleByDeath(-1.0f),
    _cannon1Position(1.0f, 0.0f, 1.5f),
    _cannon2Position(-1.0f, 0.0f, 1.5f),
    _laserPosition(-0.12f, 0.0f, 0.0f)
{
}

Player::~Player()
{
    release();
}

void Player::release()
{
    if(_pNodeShip != null)
    {
        _pNodeShip->kill();
        _pNodeShip = null;
        
    }
    if(_pForceShield1 != null)
    {
        _pForceShield1->kill();
        _pForceShield1 = null;
    }
    if(_pForceShield2 != null)
    {
        _pForceShield2->kill();
        _pForceShield2 = null;
    }
    if(_pVoxelSprite != null)
    {
        _pVoxelSprite->kill();
        _pVoxelSprite = null;
    }
}

void Player::initialize()
{
    _pNodeShip = _pWorld->createNode();
    _pNodeShip->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));

    _pNodeShip->setWorldPosition(Core::Vector3f(0.0f, 0.0f, _pLevel->playgroundDepth()));
    _pNodeShip->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));

    _pForceShield1 = _pWorld->createDecal(PLAYER_FORCE_FIELD1, Renderer::DECAL_ADD);
    _pForceShield1->setSize(10.0f, 10.0f);
    _pForceShield1->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pForceShield1->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
    _pForceShield1->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    _pForceShield1->setWorldPosition(_pNodeShip->getWorldPosition());
    _pForceShield1->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pForceShield1->setGlow(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

    _pForceShield2 = _pWorld->createDecal(PLAYER_FORCE_FIELD2, Renderer::DECAL_ADD);
    _pForceShield2->setSize(10.0f, 10.0f);
    _pForceShield2->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pForceShield2->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
    _pForceShield2->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    _pForceShield2->setWorldPosition(_pNodeShip->getWorldPosition());
    _pForceShield2->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pForceShield2->setGlow(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), _width/2)));

    _pVoxelSprite = Ptr<VoxelSprite>(new VoxelSprite(_pWorld, getTemplate()));
    _pVoxelSprite->setMatrices(_pNodeShip->getLocalToWorldMatrix(), _pNodeShip->getWorldToLocalMatrix());

    _state = PLAYER_APPARITION;

    Core::Vector3f cloudPosition;
    cloudPosition.x = Core::frand(-15, 15);
    cloudPosition.y = Core::frand(-15, 15);
    cloudPosition.z = -50;

    cloudPosition.x = 0.0f;
    cloudPosition.y = 0.0f;
    cloudPosition.z = -70.0f;

    VoxelCloud cloud;
    cloud.position      = _pNodeShip->getWorldToLocalMatrix().apply(cloudPosition);
    cloud.cosAlpha      = Core::L_cos(f_PI);
    cloud.alpha         = f_PI;
    cloud.beta          = f_PI;
    cloud.dispRadius    = 20.0f;
    _pVoxelSprite->cloudToSprite(cloud, 1.0f, 1.5f);
    _pVoxelSprite->update(0.0);

    _initialized = true;
}

void Player::update(double elapsed, float deltaX, float deltaY, float timeWarpFactor, bool gameLost)
{
    switch(_state)
    {
    case PLAYER_UNINITIALIZED:
        LM_ASSERT(false);
        throw Core::Exception(L"Unitialized Player");
        break;
    case PLAYER_APPARITION:
    case PLAYER_CREATION_IN_PROGRESS:
        updateCreation(elapsed);
        break;
    case PLAYER_ACTIVE:
        if(!gameLost || isDead())
        {
            _pControl->update(elapsed);
            updateActive(elapsed, deltaX, deltaY, timeWarpFactor);
        }
        break;
    case PLAYER_DESTRUCTION_IN_PROGRESS:
        updateDestruction(elapsed, gameLost);
        break;
    }

    Core::List<Ptr<IShot> >::iterator iShot = _shots.begin();
    while(iShot != _shots.end())
    {
        (*iShot)->update(elapsed);
                
        ++iShot;
    }

    _pVoxelSprite->setMatrices(_pNodeShip->getLocalToWorldMatrix(), _pNodeShip->getWorldToLocalMatrix());
    _pVoxelSprite->update(elapsed);
}

void Player::updateCollisionShape()
{
    _pCollisionShape->updateMatrices(_pNodeShip->getLocalToWorldMatrix(), _pNodeShip->getWorldToLocalMatrix());

    for(int32 ii=0; ii < _shots.size(); ii++)
        _shots[ii]->updateCollisionShape();
}

Ptr<VoxelSpriteTemplate> Player::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(PLAYER_SHIP_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Player::updateCreation(double elapsed)
{
    _pVoxelSprite->setMatrices(_pNodeShip->getLocalToWorldMatrix(), _pNodeShip->getWorldToLocalMatrix());

    if(_pVoxelSprite->isAnimationFinished())
    {
        if(_state == PLAYER_CREATION_IN_PROGRESS)
        {
            setInvincible(false);
        }
        else if(_state == PLAYER_APPARITION)
        {
            _pControl->setShootDir(Core::Vector3f(0.0f, 1.0f, 0.0f));
        }

        _state = PLAYER_ACTIVE;
        _isDead = false;
    }
}

void Player::updateDirection(double elapsed)
{
    if(_lookTarget)
    {
        _pNodeShip->setWorldDirection(_pControl->getShootDir());
    }
    else
    {
        Core::Vector3f keyDir = _pControl->getMoveDir();
        if(!keyDir.isZero())
        {
            Core::Vector3f shipDir = _pNodeShip->getWorldDirection();
            keyDir.normalizeSafe();

            float angle = Core::L_acosSafe(Core::dot(shipDir, keyDir));
            angle = std::min(angle, float(elapsed) * 10.0f);

            Core::Vector3f zVec(Core::cross(shipDir, keyDir));

            if(zVec.isZero())
                zVec = Core::Vector3f(0.0f, 0.0f, 1.0f);
            
            if(!Core::isEquivalent(angle, 0.0f))
            {
                zVec.normalizeSafe();
                Core::Matrix4f matR(Core::ROTATION, angle, zVec);
                shipDir = matR.apply3x3(shipDir);
                shipDir.normalizeSafe();

                _pNodeShip->setWorldDirection(shipDir);
            }
        }
    }
}

void Player::updateActive(double elapsed, float deltaX, float deltaY, float timeWarpFactor)
{
    float speed = getSpeed();
    if(_revengeMode)
    {
        if(isShooting())
        {
            speed = 5.0f;
            _pControl->setRadialSpeed(1.0f);
        }
        else
        {
            _pControl->setDefaultRadialSpeed();
        }
    }
    else
    {
        if(!isShooting())
        {
            speed *= PLAYER_SPEED_MODIFIER;
        }
    }
    
    _direction = _pControl->getMoveDir() * speed;
    Core::Vector3f playerDirection = _direction * (float)elapsed;
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f newPosition = currentPosition + playerDirection;

    if(newPosition.x + deltaX > _pLevel->xMax())
        newPosition.x = _pLevel->xMax() - deltaX;
    if(newPosition.x - deltaX < _pLevel->xMin())
        newPosition.x = _pLevel->xMin() + deltaX;

    if(newPosition.y + deltaY > _pLevel->yMax())
        newPosition.y = _pLevel->yMax() - deltaY;
    if(newPosition.y - deltaY < _pLevel->yMin())
        newPosition.y = _pLevel->yMin() + deltaY;

    _pNodeShip->setWorldPosition(newPosition);
    _pForceShield1->setWorldPosition(newPosition);
    _pForceShield2->setWorldPosition(newPosition);

    updateDirection(elapsed);

    if(_revengeMode)
    {
        if(_laserFired)
        {
            Core::Vector3f shipDir = _pNodeShip->getWorldDirection();
            Core::Vector3f laserPos = _pNodeShip->getLocalToWorldMatrix().apply(_laserPosition);
            _pLaser->setDirectionAndPosition(shipDir, laserPos);
        }
    }
    else
    {
        updateShipPowerLevel((float) elapsed * timeWarpFactor);
    }
    
    if(isInvincible())
    {
        _isInvincible = std::max(0.0f, _isInvincible - (float)elapsed);
        _invincibleByDeath = std::max(0.0f, _invincibleByDeath - (float)elapsed);

        if(_isInvincible > 0.0f || _invincibleByDeath > 0.0f)
        {
            float angle1 = -f_PI_MUL_2 * PLAYER_FORCE_FIELD_RPS * (float)elapsed;
            Core::Matrix4f rotationMatrix1(Core::ROTATION, angle1, Core::Vector3f(0.0, 0.0, 1.0));
            Core::Vector3f direction1 = _pForceShield1->getWorldDirection();
            Core::Vector3f newDirection1 = rotationMatrix1.apply3x3(direction1);
            _pForceShield1->setWorldDirection(newDirection1);

            float angle2 = f_PI_MUL_2 * PLAYER_FORCE_FIELD_RPS * (float)elapsed;
            Core::Matrix4f rotationMatrix2(Core::ROTATION, angle2, Core::Vector3f(0.0, 0.0, 1.0));
            Core::Vector3f direction2 = _pForceShield2->getWorldDirection();
            Core::Vector3f newDirection2 = rotationMatrix2.apply3x3(direction2);
            _pForceShield2->setWorldDirection(newDirection2);

            float alpha = 1.0f;
            if(_isInvincible <= 0.5f && _invincibleByDeath <= 0.5f)
                alpha = std::max(_isInvincible, _invincibleByDeath) * 2;

            Core::Vector4f color(0.0f, 0.4f, 0.4f, alpha);
            _pForceShield1->setColor(color);
            _pForceShield2->setColor(color);
        }
        else 
        {
            _pForceShield1->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
            _pForceShield2->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        }
    }

    if(isDead())
    {
        VoxelCloud cloud1;
        cloud1.position      = Core::Vector3f(0.0f, 0.0f, 0.0f);
        cloud1.cosAlpha      = Core::L_cos(f_PI_DIV_2);
        cloud1.alpha         = f_PI;
        cloud1.beta          = f_PI;
        cloud1.dispRadius    = 16.0f;

        VoxelCloud cloud2;
        cloud2.position      = _pNodeShip->getWorldToLocalMatrix().apply(Core::Vector3f(0.0f, 0.0f, 150.0f));
        cloud2.cosAlpha      = Core::L_cos(f_PI);
        cloud2.alpha         = f_PI;
        cloud2.beta          = f_PI;
        cloud2.dispRadius    = 0.2f;

        _pVoxelSprite->spriteToCloud2(cloud1, cloud2, 0.4f, 0.5f, 1.0f, 1.5f);
        _state = PLAYER_DESTRUCTION_IN_PROGRESS;

        _pForceShield1->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        _pForceShield2->setColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    }
}

void Player::updateDestruction(double elapsed, bool gameLost)
{
    if(_pVoxelSprite->isAnimationFinished() && !gameLost)
    {
        _state = PLAYER_CREATION_IN_PROGRESS;
        
        Core::Vector3f cloudPosition;
        cloudPosition.x = 0.0f;
        cloudPosition.y = 0.0f;
        cloudPosition.z = -70.0f;

        VoxelCloud cloud;
        cloud.position      = _pNodeShip->getWorldToLocalMatrix().apply(cloudPosition);
        cloud.cosAlpha      = Core::L_cos(f_PI);
        cloud.alpha         = f_PI;
        cloud.beta          = f_PI;
        cloud.dispRadius    = 20.0f;
        _pVoxelSprite->setMatrices(_pNodeShip->getLocalToWorldMatrix(), _pNodeShip->getWorldToLocalMatrix());
        _pVoxelSprite->cloudToSprite(cloud, 1.0f, 1.5f);
        _pVoxelSprite->update(0.0);
    }
}


Core::Vector3f Player::getPosition() const
{
    return _pNodeShip->getWorldPosition();
}

Core::Vector3f Player::getApproachPoint(const Core::Vector3f & localApproachPoint) const
{
    return _pNodeShip->getLocalToWorldMatrix().apply(localApproachPoint);
}


Core::Vector3f Player::getDirection() const
{
    return _direction;
}

void Player::kill()
{
    _pControl->shakeDie();
    _isDead = true;
}

bool Player::showCursor() const
{ 
    bool showCursor = false;

    if(_lookTarget)
        showCursor = _pControl->showCursor(); 
    else
        showCursor = false;

    return showCursor;
}

void Player::setInvincible(bool timeWarp)
{
    if(timeWarp)
    {
        _isInvincible = INVINCIBLE_TIME_FOR_TIMEWARP;
    }
    else
    {
        _invincibleByDeath = INVINCIBLE_TIME_FOR_DEATH;
        _isInvincible = INVINCIBLE_TIME_FOR_DEATH;
    }
}

bool Player::isShooting()
{
    return _pControl->fire();
}

bool Player::isCannonReady(double elapsed)
{
    _lastShot -= elapsed;
    return (_lastShot <= 0);
}

void Player::fire()
{
    if(_revengeMode)
    {
        fireLaser();
    }
    else
    {
        _pSoundLibrary->playSound(SOUND_PLAYER_SHOT, false);

        if(isInvincible() && _invincibleByDeath > 0.0f)
        {
            firePowerLevel5();
        }
        else
        {
            if(_shipPower <= 1.0f)
            {
                firePowerLevel1();
            }
            else if(_shipPower <= 2.0f)
            {
                firePowerLevel2();
            }
            else if(_shipPower <= 3.0f)
            {
                firePowerLevel3();
            }
            else if(_shipPower <= 4.0f)
            {
                firePowerLevel4();
            }
            else if(_shipPower <= 5.0f)
            {
                firePowerLevel5();
            }
        }
    }
}

void Player::stopFire()
{
    if(_laserFired)
    {
        _shots.clear();
        _pLaser->kill(false);
        _pLaser = null;

        _laserFired = false;
    }
}

void Player::resetTimeWarp()
{
    _timeWarpNumber = 3.0f;
}

void Player::resetPowerLevelAndTimeWarp()
{
    _shipPower= 1.0f;
    _timeWarpNumber = 3.0f;
}

bool Player::toggleTimeWarp()
{
    return _pControl->timeWarp();
}

void Player::powerUp()
{
    _shipPower = std::min(PLAYER_MAXIMUM_POWER, (_shipPower + 1.0f));
}

void Player::powerDown()
{
    _shipPower = std::max(1.0f, (_shipPower - 1.0f));
}

void Player::killDeadShots()
{
    Core::List<Ptr<IShot> >::iterator iShot = _shots.begin();

    while(iShot != _shots.end())
    {
        bool outOfBoundaries = _pLevel->isOutOfBoundaries((*iShot)->getPosition(), 20.0f);
        if( outOfBoundaries || (*iShot)->hasHit() )
        {
            (*iShot)->kill(outOfBoundaries);
            iShot = _shots.erase(iShot);
        }
        else
            ++iShot;
    }
}

void Player::updateShipPowerLevel(float elapsed)
{
    if(_shipPower > 4.0f)
    {
        _shipPower -= (elapsed * POWER5_TIME);
    }
    else if(_shipPower > 3.0f)
    {
        _shipPower -= (elapsed * POWER4_TIME);
    }
    else if(_shipPower > 2.0f)
    {
        _shipPower -= (elapsed * POWER3_TIME);
    }
    else if(_shipPower > 1.0f)
    {
        _shipPower -= (elapsed * POWER2_TIME);
    }
    else
    {
        _shipPower = 1.0f;
    }
}

void Player::foeKilled()
{
    _timeWarpNumber = std::min(TIME_BOMB_NUMBER_MAX, _timeWarpNumber + TIME_BOMB_BONUS_PER_KILL);
}

void Player::launchTimeWarp()
{
    _timeWarpNumber -= 1.0f;
}

void Player::fireLaser()
{
    if(!_laserFired)
    {
        Core::Vector3f currentPosition = getPosition();
        Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
        
        _laserFired = true;
        
        _pLaser = Ptr<RetributionLaser>(new RetributionLaser(_pWorld, _pSoundLibrary));
        _pLaser->initialize(currentPosition, shotDirection);

        _shots.push_back(_pLaser);
    }
}

void Player::firePowerLevel1()
{
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
    
    Ptr<Shot> pShot1(new Shot(_pWorld));
    Ptr<Shot> pShot2(new Shot(_pWorld));

    Core::Matrix4f localToWorldMatrix = _pNodeShip->getLocalToWorldMatrix();
    Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_cannon1Position);
    Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_cannon2Position);

    float shotSpeed = 60;

    pShot1->initialize(shotPosition1, shotDirection, shotSpeed, 2.0f, 3.0f, 1);
    pShot2->initialize(shotPosition2, shotDirection, shotSpeed, 2.0f, 3.0f, 1);
    
    _shots.push_back(pShot1);
    _shots.push_back(pShot2);

    _lastShot = 0.125;
}

void Player::firePowerLevel2()
{
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
    
    Ptr<Shot> pShot1(new Shot(_pWorld));
    Ptr<Shot> pShot2(new Shot(_pWorld));

    Core::Matrix4f localToWorldMatrix = _pNodeShip->getLocalToWorldMatrix();
    Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_cannon1Position);
    Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_cannon2Position);

    float shotSpeed = 90;

    pShot1->initialize(shotPosition1, shotDirection, shotSpeed, 2.0f, 4.0f, 2);
    pShot2->initialize(shotPosition2, shotDirection, shotSpeed, 2.0f, 4.0f, 2);
    
    _shots.push_back(pShot1);
    _shots.push_back(pShot2);

    _lastShot = 0.125;
}

void Player::firePowerLevel3()
{
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
    
    Ptr<Shot> pShot1(new Shot(_pWorld));
    Ptr<Shot> pShot2(new Shot(_pWorld));

    Core::Matrix4f localToWorldMatrix = _pNodeShip->getLocalToWorldMatrix();
    Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_cannon1Position);
    Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_cannon2Position);

    float shotSpeed1 = 90;

    pShot1->initialize(shotPosition1, shotDirection, shotSpeed1, 2.0f, 4.0f, 2);
    pShot2->initialize(shotPosition2, shotDirection, shotSpeed1, 2.0f, 4.0f, 2);
    
    Ptr<Shot> pShot3(new Shot(_pWorld));
    Ptr<Shot> pShot4(new Shot(_pWorld));
    Core::Matrix4f rotationMatrix1(Core::ROTATION, f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));
    Core::Matrix4f rotationMatrix2(Core::ROTATION, -f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));

    Core::Vector3f shotDirection2 = rotationMatrix1.apply(shotDirection);
    Core::Vector3f shotDirection3 = rotationMatrix2.apply(shotDirection);

    float shotSpeed2 = 60;

    pShot3->initialize(shotPosition1, shotDirection2, shotSpeed2, 2.0f, 3.0f, 1);
    pShot4->initialize(shotPosition2, shotDirection3, shotSpeed2, 2.0f, 3.0f, 1);

    _shots.push_back(pShot1);
    _shots.push_back(pShot2);
    _shots.push_back(pShot3);
    _shots.push_back(pShot4);

    _lastShot = 0.125;
}

void Player::firePowerLevel4()
{
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
    
    Ptr<Shot> pShot1(new Shot(_pWorld));
    Ptr<Shot> pShot2(new Shot(_pWorld));

    Core::Matrix4f localToWorldMatrix = _pNodeShip->getLocalToWorldMatrix();
    Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_cannon1Position);
    Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_cannon2Position);

    float shotSpeed = 90;

    pShot1->initialize(shotPosition1, shotDirection, shotSpeed, 2.0f, 4.0f, 2);
    pShot2->initialize(shotPosition2, shotDirection, shotSpeed, 2.0f, 4.0f, 2);
    
    Ptr<Shot> pShot3(new Shot(_pWorld));
    Ptr<Shot> pShot4(new Shot(_pWorld));
    Core::Matrix4f rotationMatrix1(Core::ROTATION, f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));
    Core::Matrix4f rotationMatrix2(Core::ROTATION, -f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));

    Core::Vector3f shotDirection2 = rotationMatrix1.apply(shotDirection);
    Core::Vector3f shotDirection3 = rotationMatrix2.apply(shotDirection);

    pShot3->initialize(shotPosition1, shotDirection2, shotSpeed, 2.0f, 4.0f, 2);
    pShot4->initialize(shotPosition2, shotDirection3, shotSpeed, 2.0f, 4.0f, 2);

    _shots.push_back(pShot1);
    _shots.push_back(pShot2);
    _shots.push_back(pShot3);
    _shots.push_back(pShot4);

    _lastShot = 0.125;
}

void Player::firePowerLevel5()
{
    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f shotDirection = _pNodeShip->getWorldDirection();
    
    Ptr<Shot> pShot1(new Shot(_pWorld));
    Ptr<Shot> pShot2(new Shot(_pWorld));

    Core::Matrix4f localToWorldMatrix = _pNodeShip->getLocalToWorldMatrix();
    Core::Vector3f shotPosition1 = localToWorldMatrix.apply(_cannon1Position);
    Core::Vector3f shotPosition2 = localToWorldMatrix.apply(_cannon2Position);

    float shotSpeed = 90;

    pShot1->initialize(shotPosition1, shotDirection, shotSpeed, 2.0f, 4.0f, 3);
    pShot2->initialize(shotPosition2, shotDirection, shotSpeed, 2.0f, 4.0f, 3);
    
    Ptr<Shot> pShot3(new Shot(_pWorld));
    Ptr<Shot> pShot4(new Shot(_pWorld));
    Core::Matrix4f rotationMatrix1(Core::ROTATION, f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));
    Core::Matrix4f rotationMatrix2(Core::ROTATION, -f_PI/24, Core::Vector3f(0.0, 0.0, 1.0));

    Core::Vector3f shotDirection2 = rotationMatrix1.apply(shotDirection);
    Core::Vector3f shotDirection3 = rotationMatrix2.apply(shotDirection);

    pShot3->initialize(shotPosition1, shotDirection2, shotSpeed, 2.0f, 4.0f, 3);
    pShot4->initialize(shotPosition2, shotDirection3, shotSpeed, 2.0f, 4.0f, 3);

    Ptr<Shot> pShot5(new Shot(_pWorld));
    Ptr<Shot> pShot6(new Shot(_pWorld));
    Core::Matrix4f rotationMatrix3(Core::ROTATION, f_PI/12, Core::Vector3f(0.0, 0.0, 1.0));
    Core::Matrix4f rotationMatrix4(Core::ROTATION, -f_PI/12, Core::Vector3f(0.0, 0.0, 1.0));

    Core::Vector3f shotDirection4 = rotationMatrix3.apply(shotDirection);
    Core::Vector3f shotDirection5 = rotationMatrix4.apply(shotDirection);

    pShot5->initialize(shotPosition1, shotDirection4, shotSpeed, 2.0f, 4.0f, 3);
    pShot6->initialize(shotPosition2, shotDirection5, shotSpeed, 2.0f, 4.0f, 3);

    _shots.push_back(pShot1);
    _shots.push_back(pShot2);
    _shots.push_back(pShot3);
    _shots.push_back(pShot4);
    _shots.push_back(pShot5);
    _shots.push_back(pShot6);

    _lastShot = 0.08;
}

}//namespace ChronoRage