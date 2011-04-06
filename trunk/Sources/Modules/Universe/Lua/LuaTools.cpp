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
#include <Core/Standard.h>
#include <Core/Logger.h>

#include <Universe/Lua/LuaTools.h>
#include <Universe/Lua/LuaWorld.h>

namespace Universe
{
//-----------------------------------------------------------------------------
struct LuaLoadStruct
{
  const char * s;
  size_t size;
};

static const char * internalGetS(lua_State *L, void *ud, size_t *size) 
{
  LuaLoadStruct * pLoad = reinterpret_cast<LuaLoadStruct*>(ud);
  (void)L;
  
  if(pLoad->size == 0) 
      return NULL;

  *size = pLoad->size;
  pLoad->size = 0;

  return pLoad->s;
}

int luat_loadbuffer(lua_State *L, const char * data, int32 size, const char * name)
{
  LuaLoadStruct ls;
  ls.s = data;
  ls.size = size;
  return lua_load(L, internalGetS, &ls, name);
}
//-----------------------------------------------------------------------------
String8 luat_traceback(lua_State *L)
{
    String8 result;

#ifdef _DEBUG
    int32 startSize = lua_gettop(L);
#endif

    lua_getglobal(L, "debug");
    lua_getfield(L, -1, "traceback");
    lua_call(L, 0, 1);

    const char * pStr = lua_tostring(L, -1);
    
    if(pStr == NULL)
        result = String8("<unable to get stack>");
    else
        result = String8(pStr);

    lua_pop(L, 2);
#ifdef _DEBUG
    LM_ASSERT(startSize == lua_gettop(L));
#endif
    return result;
}
//-----------------------------------------------------------------------------
int luat_typeerror(lua_State *L, int n, const char * name)
{
    const char * typeName = luaL_typename(L, n);

    lua_Debug ar;
    lua_getstack(L, 0, &ar);
    lua_getinfo(L, "n", &ar);
    luaL_where(L, 1);

    char buffer[1024];
    sprintf_s(buffer, sizeof(buffer), "%sbad argument #%d to '%s' (%s expected, got %s)",
        lua_tostring(L, -1), // where
        n, // arg num
        ar.name, // func name
        name, // expected
        typeName); // got

    lua_pushstring(L, buffer);
    return lua_error(L);
}
//-----------------------------------------------------------------------------
void luat_error(lua_State *L, const String8 & message)
{
    String8 errorMsg(message);
    errorMsg.append("\n");
    errorMsg.append(luat_traceback(L));

    lua_pushstring(L, errorMsg.c_str());
    lua_error(L);
}
//-----------------------------------------------------------------------------
void luat_errorarg(lua_State *L, int32 n, const String8 & msg)
{
    const char * pType = lua_typename(L, n);

    String8 message("incorrect parameter <");
    
    if(lua_isnil(L, n))
    {
        message.append("nil");
    }
    else
    {
        const char * pString = lua_tostring(L, n);
        message.append((pString != NULL) ? pString : "???");
        message.append("> with type <");
        message.append((pType != NULL) ? pType : "???");
    }

    message.append(">, ");
    message.append(msg);
    luat_error(L, message);
}
//-----------------------------------------------------------------------------
int32 luat_toint(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "int");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "int");
    else
        result = int32(lua_tonumber(L, n));
    
    return result;
}
//-----------------------------------------------------------------------------
float luat_tofloat(lua_State *L, int32 n)
{
    float result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "float");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "float");
    else
        result = float(lua_tonumber(L, n));
    
    return result;
}
//-----------------------------------------------------------------------------
bool luat_toboolean(lua_State *L, int32 n)
{
    return (lua_toboolean(L, n) != 0);
}
//-----------------------------------------------------------------------------
String luat_tostring(lua_State *L, int32 n)
{
    String result;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "string");
    else if(!lua_isstring(L, n))
        luat_typeerror(L, n, "string");
    else
    {
        const char * pStr = lua_tostring(L, n);
        return String(String8(pStr != NULL ? pStr : "<NULL Lua string>"));
    }

    return result;
}
//-----------------------------------------------------------------------------
float luat_getfloatfield(lua_State *L, int32 n, const char * field, const char * structType)
{
    if(!lua_istable(L, n))
        luat_typeerror(L, n, structType);

    lua_getfield(L, n, field); 

    if(lua_isnil(L, -1))
    {
        String8 error(structType);
        error.append(" with ");
        error.append(field);
        luat_typeerror(L, n, error.c_str());
    }
    else if(!lua_isnumber(L, -1))
    {
        String8 error(structType);
        error.append(" with number ");
        error.append(field);
        luat_typeerror(L, n, error.c_str());
    }

    float result = float(luat_tofloat(L, -1)); 
    lua_pop(L, 1);

    return result;
}
//-----------------------------------------------------------------------------
Core::Vector2f luat_tovec2f(lua_State *L, int32 n)
{
    return Core::Vector2f(luat_getfloatfield(L, n, "x", "vector2/table"),
                          luat_getfloatfield(L, n, "y", "vector2/table"));
}
//-----------------------------------------------------------------------------
Core::Vector3f luat_tovec3f(lua_State *L, int32 n)
{
    return Core::Vector3f(luat_getfloatfield(L, n, "x", "vector/table"),
                          luat_getfloatfield(L, n, "y", "vector/table"),
                          luat_getfloatfield(L, n, "z", "vector/table"));
}
//-----------------------------------------------------------------------------
Core::Vector4f luat_tovec4f(lua_State *L, int32 n)
{
    return Core::Vector4f(luat_getfloatfield(L, n, "r", "color/table"),
                          luat_getfloatfield(L, n, "g", "color/table"),
                          luat_getfloatfield(L, n, "b", "color/table"),
                          luat_getfloatfield(L, n, "a", "color/table"));
}
//-----------------------------------------------------------------------------
Core::Quaternionf luat_toquatf(lua_State *L, int32 n)
{
    return Core::Quaternionf(luat_getfloatfield(L, n, "x", "quaternion/table"),
                             luat_getfloatfield(L, n, "y", "quaternion/table"),
                             luat_getfloatfield(L, n, "z", "quaternion/table"),
                             luat_getfloatfield(L, n, "w", "quaternion/table"));
}
//-----------------------------------------------------------------------------
void luat_tomaterial(lua_State *L, int32 n, Assets::Material & m)
{
    lua_getfield(L, n, "type");             m.type = Assets::EMaterialType(lua_tointeger(L, -1)); lua_pop(L, 1);
    lua_getfield(L, n, "flags");            m.flags = lua_tointeger(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "diffuseTexName");   m.diffuseTexName = luat_tostring(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "specularTexName");  m.specularTexName = luat_tostring(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "normalTexName");    m.normalTexName = luat_tostring(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "emissive");         m.emissive = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "ambient");          m.ambient = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "diffuse");          m.diffuse = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "specular");         m.specular = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "shininess");        m.shininess = luat_tofloat(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "glowColor");        m.glowColor = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "glowExtent");       m.glowExtent = luat_tovec3f(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "reflexions");       m.reflexions = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "refracColor");      m.refracColor = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "refracScale");      m.refracScale = luat_tofloat(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "refracIndex");      m.refracIndex = luat_tofloat(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "reflexions");       m.reflexions = luat_tovec4f(L, -1); lua_pop(L, 1);

    lua_getfield(L, n, "customMode");       m.customMode = Assets::ECustomMode(lua_tointeger(L, -1)); lua_pop(L, 1);
    lua_getfield(L, n, "texture0Name");     m.texture0Name = luat_tostring(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "texture1Name");     m.texture1Name = luat_tostring(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "texture2Name");     m.texture2Name = luat_tostring(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "customColor");      m.customColor = luat_tovec4f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "colorFactor");      m.colorFactor = luat_tofloat(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "glowFactor");       m.glowFactor = luat_tofloat(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "zBias");            m.zBias = luat_tofloat(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "opColor01");        m.opColor01 = Assets::EOperationType(lua_tointeger(L, -1)); lua_pop(L, 1);
    lua_getfield(L, n, "opColor12");        m.opColor12 = Assets::EOperationType(lua_tointeger(L, -1)); lua_pop(L, 1);
    lua_getfield(L, n, "duv0");             m.duv0 = luat_tovec2f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "duv1");             m.duv1 = luat_tovec2f(L, -1); lua_pop(L, 1);
    lua_getfield(L, n, "duv2");             m.duv2 = luat_tovec2f(L, -1); lua_pop(L, 1);
}
//-----------------------------------------------------------------------------
void luat_pushboolean(lua_State *L, bool b)
{
    lua_pushboolean(L, int(b));
}
//-----------------------------------------------------------------------------
void luat_pushstring(lua_State *L, const String & s)
{
    String8 s8(s);
    lua_pushstring(L, s8.c_str());
}
//-----------------------------------------------------------------------------
void luat_pushvec2f(lua_State *L, const Core::Vector2f & v)
{
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, v.y); lua_setfield(L, -2, "y");
}
//-----------------------------------------------------------------------------
void luat_pushvec3f(lua_State *L, const Core::Vector3f & v)
{
    lua_newtable(L);
    lua_pushnumber(L, v.x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, v.y); lua_setfield(L, -2, "y");
    lua_pushnumber(L, v.z); lua_setfield(L, -2, "z");
}
//-----------------------------------------------------------------------------
void luat_pushvec4f(lua_State *L, const Core::Vector4f & v)
{
    lua_newtable(L);
    lua_pushnumber(L, v.r); lua_setfield(L, -2, "r");
    lua_pushnumber(L, v.g); lua_setfield(L, -2, "g");
    lua_pushnumber(L, v.b); lua_setfield(L, -2, "b");
    lua_pushnumber(L, v.a); lua_setfield(L, -2, "a");
}
//-----------------------------------------------------------------------------
void luat_pushquatf(lua_State *L, const Core::Quaternionf & q)
{
    lua_newtable(L);
    lua_pushnumber(L, q._v.x); lua_setfield(L, -2, "x");
    lua_pushnumber(L, q._v.y); lua_setfield(L, -2, "y");
    lua_pushnumber(L, q._v.z); lua_setfield(L, -2, "z");
    lua_pushnumber(L, q._w);   lua_setfield(L, -2, "w");
}
//-----------------------------------------------------------------------------
void luat_pushmaterial(lua_State *L, const Assets::Material & m)
{
    lua_newtable(L);
    lua_pushinteger(L, int(m.type));        lua_setfield(L, -2, "type");
    lua_pushinteger(L, int(m.flags));       lua_setfield(L, -2, "flags");

    luat_pushstring(L, m.diffuseTexName);   lua_setfield(L, -2, "diffuseTexName");
    luat_pushstring(L, m.specularTexName);  lua_setfield(L, -2, "specularTexName");
    luat_pushstring(L, m.normalTexName);    lua_setfield(L, -2, "normalTexName");

    luat_pushvec4f(L, m.emissive);          lua_setfield(L, -2, "emissive");
    luat_pushvec4f(L, m.ambient);           lua_setfield(L, -2, "ambient");
    luat_pushvec4f(L, m.diffuse);           lua_setfield(L, -2, "diffuse");
    luat_pushvec4f(L, m.specular);          lua_setfield(L, -2, "specular");
    lua_pushnumber(L, m.shininess);         lua_setfield(L, -2, "shininess");
    
    luat_pushvec4f(L, m.glowColor);         lua_setfield(L, -2, "glowColor");
    luat_pushvec3f(L, m.glowExtent);        lua_setfield(L, -2, "glowExtent");

    luat_pushvec4f(L, m.reflexions);        lua_setfield(L, -2, "reflexions");
    luat_pushvec4f(L, m.refracColor);       lua_setfield(L, -2, "refracColor");
    lua_pushnumber(L, m.refracScale);       lua_setfield(L, -2, "refracScale");
    lua_pushnumber(L, m.refracIndex);       lua_setfield(L, -2, "refracIndex");

    lua_pushinteger(L, int(m.customMode));  lua_setfield(L, -2, "customMode");
    luat_pushstring(L, m.texture0Name);     lua_setfield(L, -2, "texture0Name");
    luat_pushstring(L, m.texture1Name);     lua_setfield(L, -2, "texture1Name");
    luat_pushstring(L, m.texture2Name);     lua_setfield(L, -2, "texture2Name");
    luat_pushvec4f(L, m.customColor);       lua_setfield(L, -2, "customColor");
    lua_pushnumber(L, m.colorFactor);       lua_setfield(L, -2, "colorFactor");
    lua_pushnumber(L, m.glowFactor);        lua_setfield(L, -2, "glowFactor");
    lua_pushnumber(L, m.zBias);             lua_setfield(L, -2, "zBias");
    lua_pushinteger(L, int(m.opColor01));   lua_setfield(L, -2, "opColor01");
    lua_pushinteger(L, int(m.opColor12));   lua_setfield(L, -2, "opColor12");
    luat_pushvec2f(L, m.duv0);              lua_setfield(L, -2, "duv0");
    luat_pushvec2f(L, m.duv1);              lua_setfield(L, -2, "duv1");
    luat_pushvec2f(L, m.duv2);              lua_setfield(L, -2, "duv2");
}
//-----------------------------------------------------------------------------
extern "C" int custom_print(lua_State *L)
{
    int32 n = lua_gettop(L);
    lua_getglobal(L, "ag_output");
    ScriptOutput * pOutput = reinterpret_cast<ScriptOutput*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    LM_ASSERT(pOutput != NULL);

    for(int32 ii=0; ii < n; ii++)
    {
        const char * pString = lua_tostring(L, ii + 1);
        pOutput->output(String((pString != NULL) ? pString : "<NULL string>"));
    }

    return 0;
}
//-----------------------------------------------------------------------------
extern "C" int ag_error(lua_State *L)
{
    int32 n = lua_gettop(L);
    lua_getglobal(L, "ag_output");
    ScriptOutput * pOutput = reinterpret_cast<ScriptOutput*>(lua_touserdata(L, -1));
    lua_pop(L, 1);

    LM_ASSERT(pOutput != NULL);

    for(int32 ii=0; ii < n; ii++)
    {
        const char * pString = lua_tostring(L, ii + 1);
        pOutput->error(String((pString != NULL) ? pString : "<NULL string>"));
    }

    return 0;
}
//-----------------------------------------------------------------------------
extern "C" int luat_include(lua_State *L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld = luat_getworld(L);
    String  script = luat_tostring(L, 1);
    String context = Core::System::getFileName(script);

    const String8 & source = pWorld->getRessourcesPool()->getScript(script);

    int r = luaL_loadbuffer(L, source.data(), source.size(), String8(L"@" + context).c_str());

    if(r != LUA_OK)
    {
        pWorld->getScriptOutput()->error(L"Error in included file '" + script + L"' : " + luat_tostring(L, -1));
    }

    r = lua_pcall(L, 0, LUA_MULTRET, 0);

    if(r != LUA_OK)
    {
        pWorld->getScriptOutput()->error(L"Error in included file '" + script + L"' : " + luat_tostring(L, -1));
    }

    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
void luat_setoutput(lua_State *L, ScriptOutput * pOutput)
{
    lua_pushlightuserdata(L, pOutput);
    lua_setglobal(L, "ag_output");

    lua_register(L, "include", luat_include);
    lua_register(L, "print", custom_print);
    lua_register(L, "ag_error", ag_error);
}
//-----------------------------------------------------------------------------
}
