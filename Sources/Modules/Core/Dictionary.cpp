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
#include <Core/Dictionary.h>

#include <Core/XML/XMLDocument.h>

namespace Core
{
    Dictionary::Dictionary(ELanguage language, InputStream & inputStream) :
        _language(language),
        _content(Map<String, String>())
    {
        load(language, inputStream);
    }

    void Dictionary::load(ELanguage language, InputStream & inputStream)
    {
        _language = language;
        _content.clear();

        /* On charge de fichier .xml */
        XMLDocument doc = XMLDocument();
        doc.loadDocument(inputStream);

        /* On détermine les nodes à récupérer */
        String languageNodeTag = Core::toString(language);
        languageNodeTag.toLower();
        String languageNode = languageNodeTag;

        /* On récupère toutes les entries */
        List<Ptr<XMLNode> > entryNodes;
        doc.getNodes(L"dictionary", L"entry", entryNodes);

        /* On ne garde que celles qui sont dans le langage voulu */
        String key, value;
        for(int i =0; i < entryNodes.size(); i++)
        {
            entryNodes[i]->getAttribute(L"", L"id", key);
            entryNodes[i]->getValue(languageNode, value);
            _content[key] = value;
        }
        inputStream.close();
    }

    const String & Dictionary::operator[] (const String & id) const
    {
        Map<String, String>::const_iterator iResult = _content.find(id);

        if(iResult == _content.end())
            return String::EMPTY;
        else
            return iResult->second;
    }

    int Dictionary::size() const
    {
        return _content.size();
    }

    Dictionary::~Dictionary()
    {
    }
}
