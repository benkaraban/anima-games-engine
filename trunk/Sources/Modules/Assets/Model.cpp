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

#include <Core/Math/MathIO.h>
#include "Model.h"

namespace Assets
{
//-----------------------------------------------------------------------------
static const int32 MODEL_MAGIC = LM_FOURCC('M', 'O', 'D', '0');
//-----------------------------------------------------------------------------
Model::Model() :
    _flags(0),
    _pSkeleton(new Skeleton())
{
    Ptr<ModelNode> pNode(new ModelNode(0, L"Root", Core::Matrix4f(Core::IDENTITY)));
    _nodes.push_back(pNode);
}
//-----------------------------------------------------------------------------
String Model::getUniqueNodeName(const String & name)
{
    if(_nodesLib.count(name) > 0)
    {
        String base(name.slice(0, name.find_last_not_of(L"0123456789")));
        int32 ii=0;
        String iName = base + Core::toString(ii, 2);
        while(_nodesLib.count(iName) > 0)
        {
            ii++;
            iName = base + Core::toString(ii, 2);
        }
        return iName;
    }
    else
    {
        return name;
    }
}
//-----------------------------------------------------------------------------
String Model::getUniqueMeshName(const String & name)
{
    if(_meshesLib.count(name) > 0)
    {
        String base(name.slice(0, name.find_last_not_of(L"0123456789")));
        int32 ii=0;
        String iName = base + Core::toString(ii, 2);
        while(_meshesLib.count(iName) > 0)
        {
            ii++;
            iName = base + Core::toString(ii, 2);
        }
        return iName;
    }
    else
    {
        return name;
    }
}
//-----------------------------------------------------------------------------
String Model::getUniqueAnimName(const String & name)
{
    if(_animsLib.count(name) > 0)
    {
        String base(name.slice(0, name.find_last_not_of(L"0123456789")));
        int32 ii=0;
        String iName = base + Core::toString(ii, 2);
        while(_animsLib.count(iName) > 0)
        {
            ii++;
            iName = base + Core::toString(ii, 2);
        }
        return iName;
    }
    else
    {
        return name;
    }
}
//-----------------------------------------------------------------------------
int32 Model::getNodeId(const String & name) const
{
    Core::Map<String, int32>::const_iterator iNode = _nodesLib.find(name);
    if(iNode == _nodesLib.end())
        return UNDEFINED_NODE_ID;
    else
        return iNode->second;
}
//-----------------------------------------------------------------------------
int32 Model::getMeshId(const String & name) const
{
    Core::Map<String, int32>::const_iterator iMesh = _meshesLib.find(name);
    if(iMesh == _meshesLib.end())
        return UNDEFINED_MESH_ID;
    else
        return iMesh->second;
}
//-----------------------------------------------------------------------------
int32 Model::addNode(const String & name, int32 parentId, const Core::Matrix4f & matrix)
{
    String baseName(name);
    if(name == String::EMPTY)
        baseName = L"Node";
    String uName(getUniqueNodeName(name));
    int32 id = _nodes.size();

    Ptr<ModelNode> pNode(new ModelNode(id, uName, matrix));
    _nodes.push_back(pNode);
    _nodesLib[uName] = id;
    _nodes[parentId]->addChild(pNode);
    return id;
}
//-----------------------------------------------------------------------------
int32 Model::addMeshNode(const String & name, int32 parentId, int32 meshId, const Core::Matrix4f & matrix)
{
    String baseName(name);
    if(name == String::EMPTY)
        baseName = L"Node";
    String uName(getUniqueNodeName(name));
    int32 id = _nodes.size();

    Ptr<ModelNode> pNode(new ModelNode(id, uName, matrix, meshId));
    _nodes.push_back(pNode);
    _nodesLib[uName] = id;
    _nodes[parentId]->addChild(pNode);
    return id;
}
//-----------------------------------------------------------------------------
int32 Model::addMesh(const String & name, const ModelMesh & mesh)
{
    String baseName(name);
    if(name == String::EMPTY)
        baseName = L"Mesh";
    String uName(getUniqueMeshName(name));
    int32 id = _meshes.size();

    Ptr<ModelMesh> pMesh(new ModelMesh(id, uName, mesh));
    _meshes.push_back(pMesh);
    _meshesLib[uName] = id;
    return id;
}
//-----------------------------------------------------------------------------
int32 Model::addAnimation(const String & name, const Animation & anim)
{
    String baseName(name);
    if(name == String::EMPTY)
        baseName = L"Mesh";
    String uName(getUniqueAnimName(name));
    int32 id = _anims.size();

    Ptr<Animation> pAnim(new Animation(anim));
    _anims.push_back(pAnim);
    _animsName.push_back(uName);
    _animsLib[uName] = id;
    return id;
}
//-----------------------------------------------------------------------------
void Model::read(Core::InputStream & input)
{
    _anims.clear();
    _meshes.clear();
    _nodes.clear();
    _animsName.clear();
    _animsLib.clear();
    _meshesLib.clear();
    _nodesLib.clear();

    int32 magic = input.readInt32();

    if(magic == MAGIC_TERRAIN)
    {
        magic = input.readInt32();
        magic = input.readInt32();
    }
        
    if(magic != MODEL_MAGIC)
        throw Core::IOException(L"Model bad magic");

    _flags = input.readInt32();

    int32 meshCount = input.readInt32();
    _meshes.resize(meshCount);
    for(int32 iMesh=0; iMesh < meshCount; iMesh++)
    {
        _meshes[iMesh] = Ptr<ModelMesh>(new ModelMesh(iMesh, String::EMPTY));
        _meshes[iMesh]->read(input);
        _meshesLib[_meshes[iMesh]->getName()] = iMesh;
    }


    int32 nodeCount = input.readInt32();
    _nodes.reserve(nodeCount);
    for(int32 iNode=0; iNode < nodeCount; iNode++)
    {
        String name = input.readString();
        int32 meshId = input.readInt32();
        Core::Matrix4f matrix;
        Core::read(input, matrix);

        Ptr<ModelNode> pNode(new ModelNode(iNode, name, matrix, meshId));
        _nodes.push_back(pNode);
        _nodesLib[pNode->getName()] = iNode;
    }

    for(Core::List<Ptr<ModelNode> >::iterator iNode= _nodes.begin(); iNode != _nodes.end(); ++iNode)
    {
        ModelNode & node = **iNode;
        int32 subNodeCount = input.readInt32();
        for(int32 ii=0; ii < subNodeCount; ii++)
        {
            int32 childId = input.readInt32();
            node.addChild(_nodes[childId]);
        }
    }

    _pSkeleton = Ptr<Skeleton>(new Skeleton());
    _pSkeleton->read(input);

    int32 animCount = input.readInt32();
    _anims.reserve(animCount);
    for(int32 ii=0; ii < animCount; ii++)
    {
        String name(input.readString());
        Animation anim;
        anim.read(input);
        addAnimation(name, anim);
    }
}
//-----------------------------------------------------------------------------
void Model::write(Core::OutputStream & output) const
{
    Core::write(output, MODEL_MAGIC);
    Core::write(output, _flags);

    Core::write(output, _meshes.size());
    for(Core::List<Ptr<ModelMesh> >::const_iterator iMesh= _meshes.begin(); iMesh != _meshes.end(); ++iMesh)
        Core::write(output, **iMesh);

    Core::write(output, _nodes.size());
    for(Core::List<Ptr<ModelNode> >::const_iterator iNode= _nodes.begin(); iNode != _nodes.end(); ++iNode)
    {
        const ModelNode & node = **iNode;
        Core::write(output, node.getName());
        Core::write(output, node.getMeshId());
        Core::write(output, node.getMatrix());
    }

    for(Core::List<Ptr<ModelNode> >::const_iterator iNode= _nodes.begin(); iNode != _nodes.end(); ++iNode)
    {
        const ModelNode & node = **iNode;
        Core::write(output, node.getSubNodeCount());
        for(int32 ii=0; ii < node.getSubNodeCount(); ii++)
            Core::write(output, node.getSubNode(ii).getId());
    }

    _pSkeleton->write(output);

    Core::write(output, _anims.size());
    for(int32 ii=0; ii < _anims.size(); ii++)
    {
        output.write(_animsName[ii]);
        _anims[ii]->write(output);
    }
}
//-----------------------------------------------------------------------------
void Model::getPose(ModelPose & pose, int32 animationId, float key) const
{
    LM_ASSERT(animationId < (int32)_anims.size());
    const Animation & anim = *_anims[animationId];

    pose.setNodeCount(anim.getNodeCount());
    Core::Matrix4f identity(Core::IDENTITY);
    _nodes[0]->getPose(pose, anim, key, identity);
}
//-----------------------------------------------------------------------------
void Model::getDefaultPose(ModelPose & pose) const
{
    pose.setNodeCount(_nodes.size());
    _nodes[0]->getDefaultPose(pose, Core::Matrix4f(Core::IDENTITY));
}
//-----------------------------------------------------------------------------
int32 Model::getAnimationId(const String & name) const
{
    int32 result = UNDEFINED_ANIM_ID;
    Core::Map<String, int32>::const_iterator iId = _animsLib.find(name);

    if(iId != _animsLib.end())
        result = iId->second;

    return result;
}
//-----------------------------------------------------------------------------
void Model::cleanup()
{
    for(int32 iMesh=0; iMesh < _meshes.size(); iMesh++)
        _meshes[iMesh]->cleanup();

    for(int32 iAnim=0; iAnim < _anims.size(); iAnim++)
    {
        LM_ASSERT(_anims[iAnim]->getNodeCount() == _nodes.size());

        for(int32 iNode=0; iNode < _nodes.size(); iNode++)
        {
            if(!_anims[iAnim]->hasKeyFrame(iNode))
                _anims[iAnim]->addKeyFrame(iNode, KeyFrame(0, _nodes[iNode]->getMatrix()));
        }
    }
}
//-----------------------------------------------------------------------------
void Model::mergeAnimations(const Assets::Model & source)
{
    Core::List<int32>   cvtNodeId;
    cvtNodeId.resize(source.getNodeCount());

    for(int32 iNode=0; iNode < source.getNodeCount(); iNode++)
        cvtNodeId[iNode] = getNodeId(source.getNode(iNode).getName());

    for(int32 iAnim=0; iAnim < source.getAnimationCount(); iAnim++)
    {
        const Assets::Animation & srcAnim = *source._anims[iAnim];
        Assets::Animation newAnim(srcAnim, getNodeCount(), cvtNodeId);
        addAnimation(source.getAnimationName(iAnim), newAnim);
    }

    cleanup();
}
//-----------------------------------------------------------------------------
void Model::deleteAnimation(int32 id)
{
    _animsLib.erase(_animsLib.find(_animsName[id]));
    _anims.erase(_anims.begin() + id);
    _animsName.erase(_animsName.begin() + id);
}
//-----------------------------------------------------------------------------
void Model::renameAnimation(int32 id, const String & newName)
{
    _animsLib.erase(_animsLib.find(_animsName[id]));
    _animsName[id] = L"";
    _animsName[id] = getUniqueAnimName(newName);
    _animsLib[_animsName[id]] = id;
}
//-----------------------------------------------------------------------------
void Model::replaceMeshPtr(int32 id, const Ptr<ModelMesh> & pMesh)
{
    *pMesh = *_meshes[id];
    _meshes[id] = pMesh;
}
//-----------------------------------------------------------------------------
}
