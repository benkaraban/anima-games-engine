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
#include <Universe/NodeGroup.h>
#include <Universe/Tools.h>

#include <Core/Math/GeometryIntersect.h>

namespace Universe
{
//-----------------------------------------------------------------------------
NodeGroup::NodeGroup(const Ptr<Renderer::IAABox> & pBox, World * pWorld) :
    Node(pBox, pWorld),
    _killWhenChildrenAreDead(false)
{
}
//-----------------------------------------------------------------------------
void NodeGroup::addChild(const Ptr<Node> & pNode, bool sameWorldPos, int32 indexPos)
{
    Node::addChild(pNode, sameWorldPos, indexPos);
    update(0.0);
}
//-----------------------------------------------------------------------------
void NodeGroup::removeChild(const Ptr<Node> & pNode, bool sameWorldPos)
{
    Node::removeChild(pNode);
    update(0.0);
}
//-----------------------------------------------------------------------------
void NodeGroup::prepareForPacking()
{
    if(_isInstance)
        instanciate();

    for(int32 ii=0; ii < _children.size(); ii++)
    {
        Node & node = *_children[ii];
        node.prepareForPacking();
    }

    internalPrepareForPacking();
}
//-----------------------------------------------------------------------------
void NodeGroup::internalPrepareForPacking()
{
}
//-----------------------------------------------------------------------------
void NodeGroup::update(double elapsed)
{
    Node::update(elapsed);
    updateBoundingBox();

    if(_killWhenChildrenAreDead)
    {
        bool allDead = true;
        Core::List<Ptr<Node> >::const_iterator iChild = _children.begin();
        while( allDead && iChild < _children.end())
        {
            if((*iChild)->isAlive())
                allDead = false;
            ++iChild;
        }

        if(allDead)
            kill();
    }
}
//-----------------------------------------------------------------------------
Core::AABoxf NodeGroup::internalGetBoundingBox() const
{
    Core::AABoxf box;

    for(int32 ii=0; ii < _children.size(); ii++)
    {
        Core::AABoxf childBox;
        Universe::getHierarchyBox(childBox, *_children[ii]);

        box.setUnion(childBox);
    }

    return box;
}
//-----------------------------------------------------------------------------
bool NodeGroup::internalIsIntersecting(const Core::Rayf& ray, float& distance)
{/*
    float distanceWithChild = FLT_MAX;
    distance = FLT_MAX;

    bool ret = false;
    for(int32 ii=0; ii < _children.size(); ii++)
    {
        bool isInter = _children[ii]->isIntersecting(ray, distanceWithChild);

        if (isInter==true)
        {
            ret = true;
            if (distanceWithChild<distance) distance = distanceWithChild;
        }
    }

    return ret;*/

    return false;
}
//-----------------------------------------------------------------------------
}
