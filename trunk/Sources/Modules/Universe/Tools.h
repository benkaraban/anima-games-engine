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
#ifndef UNIVERSE_TOOLS_H_
#define UNIVERSE_TOOLS_H_

#include <Universe/Node.h>

namespace Core
{
    class Trajectory;
}

namespace Universe
{
/**
 * Retourne la bounding box de toute la hierarchie.
 */
LM_API_UNI void getHierarchyBox(Core::AABoxf & box, const Node & node);

/**
 *  children contiendra tous les enfants de node
 */
LM_API_UNI void getAllChildren(Core::List<Ptr<Node> >& children, const Node& node);


/**
 *  place dans childrenOut les node de childrenIn correspondant aux type specifie
 */
LM_API_UNI void filterByType(Core::List<Ptr<Node> >& childrenOut, const Core::List<Ptr<Node> >& childrenIn, ENodeType type);
/**
 *  place dans childrenOut les node de childrenIn correspondant aux types specifies
 */
LM_API_UNI void filterByTypes(Core::List<Ptr<Node> >& childrenOut, const Core::List<Ptr<Node> >& childrenIn, Core::List<ENodeType> types);

/**
 * parcours une liste de nodes pour tester l'intersection avec un rayon. 
 * si intersection nodeIntersecting contient le node ayant l'intersection, distance contient la distance entre l'originie du rayon et le point d'intersection
 */
LM_API_UNI bool nodesRayIntersection(const Core::List<Ptr<Node> >& nodes, const Core::Rayf& ray, Ptr<Node>& nodeIntersecting, float& distance);


LM_API_UNI void getInstanceHierarchyBox(Core::AABoxf & box, const Node & node);
LM_API_UNI void getFlattenInstances(Core::List<Node *> & children, Node * pInst);

LM_API_UNI void getCleanMatrix(Core::Matrix4f & mat, const Node & pivot, const Core::Trajectory & traj, float t);

}

#endif
