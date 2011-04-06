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
#include <EPI/Document/StateRecorder.h>
#include <EPI/Document/DocumentBase.moc.h>

#include <QAction>
#include <EPI/Constants.h>

namespace EPI
{
//-----------------------------------------------------------------------------
StateChangeSet::StateChangeSet()
{
}
//-----------------------------------------------------------------------------
StateChangeSet::~StateChangeSet()
{
}
//-----------------------------------------------------------------------------
void StateChangeSet::recordUndoState(const Ptr<IState>& state)
{
    _undoStates.push_back(state);
}
//-----------------------------------------------------------------------------
void StateChangeSet::recordRedoState(const Ptr<IState>& state)
{
    _redoStates.push_back(state);
}
//-----------------------------------------------------------------------------
void StateChangeSet::recordUndoStates(const Core::List<Ptr<IState> >& states)
{
    for (int32 iState=0; iState<states.size(); ++iState)
    {
        _undoStates.push_back(states[iState]);
    }
}
//-----------------------------------------------------------------------------
void StateChangeSet::recordRedoStates(const Core::List<Ptr<IState> >& states)
{
    for (int32 iState=0; iState<states.size(); ++iState)
    {
        _redoStates.push_back(states[iState]);
    }
}
//-----------------------------------------------------------------------------
void StateChangeSet::restoreUndo(DocumentBase& doc)
{
    Core::List<Ptr<IState> >::reverse_iterator iState    = _undoStates.rbegin();
    Core::List<Ptr<IState> >::reverse_iterator iStateEnd = _undoStates.rend();

    for(; iState!=iStateEnd; ++iState)
    {
        (*iState)->restoreState(doc);
    }
}
//-----------------------------------------------------------------------------
void StateChangeSet::restoreRedo(DocumentBase& doc)
{
    Core::List<Ptr<IState> >::iterator iState = _redoStates.begin();
    Core::List<Ptr<IState> >::iterator iStateEnd = _redoStates.end();

    for(; iState!=iStateEnd; ++iState)
    {
        (*iState)->restoreState(doc);
    }
}
//-----------------------------------------------------------------------------
void StateChangeSet::restoreUndoOnProperty(Ptr<Property>& pPty)
{
    Core::List<Ptr<IState> >::iterator iState = _undoStates.begin();
    Core::List<Ptr<IState> >::iterator iStateEnd = _undoStates.end();

    for(; iState!=iStateEnd; ++iState)
    {
        (*iState)->restoreStateOnProperty(pPty);
    }
}
//-----------------------------------------------------------------------------
void StateChangeSet::restoreUndoOnProperties(Core::List<Ptr<Property> >& properties)
{
    LM_ASSERT(properties.size()==_undoStates.size());

    Core::List<Ptr<IState> >::iterator iState = _undoStates.begin();
    Core::List<Ptr<IState> >::iterator iStateEnd = _undoStates.end();

    Core::List<Ptr<Property> >::iterator iPty = properties.begin();

    for(; iState!=iStateEnd; ++iState, ++iPty)
    {
        //TODO verifier que l'on restaure la property avec le state correct
        (*iState)->restoreStateOnProperty((*iPty));
    }
}
//-----------------------------------------------------------------------------
String StateChangeSet::toString() const
{
    String str;

    str <<L"Undo:"<< L"\n";
    for (int32 ii=0; ii<_undoStates.size(); ++ii)
        str<<L"    "<<_undoStates[ii]->toString()<< L"\n";

    str <<L"Redo:"<< L"\n";
    for (int32 ii=0; ii<_redoStates.size(); ++ii)
        str<<L"    "<<_redoStates[ii]->toString()<< L"\n";

    return str;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
StateRecorder::StateRecorder() :
    _isEnable(false),
    _pCurrentRecording(null),
    _statesPos(-1)
{

}
//-----------------------------------------------------------------------------
StateRecorder::~StateRecorder()
{
}
//-----------------------------------------------------------------------------
void StateRecorder::kill()
{
    _pCurrentRecording = null;
    _states.clear();
    _statesPos = -1;
}
//-----------------------------------------------------------------------------
void StateRecorder::enable()
{
    _isEnable = true;
}
//-----------------------------------------------------------------------------
void StateRecorder::disable()
{
    _isEnable = false;
}
//-----------------------------------------------------------------------------
bool StateRecorder::isRecording() const
{
    return _pCurrentRecording!=null;
}
//-----------------------------------------------------------------------------
void StateRecorder::startRecording()
{
    if (_isEnable==true)
    {
        if (_pCurrentRecording!=null)
        {
           // LM_ASSERT(false);
           // throw Core::Exception(L"StateRecorder::startRecording() : Current State Recording already exist");
        }
        else
        {
    
            _pCurrentRecording = Ptr<StateChangeSet>(new StateChangeSet() );
        }
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::stopRecording()
{
    if (_isEnable==true)
    {
        if (_pCurrentRecording==null)
        {
          //  LM_ASSERT(false);
          //  throw Core::Exception(L"StateRecorder::stopRecording() : Current State Recording doesn't exist");
        }
        else
        {

            _statesPos++;
            Core::LinkedList<Ptr<StateChangeSet> >::iterator first = _states.begin();
            for (int32 ii=0; ii<_statesPos; ++ii)
                first++;

            
            _states.erase(first, _states.end());
            _states.push_back(_pCurrentRecording);  //TODO a finir !!!
            _pCurrentRecording = null;   

            if (_states.size() > MAX_UNDO_REDO)
            {
                _states.erase(_states.begin());
                _statesPos--;
            }
        }
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::eraseRecording()
{
    if (_isEnable==true)
    {
        _pCurrentRecording = null;
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::cancelRecording(Ptr<Property>& pPty)
{
    if (_pCurrentRecording==null)
    {
        LM_ASSERT(false);
        throw Core::Exception(L"StateRecorder::cancelRecording() : Current State Recording doesn't exist");
    }

    if (_isEnable==true)
    {
        _pCurrentRecording->restoreUndoOnProperty(pPty);
        _pCurrentRecording = null;
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::cancelRecording(Core::List<Ptr<Property> >& properties)
{
    if (_pCurrentRecording==null)
        throw Core::Exception(L"StateRecorder::cancelRecording(Core::List<Ptr<Property> >&) : Current State Recording doesn't exist");

    if (_isEnable==true)
    {
        _pCurrentRecording->restoreUndoOnProperties(properties);
        _pCurrentRecording = null;
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::recording(const Ptr<IState>& pUndo, const Ptr<IState>& pRedo)
{
    if (_isEnable==true)
    {
        startRecording();
        getCurrentChangeSet()->recordUndoState(pUndo);
        getCurrentChangeSet()->recordRedoState(pRedo);
        stopRecording();
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::recording(const Core::List<Ptr<IState> >& undos, const Core::List<Ptr<IState> >& redos)
{
    if (_isEnable==true)
    {
        startRecording();
        getCurrentChangeSet()->recordUndoStates(undos);
        getCurrentChangeSet()->recordRedoStates(redos);
        stopRecording();
    }
}
//-----------------------------------------------------------------------------
Ptr<StateChangeSet>& StateRecorder::getCurrentChangeSet()
{
    return _pCurrentRecording;
}
//-----------------------------------------------------------------------------
void StateRecorder::previousStateChangeSet(DocumentBase& doc)
{
    if (_statesPos >=0 && _statesPos<_states.size())
    {
        Core::LinkedList<Ptr<StateChangeSet> >::iterator it = _states.begin();
        for (int32 ii=0; ii<_statesPos; ++ii)
            it++;

        (*it)->restoreUndo(doc);
        _statesPos--;
    }
}
//-----------------------------------------------------------------------------
void StateRecorder::nextStateChangeSet(DocumentBase& doc)
{
    if (_statesPos >=-1 && _statesPos<_states.size()-1)
    {
        _statesPos++;

        Core::LinkedList<Ptr<StateChangeSet> >::iterator it = _states.begin();
        for (int32 ii=0; ii<_statesPos; ++ii)
            it++;

        (*it)->restoreRedo(doc);
    }
}
//-----------------------------------------------------------------------------
int32 StateRecorder::getNbUndoState() const
{
    return _statesPos + 1;
}
//-----------------------------------------------------------------------------
int32 StateRecorder::getNbRedoState() const
{
    if (_states.size()==0 || _states.size() == _statesPos) return 0; 

    return _states.size() - (_statesPos+1);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI void addChildRecording(const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild, Property& ptyParent, int32 indexPos)
{
    ptyParent.addChild(pPtyChild, indexPos);

    bool stopRecording = false;
    
    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();

    Core::List<Ptr<IState> > undoStates;
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroy (*pPtyChild)) );
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreated (*pPtyChild)) );
        if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}
//-----------------------------------------------------------------------------
LM_API_EPI void addChildRecording(const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild, Ptr<Property>& pPtyParent, int32 indexPos)
{/*
    pPtyParent->addChild(pPtyChild);

    pStateRecorder->startRecording();

    Core::List<Ptr<IState> > undoStates;
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroy (*pPtyChild)) );
        pStateRecorder->getCurrentChangeSet()->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreated (*pPtyChild)) );
        pStateRecorder->getCurrentChangeSet()->recordRedoStates(redoStates);

    pStateRecorder->stopRecording();*/
    addChildRecording(pStateRecorder, pPtyChild, *pPtyParent, indexPos);
}
//-----------------------------------------------------------------------------
LM_API_EPI void removeChildrenRecording(const Ptr<StateRecorder>& pStateRecorder, Core::List<Ptr<Property> >& pties)
{
    bool stopRecording = false;

    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();

    Core::List<Ptr<IState> > undoStates;
        for (int32 ii=0; ii<pties.size(); ++ii)
        {
            undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreated (*pties[ii])) );
        }
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;

    for (int32 ii = 0; ii<pties.size(); ++ii)
    {
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroy (*pties[ii])) );
        Property* parent = pties[ii]->getParent();
        parent->removeChild(pties[ii]);
    }

    if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}
//-----------------------------------------------------------------------------
LM_API_EPI void removeChildRecording(const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild)
{
    bool stopRecording = false;

    

    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();

    Core::List<Ptr<IState> > undoStates;
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreated (*pPtyChild)) );
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;


    redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroy (*pPtyChild)) );
    Property* parent = pPtyChild->getParent();
    parent->removeChild(pPtyChild);


    if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI void beginChangeProperty(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPty)
{
    Core::List<Ptr<Property> > pties;
    pties.push_back(pPty);
    beginChangeProperties(pStateRecorder, pties);
}
//-----------------------------------------------------------------------------
LM_API_EPI void beginChangeProperties(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> >& properties)
{
    pStateRecorder->startRecording();

    Core::List<Ptr<IState> > undoStates;
    for (int32 iPty=0; iPty<properties.size(); ++iPty)
    {
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyChanged (*(properties[iPty]))) );
    }

    pStateRecorder->getCurrentChangeSet()->recordUndoStates(undoStates);
}
//-----------------------------------------------------------------------------
LM_API_EPI void finishChangeProperty(const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPty, bool stopRecording)
{
    Core::List<Ptr<Property> > pties;
    pties.push_back(pPty);
    finishChangeProperties(pStateRecorder, pties, stopRecording);
}
//-----------------------------------------------------------------------------
LM_API_EPI void finishChangeProperties(const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> >& properties, bool stopRecording)
{
    Core::List<Ptr<IState> > redoStates;
    for (int32 iPty=0; iPty<properties.size(); ++iPty)
    {
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyChanged (*(properties[iPty]))) );
    }

