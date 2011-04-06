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
#ifndef HOOSERVER_GAMEDATATRANSLATE_H_
#define HOOSERVER_GAMEDATATRANSLATE_H_

#include <Core/Standard.h>

#include <GameEngine/GameLibrary.h>
#include <GameEngine/GameContext.h>

#include <HOOServer/UserAccount.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

namespace HOOServer
{
Ptr<GameEngine::PlayerInstance> net2Game(const HOOServer::UserAccount & account, const GameEngine::GameLibrary * pLibrary);

void net2Game(const HOOUserProtocol::ActionCommand & command, GameEngine::PlayerAction & action, const Ptr<GameEngine::PlayerInstance> & pPlayer);

void game2Net(int32 playerId, const GameEngine::GameContext & game, HOOUserProtocol::GameStatus & status);

void game2Net(const GameEngine::PlayerInstance & player, const GameEngine::GameContext & game, HOOUserProtocol::PlayerStatus & status);
void game2Net(const GameEngine::PlayerInstance & player, const GameEngine::GameContext & game, HOOUserProtocol::OpponentStatus & status);

void game2Net(const GameEngine::PlayerAction & action,
              HOOUserProtocol::ActionCommand & command);


void game2Net(const Core::List<Ptr<GameEngine::PlayerAction> > & actions, 
              Core::List<HOOUserProtocol::ActionCommand> & commands);

}//namespace HOOServer

#endif/*HOOSERVER_CHATMESSAGEHANDLER_H_*/