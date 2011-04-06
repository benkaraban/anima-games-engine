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
#ifndef GAMEENGINE_GAMECONTEXT_H_
#define GAMEENGINE_GAMECONTEXT_H_

#include <Core/Standard.h>
#include <GameEngine/PlayerInstance.h>
#include <GameEngine/PlayerAction.h>
#include <Core/Enum.h>


namespace GameEngine
{

LM_API_HGE LM_ENUM_2(
    EPlayerType,
    PLAYER1,
    PLAYER2
)

LM_API_HGE LM_ENUM_10(
    EGameState,
    WAITING_FOR_BIDS,
    WAITING_FOR_P1_BID,
    WAITING_FOR_P2_BID,

    P1_ATTACK,
    P2_DEFEND,

    P2_ATTACK,
    P1_DEFEND,

    P1_WIN,
    P2_WIN,
    DRAW
)

LM_API_HGE LM_ENUM_2(
    EChoiceMethod,
    RANDOM,
    SMART
    )

class GameContext;

class LM_API_HGE IGameContextObs
{
public:
    virtual ~IGameContextObs() {};
    virtual void onContextUpdate(GameContext * pContext) = 0;
};

/*
 * Cette classe représente l'état du jeu, et implémente les méthodes permettant de vérifier la validité des actions émises par les joueurs,
 * et de les appliquer sur l'état courant.
 */

class LM_API_HGE GameContext : public Core::Object
{
public:
    GameContext();
    GameContext(const Ptr<PlayerInstance> & pP1, const Ptr<PlayerInstance> & pP2);
    GameContext(
        Ptr<PlayerInstance> player1,
        Ptr<PlayerInstance> player2,
        EGameState            state,
        Ptr<PlayerInstance> first,
        /*Ptr<PlayerInstance> vantagePlayer,*/
        Ptr<PlayerInstance> currentPlayer,
        Ptr<SpellInstance>  currentAttackingSpell,
        Ptr<SpellInstance>  currentDefendingSpell,
        int32                bids[2],
        int32                manaOnTable,
        ESphere              sphereOnTable,
        int32                turn);

    GameContext(
        Ptr<PlayerInstance> player1,
        Ptr<PlayerInstance> player2,
        EGameState          state,
        Ptr<PlayerInstance> first,
        Ptr<PlayerInstance> currentPlayer,
        Ptr<SpellInstance>  currentAttackingSpell,
        Ptr<SpellInstance>  currentDefendingSpell,
        int32               turn);

    void init(const Ptr<PlayerInstance> & pP1, const Ptr<PlayerInstance> & pP2);

    /*
     * Vérifie la validité de l'action passée en paramêtre dans le GameContext appelant.
     */
    bool verifyValidity(const PlayerAction &);

    /*
     * Applique une action au GameContext appelant, si cette action est valide. Retourne vrai si l'action a été
     * appliquée.
     */
    bool applyPlayerAction(const PlayerAction &, IGameContextObs * pObs = NULL);
    EGameState getState();

    bool isBidInProgress() const;

    /*
     * Retourne le joueur qui joue en premier.
     */
    const Ptr<PlayerInstance> & getFirst() const;

    const Ptr<PlayerInstance> & getP1() const { return _players[0]; };
    const Ptr<PlayerInstance> & getP2() const { return _players[1]; };

    int32 getLastBidWinnerIndex() const { return  _lastBidWinnerIndex; }
    int32 getVantageIndex() const;
    Ptr<SpellInstance> getCurrentAttackingSpell() const
    {
        return _currentAttackingSpell;
    }

    int32 getManaOnTable() const { return _manaOnTable; }
    ESphere    getSphereOnTable() const { return _sphereOnTable; }

    /*
     * Retourne l'ensemble des actions accessibles au joueur passé en paramètre.
     */
    Ptr<Core::List<Ptr<PlayerAction> > > getAvailableActions(const Ptr<PlayerInstance> &);

    /*
     * Choisit et retourne une action accessible au joueur passé en paramètre, en utilisant
     * la méthode de choix donnée.
     */
    Ptr<PlayerAction> autoAction(const Ptr<PlayerInstance> &, EChoiceMethod);

    /*
     * Mise maximum autorisée dans les épreuves de volonté.
     */
    static const int32 _cMAX_BID;
    static const int32 _cMAX_SPHERE_CONTROL;

protected:

    void resolveBids();
    void resolveTurn();
    void generateMana();
    void decrementCooldown();
    void applyPlayerSpell(const Ptr<PlayerInstance> & player, const Ptr<SpellInstance> & spell);
    bool canCastSpell(const Ptr<PlayerInstance> & player, const Ptr<SpellInstance> & spell);
    const Ptr<PlayerInstance> & getOpponent(const Ptr<PlayerInstance> & player);
    bool endOfTurn();

    Ptr<PlayerInstance> _players[2];
    Ptr<PlayerInstance> _first;
    EGameState          _state;
    /*Ptr<PlayerInstance> _vantagePlayer;*/
    Ptr<PlayerInstance> _currentPlayer;
    int32               _bids[2];
    Ptr<SpellInstance>  _currentAttackingSpell;
    Ptr<SpellInstance>  _currentDefendingSpell;
    int32               _manaOnTable;
    ESphere             _sphereOnTable;
    int32               _turn;
    int32               _lastBidWinnerIndex;
};

inline const Ptr<PlayerInstance> & GameContext::getOpponent(const Ptr<PlayerInstance> & player)
{
    if(player == _players[PLAYER1])
        return _players[PLAYER2];
    return _players[PLAYER1];
}
}

#endif /* GAMEENGINE_GAMECONTEXT_H_ */
