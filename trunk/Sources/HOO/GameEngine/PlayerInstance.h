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
#ifndef GAMEENGINE_PLAYERINSTANCE_H_
#define GAMEENGINE_PLAYERINSTANCE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <GameEngine/Player.h>
#include <GameEngine/SpellInstance.h>
#include <GameEngine/Stuff.h>

namespace GameEngine
{
/* 
 * Cette classe contient toutes les informations contextuelles d'un joueur, relativement à la partie en cours.
 */
class LM_API_HGE PlayerInstance : public Player
{
    friend class GameContext;

public:

    PlayerInstance(const Player & p);
    PlayerInstance(const Ptr<PlayerInstance> & player,
        int32 health,
        int32 willpower,
        int32 mana[],
        int32 absorb[],
        int32 chaining[],
        int32 impair[],
        int32 nextTurnsToSee,
        bool seesOpponentDeck); // utilisé pour tests unitaires

    PlayerInstance(const Ptr<PlayerInstance> & player,
        int32 health,
        int32 mana[]); // utilisé pour tests unitaires

    bool hasSpellInDeck(int32 id) const;
    void addSpellInDeck(const Spell &);

    Ptr<SpellInstance> getSpellInDeck(int32 id) const;
    const Core::Map<int32, Ptr<SpellInstance> > & getDeck() const;
    int32 getHealth() const;
    int32 getWillpower() const;
    int32 getNextTurnToSee() const { return _nextTurnsToSee; }

    int32 getMana(ESphere) const;
    int32 getAbsorb(ESphere s) const { return _absorb[s]; }
    int32 getChaining(ESphere s) const { return _chaining[s]; }
    int32 getImpair(ESphere s) const { return _impair[s]; }

    bool canAffordSpell(const Ptr<SpellInstance> & spellInstance);
    int32 getSphereControl(ESphere sphere) const
    {
        return _mana[sphere];
    }

    /*void decrementCooldown();*/

    int32 getPlayerIndex() const { return _index; }

protected:
    Core::Map<int32, Ptr<SpellInstance> > _deck;
    int32   _health;
    int32   _willpower;
    int32   _mana[3];
    int32   _absorb[3];
    int32   _chaining[3];
    int32   _impair[3];
    bool    _seesOpponentDeck;
    int32   _nextTurnsToSee;
    int32   _index;
};
}

#endif/*GAMEENGINE_PLAYERINSTANCE_H_*/
