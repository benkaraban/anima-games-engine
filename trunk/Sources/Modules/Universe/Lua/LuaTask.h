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
#ifndef UNIVERSE_LUATASK_H_
#define UNIVERSE_LUATASK_H_

#include <Universe/Tasks/ITask.h>
#include <Universe/Lua/LuaTools.h>

#include <luna.h>

namespace Universe
{
class LM_API_UNI LuaTask
{
public:
    LuaTask(lua_State* L);
    ~LuaTask();

    const Ptr<ITask> & getTask() const { return _pTask; };

    //-----------------------------------------------------------
    int isFinished(lua_State* L);
    int kill(lua_State* L);

    int wait(lua_State* L);

    //-----------------------------------------------------------
    static const char CLASS_NAME [];
    static const Luna<LuaTask>::RegType METHODS[];

protected:
    Ptr<ITask>   _pTask;
};

LM_API_UNI Ptr<ITask>  luat_totask(lua_State *L, int32 n);

LM_API_UNI void        luat_pushtask(lua_State *L, const Ptr<ITask> & pTask);

}

#endif
