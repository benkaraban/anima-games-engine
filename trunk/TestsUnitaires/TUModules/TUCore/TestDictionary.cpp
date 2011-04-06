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
#include <UTests.h>

#include <Core/Dictionary.h>
#include <Core/VFS/VFSDirectMapping.h>

void testDictionary()
{
    UTEST_PACKAGE(L"Dictionary");
    UTEST_START_TEST(L"Load dictionary");

    Core::VFSDirectMapping vfs = Core::VFSDirectMapping();
    vfs.linkDirectory(L"./Data/TUData/Dictionary/");
    UTEST_ASSERT(vfs.fileExists(L"dict.xml"), L"Le fichier n'existe pas");

    Ptr<Core::InputStream> pInputStream = vfs.readFile(L"dict.xml");
    Core::Dictionary dico = Core::Dictionary(Core::FRENCH, (*pInputStream));

    UTEST_EQU(dico.size(), 2);
    UTEST_EQU(dico[L"sort bidon"], L"Ceci est un sort bidon.");
    UTEST_EQU(dico[L"boule de feu"], L"BOULE DE FEUUUUUU !");

    dico.load(Core::ENGLISH, (*vfs.readFile(L"dict.xml")));

    UTEST_EQU(dico.size(), 2);
    UTEST_EQU(dico[L"sort bidon"], L"This is a fake spell.");
    UTEST_EQU(dico[L"boule de feu"], L"Fireball, isn't it ?");

    UTEST_END_TEST;
}