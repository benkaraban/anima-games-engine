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
#ifndef CORE_XML_DOCUMENT_H_
#define CORE_XML_DOCUMENT_H_

#include <Core/XML/XMLNode.h>
#include <Core/IO/Streams.h>

class TiXmlDocument;
class TiXmlNode;

namespace Core
{

/**
 * Cette classe permet de manipuler les fichiers XML. Ces fichiers sont encodés en UTF-8.
 * Cette classe utilise en interne la librairie TinyXML.
 * La documentation de cette librairie peut être trouvée à l'adresse :
 * http://www.grinninglizard.com/tinyxmldocs/index.html
 *
 * L'idée de cette classe est de permettre de manipuler très simplement les fichiers XML.
 * Plus précisement XMLDocument hérite de XMLNode. L'arborescence de XMLNode représente le fichier
 * XML correspondant.
 * Le XMLDocument représente la racine et présente en plus des XMLNode les opérations de sauvegarde et de
 * chargement.
 * Tout d'abord il faut définir un délimiteur qui servira pour manipuler le document par la suite.
 * Par défaut il s'agit du point.
 * @code
 * XMLDocument xmlDocument(L".");
 * //équivalent à
 * //XMLDocument xmlDocument;
 * @endcode
 * Ici nous avons spécifié le délimiteur par défaut en l'occurence le point.
 * Imaginons que nous souhaitons produire le document XML suivant :
 * @code
 * <?xml version="1.0" encoding="UTF-8" ?>
 * <ConfigGfx Détail="High" Stuff="17">
 *     <Résolution depth="32">
 *         <width>1680</width>
 *         <height>1050</height>
 *     </Résolution>
 * </ConfigGfx>
 * @endcode
 * Il nous suffit pour cela d'écrire les lignes suivantes :
 * @code
 * xmlDocument.setAttribute(L"ConfigGfx",L"Detail", L"High");
 * xmlDocument.setAttribute(L"ConfigGfx",L"Stuff", 17);
 * xmlDocument.setValue(L"ConfigGfx.Resolution.width", 1680);
 * xmlDocument.setValue(L"ConfigGfx.Resolution.height", 1050);
 * xmlDocument.setAttribute(L"ConfigGfx.Resolution",L"depth", 32);
 * @endcode
 * Pour chacune des commandes setValue ou setAttribute le délimiteur est utilisé pour
 * définir le niveau de pronfondeur dans l'arborescence du document XML.
 * A noter que la hierarchie est créée dynamiquement avec les appels.
 * Il n'y a qu'une seule règle à respecter :
 * UN DOCUMENT XML NE PEUT AVOIR QU'UNE SEULE RACINE.
 * Si on ajoute à la série de commandes précédentes la ligne suivante :
 * @code
 * xmlDocument.setValue(L"Ne.Marchera.Pas", L"Du tout");
 * @endcode
 * la méthode setValue retournera false et la commande sera ignorée.
 * Pour finir il suffit de faire appel à la méthode saveDocument pour sauvegarder le
 * fichier XML sur le disque dur.
 *
 * La récupération des données à partir d'un fichier XML se fait de manière analogue :
 * @code
 * xmlDocument.loadDocument(L"appSettings.xml");
 * xmlDocument.getAttribute(L"ConfigGfx",L"Detail", detail);
 * xmlDocument.getAttribute(L"ConfigGfx",L"Stuff", stuff);
 * xmlDocument.getValue(L"ConfigGfx.Resolution.width", width);
 * xmlDocument.getValue(L"ConfigGfx.Resolution.height", height);
 * xmlDocument.getAttribute(L"ConfigGfx.Resolution",L"depth", depth);
 * @endcode
 */
class LM_API_COR XMLDocument : public XMLNode
{
public:
    /**
     * Contructeur du XMLDocument
     * @param[in] delimiter : définit le délimiteur (L"." par défaut).
     */
    XMLDocument(const String & delimiter = L".");
    
    XMLDocument(InputStream& in, const String & delimiter = L".");

    virtual ~XMLDocument();
    /**
     * Méthode permettant de sauvegarder le document XML
     * @param[in] fileName : Nom du fichier
     * @return true si le fichier a bien été sauvegardé
     */
    bool saveDocument(const String & fileName);
    /**
     * Méthode permettant de sauvegarder le document XML
     * @param[in] outputStream : stream dans lequel les données doivent être sauvegardée
     * @return true si le stream a bien été écrit
     */
    bool saveDocument(OutputStream & outputStream);
    /**
     * Méthode permettant de charger un document XML
     * @param[in] fileName : Nom du fichier
     * @return true si le fichier a bien été chargé
     */
    bool loadDocument(const String & fileName);
    /**
     * Méthode permettant de charger un document XML
     * @param[in] inputStream : stream à partir duquel on doit effectuer la lecture
     * @return true si la stream a bien été chargé
     */
    bool loadDocument(InputStream & inputStream);

    bool saveToString(String & value);

protected:
    virtual bool isPathValid(const String & path) const;
    virtual bool acceptChild() const;
private:
    void nodesToTinyXML(TiXmlDocument * xmlDocument);
    void tinyXMLToNodes(TiXmlDocument * xmlDocument);
};

}//namespace Core

#endif/*CORE_XML_DOCUMENT_H_*/
