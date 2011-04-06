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
#include "UberBoss.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>
#include <Core/Strings/Tools.h>

#include <Universe/Tasks/PostFX.h>
#include <Universe/Tasks/Shake.h>

#include <Foes/BonusCarrier.h>
#include <Foes/Kamikaze.h>
#include <Foes/Grunt.h>

#include <Foes/Shots/Bullet.h>
#include <Foes/Shots/PlasmaTorpedo.h>
#include <Foes/Shots/Hornet.h>
#include <Foes/Shots/HomingMissile.h>

#include <Modes/GameMode.h>

namespace ChronoRage
{
static const float CUBE_INTERNAL_SIZE   = 0.4f;
static const float CUBE_PADDING         = 0.1f;
static const float CUBE_SIZE            = CUBE_INTERNAL_SIZE + CUBE_PADDING;

static const int32 PHASE_0              = 0;
static const int32 PHASE_1              = 1;
static const int32 PHASE_1_TO_2         = 2;
static const int32 PHASE_2              = 3;
static const int32 PHASE_2_TO_3         = 4;
static const int32 PHASE_3              = 5;

#if 0
static const float PH1_HEALTH           = 35.0f; // *4 damage quand core
static const float PH2_HEALTH           = 50.0f; // *4 damage quand core
static const float PH3_HEALTH           = 10.0f;
#else
static const float PH1_HEALTH           = 3500.0f; // *4 damage quand core
static const float PH2_HEALTH           = 5000.0f; // *4 damage quand core
static const float PH3_HEALTH           = 1000.0f;
#endif

// PHASE 1 -----------------------------------------------------

static const float PH1_ACCELERATION         = 10.0f;
static const float PH1_MAX_SPEED            = 10.0f;
static const float PH1_DECELERATION_DIST    = 5.0f;
static const float PH1_RADIAL_SPEED         = 0.3f;
static const float PH1_IDEAL_DISTANCE       = 30.0f;

static const float PH1_BONUS_COOLDOWN1      = 27.3f;
static const float PH1_BONUS_COOLDOWN2      = 27.3f;

static const float PH1_MINION_COOLDOWN1     = 1.85f;
static const float PH1_MINION_COOLDOWN2     = 1.85f;

static const float PH1_BULLET_COOLDOWN1     = 0.57f;
static const float PH1_BULLET_COOLDOWN2     = 0.27f;

static const float PH1_HORNETS_COOLDOWN1    = 6.3f;
static const float PH1_HORNETS_COOLDOWN2    = 2.7f;

static const float PH1_TORPEDO_COOLDOWN1    = 9.1f;
static const float PH1_TORPEDO_COOLDOWN2    = 5.0f;

static const float PH1_LASER_COOLDOWN1      = 9.8f;
static const float PH1_LASER_COOLDOWN2      = 6.3f;
static const float PH1_LASER_LOADING        = 3.0f;
static const float PH1_LASER_DURATION       = 3.17f;

// PHASE 2 -----------------------------------------------------

static const float PH2_ACCELERATION         = 15.0f;
static const float PH2_MAX_SPEED            = 20.0f;
static const float PH2_DECELERATION_DIST    = 10.0f;
static const float PH2_RADIAL_SPEED         = 0.6f;
static const float PH2_IDEAL_DISTANCE       = 45.0f;

static const float PH2_BULLET_COOLDOWN1     = 0.37f;
static const float PH2_BULLET_COOLDOWN2     = 0.17f;

static const float PH2_HOMING_COOLDOWN1     = 2.0f;
static const float PH2_HOMING_COOLDOWN2     = 1.27f;

static const float PH2_LASER_COOLDOWN1      = 9.8f;
static const float PH2_LASER_COOLDOWN2      = 9.3f;
static const float PH2_LASER_LOADING        = 3.0f;
static const float PH2_LASER_DURATION       = 9.17f;

static const float PH2_MINION_COOLDOWN1     = 2.85f;
static const float PH2_MINION_COOLDOWN2     = 1.85f;

// PHASE 3 -----------------------------------------------------

static const float PH3_ACCELERATION         = 15.0f;
static const float PH3_MAX_SPEED            = 20.0f;
static const float PH3_DECELERATION_DIST    = 10.0f;
static const float PH3_RADIAL_SPEED         = 1.6f;
static const float PH3_IDEAL_DISTANCE       = 35.0f;

static const float PH3_BULLET_COOLDOWN1     = 0.17f;
static const float PH3_BULLET_COOLDOWN2     = 0.17f;

static const float PH3_MINION_COOLDOWN1     = 0.35f;
static const float PH3_MINION_COOLDOWN2     = 0.25f;

Ptr<VoxelSpriteTemplate> UberBoss::spTemplatePartCore;
Ptr<VoxelSpriteTemplate> UberBoss::spTemplatePartHull1;
Ptr<VoxelSpriteTemplate> UberBoss::spTemplatePartHull2;
//
UberBoss::UberBoss(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   Foe(UBER_BOSS, pWorld, pPlayer, gameMode),
    _speed(0.0f)
{
    initTemplate(spTemplatePartCore, L"/ChronoRageData/Foes/voxels-uberboss-core.tga");
    initTemplate(spTemplatePartHull1, L"/ChronoRageData/Foes/voxels-uberboss-hull1.tga");
    initTemplate(spTemplatePartHull2, L"/ChronoRageData/Foes/voxels-uberboss-hull2.tga");

    _maxHealthPoint = _healthPoint = PH3_HEALTH;
}

Ptr<VoxelSpriteTemplate> UberBoss::getTemplate() const
{ 
    return spTemplatePartCore;
}

void UberBoss::initialize(const Core::Vector3f & position)
{
    Foe::initialize(position);

    _bonusCooldown = 1.0f;

    initializePhase0();
}

bool UberBoss::displayWarning() const
{
    return _phase == PHASE_0 && _pLaser == null;
}

void UberBoss::initTemplate(Ptr<VoxelSpriteTemplate> & pTemplate, const String & file) const
{
    if(pTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(file);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        pTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, CUBE_INTERNAL_SIZE, CUBE_PADDING));
    }
}

void UberBoss::initializePhase0()
{
    initializePhase1();

    _pNodeFoe->setWorldPosition(Core::Vector3f(0.0f, 60.0f, _pNodeFoe->getWorldPosition().z));
    _pNodeFoe->setWorldDirection(Core::Vector3f(0.0f, -1.0f, 0.0f));

    _phaseCooldown = 5.5f;
    _phase = PHASE_0;
}

void UberBoss::initializePhase1()
{
    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(CUBE_SIZE * Core::Vector3f(0.0f, 0.0f, 6.0f), CUBE_SIZE * 6.0f)));

    Ptr<Core::CollisionShape> pShapeHull1(new Core::CollisionShape());

    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-13.0f, 0.0f,   1.0f), CUBE_SIZE * 6.0f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( -8.0f, 0.0f,  10.0f), CUBE_SIZE * 3.5f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-11.0f, 0.0f,   7.0f), CUBE_SIZE * 3.5f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-16.0f, 0.0f,  -9.0f), CUBE_SIZE * 10.0f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( -4.0f, 0.0f, -13.0f), CUBE_SIZE * 6.0f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 13.0f, 0.0f,   1.0f), CUBE_SIZE * 6.0f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(  8.0f, 0.0f,  10.0f), CUBE_SIZE * 3.5f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 11.0f, 0.0f,   7.0f), CUBE_SIZE * 3.5f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 16.0f, 0.0f,  -9.0f), CUBE_SIZE * 10.0f));
    pShapeHull1->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(  4.0f, 0.0f, -13.0f), CUBE_SIZE * 6.0f));

    _pPartHull1 = Ptr<UberBossPart>(new UberBossPart(_pWorld, _pPlayer, _gameMode, spTemplatePartHull1, pShapeHull1, this, PH1_HEALTH));
    _pPartHull1->initialize(getPosition());
    _gameMode.addDelayedFoe(_pPartHull1);

    _pLaserLoad = _pWorld->createInstance(L"/ChronoRageData/Foes/Shots/uber-concentration.group");
    _pNodeFoe->addChild(_pLaserLoad);
    _pLaserLoad->setLocalPosition(Core::Vector3f(0.0f, 0.0f, 5.0f));
    _pLaserLoad->localPitch(f_PI_DIV_2);
    _pLaserLoad->restart(true);
    _pLaserLoad->stop(true);
    _laserLoading = false;

    _iBonus = 0;
    _bonusCooldown = PH1_BONUS_COOLDOWN1;
    _bulletCooldown = 2.0f * PH1_BULLET_COOLDOWN1;
    _hornetsCooldown = 1.0f * PH1_HORNETS_COOLDOWN1;
    _torpedoCooldown = 1.0f * PH1_TORPEDO_COOLDOWN1;
    _laserCooldown = 1.0f * PH1_LASER_COOLDOWN1;
    _minionCooldown = 5.0f;

    _phase = PHASE_1;
}

void UberBoss::initializePhase1To2()
{
    if(_pLaserLoad != null)
        _pLaserLoad->stop(true);

    if(_pLaser != null)
    {
        _pLaser->stop();
        _pLaser = null;
    }

    _phaseCooldown = 1.5f;
    _phase = PHASE_1_TO_2;
}

void UberBoss::initializePhase2To3()
{
    if(_pLaserLoad1 != null)
        _pLaserLoad1->kill();

    if(_pLaserLoad2 != null)
        _pLaserLoad2->kill();

    if(_pLaser1 != null)
    {
        _pLaser1->madeHit();
        _pLaser1 = null;
    }

    if(_pLaser2 != null)
    {
        _pLaser2->madeHit();
        _pLaser2 = null;
    }

    _phaseCooldown = 1.5f;
    _phase = PHASE_2_TO_3;
}

