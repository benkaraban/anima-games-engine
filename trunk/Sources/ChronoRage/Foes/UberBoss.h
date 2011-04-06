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
#ifndef CHRONORAGE_UBERBOSS_H_
#define CHRONORAGE_UBERBOSS_H_

#include <Foes/FoeCommon.h>
#include <Foes/Shots/Laser.h>
#include <VoxelFont.h>

namespace ChronoRage
{

//class LM_API_CHR FoeCharacter : public FoeCommon
//{
//public:
//    FoeCharacter(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode,
//        const Ptr<VoxelSpriteTemplate> & pTemplate,
//        const Core::Vector3f & deltaPos);
//
//    virtual int32 foeCount() const;
//    virtual void setTextPos(const Core::Vector3f & pos);
//
//    virtual void initialize(const Core::Vector3f & position);
//
//    virtual Ptr<VoxelSpriteTemplate> getTemplate() const { return _pTemplate; }
//
//protected:
//    Ptr<VoxelSpriteTemplate>    _pTemplate;
//    Core::Vector3f              _deltaPos;
//};

class UberBoss;

class LM_API_CHR UberBossPart : public FoeCommon
{
public:
    UberBossPart(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode,
        const Ptr<VoxelSpriteTemplate> & pTemplate,
        const Ptr<Core::CollisionShape> & pShape,
        UberBoss * pUberBoss,
        float hp
        );

    virtual int32 foeCount() const;
    virtual void initialize(const Core::Vector3f & position);
    virtual void finishCreation();
    virtual void update(double elapsed);

    //virtual void setUberBossPos(const Core::Vector3f & pos);

    virtual Ptr<VoxelSpriteTemplate> getTemplate() const { return _pTemplate; }

protected:
    Ptr<VoxelSpriteTemplate>    _pTemplate;
    UberBoss *                  _pUberBoss;
};

class LM_API_CHR UberBoss : public Foe
{
public:
    UberBoss(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);

    virtual void initialize(const Core::Vector3f & position);
    virtual void updateCreation(double elapsed);

    virtual bool displayWarning() const;

    virtual void damage(float d, const Core::Vector3f & direction, bool playerDestruction);
    virtual Ptr<VoxelSpriteTemplate> getTemplate() const;

    virtual Ptr<Universe::Node> getNode() const { return _pNodeFoe; }

protected:
    virtual void update(double elapsed);
    virtual void updateActive(double elapsed);

protected:
    void bigBoom();
    void initializePhase0();
    void initializePhase1();
    void initializePhase1To2();
    void initializePhase2();
    void initializePhase2To3();
    void initializePhase3();

    void updateActivePhase0(double elapsed);
    void updateActivePhase1(double elapsed);
    void updateActivePhase1To2(double elapsed);
    void updateActivePhase2(double elapsed);
    void updateActivePhase2To3(double elapsed);
    void updateActivePhase3(double elapsed);

    void initTemplate(Ptr<VoxelSpriteTemplate> & pTemplate, const String & file) const;

    int32                               _phase;

    float                               _phaseCooldown;
    float                               _bonusCooldown;
    float                               _minionCooldown;
    float                               _bulletCooldown;
    float                               _torpedoCooldown;
    float                               _hornetsCooldown;
    float                               _homingCooldown;
    float                               _laserCooldown;
    bool                                _laserLoading;
    bool                                _laserShooting;

    int32                               _iBonus;
    Ptr<Universe::Node>                 _pLaserLoad;
    Ptr<UberLaser>                      _pLaser;

    Ptr<Universe::Node>                 _pLaserLoad1;
    Ptr<Universe::Node>                 _pLaserLoad2;
    Ptr<Laser>                          _pLaser1;
    Ptr<Laser>                          _pLaser2;

    Ptr<UberBossPart>                   _pPartHull1;
    Ptr<UberBossPart>                   _pPartHull2;
    Core::Vector3f                      _speed;

    static Ptr<VoxelSpriteTemplate> spTemplatePartCore;
    static Ptr<VoxelSpriteTemplate> spTemplatePartHull1;
    static Ptr<VoxelSpriteTemplate> spTemplatePartHull2;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_BUCKLER_H_*/