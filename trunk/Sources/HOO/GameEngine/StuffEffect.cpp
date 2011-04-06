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
#include <GameEngine/StuffEffect.h>

namespace GameEngine
{
const String StuffEffect::_sPREFIX(L"STUFF_EFFECT_");

StuffEffect::StuffEffect(EStuffEffectType effect = STUFF_DO_NOTHING, int32 magnitude = 0, EStuffCondition condition = ALWAYS) :
    _effect(effect),
    _magnitude(magnitude),
    _condition(condition)
{
}

const String & StuffEffect::getPrefix() const
{
    return _sPREFIX;
}

const String & StuffEffect::getStuffsEffectsPrefix()
{
    return _sPREFIX;
}

EStuffEffectType StuffEffect::getEffect() const
{
    return _effect;
}

const String & StuffEffect::getName() const
{
    return (*_spDictionary)[this->getPrefix() + GameEngine::toString(_effect) + L"_NAME"];
}

String StuffEffect::getDescription(EDescriptionFormat format) const
{
    String fullid = GameEngine::toString(format);
    fullid.toLower();
    fullid = this->getPrefix() + GameEngine::toString(_effect) + L"_" + fullid;
    return (*_spDictionary)[fullid];
}
}