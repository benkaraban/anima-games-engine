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
#include <EPI/Document/PropertySelection.moc.h>

namespace EPI
{


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertySelectionDefault::PropertySelectionDefault(const String& name, const Property* pRootPtySelectable, const Ptr<StateRecorder>& pStateRecorder) :
    PropertySelectionRoot(name),
    _pStateRecorder(pStateRecorder)
{
    LOG_FCT_NAME

    _pRootPtySelectable = const_cast<Property*>(pRootPtySelectable);
}
//-----------------------------------------------------------------------------
PropertySelectionDefault::~PropertySelectionDefault()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::setRootPtySelectable(const Property* root)
{
    LOG_FCT_NAME

    _pRootPtySelectable = const_cast<Property*>(root);
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::select(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }

    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
        pUndoState = Ptr<IState> ((IState*) new StatePropertyChanged (*this));

    _selectedPties.clear();
    _selectedPtiesId.clear();

    _selectedPties.push_back(pPty);
    pPty->setSelected(true);
    _selectedPtiesId.push_back(pPty->getId());

    if (_pStateRecorder->isRecording() == false)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::select(const Core::List<Ptr<Property> >& pties)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }

    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
        pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

    _selectedPties.clear();
    _selectedPtiesId.clear();

    _selectedPties = pties;
    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(true);
        _selectedPtiesId.push_back(_selectedPties[ii]->getId());
    }

    if (_pStateRecorder->isRecording() == false)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::addToSelection(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    if (std::find(_selectedPties.begin(),  _selectedPties.end(), pPty) == _selectedPties.end())
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        pPty->setSelected(true);
        _selectedPties.push_back(pPty);
        _selectedPtiesId.push_back(pPty->getId());

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::removeFromSelection(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Core::List<Ptr<Property> >::iterator it = std::find(_selectedPties.begin(),  _selectedPties.end(), pPty);
    Core::List<int32>::iterator itId = std::find(_selectedPtiesId.begin(),  _selectedPtiesId.end(), pPty->getId());

    if ( it != _selectedPties.end())
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        (*(*it)).setSelected(true);
        _selectedPties.erase(it);
        _selectedPtiesId.erase(itId);

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::deselectAll()
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    if (getSelectedPtiesCount() != 0)
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        for (int32 ii=0; ii<_selectedPties.size(); ++ii)
        {
            _selectedPties[ii]->setSelected(false);
        }

        _selectedPties.clear();
        _selectedPtiesId.clear();

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::addToSelectWithChildren(const Ptr<Property>& pPty)
{
    if (pPty->isDeletable() == true)
        addToSelection(pPty);

    for (int32 ii=0; ii<pPty->getNbChildren(); ++ii)
    {
        addToSelectWithChildren(pPty->getChild(ii));
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::selectAll()
{
    bool closeRecording = false;
    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
    {
        _pStateRecorder->startRecording();
        closeRecording = true;
        pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));
    }

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }


    _selectedPties.clear();
    _selectedPtiesId.clear();

    for(int32 ii=0; ii<_pRootPtySelectable->getNbChildren(); ++ii)
    {
        addToSelectWithChildren(_pRootPtySelectable->getChild(ii));
    }

    if (closeRecording == true)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
        _pStateRecorder->stopRecording();
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
Ptr<Property> PropertySelectionDefault::clone() const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Ptr<PropertySelectionDefault> p (new PropertySelectionDefault( *this ));
    p->_selectedPties.clear();  //on supprime les pointeurs vers les pties mais on garde les id
    return p;
}
//-----------------------------------------------------------------------------
void PropertySelectionDefault::internalCopy(const Ptr<Property>& pSrc)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Ptr<PropertySelectionDefault> pPty = LM_DEBUG_PTR_CAST<PropertySelectionDefault>(pSrc);
    
    _selectedPtiesId = pPty->_selectedPtiesId;
    _pStateRecorder = pPty->_pStateRecorder;
    _pRootPtySelectable = pPty->_pRootPtySelectable;

    //on recup�re les pointeur vers les pty
    //Ptr<PropertyNode> pPtyNodeRoot = LM_DEBUG_PTR_CAST<PropertyNode> (_pRootPtySelectable);
    PropertyNode* pPtyNodeRoot = LM_DEBUG_CAST<PropertyNode*> (_pRootPtySelectable);

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }

    _selectedPties.clear();
    for (int32 ii=0; ii<_selectedPtiesId.size(); ++ii)
    {
        Ptr<Property>   pPtySelected;

        if (pPtyNodeRoot->findPtyWithId(pPtySelected, _selectedPtiesId[ii]) == true)
        {
            pPtySelected->setSelected(true);
            _selectedPties.push_back(pPtySelected);
        }
    }

    //////////
    //test
 /*   _selectedPtiesId.clear();
    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPtiesId.push_back(_selectedPties[ii]->getId());
    }*/
    //////////

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
int32 PropertySelectionDefault::getSelectedPtiesCount() const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());
    return _selectedPties.size();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertySelectionDefault::getSelectedPty(int32 indice) const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    return _selectedPties[indice];
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertySelectionDefault::getSelectedPties() const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());
    return _selectedPties;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertySelectionDual::PropertySelectionDual(const String& name, const Ptr<PropertySelectionRoot>& pPty1, const Ptr<PropertySelectionRoot>& pPty2) : 
    PropertySelectionRoot(name), 
    _usePty1(true),  
    _pPty1(pPty1), 
    _pPty2(pPty2)
{
    if (_pPty1!=null)
        connect(_pPty1.get(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}
//-----------------------------------------------------------------------------
PropertySelectionDual::~PropertySelectionDual()
{
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::select(const Ptr<Property>& pPty)
{
    if (_usePty1==true)  _pPty1->select(pPty);
    else                 _pPty2->select(pPty);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::select(const Core::List<Ptr<Property> >& pties)
{
    if (_usePty1==true)  _pPty1->select(pties);
    else                 _pPty2->select(pties);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::addToSelection(const Ptr<Property>& pPty)
{
    if (_usePty1==true)  _pPty1->addToSelection(pPty);
    else                 _pPty2->addToSelection(pPty);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::removeFromSelection(const Ptr<Property>& pPty)
{
    if (_usePty1==true)  _pPty1->removeFromSelection(pPty);
    else                 _pPty2->removeFromSelection(pPty);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::deselectAll()
{
    if (_usePty1==true)  _pPty1->deselectAll();
    else                 _pPty2->deselectAll();
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::selectAll()
{
    if (_usePty1==true)  _pPty1->selectAll();
    else                 _pPty2->selectAll();
}
//-----------------------------------------------------------------------------
Ptr<Property> PropertySelectionDual::clone() const
{
    Ptr<PropertySelectionDual> p (new PropertySelectionDual( *this ));

    if (_pPty1!=null)    p->_pPty1 = LM_DEBUG_PTR_CAST<PropertySelectionDual>(_pPty1->clone());
    else                 p->_pPty1 = null;

    if (_pPty2!=null)    p->_pPty2 = LM_DEBUG_PTR_CAST<PropertySelectionDual>(_pPty2->clone());
    else                 p->_pPty2 = null;

    return p;
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PropertySelectionDual>  pS = LM_DEBUG_PTR_CAST<PropertySelectionDual>(pSrc);

    _usePty1 = pS->_usePty1;

    if (pS->_pPty1 == null)    _pPty1 = null;
    else if (_pPty1!= null)    _pPty1->copy(pS->_pPty1);
    else                       _pPty1 = LM_DEBUG_PTR_CAST<PropertySelectionDual>(pS->_pPty1->clone());

    if (pS->_pPty2 == null)    _pPty2 = null;
    else if (_pPty2!= null)    _pPty2->copy(pS->_pPty2);
    else                       _pPty2 = LM_DEBUG_PTR_CAST<PropertySelectionDual>(pS->_pPty2->clone());
}
//-----------------------------------------------------------------------------
int32 PropertySelectionDual::getSelectedPtiesCount() const
{
    int32 ret=0;
    if (_usePty1==true)  ret = _pPty1->getSelectedPtiesCount();
    else                 ret = _pPty2->getSelectedPtiesCount();
    return ret;
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertySelectionDual::getSelectedPty(int32 indice) const
{
    if (_usePty1==true)  return _pPty1->getSelectedPty(indice);
    return _pPty2->getSelectedPty(indice);
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertySelectionDual::getSelectedPties() const
{
    if (_usePty1==true)  return _pPty1->getSelectedPties();
    return _pPty2->getSelectedPties();
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::setPty1(const Ptr<Property>& pPty)
{
    _pPty1 = LM_DEBUG_PTR_CAST<PropertySelectionRoot>(pPty);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::setPty2(const Ptr<Property>& pPty)
{
    if (pPty==null) _pPty2 = null;
    else _pPty2 = LM_DEBUG_PTR_CAST<PropertySelectionRoot>(pPty);
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::setUsePty1(bool flag)
{
    if (_usePty1==true && _pPty1!=null)
        disconnect(_pPty1.get(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    if (_usePty1==false && _pPty2!=null)
        disconnect(_pPty2.get(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    _usePty1 = flag;

    if (_usePty1==true && _pPty1!=null)
        connect(_pPty1.get(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

    if (_usePty1==false && _pPty2!=null)
        connect(_pPty2.get(), SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}
//-----------------------------------------------------------------------------
bool PropertySelectionDual::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (_pPty1 != null && _pPty1->getId() == id) {pResult= _pPty1; return true;}
    if (_pPty2 != null && _pPty2->getId() == id) {pResult= _pPty2; return true;}
    return false;
}
//-----------------------------------------------------------------------------
void PropertySelectionDual::setRootPtySelectable(const Property* root)
{
     if (_usePty1==true)    _pPty1->setRootPtySelectable(root);
     else                   _pPty2->setRootPtySelectable(root);
}
//-----------------------------------------------------------------------------
const Property& PropertySelectionDual::getRootPtySelectable() const
{
    if (_usePty1==true)  return _pPty1->getRootPtySelectable();
    return _pPty2->getRootPtySelectable();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertySelectionContent::PropertySelectionContent(const String& name, const Core::List<Ptr<Property> >* pPtiesSelectable, const Ptr<StateRecorder>& pStateRecorder):
    PropertySelection(name),
    _pStateRecorder(pStateRecorder)
{
    LOG_FCT_NAME

    _pPtiesSelectable = const_cast<Core::List<Ptr<Property> >*>(pPtiesSelectable);
}
//-----------------------------------------------------------------------------
PropertySelectionContent::~PropertySelectionContent()
{
    LOG_FCT_NAME
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::select(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }

    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
        pUndoState = Ptr<IState> ((IState*) new StatePropertyChanged (*this));

    _selectedPties.clear();
    _selectedPtiesId.clear();

    _selectedPties.push_back(pPty);
    pPty->setSelected(true);
    _selectedPtiesId.push_back(pPty->getId());

    if (_pStateRecorder->isRecording() == false)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::select(const Core::List<Ptr<Property> >& pties)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }

    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
        pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

    _selectedPties.clear();
    _selectedPtiesId.clear();

    _selectedPties = pties;
    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(true);
        _selectedPtiesId.push_back(_selectedPties[ii]->getId());
    }

    if (_pStateRecorder->isRecording() == false)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::addToSelection(int32 index)
{
    addToSelection((*getPtiesSelectable())[index]);
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::addToSelection(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    if (std::find(_selectedPties.begin(),  _selectedPties.end(), pPty) == _selectedPties.end())
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        pPty->setSelected(true);
        _selectedPties.push_back(pPty);
        _selectedPtiesId.push_back(pPty->getId());

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::removeFromSelection(const Ptr<Property>& pPty)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Core::List<Ptr<Property> >::iterator it = std::find(_selectedPties.begin(),  _selectedPties.end(), pPty);
    Core::List<int32>::iterator itId = std::find(_selectedPtiesId.begin(),  _selectedPtiesId.end(), pPty->getId());

    if ( it != _selectedPties.end())
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        (*(*it)).setSelected(false);
        _selectedPties.erase(it);
        _selectedPtiesId.erase(itId);

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::removeFromSelection(int32 index)
{
    removeFromSelection((*getPtiesSelectable())[index]);
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::deselectAll()
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    if (getSelectedPtiesCount() != 0)
    {
        Ptr<IState> pUndoState;
        if (_pStateRecorder->isRecording() == false)
            pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));

        for (int32 ii=0; ii<_selectedPties.size(); ++ii)
        {
            _selectedPties[ii]->setSelected(false);
        }

        _selectedPties.clear();
        _selectedPtiesId.clear();

        if (_pStateRecorder->isRecording() == false)
        {
            Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
            _pStateRecorder->recording(pUndoState, pRedoState);
        }

        emit selectionChanged();
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::addToSelectWithChildren(const Ptr<Property>& pPty)
{
    if (pPty->isLeaf() == false)
        addToSelection(pPty);

    for (int32 ii=0; ii<pPty->getNbChildren(); ++ii)
    {
        addToSelectWithChildren(pPty->getChild(ii));
    }
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::selectAll()
{
    bool closeRecording = false;
    Ptr<IState> pUndoState;
    if (_pStateRecorder->isRecording() == false)
    {
        _pStateRecorder->startRecording();
        closeRecording = true;
        pUndoState = Ptr<IState>  ((IState*) new StatePropertyChanged (*this));
    }

    for (int32 ii=0; ii<_selectedPties.size(); ++ii)
    {
        _selectedPties[ii]->setSelected(false);
    }


    _selectedPties.clear();
    _selectedPtiesId.clear();

    for(int32 ii=0; ii<_pPtiesSelectable->size(); ++ii)
    {
        addToSelectWithChildren(_pPtiesSelectable->at(ii));
    }

    if (closeRecording == true)
    {
        Ptr<IState> pRedoState ((IState*) new StatePropertyChanged (*this));
        _pStateRecorder->recording(pUndoState, pRedoState);
        _pStateRecorder->stopRecording();
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
Ptr<Property> PropertySelectionContent::clone() const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Ptr<PropertySelectionContent> p (new PropertySelectionContent( *this ));
    p->_selectedPties.clear();  //on supprime les pointeurs vers les pties mais on garde les id
    return p;
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::internalCopy(const Ptr<Property>& pSrc)
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    Ptr<PropertySelectionContent> pPty = LM_DEBUG_PTR_CAST<PropertySelectionContent>(pSrc);
    
    _selectedPtiesId = pPty->_selectedPtiesId;
    _pStateRecorder = pPty->_pStateRecorder;
    _pPtiesSelectable = pPty->_pPtiesSelectable;

    for (int32 iPty=0; iPty<getPtiesSelectable()->size(); ++iPty)
    {
        (*getPtiesSelectable())[iPty]->setSelected(false);
    }

    _selectedPties.clear();
    for (int32 ii=0; ii<_selectedPtiesId.size(); ++ii)
    {
        Ptr<Property>   pPtySelected;

        for (int32 iPty=0; iPty<getPtiesSelectable()->size(); ++iPty)
        {
            if ((*getPtiesSelectable())[iPty]->getId() == _selectedPtiesId[ii])
            {
                (*getPtiesSelectable())[iPty]->setSelected(true);
                _selectedPties.push_back((*getPtiesSelectable())[iPty]);
            }
        }
    }

    emit selectionChanged();
}
//-----------------------------------------------------------------------------
int32 PropertySelectionContent::getSelectedPtiesCount() const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());
    return _selectedPties.size();
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertySelectionContent::getSelectedPty(int32 indice) const
{
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());

    return _selectedPties[indice];
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >& PropertySelectionContent::getSelectedPties() const
{   
    LOG_FCT_NAME
    LM_ASSERT(_selectedPtiesId.size() == _selectedPties.size());
    return _selectedPties;
}
//-----------------------------------------------------------------------------
void PropertySelectionContent::setPtiesSelectable(const Core::List<Ptr<Property> >* pPtiesSelectable)
{
    _pPtiesSelectable = const_cast<Core::List<Ptr<Property> >*>(pPtiesSelectable);
}
//-----------------------------------------------------------------------------
const Core::List<Ptr<Property> >* PropertySelectionContent::getPtiesSelectable() const
{
    return _pPtiesSelectable;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI