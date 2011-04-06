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
#ifndef PACKING_KDTRITREE_H_
#define PACKING_KDTRITREE_H_

#include <Assets/VerticeSet.h>

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/GeometryComputations.h>
#include <Core/Math/GeometryIntersect.h>

namespace Packing
{
class LM_API_PAK KDTriTree : public Core::Object
{
public:
    KDTriTree();

    void clear();

    void add(const Core::Matrix4f & world, const Assets::VerticeSet & vset);

    void optimize(int32 itemPerNode = 50);

    bool isOccluded(const Core::Rayf & ray) const;

    struct Triangle
    {
        Core::Vector3f  p1;
        Core::Vector3f  p2;
        Core::Vector3f  p3;

        Core::AABoxf    box;
    };

protected:
    KDTriTree(const Core::AABoxf & box);

    void internalOptimize(Core::List<Triangle> & tris, int32 itemPerNode);
    //void internalContent(List<Ptr<IBoxable> > & items) const;
    //void internalBoxContent(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;
    //void internalBoxIntersection(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;

    Core::List<Triangle>    _tris;
    Core::AABoxf            _box;
    Ptr<KDTriTree>          _child1;
    Ptr<KDTriTree>          _child2;
};
}
#endif /* KDTREE_H_ */

