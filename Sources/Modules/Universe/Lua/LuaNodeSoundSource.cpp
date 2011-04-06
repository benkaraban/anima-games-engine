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
Ptr<NodeSoundSource> LuaNode::getNodeSoundSource(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_SOUND)
        badNodeType(L, NODE_SOUND);
    else
        return LM_DEBUG_PTR_CAST<NodeSoundSource>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::enqueue(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->enqueue(luat_tostring(L, 1), luat_toboolean(L, 2));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::eraseAllSounds(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->eraseAllSounds();
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setGain(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->setGain(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getGain(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    lua_pushnumber(L, pNode->getGain());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::stopCurrentLoop(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->stopCurrentLoop();
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::playbackCompleted(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    lua_pushboolean(L, pNode->playbackCompleted());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::fadeIn(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->fadeIn(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::fadeOut(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->fadeOut(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setPause(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->setPause(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getPaused(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    lua_pushboolean(L, pNode->getPaused());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setStaticSound(lua_State* L)
{
   LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    pNode->setStaticSound(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::isStaticSound(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeSoundSource> pNode(getNodeSoundSource(L));
    lua_pushboolean(L, pNode->isStaticSound());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::fadeOutChildren(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Core::List<Ptr<Node> > nodes;
    _pNode->getChildrenOfType(NODE_SOUND, nodes);
    for(Core::List<Ptr<Node> >::const_iterator ipChild = nodes.begin(); ipChild != nodes.end(); ++ipChild)
    {
        NodeSoundSource & soundSource = LM_DEBUG_CAST<NodeSoundSource &>(**ipChild);
        soundSource.fadeOut(luat_tofloat(L,1));
    }
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::fadeInChildren(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Core::List<Ptr<Node> > nodes;
    _pNode->getChildrenOfType(NODE_SOUND, nodes);
    for(Core::List<Ptr<Node> >::const_iterator ipChild = nodes.begin(); ipChild != nodes.end(); ++ipChild)
    {
        NodeSoundSource & soundSource = LM_DEBUG_CAST<NodeSoundSource &>(**ipChild);
        soundSource.fadeIn(luat_tofloat(L,1));
    }
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
