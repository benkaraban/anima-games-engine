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
#ifndef GAMEENGINE_PLAYERACTION_H_
#define GAMEENGINE_PLAYERACTION_H_

#include <Core/Standard.h>
#include <GameEngine/SpellInstance.h>

namespace GameEngine
{
LM_API_HGE LM_ENUM_3(
    EPlayerActionType,
    CAST,
    BID,
    PASS
)

struct PlayerAction : public Core::Object
{
    PlayerAction(Ptr<PlayerInstance> player, EPlayerActionType type, Ptr<SpellInstance> spell = null, int32 quantity = 0) :
        _player(player),
        _type(type),
        _spell(spell),
        _quantity(quantity)
    {}

    static PlayerAction bid(Ptr<PlayerInstance> player, int32 quantity)
    {
        return PlayerAction(player, BID, null, quantity);
    }

    static PlayerAction cast(Ptr<PlayerInstance> player, Ptr<SpellInstance> s)
    {
        return PlayerAction(player, CAST, s, 0);
    }

    static PlayerAction pass(Ptr<PlayerInstance> player)
    {
        return PlayerAction(player, PASS);
    }

    Ptr<PlayerInstance> _player;
    EPlayerActionType _type;
    Ptr<SpellInstance>  _spell;
    int32       _quantity;
};

}
#endif /* GAMEENGINE_PLAYERACTION_H_ */
