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
#ifndef GAMEENGINE_SPELL_H_
#define GAMEENGINE_SPELL_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/VFS/VirtualFileSystem.h>
#include <GameEngine/SpellEffect.h>


namespace GameEngine
{
/*
 * Un Spell représente un sort.
 * Cette classe contient toutes les informations concernant les effets et les coûts du sort.
 * Note : une fois chargé, un sort n'est pas sensé étre modifié.
 */
class LM_API_HGE Spell : public GameEntity
{
public:

    /* Constructeur par recopie */
    Spell(const Spell &);

    /*
     * Crée un nouveau sort avec les informations entrées en paramétre
     */
    Spell(int32 id,
        ESphere sphere,
        ESpellType type,
        const SpellEffect & effectOnHit1);
    Spell(int32 id, 
        ESphere sphere, 
        ESpellType type, 
        const SpellEffect & effectOnHit1, 
        const SpellEffect & effectOnHit2);
    Spell(int32 id, 
        ESphere sphere, 
        ESpellType type, 
        const SpellEffect & effectOnHit1, 
        const SpellEffect & effectOnHit2, 
        const SpellEffect & effectOnHit3);
    Spell(int32 id, 
        ESphere sphere, 
        ESpellType type, 
        int sphereControl[3],
        const Ptr<Core::List<SpellEffect> > & pEffectsOnCast,
        const Ptr<Core::List<SpellEffect> > & pEffectsOnHit
        );

    /*Spell(int32 id,
        ESphere sphere,
        ESpellType type,*/


    /*
     * Crée un sort en chargeant le fichier passé en paramétre dans le vfs donné.
     */
    Spell(const Ptr<Core::VirtualFileSystem> & vfs, const String & path);

    //const Ptr<Core::List<SpellEffect> > & getEffects() const;
    const Ptr<Core::List<SpellEffect> > & getEffects() const;
    const Ptr<Core::List<SpellEffect> > & getEffectsOnCast() const;
    const Ptr<Core::List<SpellEffect> > & getEffectsOnHit() const;

    int32 getSphereControl(ESphere sphere) const
    {
        return _sphereControl[sphere];
    }

    /*
     * TODO : Sauvegarde le sort dans le chemin complet passé en paramétre.
     */
    void save(const Ptr<Core::VirtualFileSystem> & vfs, const String & path) const;

    /*
     * Charge le sort correspondant é id.
     */
    void load(const Ptr<Core::VirtualFileSystem> & vfs, const String & path);

    ESphere getSphere() const
    {
        return _sphere;
    }

    ESpellType getType() const
    {
        return _type;
    }

    const String & getScript() const { return _script; }

    int32 getMagnitudeOfEffect(ESpellEffectType) const;

    const SpellEffect & getEffect(int32) const;

    /*
     * Vérifie si le sort posséde le type d'effet passé en paramétre
     */
    bool hasEffect(ESpellEffectType) const;

    static const String & getSpellsPrefix();

    virtual const String & getPrefix() const;

	virtual ~Spell() {};

protected:
    Ptr<Core::List<SpellEffect> > _pEffectsOnCast;
    Ptr<Core::List<SpellEffect> > _pEffectsOnHit;
    ESphere     _sphere;
    ESpellType  _type;
    int         _sphereControl[3];
    String      _script;
    static const String _sPREFIX;
};
}
#endif /* GAMEENGINE_SPELL_H_ */
