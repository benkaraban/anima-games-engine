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
#include <Universe/Lua/LuaSkeletonSkin.h>

namespace Universe
{
//-----------------------------------------------------------------------------
Ptr<NodeSkeleton> LuaNode::getNodeSkeleton(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_SKELETON)
        badNodeType(L, NODE_SKELETON);
    else
        return LM_DEBUG_PTR_CAST<NodeSkeleton>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::getSkin(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));

    if(lua_gettop(L) >= 2)
        luat_pushskeletonskin(L, pSkel->getSkin(luat_tostring(L, 1), luat_tostring(L, 2)));
    else
        luat_pushskeletonskin(L, pSkel->getSkin(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getSkins(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));
    Core::List<Ptr<SkeletonSkin> > skins;
    for(int32 ii=0; ii < pSkel->getSkinCount(); ii++)
        skins.push_back(pSkel->getSkin(ii));
    luat_pushskeletonskins(L, skins);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::createSkin(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));

    if(lua_gettop(L) >= 2)
        luat_pushskeletonskin(L, pSkel->createSkin(luat_tostring(L, 1), luat_tostring(L, 2)));
    else
        luat_pushskeletonskin(L, pSkel->createSkin(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setDefaultPose(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));
    pSkel->setDefaultPose();
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::animate(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));

    double duration = lua_tonumber(L, 2);
    float keyFrom   = (lua_gettop(L) >= 3) ? luat_tofloat(L, 3) : 0.0f;
    float keyTo     = (lua_gettop(L) >= 4) ? luat_tofloat(L, 4) : 1.0f;
    bool loop       = (lua_gettop(L) >= 5) ? luat_toboolean(L, 5) : false;

    if(lua_isnumber(L, 1))
        pSkel->animate(luat_toint(L, 1), duration, keyFrom, keyTo, loop);
    else
        pSkel->animate(luat_tostring(L, 1), duration, keyFrom, keyTo, loop);

    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::enqueueAnim(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));

    double duration = lua_tonumber(L, 2);
    float keyFrom   = (lua_gettop(L) >= 3) ? luat_tofloat(L, 3) : 0.0f;
    float keyTo     = (lua_gettop(L) >= 4) ? luat_tofloat(L, 4) : 1.0f;
    bool loop       = (lua_gettop(L) >= 5) ? luat_toboolean(L, 5) : false;

    if(lua_isnumber(L, 1))
        pSkel->enqueueAnim(luat_toint(L, 1), duration, keyFrom, keyTo, loop);
    else
        pSkel->enqueueAnim(luat_tostring(L, 1), duration, keyFrom, keyTo, loop);

    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getSkeletonModel(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSkeleton> pSkel(getNodeSkeleton(L));
    luat_pushstring(L, pSkel->getModelName());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
