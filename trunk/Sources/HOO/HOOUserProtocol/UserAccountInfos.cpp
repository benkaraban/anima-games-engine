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
#include <Core/IO/Tools.h>
#include <HOOUserProtocol/UserAccountInfos.h>

namespace HOOUserProtocol
{
UserAccountInfos::UserAccountInfos() :
    character(0),
    xp(0)
{
}

void UserAccountInfos::read(Core::InputStream & inputStream)
{
    inputStream.readSafe(mail, SAFE_STRING_LENGTH);
    inputStream.read(character);
    inputStream.read(xp);

    Core::readSafe(inputStream, spells, SAFE_LIST_SIZE);
    Core::readSafe(inputStream, stuff, SAFE_LIST_SIZE);

    Core::readSafe(inputStream, equipedSpells, SAFE_LIST_SIZE);
    Core::readSafe(inputStream, equipedStuff, SAFE_LIST_SIZE);

    savedDecks.clear();
    int32 savedDecksCount = inputStream.readInt32();

    if(savedDecksCount > 100)
        throw Core::IOException(L"Too many saved decks");

    for(int32 iDeck=0; iDeck < savedDecksCount; iDeck++)
    {
        String deckName;
        inputStream.readSafe(deckName, SAFE_STRING_LENGTH);

        Core::readSafe(inputStream, savedDecks[deckName], SAFE_LIST_SIZE);
    }
}

void UserAccountInfos::write(Core::OutputStream & outputStream) const
{
    outputStream.write(mail);
    outputStream.write(character);
    outputStream.write(xp);

    Core::write(outputStream, spells);
    Core::write(outputStream, stuff);

    Core::write(outputStream, equipedSpells);
    Core::write(outputStream, equipedStuff);

    outputStream.write(int32(savedDecks.size()));

    for(Core::Map<String, Core::List<int32> >::const_iterator iDeck=savedDecks.begin(); iDeck != savedDecks.end(); ++iDeck)
    {
        outputStream.write(iDeck->first);
        Core::write(outputStream, iDeck->second);
    }
}


OpponentInfos::OpponentInfos() : character(0), xp(0)
{
}

OpponentInfos::OpponentInfos(const String & name, int32 character, int32 xp, const Core::List<int32> & equipedStuff) :
    name(name),character(character), xp(xp), equipedStuff(equipedStuff)
{
}

void OpponentInfos::read(Core::InputStream & inputStream)
{
    inputStream.read(name);
    inputStream.read(character);
    inputStream.read(xp);
    Core::readSafe(inputStream, equipedStuff, SAFE_LIST_SIZE);
}

void OpponentInfos::write(Core::OutputStream & outputStream) const
{
    outputStream.write(name);
    outputStream.write(character);
    outputStream.write(xp);
    Core::write(outputStream, equipedStuff);
}

}
