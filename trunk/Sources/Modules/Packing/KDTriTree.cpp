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

#include <algorithm>
#include "KDTriTree.h"

namespace Packing
{
//-----------------------------------------------------------------------------
KDTriTree::KDTriTree()
{
}
//-----------------------------------------------------------------------------
void KDTriTree::clear()
{
}
//-----------------------------------------------------------------------------
void KDTriTree::add(const Core::Matrix4f & world, const Assets::VerticeSet & vset)
{
    const Core::List<Assets::Vertex> & vert = vset.getVertices();
    const Core::List<uint16> & ind = vset.getIndices();

    Core::List<Core::Vector3f> pos;
    pos.resize(vert.size());

    for(int32 ii=0; ii < vert.size(); ii++)
    {
        pos[ii] = world.apply(vert[ii].position);
        _box.addPoint(pos[ii]);
    }

    for(int32 ii=0; ii < ind.size(); ii+=3)
    {
        Triangle tri;
        tri.p1 = pos[ind[ii + 0]];
        tri.p2 = pos[ind[ii + 1]];
        tri.p3 = pos[ind[ii + 2]];

        tri.box.addPoint(tri.p1);
        tri.box.addPoint(tri.p2);
        tri.box.addPoint(tri.p3);

        _tris.push_back(tri);
    }
}
//-----------------------------------------------------------------------------
void KDTriTree::optimize(int32 itemPerNode)
{
    Core::List<Triangle> tris;
    tris.swap(_tris);

    internalOptimize(tris, itemPerNode);
}
//-----------------------------------------------------------------------------
struct CompareByX
{
    bool operator() (const KDTriTree::Triangle & t1, const KDTriTree::Triangle & t2) const
    { return t1.box.min.x < t2.box.min.x; };
};
//-----------------------------------------------------------------------------
struct CompareByY
{
    bool operator() (const KDTriTree::Triangle & t1, const KDTriTree::Triangle & t2) const
    { return t1.box.min.y < t2.box.min.y; };
};
//-----------------------------------------------------------------------------
struct CompareByZ
{
    bool operator() (const KDTriTree::Triangle & t1, const KDTriTree::Triangle & t2) const
    { return t1.box.min.z < t2.box.min.z; };
};
//-----------------------------------------------------------------------------
KDTriTree::KDTriTree(const Core::AABoxf & box) : _box(box)
{}
//-----------------------------------------------------------------------------
bool KDTriTree::isOccluded(const Core::Rayf & ray) const
{
    Core::Vector3f p;
    float d = 0.0f;

    if(Core::intersect_RayAABox(ray, _box, d))
    {
        for(int32 ii=0; ii < _tris.size(); ii++)
        {
            const Triangle & tri = _tris[ii];
            if(Core::intersect_RayTriangle(ray, tri.p1, tri.p2, tri.p3, p) ||
                Core::intersect_RayTriangle(ray, tri.p1, tri.p3, tri.p2, p))
                return true;
        }

        if(_child1 != null && _child2 != null)
            return _child1->isOccluded(ray) || _child2->isOccluded(ray);

        return false;
    }
    else
    {
        return false;
    }
}
//-----------------------------------------------------------------------------
void KDTriTree::internalOptimize(Core::List<Triangle> & tris, int32 itemPerNode)
{
    if(tris.size() > itemPerNode)
    {
        float w = _box.getWidth();
        float h = _box.getHeight();
        float d = _box.getDepth();

        Core::AABoxf box1(_box);
        Core::AABoxf box2(_box);

        // Divise la box en 2 suivant l'axe le plus avantageux. La division se fait
        // au niveau de la médiane (et pas du milieu) pour maximiser l'équilibre de l'arbre.

        if(w > h && w > d)
        {
            std::sort(tris.begin(), tris.end(), CompareByX());
            box1.max._x = box2.min._x = tris[tris.size() / 2].box.min.x;
        }
        else if(h > d)
        {
            std::sort(tris.begin(), tris.end(), CompareByY());
            box1.max._y = box2.min._y = tris[tris.size() / 2].box.min.y;
        }
        else
        {
            std::sort(tris.begin(), tris.end(), CompareByZ());
            box1.max._z = box2.min._z = tris[tris.size() / 2].box.min.z;
        }

        // Répartit les items entre les fils ou le père si la box est à cheval entre les 2

        Core::List<Triangle> tri1;
        Core::List<Triangle> tri2;

        for(Core::List<Triangle>::const_iterator iTri = tris.begin(); iTri != tris.end(); ++iTri)
        {
            const Core::AABoxf & box = iTri->box;

            if(box.isInside(box1))
                tri1.push_back(*iTri);
            else if(box.isInside(box2))
                tri2.push_back(*iTri);
            else
                _tris.push_back(*iTri);
        }

        if(!tri1.empty() && !tri2.empty())
        {
            _child1 = Ptr<KDTriTree>(new KDTriTree(box1));
            _child2 = Ptr<KDTriTree>(new KDTriTree(box2));

            _child1->internalOptimize(tri1, itemPerNode);
            _child2->internalOptimize(tri2, itemPerNode);
        }
        else
        {
            // Pas de quoi créer des enfants, on stocke tout dans le père
            _tris.swap(tris);
        }
    }
    else
    {
        _tris.swap(tris);
    }
}
//-----------------------------------------------------------------------------
}
