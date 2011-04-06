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
#include <GameEngine/PlayerInstance.h>

namespace GameEngine
{
PlayerInstance::PlayerInstance(const Player & p) :
    Player(p)
{
    _health = _maxHealth;
    _willpower = _maxWillpower;
    for(int i =0; i < 3; i++)
    {
        _mana[i] = _startingMana[i];
        _absorb[i] = 0;
        _chaining[i] = 0;
        _impair[i] = 0;
    }

    _seesOpponentDeck = false;
    _nextTurnsToSee = 0;
    _index = 0;
}

bool PlayerInstance::hasSpellInDeck(int32 id) const
{
    return _deck.find(id) != _deck.end();
}

void PlayerInstance::addSpellInDeck(const Spell & spell)
{
    _deck[spell.getId()] = Ptr<SpellInstance>(new SpellInstance(spell));
}

Ptr<SpellInstance> PlayerInstance::getSpellInDeck(int32 id) const
{
    Core::Map<int32, Ptr<SpellInstance> >::const_iterator iSpell = _deck.find(id);

    if(iSpell != _deck.end())
        return iSpell->second;
    else
        return null;
}

int32 PlayerInstance::getHealth() const
{
    return _health;
}

int32 PlayerInstance::getWillpower() const
{
    return _willpower;
}

int32 PlayerInstance::getMana(ESphere sphere) const
{
    return _mana[sphere];
}
bool PlayerInstance::canAffordSpell(const Ptr<SpellInstance> & spellInstance)
{
    // TODO : tester si le sort possède un effet de lancement requiérant une maitrise minimale
    // dans une sphère donnée
    //return _mana[spellInstance->getSphere()] >= spellInstance->getModifiedCost();
    return true;
}

const Core::Map<int32, Ptr<SpellInstance> > & PlayerInstance::getDeck() const
{
    return _deck;
}

//void PlayerInstance::decrementCooldown()
//{
//    for(Core::Map<int32, Ptr<SpellInstance> >::const_iterator iSpell=_deck.begin(); iSpell != _deck.end(); ++iSpell)
//        iSpell->second->decrementCooldown();
//}

PlayerInstance::PlayerInstance(const Ptr<PlayerInstance> & p,
        int32 health,
        int32 willpower,
        int32 mana[],
        int32 absorb[],
        int32 chaining[],
        int32 impair[],
        int32 nextTurnsToSee,
        bool seesOpponentDeck) :
        Player(*p),
        _health(health),
        _willpower(willpower),
        _seesOpponentDeck(seesOpponentDeck),
        _nextTurnsToSee(nextTurnsToSee),
        _index(0)
{
    _deck = p->getDeck();
    for(int i = 0; i < 3; i++)
    {
        _mana[i] = mana[i];
        _chaining[i] = chaining[i];
        _absorb[i] = absorb[i];
        impair[i] = impair[i];
    }
}

PlayerInstance::PlayerInstance(const Ptr<PlayerInstance> & p,
        int32 health,
        int32 mana[]) :
        Player(*p),
        _health(health),
        _index(0)
{
    _deck = p->getDeck();
    for(int i = 0; i < 3; i++)
        _mana[i] = mana[i];
}

}
