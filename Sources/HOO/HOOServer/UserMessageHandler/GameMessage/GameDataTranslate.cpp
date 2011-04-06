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
#include <Core/Logger.h>
#include "GameDataTranslate.h"

namespace HOOServer
{
Ptr<GameEngine::PlayerInstance> net2Game(const HOOServer::UserAccount & account, const GameEngine::GameLibrary * pLibrary)
{
    GameEngine::Player player(account.login);

    for(int32 ii=0; ii < account.equipedStuff.size(); ii++)
    {
        const GameEngine::Stuff * pStuff = pLibrary->getStuff(account.equipedStuff[ii]);

        if(pStuff == NULL)
            ERR << L"Player '" << account.login << L"' is equiped with an undefined stuff ID " 
                << Core::toString(account.equipedStuff[ii]) << L", stuff ignored\n";
        else
            player.add(pStuff);
    }

    Ptr<GameEngine::PlayerInstance> pResult(new GameEngine::PlayerInstance(player));

    for(int32 ii=0; ii < account.equipedSpells.size(); ii++)
    {
        const GameEngine::Spell * pSpell = pLibrary->getSpell(account.equipedSpells[ii]);

        if(pSpell == NULL)
            ERR << L"Player '" << account.login << L"' is equiped with an undefined spell ID " 
                << Core::toString(account.equipedSpells[ii]) << L", spell ignored\n";
        else
            pResult->addSpellInDeck(*pSpell);
    }

    return pResult;
}

void game2Net(int32 playerId, const GameEngine::GameContext & game, HOOUserProtocol::GameStatus & status)
{
    if(playerId == 1)
    {
        game2Net(*game.getP1(), game, status.player);
        game2Net(*game.getP2(), game, status.opponent);
    }
    else if(playerId == 2)
    {
        game2Net(*game.getP1(), game, status.opponent);
        game2Net(*game.getP2(), game, status.player);
    }

    LM_ASSERT(!(status.opponent.hasVantage && status.player.hasVantage));
}

void game2Net(const GameEngine::PlayerInstance & player, const GameEngine::GameContext & game, HOOUserProtocol::PlayerStatus & status)
{
    status.health = player.getHealth();
    status.willpower = player.getWillpower();
    status.nextTurnsToSee = player.getNextTurnToSee();
    //status.hasVantage = (game.getVantageIndex() == player.getPlayerIndex());
    status.hasVantage = false;

    for(int32 ii=0; ii < 3; ii++)
    {
        GameEngine::ESphere s((GameEngine::ESphere)ii);

        status.spheres[ii].mana = player.getMana(s);
        status.spheres[ii].absorb = player.getAbsorb(s);
        status.spheres[ii].chaining = player.getChaining(s);
        status.spheres[ii].impair = player.getImpair(s);
    }

    status.cooldowns.clear();
    /*const Core::Map<int32, Ptr<GameEngine::SpellInstance> > & deck = player.getDeck();

    for(Core::Map<int32, Ptr<GameEngine::SpellInstance> >::const_iterator iDeck=deck.begin(); iDeck != deck.end(); ++iDeck)
    {
        HOOUserProtocol::SpellCooldown sc;

        if(iDeck->second->isOnCooldown())
        {
            sc.spellId = iDeck->first;
            sc.cooldown = iDeck->second->getRemainingCooldown();

            status.cooldowns.push_back(sc);
        }
    }*/
}

void game2Net(const GameEngine::PlayerInstance & player, const GameEngine::GameContext & game, HOOUserProtocol::OpponentStatus & status)
{
    status.health = player.getHealth();
    /*status.hasVantage = (game.getVantageIndex() == player.getPlayerIndex());*/
    for(int32 ii=0; ii < 3; ii++)
    {
        GameEngine::ESphere s((GameEngine::ESphere)ii);

        status.spheres[ii].mana = player.getMana(s);
        status.spheres[ii].absorb = player.getAbsorb(s);
        status.spheres[ii].chaining = player.getChaining(s);
        status.spheres[ii].impair = player.getImpair(s);
    }
}

void game2Net(const GameEngine::PlayerAction & action,
              HOOUserProtocol::ActionCommand & command)
{
    switch(action._type)
    {
    case GameEngine::CAST:
        command.actionType = 0;
        command.actionValue = action._spell->getId();
        break;
    case GameEngine::BID:
        command.actionType = 1;
        command.actionValue = action._quantity;
        break;
    case GameEngine::PASS:
        command.actionType = 2;
        command.actionValue = 0;
        break;
    }
}


void game2Net(const Core::List<Ptr<GameEngine::PlayerAction> > & actions, 
              Core::List<HOOUserProtocol::ActionCommand> & commands)
{
    commands.resize(actions.size());

    for(int32 ii=0; ii < commands.size(); ii++)
        game2Net(*actions[ii], commands[ii]);
}

}//namespace HOOServer
