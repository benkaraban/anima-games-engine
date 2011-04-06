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
#include "Laser.h"

#include <Modes/GameMode.h>

namespace ChronoRage
{
Laser::Laser(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeShot(pWorld, pPlayer, gameMode),
    _width(2.0f),
    _height(150.0f),
    _totalElapsed(0.0f),
    _pSoundloadingLaser(null),
    _pSoundLaser(null)
{}

Laser::~Laser()
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

void Laser::initialize(const Core::Vector3f & position)
{
    Ptr<Universe::NodeDecal> pDecal = _pWorld->createDecal(LASER_FILE, Renderer::DECAL_ADD);
    pDecal->setSize(_width, _height);
    
    pDecal->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    pDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    pDecal->setGlow(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

    _pNodeRefrac = _pWorld->createDecal(LASER_REFRAC_FILE, Renderer::DECAL_REFRAC);
    _pNodeRefrac->setSize(_width*2.0f, _height);
    _pNodeRefrac->setLocalPosition(Core::Vector3f(0.0f, -0.5f, 0.0f));
    _pNodeRefrac->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeRefrac->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    pDecal->addChild(_pNodeRefrac);
    
    _pNodeFoeShot = pDecal;
    _pNodeFoeShot->setWorldPosition(position + Core::Vector3f(0.0f, 0.0f, _height/2));    

    _pCollisionShape = null;
    
    _pSoundloadingLaser = _gameMode.playSound(SOUND_FOE_SHOT_LASER_LOADING, false);
    _pSoundLaser        = _gameMode.playSound(SOUND_FOE_SHOT_LASER, true);
}

Core::Vector3f Laser::getPosition() const
{
    return Core::Vector3f(0.0f, 0.0f, 0.0f);
}


void Laser::update(double elapsed)
{
    elapsed *= _gameMode.getTimeWarp()->getTimeWarpFactor();

    _totalElapsed += (float)elapsed;

    Ptr<Universe::NodeDecal> pDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);

    if(_totalElapsed <= LASER_NO_DAMAGE_TIME_OUT)
    {
        Core::Vector4f color = pDecal->getColor();
        color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        pDecal->setColor(color);
        Core::Vector4f glow = pDecal->getGlow();
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        pDecal->setGlow(glow);
        
        _pNodeRefrac->setSize(_width * (2.0f + (_totalElapsed/LASER_NO_DAMAGE_TIME_OUT)), _height);
        Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
        colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        _pNodeRefrac->setColor(colorRefrac);
    }
    else
    {
        if(_totalElapsed > LASER_NO_DAMAGE_TIME_OUT && _pCollisionShape == null)
        {
            _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());

            for(float ii=0.0f; ii<_height; ii += _width)
            {
                _pCollisionShape->addSphere(Core::Sphere(Core::Vector3f(0.0f, 0.0f, ii-(_height/2)), 0.8f));
            }
        }

        float radiateFactor = 5.0f;

        float alpha = Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor));

        Core::Vector4f color = pDecal->getColor();
        color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.6f), alpha);
        pDecal->setColor(color);
        Core::Vector4f glow = pDecal->getGlow();
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), alpha);
        pDecal->setGlow(glow);
        
        _pNodeRefrac->setSize(_width* (2.0f + Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor))), _height);
        Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
        colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f), alpha);
        _pNodeRefrac->setColor(colorRefrac);
    }
    
    FoeShot::update(elapsed);
}

void Laser::fade(float alpha)
{
}

