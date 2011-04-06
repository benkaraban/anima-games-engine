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
#include "KDTree.h"

namespace Core
{
struct CompareByX
{
    bool operator() (const Ptr<IBoxable> & p1, const Ptr<IBoxable> & p2) const
    { return p1->getBoundingBox().min._x < p2->getBoundingBox().min._x; };
};
//-----------------------------------------------------------------------------
struct CompareByY
{
    bool operator() (const Ptr<IBoxable> & p1, const Ptr<IBoxable> & p2) const
    { return p1->getBoundingBox().min._y < p2->getBoundingBox().min._y; };
};
//-----------------------------------------------------------------------------
struct CompareByZ
{
    bool operator() (const Ptr<IBoxable> & p1, const Ptr<IBoxable> & p2) const
    { return p1->getBoundingBox().min._z < p2->getBoundingBox().min._z; };
};
//-----------------------------------------------------------------------------
KDTree::KDTree()
{}
//-----------------------------------------------------------------------------
KDTree::KDTree(const AABoxf & box) : _box(box)
{}
//-----------------------------------------------------------------------------
void KDTree::getContent(List<Ptr<IBoxable> > & items) const
{
    items.clear();
    internalContent(items);
}
//-----------------------------------------------------------------------------
void KDTree::getBoxContent(const AABoxf & selection, List<Ptr<IBoxable> > & items) const
{
    items.clear();
    internalBoxContent(selection, items);
}
//-----------------------------------------------------------------------------
void KDTree::getBoxIntersection(const AABoxf & selection, List<Ptr<IBoxable> > & items) const
{
    items.clear();
    internalBoxIntersection(selection, items);
}
//-----------------------------------------------------------------------------
void KDTree::internalContent(List<Ptr<IBoxable> > & items) const
{
    items.insert(items.end(), _content.begin(), _content.end());

    if(_child1 != null) _child1->internalContent(items);
    if(_child2 != null) _child2->internalContent(items);
}
//-----------------------------------------------------------------------------
void KDTree::add(const Ptr<IBoxable> & pItem)
{
    const AABoxf & box = pItem->getBoundingBox();
    _box.setUnion(box);

    if(_child1 != null && _child1->_box.includes(box))
        _child1->add(pItem);
    else if(_child2 != null && _child2->_box.includes(box))
        _child2->add(pItem);
    else
        _content.push_back(pItem);
}
//-----------------------------------------------------------------------------
void KDTree::remove(const Ptr<IBoxable> & pItem)
{
    const AABoxf & box = pItem->getBoundingBox();
    if(_child1 != null && _child1->_box.includes(box))
        _child1->remove(pItem);
    else if(_child2 != null && _child2->_box.includes(box))
        _child2->remove(pItem);
    else
    {
        List<Ptr<IBoxable> >::iterator iItem = std::find(_content.begin(), _content.end(), pItem);
        if(iItem != _content.end())
            _content.erase(iItem);
    }
}
//-----------------------------------------------------------------------------
void KDTree::clear()
{
    _box.clear();
    _content.clear();
    _child1 = _child2 = null;
}
//-----------------------------------------------------------------------------
void KDTree::optimize(int32 itemPerNode)
{
    List<Ptr<IBoxable> > items;
    getContent(items);
    clear();

    for(List<Ptr<IBoxable> >::const_iterator iBoxable = items.begin(); iBoxable != items.end(); ++iBoxable)
        _box.setUnion((*iBoxable)->getBoundingBox());

    internalOptimize(items, itemPerNode);
}
//-----------------------------------------------------------------------------
void KDTree::internalBoxContent(const AABoxf & selection, List<Ptr<IBoxable> > & items) const
{
    if(selection.includes(_box))
    {
        internalContent(items);
    }
    else if(selection.inter(_box))
    {
        for(List<Ptr<IBoxable> >::const_iterator iItem = _content.begin(); iItem != _content.end(); ++iItem)
        {
            if(selection.includes((*iItem)->getBoundingBox()))
                items.push_back(*iItem);
        }

        if(!_child1.isNull()) _child1->internalBoxContent(selection, items);
        if(!_child2.isNull()) _child2->internalBoxContent(selection, items);
    }
}
//-----------------------------------------------------------------------------
void KDTree::internalBoxIntersection(const AABoxf & selection, List<Ptr<IBoxable> > & items) const
{
    if(selection.includes(_box))
    {
        internalContent(items);
    }
    else if(selection.inter(_box))
    {
        for(List<Ptr<IBoxable> >::const_iterator iItem = _content.begin(); iItem != _content.end(); ++iItem)
        {
            if(selection.inter((*iItem)->getBoundingBox()))
                items.push_back(*iItem);
        }

        if(!(_child1 == null)) _child1->internalBoxIntersection(selection, items);
        if(!(_child2 == null)) _child2->internalBoxIntersection(selection, items);
    }
}
//-----------------------------------------------------------------------------
void KDTree::internalOptimize(List<Ptr<IBoxable> > & items, int32 itemPerNode)
{
    if(items.size() > itemPerNode)
    {
        float w = _box.getWidth();
        float h = _box.getHeight();
        float d = _box.getDepth();

        AABoxf box1(_box);
        AABoxf box2(_box);

        // Divise la box en 2 suivant l'axe le plus avantageux. La division se fait
        // au niveau de la médiane (et pas du milieu) pour maximiser l'équilibre de l'arbre.

        if(w > h && w > d)
        {
            std::sort(items.begin(), items.end(), CompareByX());
            box1.max._x = box2.min._x = items[items.size() / 2]->getBoundingBox().min._x;
        }
        else if(h > d)
        {
            std::sort(items.begin(), items.end(), CompareByY());
            box1.max._y = box2.min._y = items[items.size() / 2]->getBoundingBox().min._y;
        }
        else
        {
            std::sort(items.begin(), items.end(), CompareByZ());
            box1.max._z = box2.min._z = items[items.size() / 2]->getBoundingBox().min._z;
        }

        // Répartit les items entre les fils ou le père si la box est à cheval entre les 2

        List<Ptr<IBoxable> > items1;
        List<Ptr<IBoxable> > items2;

        for(List<Ptr<IBoxable> >::const_iterator iBoxable = items.begin(); iBoxable != items.end(); ++iBoxable)
        {
            const AABoxf & box = (*iBoxable)->getBoundingBox();

            if(box.isInside(box1))
                items1.push_back(*iBoxable);
            else if(box.isInside(box2))
                items2.push_back(*iBoxable);
            else
                _content.push_back(*iBoxable);
        }

        if(!items1.empty() && !items2.empty())
        {
            _child1 = Ptr<KDTree>(new KDTree(box1));
            _child2 = Ptr<KDTree>(new KDTree(box2));

            _child1->internalOptimize(items1, itemPerNode);
            _child2->internalOptimize(items2, itemPerNode);
        }
        else
        {
            // Pas de quoi créer des enfants, on stocke tout dans le père
            _content.swap(items);
        }
    }
    else
    {
        _content.swap(items);
    }
}
//-----------------------------------------------------------------------------
String KDTree::toString() const
{
    String result(L"KDTree { content : ");
    result << L"[";
    for(int32 ii=0; ii < _content.size(); ii++)
    {
        result << Core::toString(_content[ii]->getBoundingBox());
        if(ii < _content.size() - 1)
            result << L", ";
    }
    result << L"]";
    if(!_child1.isNull()) result << L"child1 : " << _child1->toString();
    if(!_child2.isNull()) result << L"child2 : " << _child2->toString();
    result << L" }";
    return result;
}
//-----------------------------------------------------------------------------
}
