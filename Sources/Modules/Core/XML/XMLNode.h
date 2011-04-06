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
#ifndef CORE_XML_NODE_H_
#define CORE_XML_NODE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Map.h>
#include <Core/DateAndTime.h>

class TiXmlNode;

namespace Core
{
class XMLDocument;

class LM_API_COR XMLNode : public Object
{
public:
    XMLNode(const String & name, const String & delimiter = L".");
    virtual ~XMLNode();

    /**
     * Méthodes qui agissent directement sur le noeud.
     */
    void setValue(const String & value);
    void setAttribute(const String & name, const String & value);

    bool addNode(const Ptr<XMLNode> & pChild);

    void deleteNodes(const String & name);
    bool deleteAttribute(const String & name);

    const String & getName() const;
    void getValue(String & value) const;
    const String & getValue() const { return _value; }
    bool getAttribute(const String & name, String & value) const;

    Ptr<XMLNode> firstChildNode(const String & name) const;
    Ptr<XMLNode> getNode(const String & path) const;

    void getNodes(const String & name, List<Ptr<XMLNode> > & children) const;
    void getAllNodes(const String & path, List<Ptr<XMLNode> > & children) const;

    /**
     * Méthode permettant de définir une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] value : string que l'on souhaite écrire dans le document
     * @return true si la donnée a bien été inséré
     */
    bool setValue(  const String & path,
                    const String & value);
    /**
     * Méthode permettant de définir plusieurs valeurs (au même niveau) dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] values : tableau de strings que l'on souhaite écrire dans le document
     * @return true si la donnée a bien été inséré
     */
    bool setValues( const String & path,
                    const List<String> & values);
    /**
     * Méthode permettant de définir une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] value : entier que l'on souhaite écrire dans le document
     * @return true si la donnée a bien été inséré
     */
    bool setValue(  const String & path,
                    int32 value);

    bool setValue(  const String & path,
                    int64 value);

    bool setValue(  const String & path,
                    const TimeValue & value);
    /**
     * Méthode permettant de définir une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] value : booléen que l'on souhaite écrire dans le document
     * @return true si la donnée a bien été inséré
     */
    bool setValue(  const String & path,
                    bool value);
    /**
     * Méthode permettant de définir une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] value : valeur flottante que l'on souhaite écrire dans le document
     * @return true si la donnée a bien été inséré
     */
    bool setValue(  const String & path,
                    double value);

    /**
     * Le système de type du C++ étant une merde infâme, lorsque le compilateur tombe
     * sur setValue(L"path", L"value") il convertit (silencieusement) L"value" en bool,
     * ce qui donne des résultats assez cocasses.
     *
     * Cette conversion explicite évite donc les désagréments précités.
     */
    bool setValue(const String & path, const wchar_t * value) { return setValue(path, String(value)); };

