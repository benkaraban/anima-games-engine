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
#ifndef WORKFLOW_COLLADAMESHCVT_H_
#define WORKFLOW_COLLADAMESHCVT_H_

#include <Assets/ModelMesh.h>
#include <Core/Math/Transform.h>

#include <COLLADAFWAnimationCurve.h>
#include <COLLADAFWMesh.h>
#include <COLLADAFWSkinControllerData.h>
#include <COLLADAFWSkinController.h>

namespace Workflow
{
struct SkinData
{
    struct VertexData
    {
        int32   boneIndex[Assets::MAX_BONES_PER_VERTEX];
        float   boneWeight[Assets::MAX_BONES_PER_VERTEX];
    };

    Core::Matrix4f bindShapeMatrix;
    Core::List<VertexData> bonesInfluences;
    Core::List<Core::Matrix4f>  offsetMatrices;
};

Ptr<SkinData> convertSkin(const COLLADAFW::SkinControllerData * pSkinData, bool swapYZ, float scale);

struct MeshData
{
    Ptr<Assets::ModelMesh>  pMesh;
    Core::List<int32>       newToOldIndex;
};

Ptr<MeshData> convertMesh(const COLLADAFW::Mesh * pMesh, bool swapYZ, float scale);


struct SkinMeshBinding
{
    Ptr<Assets::ModelMesh> pMesh;
    Ptr<MeshData>          pMeshData;
    Ptr<SkinData>          pSkinData;
};

struct SkeletonData
{
    Ptr<Assets::ModelMesh>            pMesh;
    COLLADAFW::UniqueId               dataUID;
    COLLADAFW::UniqueId               meshUID;
    Core::List<COLLADAFW::UniqueId>   bonesUID;
};

Ptr<SkeletonData> convertSkeleton(const COLLADAFW::SkinController * pCtrl);

void applySkinToVertices(Assets::VerticeSet & vset, const MeshData & meshData, const SkinData & skinData);

struct AnimationData
{
    struct Key
    {
        float            time;
        Core::Transformf transform;
    };

    Core::List<Key> keys;
    float           duration;
    int32           startIndex;
};

Ptr<AnimationData> convertAnimData(const COLLADAFW::AnimationCurve * pAnimCurve, bool swapYZ, float scale);

}

#endif
