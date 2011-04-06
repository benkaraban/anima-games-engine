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
#include "LuaOutput.moc.h"

namespace EPI
{
//-----------------------------------------------------------------------------
LuaOutput::LuaOutput() 
{
}
//-----------------------------------------------------------------------------
LuaOutput::~LuaOutput()
{
}
//-----------------------------------------------------------------------------
void LuaOutput::startNewExecutionMemo(const String name)
{
    _luaMemo.push_back(luaExecutionMemo (name));
}
//-----------------------------------------------------------------------------
LuaOutput::luaExecutionMemo& LuaOutput::getCurrentExecutionMemo()
{
    return _luaMemo.back();
}
//-----------------------------------------------------------------------------
void LuaOutput::output(const String & message)
{
    Universe::ScriptOutput::output(message);
    getCurrentExecutionMemo().luaOutputs.push_back(luaMsgMemo(message, LUA_OUTPUT));
    emit newOutputMsg(message);
}
//-----------------------------------------------------------------------------
void LuaOutput::error(const String & message)
{
    Universe::ScriptOutput::error(message);
    getCurrentExecutionMemo().luaOutputs.push_back(luaMsgMemo(message, LUA_ERROR));
    emit newErrorMsg(message);
}
//-----------------------------------------------------------------------------
void LuaOutput::pushBackCmd(const String & cmd)
{
    _luaCmdMemo.push_back(cmd);
}
//-----------------------------------------------------------------------------
const String& LuaOutput::getCmd(int32 index) const
{
    LM_ASSERT(index<_luaCmdMemo.size());
    return _luaCmdMemo[index];
}
//-----------------------------------------------------------------------------
int32 LuaOutput::getNbCmd() const
{
    return _luaCmdMemo.size();
}
//-----------------------------------------------------------------------------
const LuaOutput::luaExecutionMemo& LuaOutput::getExecutionMemo(int32 index) const
{
    LM_ASSERT(index<_luaMemo.size());
    return _luaMemo[index];
}
//-----------------------------------------------------------------------------
int32 LuaOutput::getNbExecutionMemo() const
{
    return _luaMemo.size();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI