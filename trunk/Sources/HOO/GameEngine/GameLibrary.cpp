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
#include <GameEngine/GameLibrary.h>
#include <GameEngine/GameLibraryNotFoundException.h>
#include <Core/List.h>

namespace GameEngine
{
//const String GameLibrary::_sSPELL_PREFIX(L"SPL_");
const String GameLibrary::_sSTUFF_PREFIX(L"STF_");
const String GameLibrary::_sSPELL_EFFECT_PREFIX(L"EFF_");

const String GameLibrary::_sSPELLS_DIR(L"spells/");
const String GameLibrary::_sSTUFFS_DIR(L"stuffs/");
const String GameLibrary::_sSPELL_EFFECTS_DIR(L"effects/");
const String GameLibrary::_sDICTIONARY_FILE(L"dict.xml");

GameLibrary::GameLibrary(const Ptr<Core::VirtualFileSystem> & vfs, const String & myPath, Core::ELanguage language) :
        _language(language),
        _pVFS(vfs),
        _myPath(myPath)
{
    load();
}

const Spell * GameLibrary::getSpell(int32 id) const
{
    Core::Map<int32, const Spell *>::const_iterator iSpell = this->_spells.find(id);

    if(iSpell == this->_spells.end())
        return NULL;
    else
        return iSpell->second;
}


const Stuff * GameLibrary::getStuff(int32 id) const
{
    Core::Map<int32, const Stuff *>::const_iterator iStuff = this->_stuffs.find(id);

    if(iStuff == this->_stuffs.end())
        return NULL;
    else
        return iStuff->second;
}

void GameLibrary::add(const Spell * spell)
{
    this->_spells[spell->getId()] = spell;
    this->_all[spell->getId()] = spell;
}

void GameLibrary::add(const Stuff * stuff)
{
    this->_stuffs[stuff->getId()] = stuff;
    this->_all[stuff->getId()] = stuff;
}

void GameLibrary::save()
{
    String fullpath;

    /* Sauvegarde de tous les sorts*/
    
    for(int32 i = 0; i < _spells.size(); i++)
    {
        fullpath = this->_myPath;
        fullpath << Spell::getSpellsPrefix() << Core::toString(_spells[i]->getId()) << L".xml";
        _spells[i]->save(this->_pVFS, fullpath);
    }
}

void GameLibrary::loadDictionary(Core::ELanguage language)
{
    _language = language;
    Ptr<Core::InputStream> is = _pVFS->readFile(_myPath + _sDICTIONARY_FILE);

    if(is == null)
        throw Core::Exception(L"Error loading game dictionary " + _myPath + _sDICTIONARY_FILE);

    if(_pDictionary != null)
        _pDictionary->load(_language, (*is));
    else
        _pDictionary = Ptr<Core::Dictionary> (new Core::Dictionary(_language, (*is)));
    GameEntity::setDictionary(_pDictionary);
}

void GameLibrary::load()
{
    try
    {
        /* --------------------------------------------------------------------------------------- */
        /*                                  Chargement du dictionnaire                             */
        /* --------------------------------------------------------------------------------------- */
        loadDictionary(_language);

        /* --------------------------------------------------------------------------------------- */
        /*                                  Chargement des sorts                                   */
        /* --------------------------------------------------------------------------------------- */

        /* Récupération de la liste des fichiers du répertoire associé à GameLibrary/_sSPELLS_DIR */
        Core::List<String> dirs;
        this->_pVFS->listFiles(this->_myPath + _sSPELLS_DIR, dirs);

        /* Chargement des sorts */
       _spells.clear();
        String fullpath;

        for(int32 i = 0; i < dirs.size(); i++)
        {
            fullpath = this->_myPath + _sSPELLS_DIR + dirs[i];
            const Spell * newSpell = new Spell(this->_pVFS, fullpath);
            this->add(newSpell);
        }

        /* --------------------------------------------------------------------------------------- */
        /*                                  Chargement des équipements                             */
        /* --------------------------------------------------------------------------------------- */

        /* Récupération de la liste des fichiers du répertoire associé à GameLibrary/_sSTUFFS_DIR */
        dirs.clear();
        this->_pVFS->listFiles(this->_myPath + _sSTUFFS_DIR, dirs);

        /* Chargement des équipements */
        _stuffs.clear();
        for(int32 i = 0; i < dirs.size(); i++)
        {
            fullpath = this->_myPath + _sSTUFFS_DIR + dirs[i];
            const Stuff * newStuff = new Stuff(this->_pVFS, fullpath);
            this->add(newStuff);
        }
    }
    catch(const Core::Exception & e)
    {
        String message = L"Error when loading Game Library";
        message << L" : " << e.getMessage();
        if(this->_pDictionary == null)
            message << L" : No dictionary";
        if(this->_spells.size() == 0)
            message << L" : No spells in library";
        throw GameLibraryNotFoundException(message);
    }
}

void GameLibrary::clear()
{
    for(int32 ii=0; ii < _all.size(); ii++)
        delete _all[ii];

    _all.clear();
    _spells.clear();
    _stuffs.clear();
}

const GameEntity & GameLibrary::operator[](int32 id)
{
    return (* this->_all[id]);
}

}
