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
#include "Wasp.h"

#include <Core/TGA.h>

#include <Foes/Shots/Hornet.h>
#include <Modes/GameMode.h>

namespace ChronoRage
{
Ptr<VoxelSpriteTemplate> Wasp::spTemplate;

Wasp::Wasp(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeCommon(WASP, pWorld, pPlayer, gameMode),
    _lastShot(WASP_SHOT_TIMER / 2.0f),
    _angle1( 3.0f * f_PI_DIV_4),
    _angle2(-3.0f * f_PI_DIV_4)
{
    _minRange = 10.0f;
    _maxRange = 25.0f;
    _speed = 10.0f;
    _radialSpeed = 2.0f;
    _maxHealthPoint = _healthPoint = 4.0f;
}

Wasp::~Wasp()
{}

void Wasp::initialize(const Core::Vector3f & position)
{
    FoeCommon::initialize(position);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), 1.3333f)));
}

Ptr<VoxelSpriteTemplate> Wasp::getTemplate() const
{
    if(spTemplate == null)
    {
        Ptr<Core::InputStream> pInput = _pWorld->getRessourcesPool()->getVFS()->readFile(WASP_FILE);
        Ptr<Core::Bitmap> pBitmap = Core::loadTGA(*pInput);
        spTemplate = Ptr<VoxelSpriteTemplate>(new VoxelSpriteTemplate(pBitmap, 0.2f, 0.05f));
    }

    return spTemplate;
}

void Wasp::updateActive(double elapsed)
{
    _lastShot = std::max(0.0f, _lastShot - (float)elapsed);

    if(_lastShot <= 0)
    {
        _pVoxelSprite->blink(WASP_BLINK_TIME, WASP_BLINK_NUMBER);

        Ptr<Hornet> pHornet1(new Hornet(_pWorld, _pPlayer, _gameMode));
        pHornet1->initialize(getPosition());
        Ptr<Hornet> pHornet2(new Hornet(_pWorld, _pPlayer, _gameMode));
        pHornet2->initialize(getPosition());

        Core::Matrix4f rotationMatrix1(Core::ROTATION, _angle1, Core::Vector3f(0.0f, 0.0f, 1.0f));
        Core::Matrix4f rotationMatrix2(Core::ROTATION, _angle2, Core::Vector3f(0.0f, 0.0f, 1.0f));

        Core::Vector3f shotDirection1 = rotationMatrix1.apply(_pNodeFoe->getWorldDirection());
        Core::Vector3f shotDirection2 = rotationMatrix2.apply(_pNodeFoe->getWorldDirection());
        
        pHornet1->setDirection(shotDirection1);
        pHornet2->setDirection(shotDirection2);

        _gameMode.addFoeShot(pHornet1);
        _gameMode.addFoeShot(pHornet2);

        _gameMode.playSound(SOUND_FOE_SHOT_HORNET, false);

        _lastShot = WASP_SHOT_TIMER;
    }

    FoeCommon::updateActive(elapsed);
}

}//namespace ChronoRage