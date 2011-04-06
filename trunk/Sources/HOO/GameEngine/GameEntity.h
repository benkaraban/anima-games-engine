/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef GAMEENGINE_GAMEENTITY_H_
#define GAMEENGINE_GAMEENTITY_H_

#include <Core/Standard.h>
#include <Core/Dictionary.h>
#include <GameEngine/Enums.h>

namespace GameEngine
{
/*
 * Une GameEntity est une entité de jeu caractérisée par un identifiant numérique unique, 
 * pouvant être nommée et pouvant être décrite dans une langue et un format précisé.
 */
class LM_API_HGE GameEntity : public Core::Object
{
public:

    GameEntity(Ptr<Core::Dictionary>);

    GameEntity(const GameEntity & entity);

    int32 getId() const;

    /*
     * Si un dictionnaire a été spécifié, retourne le nom de l'entité, sinon retourne une chaîne vide.
     */
    virtual const String & getName() const;

    /*
     * Si un dictionnaire a été spécifié, retourne une description de l'entité au format spécifié,
     * sinon retourne une chaîne vide.
     */
    virtual String getDescription(EDescriptionFormat) const;

    /* Change le dictionnaire courant */
    static void setDictionary(const Ptr<Core::Dictionary> &);

    virtual const String & getPrefix() const = 0;

    virtual ~GameEntity() {};

protected:
    int32 _id;
    static Ptr<Core::Dictionary> _spDictionary;
    GameEntity();
    GameEntity(int32 id);

};
}

#endif /* GAMEENGINE_GAMEENTITY_H_ */