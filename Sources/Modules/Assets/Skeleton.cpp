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

#include <Core/IO/Tools.h>
#include <Assets/ModelPose.h>
#include <Assets/SkeletonPose.h>
#include "Skeleton.h"

namespace Assets
{
//-----------------------------------------------------------------------------
void Skeleton::getSkeletonPose(SkeletonPose & skeletonPose, const ModelPose & modelPose) const
{
    skeletonPose.setBoneCount(_bones.size());
    for(int32 ii=0; ii < _bones.size(); ii++)
    {
        Core::Matrix4f nodeMat(modelPose.getNodeMatrix(_bones[ii].nodeId));
        Core::Matrix4f boneMat(nodeMat * _bones[ii].offsetMatrix);
        skeletonPose.setBoneMatrix(ii, boneMat);
    }
}
//-----------------------------------------------------------------------------
int32 Skeleton::addBone(int32 nodeId, const Core::Matrix4f & offsetMatrix)
{
    int32 boneId = _bones.size();
    _bones.push_back(Bone(nodeId, offsetMatrix));
    return boneId;
}
//-----------------------------------------------------------------------------
void Skeleton::read(Core::InputStream & input)
{
    Core::read(input, _bones);
}
//-----------------------------------------------------------------------------
void Skeleton::write(Core::OutputStream & output) const
{
    Core::write(output, _bones);
}
//-----------------------------------------------------------------------------
}
