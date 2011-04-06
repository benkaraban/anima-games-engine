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
Ptr<NodePointLight> LuaNode::getNodePointLight(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_POINT_LIGHT)
        badNodeType(L, NODE_POINT_LIGHT);
    else
        return LM_DEBUG_PTR_CAST<NodePointLight>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::setAmbientColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    pLight->setAmbientColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setDiffuseColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    pLight->setDiffuseColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setSpecularColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    pLight->setSpecularColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setRadius(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    pLight->setRadius(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getShadowCasting(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    luat_pushboolean(L, pLight->getShadowCasting());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getAmbientColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    luat_pushvec4f(L, pLight->getAmbientColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getDiffuseColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    luat_pushvec4f(L, pLight->getDiffuseColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getSpecularColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    luat_pushvec4f(L, pLight->getSpecularColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getRadius(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodePointLight> pLight(getNodePointLight(L));
    lua_pushnumber(L, pLight->getRadius());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
