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
#include "Mine.h"

#include <Modes/GameMode.h>

namespace ChronoRage
{

const float MINE_SELF_DESTRUCTION = 15.0f;
const float MINE_EXPLODE_SIZE = 7.0f;

const float MINE_WIDTH = 3.0f;
const float MINE_HEIGHT= 3.0f;

Mine::Mine(const Ptr<Universe::World> & pWorld, const Ptr<Player> & pPlayer, GameMode & gameMode)
:   FoeShot(pWorld, pPlayer, gameMode),
    _width(MINE_WIDTH),
    _height(MINE_HEIGHT)
{}

void Mine::initialize(const Core::Vector3f & position)
{
    _totalElapsed = 0.0f;

    _pNodeFoeShot = _pWorld->createDecal(MINE_FILE, Renderer::DECAL_ADD);
    _pNodeFoeShot->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeFoeShot->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
    _pNodeFoeShot->setWorldPosition(position);

    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);
    pNodeDecal->setSize(_width, _height);
    pNodeDecal->setGlow(Core::Vector4f(0.0f, 0.1f, 0.0f, 0.5f));
    pNodeDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pCollisionShape = Ptr<Core::CollisionShape>(new Core::CollisionShape(Core::Sphere(Core::Vector3f(0.0f, 0.0f, 0.0f), _width/3)));
}

void Mine::update(double elapsed)
{
    elapsed *= _gameMode.getTimeWarp()->getTimeWarpFactor();
    
    _totalElapsed += float(elapsed);

    float radiateFactor = 5.0f;

    float alpha = Core::L_abs(Core::L_sin(_totalElapsed*radiateFactor));

    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);
    Core::Vector4f color = pNodeDecal->getColor();
    color.setLerp(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 1.0f, 0.6f), alpha);
    pNodeDecal->setColor(color);
    Core::Vector4f glow = pNodeDecal->getGlow();
    glow.setLerp(Core::Vector4f(0.0f, 0.1f, 0.0f, 0.5f), Core::Vector4f(0.0f, 0.1f, 0.0f, 0.2f), alpha);
    pNodeDecal->setGlow(glow);
    pNodeDecal->setSize(MINE_WIDTH + alpha, MINE_HEIGHT + alpha);

    if(_totalElapsed > MINE_SELF_DESTRUCTION && !hasHit())
    {
        madeHit();
    }

    if(hasHit() && _fadeTimer < 0.0f)
    {
        kill(false);
    }

    FoeShot::update(elapsed);
}

void Mine::fade(float alpha)
{
    Ptr<Universe::NodeDecal> pNodeDecal = LM_DEBUG_PTR_CAST<Universe::NodeDecal>(_pNodeFoeShot);
    pNodeDecal->setColor(Core::Vector4f(alpha, alpha, alpha, alpha));
    pNodeDecal->setGlow(Core::Vector4f(0.7f, 0.1f, 0.75f, 0.5f)*alpha);

    pNodeDecal->setSize(_width * (MINE_EXPLODE_SIZE * (1 - alpha)), _height * (MINE_EXPLODE_SIZE * (1 - alpha)));
}

}//namespace ChronoRage