void UberBoss::initializePhase2()
{
    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(CUBE_SIZE * Core::Vector3f(0.0f, 0.0f, 6.0f), CUBE_SIZE * 6.0f)));

    Ptr<Core::CollisionShape> pShapeHull2(new Core::CollisionShape());

    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-27.0f, 0.0f,  31.0f), CUBE_SIZE * 3.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-27.0f, 0.0f,  19.0f), CUBE_SIZE * 11.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-18.0f, 0.0f,   8.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-13.0f, 0.0f,   6.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-34.0f, 0.0f,  -9.0f), CUBE_SIZE * 6.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-20.0f, 0.0f,  -7.0f), CUBE_SIZE * 6.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-27.0f, 0.0f, -13.0f), CUBE_SIZE * 4.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(-14.0f, 0.0f, -13.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( -4.0f, 0.0f, -13.0f), CUBE_SIZE * 6.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( -9.0f, 0.0f,  -8.0f), CUBE_SIZE * 4.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( -9.0f, 0.0f,   2.0f), CUBE_SIZE * 2.5f));

    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 27.0f, 0.0f,  31.0f), CUBE_SIZE * 3.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 27.0f, 0.0f,  19.0f), CUBE_SIZE * 11.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 18.0f, 0.0f,   8.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 13.0f, 0.0f,   6.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 34.0f, 0.0f,  -9.0f), CUBE_SIZE * 6.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 20.0f, 0.0f,  -7.0f), CUBE_SIZE * 6.0f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 27.0f, 0.0f, -13.0f), CUBE_SIZE * 4.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f( 14.0f, 0.0f, -13.0f), CUBE_SIZE * 3.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(  4.0f, 0.0f, -13.0f), CUBE_SIZE * 6.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(  9.0f, 0.0f,  -8.0f), CUBE_SIZE * 4.5f));
    pShapeHull2->addSphere(Core::Sphere(CUBE_SIZE * Core::Vector3f(  9.0f, 0.0f,   2.0f), CUBE_SIZE * 2.5f));

    _pPartHull2 = Ptr<UberBossPart>(new UberBossPart(_pWorld, _pPlayer, _gameMode, spTemplatePartHull2, pShapeHull2, this, PH2_HEALTH));
    _pPartHull2->initialize(getPosition());
    _gameMode.addDelayedFoe(_pPartHull2);

    _pLaserLoad1 = _pWorld->createInstance(L"/ChronoRageData/Foes/Shots/concentration.group");
    _pLaserLoad2 = _pWorld->createInstance(L"/ChronoRageData/Foes/Shots/concentration.group");

    Core::Vector3f pos1(CUBE_SIZE * Core::Vector3f(-27.0f, 0.0, 27.0f));
    Core::Vector3f pos2(CUBE_SIZE * Core::Vector3f( 27.0f, 0.0, 27.0f));

    _pNodeFoe->addChild(_pLaserLoad1);
    _pLaserLoad1->setLocalPosition(pos1);
    _pLaserLoad1->restart(true);
    _pLaserLoad1->stop(true);

    _pNodeFoe->addChild(_pLaserLoad2);
    _pLaserLoad2->setLocalPosition(pos2);
    _pLaserLoad2->restart(true);
    _pLaserLoad2->stop(true);

    _laserLoading = false;
    _iBonus = 0;
    _bulletCooldown = 4.0f * PH2_BULLET_COOLDOWN1;
    _homingCooldown = 2.0f * PH2_HOMING_COOLDOWN1;
    _laserCooldown = PH2_LASER_COOLDOWN1;
    _minionCooldown = 5.0f;

    _phase = PHASE_2;
}

void UberBoss::initializePhase3()
{
    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(CUBE_SIZE * Core::Vector3f(0.0f, 0.0f, 6.0f), CUBE_SIZE * 6.0f)));
    _minionCooldown = 1.0f;

    _phase = PHASE_3;
}

void UberBoss::damage(float d, const Core::Vector3f & direction, bool playerDestruction)
{
    if(!playerDestruction)
    {
        if(_pPartHull1 != null && _pPartHull1->isAlive())
        {
            _pVoxelSprite->blink(0.05f, 1);
            _pPartHull1->damage(4.0f * d, direction, false);
        }
        else if(_pPartHull2 != null && _pPartHull2->isAlive())
        {
            _pVoxelSprite->blink(0.05f, 1);
            _pPartHull2->damage(4.0f * d, direction, false);
        }
        else if(_phase == PHASE_3)
        {
            Foe::damage(d, direction, playerDestruction);

            if(_state == FOE_DESTRUCTION_IN_PROGRESS)
            {
                bigBoom();
                _gameMode.killAllFoes();
            }
        }
    }
}

void UberBoss::update(double elapsed)
{
    Foe::update(elapsed);

    if(_state == FOE_DESTRUCTION_IN_PROGRESS)
        _gameMode.killAllFoes();
}

void UberBoss::updateActive(double elapsed)
{
    switch(_phase)
    {
    case PHASE_0:       updateActivePhase0(elapsed); break;
    case PHASE_1:       updateActivePhase1(elapsed); break;
    case PHASE_1_TO_2:  updateActivePhase1To2(elapsed); break;
    case PHASE_2:       updateActivePhase2(elapsed); break;
    case PHASE_2_TO_3:  updateActivePhase2To3(elapsed); break;
    case PHASE_3:       updateActivePhase3(elapsed); break;
    }
}

void UberBoss::updateActivePhase1To2(double elapsed)
{
    _phaseCooldown -= float(elapsed);

    if(_phaseCooldown <= 0.0f)
        initializePhase2();
}

void UberBoss::updateActivePhase2To3(double elapsed)
{
    _phaseCooldown -= float(elapsed);

    if(_phaseCooldown <= 0.0f)
        initializePhase3();
}

void UberBoss::updateActivePhase0(double elapsed)
{
    _phaseCooldown -= float(elapsed);

    if(_phaseCooldown < 2.5f && _pLaser == null)
    {
        Core::Vector3f pos(0.0f, 0.0, 4.0f);
        _pLaser = Ptr<UberLaser>(new UberLaser(_pWorld, _pPlayer, _gameMode));
        _pLaser->initialize(pos);
        _pNodeFoe->addChild(_pLaser->getNode());
        _gameMode.addFoeShot(_pLaser);
        _gameMode.playSound(SOUND_FOE_SHOT_LASER, false);
    }

    if(_phaseCooldown <= 0.0f)
    {
        if(_pLaser != null)
        {
            _pLaser->stop();
            _pNodeFoe->removeChild(_pLaser->getNode());
            _pLaser = null;
        }

        _phase = PHASE_1;
    }
}

