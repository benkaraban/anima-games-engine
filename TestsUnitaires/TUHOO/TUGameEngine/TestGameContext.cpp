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
#include <GameEngine/GameLibrary.h>
#include <GameEngine/Player.h>
#include <GameEngine/PlayerInstance.h>
#include <UTests.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/XML/XMLDocument.h>
#include <Core/Logger.h>
#include <Core/Standard.h>
#include <Core/Math/Random.h>

using namespace GameEngine;

#define SPELL_ID 1
#define COUNTER_ID 2
#define TRICK_ID 3

/* Génère 112 contextes de jeu différents, à multiplier par le nombre d'instances de joueurs * 2 * le nombre d'actions pour obtenir le nombre réels d'états de jeu testés.*/
Ptr<Core::List<Ptr<GameContext> > > generateContextsSet(const Ptr<PlayerInstance> & player1, const Ptr<PlayerInstance> & player2)
{
	Ptr<Core::List<Ptr<GameContext> > > pContexts = Ptr<Core::List<Ptr<GameContext> > > (new Core::List<Ptr<GameContext> > ());

	for(int32 state = (int32) P1_ATTACK; state < (int32) DRAW; state++) // 7 �tats
	{
        /*switch(state)
        {
        case P1_DEFEND:
        case P2_DEFEND:
            {
                bool isAttacked = Core::irand(0,1) == 0?0:1;
                if(isAttacked)
                {
                    Core::Map<int32, Ptr<SpellInstance> > deck = player1->getDeck();
                    Ptr<SpellInstance> s = null;
                    do
                    {
                        s = deck[Core::irand(0,deck.size()-1)];
                    }
                    while(s == null || s->getType() == GameEngine::SPELLTYPE_COUNTER);
                }
            }
            break;
        default:
            ;
        }*/
		for(int32 current = 1; current <= 2; current++)
		{
			Ptr<PlayerInstance> currentPlayer;
			switch(current)
			{
			case 1: currentPlayer = player1; break;
			case 2: currentPlayer = player2; break;
			}
			for(int32 first = 1; first <= 2; first++)
			{
				Ptr<PlayerInstance> firstPlayer;
				switch(first)
				{
				case 1: firstPlayer = player1; break;
				case 2: firstPlayer = player2; break;
				}
				pContexts->push_back(Ptr<GameContext> (new GameContext(player1, 
												  player2,
												  (EGameState) state,
												  firstPlayer,
												  currentPlayer,
												  null,
												  null,
												  10)));
			}
		}
	}
	return pContexts;
}


/* Chaque joueur peut réaliser X lancer de sorts + 1 action pass */
Ptr<Core::List<Ptr<PlayerAction> > > generateActionsSet(const Ptr<PlayerInstance> & player1, const Ptr<PlayerInstance> & player2)
{
	Ptr<Core::List<Ptr<PlayerAction> > > pActions = Ptr<Core::List<Ptr<PlayerAction> > > (new Core::List<Ptr<PlayerAction> > ());

	/* On génère un joueur1 et un joueur2*/
	for(int p = 0; p <=1; p++)
	{
		Ptr<PlayerInstance> player = p == 0 ? player1 : player2;
		/* On génère toutes les actions possibles */
		for(int action = 0; action < 2; action++)
		{
			switch(action)
			{
			/*case BID:
				for(int i = 0; i <= 4; i+=4)
					pActions->push_back(Ptr<PlayerAction> (new PlayerAction(player, BID, null, i)));
				break;*/
			case CAST:
				{
					Core::Map<int32, Ptr<SpellInstance> > spells = player->getDeck();
					for(int i = 0; i < spells.size(); i++)
						pActions->push_back(Ptr<PlayerAction> (new PlayerAction(player, CAST, spells[1001+i], 0)));
				}
				break;
			case PASS:
				pActions->push_back(Ptr<PlayerAction> (new PlayerAction(player, PASS, null, 0)));
				break;
			}
		}
	}
	return pActions;
}



