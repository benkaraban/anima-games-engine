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
Ptr<NodeMesh> LuaNode::getNodeMesh(lua_State* L) const
{
    if(_pNode->getNodeType() != NODE_MESH)
        badNodeType(L, NODE_MESH);
    else
        return LM_DEBUG_PTR_CAST<NodeMesh>(_pNode);
    return null;
}
//-----------------------------------------------------------------------------
int LuaNode::getModel(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeMesh> pMesh(getNodeMesh(L));
    luat_pushstring(L, pMesh->getModelName());
    luat_pushstring(L, pMesh->getMeshName());
    return 2;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setModel(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<NodeMesh> pMesh(getNodeMesh(L));
    pMesh->setMesh(luat_tostring(L, 1), luat_tostring(L, 2));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
}
