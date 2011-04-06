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
#include <Universe/Lua/LuaTask.h>
#include <Universe/Tasks/Script.h>

namespace Universe
{
//-----------------------------------------------------------------------------
const char LuaTask::CLASS_NAME [] = "ITask";
const Luna<LuaTask>::RegType LuaTask::METHODS [] = {
    {"isFinished", &LuaTask::isFinished},
    {"kill", &LuaTask::kill},
    {"wait", &LuaTask::wait},
    {0}
};
//-----------------------------------------------------------------------------
LuaTask::LuaTask(lua_State* L)
{
    lua_pushstring(L, "pTask");
    lua_gettable(L, -2);
    Ptr<ITask> * pTask = reinterpret_cast<Ptr<ITask>*>(lua_touserdata(L, -1));
    
    if(pTask == NULL)
        luat_error(L, "NULL Universe::ITask");

    _pTask = *pTask;
    lua_pop(L, 2);
}
//-----------------------------------------------------------------------------
LuaTask::~LuaTask()
{
    _pTask = null;
}
//-----------------------------------------------------------------------------
int LuaTask::isFinished(lua_State* L)
{
    luat_pushboolean(L, _pTask->isFinished());
    return 1;
}
//-----------------------------------------------------------------------------
int LuaTask::kill(lua_State* L)
{
    _pTask->kill();
    return 0;
}
//-----------------------------------------------------------------------------
int LuaTask::wait(lua_State* L)
{
    LM_ASSERT(TaskScript::spActiveTask != NULL);
    TaskScript::spActiveTask->suspendTask(_pTask, L);

    return lua_yield(L, 0);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Ptr<ITask> luat_totask(lua_State *L, int32 n)
{
    return Luna<LuaTask>::check(L, n, false)->getTask();
}
//-----------------------------------------------------------------------------
void luat_pushtask(lua_State *L, const Ptr<ITask> & pTask)
{
    if(pTask == null)
    {
        lua_pushnil(L);
    }
    else
    {
        Ptr<ITask> pTaskCopy(pTask);
        lua_getglobal(L, "ITask");

        lua_newtable(L); // Arg1 = table {pTask=x}
        lua_pushlightuserdata(L, &pTaskCopy);
        lua_setfield(L, -2, "pTask");

        lua_pcall(L, 1, 1, 0);
    }
}
//-----------------------------------------------------------------------------
}