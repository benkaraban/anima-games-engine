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
#ifndef GAMEENGINE_SPELLINSTANCE_H_
#define GAMEENGINE_SPELLINSTANCE_H_

#include <Core/Standard.h>
#include <GameEngine/Spell.h>

namespace GameEngine
{
/*
 * Un SpellInstance est un sort contenant des informations contextuelles relatives
 * au déroulement de la partie, telles que le cooldown restant, les modificateurs
 * de coût, etc.
 */
class LM_API_HGE SpellInstance : public Spell
{
public:
    /* Crée une instance du sort passé en paramètre */
    SpellInstance(const Spell &);

    /* Crée une instance du sort dont l'id est passé en paramètre */
    //SpellInstance(int32 id);

    /*bool isOnCooldown();*/
    /*int32 getRemainingCooldown() const { return _remainingCooldown; }*/


    /*void setRemainingCooldown(int32);*/
    /*void setBonusOnCost(int32);
    void setMalusOnCost(int32);
	int32 getBonusOnCost() const
	{
		return _bonusOnCost;
	}
	int32 getMalusOnCost() const
	{
		return _malusOnCost;
	}*/
    /*void setOnCooldown();
    int32 getModifiedCost();*/

    /*void decrementCooldown();*/

protected:
    /*int32 _remainingCooldown;*/
    /*int32 _bonusOnCost;
    int32 _malusOnCost;*/
};
}
#endif /* GAMEENGINE_SPELLINSTANCE_H_ */