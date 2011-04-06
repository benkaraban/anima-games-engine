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
#include "Cruiser.h"

#include <Core/TGA.h>

#include <Foes/Shots/PlasmaTorpedo.h>
#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Cruiser::spTemplate;

Cruiser::Cruiser(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(CRUISER, pWorld, pPlayer, gameMode),
    _lastShot(CRUISER_SHOT_TIMER / 2.0f),
    _shotNumber(0)
{
    _minRange = 20.0f;
    _maxRange = 40.0f;
    _speed = 7.0f;
    _radialSpeed = 2.0f;
    _maxHealthPoint = _healthPoint = 64.0f;
}

Cruiser::~Cruiser()
{}

void Cruiser::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.3333f)));
}

Ptr<VoxelSpriteTemplate> Cruiser::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(CRUISER_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Cruiser::updateActive(double elapsed)
{
    _lastShot = std::max(0.0f, _lastShot - (float)elapsed);

    if(_lastShot <= 0)
    {
        _pVoxelSprite->blink(CRUISER_BLINK_TIME, CRUISER_BLINK_NUMBER);

        Ptr<PlasmaTorpedo> pPlasmaTorpedo(new PlasmaTorpedo(_pWorld, _pPlayer, _gameMode));
        pPlasmaTorpedo->initialize(getPosition());
        _gameMode.addFoeShot(pPlasmaTorpedo);
        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _lastShot = CRUISER_SHOT_TIMER;
    }

    FoeCommon::updateActive(elapsed);
}

}//namespace ChronoRage