/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef UNIVERSE_NODEGROUP_H_
#define UNIVERSE_NODEGROUP_H_

#include <Universe/Node.h>

namespace Universe
{

/**
 * Un NodeGroup se comporte comme un Node normal si ce n'est que sa bounding box englobe tous ses descendants.
 * (d'autres différences sont à envisager ?)
 *
 * Attention : la bounding box est mise à jour à chaque update, il peut donc y avoir un décalage d'une frame entre
 * une modification de bounding box des enfants et celle du NodeGroup
 */
class LM_API_UNI NodeGroup : public Node
{
public:
    NodeGroup(const Ptr<Renderer::IAABox> & pBox, World * pWorld);

    virtual void addChild(const Ptr<Node> & pNode, bool sameWorldPos = false, int32 indexPos=-1);
    virtual void removeChild(const Ptr<Node> & pNode, bool sameWorldPos = true);

    virtual ENodeType getNodeType() const { return NODE_GROUP; };

    virtual void update(double elapsed);

    virtual void killWhenChildrenAreDead(bool killWhenChildrenAreDead) { _killWhenChildrenAreDead = killWhenChildrenAreDead; }
    virtual bool killWhenChildrenAreDead() const { return _killWhenChildrenAreDead; }

    virtual void prepareForPacking();

protected:
    virtual Core::AABoxf internalGetBoundingBox() const;
    virtual bool internalIsIntersecting(const Core::Rayf& ray, float& distance);
    virtual void internalPrepareForPacking();

    friend class World;

protected:
    bool    _killWhenChildrenAreDead;
};

}

#endif /* NODE_H_ */