void UberBoss::updateActivePhase1(double elapsed)
{
    float ratio = Core::clamp(0.0f, 1.0f, (1.0f - _pPartHull1->getHealthRatio()) * 1.2f);

    // Move
    Core::Vector3f playerPosition = _pPlayer->getPosition();
    Core::Vector3f medianPos(0.0f, 0.0f, playerPosition.z);
    float yMul = Core::clamp(0.0f, 1.0f, ((playerPosition.y + 35.0f) / 70.0f));
    medianPos.y = Core::lerp(10.0f, 40.0f, yMul);

    Core::Vector3f medianVec(medianPos - playerPosition);
    medianVec.normalizeSafe();
    medianVec *= PH1_IDEAL_DISTANCE;

    Core::Vector3f targetPos(playerPosition + medianVec);

    if(_laserCooldown <= 0.0f)
    {
        Core::Vector3f playerVec((playerPosition - getPosition()).getNormalizedSafe());
        float angle = Core::L_acosSafe(Core::dot(playerVec, _pNodeFoe->getWorldDirection()));
        Core::Vector3f currentPosition = getPosition();
        Core::Vector3f direction = _pNodeFoe->getWorldDirection();
        if(angle > 0.5f * f_PI_DIV_2)
            targetPos = getPosition() - 40.0f * _pNodeFoe->getWorldDirection();
        targetPos.z = playerPosition.z;
    }

    _gameMode.movePointInPlayground(targetPos);

    Core::Vector3f accel(targetPos - getPosition());
    float targetDist = accel.getLengthSafe();
    accel.normalizeSafe();

    _speed = _speed + PH1_ACCELERATION * float(elapsed) * accel;

    LM_ASSERT(_speed.z == 0.0f);

    float scalarSpeed = _speed.getLengthSafe();
    float maxSpeed = Core::lerp(0.0f, PH1_MAX_SPEED, Core::clamp(0.0f, 1.0f, targetDist / PH1_DECELERATION_DIST));
    
    if(scalarSpeed > maxSpeed)
        _speed *= maxSpeed / scalarSpeed;

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = getPosition() + (_speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    // Turn

    Core::Vector3f localTarget(_pNodeFoe->getParentWorldToLocalMatrix().apply(playerPosition));
    Core::Vector3f localDir(localTarget - _pNodeFoe->getLocalPosition());
    Core::Matrix3f mat;
    mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoe->getUpVector(), _pNodeFoe->getLocalMatrix().getYVector());

    Core::Quaternionf q1(_pNodeFoe->getLocalOrient());
    Core::Quaternionf q2(mat);

    double rotTime = elapsed;
    float angleWithPlayer = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
    LM_ASSERT(Core::isValid(angleWithPlayer));
    float maxAngle = float(((_laserCooldown <= 0.0f) ? (0.2f * PH1_RADIAL_SPEED) : PH1_RADIAL_SPEED) * rotTime);
    float alpha = 1.0;

    if(angleWithPlayer != 0.0)
        alpha = std::min(1.0f, maxAngle / angleWithPlayer);

    Core::Quaternionf newOrient(q1, q2, alpha);

    _pNodeFoe->setLocalOrientation(newOrient);

    _laserCooldown -= float(elapsed);
    _minionCooldown -= float(elapsed);
    _bonusCooldown -= float(elapsed);

    // Hornets toujours activ�s
    if(_hornetsCooldown <= 0.0f)
    {
        Ptr<Hornet> pHornet(new Hornet(_pWorld, _pPlayer, _gameMode));

        Core::Vector3f pos(-23.0f, 0.0, 2.0f);
        Core::Vector3f dir(1.0f, 0.0, 0.0f);

        switch(Core::irand(0, 3))
        {
        case 0: pos = Core::Vector3f(-19.0f, 0.0, -19.0f); dir = Core::Vector3f(-1.0f, 0.0f, -1.0f); break;
        case 1: pos = Core::Vector3f(-15.0f, 0.0, -21.0f); dir = Core::Vector3f(-1.0f, 0.0f, -1.0f); break;
        case 2: pos = Core::Vector3f( 19.0f, 0.0, -19.0f); dir = Core::Vector3f( 1.0f, 0.0f, -1.0f); break;
        case 3: pos = Core::Vector3f( 15.0f, 0.0, -21.0f); dir = Core::Vector3f( 1.0f, 0.0f, -1.0f); break;
        }

        pHornet->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos));
        pHornet->setDirection(_pNodeFoe->getLocalToWorldMatrix().apply3x3(dir));

        _gameMode.addFoeShot(pHornet);
        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _hornetsCooldown = Core::lerp(PH1_HORNETS_COOLDOWN1, PH1_HORNETS_COOLDOWN2, ratio);
    }

    if(_laserCooldown > 0.0f && _laserCooldown <= PH1_LASER_LOADING && !_laserLoading)
    {
        _pLaserLoad->restart(true);
        _laserLoading = true;        
    }

    if(_laserCooldown <= 0.0f)
    {
        // Laser actif => 4* plus de hornets
        _hornetsCooldown -= 4.0f * float(elapsed);

        if(_laserCooldown < (0.5f -PH1_LASER_DURATION))
        {
            _pLaserLoad->stop(true);
            _laserLoading = false;
        }

        if(_pLaser == null)
        {
            Core::Vector3f pos(0.0f, 0.0, 4.0f);
            _pLaser = Ptr<UberLaser>(new UberLaser(_pWorld, _pPlayer, _gameMode));
            _pLaser->initialize(pos);
            _pNodeFoe->addChild(_pLaser->getNode());
            _gameMode.addFoeShot(_pLaser);
            _gameMode.playSound(SOUND_FOE_SHOT_LASER, false);
        }
        else if(_laserCooldown < -PH1_LASER_DURATION)
        {
            _pLaser->stop();
            _pNodeFoe->removeChild(_pLaser->getNode());
            _pLaser = null;
            _laserCooldown = Core::lerp(PH1_LASER_COOLDOWN1, PH1_LASER_COOLDOWN2, ratio);
        }
    }
    else
    {
        // Sans laser => bullets + torpedos + hornets
        _hornetsCooldown -= float(elapsed);
        _bulletCooldown -= float(elapsed);
        _torpedoCooldown -= float(elapsed);

        if(_bulletCooldown <= 0.0f)
        {
            Ptr<Bullet> pBullet(new Bullet(_pWorld, _pPlayer, _gameMode));

            Core::Vector3f pos(-23.0f, 0.0, 2.0f);

            switch(Core::irand(0, 3))
            {
            case 0: pos = Core::Vector3f(-23.0f, 0.0, 2.0f); break;
            case 1: pos = Core::Vector3f(-13.0f, 0.0, 12.0f); break;
            case 2: pos = Core::Vector3f( 23.0f, 0.0, 2.0f); break;
            case 3: pos = Core::Vector3f( 13.0f, 0.0, 12.0f); break;
            }

            pBullet->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos), _pNodeFoe->getWorldDirection());

            _gameMode.addFoeShot(pBullet);
            _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

            _bulletCooldown = Core::lerp(PH1_BULLET_COOLDOWN1, PH1_BULLET_COOLDOWN2, ratio);
        }

        if(_torpedoCooldown <= 0.0f)
        {
            Ptr<PlasmaTorpedo> pPlasmaTorpedo(new PlasmaTorpedo(_pWorld, _pPlayer, _gameMode));
            Core::Vector3f pos(0.0f, 0.0, 8.0f);
            pPlasmaTorpedo->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos), _pNodeFoe->getWorldDirection());
            _gameMode.addFoeShot(pPlasmaTorpedo);
            _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

            _torpedoCooldown = Core::lerp(PH1_TORPEDO_COOLDOWN1, PH1_TORPEDO_COOLDOWN2, ratio);
        }
    }

    if(_minionCooldown <= 0.0f)
    {
        _minionCooldown = Core::lerp(PH1_MINION_COOLDOWN1, PH1_MINION_COOLDOWN2, ratio);

        if(_gameMode.getFoesNumber() < 20)
        {
            Core::Vector3f pos(getPosition());
            Core::Vector3f foePos;

            if(pos.x < 0.0f)
                foePos.x = Core::frand(50.0f, 60.0f);
            else
                foePos.x = -Core::frand(50.0f, 60.0f);

            if(pos.y < 0.0f)
                foePos.y = Core::frand(35.0f, 40.0f);
            else
                foePos.y = -Core::frand(35.0f, 40.0f);

            foePos.z = pos.z;
            _gameMode.movePointInPlayground(foePos);    

            Ptr<Foe> pFoe(new Kamikaze(_pWorld, _pPlayer, _gameMode));
            pFoe->initialize(foePos);
            _gameMode.addDelayedFoe(pFoe);
        }
    }

    if(_bonusCooldown <= 0.0f)
    {
        _bonusCooldown = (_iBonus < 4 || _pPlayer->getPowerLevel() < 2.3f) ? 17.0f : 30.0f;
        _iBonus ++;

        Core::Vector3f pos(getPosition());
        Core::Vector3f foePos;

        if(pos.x < 0.0f)
            foePos.x = Core::frand(50.0f, 60.0f);
        else
            foePos.x = -Core::frand(50.0f, 60.0f);

        foePos.y = -35.0f;
        foePos.z = pos.z;
        _gameMode.movePointInPlayground(foePos);    

        Ptr<Foe> pFoe(new BonusCarrier(_pWorld, _pPlayer, _gameMode));
        pFoe->initialize(foePos);
        _gameMode.addDelayedFoe(pFoe);
    }

    if(!_pPartHull1->isAlive())
        initializePhase1To2();
}

