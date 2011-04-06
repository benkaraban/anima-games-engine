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
Ptr<NodeTrail> LuaNode::getNodeTrail(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_TRAIL)
        badNodeType(L, NODE_TRAIL);
    else
        return LM_DEBUG_PTR_CAST<NodeTrail>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::setLength(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setLength(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setDivisions(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setDivisions(luat_toint(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setHeadWidth(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setHeadWidth(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTailWidth(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setTailWidth(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setHeadU(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setHeadU(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTailU(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setTailU(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setHeadColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setHeadColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTailColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setTailColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setHeadGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setHeadGlow(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTailGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    pTrail->setTailGlow(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLength(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getLength());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getDivisions(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getDivisions());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getHeadWidth(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getHeadWidth());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTailWidth(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getTailWidth());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getHeadU(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getHeadU());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTailU(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    lua_pushnumber(L, pTrail->getTailU());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getHeadColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    luat_pushvec4f(L, pTrail->getHeadColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTailColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    luat_pushvec4f(L, pTrail->getTailColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getHeadGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    luat_pushvec4f(L, pTrail->getHeadGlow());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTailGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeTrail> pTrail(getNodeTrail(L));
    luat_pushvec4f(L, pTrail->getTailGlow());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
