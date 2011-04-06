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
#include "GamePool.h"

#include <Network/NetworkException.h>

namespace HOOServer
{

GamePool::GamePool()
:   _first(NULL)
{
    for(int32 ii=0; ii < Network::SOCKET_MAX_CONNECTION; ++ii)
    {
        _games[ii]._gameId = ii;
        _freeGames.push(ii);
    }
}

GamePool::~GamePool()
{}

Game * GamePool::getNewGame()
{
    Core::ScopedLock lock(_gamesMutex);

    if(_freeGames.size() <= 0)
        return NULL;

    int32 id = _freeGames.front();
    _freeGames.pop();
    
    _games[id]._next = _first;
    _games[id]._prev = NULL;
    
    if(_first != NULL)
        _first->_prev = &_games[id];

    _first = &_games[id];
    
    _first->newGame();

    return _first;
}

Game * GamePool::getGame(int32 sessionId)
{
    if(sessionId < 0 && sessionId >= Network::SOCKET_MAX_CONNECTION)
    {
        Core::String message;
        message << L"Invalid session id : "<<sessionId;
        throw Core::Exception(message);
    }

    return &_games[sessionId];
}

void GamePool::releaseGame(Game * game)
{
    Core::ScopedLock lock(_gamesMutex);

    if(game->_prev != NULL)
        game->_prev->_next = game->_next;

    if(game->_next != NULL)
        game->_next->_prev = game->_prev;

    if(game == _first)
    {
        _first = game->_next;
        if(_first != NULL)
            _first->_prev = NULL;
    }

    int32 id = game->_gameId;
    _freeGames.push(id);
}

void GamePool::initGames(const GameEngine::GameLibrary * pGameLib, UserSessionManager * pManager)
{
    Core::ScopedLock lock(_gamesMutex);

    for(int32 ii=0; ii < Network::SOCKET_MAX_CONNECTION; ii++)
    {
        _games[ii]._pPool = this;
        _games[ii]._pGameLib = pGameLib;
        _games[ii]._pSessions = pManager;
    }
}

}//namespace HOOServer