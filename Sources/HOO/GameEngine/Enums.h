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
#ifndef GAMEENGINE_ENUMS_H_
#define GAMEENGINE_ENUMS_H_

#include <Core/Enum.h>
#include <Core/Standard.h>

namespace GameEngine
{
/*
 * Un GameEffectType est un type de modification du contexte de jeu effectuée par le contexte lui-même, en fonction de son état.
 */
LM_API_HGE LM_ENUM_7(
    EGameEffectType,
    GAME_EFFECT_APPLY_MANA_COST,
    GAME_EFFECT_APPLY_COOLDOWN,
    GAME_EFFECT_REDUCE_MANA_COST,
    GAME_EFFECT_REDUCE_COOLDOWN,
    GAME_EFFECT_GENERATE_MANA,
    GAME_EFFECT_P1_WIN_MANA,
    GAME_EFFECT_P2_WIN_MANA
)

/*
 * Un ESpellEffectType est un type de modification du contexte de jeu effectuée par un sort.
 */
LM_API_HGE LM_ENUM_22(
    ESpellEffectType,
    DO_NOTHING,
    DAMAGE,
    HEAL,
    ABSORB,
    BLOCK,
    VANTAGE,
    DIVINATION,
    ANTICIPATION,
    CHAINING,
    IMPAIR,
    SURGE,
    REQUIRE_ENERGY,
    REQUIRE_SPIRIT,
    REQUIRE_LIFE,
    ENERGY_ASSAULT,
    SPIRIT_ASSAULT,
    LIFE_ASSAULT,
    ROTATION_CW,
    ROTATION_CCW,
    LIFE_BURN,
    ENERGY_BURN,
    SPIRIT_BURN
)
ESpellEffectType toSpellEffectType(const String & s);

/*
 *
 */
LM_API_HGE LM_ENUM_6(
    EStuffSlot,
    POWER_SYMBOL,
    CLOTHES,
    HEAD,
    AURA,
    JEWELS,
    COSMETIC
)

EStuffSlot toStuffSlot(const String & s);


/*
 * Un EStuffEffectType est un type de modification du contexte de jeu effectuée par un équipement.
 */
LM_API_HGE LM_ENUM_5(
    EStuffEffectType,
    STUFF_DO_NOTHING,
    RAISES_MANA_ENERGY,
    RAISES_MANA_SPIRIT,
    RAISES_MANA_LIFE,
    RAISES_WILLPOWER
)

EStuffEffectType toStuffEffectType(const String & s);

/*
 * Enumération pour les conditions d'activation des effets des équipements.
 */
 LM_API_HGE LM_ENUM_10(
    EStuffCondition,
    NEVER,
    ALWAYS,
    PLAYER_CASTS,
    PLAYER_CASTS_SPELL,
    PLAYER_CASTS_COUNTER,
    PLAYER_CASTS_TRICK,
    PLAYER_CASTS_RETRIBUTION,
    PLAYER_CASTS_ENERGY,
    PLAYER_CASTS_LIFE,
    PLAYER_CASTS_SPIRIT
 )
 EStuffCondition toStuffCondition(const String & s);


/*
 * Enumération pour les sphères
 */
LM_API_HGE LM_ENUM_3(
    ESphere,
    SPHERE_ENERGY,
    SPHERE_SPIRIT,
    SPHERE_LIFE
)

/*
 * Enumération pour les types de sorts
 */
LM_API_HGE LM_ENUM_4(
    ESpellType,
    SPELLTYPE_SPELL,
    SPELLTYPE_RETRIBUTION,
    SPELLTYPE_TRICK,
    SPELLTYPE_COUNTER
)


/*
 * Enumération pour le format des descriptions.
 */
LM_API_HGE LM_ENUM_2(
    EDescriptionFormat,
    TOOLTIP,
    DETAILED
)
}
#endif /* GAMEENGINE_ENUMS_H_ */
