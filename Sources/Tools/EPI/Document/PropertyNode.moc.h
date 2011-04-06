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
#ifndef PROPERTYNODE_H_
#define PROPERTYNODE_H_

#include <EPI/Document/Property.moc.h>


namespace Core
{
template <class TElement> class List;
}

namespace EPI
{
/**
* La classe PropertyNode permet de définir une propriete qui peut contenir des proprietes enfants
*/
class LM_API_EPI PropertyNode : public Property
{
    Q_OBJECT

public:
    ///
    PropertyNode(const Core::List<Ptr<Property> >& children = Core::List<Ptr<Property> >(), const EPropertyType type = CONTENT);
    ///
    PropertyNode(const Core::String& name, bool isSelectable = false, bool isDeletable = true, const EPropertyType type = CONTENT);
    ///
    PropertyNode(const PropertyNode& other);
    ///
    virtual  ~PropertyNode();

    ///Constructeur de copie virtuel
    virtual Ptr<Property> clone() const;
    virtual void copy(const Ptr<Property>& pSrc);
    virtual void internalCopy(const Ptr<Property>& pSrc);

     ///Ajout d'un enfant : comportement par defaut
    virtual void addChild(Ptr<Property> pChild, int32 indexPos=-1);
    //void 

    ///Suppression d'un enfant : comportement par defaut
    virtual void removeChild(Ptr<Property> pChild);
    virtual void removeAllChildren();
    virtual void removeChild(const int32 index);

    ///Retourne un enfant : comportement par defaut
    virtual const Ptr<Property>& getChild(const int32 index) const;
    virtual Ptr<Property>& getChild(const int32 index);
    const Core::List<Ptr<Property> >& getChildren() const;
    Core::List<Ptr<Property> >& getChildren();

    ///Retourne le nombre d'enfants : comportement par defaut
    virtual int32 getNbChildren() const;
    virtual void setDocId(const int32 docId);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);

    virtual void resurrect(const Ptr<Universe::World>& pWorld = null, const Ptr<Universe::World>& pWorldInfoContent = null, const Ptr<Property>& pty = null);

    virtual void restart();
    virtual void stop();

private:
    ///Désactive l'opérateur d'assignation
    Property& operator=( const Property& other );

private:
    class implementation;
	implementation* const _impl;
};

LM_API_EPI Ptr<Property>  getChildRecursive2(const PropertyNode& pty, int32 id);
LM_API_EPI void intersect_RayPropertyNodeRecursive(const Property& pPtyRoot, const Core::Rayf& ray, float& distance, Ptr<Property>& pPtyIntersecting);
} // namespace EPI

#endif PROPERTYNODE_H_