void UberBoss::updateActivePhase2(double elapsed)
{
    float ratio = Core::clamp(0.0f, 1.0f, (1.0f - _pPartHull2->getHealthRatio()) * 1.2f);

    // Move
    Core::Vector3f playerPosition = _pPlayer->getPosition();
    Core::Vector3f medianPos(0.0f, 0.0f, playerPosition.z);
    Core::Vector3f medianVec(medianPos - playerPosition);
    medianVec.normalizeSafe();
    medianVec *= PH2_IDEAL_DISTANCE;

    Core::Vector3f targetPos(playerPosition + medianVec);

    _gameMode.movePointInPlayground(targetPos);

    Core::Vector3f accel(targetPos - getPosition());
    float targetDist = accel.getLengthSafe();
    accel.normalizeSafe();

    _speed = _speed + PH2_ACCELERATION * float(elapsed) * accel;

    LM_ASSERT(_speed.z == 0.0f);

    float scalarSpeed = _speed.getLengthSafe();
    float maxSpeed = Core::lerp(0.0f, PH2_MAX_SPEED, Core::clamp(0.0f, 1.0f, targetDist / PH2_DECELERATION_DIST));
    
    if(scalarSpeed > maxSpeed)
        _speed *= maxSpeed / scalarSpeed;

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = getPosition() + (_speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    // Turn

    Core::Vector3f localTarget(_pNodeFoe->getParentWorldToLocalMatrix().apply(playerPosition));
    Core::Vector3f localDir(localTarget - _pNodeFoe->getLocalPosition());
    Core::Matrix3f mat;
    mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoe->getUpVector(), _pNodeFoe->getLocalMatrix().getYVector());

    Core::Quaternionf q1(_pNodeFoe->getLocalOrient());
    Core::Quaternionf q2(mat);

    double rotTime = elapsed;
    float angleWithPlayer = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
    LM_ASSERT(Core::isValid(angleWithPlayer));
    float maxAngle = float(((_laserCooldown <= 0.0f) ? (0.2f * PH2_RADIAL_SPEED) : PH2_RADIAL_SPEED) * rotTime);
    float alpha = 1.0;

    if(angleWithPlayer != 0.0)
        alpha = std::min(1.0f, maxAngle / angleWithPlayer);

    Core::Quaternionf newOrient(q1, q2, alpha);

    _pNodeFoe->setLocalOrientation(newOrient);

    // Fire
    _bulletCooldown -= float(elapsed);
    _homingCooldown -= float(elapsed);
    _laserCooldown -= float(elapsed);
    _bonusCooldown -= float(elapsed);

    if(_bulletCooldown <= 0.0f)
    {
        Ptr<Bullet> pBullet(new Bullet(_pWorld, _pPlayer, _gameMode));

        Core::Vector3f pos(0.0f, 0.0, 8.0f);
        Core::Vector3f dir(_pNodeFoe->getWorldDirection());
        Core::Matrix4f rot(Core::ROTATION, Core::frand(-0.7f, 0.7f), Core::ROT_Z_ROLL);

        pBullet->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos), rot.apply3x3(dir));

        _gameMode.addFoeShot(pBullet);
        _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

        _bulletCooldown = Core::lerp(PH2_BULLET_COOLDOWN1, PH2_BULLET_COOLDOWN2, ratio);
    }

    if(_homingCooldown <= 0.0f)
    {
        Ptr<HomingMissile> pHoming(new HomingMissile(_pWorld, _pPlayer, _gameMode));

        Core::Vector3f pos(-23.0f, 0.0, 2.0f);
        Core::Vector3f dir(1.0f, 0.0, 0.0f);

        switch(Core::irand(0, 3))
        {
        case 0: pos = Core::Vector3f(-36.0f, 0.0, -4.0f); dir = Core::Vector3f(-1.0f, 0.0f, 1.0f); break;
        case 1: pos = Core::Vector3f(-21.0f, 0.0, -4.0f); dir = Core::Vector3f( 0.0f, 0.0f, 1.0f); break;
        case 2: pos = Core::Vector3f( 21.0f, 0.0, -4.0f); dir = Core::Vector3f( 0.0f, 0.0f, 1.0f); break;
        case 3: pos = Core::Vector3f( 36.0f, 0.0, -4.0f); dir = Core::Vector3f( 1.0f, 0.0f, 1.0f); break;
        }

        pHoming->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos));
        pHoming->setDirection(_pNodeFoe->getLocalToWorldMatrix().apply3x3(dir));

        _gameMode.addFoeShot(pHoming);
        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _homingCooldown = Core::lerp(PH2_HOMING_COOLDOWN1, PH2_HOMING_COOLDOWN2, ratio);
    }

    if(_laserCooldown > 0.0f && _laserCooldown <= PH2_LASER_LOADING && !_laserLoading)
    {
        _pLaserLoad1->restart(true);
        _pLaserLoad2->restart(true);
        _laserLoading = true;        
    }

    if(_laserCooldown <= 0.0f)
    {
        if(_laserCooldown < (0.5f - PH2_LASER_DURATION))
        {
            _pLaserLoad1->stop(true);
            _pLaserLoad2->stop(true);
            _laserLoading = false;
        }

        if(_pLaser1 == null)
        {
            Core::Vector3f pos1(CUBE_SIZE * Core::Vector3f(-27.0f, 0.0, 27.0f));
            Core::Vector3f pos2(CUBE_SIZE * Core::Vector3f( 27.0f, 0.0, 27.0f));
            _pLaser1 = Ptr<Laser>(new Laser(_pWorld, _pPlayer, _gameMode));
            _pLaser2 = Ptr<Laser>(new Laser(_pWorld, _pPlayer, _gameMode));
            _pLaser1->initialize(pos1);
            _pLaser2->initialize(pos2);
            _pNodeFoe->addChild(_pLaser1->getNode());
            _pNodeFoe->addChild(_pLaser2->getNode());
            _gameMode.addFoeShot(_pLaser1);
            _gameMode.addFoeShot(_pLaser2);
            _gameMode.playSound(SOUND_FOE_SHOT_LASER, false);
        }
        else if(_laserCooldown < -PH2_LASER_DURATION)
        {
            _pLaser1->madeHit();
            _pLaser2->madeHit();
            _pNodeFoe->removeChild(_pLaser1->getNode());
            _pNodeFoe->removeChild(_pLaser2->getNode());
            _pLaser1 = null;
            _pLaser2 = null;
            _laserCooldown = Core::lerp(PH2_LASER_COOLDOWN1, PH2_LASER_COOLDOWN2, ratio);
        }
    }
    else
    {
        _minionCooldown -= float(elapsed);

        if(_minionCooldown <= 0.0f)
        {
            _minionCooldown = Core::lerp(PH2_MINION_COOLDOWN1, PH2_MINION_COOLDOWN2, ratio);

            if(_gameMode.getFoesNumber() < 20)
            {
                Core::Vector3f foePos;

                if(playerPosition.x < 0.0f)
                {
                    if(playerPosition.y < 0.0f) // Down left => Down Right
                    {
                        foePos.x = Core::frand(50.0f, 60.0f);
                        foePos.y = Core::frand(-35.0f, -40.0f);
                    }
                    else // Up left => Up right
                    {
                        foePos.x = Core::frand(50.0f, 60.0f);
                        foePos.y = Core::frand(35.0f, 40.0f);
                    }
                }
                else
                {
                    if(playerPosition.y < 0.0f) // Down right => Down left
                    {
                        foePos.x = Core::frand(-50.0f, -60.0f);
                        foePos.y = Core::frand(-35.0f, -40.0f);
                    }
                    else // Up right => Up left
                    {
                        foePos.x = Core::frand(-50.0f, -60.0f);
                        foePos.y = Core::frand(35.0f, 40.0f);
                    }
                }

                if(Core::frand() > 0.5f)
                    foePos = -foePos;

                foePos.z = getPosition().z;
                _gameMode.movePointInPlayground(foePos);    

                Ptr<Foe> pFoe(new Kamikaze(_pWorld, _pPlayer, _gameMode));
                pFoe->initialize(foePos);
                _gameMode.addDelayedFoe(pFoe);
            }
        }
    }

    if(_bonusCooldown <= 0.0f)
    {
        _bonusCooldown = _pPlayer->getPowerLevel() < 2.3f ? 17.0f : 30.0f;
        _iBonus ++;

        Core::Vector3f pos(getPosition());
        Core::Vector3f foePos;

        if(pos.x < 0.0f)
            foePos.x = Core::frand(50.0f, 60.0f);
        else
            foePos.x = -Core::frand(50.0f, 60.0f);

        foePos.y = -35.0f;
        foePos.z = pos.z;
        _gameMode.movePointInPlayground(foePos);    

        Ptr<Foe> pFoe(new BonusCarrier(_pWorld, _pPlayer, _gameMode));
        pFoe->initialize(foePos);
        _gameMode.addDelayedFoe(pFoe);
    }

    if(_pPartHull2->getState() > FOE_ACTIVE)
        initializePhase2To3();
}

