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
#include <Renderer/common/Mesh.h>

#include <algorithm>

namespace Renderer
{
//-----------------------------------------------------------------------------
Mesh::Mesh(const Ptr<Assets::VerticeSet> & pSourceVerts, const Ptr<IPackedStaticVertices> & pPackedVerts, const Material & material) :
    _pSourceVerts(pSourceVerts),
    _pPackedVerts(pPackedVerts),
    _material(material)
{
    const Core::List<Assets::Vertex> & vert = pSourceVerts->getVertices();

    // Bounding box

    _box.clear();

    for(int32 iVert=0; iVert < vert.size(); iVert++)
        _box.addPoint(vert[iVert].position);


    // Used bones

    int32 maxBoneId = -1;

    for(int32 iVert=0; iVert < vert.size(); iVert++)
        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
            if(vert[iVert].boneWeight[iBone] > 0.0)
                maxBoneId = std::max(maxBoneId, vert[iVert].boneId[iBone]);

    Core::List<bool> isBoneUsed;
    isBoneUsed.resize(maxBoneId + 1, false);

    for(int32 iVert=0; iVert < vert.size(); iVert++)
        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
            if(vert[iVert].boneWeight[iBone] > 0.0)
                isBoneUsed[vert[iVert].boneId[iBone]] = true;

    for(int32 iBone=0; iBone < isBoneUsed.size(); iBone++)
        if(isBoneUsed[iBone])
            _usedBones.push_back(iBone);
}
//-----------------------------------------------------------------------------
void Mesh::copyMesh(const Ptr<IMesh> & pMesh)
{
    Ptr<Mesh> pMeshCopy = LM_DEBUG_PTR_CAST<Mesh>(pMesh);

    _pSourceVerts = pMeshCopy->_pSourceVerts;
    _pPackedVerts = pMeshCopy->_pPackedVerts;
    _material = pMeshCopy->_material;
    _usedBones = pMeshCopy->_usedBones;
    _box = pMeshCopy->_box;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SkinMesh::SkinMesh(const Ptr<Assets::VerticeSet> & pSourceVerts, const Ptr<IPackedSkinnedVertices> & pPackedVerts, const Material & material) :
    _pSourceVerts(pSourceVerts),
    _pPackedVerts(pPackedVerts),
    _material(material),
    _invertedCulling(material.invertedCulling)
{
    const Core::List<Assets::BlendingGroup> & groups = pSourceVerts->getBlendingGroups();
    const Core::List<Assets::Vertex> & vert = pSourceVerts->getVertices();

    // Used bones

    int32 maxBoneId = -1;

    for(int32 iVert=0; iVert < vert.size(); iVert++)
    {
        const Core::List<int32> & palette = groups[vert[iVert].blendingGroup].getMatrixPalette();

        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
        {
            if(vert[iVert].boneWeight[iBone] > 0.0)
            {
                maxBoneId = std::max(maxBoneId, palette[vert[iVert].boneId[iBone]]);
            }
        }
    }

    Core::List<bool> isBoneUsed;
    isBoneUsed.resize(maxBoneId + 1, false);

    for(int32 iVert=0; iVert < vert.size(); iVert++)
    {
        const Core::List<int32> & palette = groups[vert[iVert].blendingGroup].getMatrixPalette();

        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
        {
            if(vert[iVert].boneWeight[iBone] > 0.0)
                isBoneUsed[palette[vert[iVert].boneId[iBone]]] = true;
        }
    }

    for(int32 iBone=0; iBone < isBoneUsed.size(); iBone++)
        if(isBoneUsed[iBone])
            _usedBones.push_back(iBone);

    // Bounding box

    _boxes.clear();
    _boxes.resize(maxBoneId + 1);

    for(int32 iVert=0; iVert < vert.size(); iVert++)
    {
        const Core::List<int32> & palette = groups[vert[iVert].blendingGroup].getMatrixPalette();

        for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
        {
            if(vert[iVert].boneWeight[iBone] > 0.0f)
            {
                int32 boneId = palette[vert[iVert].boneId[iBone]];
                _boxes[boneId].addPoint(vert[iVert].position);
            }
        }
    }
}
//-----------------------------------------------------------------------------
Core::AABoxf SkinMesh::getBoundingBox(const Assets::SkeletonPose & skelPose) const
{
    Core::AABoxf result;
    int32 boneCount = std::min(skelPose.getBoneCount(), _boxes.size());

    for(int32 iBone=0; iBone < boneCount; iBone++)
    {
        Core::AABoxf boneBox(_boxes[iBone], skelPose.getBoneMatrix(iBone));
        result.setUnion(boneBox);
    }

    return result;
}
//-----------------------------------------------------------------------------
void SkinMesh::setBindShapeMatrix(const Core::Matrix4f & bindShapeMatrix)
{ 
    _bindShapeMatrix = bindShapeMatrix; 
    Core::Transformf trans(_bindShapeMatrix);
    _invertedCulling = (trans._scale.x < 0.0f || trans._scale.y < 0.0f || trans._scale.z < 0.0f);
    _material.invertedCulling = _invertedCulling;
}
//-----------------------------------------------------------------------------
void SkinMesh::copySkinMesh(const Ptr<ISkinMesh> & pMesh)
{
    Ptr<SkinMesh> pMeshCopy = LM_DEBUG_PTR_CAST<SkinMesh>(pMesh);

    _pSourceVerts = pMeshCopy->_pSourceVerts;
    _pPackedVerts = pMeshCopy->_pPackedVerts;
    _material = pMeshCopy->_material;
    _usedBones = pMeshCopy->_usedBones;
    _boxes = pMeshCopy->_boxes;
    _bindShapeMatrix = pMeshCopy->_bindShapeMatrix;
    _invertedCulling = pMeshCopy->_invertedCulling;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
MeshInstance::MeshInstance(const Ptr<IMesh> & pMesh) :
    _pMesh(pMesh),
    _material(pMesh->getMaterial()),
    _worldMatrix(Core::IDENTITY),
    _isAlive(true),
    _isVisible(true)
{
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
Ptr<IMesh> MeshInstance::getMesh() const
{
    return _pMesh;
}
//-----------------------------------------------------------------------------
void MeshInstance::setMesh(const Ptr<IMesh> & pMesh)
{
    _pMesh = pMesh;
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
const Renderer::Material & MeshInstance::getMaterial() const
{
    return _material;
}
//-----------------------------------------------------------------------------
void MeshInstance::setMaterial(const Renderer::Material & material)
{
    _material = material;
}
//-----------------------------------------------------------------------------
const Core::Matrix4f & MeshInstance::getWorldMatrix() const
{
    return _worldMatrix;
}
//-----------------------------------------------------------------------------
void MeshInstance::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void MeshInstance::updateBoundingBox()
{
    _box.set(_pMesh->getBoundingBox(), _worldMatrix);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SkeletonInstance::SkeletonInstance(const Ptr<Assets::Skeleton> & pSkeleton) :
    _pSkeleton(pSkeleton),
    _worldMatrix(Core::IDENTITY),
    _isAlive(true)
{
}
//-----------------------------------------------------------------------------
void SkeletonInstance::setSkeleton(const Ptr<Assets::Skeleton> & pSkeleton)
{
    _pSkeleton = pSkeleton;
}
//-----------------------------------------------------------------------------
const Assets::Skeleton & SkeletonInstance::getSkeleton() const
{
    return *_pSkeleton;
}
//-----------------------------------------------------------------------------
void SkeletonInstance::setWorldMatrix(const Core::Matrix4f & matrix)
{
    _worldMatrix = matrix;
    updatePose();
}
//-----------------------------------------------------------------------------
const Core::Matrix4f & SkeletonInstance::getWorldMatrix() const
{
    return _worldMatrix;
}
//-----------------------------------------------------------------------------
void SkeletonInstance::updatePose()
{
    LM_ASSERT(_pBasePose != null);
    _pose.setTransform(*_pBasePose, _worldMatrix);

    for(Core::List<IPoseObserver*>::const_iterator ipObs = _poseObservers.begin(); ipObs != _poseObservers.end(); ++ipObs)
        (*ipObs)->onPoseChange(_pose);
}
//-----------------------------------------------------------------------------
void SkeletonInstance::setPose(const Ptr<Assets::SkeletonPose> & pPose)
{
    _pBasePose = pPose;
    updatePose();
}
//-----------------------------------------------------------------------------
const Assets::SkeletonPose & SkeletonInstance::getPose() const
{
    return _pose;
}
//-----------------------------------------------------------------------------
void SkeletonInstance::registerObserver(IPoseObserver * pObs)
{
    _poseObservers.push_back(pObs);
}
//-----------------------------------------------------------------------------
void SkeletonInstance::unregisterObserver(IPoseObserver * pObs)
{
    Core::List<IPoseObserver*>::iterator iObs = std::find(_poseObservers.begin(), _poseObservers.end(), pObs);
    if(iObs != _poseObservers.end())
        _poseObservers.erase(iObs);
}
//-----------------------------------------------------------------------------
void SkeletonInstance::kill()
{
    _isAlive = false;
}
//-----------------------------------------------------------------------------
bool SkeletonInstance::isAlive() const
{
    return _isAlive;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
SkinMeshInstance::SkinMeshInstance(const Ptr<ISkinMesh> & pMesh, const Ptr<ISkeletonInstance> & pSkeleton) :
    _pMesh(pMesh),
    _pSkeleton(pSkeleton),
    _isAlive(true),
    _isVisible(true)
{
    (LM_DEBUG_PTR_CAST<SkeletonInstance>(_pSkeleton))->registerObserver(this);
}
//-----------------------------------------------------------------------------
SkinMeshInstance::~SkinMeshInstance()
{
    (LM_DEBUG_PTR_CAST<SkeletonInstance>(_pSkeleton))->unregisterObserver(this);
}
//-----------------------------------------------------------------------------
Ptr<ISkinMesh> SkinMeshInstance::getMesh() const
{
    return _pMesh;
}
//-----------------------------------------------------------------------------
void SkinMeshInstance::setMesh(const Ptr<ISkinMesh> & pMesh)
{
    _pMesh = pMesh;
    onPoseChange(_pSkeleton->getPose());
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
const Renderer::Material & SkinMeshInstance::getMaterial() const
{
    return _material;
}
//-----------------------------------------------------------------------------
void SkinMeshInstance::setMaterial(const Renderer::Material & material)
{
    _material = material;
    _material.invertedCulling = _pMesh->getMaterial().invertedCulling;
}
//-----------------------------------------------------------------------------
Ptr<ISkeletonInstance> SkinMeshInstance::getSkeleton() const
{
    return _pSkeleton;
}
//-----------------------------------------------------------------------------
void SkinMeshInstance::onPoseChange(const Assets::SkeletonPose & pose)
{
    _pose.setBindShapeMatrix(pose, _pMesh->getBindShapeMatrix());
    updateBoundingBox();
}
//-----------------------------------------------------------------------------
void SkinMeshInstance::updateBoundingBox()
{
    _box = _pMesh->getBoundingBox(_pose);
}
//-----------------------------------------------------------------------------
const Core::AABoxf & SkinMeshInstance::getBoundingBox() const
{
    return _box;
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
}
