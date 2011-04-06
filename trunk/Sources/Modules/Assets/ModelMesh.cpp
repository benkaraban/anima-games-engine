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
#include <Assets/SkeletonPose.h>
#include "ModelMesh.h"

namespace Assets
{
//-----------------------------------------------------------------------------
static const int32 MODEL_MESH_MAGIC = LM_FOURCC('M', 'E', 'S', '0');
//-----------------------------------------------------------------------------
void ModelMesh::copyLODs(const ModelMesh & mesh)
{
    _LODs.resize(mesh._LODs.size());
    for(int32 ii=0; ii < _LODs.size(); ii++)
    {
        _LODs[ii] = Ptr<VerticeSet>(new VerticeSet(*mesh._LODs[ii]));
    }
}
//-----------------------------------------------------------------------------
ModelMesh::ModelMesh()
    : _id(UNDEFINED_MESH_ID),
      _name(String::EMPTY),
      _isSkin(false),
      _flags(0)
{
    setLODCount(1);
}
//-----------------------------------------------------------------------------
ModelMesh::ModelMesh(const ModelMesh & mesh)
    : _id(mesh._id),
      _name(mesh._name),
      _isSkin(mesh._isSkin),
      _flags(mesh._flags),
      _material(mesh._material),
      _box(mesh._box),
      _bindShapeMatrix(Core::IDENTITY)
{
    copyLODs(mesh);
}
//-----------------------------------------------------------------------------
ModelMesh::ModelMesh(int32 id, const String & name, const ModelMesh & mesh)
    : _id(id),
      _name(name),
      _isSkin(mesh._isSkin),
      _flags(mesh._flags),
      _material(mesh._material),
      _box(mesh._box),
      _bindShapeMatrix(Core::IDENTITY)
{
    copyLODs(mesh);
}
//-----------------------------------------------------------------------------
ModelMesh::ModelMesh(int32 id, const String & name)
    : _id(id),
      _name(name),
      _isSkin(false),
      _flags(0),
      _bindShapeMatrix(Core::IDENTITY)
{
    setLODCount(1);
}
//-----------------------------------------------------------------------------
ModelMesh & ModelMesh::operator = (const ModelMesh & mesh)
{
    if(this != &mesh)
    {
        _name = mesh._name;
        _isSkin = mesh._isSkin;
        _flags = mesh._flags,
        _material = mesh._material;
        _box = mesh._box;
        _bindShapeMatrix = mesh._bindShapeMatrix;
        copyLODs(mesh);
    }
    return *this;
}
//-----------------------------------------------------------------------------
void ModelMesh::read(Core::InputStream & input)
{
    if(input.readInt32() != MODEL_MESH_MAGIC)
        throw Core::IOException(L"ModelMesh bad magic");

    int32 lodCount = 0;
    Core::read(input, _name);
    Core::read(input, _isSkin);
    Core::read(input, _flags);
    Core::read(input, _material);
    Core::read(input, lodCount);
    Core::read(input, _bindShapeMatrix);

    _LODs.resize(lodCount);

    for(int32 ii=0; ii < lodCount; ii++)
    {
        Ptr<VerticeSet> pVertices(new VerticeSet());
        pVertices->read(input);
        _LODs[ii] = pVertices;
    }

    cleanup();
}
//-----------------------------------------------------------------------------
void ModelMesh::write(Core::OutputStream & output) const
{
    int32 lodCount = _LODs.size();
    Core::write(output, MODEL_MESH_MAGIC);
    Core::write(output, _name);
    Core::write(output, _isSkin);
    Core::write(output, _flags);
    Core::write(output, _material);
    Core::write(output, lodCount);
    Core::write(output, _bindShapeMatrix);

    for(Core::List<Ptr<VerticeSet> >::const_iterator iLOD = _LODs.begin(); iLOD != _LODs.end(); ++iLOD)
        Core::write(output, **iLOD);
}
//-----------------------------------------------------------------------------
void ModelMesh::setLODCount(int32 lodCount)
{
    lodCount = std::min(EDetailLevel_COUNT, lodCount);
    int32 oldSize = _LODs.size();
    _LODs.resize(lodCount);
    for(int32 ii=oldSize; ii < lodCount; ii++)
    {
        _LODs[ii] = Ptr<VerticeSet>(new VerticeSet());
    }
}
//-----------------------------------------------------------------------------
bool ModelMesh::operator == (const ModelMesh & mesh) const
{
    bool result = (_name == mesh._name && _isSkin == mesh._isSkin && _flags == mesh._flags &&
                   _material == mesh._material && _LODs.size() == mesh._LODs.size() &&
                   _bindShapeMatrix == mesh._bindShapeMatrix);
    int32 ii=0;
    while(result && ii < _LODs.size())
    {
        result = result && (*_LODs[ii] == *mesh._LODs[ii]);
        ii ++;
    }
    return result;
}
//-----------------------------------------------------------------------------
bool ModelMesh::operator != (const ModelMesh & mesh) const
{
    return !(*this == mesh);
}
//-----------------------------------------------------------------------------
void ModelMesh::updateBoundingBox()
{
    _box.clear();
    const Core::List<Vertex> & vert = _LODs[0]->getVertices();

    for(Core::List<Vertex>::const_iterator iVert = vert.begin(); iVert != vert.end(); ++iVert)
        _box.addPoint(iVert->position);
}
//-----------------------------------------------------------------------------
void ModelMesh::cleanup()
{
    updateBoundingBox();

    for(int32 iLOD=0; iLOD < _LODs.size(); iLOD++)
        _LODs[iLOD]->cleanup();
}
//-----------------------------------------------------------------------------
}
