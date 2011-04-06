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
#include "FoeDirection.h"

namespace ChronoRage
{

const Core::String FOE_DIRECTION_FILE = L"/ChronoRageData/incoming.tex";

FoeDirection::FoeDirection()
:   _pNodeDecal(null),
    _totalElapsed(0.0f)
{
}

FoeDirection::~FoeDirection()
{
    if(_pNodeDecal != null)
    {
        _pNodeDecal->kill();
        _pNodeDecal = null;
    }
}

void FoeDirection::initialize(const Ptr<Universe::World> & pWorld)
{
    _pWorld = pWorld;
    _pNodeDecal = _pWorld->createDecal(FOE_DIRECTION_FILE, Renderer::DECAL_ADD);
    _pNodeDecal->setSize(3.0f, 3.0f);

    _pNodeDecal->setUpVector(Core::Vector3f(0.0f, 0.0f, 1.0f));
    _pNodeDecal->setDecalNormal(Renderer::DECAL_NORMAL_Y);

    _pNodeDecal->setVisible(false);
    _pNodeDecal->setColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
    _pNodeDecal->setGlow(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.5f));
}

void FoeDirection::update(double elapsed)
{
    _totalElapsed += (float)elapsed;

    float blinkFactor = 10.0f;

    float alpha = (Core::L_sin(_totalElapsed * blinkFactor)+1)/2;

    Core::Vector4f color = _pNodeDecal->getColor();
    color.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), Core::Vector4f(1.0f, 1.0f, 0.0f, 1.0f), alpha);
    _pNodeDecal->setColor(color);
    Core::Vector4f glow = _pNodeDecal->getGlow();
    glow.setLerp(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.3f), Core::Vector4f(1.0f, 1.0f, 0.0f, 0.3f), alpha);
    _pNodeDecal->setGlow(glow);
}

void FoeDirection::setPosition(const Core::Vector3f & position)
{
    _pNodeDecal->setWorldPosition(position);
}

void FoeDirection::setDirection(const Core::Vector3f & direction)
{
    _pNodeDecal->setWorldDirection(direction);
}

void FoeDirection::setVisible(bool visible)
{
    _pNodeDecal->setVisible(visible);
}

}