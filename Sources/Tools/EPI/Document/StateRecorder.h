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
#ifndef STATE_RECORDER_BASE_H_
#define STATE_RECORDER_BASE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/LinkedList.h>
#include <EPI/Document/State.h>

class QAction;

namespace EPI
{

class IState;
class DocumentBase;
//-----------------------------------------------------------------------------
/**
*  StateChangeSet s'occupe de stoker le ou les changements d'etat suite a l'action de l'utilisateur
*/
class LM_API_EPI StateChangeSet : public Core::Object
{
public:
    StateChangeSet();
    virtual ~StateChangeSet();

    void recordUndoState(const Ptr<IState>& state);
    void recordRedoState(const Ptr<IState>& state);

    void recordUndoStates(const Core::List<Ptr<IState> >& states);
    void recordRedoStates(const Core::List<Ptr<IState> >& states);

    void restoreUndo(DocumentBase& doc);
    void restoreRedo(DocumentBase& doc);
    void restoreUndoOnProperty(Ptr<Property>& pPty);
    void restoreUndoOnProperties(Core::List<Ptr<Property> >& properties);

    String toString() const;

private:
    Core::List<Ptr<IState> > _undoStates;
    Core::List<Ptr<IState> > _redoStates;
};
//-----------------------------------------------------------------------------
/**
*  StateRecorder enregistre les StateChangeSet durant toute la vie d'un document. (a voir quelle limite on imposera)
*/
class LM_API_EPI StateRecorder : public Core::Object
{
public:
    virtual ~StateRecorder();

	void enable();
	void disable();

    void startRecording();
    void stopRecording();
    void eraseRecording();
    void cancelRecording(Ptr<Property>& pPty);
    void cancelRecording(Core::List<Ptr<Property> >& properties);

    void recording(const Ptr<IState>& pUndo, const Ptr<IState>& pRedo);
    void recording(const Core::List<Ptr<IState> >& undos, const Core::List<Ptr<IState> >& redos);

    Ptr<StateChangeSet>& getCurrentChangeSet();

    StateRecorder();

    bool isRecording() const;

    //reviens sur le precedent StateChangeSet
    void previousStateChangeSet(DocumentBase& doc);

    //reviens sur le StateChangeSet suivant
    void nextStateChangeSet(DocumentBase& doc);

    int32 getNbUndoState() const;
    int32 getNbRedoState() const;

    const Core::LinkedList<Ptr<StateChangeSet> >& getStates() {return _states;}
    void kill();
private:
    bool _isEnable;
    Ptr<StateChangeSet> _pCurrentRecording;
    Core::LinkedList<Ptr<StateChangeSet> > _states;
    int32 _statesPos;
};
//-----------------------------------------------------------------------------
LM_API_EPI void addChildRecording         (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild, Property& ptyParent, int32 indexPos = -1);
LM_API_EPI void addChildRecording         (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild, Ptr<Property>& pPtyParent, int32 indexPos = -1);
LM_API_EPI void removeChildRecording      (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPtyChild);
LM_API_EPI void removeChildrenRecording   (const Ptr<StateRecorder>& pStateRecorder, Core::List<Ptr<Property> >& pPtyChild);
LM_API_EPI void beginChangeProperty       (const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPty);
LM_API_EPI void beginChangeProperties     (const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> >& properties);
LM_API_EPI void finishChangeProperty      (const Ptr<StateRecorder>& pStateRecorder, const Ptr<Property>& pPty, bool stopRecording=true);
LM_API_EPI void finishChangeProperties    (const Ptr<StateRecorder>& pStateRecorder, const Core::List<Ptr<Property> >& properties, bool stopRecording=true);
LM_API_EPI void cancelChangeProperties    (const Ptr<StateRecorder>& pStateRecorder, Core::List<Ptr<Property> >& properties);


LM_API_EPI void addContentRecording       (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPty, PtyNodeEditableContent& pPtyContent, int32 indexPos = -1);
LM_API_EPI void removeContentRecording    (const Ptr<StateRecorder>& pStateRecorder, Ptr<Property>& pPty, const Ptr<PtyNodeEditableContent>& pPtyContent);
LM_API_EPI void removeContentRecording    (const Ptr<StateRecorder>& pStateRecorder, int32 start, int32 end, PtyNodeEditableContent& ptyContent);
//-----------------------------------------------------------------------------
}  // namespace EPI

#endif /*STATE_RECORDER_BASE_H_*/