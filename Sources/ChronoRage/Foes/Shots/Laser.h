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
#ifndef CHRONORAGE_LASER_H_
#define CHRONORAGE_LASER_H_

#include <Foes/Shots/FoeShot.h>

namespace ChronoRage
{

const String LASER_FILE = L"/ChronoRageData/Foes/Shots/laser.tex";
const String UBER_LASER_FILE = L"/ChronoRageData/Foes/Shots/uberlaser.tex";
const String LASER_REFRAC_FILE = L"/ChronoRageData/Foes/Shots/laser-refrac.tex";
const float LASER_NO_DAMAGE_TIME_OUT = 0.5f;

class LM_API_CHR Laser : public FoeShot, public Core::Object
{
public:
    Laser(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);

    virtual ~Laser();

    virtual void initialize(const Core::Vector3f & position);
    virtual Core::Vector3f getPosition() const;

    virtual void update(double elapsed);

    virtual void fade(float alpha);

    virtual void kill(bool outOfBoundaries);

private:
    float   _width;
    float   _height;
    float   _totalElapsed;

    Ptr<Universe::NodeDecal> _pNodeRefrac;
    Ptr<Universe::NodeSoundSource> _pSoundloadingLaser;
    Ptr<Universe::NodeSoundSource> _pSoundLaser;
};

class LM_API_CHR UberLaser : public FoeShot, public Core::Object
{
public:
    UberLaser(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode);

    virtual ~UberLaser();

    virtual void initialize(const Core::Vector3f & position);
    virtual Core::Vector3f getPosition() const;

    virtual void update(double elapsed);

    virtual void fade(float alpha);

    virtual void kill(bool outOfBoundaries);
    virtual void stop();

private:
    float   _width;
    float   _height;
    float   _totalElapsed;
    bool    _killed;

    Ptr<Universe::NodeDecal> _pNodeRefrac;
    Ptr<Universe::NodeSoundSource> _pSoundloadingLaser;
    Ptr<Universe::NodeSoundSource> _pSoundLaser;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_LASER_H_*/