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
#include "XMLDocument.h"

#include <tinyxml/tinyxml.h>

#include <Core/Strings/Tools.h>
#include <Core/IO/Tools.h>

namespace Core
{

XMLDocument::XMLDocument(const String & delimiter)
:   XMLNode(L"", delimiter)
{}

XMLDocument::XMLDocument(InputStream& in, const String & delimiter)
:   XMLNode(L"", delimiter)
{
    loadDocument(in);
}

XMLDocument::~XMLDocument()
{}

bool XMLDocument::saveDocument(const String & fileName)
{
    TiXmlDocument xmlDocument;
    nodesToTinyXML(&xmlDocument);
    return xmlDocument.SaveFile(String8(fileName).c_str());
}

bool XMLDocument::saveDocument(OutputStream & outputStream)
{
    bool returnCode = false;
    TiXmlPrinter printer;
    TiXmlDocument xmlDocument;
    nodesToTinyXML(&xmlDocument);
    if( (returnCode = xmlDocument.Accept(&printer)) )
    {
        outputStream.writeData(printer.CStr(), printer.Size());
    }
    return returnCode;
}

bool XMLDocument::saveToString(String & value)
{
    bool returnCode = false;
    TiXmlPrinter printer;
    TiXmlDocument xmlDocument;
    nodesToTinyXML(&xmlDocument);
    if( (returnCode = xmlDocument.Accept(&printer)) )
    {
        String8 str8 = printer.CStr();
        value = String(str8);
    }
    return returnCode;
}

bool XMLDocument::loadDocument(const String & fileName)
{
    bool returnCode = false;
    TiXmlDocument xmlDocument;
    if(xmlDocument.LoadFile(String8(fileName).c_str()))
    {
        tinyXMLToNodes(&xmlDocument);
        returnCode = true;
    }
    return returnCode;
}

bool XMLDocument::loadDocument(InputStream & inputStream)
{
    TiXmlDocument xmlDocument;
    List<byte> data;
    //On transforme les données du InputStream en tableau
    readAllStream(data, inputStream);
    //On ajoute un '\0'
    data.push_back(0);

    //On récupère un pointeur sur les data
    char * charData = reinterpret_cast<char *>(&data[0]);

    xmlDocument.Parse(charData);
    tinyXMLToNodes(&xmlDocument);

    return !xmlDocument.Error();
}

bool XMLDocument::isPathValid(const String & path) const
{
    if(path == L"")
        return false;

    List<String> tokens;
    tokenize(path, tokens, _delimiter);

    if(tokens.size() < 1)//ne devrait pas arriver puisque path n'est pas vide
        return false;

    if(_children.size() == 1 && tokens[0] != (_children[0]->getName()))//Il ne peut y avoir qu'une racine
        return false;

    return XMLNode::isPathValid(path);
}

void XMLDocument::tinyXMLToNodes(TiXmlDocument * xmlDocument)
{
    TiXmlNode * pRoot = xmlDocument->RootElement();

    if(pRoot != NULL)
    {
        Ptr<XMLNode> pNode(new XMLNode(L""));
        pNode->constructXMLTree(pRoot);
        _children.push_back(pNode);
    }
}

void XMLDocument::nodesToTinyXML(TiXmlDocument * xmlDocument)
{
    TiXmlDeclaration* xmlDeclaration = new TiXmlDeclaration( "1.0", "UTF-8", "" );
    xmlDocument->LinkEndChild(xmlDeclaration);

    if(_children.size() >= 1)
    {
        LM_ASSERT(_children.size() == 1);
        TiXmlNode * pDocument = _children[0]->browseXMLTree();
        xmlDocument->LinkEndChild(pDocument);
    }
}

bool XMLDocument::acceptChild() const
{
    return _children.size() == 0;
}

//-----------------------------------------------------------------------------
//
// Les deux méthodes suivantes sont déclarées ici pour éviter d'inclure tinyxml/tinyxml.h
// dans deux fichiers différents (ça pose problème)
//
//-----------------------------------------------------------------------------

TiXmlNode * XMLNode::browseXMLTree()
{
    TiXmlElement * pNode = new TiXmlElement(String8(_name).c_str());

    // Attributs
    for(Map<String, String>::const_iterator iAttribute = _attributes.begin(); iAttribute != _attributes.end(); ++iAttribute)
    {
        pNode->SetAttribute(String8(iAttribute->first).c_str(), String8(iAttribute->second).c_str());
    }

    // Valeur
    if(_value != String::EMPTY)
    {
        TiXmlText * pText = new TiXmlText(String8(_value).c_str());
        pNode->LinkEndChild(pText);
    }

    // Enfants
    for(List<Ptr<XMLNode> >::const_iterator ipChild = _children.begin(); ipChild != _children.end(); ++ipChild)
    {
        TiXmlNode * pChild = (*ipChild)->browseXMLTree();
        pNode->LinkEndChild(pChild);
    }

    return pNode;
}

void XMLNode::constructXMLTree(TiXmlNode * pNode)
{
    // Récupère le nom du noeud ex : <Truc>blah</Truc> retourne Truc
    _name = String(pNode->Value());

    // Recherche des fils et des valeurs textes
    TiXmlNode * pChild = pNode->FirstChild();

    while(pChild != NULL)
    {
        if(pChild->Type() == TiXmlNode::ELEMENT)
        {
            // Ajoute un fils du noeud
            Ptr<XMLNode> pChildNode(new XMLNode(String(pChild->Value()), _delimiter));
            pChildNode->constructXMLTree(pChild);
            _children.push_back(pChildNode);
        }
        else if(pChild->Type() == TiXmlNode::TEXT)
        {
            // Récupère les données du noeud
            TiXmlText * pText = static_cast<TiXmlText *>(pChild);
            _value += String(pText->Value());
        }

        pChild = pChild->NextSibling();
    }

    // Parcours attributs
    if(pNode->Type() == TiXmlNode::ELEMENT)
    {
        TiXmlElement * pElement = static_cast<TiXmlElement *>(pNode);
        TiXmlAttribute * pAttribute = pElement->FirstAttribute();

        while(pAttribute != NULL)
        {
            _attributes[String(pAttribute->Name())] = String(pAttribute->Value());
            pAttribute = pAttribute->Next();
        }
    }
}


}//namespace Core
