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
#ifndef ABSTRACT_IMESH_H_
#define ABSTRACT_IMESH_H_

#include <Core/Standard.h>
#include <Core/Math/Matrix.h>
#include <Core/Math/Vector.h>
#include <Core/Math/GeometryComputations.h>

#include <Assets/ModelMesh.h>
#include <Assets/Model.h>
#include <Assets/Skeleton.h>

#include <Renderer/Material.h>

namespace Renderer
{
/**
 * Cette classe représente un mesh utilisable pour le rendu. Il s'agit d'une ressource,
 * au même titre qu'une texture, et pas d'un objet visible. C'est la classe IMeshInstance
 * qui représente l'entité visible elle même.
 */
class LM_API_RDR IMesh : public Core::NonCopyable
{
public:
    virtual ~IMesh() {};
    virtual Core::AABoxf getBoundingBox() const = 0;

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const = 0;

    virtual const Renderer::Material & getMaterial() const = 0;
    virtual void setMaterial(const Renderer::Material & material) = 0;

    virtual const Core::List<int32> & usedBones() const = 0;

    virtual void copyMesh(const Ptr<IMesh> & pMesh) = 0;
};

class LM_API_RDR ISkinMesh : public Core::NonCopyable
{
public:
    virtual ~ISkinMesh() {};
    
    virtual Core::AABoxf getBoundingBox(const Assets::SkeletonPose & skelPose) const = 0;

    virtual Ptr<Assets::VerticeSet> getVerticeSet() const = 0;

    virtual const Renderer::Material & getMaterial() const = 0;
    virtual void setMaterial(const Renderer::Material & material) = 0;

    virtual const Core::Matrix4f & getBindShapeMatrix() const = 0;


    virtual const Core::List<int32> & usedBones() const = 0;

    virtual void copySkinMesh(const Ptr<ISkinMesh> & pMesh) = 0;
};

/**
 * Représente une entité 3D dont la forme est définie par un mesh.
 */
class LM_API_RDR IMeshInstance : public Core::NonCopyable
{
public:
    virtual ~IMeshInstance() {};

    virtual Ptr<IMesh> getMesh() const = 0;
    virtual void setMesh(const Ptr<IMesh> & pMesh) = 0;

    virtual const Renderer::Material & getMaterial() const = 0;
    virtual void setMaterial(const Renderer::Material & material) = 0;

    virtual const Core::Matrix4f & getWorldMatrix() const = 0;
    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;

    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};

/**
 * Cette classe représente une instance de squelette, sur laquelle on peut
 * appliquer une ou plusieurs instances de ISkinMeshInstance.
 */
class LM_API_RDR ISkeletonInstance : public Core::NonCopyable
{
public:
    virtual ~ISkeletonInstance() {};

    virtual void setSkeleton(const Ptr<Assets::Skeleton> & pSkeleton) = 0;
    virtual const Assets::Skeleton & getSkeleton() const = 0;

    virtual void setWorldMatrix(const Core::Matrix4f & matrix) = 0;
    virtual const Core::Matrix4f & getWorldMatrix() const = 0;

    virtual void setPose(const Ptr<Assets::SkeletonPose> & pPose) = 0;
    virtual const Assets::SkeletonPose & getPose() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};

/**
 * Représente un mesh animé par squelette. La classe n'offre pas de moyen
 * de modifier cette instance ou de la déplacer car la le ISkinMeshInstance
 * est créé pour un ISkeletonInstance et c'est cette instance de squelette
 * qui définit sa position et son animation.
 */
class LM_API_RDR ISkinMeshInstance : public Core::NonCopyable
{
public:
    virtual ~ISkinMeshInstance() {};

    virtual Ptr<ISkinMesh> getMesh() const = 0;
    virtual void setMesh(const Ptr<ISkinMesh> & pMesh) = 0;

    virtual const Renderer::Material & getMaterial() const = 0;
    virtual void setMaterial(const Renderer::Material & material) = 0;
    virtual Ptr<ISkeletonInstance> getSkeleton() const = 0;

    virtual const Core::AABoxf & getBoundingBox() const = 0;
    
    virtual void setVisible(bool flag) = 0;
    virtual bool isVisible() const = 0;

    virtual void kill() = 0;
    virtual bool isAlive() const = 0;
};

}
#endif /*ABSTRACT_IMESH_H_*/
