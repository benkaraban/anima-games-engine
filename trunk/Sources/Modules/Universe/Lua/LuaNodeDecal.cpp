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
#include <Universe/Lua/LuaNode.h>
#include <Universe/Lua/LuaConstants.h>

namespace Universe
{
//-----------------------------------------------------------------------------
void LuaNode::badNodeType(lua_State* L, ENodeType expected) const
{
    lua_Debug ar;
    lua_getstack(L, 0, &ar);
    lua_getinfo(L, "n", &ar);
    luaL_where(L, 1);

    char buffer[1024];
    sprintf_s(buffer, sizeof(buffer), "%sbad self/this calling method '%s' (%s expected, got %s)",
        lua_tostring(L, -1), // where
        ar.name, // func name
        String8(Universe::toString(expected)).c_str(), // expected
        String8(Universe::toString(_pNode->getNodeType())).c_str()); // got

    lua_pushstring(L, buffer);
    lua_error(L);
}
//-----------------------------------------------------------------------------
Ptr<NodeDecal> LuaNode::getNodeDecal(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_DECAL)
        badNodeType(L, NODE_DECAL);
    else
        return LM_DEBUG_PTR_CAST<NodeDecal>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::setTexture(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_TRAIL)
    {
        Ptr<NodeTrail> pTrail(getNodeTrail(L));
        pTrail->setTexture(luat_tostring(L, 1));
    }
    else
    {
        Ptr<NodeDecal> pDecal(getNodeDecal(L));
        pDecal->setTexture(luat_tostring(L, 1));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTexture(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_TRAIL)
    {
        Ptr<NodeTrail> pTrail(getNodeTrail(L));
        luat_pushstring(L, pTrail->getTexture());
    }
    else
    {
        Ptr<NodeDecal> pDecal(getNodeDecal(L));
        luat_pushstring(L, pDecal->getTexture());
    }
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setSize(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    if(lua_gettop(L) >= 2)
        pDecal->setSize(luat_tofloat(L, 1), luat_tofloat(L, 2));
    else
        pDecal->setSize(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getSize(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushnumber(L, pDecal->getWidth());
    lua_pushnumber(L, pDecal->getHeight());
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setMode(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_TRAIL)
    {
        Ptr<NodeTrail> pTrail(getNodeTrail(L));
        pTrail->setMode(luat_tofreeformmode(L, 1));
    }
    else
    {
        Ptr<NodeDecal> pDecal(getNodeDecal(L));
        pDecal->setMode(luat_todecalmode(L, 1));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}//-----------------------------------------------------------------------------
int LuaNode::getMode(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_TRAIL)
    {
        Ptr<NodeTrail> pTrail(getNodeTrail(L));
        luat_pushfreeformmode(L, pTrail->getMode());
    }
    else
    {
        Ptr<NodeDecal> pDecal(getNodeDecal(L));
        luat_pushdecalmode(L, pDecal->getMode());
    }
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setSpriteLayout(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    if(lua_gettop(L) >= 2)
        pDecal->setSpriteLayout(luat_toint(L, 1), luat_toint(L, 2));
    else
    {
        int w = luat_toint(L, 1);
        pDecal->setSpriteLayout(w, w);
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getSpriteLayout(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushinteger(L, pDecal->getUSpriteLayout());
    lua_pushinteger(L, pDecal->getVSpriteLayout());
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::hasSpriteLayout(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    luat_pushboolean(L, pDecal->hasSpriteLayout());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::animateSprite(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));

    int top = lua_gettop(L);
    float duration = luat_tofloat(L, 1);
    float keyFrom = (top >= 2) ? luat_tofloat(L, 2) : 0.0f;
    float keyTo = (top >= 3) ? luat_tofloat(L, 3) : 0.0f;
    bool loop = (top >= 4) ? luat_toboolean(L, 4) : false;

    pDecal->animateSprite(duration, keyFrom, keyTo, loop);
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getDuration(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushnumber(L, pDecal->getDuration());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getKeyFrom(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushnumber(L, pDecal->getKeyFrom());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getKeyTo(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushnumber(L, pDecal->getKeyTo());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLoop(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushboolean(L, pDecal->getLoop());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTimeKey(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    pDecal->setTimeKey(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTimeKey(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeDecal> pDecal(getNodeDecal(L));
    lua_pushnumber(L, pDecal->getTimeKey());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