void UberBoss::updateActivePhase3(double elapsed)
{
    float ratio = Core::clamp(0.0f, 1.0f, (1.0f - getHealthRatio()) * 1.2f);

    // Move
    Core::Vector3f playerPosition = _pPlayer->getPosition();
    Core::Vector3f medianPos(0.0f, 0.0f, playerPosition.z);
    Core::Vector3f medianVec(medianPos - playerPosition);
    medianVec.normalizeSafe();
    medianVec *= PH3_IDEAL_DISTANCE;

    Core::Vector3f targetPos(playerPosition + medianVec);

    _gameMode.movePointInPlayground(targetPos);

    Core::Vector3f accel(targetPos - getPosition());
    float targetDist = accel.getLengthSafe();
    accel.normalizeSafe();

    _speed = _speed + PH3_ACCELERATION * float(elapsed) * accel;

    LM_ASSERT(_speed.z == 0.0f);

    float scalarSpeed = _speed.getLengthSafe();
    float maxSpeed = Core::lerp(0.0f, PH3_MAX_SPEED, Core::clamp(0.0f, 1.0f, targetDist / PH3_DECELERATION_DIST));
    
    if(scalarSpeed > maxSpeed)
        _speed *= maxSpeed / scalarSpeed;

    Core::Vector3f currentPosition = getPosition();
    Core::Vector3f direction = _pNodeFoe->getWorldDirection();

    Core::Vector3f newPosition = getPosition() + (_speed * (float)elapsed);

    _pNodeFoe->setWorldPosition(newPosition);

    // Turn

    Core::Vector3f localTarget(_pNodeFoe->getParentWorldToLocalMatrix().apply(playerPosition));
    Core::Vector3f localDir(localTarget - _pNodeFoe->getLocalPosition());
    Core::Matrix3f mat;
    mat.setZVec(localDir.getNormalizedSafe(), _pNodeFoe->getUpVector(), _pNodeFoe->getLocalMatrix().getYVector());

    Core::Quaternionf q1(_pNodeFoe->getLocalOrient());
    Core::Quaternionf q2(mat);

    double rotTime = elapsed;
    float angleWithPlayer = 2.0f * Core::L_acosSafe(Core::L_abs(Core::dot(q1, q2)));
    LM_ASSERT(Core::isValid(angleWithPlayer));
    float maxAngle = float(((_laserCooldown <= 0.0f) ? (0.2f * PH3_RADIAL_SPEED) : PH3_RADIAL_SPEED) * rotTime);
    float alpha = 1.0;

    if(angleWithPlayer != 0.0)
        alpha = std::min(1.0f, maxAngle / angleWithPlayer);

    Core::Quaternionf newOrient(q1, q2, alpha);

    _pNodeFoe->setLocalOrientation(newOrient);

    // Fire
    _bulletCooldown -= float(elapsed);
    _bonusCooldown -= float(elapsed);
    _minionCooldown -= float(elapsed);

    if(_bulletCooldown <= 0.0f)
    {
        Ptr<Bullet> pBullet(new Bullet(_pWorld, _pPlayer, _gameMode));

        Core::Vector3f pos(0.0f, 0.0, 8.0f);
        Core::Vector3f dir(_pNodeFoe->getWorldDirection());
        Core::Matrix4f rot(Core::ROTATION, Core::frand(-0.7f, 0.7f), Core::ROT_Z_ROLL);

        pBullet->initialize(_pNodeFoe->getLocalToWorldMatrix().apply(CUBE_SIZE * pos), rot.apply3x3(dir));

        _gameMode.addFoeShot(pBullet);
        _gameMode.playSound(SOUND_FOE_SHOT_BULLET, false);

        _bulletCooldown = Core::lerp(PH3_BULLET_COOLDOWN1, PH3_BULLET_COOLDOWN2, ratio);
    }

    if(_minionCooldown <= 0.0f)
    {
        _minionCooldown = Core::lerp(PH3_MINION_COOLDOWN1, PH3_MINION_COOLDOWN2, ratio);

        if(_gameMode.getFoesNumber() < 20)
        {
            Core::Vector3f foePos;

            switch(Core::irand(0, 5))
            {
            case 0:
            case 1:
                foePos.x = Core::frand(-60.0f, 60.0f);
                foePos.y = Core::frand(35.0f, 40.0f);
                break;
            case 2:
            case 3:
                foePos.x = Core::frand(-60.0f, 60.0f);
                foePos.y = Core::frand(-35.0f, -40.0f);
                break;
            case 4:
                foePos.x = Core::frand(-60.0f, -50.0f);
                foePos.y = Core::frand(-40.0f, 40.0f);
                break;
            case 5:
                foePos.x = Core::frand(50.0f, 60.0f);
                foePos.y = Core::frand(-40.0f, 40.0f);
                break;
            }

            if(Core::frand() > 0.5f)
                foePos = -foePos;

            foePos.z = getPosition().z;
            _gameMode.movePointInPlayground(foePos);    

            Ptr<Foe> pFoe(new Grunt(_pWorld, _pPlayer, _gameMode));
            pFoe->initialize(foePos);
            _gameMode.addDelayedFoe(pFoe);
        }
    }

    if(_bonusCooldown <= 0.0f)
    {
        _bonusCooldown = 8.0f;
        _iBonus ++;

        Core::Vector3f pos(getPosition());
        Core::Vector3f foePos;

        foePos.x = Core::frand(-60.0f, 60.0f);
        foePos.y = Core::frand(-40.0f, 40.0f);

        foePos.z = pos.z;
        _gameMode.movePointInPlayground(foePos);    

        Ptr<Foe> pFoe(new BonusCarrier(_pWorld, _pPlayer, _gameMode));
        pFoe->initialize(foePos);
        _gameMode.addDelayedFoe(pFoe);
    }
}

