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
#ifndef RENDERER_MESHSTUB_H_
#define RENDERER_MESHSTUB_H_

#include <Assets/SkeletonPose.h>
#include <Renderer/IMesh.h>

namespace Renderer
{
class LM_API_RDS MeshStub : public Core::Object, public IMesh
{
public:
    MeshStub(const Ptr<Assets::ModelMesh> & pSourceMesh) :
        _pSourceMesh(pSourceMesh)
    {};

    virtual Core::AABoxf getBoundingBox() const { return Core::AABoxf(); };

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const { return _pSourceMesh->getLOD(Assets::LOD_HIGH); };

    virtual const Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    virtual const Core::List<int32> & usedBones() const { return _usedBones; };

    virtual void copyMesh(const Ptr<IMesh> & pMesh) {};

    Ptr<Assets::ModelMesh> _pSourceMesh;
    Material               _material;
    Core::List<int32>      _usedBones;
};

class LM_API_RDS SkinMeshStub : public Core::Object, public ISkinMesh
{
public:
    SkinMeshStub(const Ptr<Assets::ModelMesh> & pSourceMesh) :
        _pSourceMesh(pSourceMesh)
    {};

    virtual Core::AABoxf getBoundingBox(const Assets::SkeletonPose & skelPose) const { return Core::AABoxf(); };

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const { return _pSourceMesh->getLOD(Assets::LOD_HIGH); };

    virtual const Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    virtual const Core::Matrix4f & getBindShapeMatrix() const { return _bindMat; };

    virtual const Core::List<int32> & usedBones() const { return _usedBones; };

    virtual void copySkinMesh(const Ptr<ISkinMesh> & pMesh) {};

    Ptr<Assets::ModelMesh> _pSourceMesh;
    Material               _material;
    Core::List<int32>      _usedBones;
    Core::Matrix4f         _bindMat;
};

class LM_API_RDS MeshInstanceStub : public Core::Object, public IMeshInstance
{
public:
    MeshInstanceStub(const Ptr<IMesh> & pMesh) :
        _isAlive(true), _isVisible(true), _pMesh(pMesh), _matrix(Core::IDENTITY)
    {};

    virtual Ptr<IMesh> getMesh() const { return _pMesh; };
    virtual void setMesh(const Ptr<IMesh> & pMesh) { _pMesh = pMesh; };

    virtual const Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    virtual const Core::Matrix4f & getWorldMatrix() const { return _matrix; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) { _matrix = matrix; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };


    Core::AABoxf      _box;
    bool              _isAlive;
    bool              _isVisible;
    Ptr<IMesh>        _pMesh;
    Core::Matrix4f    _matrix;
    Material          _material;
};

class LM_API_RDS SkeletonInstanceStub : public Core::Object, public ISkeletonInstance
{
public:
    SkeletonInstanceStub(const Ptr<Assets::Skeleton> & pSkeleton) :
        _isAlive(true), _pSkeleton(pSkeleton), _matrix(Core::IDENTITY)
    {};

    virtual void setSkeleton(const Ptr<Assets::Skeleton> & pSkeleton) { _pSkeleton = pSkeleton; };
    virtual const Assets::Skeleton & getSkeleton() const { return *_pSkeleton; };

    virtual const Core::Matrix4f & getWorldMatrix() const { return _matrix; };
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) { _matrix = matrix; };

    virtual void setPose(const Ptr<Assets::SkeletonPose> & pPose) { _pPose = pPose; };
    virtual const Assets::SkeletonPose & getPose() const { return *_pPose; };

    virtual void kill() {};
    virtual bool isAlive() const { return true; };

    bool                      _isAlive;
    Ptr<Assets::Skeleton>     _pSkeleton;
    Ptr<Assets::SkeletonPose> _pPose;
    Core::Matrix4f            _matrix;
    Material          _material;
};

class LM_API_RDS SkinMeshInstanceStub : public Core::Object, public ISkinMeshInstance
{
public:
     SkinMeshInstanceStub(const Ptr<ISkinMesh> & pMesh, const Ptr<ISkeletonInstance> & pSkeleton) :
        _isAlive(true), _isVisible(true), _pMesh(pMesh), _pSkeleton(pSkeleton), _matrix(Core::IDENTITY)
    {};

    virtual Ptr<ISkinMesh> getMesh() const { return _pMesh; };
    virtual void setMesh(const Ptr<ISkinMesh> & pMesh) { _pMesh = pMesh; };

    virtual const Material & getMaterial() const { return _material; };
    virtual void setMaterial(const Renderer::Material & material) { _material = material; };

    virtual Ptr<ISkeletonInstance> getSkeleton() const { return _pSkeleton; };

    virtual const Core::AABoxf & getBoundingBox() const { return _box; };

    virtual void setVisible(bool flag) { _isVisible = flag; };
    virtual bool isVisible() const { return _isVisible; };

    virtual void kill() { _isAlive = false; };
    virtual bool isAlive() const { return _isAlive; };


    Core::AABoxf           _box;
    bool                   _isAlive;
    bool                   _isVisible;
    Ptr<ISkinMesh>         _pMesh;
    Ptr<ISkeletonInstance> _pSkeleton;
    Core::Matrix4f         _matrix;
    Material               _material;
};

}
#endif /*ABSTRACT_IMESH_H_*/