    pStateRecorder->getCurrentChangeSet()->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}
//-----------------------------------------------------------------------------
LM_API_EPI void cancelChangeProperties(const Ptr<StateRecorder>& pStateRecorder, Core::List<Ptr<Property> >& properties)
{
    pStateRecorder->cancelRecording(properties);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_EPI void removeContentRecording      (const Ptr<StateRecorder>& pStateRecorder, int32 start, int32 end, PtyNodeEditableContent& ptyContent)
{
    bool stopRecording = false;

    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();
    const Core::List<Ptr<Property> >& ptiesContent = ptyContent.getPtiesContentSelectable();

    Core::List<Ptr<IState> > undoStates;
        for (int32 ii=0; ii<end-start+1; ++ii)
        {
           // undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreatedContent (*ptiesContent[ii+start], ptyContent)) );

            undoStates.insert(undoStates.begin(), Ptr<IState> ((IState*) new StatePropertyCreatedContent (*ptiesContent[ii+start], ptyContent)) );
        }
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;

    for (int32 ii = end; ii>=start; --ii)
    {
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroyContent (*ptiesContent[ii], ptyContent)) );
        //redoStates.insert(redoStates.begin(), Ptr<IState> ((IState*) new StatePropertyDestroyContent (*ptiesContent[ii], ptyContent)) );
        ptyContent.removePtyContent(ii);
    }



    if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}

