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
#include <UTests.h>
#include <Core/VFS/VFSDirectMapping.h>
#include <Core/XML/XMLDocument.h>
#include <Core/Logger.h>

using namespace GameEngine;

void testGameLibrary()
{
    
    UTEST_START_TEST(L"GameLibrary");

    /* On crée le VFS */
    Ptr<Core::VFSDirectMapping> pVFS = Ptr<Core::VFSDirectMapping> (new Core::VFSDirectMapping());

    /* On le lie à un répertoire */
    pVFS->linkDirectory(L"./Data/TUData/HOOGameEngine/");
    
    /* On crée la bibliothèque */
    GameLibrary library = GameLibrary(pVFS, L"GameLibrary/");

    /* On vérifie que tous les sorts sont là. */
    UTEST_EQU(library.spellCount(), 24);

    /* Vérification accesseurs et dictionnaire. */
    const Spell * spell = library.getSpell(1001);

    UTEST_NOT_EQU(spell, (const void *)NULL);

    UTEST_EQU(spell->getSphere(), GameEngine::SPHERE_LIFE);

    UTEST_EQU(spell->hasEffect(BLOCK), true);

    UTEST_EQU(spell->getName(), L"Erosion");

    UTEST_EQU(library[1001].getName(), L"Erosion");

    SpellEffect e = spell->getEffect(0);

    UTEST_EQU(e.getName(), L"Counter");

    /*Ptr<Stuff> stuff = library.getStuff(100);

    UTEST_EQU(stuff->getName(), L"Ultimate sword of chaos");

    StuffEffect st = stuff->getEffect(0);

    UTEST_EQU(st.getName(), L"Increases mana energy");*/

    UTEST_END_TEST;
}
