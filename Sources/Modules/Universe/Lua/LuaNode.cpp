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
#include <Universe/Lua/LuaWorld.h>
#include <Universe/Tools.h>

namespace Universe
{
//-----------------------------------------------------------------------------
const char LuaNode::CLASS_NAME [] = "Node";
const Luna<LuaNode>::RegType LuaNode::METHODS [] = {
    {"kill", &LuaNode::kill},
    {"isAlive", &LuaNode::isAlive},
    {"update", &LuaNode::update},
    {"getNodeType", &LuaNode::getNodeType},
    {"getLocalPosition", &LuaNode::getLocalPosition},
    {"getLocalScale", &LuaNode::getLocalScale},
    {"getLocalOrient", &LuaNode::getLocalOrient},
    {"getLocalDirection", &LuaNode::getLocalDirection},
    {"getWorldPosition", &LuaNode::getWorldPosition},
    {"getWorldDirection", &LuaNode::getWorldDirection},

    {"setLocalPosition", &LuaNode::setLocalPosition},
    {"setLocalScale", &LuaNode::setLocalScale},
    {"setLocalOrientation", &LuaNode::setLocalOrientation},
    {"setLocalDirection", &LuaNode::setLocalDirection},
    {"localLookAt", &LuaNode::localLookAt},

    {"setWorldPosition", &LuaNode::setWorldPosition},
    {"setWorldDirection", &LuaNode::setWorldDirection},
    {"worldLookAt", &LuaNode::worldLookAt},

    {"setTrajectoryPosition", &LuaNode::setTrajectoryPosition},

    {"localPitch", &LuaNode::localPitch},
    {"localYaw", &LuaNode::localYaw},
    {"localRoll", &LuaNode::localRoll},

    {"worldPitch", &LuaNode::worldPitch},
    {"worldYaw", &LuaNode::worldYaw},
    {"worldRoll", &LuaNode::worldRoll},

    {"worldToLocal", &LuaNode::worldToLocal},
    {"localToWorld", &LuaNode::localToWorld},
    
    {"setUpVector", &LuaNode::setUpVector},
    {"getUpVector", &LuaNode::getUpVector},

    {"setTimeMultiplier", &LuaNode::setTimeMultiplier},
    {"getTimeMultiplier", &LuaNode::getTimeMultiplier},

    {"setVisible", &LuaNode::setVisible},
    {"isVisible", &LuaNode::isVisible},

    {"setBoundingBoxVisible", &LuaNode::setBoundingBoxVisible},
    {"isBoundingBoxVisible", &LuaNode::isBoundingBoxVisible},

    {"addTag", &LuaNode::addTag},
    {"removeTag", &LuaNode::removeTag},
    {"hasTag", &LuaNode::hasTag},

    {"isRoot", &LuaNode::isRoot},
    {"addChild", &LuaNode::addChild},
    {"removeChild", &LuaNode::removeChild},

    {"getChildrenWithTag", &LuaNode::getChildrenWithTag},
    {"getChildWithTag", &LuaNode::getChildWithTag},
    {"getChildrenOfType", &LuaNode::getChildrenOfType},

    {"getChildren", &LuaNode::getChildren},

    {"restart", &LuaNode::restart},
    {"stop", &LuaNode::stop},

    {"setTexture", &LuaNode::setTexture},
    {"getTexture", &LuaNode::getTexture},

    {"setSize", &LuaNode::setSize},
    {"getSize", &LuaNode::getSize},

    {"setMode", &LuaNode::setMode},
    {"getMode", &LuaNode::getMode},

    {"setColor", &LuaNode::setColor},
    {"getColor", &LuaNode::getColor},

    {"setGlow", &LuaNode::setGlow},
    {"getGlow", &LuaNode::getGlow},

    {"setSpriteLayout", &LuaNode::setSpriteLayout},
    {"getSpriteLayout", &LuaNode::getSpriteLayout},

    {"hasSpriteLayout", &LuaNode::hasSpriteLayout},

    {"animateSprite", &LuaNode::animateSprite},
    {"getDuration", &LuaNode::getDuration},
    {"getKeyFrom", &LuaNode::getKeyFrom},
    {"getKeyTo", &LuaNode::getKeyTo},
    {"getLoop", &LuaNode::getLoop},


    {"setTimeKey", &LuaNode::setTimeKey},
    {"getTimeKey", &LuaNode::getTimeKey},

    {"setMaterial", &LuaNode::setMaterial},
    {"getMaterial", &LuaNode::getMaterial},

    {"getModel", &LuaNode::getModel},
    {"setModel", &LuaNode::setModel},

    {"getSkin", &LuaNode::getSkin},
    {"getSkins", &LuaNode::getSkins},
    {"getSkeletonModel", &LuaNode::getSkeletonModel},
    {"createSkin", &LuaNode::createSkin},
    {"setDefaultPose", &LuaNode::setDefaultPose},
    {"animate", &LuaNode::animate},
    {"enqueueAnim", &LuaNode::enqueueAnim},

    {"setAmbientColor", &LuaNode::setAmbientColor},
    {"setDiffuseColor", &LuaNode::setDiffuseColor},
    {"setSpecularColor", &LuaNode::setSpecularColor},
    {"setRadius", &LuaNode::setRadius},

    {"getShadowCasting", &LuaNode::getShadowCasting},
    {"getAmbientColor", &LuaNode::getAmbientColor},
    {"getDiffuseColor", &LuaNode::getDiffuseColor},
    {"getSpecularColor", &LuaNode::getSpecularColor},
    {"getRadius", &LuaNode::getRadius},

    {"setPeriod", &LuaNode::setPeriod},
    {"setStickyParticles", &LuaNode::setStickyParticles},
    {"setGenerationTime", &LuaNode::setGenerationTime},
    {"killAtEnd", &LuaNode::killAtEnd},

    {"getPeriod", &LuaNode::getPeriod},
    {"getStickyParticles", &LuaNode::getStickyParticles},
    {"getGenerationTime", &LuaNode::getGenerationTime},
    {"getKillAtEnd", &LuaNode::getKillAtEnd},

    {"addAttractor", &LuaNode::addAttractor},
    {"addForce", &LuaNode::addForce},
    {"removeEffect", &LuaNode::removeEffect},

    {"setLength", &LuaNode::setLength},
    {"setDivisions", &LuaNode::setDivisions},

    {"setHeadWidth", &LuaNode::setHeadWidth},
    {"setTailWidth", &LuaNode::setTailWidth},

    {"setHeadU", &LuaNode::setHeadU},
    {"setTailU", &LuaNode::setTailU},

    {"setHeadColor", &LuaNode::setHeadColor},
    {"setTailColor", &LuaNode::setTailColor},

    {"setHeadGlow", &LuaNode::setHeadGlow},
    {"setTailGlow", &LuaNode::setTailGlow},

    {"getLength", &LuaNode::getLength},
    {"getDivisions", &LuaNode::getDivisions},

    {"getHeadWidth", &LuaNode::getHeadWidth},
    {"getTailWidth", &LuaNode::getTailWidth},

    {"getHeadU", &LuaNode::getHeadU},
    {"getTailU", &LuaNode::getTailU},

    {"getHeadColor", &LuaNode::getHeadColor},
    {"getTailColor", &LuaNode::getTailColor},

    {"getHeadGlow", &LuaNode::getHeadGlow},
    {"getTailGlow", &LuaNode::getTailGlow},

    {"setHeadSphereCenter", &LuaNode::setHeadSphereCenter},
    {"setHeadSphereSize", &LuaNode::setHeadSphereSize},
    {"setTailSphereCenter", &LuaNode::setTailSphereCenter},
    {"setTailSphereSize", &LuaNode::setTailSphereSize},
    {"setTailNode", &LuaNode::setTailNode},

    {"getHeadSphereCenter", &LuaNode::getHeadSphereCenter},
    {"getHeadSphereSize", &LuaNode::getHeadSphereSize},
    {"getTailSphereCenter", &LuaNode::getTailSphereCenter},
    {"getTailSphereSize", &LuaNode::getTailSphereSize},
    {"getTailNode", &LuaNode::getTailNode},

    {"enqueue", &LuaNode::enqueue},
    {"eraseAllSounds", &LuaNode::eraseAllSounds},
    {"setGain", &LuaNode::setGain},
    {"getGain", &LuaNode::getGain},
    {"stopCurrentLoop", &LuaNode::stopCurrentLoop},
    {"playbackCompleted", &LuaNode::playbackCompleted},
    {"fadeIn", &LuaNode::fadeIn},
    {"fadeOut", &LuaNode::fadeOut},
    {"setPause", &LuaNode::setPause},
    {"getPaused", &LuaNode::getPaused},
    {"setStaticSound", &LuaNode::setStaticSound},
    {"isStaticSound", &LuaNode::isStaticSound},
    {"fadeOutChildren", &LuaNode::fadeOutChildren},
    {"fadeInChildren", &LuaNode::fadeOutChildren},

    {0}
};
//-----------------------------------------------------------------------------
LuaNode::LuaNode(lua_State* L)
{
    lua_pushstring(L, "pNode");
    lua_gettable(L, -2);
    Ptr<Node> * pNode = reinterpret_cast<Ptr<Node>*>(lua_touserdata(L, -1));
    
    if(pNode == NULL)
        luat_error(L, "NULL Universe::Node");

    _pNode = *pNode;
    lua_pop(L, 2);
}
//-----------------------------------------------------------------------------
LuaNode::~LuaNode()
{
    _pNode = null;
}
//-----------------------------------------------------------------------------
int LuaNode::kill(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->kill();
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::isAlive(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    lua_pushboolean(L, int(_pNode->isAlive()));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::update(lua_State* L)
{
    LM_LUA_FUNC_START(L);

    if(lua_gettop(L) >= 2 && luat_toboolean(L, 2))
    {
        float t = luat_tofloat(L, 1);
        Core::List<Ptr<Node> > children;
        Universe::getAllChildren(children, *_pNode);
        children.push_back(_pNode);

        for(int32 ii=0; ii < children.size(); ii++)
            children[ii]->update(t);
    }
    else
    {
        _pNode->update(luat_tofloat(L, 1));
    }
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getNodeType(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushstring(L, Universe::toString(_pNode->getNodeType()));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLocalPosition(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getLocalPosition());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLocalScale(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getLocalScale());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLocalOrient(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushquatf(L, _pNode->getLocalOrient());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getLocalDirection(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getLocalDirection());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getWorldPosition(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getWorldPosition());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getWorldDirection(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getWorldDirection());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::worldToLocal(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getWorldToLocalMatrix().apply(luat_tovec3f(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::localToWorld(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getLocalToWorldMatrix().apply(luat_tovec3f(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
//int getLocalMatrix(lua_State* L);
//int getLocalToWorldMatrix(lua_State* L);
//int getWorldToLocalMatrix(lua_State* L);
//int getParentLocalToWorldMatrix(lua_State* L);
//int getParentWorldToLocalMatrix(lua_State* L);
//-----------------------------------------------------------------------------
int LuaNode::setLocalPosition(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setLocalPosition(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setLocalScale(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setLocalScale(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setLocalOrientation(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setLocalOrientation(luat_toquatf(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setLocalDirection(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setLocalDirection(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::localLookAt(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->localLookAt(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setWorldPosition(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setWorldPosition(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setWorldDirection(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setWorldDirection(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::worldLookAt(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->worldLookAt(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::localPitch(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->localPitch(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::localYaw(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->localYaw(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::localRoll(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->localRoll(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::worldPitch(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->worldPitch(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::worldYaw(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->worldYaw(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::worldRoll(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->worldRoll(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setUpVector(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setUpVector(luat_tovec3f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getUpVector(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec3f(L, _pNode->getUpVector());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTimeMultiplier(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 2)
        _pNode->setTimeMultiplier(luat_tofloat(L, 1), luat_toboolean(L, 2));
    else
        _pNode->setTimeMultiplier(luat_tofloat(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getTimeMultiplier(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    lua_pushnumber(L, _pNode->getTimeMultiplier());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 2)
        _pNode->setVisible(luat_toboolean(L, 1), luat_toboolean(L, 2));
    else
        _pNode->setVisible(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::isVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushboolean(L, _pNode->isVisible());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setBoundingBoxVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->setBoundingBoxVisible(luat_toboolean(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::isBoundingBoxVisible(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushboolean(L, _pNode->isBoundingBoxVisible());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::addTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->addTag(luat_tostring(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::removeTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->removeTag(luat_tostring(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::hasTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushboolean(L, _pNode->hasTag(luat_tostring(L, 1)));
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::isRoot(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushboolean(L, _pNode->isRoot());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::addChild(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->addChild(luat_tonode(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::removeChild(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    _pNode->removeChild(luat_tonode(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getChildrenWithTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Core::List<Ptr<Node> > nodes;

    if(lua_gettop(L) >= 2)
        _pNode->getChildrenWithTag(luat_tostring(L, 1), nodes, luat_toboolean(L, 2));
    else
        _pNode->getChildrenWithTag(luat_tostring(L, 1), nodes);

    luat_pushnodes(L, nodes);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getChildrenOfType(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Core::List<Ptr<Node> > nodes;

    if(lua_gettop(L) >= 2)
        _pNode->getChildrenOfType(luat_tonodetype(L, 1), nodes, luat_toboolean(L, 2));
    else
        _pNode->getChildrenOfType(luat_tonodetype(L, 1), nodes);

    luat_pushnodes(L, nodes);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getChildWithTag(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Ptr<Node> pChild;

    if(lua_gettop(L) >= 2)
        pChild = _pNode->getChildWithTag(luat_tostring(L, 1), luat_toboolean(L, 2));
    else
        pChild = _pNode->getChildWithTag(luat_tostring(L, 1));

    luat_pushnode(L, pChild);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getChildren(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushnodes(L, _pNode->_children);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 2)
        _pNode->setNodeColor(luat_tovec4f(L, 1), luat_toboolean(L, 2));
    else
        _pNode->setNodeColor(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getColor(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec4f(L, _pNode->getNodeColor());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    if(lua_gettop(L) >= 2)
        _pNode->setNodeGlow(luat_tovec4f(L, 1), luat_toboolean(L, 2));
    else
        _pNode->setNodeGlow(luat_tovec4f(L, 1));
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getGlow(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    luat_pushvec4f(L, _pNode->getNodeGlow());
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::restart(lua_State* L)
{
    LM_LUA_FUNC_START(L);

    if(lua_gettop(L) >= 1)
    {
        _pNode->restart(luat_toboolean(L, 1));
    }
    else
    {
        _pNode->restart();
    }

    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::stop(lua_State* L)
{
    LM_LUA_FUNC_START(L);

    if(lua_gettop(L) >= 2)
    {
        _pNode->stop(luat_toboolean(L, 1), luat_tofloat(L, 2));
    }
    else if(lua_gettop(L) >= 1)
    {
        _pNode->stop(luat_toboolean(L, 1));
    }
    else
    {
        _pNode->stop();
    }

    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setTrajectoryPosition(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    World * pWorld          = luat_getworld(L);
    String spline           = luat_tostring(L, 1);
    Ptr<Node> pPivot        = luat_tonode(L, 2);
    float key               = luat_tofloat(L, 3);

    Ptr<Core::Trajectory> pSpline(pWorld->getRessourcesPool()->getSpline(spline));

    Core::Matrix4f mat;
    Universe::getCleanMatrix(mat, *pPivot, *pSpline, key * pSpline->getDuration());

    _pNode->setLocalMatrix(mat);
   
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::setMaterial(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Assets::Material mat;
    luat_tomaterial(L, 1, mat);
    if(lua_gettop(L) >= 2)
        _pNode->setMaterial(mat, luat_toboolean(L, 2));
    else
        _pNode->setMaterial(mat);
    return 0;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
int LuaNode::getMaterial(lua_State* L)
{
    LM_LUA_FUNC_START(L);
    Assets::Material mat;
    _pNode->getMaterial(mat);
    luat_pushmaterial(L, mat);
    return 1;
    LM_LUA_FUNC_END(L);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
Ptr<Node> luat_tonode(lua_State *L, int32 n)
{
    return Luna<LuaNode>::check(L, n, false)->getNode();
}
//-----------------------------------------------------------------------------
Ptr<NodeEmitter> luat_tonodeemitter(lua_State *L, int32 n)
{
    Ptr<NodeEmitter> pResult;
    Ptr<Node> pNode = luat_tonode(L, n);
    
    if(pNode->getNodeType() != NODE_EMITTER)
        luat_typeerror(L, n, "NodeEmitter");
    else
        pResult = LM_DEBUG_PTR_CAST<NodeEmitter>(pNode);

    return pResult;
}
//-----------------------------------------------------------------------------
Ptr<NodeCamera>    luat_tonodecamera(lua_State *L, int32 n)
{
    Ptr<NodeCamera> pResult;
    Ptr<Node> pNode = luat_tonode(L, n);
    
    if(pNode->getNodeType() != NODE_CAMERA)
        luat_typeerror(L, n, "NodeCamera");
    else
        pResult = LM_DEBUG_PTR_CAST<NodeCamera>(pNode);

    return pResult;
//-----------------------------------------------------------------------------
}
Ptr<NodeSkeleton>    luat_tonodeskeleton(lua_State *L, int32 n)
{
    Ptr<NodeSkeleton> pResult;
    Ptr<Node> pNode = luat_tonode(L, n);
    
    if(pNode->getNodeType() != NODE_SKELETON)
        luat_typeerror(L, n, "NodeSkeleton");
    else
        pResult = LM_DEBUG_PTR_CAST<NodeSkeleton>(pNode);

    return pResult;
}
//-----------------------------------------------------------------------------
void luat_pushnode(lua_State *L, const Ptr<Node> & pNode)
{
    if(pNode == null)
    {
        lua_pushnil(L);
    }
    else
    {
        Ptr<Node> pNodeCopy(pNode);
        lua_getglobal(L, "Node");

        lua_newtable(L); // Arg1 = table {pNode=x}
        lua_pushlightuserdata(L, &pNodeCopy);
        lua_setfield(L, -2, "pNode");

        lua_pcall(L, 1, 1, 0);
    }
}
//-----------------------------------------------------------------------------
void luat_pushnodes(lua_State *L, const Core::List<Ptr<Node> > & nodes)
{
    lua_createtable(L, nodes.size(), 0);

    for(int32 ii=0; ii < nodes.size(); ii++)
    {
        lua_pushinteger(L, ii + 1);
        luat_pushnode(L, nodes[ii]);
        lua_settable(L, -3);
    }
}
//-----------------------------------------------------------------------------
}