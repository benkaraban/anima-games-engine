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
#ifndef GAMEENGINE_STUFFEFFECT_H_
#define GAMEENGINE_STUFFEFFECT_H_

#include <Core/Standard.h>
#include <GameEngine/Enums.h>
#include <GameEngine/GameEntity.h>

namespace GameEngine
{
/*
 * Un StuffEffect est une modification du contexte de jeu réalisée par un équipement.
 * Contrairement aux effets de sorts, les StuffEffect ne sont pas déclenchés par des actions
 * du joueur mais par les conditions définies par l'énumération EStuffCondition.
 */
class LM_API_HGE StuffEffect : public GameEntity
{
public:

    StuffEffect(EStuffEffectType effect, int32 magnitude, EStuffCondition condition);

    EStuffEffectType getEffect() const;

    virtual const String & getPrefix() const;

    static const String & getStuffsEffectsPrefix();

    virtual const String & getName() const;

    virtual String getDescription(EDescriptionFormat format) const;

protected:

    static const String _sPREFIX;

    EStuffEffectType    _effect;
    int32               _magnitude;
    EStuffCondition     _condition;
};


}
#endif /* GAMEENGINE_STUFFEFFECT_H_ */
