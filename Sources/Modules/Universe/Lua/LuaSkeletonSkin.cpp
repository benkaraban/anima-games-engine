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
#include <Universe/Lua/LuaSkeletonSkin.h>

namespace Universe
{
//-----------------------------------------------------------------------------
const char LuaSkeletonSkin::CLASS_NAME [] = "SkeletonSkin";
const Luna<LuaSkeletonSkin>::RegType LuaSkeletonSkin::METHODS [] = {
    {"kill", &LuaSkeletonSkin::kill},
    {"isAlive", &LuaSkeletonSkin::isAlive},

    {"setMaterial", &LuaSkeletonSkin::setMaterial},
    {"getMaterial", &LuaSkeletonSkin::getMaterial},

    {"getMesh", &LuaSkeletonSkin::getMesh},
    {"setMesh", &LuaSkeletonSkin::setMesh},

    {"setVisible", &LuaSkeletonSkin::setVisible},
    {"isVisible", &LuaSkeletonSkin::isVisible},

    {0}
};
//-----------------------------------------------------------------------------
LuaSkeletonSkin::LuaSkeletonSkin(lua_State* L)
{
    lua_pushstring(L, "pSkin");
    lua_gettable(L, -2);
    Ptr<SkeletonSkin> * pSkin = reinterpret_cast<Ptr<SkeletonSkin>*>(lua_touserdata(L, -1));
    
    if(pSkin == NULL)
        luat_error(L, "NULL Universe::SkeletonSkin");

    _pSkin = *pSkin;
    lua_pop(L, 2);
}
//-----------------------------------------------------------------------------
LuaSkeletonSkin::~LuaSkeletonSkin()
{
    _pSkin = null;
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::kill(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pSkin->kill();
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::isAlive(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    lua_pushboolean(L, int(_pSkin->isAlive()));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::setMaterial(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Assets::Material m;
    luat_tomaterial(L, 1, m);
    _pSkin->setMaterial(m);
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::getMaterial(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Assets::Material m;
    _pSkin->getMaterial(m);
    luat_pushmaterial(L, m);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::getMesh(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushstring(L, _pSkin->getModelName());
    luat_pushstring(L, _pSkin->getMeshName());
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::setMesh(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 2)
        _pSkin->setMesh(luat_tostring(L, 1), luat_tostring(L, 2));
    else
        _pSkin->setMesh(luat_tostring(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::setVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pSkin->setVisible(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaSkeletonSkin::isVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushboolean(L, _pSkin->isVisible());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Ptr<SkeletonSkin> luat_toskeletonskin(lua_State *L, int32 n)
{
    return Luna<LuaSkeletonSkin>::check(L, n, false)->getSkin();
}
//-----------------------------------------------------------------------------
void luat_pushskeletonskin(lua_State *L, const Ptr<SkeletonSkin> & pSkin)
{
    if(pSkin == null)
    {
        lua_pushnil(L);
    }
    else
    {
        Ptr<SkeletonSkin> pSkinCopy(pSkin);
        lua_getglobal(L, "SkeletonSkin");

        lua_newtable(L); // Arg1 = table {pNode=x}
        lua_pushlightuserdata(L, &pSkinCopy);
        lua_setfield(L, -2, "pSkin");

        lua_pcall(L, 1, 1, 0);
    }
}
//-----------------------------------------------------------------------------
void luat_pushskeletonskins(lua_State *L, const Core::List<Ptr<SkeletonSkin> > & skins)
{
    lua_createtable(L, skins.size(), 0);

    for(int32 ii=0; ii < skins.size(); ii++)
    {
        lua_pushinteger(L, ii + 1);
        luat_pushskeletonskin(L, skins[ii]);
        lua_settable(L, -3);
    }
}
//-----------------------------------------------------------------------------
}