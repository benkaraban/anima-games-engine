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

#include <Assets/Animation.h>
#include "ModelNode.h"

namespace Assets
{
//-----------------------------------------------------------------------------
ModelNode::ModelNode(int32 id, const String & name, const Core::Matrix4f & matrix, int32 meshId)
    : _id(id),
      _name(name),
      _meshId(meshId),
      _matrix(matrix)
{}
//-----------------------------------------------------------------------------
bool ModelNode::operator == (const ModelNode & node) const
{
    return _id == node._id && _name == node._name && _meshId == node._meshId;
}
//-----------------------------------------------------------------------------
bool ModelNode::operator != (const ModelNode & node) const
{
    return !(*this == node);
}
//-----------------------------------------------------------------------------
void ModelNode::getPose(ModelPose & pose, const Animation & anim, float key, const Core::Matrix4f & parentMatrix) const
{
    Core::Matrix4f localeMatrix;
    anim.getNodeMatrix(localeMatrix, _id, key);

    Core::Matrix4f worldMatrix(parentMatrix * localeMatrix);
    pose.setNodeMatrix(_id, worldMatrix);

    for(Core::List<Ptr<ModelNode> >::const_iterator iChild = _children.begin(); iChild != _children.end(); ++iChild)
        (*iChild)->getPose(pose, anim, key, worldMatrix);
}
//-----------------------------------------------------------------------------
void ModelNode::getDefaultPose(ModelPose & pose, const Core::Matrix4f & parentMatrix) const
{
    Core::Matrix4f worldMatrix(parentMatrix * _matrix);
    pose.setNodeMatrix(_id, worldMatrix);

    for(Core::List<Ptr<ModelNode> >::const_iterator iChild = _children.begin(); iChild != _children.end(); ++iChild)
        (*iChild)->getDefaultPose(pose, worldMatrix);
}
//-----------------------------------------------------------------------------
}