/* Note : cette fonction crée actuellement X instances de joueurs différentes */
Ptr<Core::List<Ptr<PlayerInstance> > > generatePlayersSet(const Ptr<PlayerInstance> & player)
{
	Ptr<Core::List<Ptr<PlayerInstance> > > pPlayers(new Core::List<Ptr<PlayerInstance> >());

	int mana[3];
	for(int32 sphereMana = 0; sphereMana < 3; sphereMana++)
	{
		for(int32 manaQuantity = 0; manaQuantity <=5; manaQuantity++)
		{
			for(int32 health = 0; health <= 100; health += 50)
			{
                mana[sphereMana] = manaQuantity;
				//int sphereMana = Core::irand(0,2);
				//int sphereChaining = Core::irand(0,2);
				//int sphereImpair = Core::irand(0,2);
				//int sphereAbsorb = Core::irand(0,2);
				//int health = Core::irand(0,100);

				pPlayers->push_back(Ptr<PlayerInstance> (new PlayerInstance(
				player,health,mana)));
			}
		}
	}

	return pPlayers;
}

void testGameContext()
{
    UTEST_START_TEST(L"GameContext");

	/************************************************************************************************
	 * Test d'enchainement des états
	 ************************************************************************************************/

    /* Création du GameContext */
    Ptr<PlayerInstance> player1 = Ptr<PlayerInstance> (new PlayerInstance(Player(L"Bill")));
    Ptr<PlayerInstance> player2 = Ptr<PlayerInstance> (new PlayerInstance(Player(L"Joe")));

    /* Chargement de la bibliothèque de jeu */
    /* On crée le VFS */
    Ptr<Core::VFSDirectMapping> pVFS = Ptr<Core::VFSDirectMapping> (new Core::VFSDirectMapping());

    /* On le lie à un répertoire */
    pVFS->linkDirectory(L"./Data/TUData/HOOGameEngine/");
    
    /* On crée la bibliothèque */
    GameLibrary library = GameLibrary(pVFS, L"GameLibrary/");

    for(int32 ii = 1001; ii <= 1024; ii++)
    {
        player1->addSpellInDeck((*library.getSpell(ii)));
        player2->addSpellInDeck((*library.getSpell(ii)));
    }

	/************************************************************************************************
	 * Tests de cohérence
	 ************************************************************************************************/

	/* On vérifie la cohérence de tous les triplets (GameContext, (PlayerInstance1, PlayerInstance2), PlayerAction) */
	Ptr<Core::List<Ptr<PlayerInstance> > > pPlayer1List = generatePlayersSet(player1);
	Ptr<Core::List<Ptr<PlayerInstance> > > pPlayer2List = generatePlayersSet(player2);

	/* Pour chaque instance du joueur 1 */
	for(Core::List<Ptr<PlayerInstance> >::iterator p1 = pPlayer1List->begin(); p1 != pPlayer1List->end(); p1++)
	{
		/* Pour chaque instance du joueur 2 */
		for(Core::List<Ptr<PlayerInstance> >::iterator p2 = pPlayer2List->begin(); p2 != pPlayer2List->end(); p2++)
		{
			Ptr<Core::List<Ptr<GameContext> > > pContextList = generateContextsSet(*p1, *p2);
			Ptr<Core::List<Ptr<PlayerAction> > > pActionList = generateActionsSet(*p1, *p2);

			/* Pour chaque contexte de jeu entre p1 et p2 */
			for(Core::List<Ptr<GameContext> >::iterator context = pContextList->begin(); context != pContextList->end(); context++)
			{
				/* Pour chaque action dans ce contexte */
				for(Core::List<Ptr<PlayerAction> >::iterator action = pActionList->begin(); action != pActionList->end(); action++)
				{
                    /*
                     * Test de validité des actions
                     */

                    /* Action CAST */
                    if((**action)._type == GameEngine::CAST)
                    {
                        UTEST_ASSERT(
                               ((((**context).getState() == P1_ATTACK || (**context).getState() == P2_ATTACK) && (*action)->_spell->getType() != GameEngine::SPELLTYPE_COUNTER && (*context)->verifyValidity((**action))))
                            || ((((**context).getState() == P1_DEFEND || (**context).getState() == P2_DEFEND) && (*action)->_spell->getType() != GameEngine::SPELLTYPE_SPELL && (*context)->verifyValidity((**action))))
                            || (((**context).getState() == P1_WIN || (**context).getState() == P2_WIN || (**context).getState() == DRAW) && !(*context)->verifyValidity((**action))),
                            L"Action CAST et état incompatible : (" +  GameEngine::toString((**context).getState()) + L"," + ((*context)->verifyValidity(**action)?L"true":L"false") + L")\n");

                        /*UTEST_ASSERT(((((**context).getState() >= P1_ATTACK && (**context).getState() <= P1_DEFEND) && (*context)->verifyValidity((**action)))) 
                            || (((**context).getState() == P1_WIN || (**context).getState() == P2_WIN || (**context).getState() == DRAW) && !(*context)->verifyValidity((**action))),
                            L"Action CAST et état incompatible : (" +  GameEngine::toString((**context).getState()) + L"," + ((*context)->verifyValidity(**action)?L"true":L"false") + L")\n");*/

                        EGameState stateBeforeCast = (*context)->getState();
                        int manaBeforeCast[3];
                        manaBeforeCast[GameEngine::SPHERE_ENERGY] = (**action)._player->getSphereControl(GameEngine::SPHERE_ENERGY);
                        manaBeforeCast[GameEngine::SPHERE_LIFE] = (**action)._player->getSphereControl(GameEngine::SPHERE_LIFE);
                        manaBeforeCast[GameEngine::SPHERE_SPIRIT] = (**action)._player->getSphereControl(GameEngine::SPHERE_SPIRIT);
                        (*context)->applyPlayerAction(*(*action));

                        /* La transition d'état est-elle correcte ? */
                        UTEST_ASSERT((stateBeforeCast != P1_ATTACK && stateBeforeCast != P2_ATTACK && stateBeforeCast != P1_DEFEND &&stateBeforeCast != P2_DEFEND) ||
                                     (stateBeforeCast == P1_ATTACK && ((*context)->getState() == P2_DEFEND || (*context)->getState() == P1_WIN)) ||
                                     (stateBeforeCast == P2_DEFEND && (*context)->getState() == P2_ATTACK) ||
                                     (stateBeforeCast == P2_ATTACK && ((*context)->getState() == P1_DEFEND || (*context)->getState() == P2_WIN)) ||
                                     (stateBeforeCast == P1_DEFEND && (*context)->getState() == P1_ATTACK), "Transition incorrecte après action CAST\n");

                        /* La modification des scores de controle est-elle cohérente ? */
                        int manaAfterCast[3];
                        manaAfterCast[GameEngine::SPHERE_ENERGY] = (**action)._player->getSphereControl(GameEngine::SPHERE_ENERGY);
                        manaAfterCast[GameEngine::SPHERE_LIFE] = (**action)._player->getSphereControl(GameEngine::SPHERE_LIFE);
                        manaAfterCast[GameEngine::SPHERE_SPIRIT] = (**action)._player->getSphereControl(GameEngine::SPHERE_SPIRIT);

                        /*if((*action)->_spell->getType() == GameEngine::SPELLTYPE_SPELL || (*action)->_spell->getType() == GameEngine::SPELLTYPE_TRICK)
                        {
                            UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_ENERGY] == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_ENERGY] + (*action)->_spell->getSphereControl(GameEngine::SPHERE_ENERGY)), "Controle de sphère(énergie) incohérent après CAST");
                            UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_SPIRIT] == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_SPIRIT] + (*action)->_spell->getSphereControl(GameEngine::SPHERE_SPIRIT)), "Controle de sphère(esprit) incohérent après CAST");
                            UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_LIFE] == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_LIFE] + (*action)->_spell->getSphereControl(GameEngine::SPHERE_LIFE)), "Controle de sphère(vie) incohérent après CAST");
                        }*/
                        //else
                        //{
                        //    UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_ENERGY] == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_ENERGY] + (*action)->_spell->getSphereControl(GameEngine::SPHERE_ENERGY)) - ((*context)->getCurrentAttackingSpell() == null?0:(*context)->getCurrentAttackingSpell()->getSphereControl(GameEngine::SPHERE_ENERGY)), "Controle de sphère(énergie) incohérent après CAST");
                        //    UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_SPIRIT] == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_SPIRIT] + (*action)->_spell->getSphereControl(GameEngine::SPHERE_SPIRIT)) - ((*context)->getCurrentAttackingSpell() == null?0:(*context)->getCurrentAttackingSpell()->getSphereControl(GameEngine::SPHERE_SPIRIT)), "Controle de sphère(esprit) incohérent après CAST");
                        //    UTEST_ASSERT(manaAfterCast[GameEngine::SPHERE_LIFE]   == Core::clamp(0, GameEngine::GameContext::_cMAX_SPHERE_CONTROL, manaBeforeCast[GameEngine::SPHERE_LIFE]   + (*action)->_spell->getSphereControl(GameEngine::SPHERE_LIFE)) - ((*context)->getCurrentAttackingSpell() == null?0:(*context)->getCurrentAttackingSpell()->getSphereControl(GameEngine::SPHERE_LIFE)), "Controle de sphère(vie) incohérent après CAST");
                        //}
                    }
                    else
                    {                
                        UTEST_ASSERT((**action)._type != GameEngine::PASS || ((**action)._type == GameEngine::PASS && ((**context).getState() < P1_ATTACK || (**context).getState() > P1_DEFEND) && (*context)->verifyValidity((**action))), "Action PASS validée avec un état incompatible\n");

                        EGameState stateBeforeCast = (*context)->getState();

					    /* On applique l'action */
					    (*context)->applyPlayerAction(*(*action));

                        /* La transition d'état est-elle correcte ? */
                        UTEST_ASSERT((stateBeforeCast == P1_ATTACK && (*context)->getState() == P2_DEFEND) ||
                                     (stateBeforeCast == P2_DEFEND && (*context)->getState() == P2_ATTACK) ||
                                     (stateBeforeCast == P2_ATTACK && (*context)->getState() == P1_DEFEND) ||
                                     (stateBeforeCast == P1_DEFEND && (*context)->getState() == P1_ATTACK), "Transition incorrecte après action CAST\n");

                    }

					/*
					 *	Test de cohérence du controle
					 */
					UTEST_ASSERT(player1->getMana(SPHERE_ENERGY) >= 0, "controle de sphère négatif\n");
					UTEST_ASSERT(player1->getMana(SPHERE_SPIRIT) >= 0, "controle de sphère  négatif\n");
					UTEST_ASSERT(player1->getMana(SPHERE_LIFE) >= 0, "controle de sphère  négatif\n");
					UTEST_ASSERT(player2->getMana(SPHERE_ENERGY) >= 0, "controle de sphère  négatif\n");
					UTEST_ASSERT(player2->getMana(SPHERE_SPIRIT) >= 0, "controle de sphère  négatif\n");
					UTEST_ASSERT(player2->getMana(SPHERE_LIFE) >= 0, "controle de sphère  négatif\n");

                    UTEST_ASSERT(player1->getMana(SPHERE_ENERGY) <= 10, "controle de sphère  supérieur à 10\n");
					UTEST_ASSERT(player1->getMana(SPHERE_SPIRIT) <= 10, "controle de sphère  supérieur à 10\n");
					UTEST_ASSERT(player1->getMana(SPHERE_LIFE) <= 10, "controle de sphère  supérieur à 10\n");
					UTEST_ASSERT(player2->getMana(SPHERE_ENERGY) <= 10, "controle de sphère  supérieur à 10\n");
					UTEST_ASSERT(player2->getMana(SPHERE_SPIRIT) <= 10, "controle de sphère  supérieur à 10\n");
					UTEST_ASSERT(player2->getMana(SPHERE_LIFE) <= 10, "controle de sphère  supérieur à 10\n");

                    /*
                     * Test des conditions de victoires
                     */
                    GameEngine::EGameState state = (*context)->getState();
                    UTEST_ASSERT((player1->getHealth() > 0 && state != P2_WIN) ||
                                 (player1->getHealth() <= 0 && state == P2_WIN) ||
                                 (player2->getHealth() > 0 && state != P1_WIN) ||
                                 (player2->getHealth() <= 0 && state == P1_WIN)
                                  , "Conditions de victoire incohérentes\n");
				}
			}
		}
	}
	
    UTEST_END_TEST;
}
