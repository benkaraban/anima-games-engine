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
#include <Universe/Particles/PartEffectAttractor.h>
#include <Universe/Particles/PartEffectForce.h>

namespace Universe
{
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> LuaNode::getNodeEmitter(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_EMITTER)
        badNodeType(L, NODE_EMITTER);
    else
        return LM_DEBUG_PTR_CAST<NodeEmitter>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::setPeriod(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        pStorm->setPeriod(luat_tofloat(L, 1), luat_tofloat(L, 2));
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        pEmitter->setPeriod(luat_tofloat(L, 1), luat_tofloat(L, 2));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setStickyParticles(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
    pEmitter->setStickyParticles(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setGenerationTime(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        pStorm->setGenerationTime(luat_tofloat(L, 1), luat_tofloat(L, 2));
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        pEmitter->setGenerationTime(luat_tofloat(L, 1), luat_tofloat(L, 2));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::killAtEnd(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        pStorm->killAtEnd(luat_toboolean(L, 1));
    }
    else if(_pNode->getNodeType() == NODE_SOUND)
    {
        Ptr<NodeSoundSource> pSound(getNodeSoundSource(L));
        pSound->killAtEnd(luat_toboolean(L, 1));
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        pEmitter->killAtEnd(luat_toboolean(L, 1));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getPeriod(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    float p1, p2;

    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        pStorm->getPeriod(p1, p2);
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        pEmitter->getPeriod(p1, p2);
    }
    lua_pushnumber(L, p1);
    lua_pushnumber(L, p2);
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getGenerationTime(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    double p1, p2;

    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        pStorm->getGenerationTime(p1, p2);
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        pEmitter->getGenerationTime(p1, p2);
    }
    lua_pushnumber(L, p1);
    lua_pushnumber(L, p2);
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getStickyParticles(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
    luat_pushboolean(L, pEmitter->getStickyParticles());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getKillAtEnd(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(_pNode->getNodeType() == NODE_STORM)
    {
        Ptr<NodeStorm> pStorm(getNodeStorm(L));
        luat_pushboolean(L, pStorm->killAtEnd());
    }
    else if (_pNode->getNodeType() == NODE_SOUND)
    {
        Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
        lua_pushboolean(L, pNode->killAtEnd());
    }
    else
    {
        Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
        luat_pushboolean(L, pEmitter->killAtEnd());
    }
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::addAttractor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
    Ptr<Universe::PartEffectAttractor> pAttractor(new Universe::PartEffectAttractor(luat_tovec3f(L, 1), luat_tofloat(L, 2)));
    pEmitter->addEffect(pAttractor);
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::addForce(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
    Ptr<Universe::PartEffectForce> pForce(new Universe::PartEffectForce(luat_tovec3f(L, 1), luat_tofloat(L, 2)));
    pEmitter->addEffect(pForce);
    return 0;
    LM_LUA_FUNC_END(L);
}

//-----------------------------------------------------------------------------
int LuaNode::removeEffect(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeEmitter> pEmitter(getNodeEmitter(L));
    pEmitter->removeEffects(luat_toeffect(L,1));
    return 0;
    LM_LUA_FUNC_END(L);
}
}
