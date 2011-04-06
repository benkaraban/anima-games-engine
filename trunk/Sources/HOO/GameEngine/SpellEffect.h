/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef GAMEENGINE_SPELLEFFECT_H_
#define GAMEENGINE_SPELLEFFECT_H_

#include <Core/Standard.h>
#include <GameEngine/Enums.h>
#include <GameEngine/GameEntity.h>

namespace GameEngine
{
/*
 * Un SpellEffect est une modification "atomique" du contexte de jeu réalisée par un sort.
 * Un SpellEffect est caractérisé par un type d'effet (ESpellEffectType) et une 
 * quantification. Si un effet a besoin de paramêtres qualicatifs, ceux-ci
 * devraient être considérés comme des effets différents et décomposés.
 *
 * Par exemple, un effet de resistance à une sphère spécifique devrait être 
 * décomposé en trois effets : RESIST_ENERGY, RESIST_LIFE, RESIST_SPIRIT.
 *
 * Note : dans le dictionnaire, les noeuds associés aux SpellEffects sont caractérisés
 * par l'identifiant SPELL_EFFECT_[SpellEffectType]_NAME.
 * Le SpellEffect "DAMAGE" sera donc associé à l'identifiant SPELL_EFFECT_DAMAGE_NAME
 */
class LM_API_HGE SpellEffect : public GameEntity
{
public:

    SpellEffect() :
        _effect(DO_NOTHING),
        _magnitude(0)
    {
    }

    SpellEffect(ESpellEffectType effect, int32 magnitude) :
        _effect(effect),
        _magnitude(magnitude)
    {
    }

    int32 getMagnitude() const
    {
        return _magnitude;
    }

    ESpellEffectType getEffect() const
    {
        return _effect;
    }

    const String & getName() const;

    virtual String getDescription(EDescriptionFormat format) const;

    virtual const String & getPrefix() const;

    static const String & getSpellEffectsPrefix();

protected:
    ESpellEffectType _effect;
    int32 _magnitude;
    static const String _sPREFIX;
};


}
#endif /* GAMEENGINE_SPELLEFFECT_H_ */