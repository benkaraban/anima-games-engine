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
#ifndef UNIVERSE_NODESKELETON_H_
#define UNIVERSE_NODESKELETON_H_

#include <Core/Queue.h>
#include <Assets/SkeletonPose.h>
#include <Renderer/IMesh.h>
#include <Universe/Node.h>
#include <Universe/RessourcePool.h>

#define DEBUG_NODE_SKELETON_ANIM 0

namespace Universe
{
class LM_API_UNI SkeletonSkin : public Core::Object, public Core::IBoxable
{
public:
    SkeletonSkin(const Ptr<Renderer::IAABox> & pBox,
                 const Ptr<Renderer::ISkinMeshInstance> & pSkin,
                 const Ptr<RessourcePool> & pRessourcesPool);

    virtual void setMesh(const String & modelName);
    virtual void setMesh(const String & modelName, const String & meshName);
    virtual void setDiffuseTexture(const String & texName);
    virtual void setSpecularTexture(const String & texName);
    virtual void setNormalTexture(const String & texName);

    virtual void getDiffuseTexture(String & texName);
    virtual void getSpecularTexture(String & texName);
    virtual void getNormalTexture(String & texName);

    virtual void getMaterial(      Assets::Material & material);
    virtual void setMaterial(const Assets::Material & material);

    virtual inline const String & getModelName() const { return _modelName; }
    virtual inline const String & getMeshName() const { return _meshName; }

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    bool isAlive() const;
    void kill();

    void setVisible(bool flag);
    bool isVisible() const;

    void setBoundingBoxVisible(bool flag);
    bool isBoundingBoxVisible() const; 

    const Core::AABoxf & getBoundingBox() const;
    void updateBoundingBox();

    const Ptr<Renderer::ISkinMeshInstance> & getSkin() const { return _pSkin; };

    bool isIntersectingBoundingBox(const Core::Rayf& ray, float& distance) const;
    bool isIntersecting(const Core::Rayf& ray, float& distance) const;

protected:
    Ptr<Renderer::IAABox>              _pBox;
    Ptr<Renderer::ISkinMeshInstance>   _pSkin;
    Ptr<RessourcePool>                 _pRessourcesPool;
    String                             _modelName;
    String                             _meshName;
    String                             _diffuseTexName;
    String                             _specularTexName;
    String                             _normalTexName;
    bool                               _isAlive;
    bool                               _isVisible;
    bool                               _isBoxVisible;
};

class LM_API_UNI IAnimObserver
{
public:
    virtual ~IAnimObserver() {};

    virtual void animProgress(float p) = 0;
};

class LM_API_UNI NodeSkeleton : public Node
{
public:
    NodeSkeleton(const Ptr<Renderer::IAABox> & pBox,
                 const Ptr<Renderer::IScene> & pScene,
                 const Ptr<RessourcePool> & pRessourcesPool,
                 World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_SKELETON; };

    virtual void setModel(const String & modelName);

    Ptr<SkeletonSkin> createSkin();
    Ptr<SkeletonSkin> createSkin(const String & modelName);
    Ptr<SkeletonSkin> createSkin(const String & modelName, const String & meshName);

    void              createAllSkins(const String & modelName);

    int32             getSkinCount() const { return _skins.size(); }
    Ptr<SkeletonSkin> getSkin(int32 id) const { return _skins[id]; };
    Ptr<SkeletonSkin> getSkin(const String & meshName) const;
    Ptr<SkeletonSkin> getSkin(const String & modelName, const String & meshName) const;

    void addModelNode(int32 iNode, const Ptr<Node> & pNode);

    int32 getAnimCount() const;
    const String & getAnimName(int32 id) const;

    void removeObserver(IAnimObserver * pObs);

    void setDefaultPose();

    void animate(int32          id,   double duration, float keyFrom = 0.0, float keyTo = 1.0, bool loop = false, IAnimObserver * pObserver = NULL);
    void animate(const String & name, double duration, float keyFrom = 0.0, float keyTo = 1.0, bool loop = false, IAnimObserver * pObserver = NULL);

    void enqueueAnim(int32          id,   double duration, float keyFrom = 0.0, float keyTo = 1.0, bool loop = false, IAnimObserver * pObserver = NULL);
    void enqueueAnim(const String & name, double duration, float keyFrom = 0.0, float keyTo = 1.0, bool loop = false, IAnimObserver * pObserver = NULL);

    virtual inline const String & getModelName() const { return _modelName; }

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    // KEEP ???

    const Ptr<Renderer::ISkeletonInstance> & getSkeleton() const;

    // PACKAGE

    virtual void update(double elapsed);

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    virtual void updateSkeleton(int32 id, float key);

    Ptr<Renderer::IScene>            _pScene;
    Ptr<RessourcePool>               _pRessourcesPool;
    Ptr<Renderer::ISkeletonInstance> _pSkeleton;
    String                           _modelName;

    Core::List<Ptr<Node> >           _modelNodes;

    Ptr<Assets::Model>               _pModel;
    Ptr<Assets::SkeletonPose>        _pSkelPose;
    Assets::ModelPose                _pose;
    Core::List<Ptr<SkeletonSkin> >   _skins;

    struct AnimInfo
    {
        AnimInfo();

        int32               id;
        bool                loop;
        float               keyFrom;
        float               keyTo;
        double              duration;
        IAnimObserver *     pObserver;
    };

    Core::Queue<AnimInfo> _anims;

#if DEBUG_NODE_SKELETON_ANIM
    int32    _lastAnim;
#endif
    double   _elapsed;

    friend class World;
};

}

#endif /* NODESKELETON_H_ */
