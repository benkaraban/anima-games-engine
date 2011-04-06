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
#ifndef RENDERER_COMMON_MESH_H_
#define RENDERER_COMMON_MESH_H_

#include <Renderer/IMesh.h>
#include <Renderer/Common/PackedVertices.h>
#include <Assets/SkeletonPose.h>

namespace Renderer
{
//-----------------------------------------------------------------------------
class LM_API_RDR Mesh : public Core::Object, public IMesh
{
public:
    Mesh(const Ptr<Assets::VerticeSet> & pSourceVerts, const Ptr<IPackedStaticVertices> & pPackedVerts, const Material & material);
    virtual Core::AABoxf getBoundingBox() const { return _box; };

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const { return _pSourceVerts; };

    virtual const Renderer::Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    const IPackedStaticVertices & getVertices() const { return *_pPackedVerts; };

    virtual const Core::List<int32> & usedBones() const { return _usedBones; };

    virtual void copyMesh(const Ptr<IMesh> & pMesh);

protected:
    Ptr<Assets::VerticeSet>     _pSourceVerts;
    Ptr<IPackedStaticVertices>  _pPackedVerts;
    Material                    _material;
    Core::List<int32>           _usedBones;
    Core::AABoxf                _box;
};
//-----------------------------------------------------------------------------
class LM_API_RDR SkinMesh : public Core::Object, public ISkinMesh
{
public:
    SkinMesh(const Ptr<Assets::VerticeSet> & pSourceVerts, const Ptr<IPackedSkinnedVertices> & pPackedVerts, const Material & material);

    virtual Core::AABoxf getBoundingBox(const Assets::SkeletonPose & skelPose) const;

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const { return _pSourceVerts; };

    virtual void setBindShapeMatrix(const Core::Matrix4f & bindShapeMatrix);
    virtual const Core::Matrix4f & getBindShapeMatrix() const { return _bindShapeMatrix; };

    virtual const Renderer::Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    const IPackedSkinnedVertices & getVertices() const { return *_pPackedVerts; };

    virtual const Core::List<int32> & usedBones() const { return _usedBones; };

    virtual void copySkinMesh(const Ptr<ISkinMesh> & pMesh);

protected:
    Ptr<Assets::VerticeSet>     _pSourceVerts;
    Ptr<IPackedSkinnedVertices> _pPackedVerts;
    Material                    _material;
    Core::List<int32>           _usedBones;
    Core::List<Core::AABoxf>    _boxes;
    Core::Matrix4f              _bindShapeMatrix;
    bool                        _invertedCulling;
};
//-----------------------------------------------------------------------------
class LM_API_RDR MeshInstance : public Core::Object, public IMeshInstance
{
public:
    MeshInstance(const Ptr<IMesh> & pMesh);

    virtual Ptr<IMesh> getMesh() const;
    virtual void setMesh(const Ptr<IMesh> & pMesh);

    virtual const Renderer::Material & getMaterial() const;
    virtual void setMaterial(const Renderer::Material & material);

    virtual const Core::Matrix4f & getWorldMatrix() const;
    virtual void setWorldMatrix(const Core::Matrix4f & matrix);

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

protected:
    virtual void updateBoundingBox();

    Ptr<IMesh>         _pMesh;
    Renderer::Material _material;
    Core::Matrix4f     _worldMatrix;
    Core::AABoxf       _box;
    bool               _isAlive;
    bool               _isVisible;
};
//-----------------------------------------------------------------------------
class LM_API_RDR IPoseObserver
{
public:
    virtual ~IPoseObserver() {};
    virtual void onPoseChange(const Assets::SkeletonPose & pose) = 0;
};
//-----------------------------------------------------------------------------
class LM_API_RDR SkeletonInstance : public ISkeletonInstance
{
public:
    SkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton);

    virtual void setSkeleton(const Ptr<Assets::Skeleton> & pSkeleton);
    virtual const Assets::Skeleton & getSkeleton() const;

    virtual void setWorldMatrix(const Core::Matrix4f & matrix);
    virtual const Core::Matrix4f & getWorldMatrix() const;

    virtual void setPose(const Ptr<Assets::SkeletonPose> & pPose);
    virtual const Assets::SkeletonPose & getPose() const;

    virtual void kill();
    virtual bool isAlive() const;

    // Package

    void registerObserver(IPoseObserver * pObs);
    void unregisterObserver(IPoseObserver * pObs);

protected:
    void updatePose();

    Ptr<Assets::Skeleton>     _pSkeleton;
    Ptr<Assets::SkeletonPose> _pBasePose;
    Assets::SkeletonPose      _pose;
    Core::Matrix4f            _worldMatrix;
    Core::List<IPoseObserver*> _poseObservers;
    bool                        _isAlive;
};
//-----------------------------------------------------------------------------
class LM_API_RDR SkinMeshInstance : public ISkinMeshInstance, public IPoseObserver
{
public:
    SkinMeshInstance(const Ptr<ISkinMesh> & pMesh, const Ptr<ISkeletonInstance> & pSkeleton);
    ~SkinMeshInstance();

    virtual Ptr<ISkinMesh> getMesh() const;
    virtual void setMesh(const Ptr<ISkinMesh> & pMesh);

    virtual const Renderer::Material & getMaterial() const;
    virtual void setMaterial(const Renderer::Material & material);
    virtual Ptr<ISkeletonInstance> getSkeleton() const;

    virtual const Core::AABoxf & getBoundingBox() const;

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };

    virtual void onPoseChange(const Assets::SkeletonPose & pose);

    const Assets::SkeletonPose & getPose() const { return _pose; };

protected:
    void updateBoundingBox();

    Ptr<ISkinMesh>         _pMesh;
    Ptr<ISkeletonInstance> _pSkeleton;
    Assets::SkeletonPose   _pose; // pose with bind shape matrix
    mutable Core::AABoxf   _box;
    Renderer::Material     _material;
    bool                   _isAlive;
    bool                   _isVisible;
};
}
#endif /* MESH_H_ */
