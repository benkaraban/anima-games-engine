/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef UNIVERSE_LUAWORLD_H_
#define UNIVERSE_LUAWORLD_H_

#include <Universe/World.h>
#include <Universe/Lua/LuaTools.h>

#include <luna.h>

namespace Universe
{
class LM_API_UNI LuaWorld
{
public:
    //-----------------------------------------------------------
    LuaWorld(lua_State* L);

    World * getWorld() { return _pWorld; };

    static lua_State * newState(World * pWorld);


    //-----------------------------------------------------------
    int getAllNodes(lua_State* L);
    int getNodesWithTag(lua_State* L);
    int getFirstNodeWithTag(lua_State* L);

    int createNode(lua_State* L);
    int createInstance(lua_State* L);
    int createMesh(lua_State* L);
    int createSkeleton(lua_State* L);
    int createTerrain(lua_State* L);
    int createModel(lua_State* L);
    int createDecal(lua_State* L);
    int createSoundSource(lua_State* L);
    int createBillboardEmitter(lua_State* L);
    int createPointLight(lua_State* L);
    int createSpline(lua_State* L);
    int createStorm(lua_State* L);
    int createTrail(lua_State* L);
    int createWaterPatch(lua_State* L);
    
    int setTimeMultiplier(lua_State* L);
    int getTimeMultiplier(lua_State* L);

    int getMaterial(lua_State* L);

    int setSkyColor(lua_State* L);
    int getSkyColor(lua_State* L);

    int setSkyGlow(lua_State* L);
    int getSkyGlow(lua_State* L);

/*
    Ptr<ITask> runScript(const String  & scriptFileName);
    Ptr<ITask> runInline(const String  & source, const String & name = String(L"<console>"));
    Ptr<ITask> runInline(const String8 & source, const String & name = String(L"<console>"));
*/

    //-----------------------------------------------------------
    static const char CLASS_NAME [];
    static const Luna<LuaWorld>::RegType METHODS[];

protected:
    // Pas de Ptr ici car c'est le World qui contrôle la durée de vie des scripts,
    // pas l'inverse.
    World *   _pWorld;
};

LM_API_UNI World *     luat_toworld(lua_State *L, int32 n);
LM_API_UNI World *     luat_getworld(lua_State * L);

}

#endif
