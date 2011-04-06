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
#ifndef UNIVERSE_LUASKELETONSKIN_H_
#define UNIVERSE_LUASKELETONSKIN_H_

#include <Universe/NodeSkeleton.h>
#include <Universe/Lua/LuaTools.h>

#include <luna.h>

namespace Universe
{
class LM_API_UNI LuaSkeletonSkin
{
public:
    LuaSkeletonSkin(lua_State* L);
    ~LuaSkeletonSkin();

    const Ptr<SkeletonSkin> & getSkin() const { return _pSkin; };

    //-----------------------------------------------------------
    int kill(lua_State* L);
    int isAlive(lua_State* L);

    int setMaterial(lua_State* L);
    int getMaterial(lua_State* L);

    int getMesh(lua_State* L); // retourne model, mesh
    int setMesh(lua_State* L);

    int setVisible(lua_State* L);
    int isVisible(lua_State* L);

    //-----------------------------------------------------------
    static const char CLASS_NAME [];
    static const Luna<LuaSkeletonSkin>::RegType METHODS[];

protected:
    Ptr<SkeletonSkin>   _pSkin;
};

LM_API_UNI Ptr<SkeletonSkin>   luat_toskeletonskin(lua_State *L, int32 n);

LM_API_UNI void        luat_pushskeletonskin(lua_State *L, const Ptr<SkeletonSkin> & pSkin);
LM_API_UNI void        luat_pushskeletonskins(lua_State *L, const Core::List<Ptr<SkeletonSkin> > & skins);

}

#endif
