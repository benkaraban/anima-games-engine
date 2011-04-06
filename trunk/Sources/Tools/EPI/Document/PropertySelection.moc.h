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
#ifndef PROPERTYSELECTION_H_
#define PROPERTYSELECTION_H_

#include <EPI/Document/PropertyNode.moc.h>

#include <Core/List.h>
#include <EPI/Document/StateRecorder.h>


namespace EPI
{
class LM_API_EPI PropertySelection : public PropertyNode
{
    Q_OBJECT

public:
    virtual ~PropertySelection() {}

    virtual void select(const Ptr<Property>& pPty) = 0;
    virtual void select(const Core::List<Ptr<Property> >& pties) = 0;
    virtual void addToSelection(const Ptr<Property>& pPty) = 0;
    virtual void removeFromSelection(const Ptr<Property>& pPty) = 0;
    virtual void deselectAll() = 0;
    virtual void selectAll() = 0;

    virtual Ptr<Property> clone() const = 0;
    virtual void internalCopy(const Ptr<Property>& pSrc) = 0;

    virtual int32 getSelectedPtiesCount() const = 0;
    virtual const Ptr<Property>& getSelectedPty(int32 indice) const = 0;
    virtual const Core::List<Ptr<Property> >& getSelectedPties() const = 0;
/*
    virtual void setRootPtySelectable(const Property* root) = 0;
    virtual const Property& getRootPtySelectable() const = 0;*/

Q_SIGNALS:
    void selectionChanged();

protected:
    PropertySelection(const String& name) : PropertyNode(name, false, false, DOCUMENT_EDITION) {}
};


class LM_API_EPI PropertySelectionRoot : public PropertySelection
{
protected:
    PropertySelectionRoot(const String& name) : PropertySelection(name) {}
public:
    virtual ~PropertySelectionRoot() {}

    virtual void setRootPtySelectable(const Property* root) = 0;
    virtual const Property& getRootPtySelectable() const = 0;
};

class LM_API_EPI PropertySelectionDual : public PropertySelectionRoot
{
public:
    PropertySelectionDual(const String& name, const Ptr<PropertySelectionRoot>& pPty1, const Ptr<PropertySelectionRoot>& pPty2);
    virtual ~PropertySelectionDual();

    virtual void select(const Ptr<Property>& pPty);
    virtual void select(const Core::List<Ptr<Property> >& pties);
    virtual void addToSelection(const Ptr<Property>& pPty);
    virtual void removeFromSelection(const Ptr<Property>& pPty);
    virtual void deselectAll();
    virtual void selectAll();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual int32 getSelectedPtiesCount() const;
    virtual const Ptr<Property>& getSelectedPty(int32 indice) const;
    virtual const Core::List<Ptr<Property> >& getSelectedPties() const;

    void setUsePty1(bool flag);
    void setPty1(const Ptr<Property>& pPty);
    void setPty2(const Ptr<Property>& pPty);

    virtual bool            findPtyWithId(Ptr<Property>& pResult, int32 id);

    virtual void setRootPtySelectable(const Property* root);
    virtual const Property& getRootPtySelectable() const;

private:
    Ptr<PropertySelectionRoot> _pPty1;
    Ptr<PropertySelectionRoot> _pPty2;
    bool _usePty1;
};

class LM_API_EPI PropertySelectionDefault : public PropertySelectionRoot
{
public:
    PropertySelectionDefault(const String& name, const Property* pRootPtySelectable, const Ptr<StateRecorder>& pStateRecorder);
    virtual ~PropertySelectionDefault();

    virtual void select(const Ptr<Property>& pPty);
    virtual void addToSelectWithChildren(const Ptr<Property>& pPty);
    virtual void select(const Core::List<Ptr<Property> >& pties);
    void addToSelection(const Ptr<Property>& pPty);
    virtual void removeFromSelection(const Ptr<Property>& pPty);
    virtual void deselectAll();
    virtual void selectAll();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    

    virtual int32 getSelectedPtiesCount() const;
    virtual const Ptr<Property>& getSelectedPty(int32 indice) const;
    virtual const Core::List<Ptr<Property> >& getSelectedPties() const;

    virtual void setRootPtySelectable(const Property* root);
    virtual const Property& getRootPtySelectable() const {return *_pRootPtySelectable;}

private:
    Property*                   _pRootPtySelectable; //La racine des pty selectionnable
    Core::List<Ptr<Property> >  _selectedPties;  //les propriete selectionnees
    Core::List<int32 >          _selectedPtiesId;  //les propriete selectionnees
    Ptr<StateRecorder>          _pStateRecorder;
};

class LM_API_EPI PropertySelectionContent : public PropertySelection
{
public:
    PropertySelectionContent(const String& name, const Core::List<Ptr<Property> >* pPtiesSelectable, const Ptr<StateRecorder>& pStateRecorder);
    virtual ~PropertySelectionContent();

    
    virtual void select(const Ptr<Property>& pPty);
    void addToSelectWithChildren(const Ptr<Property>& pPty);
    virtual void select(const Core::List<Ptr<Property> >& pties);
    virtual void addToSelection(const Ptr<Property>& pPty);
    virtual void addToSelection(int32 index);
    virtual void removeFromSelection(const Ptr<Property>& pPty);
    virtual void removeFromSelection(int32 index);
    virtual void deselectAll();
    virtual void selectAll();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);
    

    virtual int32 getSelectedPtiesCount() const;
    virtual const Ptr<Property>& getSelectedPty(int32 indice) const;
    virtual const Core::List<Ptr<Property> >& getSelectedPties() const;

    void setPtiesSelectable(const Core::List<Ptr<Property> >* pPtiesSelectable);
    const Core::List<Ptr<Property> >* getPtiesSelectable() const;

private:
    Core::List<Ptr<Property> >* _pPtiesSelectable;
    Core::List<Ptr<Property> >  _selectedPties;  //les propriete selectionnees
    Core::List<int32 >          _selectedPtiesId;  //les propriete selectionnees
    Ptr<StateRecorder>          _pStateRecorder;
};
} // namespace EPI

#endif
