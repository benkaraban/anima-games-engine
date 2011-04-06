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
#include <GameEngine/GameEntity.h>

namespace GameEngine
{
Ptr<Core::Dictionary> GameEntity::_spDictionary = null;

GameEntity::GameEntity() :
    _id(0)
{
}

GameEntity::GameEntity(int32 id) :
    _id(id)
{
}

GameEntity::GameEntity(Ptr<Core::Dictionary> pDictionary)
{
    if(_spDictionary == null)
        _spDictionary = pDictionary;
}

GameEntity::GameEntity(const GameEntity & entity) :
    _id(entity._id)
{
}

int32 GameEntity::getId() const
{
    return _id;
}

/* Retourne le nom de l'entit�.
 * Pour cela, on cherche la cl� "id_name" dans le dictionnaire.
 */
const String & GameEntity::getName() const
{
    return (*_spDictionary)[this->getPrefix() + Core::toString(_id) + L"_NAME"];
}

/*
 * Retourne une description de l'entit� au format et dans le langage sp�cifi�.
 * Pour cela, on cherche la cl� "id_format" dans le dictionnaire.
 */
String GameEntity::getDescription(EDescriptionFormat format) const
{
    String fullid = GameEngine::toString(format);
    fullid = this->getPrefix() + Core::toString(_id) + L"_" + fullid;
    return (*_spDictionary)[fullid];
}

void GameEntity::setDictionary(const Ptr<Core::Dictionary> & pDictionary)
{
    GameEntity::_spDictionary = pDictionary;
}

}