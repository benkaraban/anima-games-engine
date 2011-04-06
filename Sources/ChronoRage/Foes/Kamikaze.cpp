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
#include "Kamikaze.h"

#include <Core/TGA.h>
#include <Core/Math/Random.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Kamikaze::spTemplate;
Ptr<VoxelSpriteTemplate> Kamikaze::spTemplate2;
Ptr<VoxelSpriteTemplate> Kamikaze::spTemplate3;

Kamikaze::Kamikaze(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(KAMIKAZE, pWorld, pPlayer, gameMode)
{
    _minRange = 20.0f;
    _maxRange = 30.0f;
    _speed = 20.0f;
    _radialSpeed = 2.0f;
    _maxHealthPoint = _healthPoint = 4.0f;
    _tolerance = 0.0f;
}

Kamikaze::~Kamikaze()
{}

void Kamikaze::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.3333f)));

    _approachPointSave = _approachPoint;
    _distance = _approachPointSave.getLengthFast();

    _approachPointSave.normalizeFast();
    _approachPointSave *= (_distance/2.0f);
}

Ptr<VoxelSpriteTemplate> Kamikaze::getTemplate() const
{
    int32 which = Core::irand(0, 3);
    if(which == 0)
    {
        if(spTemplate == null)
        {
            Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(KAMIKAZE_FILE);
            Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
            spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
        }
        return spTemplate;
    }
    else if(which == 1)
    {
        if(spTemplate2 == null)
        {
            Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(KAMIKAZE_FILE2);
            Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
            spTemplate2 = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
        }
        return spTemplate2;
    }
    else
    {
        if(spTemplate3 == null)
        {
            Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(KAMIKAZE_FILE3);
            Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
            spTemplate3 = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
        }
        return spTemplate3;
    }
}

void Kamikaze::updateActive(double elapsed)
{
    Core::Vector3f distanceVector = _pPlayer->getPosition() - getPosition();

    float distance = 0.0f;
    if(!distanceVector.isZero())
        distance = distanceVector.getLengthFast();

    if(distance <= _distance)
    {
        _approachPoint.setLerp(Core::Vector3f(0.0f, 0.0f, 0.0f), _approachPointSave, distance/_distance);
    }

    FoeCommon::updateActive(elapsed);
}

}//namespace ChronoRage