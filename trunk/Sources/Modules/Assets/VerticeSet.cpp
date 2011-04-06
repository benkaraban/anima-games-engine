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
#include "VerticeSet.h"

namespace Assets
{
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
BlendingGroup::BlendingGroup() :
    _startIndice(0),
    _indiceCount(0)
{}
//-----------------------------------------------------------------------------
BlendingGroup::BlendingGroup(int32 startIndice, int32 indiceCount, Core::List<int32> & palette) :
    _startIndice(startIndice),
    _indiceCount(indiceCount),
    _palette(palette)
{}
//-----------------------------------------------------------------------------
BlendingGroup::BlendingGroup(const BlendingGroup & group) :
    _startIndice(group._startIndice),
    _indiceCount(group._indiceCount),
    _palette(group._palette)
{}
//-----------------------------------------------------------------------------
void BlendingGroup::read(Core::InputStream & input)
{
    Core::read(input, _startIndice);
    Core::read(input, _indiceCount);
    Core::read(input, _palette);
}
//-----------------------------------------------------------------------------
void BlendingGroup::write(Core::OutputStream & output) const
{
    Core::write(output, _startIndice);
    Core::write(output, _indiceCount);
    Core::write(output, _palette);
}
//-----------------------------------------------------------------------------
bool BlendingGroup::operator == (const BlendingGroup & group) const
{
    return _startIndice == group._startIndice && _indiceCount == group._indiceCount
        && _palette == group._palette;
}
//-----------------------------------------------------------------------------
bool BlendingGroup::operator != (const BlendingGroup & group) const
{
    return !(*this == group);
}
//-----------------------------------------------------------------------------
//*****************************************************************************
//-----------------------------------------------------------------------------
VerticeSet::VerticeSet()
{}
//-----------------------------------------------------------------------------
void VerticeSet::read(Core::InputStream & input)
{
    _indices.clear();
    _vertices.clear();

    Core::read(input, _vertices);
    Core::read(input, _indices);
    Core::read(input, _blendGroups);
}
//-----------------------------------------------------------------------------
void VerticeSet::write(Core::OutputStream & output) const
{
    Core::write(output, _vertices);
    Core::write(output, _indices);
    Core::write(output, _blendGroups);
}
//-----------------------------------------------------------------------------
bool VerticeSet::operator == (const VerticeSet & set) const
{
    return _vertices == set._vertices && _indices == set._indices && _blendGroups == set._blendGroups;
}
//-----------------------------------------------------------------------------
bool VerticeSet::operator != (const VerticeSet & set) const
{
    return !(*this == set);
}
//-----------------------------------------------------------------------------
void VerticeSet::cleanup()
{
    if(_blendGroups.empty())
    {
        int32 maxBone = 0;

        for(int32 ii=0; ii < _vertices.size(); ii++)
            for(int32 iBone=0; iBone < Assets::MAX_BONES_PER_VERTEX; iBone++)
                maxBone = std::max(maxBone, _vertices[ii].boneId[iBone]);

        Core::List<int32> palette;
        palette.resize(maxBone + 1);

        for(int32 ii=0; ii < palette.size(); ii++)
            palette[ii] = ii;

        _blendGroups.push_back(BlendingGroup(0, _indices.size(), palette));
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_ASS bool intersect_RayVerticeSet(const Core::Rayf& ray, const VerticeSet& vset, Core::Vector3f&  posIntersec)
{
    const Core::List<uint16> &          indices = vset.getIndices();
    const Core::List<Vertex> & vertices = vset.getVertices();

    bool intersec = false;

    for(int32 iInd=0; iInd<indices.size(); iInd+=3)
    {
        const Assets::Vertex& p1 = vertices[indices[iInd + 0]];
        const Assets::Vertex& p2 = vertices[indices[iInd + 1]];
        const Assets::Vertex& p3 = vertices[indices[iInd + 2]];

        intersec = intersect_RayTriangle(ray, p1.position, p2.position, p3.position, posIntersec)
                || intersect_RayTriangle(ray, p3.position, p2.position, p1.position, posIntersec);

        if(intersec)
        {
            break;
        }
    }

    return intersec;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}
