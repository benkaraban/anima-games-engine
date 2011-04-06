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
#include <Assets/SkeletonPose.h>

namespace Assets
{

SkeletonPose::SkeletonPose()
{
}

SkeletonPose::SkeletonPose(const SkeletonPose & pose, const Core::Matrix4f & mat)
{
    setTransform(pose, mat);
}

void SkeletonPose::setTransform(const SkeletonPose & pose, const Core::Matrix4f & mat)
{
    _bonesMat.resize(pose._bonesMat.size());
    for(int32 ii=0; ii < _bonesMat.size(); ii++)
        _bonesMat[ii] = mat * pose._bonesMat[ii];
}

void SkeletonPose::setBindShapeMatrix(const SkeletonPose & pose, const Core::Matrix4f & mat)
{
    _bonesMat.resize(pose._bonesMat.size());
    for(int32 ii=0; ii < _bonesMat.size(); ii++)
        _bonesMat[ii] = pose._bonesMat[ii] * mat;
}

void SkeletonPose::transformPosition(const VerticeSet & src, VerticeSet & dst) const
{
    dst = src;

    Core::List<Vertex> & vert = dst.getVertices();
    const Core::List<BlendingGroup> & groups = dst.getBlendingGroups();

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        const BlendingGroup & group = groups[vert[ii].blendingGroup];
        const Core::List<int32> & palette = group.getMatrixPalette();

        int32 boneId = vert[ii].boneId[0];
        vert[ii].position = _bonesMat[palette[boneId]].apply(vert[ii].position);
    }
}

}
