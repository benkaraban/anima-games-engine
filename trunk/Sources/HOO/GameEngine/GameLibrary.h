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
#ifndef GAMEENGINE_GAMELIBRARY_H_
#define GAMEENGINE_GAMELIBRARY_H_

#include <Core/Standard.h>
#include <Core/Map.h>
#include <Core/List.h>
#include <GameEngine/Spell.h>
#include <GameEngine/Stuff.h>
#include <Core/VFS/VirtualFileSystem.h>

namespace GameEngine
{
/*
 * Classe contenant la bibliothèque des entités de jeu. Une GameLibrary est associée à un VFS
 * et un répertoire dédié dans celui-ci, lesquels sont donnés à la construction.
 *
 * Chaque élément de la GameLibrary est une GameEntity possédant un identifiant numérique unique,
 * pouvant être obtenu à l'aide de l'opérateur [].
 *
 * L'organisation de la GameLibrary dans le VFS est la suivante :
 *
 *     /
 *     |
 *     +---_myPath
 *           |
 *           |   dictionary.xml
 *           |
 *           +---_sSPELLS_DIR
 *           |            "SPL_0.xml"
 *           |            "SPL_1.xml"
 *           |            "SPL_2.xml"
 *           |            .
 *           |            .
 *           |            .
 *           +---_sSTUFF_DIR
 *                        "STF_0.xml"
 *                        "STF_1.xml"
 *                        "STF_2.xml"
 *         
 * Note : chaque entrée du dictionnaire est indexée par un identifiant, qui est une
 * chaîne de caractère sous la forme :
 *
 *      [SPELL|STUFF|EFFECT|GUI]_[0-9|enum literal]*[NAME|TOOLTIP|DETAILED]
 *        |                            |                    |
 *        |                            |                    |
 *   type d'entrée                     |                    |
 *   numérique                         |                    |
 *                                     |                type d'entrée
 *                               identifiant
 *                                                      utilisé notamment pour les entités de jeu,
 *                                                      qui ont un identifiant numérique mais
 *                                                      plusieurs descriptions.
 */
class LM_API_HGE GameLibrary : public Core::Object
{
public:
    /*
     * Construit une nouvelle bibliothèque à partir du vfs spécifié.
     * Les fichiers de la bibliothèque sont chargés et sauvés dans un répertoire dédié passé en paramètre.
     */
    GameLibrary(const Ptr<Core::VirtualFileSystem> & vfs, const String & myPath, Core::ELanguage language = Core::ENGLISH);

    /*
     * Retourne un pointeur vers le sort dont l'identifiant est donné.
     * Si le sort n'existe pas, cette méthode renvoie null.
     */
    const Spell * getSpell(int32 id) const;
    const Stuff * getStuff(int32 id) const;


    /*
     * Sauvegarde la bibliothèque dans son répertoire dédié.
     * Chaque élément de la bibliothèque (sorts, équipements) est sauvegardé dans un fichier au nom
     * sous la forme : préfixe + identifiant numérique du sort + ".xml".
     */
    void save();

    /*
     * Charge la bibliothèque à partir du répertoire myPath spécifié à la construction.
     * Cette fonction peut lancer l'exception GameLibraryNotFoundException.
     */
    void load();

    /*
     * Vide le contenu de la bibliothèque.
     */
    void clear();

    void reload()
    {
        clear();
        load();
    }

    /*
     * Charge le dictionaire de langue spécifié.
     */
    void loadDictionary(Core::ELanguage);

    /*
     * Ajoute un sort à la bibliothèque.
     */
    void add(const Spell * spell);
    void add(const Stuff * stuff);

    int32 spellCount() const
    {
        return _spells.size();
    }

    int32 stuffCount() const
    {
        return _stuffs.size();
    }

    /* Retourne l'entité de jeu correspondant à l'identifiant donné. */
    const GameEntity & operator[](int32);

protected:
    Ptr<Core::Dictionary>                   _pDictionary;
    Core::Map<int32, const Spell *>         _spells;
    Core::Map<int32, const Stuff *>         _stuffs;
    Core::Map<int32, const GameEntity *>    _all;

    Core::ELanguage                     _language;

    Ptr<Core::VirtualFileSystem>    _pVFS;
    String                          _myPath;

    /*const static String             _sSPELL_PREFIX;*/
    const static String             _sSTUFF_PREFIX;
    const static String             _sSPELL_EFFECT_PREFIX;

    const static String             _sSPELLS_DIR;
    const static String             _sSTUFFS_DIR;
    const static String             _sSPELL_EFFECTS_DIR;
    const static String             _sDICTIONARY_FILE;
};
}
#endif /* GAMEENGINE_GAMELIBRARY_H_ */
