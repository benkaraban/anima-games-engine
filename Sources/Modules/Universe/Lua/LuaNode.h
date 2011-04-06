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
#ifndef UNIVERSE_LUANODE_H_
#define UNIVERSE_LUANODE_H_

#include <Universe/Node.h>
#include <Universe/NodeCamera.h>
#include <Universe/NodeDecal.h>
#include <Universe/NodeEmitter.h>
#include <Universe/NodeMesh.h>
#include <Universe/NodeSkeleton.h>
#include <Universe/NodeStorm.h>
#include <Universe/NodePointLight.h>
#include <Universe/NodeTrail.h>
#include <Universe/NodeSoundSource.h>
#include <Universe/Lua/LuaTools.h>
#include <Universe/Lua/LuaConstants.h>

#include <luna.h>

namespace Universe
{
class LM_API_UNI LuaNode
{
public:
    LuaNode(lua_State* L);
    ~LuaNode();

    const Ptr<Node> & getNode() const { return _pNode; };

    //-----------------------------------------------------------
    int kill(lua_State* L);
    int isAlive(lua_State* L);

    int update(lua_State* L);

    int getNodeType(lua_State* L);

    int getLocalPosition(lua_State* L);
    int getLocalScale(lua_State* L);
    int getLocalOrient(lua_State* L);
    int getLocalDirection(lua_State* L);

    int getWorldPosition(lua_State* L);
    int getWorldDirection(lua_State* L);

    //int getLocalMatrix(lua_State* L);
    //int getLocalToWorldMatrix(lua_State* L);
    //int getWorldToLocalMatrix(lua_State* L);

    //int getParentLocalToWorldMatrix(lua_State* L);
    //int getParentWorldToLocalMatrix(lua_State* L);

    int worldToLocal(lua_State* L);
    int localToWorld(lua_State* L);

    int setLocalPosition(lua_State* L);
    int setLocalScale(lua_State* L);
    int setLocalOrientation(lua_State* L);
    int setLocalDirection(lua_State* L);
    int localLookAt(lua_State* L);

    int setWorldPosition(lua_State* L);
    int setWorldDirection(lua_State* L);
    int worldLookAt(lua_State* L);

    int localPitch(lua_State* L);
    int localYaw(lua_State* L);
    int localRoll(lua_State* L);

    int worldPitch(lua_State* L);
    int worldYaw(lua_State* L);
    int worldRoll(lua_State* L);

    int setUpVector(lua_State* L);
    int getUpVector(lua_State* L);

    int setTimeMultiplier(lua_State* L);
    int getTimeMultiplier(lua_State* L);

    int setVisible(lua_State* L);
    int isVisible(lua_State* L);

    int setBoundingBoxVisible(lua_State* L);
    int isBoundingBoxVisible(lua_State* L);

    int addTag(lua_State* L);
    int removeTag(lua_State* L);
    int hasTag(lua_State* L);

    int getChildrenWithTag(lua_State* L); // retourne une liste
    int getChildWithTag(lua_State* L);
    int getChildrenOfType(lua_State* L); // retourne une liste

    int isRoot(lua_State* L);
    int addChild(lua_State* L);
    int removeChild(lua_State* L);

    int getChildren(lua_State* L); // retourne une liste

    int setTrajectoryPosition(lua_State* L);

    int restart(lua_State* L);
    int stop(lua_State* L);

    //-----------------------------------------------------------
    // NODE_MESH
    //-----------------------------------------------------------

    int setMaterial(lua_State* L);
    int getMaterial(lua_State* L);

    int getModel(lua_State* L); // retourne model, mesh
    int setModel(lua_State* L);

    //-----------------------------------------------------------
    // NODE_SKELETON
    //-----------------------------------------------------------

    int getSkeletonModel(lua_State* L);
    int createSkin(lua_State* L);

    int getSkin(lua_State* L);
    int getSkins(lua_State* L); // retourne la liste des skins

    int setDefaultPose(lua_State* L);
    int animate(lua_State* L);
    int enqueueAnim(lua_State* L);

    //-----------------------------------------------------------
    // NODE_POINT_LIGHT
    //-----------------------------------------------------------

    int setAmbientColor(lua_State* L);
    int setDiffuseColor(lua_State* L);
    int setSpecularColor(lua_State* L);
    int setRadius(lua_State* L);

    int getShadowCasting(lua_State* L);
    int getAmbientColor(lua_State* L);
    int getDiffuseColor(lua_State* L);
    int getSpecularColor(lua_State* L);
    int getRadius(lua_State* L);

    //-----------------------------------------------------------
    // NODE_EMITTER / NODE_STORM
    //-----------------------------------------------------------
    

    int setPeriod(lua_State* L);
    int setGenerationTime(lua_State* L);
    int killAtEnd(lua_State* L);

    int getPeriod(lua_State* L);
    int getGenerationTime(lua_State* L);
    int getKillAtEnd(lua_State* L);
    
    //-----------------------------------------------------------
    // NODE_STORM
    //-----------------------------------------------------------

