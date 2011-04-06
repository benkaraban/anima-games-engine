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
#ifndef CORE_DICTIONARY_H_
#define CORE_DICTIONARY_H_

#include <Core/Standard.h>
#include <Core/IO/Streams.h>
#include <Core/Map.h>

namespace Core
{
/*
 * Enumération pour les langues.
 */
 LM_API_COR LM_ENUM_2(
    ELanguage,
    ENGLISH,
    FRENCH
);

/*
 * La classe Dictionary permet de manipuler des informations de localisation représentées
 * dans un fichier xml.
 */
class LM_API_COR Dictionary : public Object
{
public:
    Dictionary(ELanguage, InputStream &);

    /* Charge le dictionnaire de langue passée en paramètre */
    void load(ELanguage, InputStream &);
    
    ELanguage getLoadedLanguage () {return _language;}

    int size() const;

    virtual ~Dictionary();

    const String & operator[] (const String &) const;

protected:
    ELanguage           _language;
    Map<String, String> _content;
};
}

#endif /* CORE_DICTIONARY_H_ */