LM_API_EPI void addContentRecording       (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPty, PtyNodeEditableContent& pPtyContent, int32 indexPos)
{
    pPtyContent.getPtiesContentSelectable().insert(pPtyContent.getPtiesContentSelectable().begin()+indexPos, pPty);
    bool stopRecording = false;
    
    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();

    Core::List<Ptr<IState> > undoStates;
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroyContent (*pPty, pPtyContent)) );
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;
        redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreatedContent (*pPty, pPtyContent)) );
        if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}

LM_API_EPI void removeContentRecording      (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPty, const Ptr<PtyNodeEditableContent>& pPtyContent)
{
    bool stopRecording = false;

    if (pStateRecorder->isRecording()==false)
    {
        pStateRecorder->startRecording();
        stopRecording = true;
    }

    Ptr<StateChangeSet> pScs = pStateRecorder->getCurrentChangeSet();

    Core::List<Ptr<IState> > undoStates;
        undoStates.push_back( Ptr<IState> ((IState*) new StatePropertyCreatedContent (*pPty, *pPtyContent)) );
        if (pScs!=null) pScs->recordUndoStates(undoStates);

    Core::List<Ptr<IState> > redoStates;


    redoStates.push_back( Ptr<IState> ((IState*) new StatePropertyDestroyContent (*pPty, *pPtyContent)) );


    pPtyContent->removePtyContent(pPty);


    if (pScs!=null) pScs->recordRedoStates(redoStates);

    if (stopRecording==true)
        pStateRecorder->stopRecording();
}


}  // namespace EPI