void Laser::kill(bool outOfBoundaries)
{
    _fadeTimer = FADE_TIMER;

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

UberLaser::UberLaser(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeShot(pWorld, pPlayer, gameMode),
    _width(30.0f),
    _height(150.0f),
    _totalElapsed(0.0f),
    _killed(false),
    _pSoundloadingLaser(null),
    _pSoundLaser(null)
{}

UberLaser::~UberLaser()
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

void UberLaser::initialize(const Core::Vector3f & position)
{
    Ptr<Universe::NodeDecal> pDecal = _pWorld->createDecal(UBER_LASER_FILE, Renderer::DECAL_ADD);
    pDecal->setSize(_width, _height);
    
    pDecal->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    pDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);
    pDecal->setGlow(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

    _pNodeRefrac = _pWorld->createDecal(LASER_REFRAC_FILE, Renderer::DECAL_REFRAC);
    _pNodeRefrac->setSize(_width*2.0f, _height);
    _pNodeRefrac->setLocalPosition(Core::Vector3f(0.0f, -0.5f, 0.0f));
    _pNodeRefrac->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeRefrac->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    pDecal->addChild(_pNodeRefrac);
    
    _pNodeFoeShot = pDecal;
    _pNodeFoeShot->setWorldPosition(position + Core::Vector3f(0.0f, 0.0f, _height/2));    

    _pCollisionShape = null;
    
    _pSoundloadingLaser = _gameMode.playSound(SOUND_FOE_SHOT_LASER_LOADING, false);
    _pSoundLaser        = _gameMode.playSound(SOUND_FOE_SHOT_LASER, true);
}

Core::Vector3f UberLaser::getPosition() const
{
    return Core::Vector3f(0.0f, 0.0f, 0.0f);
}


void UberLaser::update(double elapsed)
{
    elapsed *= _gameMode.getTimeWarp()->getTimeWarpFactor();

    _totalElapsed += (float)elapsed;

    Ptr<Universe::NodeDecal> pDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);

    if(_killed)
    {
        Core::Vector4f color = pDecal->getColor();
        color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        float width = Core::lerp(_width, 0.0f, Core::interpolate(_totalElapsed/LASER_NO_DAMAGE_TIME_OUT, Core::INTER_FADE_OUT));
        pDecal->setColor(color);
        pDecal->setSize(width, _height);
        Core::Vector4f glow = pDecal->getGlow();
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        pDecal->setGlow(glow);

        Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
        colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        _pNodeRefrac->setColor(colorRefrac);

        if(_totalElapsed > LASER_NO_DAMAGE_TIME_OUT)
            madeHit();
    }
    else if(_totalElapsed <= LASER_NO_DAMAGE_TIME_OUT)
    {
        Core::Vector4f color = pDecal->getColor();
        color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        float width = Core::lerp(0.0f, _width, Core::interpolate(_totalElapsed/LASER_NO_DAMAGE_TIME_OUT, Core::INTER_FADE_IN));
        pDecal->setColor(color);
        pDecal->setSize(width, _height);
        Core::Vector4f glow = pDecal->getGlow();
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        pDecal->setGlow(glow);
        
        _pNodeRefrac->setSize(_width * (2.0f + (_totalElapsed/LASER_NO_DAMAGE_TIME_OUT)), _height);
        Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
        colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f), _totalElapsed/LASER_NO_DAMAGE_TIME_OUT);
        _pNodeRefrac->setColor(colorRefrac);

        if(_totalElapsed > LASER_NO_DAMAGE_TIME_OUT)
        {
            madeHit();
            _fadeTimer = FADE_TIMER;
        }
    }
    else
    {
        if(_totalElapsed > LASER_NO_DAMAGE_TIME_OUT && _pCollisionShape == null)
        {
            _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());

            for(float ii=0.0f; ii<_height; ii += (0.35f * _width))
            {
                _pCollisionShape->addSphere(Core::Sphere(Core::Vector3f(0.0f, 0.0f, ii-(_height/2)), 0.45f * _width));
            }
        }

        float radiateFactor = 5.0f;

        float alpha = Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor));

        Core::Vector4f color = pDecal->getColor();
        color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.6f), alpha);
        pDecal->setColor(color);
        Core::Vector4f glow = pDecal->getGlow();
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), alpha);
        pDecal->setGlow(glow);
        
        _pNodeRefrac->setSize(_width* (2.0f + Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor))), _height);
        Core::Vector4f colorRefrac = _pNodeRefrac->getColor();
        colorRefrac.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.2f), alpha);
        _pNodeRefrac->setColor(colorRefrac);
    }

    if(_pCollisionShape != null)
    {
        const Core::List<Ptr<Foe> > & foes = _gameMode.getFoes();

        for(int32 iFoe=0; iFoe < foes.size(); iFoe++)
        {
            Foe & foe = *foes[iFoe];
            if(foe.getType() < UBER_BOSS)
                if(foe.collides(*_pCollisionShape))
                    foe.damage(float(elapsed) * 1000.0f, getDirection(), false);
        }
    }

    FoeShot::update(elapsed);
}

void UberLaser::fade(float alpha)
{
}

void UberLaser::stop()
{
    _totalElapsed = 0.0f;
    _killed = true;

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape());

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

void UberLaser::kill(bool outOfBoundaries)
{
    _fadeTimer = FADE_TIMER;
    stop();
}

}//namespace ChronoRage