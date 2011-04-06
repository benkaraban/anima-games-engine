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
#ifndef UNIVERSE_LUATOOLS_H_
#define UNIVERSE_LUATOOLS_H_

#include <lua.hpp>
#include <Assets/Material.h>
#include <Core/Math/Vector.h>
#include <Core/Standard.h>
#include <Universe/ScriptOutput.h>

namespace Universe
{
//-----------------------------------------------------------------------------

LM_API_UNI void    luat_setoutput(lua_State *L, ScriptOutput * pOutput);
LM_API_UNI int     luat_loadbuffer(lua_State *L, const char * data, int32 size, const char * name);

//-----------------------------------------------------------------------------
LM_API_UNI String8 luat_traceback(lua_State *L);
LM_API_UNI void    luat_error(lua_State *L, const String8 & message);
LM_API_UNI void    luat_errorarg(lua_State *L, int32 n, const String8 & msg);
LM_API_UNI int     luat_typeerror(lua_State *L, int n, const char * name);

//-----------------------------------------------------------------------------
LM_API_UNI int32                    luat_toint(lua_State *L, int32 n);
LM_API_UNI float                    luat_tofloat(lua_State *L, int32 n);
LM_API_UNI bool                     luat_toboolean(lua_State *L, int32 n);
LM_API_UNI String                   luat_tostring(lua_State *L, int32 n);

LM_API_UNI Core::Vector2f           luat_tovec2f(lua_State *L, int32 n);
LM_API_UNI Core::Vector3f           luat_tovec3f(lua_State *L, int32 n);
LM_API_UNI Core::Vector4f           luat_tovec4f(lua_State *L, int32 n);
LM_API_UNI Core::Quaternionf        luat_toquatf(lua_State *L, int32 n);
LM_API_UNI void                     luat_tomaterial(lua_State *L, int32 n, Assets::Material & m);

//-----------------------------------------------------------------------------
LM_API_UNI void                     luat_pushboolean(lua_State *L, bool b);
LM_API_UNI void                     luat_pushstring(lua_State *L, const String & s);

LM_API_UNI void                     luat_pushvec2f(lua_State *L, const Core::Vector2f & v);
LM_API_UNI void                     luat_pushvec3f(lua_State *L, const Core::Vector3f & v);
LM_API_UNI void                     luat_pushvec4f(lua_State *L, const Core::Vector4f & v);
LM_API_UNI void                     luat_pushquatf(lua_State *L, const Core::Quaternionf & v);
LM_API_UNI void                     luat_pushmaterial(lua_State *L, const Assets::Material & m);

#define LM_LUA_FUNC_START(L) \
    try {

#define LM_LUA_FUNC_END(L) \
    } catch(Core::Exception & e) { \
        String8 msg("Error in C++ code called from Lua : "); \
        msg.append(String8(e.getMessage())); \
        Universe::luat_error(L, msg); \
    } \
    return 0;

#ifdef _DEBUG

#define LM_LUA_BLOC_START \
    { \
        int32 initialStackSize = lua_gettop(L);

#define LM_LUA_BLOC_END \
        int32 finalStackSize = lua_gettop(L); \
        LM_ASSERT(initialStackSize == finalStackSize); \
    }
#else
#define LM_LUA_BLOC_START
#define LM_LUA_BLOC_END
#endif

}

#endif
