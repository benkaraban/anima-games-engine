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
#include <GameEngine/Player.h>

namespace GameEngine
{
Player::Player() :
    _maxHealth(100),
    _maxWillpower(20),
    _name("")
{
    _startingMana[SPHERE_ENERGY] = 5;
    _startingMana[SPHERE_SPIRIT] = 5;
    _startingMana[SPHERE_LIFE] = 5;
}

Player::Player(const String & name) :
    _maxHealth(100),
    _maxWillpower(20),
    _name(name)
{
    _startingMana[SPHERE_ENERGY] = 5;
    _startingMana[SPHERE_SPIRIT] = 5;
    _startingMana[SPHERE_LIFE] = 5;
}

Player::Player(const Player & p) :
    _ownedSpells(p._ownedSpells),
    _ownedStuffs(p._ownedStuffs),
    _maxHealth(p._maxHealth),
    _maxWillpower(p._maxWillpower),
    _name(p._name)
{
    _startingMana[SPHERE_ENERGY] = p._startingMana[SPHERE_ENERGY];
    _startingMana[SPHERE_SPIRIT] = p._startingMana[SPHERE_SPIRIT];
    _startingMana[SPHERE_LIFE] = p._startingMana[SPHERE_LIFE];
}


const String & Player::getName() const
{
    return _name;
}

bool Player::knowsSpell(const Spell * s)
{
    if(_ownedSpells[s->getId()] != NULL)
        return true;
    return false;

    //bool found = false;
    //Core::List<Ptr<Spell>>::iterator i = _ownedSpells.begin();

    //while(!found && i < _ownedSpells.end())
    //{
    //    if(*i == s)
    //        found = true;
    //    i++;
    //}

    //return found;
}

void Player::add(const Spell * spell)
{
    _ownedSpells[spell->getId()] = spell;
    //_ownedSpells.push_back(spell);
}

void Player::add(const Stuff * stuff)
{
    _ownedStuffs[stuff->getId()] = stuff;
    //_ownedStuffs.push_back(stuff);
}

}

