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
#include <Core/Math/Interpolate.h>
#include <Universe/Lua/LuaConstants.h>

namespace Universe
{
//-----------------------------------------------------------------------------
Core::EInterpolateFunc luat_tointerpolate(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "InterpolateFunc");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "InterpolateFunc");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Core::EInterpolateFunc_COUNT)
            luat_typeerror(L, n, "InterpolateFunc");
    }
    
    return Core::EInterpolateFunc(result);
}
//-----------------------------------------------------------------------------
EReferential luat_toreferential(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "Referential");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "Referential");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= EReferential_COUNT)
            luat_typeerror(L, n, "Referential");
    }
    
    return EReferential(result);
}
//-----------------------------------------------------------------------------
Renderer::EDecalMode luat_todecalmode(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "DecalMode");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "DecalMode");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Renderer::EDecalMode_COUNT)
            luat_typeerror(L, n, "DecalMode");
    }
    
    return Renderer::EDecalMode(result);
}
//-----------------------------------------------------------------------------
Renderer::EBillboardMode luat_tobillboardmode(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "BillboardMode");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "BillboardMode");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Renderer::EBillboardMode_COUNT)
            luat_typeerror(L, n, "BillboardMode");
    }
    
    return Renderer::EBillboardMode(result);
}
//-----------------------------------------------------------------------------
Renderer::EFreeFormMode luat_tofreeformmode(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "FreeFormMode");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "FreeFormMode");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Renderer::EFreeFormMode_COUNT)
            luat_typeerror(L, n, "FreeFormMode");
    }
    
    return Renderer::EFreeFormMode(result);
}
//-----------------------------------------------------------------------------
Universe::EEffect luat_toeffect(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "PartEffect");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "PartEffect");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Universe::EEffect_COUNT)
            luat_typeerror(L, n, "PartEffect");
    }
    
    return Universe::EEffect(result);
}
//-----------------------------------------------------------------------------
Universe::ENodeType luat_tonodetype(lua_State *L, int32 n)
{
    int32 result = 0;

    if(lua_isnone(L, n))
        luat_typeerror(L, n, "NodeType");
    else if(!lua_isnumber(L, n))
        luat_typeerror(L, n, "NodeType");
    else
    {
        result = int32(lua_tonumber(L, n));
        if(result < 0 || result >= Universe::ENodeType_COUNT)
            luat_typeerror(L, n, "NodeType");
    }
    
    return Universe::ENodeType(result);
}
//-----------------------------------------------------------------------------
void  luat_pushinterpolate(lua_State *L, Core::EInterpolateFunc f)
{
    lua_pushinteger(L, int(f));
}
//-----------------------------------------------------------------------------
void  luat_pushreferential(lua_State *L, EReferential r)
{
    lua_pushinteger(L, int(r));
}
//-----------------------------------------------------------------------------
void  luat_pushdecalmode(lua_State *L, Renderer::EDecalMode m)
{
    lua_pushinteger(L, int(m));
}
//-----------------------------------------------------------------------------
LM_API_UNI void  luat_pusheffect(lua_State *L, Universe::EEffect e)
{
    lua_pushinteger(L, int(e));
}
//-----------------------------------------------------------------------------
void  luat_pushbillboardmode(lua_State *L, Renderer::EBillboardMode m)
{
    lua_pushinteger(L, int(m));
}
//-----------------------------------------------------------------------------
void  luat_pushfreeformmode(lua_State *L, Renderer::EFreeFormMode m)
{
    lua_pushinteger(L, int(m));
}
//-----------------------------------------------------------------------------
void  luat_pushnodetype(lua_State *L, Universe::ENodeType t)
{
    lua_pushinteger(L, int(t));
}
//-----------------------------------------------------------------------------
void luat_setglobalint(lua_State * L, const char * name, int32 value)
{
    lua_pushinteger(L, value);
    lua_setglobal(L, name);
}
//-----------------------------------------------------------------------------
void registerConstants(lua_State * L)
{
    luat_setglobalint(L, "LINEAR", int32(Core::INTER_LINEAR));
    luat_setglobalint(L, "FADE_IN", int32(Core::INTER_FADE_IN));
    luat_setglobalint(L, "FADE_OUT", int32(Core::INTER_FADE_OUT));
    luat_setglobalint(L, "FADE_IN_OUT", int32(Core::INTER_FADE_IN_OUT));

    luat_setglobalint(L, "LOCAL_COORD", int32(LOCAL_COORD));
    luat_setglobalint(L, "WORLD_COORD", int32(WORLD_COORD));

    luat_setglobalint(L, "DECAL_ADD", int32(Renderer::DECAL_ADD));
    luat_setglobalint(L, "DECAL_LERP", int32(Renderer::DECAL_LERP));
    luat_setglobalint(L, "DECAL_REFRAC", int32(Renderer::DECAL_REFRAC));

    luat_setglobalint(L, "FREE_FORM_ADD", int32(Renderer::FREE_FORM_ADD));
    luat_setglobalint(L, "FREE_FORM_LERP", int32(Renderer::FREE_FORM_LERP));
    luat_setglobalint(L, "FREE_FORM_REFRAC", int32(Renderer::FREE_FORM_REFRAC));

    luat_setglobalint(L, "BILLBOARD_ADD", int32(Renderer::BILLBOARD_ADD));
    luat_setglobalint(L, "BILLBOARD_LERP", int32(Renderer::BILLBOARD_LERP));
    luat_setglobalint(L, "BILLBOARD_ADD_LERP", int32(Renderer::BILLBOARD_ADD_LERP));
    luat_setglobalint(L, "BILLBOARD_REFRAC", int32(Renderer::BILLBOARD_REFRAC));

    luat_setglobalint(L, "FADE", int32(EFFECT_FADE));
    luat_setglobalint(L, "FADE2", int32(EFFECT_FADE2));
    luat_setglobalint(L, "FADE3", int32(EFFECT_FADE3));
    luat_setglobalint(L, "FORCE", int32(EFFECT_FORCE));
    luat_setglobalint(L, "ATTRACTOR", int32(EFFECT_ATTRACTOR));
    luat_setglobalint(L, "SPEED_LIMIT", int32(EFFECT_SPEED_LIMIT));
    luat_setglobalint(L, "RESISTANCE", int32(EFFECT_RESISTANCE));
    luat_setglobalint(L, "ANIMATE", int32(EFFECT_ANIMATE));
    luat_setglobalint(L, "SIZE", int32(EFFECT_SIZE));
    luat_setglobalint(L, "SIZE2", int32(EFFECT_SIZE2));
    luat_setglobalint(L, "ROLL", int32(EFFECT_ROLL));
    luat_setglobalint(L, "TRANSLATE", int32(EFFECT_TRANSLATE));
    luat_setglobalint(L, "COLOR_BY_POS", int32(EFFECT_COLOR_BY_POS));
    luat_setglobalint(L, "TURN", int32(EFFECT_TURN));


    luat_setglobalint(L, "NODE", int32(Universe::NODE));
    luat_setglobalint(L, "DECAL", int32(Universe::NODE_DECAL));
    luat_setglobalint(L, "EMITTER", int32(Universe::NODE_EMITTER));
    luat_setglobalint(L, "MESH", int32(Universe::NODE_MESH));
    luat_setglobalint(L, "SKELETON", int32(Universe::NODE_SKELETON));
    luat_setglobalint(L, "POINT_LIGHT", int32(Universe::NODE_POINT_LIGHT));
    luat_setglobalint(L, "WATER_PATCH", int32(Universe::NODE_WATER_PATCH));
    luat_setglobalint(L, "GROUP", int32(Universe::NODE_GROUP));
    luat_setglobalint(L, "TERRAIN", int32(Universe::NODE_TERRAIN));
    luat_setglobalint(L, "SPLINE", int32(Universe::NODE_SPLINE));
    luat_setglobalint(L, "TRAIL", int32(Universe::NODE_TRAIL));
    luat_setglobalint(L, "STORM", int32(Universe::NODE_STORM));
}
//-----------------------------------------------------------------------------
}