    /**
     * Méthode permettant de définir un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut que l'on souhaite écrire dans le document
     * @param[in] value : string que l'on souhaite affecter à l'attribut
     * @param[in] specificValue : spécifie la valeur spécifique du noeud pour lequel l'attribut doit être appliqué
     * @return true si la donnée a bien été inséré
     */
    bool setAttribute(  const String & path,
                        const String & attributeName,
                        const String & attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de définir un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut que l'on souhaite écrire dans le document
     * @param[in] value : entier que l'on souhaite affecter à l'attribut
     * @param[in] specificValue : spécifie la valeur spécifique du noeud pour lequel l'attribut doit être appliqué
     * @return true si la donnée a bien été inséré
     */
    bool setAttribute(  const String & path,
                        const String & attributeName,
                        int32 attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de définir un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut que l'on souhaite écrire dans le document
     * @param[in] value : valeur flottante que l'on souhaite affecter à l'attribut
     * @param[in] specificValue : spécifie la valeur spécifique du noeud pour lequel l'attribut doit être appliqué
     * @return true si la donnée a bien été inséré
     */
    bool setAttribute(  const String & path,
                        const String & attributeName,
                        double attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de récupérer une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] value : valeur que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValue(  const String & path,
                    String & value) const;
    /**
     * Méthode permettant de récupérer plusieurs valeurs dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] values : les valeurs que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValues( const String & path,
                    List<String> & values) const;
    /**
     * Méthode permettant de récupérer une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] value : entier que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValue(  const String & path,
                    int32 & value) const;

    bool getValue(  const String & path,
                    int64 & value) const;

    bool getValue(  const String & path,
                    TimeValue & value) const;
    /**
     * Méthode permettant de récupérer une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] value : valeur flottante que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValue(  const String & path,
                    double & value) const;
    /**
     * Méthode permettant de récupérer une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] value : valeur flottante que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValue(  const String & path,
                    float & value) const;
    /**
     * Méthode permettant de récupérer une valeur dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[out] value : valeur booléenne que l'on souhaite récupérer
     * @return true si la donnée a bien été récupéré
     */
    bool getValue(  const String & path,
                    bool & value) const;
    /**
     * Méthode permettant de récupérer un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut dont on souhaite récupérer la valeur
     * @param[out] value : valeur que l'on souhaite récupérer
     * @param[in] specificValue : la valeur specifique pour laquelle on souhaite récupérer l'attribut
     * @return true si la donnée a bien été récupérer
     */
    bool getAttribute(  const String & path,
                        const String & attributeName,
                        String & attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de récupérer un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut dont on souhaite récupérer la valeur
     * @param[out] value : entier que l'on souhaite récupérer
     * @param[in] specificValue : la valeur specifique pour laquelle on souhaite récupérer l'attribut
     * @return true si la donnée a bien été récupérer
     */
    bool getAttribute(  const String & path,
                        const String & attributeName,
                        int32 & attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de récupérer un attribut dans le document XML
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : nom de l'attribut dont on souhaite récupérer la valeur
     * @param[out] value : valeur flottante que l'on souhaite récupérer
     * @param[in] specificValue : la valeur specifique pour laquelle on souhaite récupérer l'attribut
     * @return true si la donnée a bien été récupérer
     */
    bool getAttribute(  const String & path,
                        const String & attributeName,
                        double & attributeValue,
                        const String & specificValue = L"");
    /**
     * Méthode permettant de supprimer un noeud ainsi que tous ces fils
     * @param[in] path : chemin jusqu'au parent du noeud à supprimer
     * @param[in] nodeName : le nom du noeud à supprimer
     * @return true si le noeud a bien été supprimé
     */
    bool deleteNode(const String & path,
                    const String & nodeName);
    /**
     * Méthode permettant de supprimer un attribut
     * @param[in] path : chemin dans l'arborescence du fichier XML
     * @param[in] attributeName : le nom de l'attribut à supprimer
     * @param[in] specificValue : la valeur specifique pour laquelle on souhaite supprimer l'attribut
     * @return true si le noeud a bien été supprimé
     */
    bool deleteAttribute(   const String & path,
                            const String & attributeName,
                            const String & specificValue = L"");
    /**
     * Méthode permettant d'ajouter un XMLNode de l'arborescence
     * @param[in] path : chemin jusqu'au parent du noeud à ajouter
     */
    bool addNode(   const String & path,
                    const Ptr<XMLNode> & node);
    /**
     * Méthode permettant de récupérer un XMLNode
     * @param[in] path : chemin jusqu'au parent du noeud à ajouter
     * @param[in] nodeName : le nom du noeud que l'on souhaite ajouter
     */
    bool getNodes(  const String & path,
                    const String & nodeName,
                    List<Ptr<XMLNode> > & node) const;

protected:
    virtual bool isPathValid(const String & path) const;
    virtual bool acceptChild() const;
    void getChildNameAndNewPath(const String & path, String & childName, String & newPath) const;
    void constructXMLTree(TiXmlNode * pNode);
    TiXmlNode * browseXMLTree();

protected:
    String                 _name;
    String                 _delimiter;
    String                 _value;
    Map<String, String>    _attributes;
    List<Ptr<XMLNode> >    _children;

    friend class XMLDocument;
};

}//namespace Core

#endif/*CORE_XML_NODE_H_*/
