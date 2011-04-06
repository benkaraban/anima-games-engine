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
#include <Core/Logger.h>
#include <Universe/Lua/LuaTools.h>
#include <Universe/Lua/LuaNode.h>
#include <Universe/Tasks/Script.h>

#define TRACE_SCRIPT 0

namespace Universe
{
//-----------------------------------------------------------------------------
TaskScript * TaskScript::spActiveTask = NULL;
//-----------------------------------------------------------------------------
TaskScript::TaskScript(const ScriptContext & context, lua_State * pWorldState, const String8 & source, const Ptr<ScriptOutput> & pOutput) :
    _context(context),
    _source(source),
    _pWorldState(pWorldState),
    _pLockedState(NULL),
    _pOutput(pOutput),
    _gcRef(0),
    _firstExec(true),
    _hasRef(false),
    _errorStatus(false)
{
    _priority = 100;
}
//-----------------------------------------------------------------------------
TaskScript::~TaskScript()
{
    if(_hasRef)
        luaL_unref(_pWorldState, LUA_REGISTRYINDEX, _gcRef);

    _gcRef = 0xABCDABCD;
    _hasRef = false;

    _pLockedState = NULL;
    _pWorldState = NULL;
    _pBlockingTask = null;
}
//-----------------------------------------------------------------------------
void TaskScript::suspendTask(const Ptr<ITask> & pBlockingTask, lua_State * pLockedState)
{
    LM_ASSERT(_pBlockingTask == null);
    _pBlockingTask = pBlockingTask;

    if(_hasRef)
    {
        LM_ASSERT(_pLockedState == pLockedState);
    }
    else
    {
        _pLockedState = pLockedState;
        lua_getglobal(_pWorldState, "globalCoroutine");
        _gcRef = luaL_ref(_pWorldState, LUA_REGISTRYINDEX);
        _hasRef = true;
    }

    setPriority(pBlockingTask->getPriority() + 100);
}
//-----------------------------------------------------------------------------
void TaskScript::update(double elapsed)
{
    if(_pWorldState == NULL || _errorStatus) // script en erreur
        return;

    if(_firstExec)
    {
#if TRACE_SCRIPT
        DBG << L"Start   task [" << _name << L"]\n";
#endif
        String8 encaspulatedSource;
        encaspulatedSource.append("do local function run_me() ");

        if(_context.pNode != null)
        {
            luat_pushnode(_pWorldState, _context.pNode);
            lua_setglobal(_pWorldState, "globalNode");
            encaspulatedSource.append("local node=globalNode; ");
        }

        encaspulatedSource.append(_source);
        encaspulatedSource.append("\n");
        encaspulatedSource.append("end\n");
        encaspulatedSource.append("local c = coroutine.create(run_me)\n");
        encaspulatedSource.append("globalCoroutine = c\n");
        encaspulatedSource.append("local ok, msg = coroutine.resume(c)\n");
        encaspulatedSource.append("if not ok then\n");
        encaspulatedSource.append("  ag_error(msg)\n");
        encaspulatedSource.append("end\n");
        encaspulatedSource.append("end\n");

        int r = luaL_loadbuffer(_pWorldState, encaspulatedSource.data(), encaspulatedSource.size(), String8(L"@" + _context.fileName).c_str());
        
        if(r != LUA_OK)
        {
            _pOutput->error(luat_tostring(_pWorldState, -1) + L"\n");
            _errorStatus = true;
        }
        else
        {
            LM_ASSERT(spActiveTask == NULL);
            spActiveTask = this;
            int r = lua_pcall(_pWorldState, 0, LUA_MULTRET, 0);
            spActiveTask = NULL;

            if(r != LUA_OK)
            {
                _pOutput->error(luat_tostring(_pWorldState, -1) + L"\n");
                _errorStatus = true;
            }
        }

        _firstExec = false;

#if TRACE_SCRIPT
        if(_pBlockingTask == null)
            DBG << L"Finish task [" << _name << L"] 0x" << Core::toStringHex(int32(_pLockedState)) << L"\n";
        else
            DBG << L"Pause  task [" << _name << L"] 0x" << Core::toStringHex(int32(_pLockedState)) << L"\n";
#endif
    }

    while(_pBlockingTask != null && _pBlockingTask->isFinished())
    {
        LM_ASSERT(_pLockedState != NULL);
        LM_ASSERT(spActiveTask == NULL);
        spActiveTask = this;
        _pBlockingTask = null;
        
#if TRACE_SCRIPT
        DBG << L"Resume task [" << _name << L"]  0x" << Core::toStringHex(int32(_pLockedState)) << L"\n";
#endif

        int r = lua_resume(_pLockedState, 0);
        spActiveTask = NULL;

        if(r != LUA_OK && r != LUA_YIELD)
        {
            _pOutput->error(luat_tostring(_pLockedState, -1) + L"\n");
            _errorStatus = true;
        }
        else
        {
            LM_ASSERT((r == LUA_OK && _pBlockingTask == null) || (r == LUA_YIELD && _pBlockingTask != null));
        }

#if TRACE_SCRIPT
        if(_pBlockingTask == null)
            DBG << L"Finish task [" << _name << L"]  0x" << Core::toStringHex(int32(_pLockedState)) << L"\n";
        else
            DBG << L"Pause  task [" << _name << L"]  0x" << Core::toStringHex(int32(_pLockedState)) << L"\n";
#endif
    }

    for(int32 ii=0; ii < _postponedTasks.size(); ii++)
        _postponedTasks[ii]->update(0.0);

    _postponedTasks.clear();
}
//-----------------------------------------------------------------------------
void TaskScript::kill()
{
    ITask::kill();

    if(_pBlockingTask != null)
        _pBlockingTask->kill();

    for(int32 ii=0; ii < _spawnedTasks.size(); ii++)
        _spawnedTasks[ii]->kill();

    for(int32 ii=0; ii < _spawnedNodes.size(); ii++)
        _spawnedNodes[ii]->kill();

    _spawnedTasks.clear();
    _spawnedNodes.clear();
}
//-----------------------------------------------------------------------------
bool TaskScript::internalIsFinished() const
{
    return _errorStatus || ((!_firstExec) && (_pBlockingTask == null));
}
//-----------------------------------------------------------------------------
void TaskScript::addSpawnedTask(const Ptr<ITask> & pTask)
{
    _spawnedTasks.push_back(pTask);
}
//-----------------------------------------------------------------------------
void TaskScript::addSpawnedNode(const Ptr<Node> & pNode)
{
    _spawnedNodes.push_back(pNode);
}
//-----------------------------------------------------------------------------
void TaskScript::addPostponedTask(const Ptr<ITask> & pTask)
{
    _postponedTasks.push_back(pTask);
}
//-----------------------------------------------------------------------------
}
