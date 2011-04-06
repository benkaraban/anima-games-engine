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
#ifndef PROPERTY_DOCUMENT_EDITION_H_
#define PROPERTY_DOCUMENT_EDITION_H_

#include <Core/Standard.h>
#include <EPI/Document/PropertyNode.moc.h>
#include <EPI/Document/StateRecorder.h>

namespace EPI
{

/*
//-----------------------------------------------------------------------------
class LM_API_EPI DocumentPropertySelection : public PropertySelection
{
    Q_OBJECT

public:
    DocumentPropertySelection(const String& name, const Ptr<Property>& pRootPtySelectable, const Ptr<StateRecorder>& pStateRecorder);
    virtual ~DocumentPropertySelection();

    virtual void select(const Ptr<Property>& pPty);
    virtual void select(const Core::List<Ptr<Property> >& pties);
    virtual void addToSelection(const Ptr<Property>& pPty);
    virtual void removeFromSelection(const Ptr<Property>& pPty);
    virtual void deselectAll();

    virtual Ptr<Property> clone() const;
    virtual void internalCopy(const Ptr<Property>& pSrc);

Q_SIGNALS:
    void selectionChanged();

private:
   // Ptr<Property>               _pRootPtySelectable; //La racine des pty selectionnable
   // Core::List<Ptr<Property> >  _selectedPties;  //les propriete selectionnees
  //  Core::List<int32 >          _selectedPtiesId;  //les propriete selectionnees
    Ptr<StateRecorder>          _pStateRecorder;
};*/
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
class LM_API_EPI PtyDocEdition : public PropertyNode
{
    Q_OBJECT

public:
    PtyDocEdition(const String& name, const Ptr<Property>& pRootPtySelectable, const Ptr<StateRecorder>& pStateRecorder);
    virtual ~PtyDocEdition();

    void changePropertySelection(const Ptr<Property>& pPtySelection);

    const Ptr<Property>& getPtySelection() const;
    Ptr<Property>& getPtySelection();
};
//-----------------------------------------------------------------------------
} // namespace EPI
#endif // PROPERTY_DOCUMENT_EDITION_H_