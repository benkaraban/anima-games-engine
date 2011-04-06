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
#include <Renderer/InstancesGroup.h>

namespace Renderer
{
InstancesGroup::InstancesGroup(const Ptr<IFreeForm> & pFreeForm, const Ptr<Assets::VerticeSet> & pVSet) : 
    _pFreeForm(pFreeForm),
    _pVSet(pVSet),
    _indices(pVSet->getIndices())
{
    _pFreeForm->setMax(65535, 100000);
    const Core::List<Assets::Vertex> vert = pVSet->getVertices();

    _vertices.resize(vert.size());

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        _vertices[ii].position = vert[ii].position;
        _vertices[ii].texcoords[0] = vert[ii].normale.x;
        _vertices[ii].texcoords[1] = vert[ii].normale.y;
        _vertices[ii].texcoords[2] = vert[ii].normale.z;
    }
}

void InstancesGroup::endUpdate()
{
    FreeFormData & data = _pFreeForm->beginUpdate();

    Core::List<FreeFormVertex>   & vertices = data.vertices;
    Core::List<uint16>           & indices = data.indices;

    vertices.resize(_instances.size() * _vertices.size());
    indices.resize(_instances.size() * _indices.size());

    int32 iVert = 0;
    int32 iInd = 0;

    for(int32 iInst=0; iInst < _instances.size(); iInst++)
    {
        const Instance & inst = _instances[iInst];
        Core::Matrix4f world(inst.worldMat);

        union { byte colorBytes[4]; uint32 color; };
        union { byte glowBytes[4]; uint32 glow; };

        Assets::packColor(inst.color, colorBytes);
        Assets::packColor(inst.glow, glowBytes);

        for(int32 ii=0; ii < _vertices.size(); ii++)
        {
            const FreeFormVertex & v1 = _vertices[ii];
            FreeFormVertex & v2 = vertices[iVert + ii];

            v2.position = world.apply(v1.position);

            Core::Vector3f normal = world.apply3x3(Core::Vector3f(v1.texcoords));
            v2.texcoords[0] = normal.x;
            v2.texcoords[1] = normal.y;
            v2.texcoords[2] = normal.z;
            v2.colors32 = color;
            v2.glow32 = glow;
        }

        for(int32 ii=0; ii < _indices.size(); ii++)
        {
            indices[iInd + ii] = _indices[ii] + iVert;
        }

        iVert += _vertices.size();
        iInd += _indices.size();
    }


    _pFreeForm->endUpdate();
}

}
