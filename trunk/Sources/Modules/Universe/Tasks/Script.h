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
#ifndef TASKSCRIPT_H_
#define TASKSCRIPT_H_

#include <Universe/Node.h>
#include <Universe/Tasks/ITask.h>
#include <Universe/ScriptOutput.h>
#include <Universe/ScriptContext.h>

struct lua_State;

namespace Universe
{
class LuaTask;

class LM_API_UNI TaskScript : public ITask
{
public:
    TaskScript(const ScriptContext & context, lua_State * pWorldState, const String8 & source, const Ptr<ScriptOutput> & pOutput);
    ~TaskScript();

    virtual void update(double elapsed);

    virtual void kill();

    void suspendTask(const Ptr<ITask> & pBlockingTask, lua_State * pLockedState);

    void addSpawnedTask(const Ptr<ITask> & pTask);
    void addSpawnedNode(const Ptr<Node> & pNode);
    void addPostponedTask(const Ptr<ITask> & pTask);

    static TaskScript * spActiveTask;

protected:
    virtual bool internalIsFinished() const;

    ScriptContext       _context;
    String8             _source;
    lua_State *         _pWorldState;
    lua_State *         _pLockedState;

    Ptr<ScriptOutput> _pOutput;
    Ptr<ITask>        _pBlockingTask;
    int32             _gcRef;
    bool              _firstExec;
    bool              _hasRef;
    bool              _errorStatus;

    Core::List<Ptr<Node> >  _spawnedNodes;
    Core::List<Ptr<ITask> > _spawnedTasks;
    Core::List<Ptr<ITask> > _postponedTasks;
};
}

#endif /* TASKWAIT_H_ */
