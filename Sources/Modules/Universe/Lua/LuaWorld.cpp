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
#include <Universe/Lua/LuaWorld.h>
#include <Universe/Lua/LuaNode.h>
#include <Universe/Lua/LuaSkeletonSkin.h>
#include <Universe/Lua/LuaTask.h>
#include <Universe/Lua/LuaConstants.h>
#include <Universe/Lua/LuaCreateTasks.h>
#include <Universe/Tasks/Script.h>
#include <Universe/Tools.h>

namespace Universe
{
//-----------------------------------------------------------------------------
const char LuaWorld::CLASS_NAME [] = "World";
const Luna<LuaWorld>::RegType LuaWorld::METHODS [] = {
     {"createNode", &LuaWorld::createNode},
     {"createInstance", &LuaWorld::createInstance},
     {"createSkeleton", &LuaWorld::createSkeleton},
     {"createTerrain", &LuaWorld::createTerrain},
     {"createMesh", &LuaWorld::createMesh},
     {"createModel", &LuaWorld::createModel},
     {"createDecal", &LuaWorld::createDecal},
     {"createBillboardEmitter", &LuaWorld::createBillboardEmitter},
     {"createPointLight", &LuaWorld::createPointLight},
     {"createSpline", &LuaWorld::createSpline},
     {"createStorm", &LuaWorld::createStorm},
     {"createTrail", &LuaWorld::createTrail},
     {"createSoundSource", &LuaWorld::createSoundSource},
     {"createWaterPatch", &LuaWorld::createWaterPatch},
     {"getMaterial", &LuaWorld::getMaterial},
     {"setSkyColor", &LuaWorld::setSkyColor},
     {"getSkyColor", &LuaWorld::getSkyColor},
     {"setSkyGlow", &LuaWorld::setSkyGlow},
     {"getSkyGlow", &LuaWorld::getSkyGlow},

     {"getAllNodes", &LuaWorld::getAllNodes},
     {"getNodesWithTag", &LuaWorld::getNodesWithTag},
     {"getFirstNodeWithTag", &LuaWorld::getFirstNodeWithTag},
     {0}
};
//-----------------------------------------------------------------------------
void luat_pushspawnednode(lua_State *L, const Ptr<Node> & pNode)
{
    LM_ASSERT(TaskScript::spActiveTask != NULL);
    TaskScript::spActiveTask->addSpawnedNode(pNode);
    luat_pushnode(L, pNode);
}
//-----------------------------------------------------------------------------
lua_State * LuaWorld::newState(World * pWorld)
{
    lua_State * L = luaL_newstate();

    // libs
    luaL_openlibs(L);
    luaopen_debug(L);

    // classes
    Luna<LuaWorld>::Register(L);
    Luna<LuaNode>::Register(L);
    Luna<LuaSkeletonSkin>::Register(L);
    Luna<LuaTask>::Register(L);

    // fonctions
    registerTaskFunctions(L);

    // constants
    registerConstants(L);

    // world global
    LM_LUA_BLOC_START;
    {
        int n = lua_gettop(L);
        lua_getglobal(L, "World"); // Fonction World

        lua_newtable(L); // Arg1 = table {pWorld=x}
        lua_pushlightuserdata(L, pWorld);
        lua_setfield(L, -2, "pWorld");

        lua_pcall(L, 1, 1, 0);
        // top = objet World

        lua_setglobal(L, "world");
    }
    LM_LUA_BLOC_END;

    return L;
}
//-----------------------------------------------------------------------------
LuaWorld::LuaWorld(lua_State* L)
{
    lua_pushstring(L, "pWorld");
    lua_gettable(L, -2);
    _pWorld = reinterpret_cast<World*>(lua_touserdata(L, -1));
    
    if(_pWorld == NULL)
        luat_error(L, "NULL Universe::world");

    lua_pop(L, 2);
}
//-----------------------------------------------------------------------------
int LuaWorld::getAllNodes(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushnodes(L, _pWorld->getAllNodes());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getNodesWithTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Core::List<Ptr<Node> > nodes;
    _pWorld->getNodesWithTag(luat_tostring(L, 1), nodes);
    luat_pushnodes(L, nodes);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getFirstNodeWithTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushnode(L, _pWorld->getFirstNodeWithTag(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createNode(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 1)
        luat_pushspawnednode(L, _pWorld->importXML(luat_tostring(L, 1)));
    else
        luat_pushspawnednode(L, _pWorld->createNode());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createInstance(lua_State* L)
{
    LM_ASSERT(TaskScript::spActiveTask != NULL);
    LM_LUA_FUNC_START(L);
    Ptr<Node> pNode = _pWorld->createInstance(luat_tostring(L, 1));
    luat_pushspawnednode(L, pNode);
    
    if((lua_gettop(L) >= 2) && luat_toboolean(L, 2))
    {
        Core::List<Ptr<Node> > children;
        Universe::getAllChildren(children, *pNode);
        children.push_back(pNode);

        for(int32 ii=0; ii < children.size(); ii++)
        {
            if(!children[ii]->getNodeScript().empty())
            {
                Ptr<ITask> pTask(_pWorld->runNodeScript(children[ii], true));
                TaskScript::spActiveTask->addSpawnedTask(pTask);
                TaskScript::spActiveTask->addPostponedTask(pTask);
            }
        }
    }

    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createMesh(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    String s1 = luat_tostring(L, 1);
    String s2 = luat_tostring(L, 2);
    luat_pushspawnednode(L, _pWorld->createMesh(s1, s2));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createSkeleton(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createSkeleton(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createTerrain(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createTerrain(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createModel(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createModel(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createDecal(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createDecal(luat_tostring(L, 1), luat_todecalmode(L, 2)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createSoundSource(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createSoundSource());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createBillboardEmitter(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createBillboardEmitter(luat_tostring(L, 1), luat_tobillboardmode(L, 2)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createPointLight(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createPointLight());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createSpline(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createSpline());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createStorm(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createStorm());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createTrail(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createTrail());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::createWaterPatch(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushspawnednode(L, _pWorld->createWaterPatch(luat_tofloat(L, 1), luat_tofloat(L, 2)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::setTimeMultiplier(lua_State* L)
{
    LM_LUA_FUNC_START(L);
     _pWorld->setTimeMultiplier(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getTimeMultiplier(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    lua_pushnumber(L, _pWorld->getTimeMultiplier());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getMaterial(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Assets::Material mat;
    if(lua_gettop(L) >= 2)
        _pWorld->getMaterial(mat, luat_tostring(L, 1), luat_tostring(L, 2));
    else
        _pWorld->getMaterial(mat, luat_tostring(L, 1));
    luat_pushmaterial(L, mat);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::setSkyColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pWorld->setSkyColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getSkyColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec4f(L, _pWorld->getSkyColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::setSkyGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pWorld->setSkyGlow(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaWorld::getSkyGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec4f(L, _pWorld->getSkyGlow());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
World * luat_toworld(lua_State *L, int32 n)
{
    return Luna<LuaWorld>::check(L, n, false)->getWorld();
}
//-----------------------------------------------------------------------------
World * luat_getworld(lua_State * L)
{
    lua_getglobal(L, "world");
    int t = lua_type(L, -1);
    World * pWorld = luat_toworld(L, -1);
    lua_pop(L, 1);
    return pWorld;
}
//-----------------------------------------------------------------------------
}