void UberBoss::updateCreation(double elapsed)
{
    _pVoxelSprite->defaultSprite();
    _state = FOE_ACTIVE;

    if(_pPartHull1 != null)
        _pPartHull1->finishCreation();
}

void UberBoss::bigBoom()
{
    Ptr<Universe::Node> pCamNode = _pWorld->getFirstNodeWithTag(L"camera");
    
    if(pCamNode != null)
    {
        Ptr<Universe::NodePerspectiveCamera> pCamera = LM_DEBUG_PTR_CAST<Universe::NodePerspectiveCamera>(_pWorld->getFirstNodeWithTag(L"camera"));

        Ptr<Universe::ITask> pTaskFlash(new Universe::TaskFXFade(   pCamera,
                                                                    Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f),
                                                                    Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f),
                                                                    0.3,
                                                                    Core::INTER_LINEAR));
        Ptr<Universe::ITask> pTaskShake(new Universe::TaskShake(   pCamera,
                                                                    3.0f, 0.1f,
                                                                    5.0f, 0.25f,
                                                                    1.0f,
                                                                    Core::INTER_FADE_OUT));
        Ptr<Universe::ITask> pTaskBlur(new Universe::TaskFXRadialBlur(pCamera, _pNodeFoe, 1.0f, 0.0f, 2.0f, Core::INTER_LINEAR));

        _pWorld->registerTask(pTaskFlash);
        _pWorld->registerTask(pTaskShake);
        _pWorld->registerTask(pTaskBlur);

        _gameMode.playSound(SOUND_PLAYER_DESTRUCTION, false);
    }
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
UberBossPart::UberBossPart(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode, 
                           const Ptr<VoxelSpriteTemplate> & pTemplate,
                           const Ptr<Core::CollisionShape> & pShape,
                           UberBoss * pUberBoss,
                           float hp
                           )
:   FoeCommon(UBER_BOSS_PART, pWorld, pPlayer, gameMode),
    _pTemplate(pTemplate),
    _pUberBoss(pUberBoss)
{
    _createShockWave = false;
    _maxHealthPoint = _healthPoint = hp;
    _minRange = 0.0f;
    _maxRange = 0.0f;
    _speed = 0.0f;
    _radialSpeed = 0.0f;
    _pCollisionShape = pShape;
}

int32 UberBossPart::foeCount() const
{
    return 0;
}

void UberBossPart::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pNodeFoe->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
}

void UberBossPart::finishCreation()
{
    _pVoxelSprite->defaultSprite();
    _state = FOE_ACTIVE;
}

void UberBossPart::update(double elapsed)
{
    _pNodeFoe->setLocalMatrix(_pUberBoss->getNode()->getLocalMatrix());

    FoeCommon::update(elapsed);
}

}//namespace ChronoRage