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
#ifndef UNIVERSE_LUACONSTANTS_H_
#define UNIVERSE_LUACONSTANTS_H_

#include <Core/Math/Interpolate.h>
#include <Renderer/IDecal.h>
#include <Universe/Particles/IPartEffect.h>
#include <Renderer/IBillboard.h>
#include <Renderer/IFreeForm.h>
#include <Universe/Node.h>
#include <Universe/Lua/LuaTools.h>

#include <luna.h>

namespace Universe
{
LM_API_UNI Core::EInterpolateFunc   luat_tointerpolate(lua_State *L, int32 n);
LM_API_UNI EReferential             luat_toreferential(lua_State *L, int32 n);
LM_API_UNI Renderer::EDecalMode     luat_todecalmode(lua_State *L, int32 n);
LM_API_UNI Renderer::EBillboardMode luat_tobillboardmode(lua_State *L, int32 n);
LM_API_UNI Renderer::EFreeFormMode  luat_tofreeformmode(lua_State *L, int32 n);
LM_API_UNI Universe::EEffect        luat_toeffect(lua_State *L, int32 n);
LM_API_UNI Universe::ENodeType      luat_tonodetype(lua_State *L, int32 n);

LM_API_UNI void  luat_pushinterpolate(lua_State *L, Core::EInterpolateFunc f);
LM_API_UNI void  luat_pushreferential(lua_State *L, EReferential r);
LM_API_UNI void  luat_pushdecalmode(lua_State *L, Renderer::EDecalMode m);
LM_API_UNI void  luat_pushbillboardmode(lua_State *L, Renderer::EBillboardMode m);
LM_API_UNI void  luat_pushfreeformmode(lua_State *L, Renderer::EFreeFormMode m);
LM_API_UNI void  luat_pusheffect(lua_State *L, Universe::EEffect e);
LM_API_UNI void  luat_pushnodetype(lua_State *L, Universe::ENodeType e);


LM_API_UNI void luat_setglobalint(lua_State * L, const char * name, int32 value);

void registerConstants(lua_State * L);
}

#endif
