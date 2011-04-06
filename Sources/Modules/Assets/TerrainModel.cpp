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
#include <Assets/TerrainModel.h>

namespace Assets
{
//-----------------------------------------------------------------------------
TerrainModel::TerrainModel() :
    _terFlags(0)
{
}
//-----------------------------------------------------------------------------
void TerrainModel::read(Core::InputStream & input)
{
    int32 magic = input.readInt32();
    if(magic != MAGIC_TERRAIN)
        throw Core::Exception(L"Bad magic for terrain.");

    _terFlags = input.readInt32();

    Model::read(input);

    _nodeIdByLOD.resize(input.readInt32());

    for(int32 iLOD=0; iLOD < _nodeIdByLOD.size(); iLOD++)
    {
        _nodeIdByLOD[iLOD].resize(input.readInt32());

        for(int32 ii=0; ii < _nodeIdByLOD[iLOD].size(); ii++)
            _nodeIdByLOD[iLOD][ii] = input.readInt32();
    }
}
//-----------------------------------------------------------------------------
void TerrainModel::write(Core::OutputStream & output) const
{
    output.write(MAGIC_TERRAIN);
    output.write(_flags);

    Model::write(output);

    output.write(_nodeIdByLOD.size());

    for(int32 iLOD=0; iLOD < _nodeIdByLOD.size(); iLOD++)
    {
        output.write(_nodeIdByLOD[iLOD].size());

        for(int32 ii=0; ii < _nodeIdByLOD[iLOD].size(); ii++)
            output.write(_nodeIdByLOD[iLOD][ii]);
    }
}
//-----------------------------------------------------------------------------
int32 TerrainModel::getLODCount() const
{
    return _nodeIdByLOD.size();
}
//-----------------------------------------------------------------------------
int32 TerrainModel::getNodeCountForLOD(int32 LOD) const
{
    return _nodeIdByLOD[LOD].size();
}
//-----------------------------------------------------------------------------
Ptr<ModelNode> TerrainModel::getNodeByLOD(int32 LOD, int32 id) const
{
    return _nodes[_nodeIdByLOD[LOD][id]];
}
//-----------------------------------------------------------------------------
void TerrainModel::setLODCount(int32 LOD)
{
    _nodeIdByLOD.clear();
    _nodeIdByLOD.resize(LOD);
}
//-----------------------------------------------------------------------------
void TerrainModel::addNodeRef(int32 LOD, int32 nodeId)
{
    _nodeIdByLOD[LOD].push_back(nodeId);
}
//-----------------------------------------------------------------------------
}
