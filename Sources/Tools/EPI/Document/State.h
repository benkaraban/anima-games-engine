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
#ifndef STATE_H_
#define STATE_H_

#include <Core/Standard.h>
#include <EPI/Document/Property.moc.h> 

#include <EPI/Document/Properties/PtyNode.moc.h>

namespace EPI
{
//class Property;
class DocumentBase;
/**
* IState est l'interface au classe permettant de d�crire l'etat d'un document
*
* Pour chaque action les etat existe en deux exemplaire undo et redo. 
* L'etat undo permet de retrouver le document avant l'action
* L'etat redo permet d'appliquer la modification si un undo a ete realise
*/
class LM_API_EPI IState : public Core::Object
{
public:
    virtual ~IState();
    virtual void restoreState(DocumentBase& pDoc) = 0;
    virtual void restoreStateOnProperty(Ptr<Property>& pPty) = 0;
    virtual String toString() const = 0;

protected:
    IState();
};

/**
* StatePropertyChanged : permet de sockque un changement d'etat ayant pour but de modifier la valeur d'une propriete
*/
class LM_API_EPI StatePropertyChanged : public IState
{
public:
    StatePropertyChanged(const Property& pty);
    virtual ~StatePropertyChanged();
    virtual void restoreState(DocumentBase& doc);
    virtual void restoreStateOnProperty(Ptr<Property>& pPty);
    virtual String toString() const;

private:
    Ptr<Property> _pPty;
    int32 _docPtyId;    //l'id de la property dans le document
};

class LM_API_EPI StatePropertyDestroy : public IState
{
public:
    StatePropertyDestroy(const Property& pty);
    virtual ~StatePropertyDestroy();
    virtual void restoreState(DocumentBase& doc);
    virtual void restoreStateOnProperty(Ptr<Property>& pPty);
    virtual String toString() const;

private:
    int32 _docPtyId;        //l'id de la property dans le document
    int32 _docParentPtyId;  //l'id de la property parent dans le document
};

class LM_API_EPI StatePropertyCreated : public IState
{
public:
    StatePropertyCreated(const Property& pty);
    virtual ~StatePropertyCreated();
    virtual void restoreState(DocumentBase& doc);
    virtual void restoreStateOnProperty(Ptr<Property>& pPty);
    virtual String toString() const;

private:
    Ptr<Property> _pPty;
    int32 _docParentPtyId;  //l'id de la property parent dans le document
};


class LM_API_EPI StatePropertyDestroyContent : public IState
{
public:
    StatePropertyDestroyContent(const Property& pty, const PtyNodeEditableContent& ptyContent);
    virtual ~StatePropertyDestroyContent();
    virtual void restoreState(DocumentBase& doc);
    virtual void restoreStateOnProperty(Ptr<Property>& pPty);
    virtual String toString() const;

private:
    int32 _ptyIndexContent;        //ll'index dans la list des ptyContent
    int32 _docContentPtyId;  //l'id de la property content dans le document
};

class LM_API_EPI StatePropertyCreatedContent : public IState
{
public:
    StatePropertyCreatedContent(const Property& pty, const PtyNodeEditableContent& ptyContent);
    virtual ~StatePropertyCreatedContent();
    virtual void restoreState(DocumentBase& doc);
    virtual void restoreStateOnProperty(Ptr<Property>& pPty);
    virtual String toString() const;

private:
    Ptr<Property> _pPty;
    int32 _docContentPtyId;  //l'id de la property content dans le document
    int32 _ptyIndexContent;        //ll'index dans la list des ptyContent
};
} // namespace EPI

#endif STATE_H_