    int setHeadSphereCenter(lua_State* L);
    int setHeadSphereSize(lua_State* L);
    int setTailSphereCenter(lua_State* L);
    int setTailSphereSize(lua_State* L);
    int setTailNode(lua_State* L);

    int getHeadSphereCenter(lua_State* L);
    int getHeadSphereSize(lua_State* L);
    int getTailSphereCenter(lua_State* L);
    int getTailSphereSize(lua_State* L);
    int getTailNode(lua_State* L);

    //-----------------------------------------------------------
    // NODE_EMITTER
    //-----------------------------------------------------------

    int setStickyParticles(lua_State* L);
    int getStickyParticles(lua_State* L);
    int addAttractor(lua_State* L);
    int addForce(lua_State* L);
    int removeEffect(lua_State* L);
    

    //-----------------------------------------------------------
    // NODE_TRAIL
    //-----------------------------------------------------------

    int setLength(lua_State* L);
    int setDivisions(lua_State* L);

    int setHeadWidth(lua_State* L);
    int setTailWidth(lua_State* L);

    int setHeadU(lua_State* L);
    int setTailU(lua_State* L);

    int setHeadColor(lua_State* L);
    int setTailColor(lua_State* L);

    int setHeadGlow(lua_State* L);
    int setTailGlow(lua_State* L);

    int getLength(lua_State* L);
    int getDivisions(lua_State* L);

    int getHeadWidth(lua_State* L);
    int getTailWidth(lua_State* L);

    int getHeadU(lua_State* L);
    int getTailU(lua_State* L);

    int getHeadColor(lua_State* L);
    int getTailColor(lua_State* L);

    int getHeadGlow(lua_State* L);
    int getTailGlow(lua_State* L);

    //-----------------------------------------------------------
    // NODE_DECAL / NODE_TRAIL
    //-----------------------------------------------------------

    int setTexture(lua_State* L);
    int getTexture(lua_State* L);

    int setMode(lua_State* L);
    int getMode(lua_State* L);

    //-----------------------------------------------------------
    // NODE_DECAL
    //-----------------------------------------------------------

    int setSize(lua_State* L);
    int getSize(lua_State* L); // retourne width, height

    int setColor(lua_State* L);
    int getColor(lua_State* L);

    int setGlow(lua_State* L);
    int getGlow(lua_State* L);

    int setSpriteLayout(lua_State* L);
    int getSpriteLayout(lua_State* L);

    int hasSpriteLayout(lua_State* L);

    int animateSprite(lua_State* L);
    int getDuration(lua_State* L);
    int getKeyFrom(lua_State* L);
    int getKeyTo(lua_State* L);
    int getLoop(lua_State* L);

    int setTimeKey(lua_State* L);
    int getTimeKey(lua_State* L);

    //-----------------------------------------------------------
    // NODE_SOUND_SOURCE
    //-----------------------------------------------------------
    int enqueue(lua_State* L);
    int eraseAllSounds(lua_State* L);
    int setGain(lua_State* L);
    int getGain(lua_State* L);
    int stopCurrentLoop(lua_State* L);
    int playbackCompleted(lua_State* L);
    int fadeIn(lua_State* L);
    int fadeOut(lua_State* L);
    int setPause(lua_State* L);
    int getPaused(lua_State* L);
    int setStaticSound(lua_State* L);
    int isStaticSound(lua_State* L);
    int fadeOutChildren(lua_State* L);
    int fadeInChildren(lua_State* L);


    //-----------------------------------------------------------
    static const char CLASS_NAME [];
    static const Luna<LuaNode>::RegType METHODS[];

protected:
    Ptr<Node>   _pNode;

    void badNodeType(lua_State* L, ENodeType expected) const;
    Ptr<NodeDecal> getNodeDecal(lua_State* L) const;
    Ptr<NodeMesh> getNodeMesh(lua_State* L) const;
    Ptr<NodeSkeleton> getNodeSkeleton(lua_State* L) const;
    Ptr<NodePointLight> getNodePointLight(lua_State* L) const;
    Ptr<NodeEmitter> getNodeEmitter(lua_State* L) const;
    Ptr<NodeTrail> getNodeTrail(lua_State* L) const;
    Ptr<NodeStorm> getNodeStorm(lua_State* L) const;
    Ptr<NodeSoundSource> getNodeSoundSource(lua_State* L) const;
};

LM_API_UNI Ptr<Node>          luat_tonode(lua_State *L, int32 n);
LM_API_UNI Ptr<NodeEmitter>   luat_tonodeemitter(lua_State *L, int32 n);
LM_API_UNI Ptr<NodeCamera>    luat_tonodecamera(lua_State *L, int32 n);
LM_API_UNI Ptr<NodeSkeleton>  luat_tonodeskeleton(lua_State *L, int32 n);

LM_API_UNI void        luat_pushnode(lua_State *L, const Ptr<Node> & pNode);
LM_API_UNI void        luat_pushnodes(lua_State *L, const Core::List<Ptr<Node> > & nodes);

}

#endif
