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
#include <Universe/Tools.h>
#include <Core/Math/Splines/Trajectory.h>

namespace Universe
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void internalGetHierarchyBox(Core::AABoxf & box, const Node & node)
{/*
    box.setUnion(node.getBoundingBox());

    for(int32 ii=0; ii < node.getChildCount(); ii++)
        internalGetHierarchyBox(box, *node.getChild(ii));

*/
    Core::AABoxf boxNode = node.getBoundingBox();
    if (boxNode.isValid()==true)
        box.setUnion(boxNode);

    for(int32 ii=0; ii < node.getChildCount(); ii++)
        internalGetHierarchyBox(box, *node.getChild(ii));
}
//-----------------------------------------------------------------------------
LM_API_UNI void getHierarchyBox(Core::AABoxf & box, const Node & node)
{
    box.clear();
    internalGetHierarchyBox(box, node);
}
//-----------------------------------------------------------------------------
LM_API_UNI void getInstanceHierarchyBox(Core::AABoxf & box, const Node & node)
{
    for(int32 ii=0; ii < node.getChildCount(); ii++)
        if(node.getChild(ii)->isInstanceChild())
            internalGetHierarchyBox(box, *node.getChild(ii));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void internalGetAllChildren(Core::List<Ptr<Node> >& children, const Ptr<Node>& node)
{
    children.push_back(node);

    for(int32 ii=0; ii < node->getChildCount(); ii++)
        internalGetAllChildren(children, node->getChild(ii));
}
//-----------------------------------------------------------------------------
LM_API_UNI void getAllChildren(Core::List<Ptr<Node> >& children, const Node& node)
{
    children.clear();

    for(int32 ii=0; ii < node.getChildCount(); ii++)
        internalGetAllChildren(children, node.getChild(ii));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void internalFlattenInstances(Core::List<Node * > & children, Node * pInst)
{
    children.push_back(pInst);

    for(int32 ii=0; ii < pInst->getChildCount(); ii++)
        internalFlattenInstances(children, (Node*)pInst->getChild(ii).get());
}
//-----------------------------------------------------------------------------
LM_API_UNI void getFlattenInstances(Core::List<Node * > & children, Node * pInst)
{
    children.push_back(pInst);

    for(int32 ii=0; ii < pInst->getChildCount(); ii++)
        if(pInst->getChild(ii)->isInstanceChild())
            internalFlattenInstances(children, (Node*)pInst->getChild(ii).get());
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
LM_API_UNI void filterByType(Core::List<Ptr<Node> >& childrenOut, const Core::List<Ptr<Node> >& childrenIn, ENodeType type)
{
    Core::List<ENodeType> types;
        types.push_back(type);

    filterByTypes(childrenOut, childrenIn, types);
}
//-----------------------------------------------------------------------------
LM_API_UNI void filterByTypes(Core::List<Ptr<Node> >& childrenOut, const Core::List<Ptr<Node> >& childrenIn, Core::List<ENodeType> types)
{
    childrenOut.clear();

    for(int32 ii=0; ii < childrenIn.size(); ii++)
    {
        for(int32 iType=0; iType<types.size(); iType++)
        {
            if (childrenIn[ii]->getNodeType() == types[iType])
                childrenOut.push_back(childrenIn[ii]);
        }
    }
}
//-----------------------------------------------------------------------------
/*
 * parcours les enfants de node puis test l'intersection entre node et ray 
 * si il y a intersection on test la distance si l'intersection est plus proche du rayon on stocke les info dans nodeIntersecting et distance;
*/
void travelNodesRayIntersection(const Ptr<Node>& node, const Core::Rayf& ray, Ptr<Node>& nodeIntersecting, float& distance)
{
    if (node->isRoot() == true)
    {
        for (int32 iChild=0; iChild<node->getChildCount(); ++iChild)
        {
            const Ptr<Node>& child = node->getChild(iChild);
            travelNodesRayIntersection(child, ray, nodeIntersecting, distance);
        }

        //node->setBoundingBoxVisible(true);
        float distanceIntersection;
        if (node->isIntersecting(ray, distanceIntersection) == true)
        {
            if (distanceIntersection < distance || distance == -1)
            {
                distance = distanceIntersection;
                nodeIntersecting = node;
            }
        }
    }
}

LM_API_UNI bool nodesRayIntersection(const Core::List<Ptr<Node> >& nodes, const Core::Rayf& ray, Ptr<Node>& nodeIntersecting, float& distance)
{
    bool ret = false;
    Ptr<Node> node;
    float distanceIntersection = -1.f;

    for (int32 iNode=0; iNode<nodes.size(); ++iNode)
    {
        travelNodesRayIntersection(nodes[iNode], ray, nodeIntersecting, distanceIntersection);
    }

    if (distanceIntersection>=0)
    {
        ret = true;
        distance = distanceIntersection;
    }

    return ret;
}
//-----------------------------------------------------------------------------
void getCleanMatrix(Core::Matrix4f & mat, const Node & pivot, const Core::Trajectory & traj, float t)
{
    Core::Vector3f position;
    Core::Quaternionf orient;

    traj.getORPO(position, orient, t);

    mat.setQT(orient, position);
    mat = pivot.getLocalToWorldMatrix() * mat;

    const Core::Vector3f & scale = pivot.getLocalScale();

    if(scale.x < 0.0f)
        mat.setXVector(Core::cross(mat.getYVector(), mat.getZVector()).getNormalizedSafe());
    else if(scale.y < 0.0f)
        mat.setXVector(Core::cross(mat.getZVector(), mat.getXVector()).getNormalizedSafe());
    else if(scale.z < 0.0f)
        mat.setZVector(Core::cross(mat.getXVector(), mat.getYVector()).getNormalizedSafe());
}
//-----------------------------------------------------------------------------
}
