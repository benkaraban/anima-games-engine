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
#include <GameEngine/GameContext.h>
#include <Core/Math/Random.h>
#include <Core/Math/MathTools.h>


namespace GameEngine
{
const int32 GameContext::_cMAX_BID(4);
const int32 GameContext::_cMAX_SPHERE_CONTROL(10);


GameContext::GameContext()
{
}

GameContext::GameContext(const Ptr<PlayerInstance> & pP1, const Ptr<PlayerInstance> & pP2)
{
    init(pP1, pP2);
}

void GameContext::init(const Ptr<PlayerInstance> & pP1, const Ptr<PlayerInstance> & pP2)
{
    //_state = WAITING_FOR_BIDS;
    _state = P1_ATTACK;
    _first = pP1;
    _currentAttackingSpell = null;
    _currentDefendingSpell = null;
    _turn = 1;

    _players[PLAYER1] = pP1;
    _players[PLAYER2] = pP2;

    pP1->_index = 1;
    pP2->_index = 2;

    _lastBidWinnerIndex = 0;

    generateMana();
}

bool GameContext::verifyValidity(const PlayerAction & action)
{
    bool result = false;

    switch(_state)
    {
    case WAITING_FOR_BIDS:
        result = action._type == BID && action._quantity <= _cMAX_BID ;
        break;
    case WAITING_FOR_P1_BID:
        result = action._type == BID && action._player == _players[PLAYER1] && action._quantity <= _cMAX_BID;
        break;
    case WAITING_FOR_P2_BID:
        result = action._type == BID && action._player == _players[PLAYER2] && action._quantity <= _cMAX_BID;
        break;
    case P1_ATTACK:
    case P2_DEFEND:
    case P2_ATTACK:
    case P1_DEFEND:
        result = (action._type == CAST && canCastSpell(action._player, action._spell))
                || (action._type == PASS && action._player == _currentPlayer);
        break;

    case P1_WIN:
    case P2_WIN:
    case DRAW:
        result = false;
        break;
    }
    return result;
}

bool GameContext::applyPlayerAction(const PlayerAction & action, IGameContextObs * pObs)
{
    if(verifyValidity(action))
    {
        switch(action._type)
        {
        case CAST:
            //TODO : appliquer les modificateurs de maitrise
            //action._player->_mana[action._spell->getSphere()] -= action._spell->getCost() + action._player->_impair[action._spell->getSphere()];
            action._player->_mana[SPHERE_ENERGY] += action._spell->getSphereControl(SPHERE_ENERGY);
            action._player->_mana[SPHERE_LIFE] += action._spell->getSphereControl(SPHERE_LIFE);
            action._player->_mana[SPHERE_SPIRIT] += action._spell->getSphereControl(SPHERE_SPIRIT);

            if(action._spell->getType() == SPELLTYPE_COUNTER)
            {
                action._player->_mana[SPHERE_ENERGY] -= _currentAttackingSpell->getMagnitudeOfEffect(ENERGY_ASSAULT);
                action._player->_mana[SPHERE_SPIRIT] -= _currentAttackingSpell->getMagnitudeOfEffect(SPIRIT_ASSAULT);
                action._player->_mana[SPHERE_LIFE] -= _currentAttackingSpell->getMagnitudeOfEffect(LIFE_ASSAULT);
            }

            action._player->_mana[SPHERE_LIFE] = Core::clamp(0, _cMAX_SPHERE_CONTROL, action._player->_mana[SPHERE_LIFE]);
            action._player->_mana[SPHERE_ENERGY] = Core::clamp(0, _cMAX_SPHERE_CONTROL, action._player->_mana[SPHERE_ENERGY]);
            action._player->_mana[SPHERE_SPIRIT] = Core::clamp(0, _cMAX_SPHERE_CONTROL, action._player->_mana[SPHERE_SPIRIT]);

            for(int32 iSphere=0; iSphere < 3; iSphere++)
            {
                action._player->_chaining[iSphere] = 0;
                action._player->_impair[iSphere] = 0;
            }

            if(pObs != NULL)
                pObs->onContextUpdate(this);

            switch(_state)
            {
                case P1_ATTACK:
                    _currentAttackingSpell = action._spell;
                    _state = P2_DEFEND;
                    _currentPlayer = _players[PLAYER2];
                    break;
                case P2_ATTACK:
                    _currentAttackingSpell = action._spell;
                    _state = P1_DEFEND;
                    _currentPlayer = _players[PLAYER1];
                    break;
                case P2_DEFEND:
                case P1_DEFEND:
                    _currentDefendingSpell = action._spell;
                    resolveTurn();
                    break;
                default:
                    break;
            }
            break;
        case BID:
            switch(_state)
            {
                case WAITING_FOR_BIDS:
                    if(action._player == _players[PLAYER1])
                    {
                        _bids[PLAYER1] =  action._quantity;
                        _state = WAITING_FOR_P2_BID;
                    }
                    else
                    {
                        _bids[PLAYER2] =  action._quantity;
                        _state = WAITING_FOR_P1_BID;
                    }
                    break;
                case WAITING_FOR_P1_BID:
                    _bids[PLAYER1] =  action._quantity;
                    resolveBids();
                    break;
                case WAITING_FOR_P2_BID:
                    _bids[PLAYER2] =  action._quantity;
                    resolveBids();
                    break;
                default:
                    break;
            }
            break;
        case PASS:
            switch(_state)
            {
                case P1_ATTACK:
                    _currentPlayer = _players[PLAYER2];
                    _currentAttackingSpell = null;
                    _state = P2_DEFEND;
                    break;
                case P2_ATTACK:
                    _currentPlayer = _players[PLAYER1];
                    _currentAttackingSpell = null;
                    _state = P1_DEFEND;
                    break;
                case P1_DEFEND:
                case P2_DEFEND:
                    _currentDefendingSpell = null;
                    resolveTurn();
                    break;
                default:
                    break;
            }
            break;
        }
        return true;
    }
    return false;
}

EGameState GameContext::getState()
{
    return _state;
}

bool GameContext::isBidInProgress() const
{
    return _state == WAITING_FOR_BIDS || _state == WAITING_FOR_P1_BID || _state == WAITING_FOR_P2_BID;
}


void GameContext::resolveBids()
{
    Ptr<PlayerInstance> winner = null;

    if(_bids[PLAYER1] > _bids[PLAYER2])
    {
        winner = _players[PLAYER1];
        _lastBidWinnerIndex = 1;
    }
    else if(_bids[PLAYER2] > _bids[PLAYER1])
    {
        winner = _players[PLAYER2];
        _lastBidWinnerIndex = 2;
    }
    else
    {
        _lastBidWinnerIndex = 0;
    }

    if(winner != null)
    {
        winner->_mana[_sphereOnTable] += _manaOnTable;
        winner->_willpower -= _bids[(winner == _players[PLAYER1])?PLAYER1:PLAYER2];
        if(_turn == 1)
            _first = winner;
    }
    else
    {
        _players[PLAYER1]->_mana[_sphereOnTable] += _manaOnTable/2;
        _players[PLAYER1]->_willpower -= _bids[PLAYER1];
        _players[PLAYER2]->_mana[_sphereOnTable] += _manaOnTable/2;
        _players[PLAYER2]->_willpower -= _bids[PLAYER2];
        if(_turn == 1)
            _first = _players[PLAYER1];
    }

    if(_first == _players[PLAYER1])
    {
        _state = P1_ATTACK;
        _currentPlayer = _players[PLAYER1];
    }
    else
    {
        _state = P2_ATTACK;
        _currentPlayer = _players[PLAYER2];
    }
}

void GameContext::resolveTurn()
{

    bool applyAttackingSpell = true;

    /*
     * Si l'attaquant a jeté un sort, on doit vérifier s'il a été paré avant de pouvoir
     * appliquer les effets
     */
    if(_currentAttackingSpell != null)
    {
        /* Si le sort de l'attaquant est contré, ses effets ne seront pas appliqués */
        if( _currentDefendingSpell != null
            && _currentAttackingSpell->getType() == SPELLTYPE_SPELL 
            && _currentDefendingSpell->getType() == SPELLTYPE_COUNTER 
            && _currentDefendingSpell->hasEffect(BLOCK))
        {
            applyAttackingSpell = false;
        }
    }
    

    // A cet instant _currentPlayer == le défenseur donc il faut prendre son
    // adversaire pour retrouver l'attaquant

    if(_currentAttackingSpell != null && applyAttackingSpell)
        applyPlayerSpell(getOpponent(_currentPlayer), _currentAttackingSpell);
    if(_currentDefendingSpell != null)
        applyPlayerSpell(_currentPlayer, _currentDefendingSpell);

    _turn++;

    if(_state == P1_DEFEND)
        _state = P1_ATTACK;
    else if(_state == P2_DEFEND)
        _state = P2_ATTACK;
    else
        LM_ASSERT(false);

    if(_players[0]->getHealth() <= 0 && _players[1]->getHealth() <= 0)
        _state = DRAW;
    if(_players[0]->getHealth() <= 0)
        _state = P2_WIN;
    if(_players[1]->getHealth() <= 0)
        _state = P1_WIN;
}

bool GameContext::endOfTurn()
{
    if(    (_state == P1_DEFEND && _first == _players[0])
        || (_state == P2_DEFEND && _first == _players[1]))
        return true;
    return false;
}

void GameContext::generateMana()
{
    _sphereOnTable = (ESphere) Core::irand(0,2);
    _manaOnTable = Core::irand(1,5);
}

bool GameContext::canCastSpell(const Ptr<PlayerInstance> & player, const Ptr<SpellInstance> & spell)
{
    ESpellType type = spell->getType();

    if(player->getSphereControl(SPHERE_ENERGY) >= spell->getMagnitudeOfEffect(REQUIRE_ENERGY)
        && player->getSphereControl(SPHERE_SPIRIT) >= spell->getMagnitudeOfEffect(REQUIRE_SPIRIT)
        && player->getSphereControl(SPHERE_LIFE) >= spell->getMagnitudeOfEffect(REQUIRE_LIFE))
    {
        if((_state == P1_ATTACK && player == _players[PLAYER1]) || (_state == P2_ATTACK && player == _players[PLAYER2]))
        {
            if(type != SPELLTYPE_COUNTER)
            {
                return true;
            }
        }
        else
        if((_state == P1_DEFEND && player == _players[PLAYER1]) || (_state == P2_DEFEND && player == _players[PLAYER2]))
        {
            if((type == SPELLTYPE_COUNTER
                && _currentAttackingSpell != null 
                && _currentAttackingSpell->getType() == SPELLTYPE_SPELL 
                && player->getSphereControl(SPHERE_ENERGY) >= _currentAttackingSpell->getMagnitudeOfEffect(ENERGY_ASSAULT)
                && player->getSphereControl(SPHERE_SPIRIT) >= _currentAttackingSpell->getMagnitudeOfEffect(SPIRIT_ASSAULT)
                &&  player->getSphereControl(SPHERE_LIFE)  >= _currentAttackingSpell->getMagnitudeOfEffect(LIFE_ASSAULT)) 
                || type == SPELLTYPE_TRICK)
            {
                return true;
            }
        }
    }

    return false;
}

void GameContext::applyPlayerSpell(const Ptr<PlayerInstance> & player, const Ptr<SpellInstance> & spell)
{
    Ptr<Core::List<SpellEffect> > effects = spell->getEffects();
    Ptr<PlayerInstance> opponent = getOpponent(player);

    for(int i = 0; i < effects->size(); i++)
    {
        SpellEffect effect = (*effects)[i];
        switch(effect.getEffect())
        {
        case DAMAGE:
            opponent->_health = std::max(0, opponent->_health - effect.getMagnitude());
            break;
        case HEAL:
            player->_health += effect.getMagnitude();
            break;
        case ABSORB:
            for(int i = 0; i <3; i++)
                player->_absorb[i] = effect.getMagnitude();
            break;
        case DIVINATION:
            player->_nextTurnsToSee = effect.getMagnitude();
            break;
        case ANTICIPATION:
            player->_seesOpponentDeck = true;
            break;
        case CHAINING:
            player->_chaining[spell->getSphere()] = effect.getMagnitude();
            break;
        case IMPAIR:
            for(int i =0; i < 3; i++)
                opponent->_impair[i] = effect.getMagnitude();
        case LIFE_BURN:
            opponent->_mana[SPHERE_LIFE] = Core::clamp(0, _cMAX_SPHERE_CONTROL, opponent->_mana[SPHERE_LIFE]-effect.getMagnitude());
            break;
        case SPIRIT_BURN:
            opponent->_mana[SPHERE_SPIRIT] = Core::clamp(0, _cMAX_SPHERE_CONTROL, opponent->_mana[SPHERE_SPIRIT]-effect.getMagnitude());
            break;
        case ENERGY_BURN:
            opponent->_mana[SPHERE_ENERGY] = Core::clamp(0, _cMAX_SPHERE_CONTROL, opponent->_mana[SPHERE_ENERGY]-effect.getMagnitude());
            break;
        case ROTATION_CW:
            {
                int32 tmp = player->_mana[SPHERE_ENERGY];
                player->_mana[SPHERE_ENERGY] = player->_mana[SPHERE_LIFE];
                player->_mana[SPHERE_LIFE] = player->_mana[SPHERE_SPIRIT];
                player->_mana[SPHERE_SPIRIT] = tmp;
            }
            break;
        case ROTATION_CCW:
            {
                int32 tmp = player->_mana[SPHERE_ENERGY];
                player->_mana[SPHERE_ENERGY] = player->_mana[SPHERE_SPIRIT];
                player->_mana[SPHERE_SPIRIT] = player->_mana[SPHERE_LIFE];
                player->_mana[SPHERE_LIFE] = tmp;
            }
            break;
        default:
            break;
        }
    }
}

const Ptr<PlayerInstance> & GameContext::getFirst() const
{
    return _first;
}

GameContext::GameContext(
        Ptr<PlayerInstance> player1,
        Ptr<PlayerInstance> player2,
        EGameState            state,
        Ptr<PlayerInstance> first,
        Ptr<PlayerInstance> currentPlayer,
        Ptr<SpellInstance>  currentAttackingSpell,
        Ptr<SpellInstance>  currentDefendingSpell,
        int32                bids[2],
        int32                manaOnTable,
        ESphere                sphereOnTable,
        int32                turn) :
    _state(state),
    _currentPlayer(currentPlayer),
    _currentAttackingSpell(currentAttackingSpell),
    _currentDefendingSpell(currentDefendingSpell),
    _manaOnTable(manaOnTable),
    _sphereOnTable(sphereOnTable),
    _turn(turn)
{
    _players[PLAYER1] = player1;
    _players[PLAYER2] = player2;
    _bids[0] = bids[0];
    _bids[1] = bids[1];
}

GameContext::GameContext(
        Ptr<PlayerInstance> player1,
        Ptr<PlayerInstance> player2,
        EGameState            state,
        Ptr<PlayerInstance> first,
        Ptr<PlayerInstance> currentPlayer,
        Ptr<SpellInstance>  currentAttackingSpell,
        Ptr<SpellInstance>  currentDefendingSpell,
        int32                turn) :
    _state(state),
    _currentPlayer(currentPlayer),
    _currentAttackingSpell(currentAttackingSpell),
    _currentDefendingSpell(currentDefendingSpell),
    _turn(turn)
{
    _players[PLAYER1] = player1;
    _players[PLAYER2] = player2;
}

Ptr<Core::List<Ptr<PlayerAction> > > GameContext::getAvailableActions(const Ptr<PlayerInstance> & player)
{
    Core::Map<int32, Ptr<SpellInstance> > deck = player->getDeck();

    Ptr<Core::List<Ptr<PlayerAction> > > result(new Core::List<Ptr<PlayerAction> > ());

    for(Core::Map<int32, Ptr<SpellInstance> >::iterator it = deck.begin(); it != deck.end(); it++)
    {
        if(this->verifyValidity(PlayerAction::cast(player, it->second)))
        {
            Ptr<PlayerAction> newAction(new PlayerAction(player, CAST, it->second));
            result->push_back(newAction);
        }
    }

    if(this->verifyValidity(PlayerAction::pass(player)))
    {
        Ptr<PlayerAction> newAction(new PlayerAction(player, PASS));
        result->push_back(newAction);
    }

    for(int32 i = 0; i <= GameContext::_cMAX_BID; i++)
    {
        if(this->verifyValidity(PlayerAction::bid(player, i)))
        {
            Ptr<PlayerAction> newAction(new PlayerAction(player, BID, null, i));
            result->push_back(newAction);
        }
    }

    return result;
}

Ptr<PlayerAction> GameContext::autoAction(const Ptr<PlayerInstance> & player, EChoiceMethod method)
{
    Ptr<Core::List<Ptr<PlayerAction> > > availableActions = getAvailableActions(player);

    if(availableActions->size() == 0)
        return null;

    int32 choice;
    switch(method)
    {
    case SMART:
    case RANDOM:
        choice = Core::irand(0, availableActions->size()-1);
    }

    return (*availableActions)[choice];
}
}
