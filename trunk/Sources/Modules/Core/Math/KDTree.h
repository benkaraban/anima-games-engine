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

#ifndef CORE_KDTREE_H_
#define CORE_KDTREE_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/Math/GeometryComputations.h>

namespace Core
{
class LM_API_COR IBoxable
{
public:
    virtual ~IBoxable() {};
    virtual const Core::AABoxf & getBoundingBox() const = 0;
};

class LM_API_COR KDTree : public Object
{
public:
    KDTree();

    /**
     * Retourne l'intégralité du contenu de l'arbre.
     */
    void getContent(List<Ptr<IBoxable> > & items) const;

    /**
     * Retourne l'ensemble des éléments strictement inclus dans la box passée en argument.
     */
    void getBoxContent(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;

    /**
     * Retourne l'ensemble des éléments en contact avec la box passée en argument.
     */
    void getBoxIntersection(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;

    /**
     * Vide l'arbre.
     */
    void clear();

    void add(const Ptr<IBoxable> & pItem);
    void remove(const Ptr<IBoxable> & pItem);

    void optimize(int32 itemPerNode = 5);

    virtual String toString() const;

protected:
    KDTree(const AABoxf & box);

    void internalOptimize(List<Ptr<IBoxable> > & items, int32 itemPerNode);
    void internalContent(List<Ptr<IBoxable> > & items) const;
    void internalBoxContent(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;
    void internalBoxIntersection(const AABoxf & selection, List<Ptr<IBoxable> > & items) const;

    List<Ptr<IBoxable> > _content;
    AABoxf        _box;
    Ptr<KDTree>  _child1;
    Ptr<KDTree>  _child2;
};
}
#endif /* KDTREE_H_ */
