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
#include <EPI/Document/State.h>

#include <EPI/Document/DocumentBase.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
IState::IState()
{
}
//-----------------------------------------------------------------------------
IState::~IState()
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StatePropertyChanged::StatePropertyChanged(const Property& pty)
{
    _pPty     = pty.clone();
    _docPtyId = pty.getId();
}
//-----------------------------------------------------------------------------
StatePropertyChanged::~StatePropertyChanged()
{
}
//-----------------------------------------------------------------------------
void StatePropertyChanged::restoreState(DocumentBase& doc)
{
    Ptr<Property> pDocPty = doc.getProperty(_docPtyId);
    LM_ASSERT(pDocPty!=null);
    restoreStateOnProperty(pDocPty);
}
//-----------------------------------------------------------------------------
void StatePropertyChanged::restoreStateOnProperty(Ptr<Property>& pPty)
{
    Property* pParent = pPty->getParent();
    int32     docId   = pPty->getDocId();

    pPty->copy(_pPty);

    pPty->setParent(pParent);
    pPty->setDocId(docId);
}
//-----------------------------------------------------------------------------
String StatePropertyChanged::toString() const
{
    String str;

    str<<L"State Changed : "<<L"Id:"<<_docPtyId<<L" Pty:"<<_pPty->toString();

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StatePropertyDestroy::StatePropertyDestroy(const Property& pty)
{
    LM_ASSERT(pty.getParent() != NULL);

    _docPtyId       = pty.getId();                  
    _docParentPtyId = pty.getParent()->getId();     
}
//-----------------------------------------------------------------------------
StatePropertyDestroy::~StatePropertyDestroy()
{
}
//-----------------------------------------------------------------------------
void StatePropertyDestroy::restoreState(DocumentBase& doc)
{
    Ptr<Property> pParentPty = doc.getProperty(_docParentPtyId);

    Ptr<Property> pChild;
    bool res = pParentPty->findPtyWithId(pChild, _docPtyId);

    LM_ASSERT(res==true);
    pParentPty->removeChild(pChild);
    doc.removeContent(*pChild);
    pParentPty->launchSignalProeprtyChanged(true);
}
//-----------------------------------------------------------------------------
void StatePropertyDestroy::restoreStateOnProperty(Ptr<Property>& pPty)
{
    throw Core::Exception(L"StatePropertyDestroy::restoreStateOnProperty");
}
//-----------------------------------------------------------------------------
String StatePropertyDestroy::toString() const
{
    String str;
    str<<L"State Destroy : "<<L"Id:"<<_docPtyId<<L" ParentId:"<<_docParentPtyId;
        
    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StatePropertyCreated::StatePropertyCreated(const Property& pty)
{
    _docParentPtyId = pty.getParent()->getId();
    cloneWithChild(_pPty, pty, true);
}
//-----------------------------------------------------------------------------
StatePropertyCreated::~StatePropertyCreated()
{
}
//-----------------------------------------------------------------------------
void StatePropertyCreated::restoreState(DocumentBase& doc)
{
    Ptr<Property> pParentPty = doc.getProperty(_docParentPtyId);

    Ptr<Property> pChild;
    bool res = pParentPty->findPtyWithId(pChild, _pPty->getId());

    LM_ASSERT(res == false);

    cloneWithChild(pChild, *_pPty, true);
    pChild->resurrect(doc.getWorldDocument(), doc.getWorldInfoContent());

    pParentPty->addChild(pChild);
    doc.addContent(pChild);
    pParentPty->launchSignalProeprtyChanged(true);
}
//-----------------------------------------------------------------------------
void StatePropertyCreated::restoreStateOnProperty(Ptr<Property>& pPty)
{
    throw Core::Exception(L"StatePropertyCreated::restoreStateOnProperty");
}
//-----------------------------------------------------------------------------
String StatePropertyCreated::toString() const
{
    String str;

    str<<L"State Created : "<<L"Parent Id:"<<_docParentPtyId<<L" Pty:"<<_pPty->toString();

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StatePropertyDestroyContent::StatePropertyDestroyContent(const Property& pty, const PtyNodeEditableContent& ptyContent)
{
    const Core::List<Ptr<Property> >& ptiesContent = ptyContent.getPtiesContentSelectable();

    int32 ii=0;
    for (; ii<ptiesContent.size(); ++ii)
    {
        if (ptiesContent[ii].get() == &pty)
        {
            break;
        }
    }

    LM_ASSERT(ii<ptiesContent.size());

    _ptyIndexContent = ii;

    _docContentPtyId = ptyContent.getId();
}
//-----------------------------------------------------------------------------
StatePropertyDestroyContent::~StatePropertyDestroyContent()
{
}
//-----------------------------------------------------------------------------
void StatePropertyDestroyContent::restoreState(DocumentBase& doc)
{
    Ptr<PtyNodeEditableContent> pContentPty = LM_DEBUG_PTR_CAST<PtyNodeEditableContent>(doc.getProperty(_docContentPtyId));

    pContentPty->removePtyContent(_ptyIndexContent);
    pContentPty->updateData();

    pContentPty->launchSignalProeprtyChanged(true);
}
//-----------------------------------------------------------------------------
void StatePropertyDestroyContent::restoreStateOnProperty(Ptr<Property>& pPty)
{
    throw Core::Exception(L"StatePropertyDestroyContent::restoreStateOnProperty");
}
//-----------------------------------------------------------------------------
String StatePropertyDestroyContent::toString() const
{
    String str;

    str<<L"State Destroy Content : " << L"Index : " << _ptyIndexContent << L" ContentPtyId : " << _docContentPtyId;

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StatePropertyCreatedContent::StatePropertyCreatedContent(const Property& pty, const PtyNodeEditableContent& ptyContent)
{
    const Core::List<Ptr<Property> >& ptiesContent = ptyContent.getPtiesContentSelectable();

    _docContentPtyId = ptyContent.getId();

    int32 ii=0;
    for (; ii<ptiesContent.size(); ++ii)
    {
        if (ptiesContent[ii].get() == &pty)
        {
            break;
        }
    }

    if (ii<ptiesContent.size()) _ptyIndexContent = ii;
    else                        _ptyIndexContent = -1;

    cloneWithChild(_pPty, pty, true);
}
//-----------------------------------------------------------------------------
StatePropertyCreatedContent::~StatePropertyCreatedContent()
{
}
//-----------------------------------------------------------------------------
void StatePropertyCreatedContent::restoreState(DocumentBase& doc)
{
    Ptr<PtyNodeEditableContent> pContentPty = LM_DEBUG_PTR_CAST<PtyNodeEditableContent>(doc.getProperty(_docContentPtyId));

    Ptr<Property> pPty;

    cloneWithChild(pPty, *_pPty, true);
    pPty->resurrect(doc.getWorldDocument(), doc.getWorldInfoContent(), pContentPty);

    pContentPty->insertPtyContent(pPty, _ptyIndexContent);
    pContentPty->updateData();
    pContentPty->launchSignalProeprtyChanged(true);
}
//-----------------------------------------------------------------------------
void StatePropertyCreatedContent::restoreStateOnProperty(Ptr<Property>& pPty)
{
    throw Core::Exception(L"StatePropertyCreatedContent::restoreStateOnProperty");
}
//-----------------------------------------------------------------------------
String StatePropertyCreatedContent::toString() const
{
    String str;

    str<<L"State Created Content : " << L"Index : " << _ptyIndexContent << L" ContentPtyId : " << _docContentPtyId;

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI
