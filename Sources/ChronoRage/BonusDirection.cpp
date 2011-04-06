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
#include "BonusDirection.h"

namespace ChronoRage
{

const Core::String BONUS_DIRECTION_FILE = L"/ChronoRageData/bonus-arrow.tex";
const Core::String BONUS_TEXT_FILE = L"/ChronoRageData/bonus.tex";

BonusDirection::BonusDirection()
:   _pNodeDecal(null),
    _pNodeBonusText(null),
    _totalElapsed(0.0f),
    _bonusCarrier(true)
{
}

BonusDirection::~BonusDirection()
{
    if(_pNodeDecal != null)
    {
        _pNodeDecal->kill();
        _pNodeDecal = null;
    }

    if(_pNodeBonusText != null)
    {
        _pNodeBonusText->kill();
        _pNodeBonusText = null;
    }
}

void BonusDirection::initialize(const Ptr<Universe::World> & pWorld)
{
    _pWorld = pWorld;
    _pNodeDecal = _pWorld->createDecal(BONUS_DIRECTION_FILE, Renderer::DECAL_ADD);
    _pNodeDecal->setSize(6.0f, 6.0f);

    _pNodeDecal->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pNodeDecal->setVisible(false);
    _pNodeDecal->setColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    _pNodeDecal->setGlow(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.5f));

    _pNodeBonusText = _pWorld->createDecal(BONUS_TEXT_FILE, Renderer::DECAL_ADD);
    _pNodeBonusText->setSize(3.0f, 3.0f);

    _pNodeBonusText->setUpVector(Core::Vector3f(0.0f, 0.0f, -1.0f));
    _pNodeBonusText->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pNodeBonusText->setVisible(false);
    _pNodeBonusText->setColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f));
    _pNodeBonusText->setGlow(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.3f));
    _pNodeBonusText->setWorldDirection(Core::Vector3f(0.0f, 1.0f, 0.0f));
}

void BonusDirection::update(double elapsed)
{
    _totalElapsed += (float)elapsed;

    float blinkFactor = 10.0f;

    float alpha = (Core::L_sin(_totalElapsed * blinkFactor)+1)/2;

    Core::Vector4f color = _pNodeDecal->getColor();
    Core::Vector4f glow = _pNodeDecal->getGlow();
    if(_bonusCarrier)
    {
        color.setLerp(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), Core::Vector4f(0.0f, 1.0f, 1.0f, 1.0f), alpha);
        glow.setLerp(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.3f), Core::Vector4f(0.0f, 1.0f, 1.0f, 0.3f), alpha);
    }
    else
    {
        color.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 0.0f, 1.0f), alpha);
        glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), Core::Vector4f(1.0f, 1.0f, 0.0f, 0.3f), alpha);
    }

    _pNodeDecal->setColor(color);
    _pNodeDecal->setGlow(glow);
    _pNodeBonusText->setColor(color);
    _pNodeBonusText->setGlow(glow);
}

void BonusDirection::setPosition(const Core::Vector3f & position)
{
    _pNodeDecal->setWorldPosition(position);
    _pNodeBonusText->setWorldPosition(position);
}

void BonusDirection::setDirection(const Core::Vector3f & direction)
{
    _pNodeDecal->setWorldDirection(direction);
}

void BonusDirection::setVisible(bool visible)
{
    _pNodeDecal->setVisible(visible);
    _pNodeBonusText->setVisible(visible);
}

void BonusDirection::setBonusCarrierColor(bool bonusCarrier)
{
    _bonusCarrier = bonusCarrier;
}

}