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
#include <Universe/Tasks/Fade.h>
#include <Universe/Tasks/LookAt.h>
#include <Universe/Tasks/Move.h>
#include <Universe/Tasks/MoveTrajectory.h>
#include <Universe/Tasks/Particles.h>
#include <Universe/Tasks/PostFX.h>
#include <Universe/Tasks/Rotate.h>
#include <Universe/Tasks/Scale.h>
#include <Universe/Tasks/TimeWarp.h>
#include <Universe/Tasks/TurnAround.h>
#include <Universe/Tasks/Wait.h>
#include <Universe/Tasks/Animate.h>
#include <Universe/Tasks/Script.h>
#include <Universe/Tasks/SkyTasks.h>
#include <Universe/Tasks/Shake.h>

#include <Universe/Lua/LuaConstants.h>
#include <Universe/Lua/LuaCreateTasks.h>
#include <Universe/Lua/LuaWorld.h>
#include <Universe/Lua/LuaNode.h>
#include <Universe/Lua/LuaTask.h>

#include <Universe/Tools.h>

namespace Universe
{
//-----------------------------------------------------------------------------
int luat_rettask(lua_State * L, const Ptr<ITask> & pTask)
{
    LM_ASSERT(TaskScript::spActiveTask != NULL);
    TaskScript::spActiveTask->addSpawnedTask(pTask);

    int r;
    r = lua_gettop(L);
    World * pWorld = luat_getworld(L);
    r = lua_gettop(L);
    pWorld->registerTask(pTask);

    r = lua_gettop(L);
    luat_pushtask(L, pTask);
    return 1;
}
//-----------------------------------------------------------------------------
int luaTaskWait(lua_State * L)
{
    double duration = lua_tonumber(L, 1);
    return luat_rettask(L, Ptr<TaskWait>(new TaskWait(duration)));
}
//-----------------------------------------------------------------------------
int luaTaskMove(lua_State * L)
{
    if(lua_gettop(L) >= 5)
    {
        Ptr<Node> pNode             = luat_tonode(L, 1);
        Core::Vector3f source       = luat_tovec3f(L, 2);
        Core::Vector3f target       = luat_tovec3f(L, 3);
        float duration              = luat_tofloat(L, 4);
        Core::EInterpolateFunc func = luat_tointerpolate(L, 5);

        return luat_rettask(L, Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, pNode, source, target, duration, func)));
    }
    else
    {
        Ptr<Node> pNode             = luat_tonode(L, 1);
        Core::Vector3f target       = luat_tovec3f(L, 2);
        float duration              = luat_tofloat(L, 3);
        Core::EInterpolateFunc func = luat_tointerpolate(L, 4);

        return luat_rettask(L, Ptr<ITask>(new TaskMove(DURATION_DEPENDANT, pNode, target, duration, func)));
    }
}
//-----------------------------------------------------------------------------
int luaTaskScale(lua_State * L)
{
    Ptr<Node> pNode          = luat_tonode(L, 1);
    Core::Vector3f scale     = luat_tovec3f(L, 2);
    float duration           = luat_tofloat(L, 3);
    bool relative            = luat_toboolean(L, 4);
    Core::EInterpolateFunc f = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskScale(pNode, scale, duration, relative, f)));
}
//-----------------------------------------------------------------------------
int luaTaskScaleLoop(lua_State * L)
{
    Ptr<Node> pNode          = luat_tonode(L, 1);
    Core::Vector3f scale1    = luat_tovec3f(L, 2);
    Core::Vector3f scale2    = luat_tovec3f(L, 3);
    float period             = luat_tofloat(L, 4);
    float duration           = luat_tofloat(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskScaleLoop(pNode, scale1, scale2, period, duration)));
}
//-----------------------------------------------------------------------------
int luaTaskRotate(lua_State * L)
{
    Ptr<Node> pNode          = luat_tonode(L, 1);
    float pitch              = luat_tofloat(L, 2);
    float yaw                = luat_tofloat(L, 3);
    float roll               = luat_tofloat(L, 4);
    float duration           = luat_tofloat(L, 5);
    Core::EInterpolateFunc f = luat_tointerpolate(L, 6);
    EReferential referentiel = LOCAL_COORD;

    if(lua_gettop(L) >= 7)
        referentiel = luat_toreferential(L, 7);

    return luat_rettask(L, Ptr<ITask>(new TaskRotate(DURATION_DEPENDANT, pNode, pitch, yaw, roll, duration, f, referentiel)));
}
//-----------------------------------------------------------------------------
int luaTaskRotateLoop(lua_State * L)
{
    Ptr<Node> pNode          = luat_tonode(L, 1);
    float pitchSpeed         = luat_tofloat(L, 2);
    float yawSpeed           = luat_tofloat(L, 3);
    float rollSpeed          = luat_tofloat(L, 4);
    float duration           = luat_tofloat(L, 5);
    EReferential referentiel = LOCAL_COORD;

    if(lua_gettop(L) >= 6)
        referentiel = luat_toreferential(L, 6);

    return luat_rettask(L, Ptr<ITask>(new TaskRotateLoop(pNode, pitchSpeed, yawSpeed, rollSpeed, duration, referentiel)));
}
//-----------------------------------------------------------------------------
int luaTaskFade(lua_State * L)
{
    if(lua_gettop(L) >= 5 && lua_istable(L, 3))
    {
        Ptr<Node> pNode          = luat_tonode(L, 1);
        Core::Vector4f color0    = luat_tovec4f(L, 2);
        Core::Vector4f color1    = luat_tovec4f(L, 3);
        float duration           = luat_tofloat(L, 4);
        Core::EInterpolateFunc f = luat_tointerpolate(L, 5);
        bool recursive           = (lua_gettop(L) >= 6) ? luat_toboolean(L, 6) : true;

        return luat_rettask(L, Ptr<ITask>(new TaskFade(pNode, color0, color1, duration, f, recursive)));
    }
    else
    {
        Ptr<Node> pNode          = luat_tonode(L, 1);
        Core::Vector4f color1    = luat_tovec4f(L, 2);
        float duration           = luat_tofloat(L, 3);
        Core::EInterpolateFunc f = luat_tointerpolate(L, 4);
        bool recursive           = (lua_gettop(L) >= 5) ? luat_toboolean(L, 5) : true;

        return luat_rettask(L, Ptr<ITask>(new TaskFade(pNode, color1, duration, f, recursive)));
    }
}
//-----------------------------------------------------------------------------
int luaTaskFade2(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector4f color0     = luat_tovec4f(L, 2);
    Core::Vector4f color1     = luat_tovec4f(L, 3);
    Core::Vector4f color2     = luat_tovec4f(L, 4);
    float time1               = luat_tofloat(L, 5);
    float time2               = luat_tofloat(L, 6);
    Core::EInterpolateFunc f1 = (lua_gettop(L) >= 7) ? luat_tointerpolate(L, 7) : Core::INTER_LINEAR;
    Core::EInterpolateFunc f2 = (lua_gettop(L) >= 8) ? luat_tointerpolate(L, 8) : Core::INTER_LINEAR;

    return luat_rettask(L, Ptr<ITask>(new TaskFade2(pNode, color0, color1, color2, time1, time2, f1, f2)));
}
//-----------------------------------------------------------------------------
int luaTaskFadeLoop(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector4f color0     = luat_tovec4f(L, 2);
    Core::Vector4f color1     = luat_tovec4f(L, 3);
    float period              = luat_tofloat(L, 4);
    float duration            = (lua_gettop(L) >= 5) ? luat_tofloat(L, 5) : INFINITY;

    return luat_rettask(L, Ptr<ITask>(new TaskFadeLoop(pNode, color0, color1, period, duration)));
}
//-----------------------------------------------------------------------------
int luaTaskGlowFade(lua_State * L)
{
    if(lua_gettop(L) >= 5 && lua_istable(L, 3))
    {
        Ptr<Node> pNode          = luat_tonode(L, 1);
        Core::Vector4f color0    = luat_tovec4f(L, 2);
        Core::Vector4f color1    = luat_tovec4f(L, 3);
        float duration           = luat_tofloat(L, 4);
        Core::EInterpolateFunc f = luat_tointerpolate(L, 5);
        bool recursive           = (lua_gettop(L) >= 6) ? luat_toboolean(L, 6) : true;

        return luat_rettask(L, Ptr<ITask>(new TaskGlowFade(pNode, color0, color1, duration, f, recursive)));
    }
    else
    {
        Ptr<Node> pNode          = luat_tonode(L, 1);
        Core::Vector4f color1    = luat_tovec4f(L, 2);
        float duration           = luat_tofloat(L, 3);
        Core::EInterpolateFunc f = luat_tointerpolate(L, 4);
        bool recursive           = (lua_gettop(L) >= 5) ? luat_toboolean(L, 5) : true;

        return luat_rettask(L, Ptr<ITask>(new TaskGlowFade(pNode, color1, duration, f, recursive)));
    }
}
//-----------------------------------------------------------------------------
int luaTaskGlowFade2(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector4f color0     = luat_tovec4f(L, 2);
    Core::Vector4f color1     = luat_tovec4f(L, 3);
    Core::Vector4f color2     = luat_tovec4f(L, 4);
    float time1               = luat_tofloat(L, 5);
    float time2               = luat_tofloat(L, 6);
    Core::EInterpolateFunc f1 = (lua_gettop(L) >= 7) ? luat_tointerpolate(L, 7) : Core::INTER_LINEAR;
    Core::EInterpolateFunc f2 = (lua_gettop(L) >= 8) ? luat_tointerpolate(L, 8) : Core::INTER_LINEAR;

    return luat_rettask(L, Ptr<ITask>(new TaskGlowFade2(pNode, color0, color1, color2, time1, time2, f1, f2)));
}
//-----------------------------------------------------------------------------
int luaTaskGlowFadeLoop(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector4f color0     = luat_tovec4f(L, 2);
    Core::Vector4f color1     = luat_tovec4f(L, 3);
    float period              = luat_tofloat(L, 4);
    float duration            = (lua_gettop(L) >= 5) ? luat_tofloat(L, 5) : INFINITY;

    return luat_rettask(L, Ptr<ITask>(new TaskFadeLoop(pNode, color0, color1, period, duration)));
}
//-----------------------------------------------------------------------------
int luaTaskLookAt(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector3f target     = luat_tovec3f(L, 2);
    float duration            = luat_tofloat(L, 3);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 4);

    return luat_rettask(L, Ptr<ITask>(new TaskLookAt(DURATION_DEPENDANT, pNode, target, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskLockLookAtPos(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector3f target     = luat_tovec3f(L, 2);
    float duration            = luat_tofloat(L, 3);
    float radialSpeed         = luat_tofloat(L, 4);

    return luat_rettask(L, Ptr<ITask>(new TaskLockLookAt(pNode, target, duration, radialSpeed)));
}
//-----------------------------------------------------------------------------
int luaTaskLockLookAtNode(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Ptr<Node> pTarget         = luat_tonode(L, 2);
    float duration            = luat_tofloat(L, 3);
    float radialSpeed         = luat_tofloat(L, 4);

    return luat_rettask(L, Ptr<ITask>(new TaskLockLookAt(pNode, pTarget, duration, radialSpeed)));
}
//-----------------------------------------------------------------------------
int luaTaskChangeEmitterPeriod(lua_State * L)
{
    Ptr<NodeEmitter> pNode    = luat_tonodeemitter(L, 1);
    float minv                = luat_tofloat(L, 2);
    float maxv                = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskChangeEmitterPeriod(pNode, minv, maxv, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskChangeEmitterStartSpeed(lua_State * L)
{
    Ptr<NodeEmitter> pNode    = luat_tonodeemitter(L, 1);
    float minv                = luat_tofloat(L, 2);
    float maxv                = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskChangeEmitterStartSpeed(pNode, minv, maxv, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskChangeEmitterStartWidth(lua_State * L)
{
    Ptr<NodeEmitter> pNode    = luat_tonodeemitter(L, 1);
    float minv                = luat_tofloat(L, 2);
    float maxv                = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskChangeEmitterStartWidth(pNode, minv, maxv, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskChangeEmitterRadius(lua_State * L)
{
    Ptr<NodeEmitter> pNode    = luat_tonodeemitter(L, 1);
    float minv                = luat_tofloat(L, 2);
    float maxv                = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskChangeEmitterRadius(pNode, minv, maxv, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskTimeWarp(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    float startTimeMul        = luat_tofloat(L, 2);
    float endTimeMul          = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);

    return luat_rettask(L, Ptr<ITask>(new TaskTimeWarp(pNode, startTimeMul, endTimeMul, duration)));
}
//-----------------------------------------------------------------------------
int luaTaskTurnAround(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector3f startPos   = luat_tovec3f(L, 2);
    Core::Vector3f endPos     = luat_tovec3f(L, 3);
    Core::Vector3f pivot      = luat_tovec3f(L, 4);
    float duration            = luat_tofloat(L, 5);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 6);
    bool shortestPath         = luat_toboolean(L, 7);

    return luat_rettask(L, Ptr<ITask>(new TaskTurnAround(pNode, startPos, endPos, pivot, duration, f, shortestPath)));
}
//-----------------------------------------------------------------------------
int luaTaskTurnAroundLoop(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::Vector3f pivot      = luat_tovec3f(L, 2);
    float pitchSpeed          = luat_tofloat(L, 3);
    float yawSpeed            = luat_tofloat(L, 4);
    float rollSpeed           = luat_tofloat(L, 5);
    float duration            = luat_tofloat(L, 6);

    return luat_rettask(L, Ptr<ITask>(new TaskTurnAroundLoop(pNode, pivot, pitchSpeed, yawSpeed, rollSpeed, duration)));
}
//-----------------------------------------------------------------------------
int luaTaskFXRadialBlur(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    Core::Vector2f center     = luat_tovec2f(L, 2);
    float blur1               = luat_tofloat(L, 3);
    float blur2               = luat_tofloat(L, 4);
    float duration            = luat_tofloat(L, 5);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 6);

    return luat_rettask(L, Ptr<ITask>(new TaskFXRadialBlur(pCamera, center, blur1, blur2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskFXRadialBlurNode(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    Ptr<Node> pNode           = luat_tonode(L, 2);
    float blur1               = luat_tofloat(L, 3);
    float blur2               = luat_tofloat(L, 4);
    float duration            = luat_tofloat(L, 5);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 6);

    return luat_rettask(L, Ptr<ITask>(new TaskFXRadialBlur(pCamera, pNode, blur1, blur2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskFXDirBlur(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    Core::Vector2f dir        = luat_tovec2f(L, 2);
    float blur1               = luat_tofloat(L, 3);
    float blur2               = luat_tofloat(L, 4);
    float duration            = luat_tofloat(L, 5);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 6);

    return luat_rettask(L, Ptr<ITask>(new TaskFXDirBlur(pCamera, dir, blur1, blur2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskFXBlur(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    float blur1               = luat_tofloat(L, 2);
    float blur2               = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskFXBlur(pCamera, blur1, blur2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskFXFade(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    Core::Vector4f col1       = luat_tovec4f(L, 2);
    Core::Vector4f col2       = luat_tovec4f(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskFXFade(pCamera, col1, col2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskFXSaturate(lua_State * L)
{
    Ptr<NodeCamera> pCamera   = luat_tonodecamera(L, 1);
    float sat1                = luat_tofloat(L, 2);
    float sat2                = luat_tofloat(L, 3);
    float duration            = luat_tofloat(L, 4);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 5);

    return luat_rettask(L, Ptr<ITask>(new TaskFXSaturate(pCamera, sat1, sat2, duration, f)));
}
//-----------------------------------------------------------------------------
int luaTaskMoveTrajectory(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    String spline             = luat_tostring(L, 2);
    float duration            = (lua_gettop(L) >= 3) ? luat_tofloat(L, 3) : 0.0f;

    Ptr<Core::Trajectory> pSpline(pWorld->getRessourcesPool()->getSpline(spline));

    float timeMul = 1.0f;
    
    if(duration != 0.0f)
        timeMul = duration / pSpline->getDuration();
    
    return luat_rettask(L, Ptr<ITask>(new TaskMoveTrajectory(pNode, *pSpline, timeMul)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskMoveTrajectoryPivot(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Ptr<Node> pPivot          = luat_tonode(L, 2);
    String spline             = luat_tostring(L, 3);
    float duration            = (lua_gettop(L) >= 4) ? luat_tofloat(L, 4) : 0.0f;
    float key1                = (lua_gettop(L) >= 5) ? luat_tofloat(L, 5) : 0.0f;
    float key2                = (lua_gettop(L) >= 6) ? luat_tofloat(L, 6) : 1.0f;

    Ptr<Core::Trajectory> pSpline(pWorld->getRessourcesPool()->getSpline(spline));

    float timeMul = 1.0f;
    
    if(duration != 0.0f)
        timeMul = duration / pSpline->getDuration();
    
    return luat_rettask(L, Ptr<ITask>(new TaskMoveTrajectory(pNode, pPivot, *pSpline, timeMul, key1, key2)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskMoveToTrajectoryPoint(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Ptr<Node> pPivot          = luat_tonode(L, 2);
    String spline             = luat_tostring(L, 3);
    float key                 = luat_tofloat(L, 4);
    float duration            = luat_tofloat(L, 5);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 6);

    Ptr<Core::Trajectory> pSpline(pWorld->getRessourcesPool()->getSpline(spline));

    Core::Matrix4f mat;
    Universe::getCleanMatrix(mat, *pPivot, *pSpline, key * pSpline->getDuration());

    Core::Transformf trans(mat);

    return luat_rettask(L, Ptr<ITask>(new TaskMoveORPO(pNode, trans._translation, trans._rotation, duration, f)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskMoveORPO(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Ptr<Node> pTarget         = luat_tonode(L, 2);
    float duration            = luat_tofloat(L, 3);
    Core::EInterpolateFunc f  = luat_tointerpolate(L, 4);

    return luat_rettask(L, Ptr<ITask>(new TaskMoveORPO(pNode, pTarget->getWorldPosition(), pTarget->getLocalOrient(), duration, f)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskAnimate(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<NodeSkeleton> pNode   = luat_tonodeskeleton(L, 1);
    String anim               = luat_tostring(L, 2);
    float triggerKey          = luat_tofloat(L, 3);
    bool loop                 = (lua_gettop(L) >= 4) ? luat_toboolean(L, 4) : false;
    float duration            = (lua_gettop(L) >= 5) ? luat_tofloat(L, 5) : 0.0f;
    float keyFrom             = (lua_gettop(L) >= 6) ? luat_tofloat(L, 6) : 0.0f;
    float keyTo               = (lua_gettop(L) >= 7) ? luat_tofloat(L, 7) : 1.0f;

    return luat_rettask(L, Ptr<ITask>(new TaskAnimate(pNode, anim, /*enqueue = */false, triggerKey, loop, duration, keyFrom, keyTo)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskEnqueueAnimate(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<NodeSkeleton> pNode   = luat_tonodeskeleton(L, 1);
    String anim               = luat_tostring(L, 2);
    float triggerKey          = luat_tofloat(L, 3);
    bool loop                 = (lua_gettop(L) >= 4) ? luat_toboolean(L, 4) : false;
    float duration            = (lua_gettop(L) >= 5) ? luat_tofloat(L, 5) : 0.0f;
    float keyFrom             = (lua_gettop(L) >= 6) ? luat_tofloat(L, 6) : 0.0f;
    float keyTo               = (lua_gettop(L) >= 7) ? luat_tofloat(L, 7) : 1.0f;

    return luat_rettask(L, Ptr<ITask>(new TaskAnimate(pNode, anim, /*enqueue = */true, triggerKey, loop, duration, keyFrom, keyTo)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskWaitTag(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    String tag                = luat_tostring(L, 2);
    bool hasTag               = (lua_gettop(L) >= 3) ? luat_toboolean(L, 3) : true;

    return luat_rettask(L, Ptr<ITask>(new TaskWaitTag(pNode, tag, hasTag)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskWaitTagsAnd(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::List<String> tags;

    for(int32 ii=2; ii < lua_gettop(L); ii++)
        tags.push_back(luat_tostring(L, ii));

    return luat_rettask(L, Ptr<ITask>(new TaskWaitTag(pNode, tags, WAIT_TAGS_AND)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskWaitTagsOr(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Ptr<Node> pNode           = luat_tonode(L, 1);
    Core::List<String> tags;

    for(int32 ii=2; ii < lua_gettop(L); ii++)
        tags.push_back(luat_tostring(L, ii));

    return luat_rettask(L, Ptr<ITask>(new TaskWaitTag(pNode, tags, WAIT_TAGS_OR)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskSkyColorFade(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Core::Vector4f col        = luat_tovec4f(L, 1);
    float duration            = luat_tofloat(L, 2);

    return luat_rettask(L, Ptr<ITask>(new TaskSkyColorFade(pWorld, col, duration)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskSkyGlowFade(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    Core::Vector4f col        = luat_tovec4f(L, 1);
    float duration            = luat_tofloat(L, 2);

    return luat_rettask(L, Ptr<ITask>(new TaskSkyGlowFade(pWorld, col, duration)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskGlobalColorFade(lua_State * L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld            = luat_getworld(L);
    float globalFade          = luat_tofloat(L, 1);
    float duration            = luat_tofloat(L, 2);

    return luat_rettask(L, Ptr<ITask>(new TaskGlobalColorFade(pWorld, globalFade, duration)));
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int luaTaskShake(lua_State * L)
{
    Ptr<Node> pNode           = luat_tonode(L, 1);
    float dx                  = luat_tofloat(L, 2);
    float periodx             = luat_tofloat(L, 3);
    float dy                  = luat_tofloat(L, 4);
    float periody             = luat_tofloat(L, 5);
    float duration            = luat_tofloat(L, 6);
    bool isLocal              = luat_toboolean(L, 7);
    bool isGain               = lua_gettop(L) >= 8 ? luat_toboolean(L, 8) : false;
    Core::EInterpolateFunc f  = lua_gettop(L) >= 9 ? luat_tointerpolate(L, 9) : Core::INTER_LINEAR;

    return luat_rettask(L, Ptr<ITask>(new TaskShake(pNode, dx, periodx, dy, periody, duration, isLocal, isGain, f)));
}
//-----------------------------------------------------------------------------
void registerTaskFunctions(lua_State * L)
{
    lua_register(L, "taskFXRadialBlur", luaTaskFXRadialBlur);
    lua_register(L, "taskFXRadialBlurNode", luaTaskFXRadialBlurNode);
    lua_register(L, "taskFXDirBlur", luaTaskFXDirBlur);
    lua_register(L, "taskFXBlur", luaTaskFXBlur);
    lua_register(L, "taskFXFade", luaTaskFXFade);
    lua_register(L, "taskFXSaturate", luaTaskFXSaturate);

    lua_register(L, "taskTurnAround", luaTaskTurnAround);
    lua_register(L, "taskTurnAroundLoop", luaTaskTurnAroundLoop);

    lua_register(L, "taskTimeWarp", luaTaskTimeWarp);

    lua_register(L, "taskChangeEmitterPeriod", luaTaskChangeEmitterPeriod);
    lua_register(L, "taskChangeEmitterStartSpeed", luaTaskChangeEmitterStartSpeed);
    lua_register(L, "taskChangeEmitterStartWidth", luaTaskChangeEmitterStartWidth);
    lua_register(L, "taskChangeEmitterRadius", luaTaskChangeEmitterRadius);

    lua_register(L, "taskLookAt", luaTaskLookAt);
    lua_register(L, "taskLockLookAtPos", luaTaskLockLookAtPos);
    lua_register(L, "taskLockLookAtNode", luaTaskLockLookAtNode);

    lua_register(L, "taskWait", luaTaskWait);
    lua_register(L, "taskWaitTag", luaTaskWaitTag);    
    lua_register(L, "taskWaitTagsAnd", luaTaskWaitTagsAnd);
    lua_register(L, "taskWaitTagsOr", luaTaskWaitTagsOr);
    lua_register(L, "taskMove", luaTaskMove);
    lua_register(L, "taskMoveORPO", luaTaskMoveORPO);
    lua_register(L, "taskMoveTrajectory", luaTaskMoveTrajectory);
    lua_register(L, "taskMoveTrajectoryPivot", luaTaskMoveTrajectoryPivot);
    lua_register(L, "taskMoveToTrajectoryPoint", luaTaskMoveToTrajectoryPoint);

    lua_register(L, "taskScale", luaTaskScale);
    lua_register(L, "taskScaleLoop", luaTaskScaleLoop);
    lua_register(L, "taskRotate", luaTaskRotate);
    lua_register(L, "taskRotateLoop", luaTaskRotateLoop);

    lua_register(L, "taskFade", luaTaskFade);
    lua_register(L, "taskFade2", luaTaskFade2);
    lua_register(L, "taskFadeLoop", luaTaskFadeLoop);
    lua_register(L, "taskGlowFade", luaTaskGlowFade);
    lua_register(L, "taskGlowFade2", luaTaskGlowFade2);
    lua_register(L, "taskGlowFadeLoop", luaTaskGlowFadeLoop);

    lua_register(L, "taskAnimate", luaTaskAnimate);
    lua_register(L, "taskEnqueueAnimate", luaTaskEnqueueAnimate);

    lua_register(L, "taskSkyColorFade", luaTaskSkyColorFade);
    lua_register(L, "taskSkyGlowFade", luaTaskSkyGlowFade);
    lua_register(L, "taskGlobalColorFade", luaTaskGlobalColorFade);

    lua_register(L, "taskShake", luaTaskShake);
}
//-----------------------------------------------------------------------------
}
