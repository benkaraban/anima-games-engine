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
#ifndef LUA_OUTPUT_H_
#define LUA_OUTPUT_H_

#include <Core/Standard.h>
#include <Universe/ScriptOutput.h>

namespace EPI
{
LM_ENUM_2(ELuaMsgType,
                LUA_ERROR,
                LUA_OUTPUT)
//-----------------------------------------------------------------------------
class LM_API_EPI LuaOutput : public QObject, public Universe::ScriptOutput
{
    Q_OBJECT

public:
    struct luaMsgMemo
    {
        luaMsgMemo(const String& m, ELuaMsgType t) : msg(m), type(t) {}

        String msg;
        ELuaMsgType type;
    };

    struct luaExecutionMemo
    {
        luaExecutionMemo(const String& n) : name(n) {}

        String                  name;
        Core::List<luaMsgMemo>  luaOutputs;
    };

public:
    LuaOutput();
    virtual ~LuaOutput();

    virtual void output(const String & message);
    virtual void error(const String & message);

    void pushBackCmd(const String & cmd);
    const String& getCmd(int32 index) const;
    int32   getNbCmd() const;

    const luaExecutionMemo& getExecutionMemo(int32 index) const;
    int32 getNbExecutionMemo() const;

    void startNewExecutionMemo(const String name);
    luaExecutionMemo& getCurrentExecutionMemo();
    


Q_SIGNALS:
    void newOutputMsg(const String & message);
    void newErrorMsg(const String & message);

private:
    Core::List<luaExecutionMemo>  _luaMemo;
    Core::List<String>            _luaCmdMemo;
};
//-----------------------------------------------------------------------------
}//namespace EPI

#endif //LUA_OUTPUT_H_
