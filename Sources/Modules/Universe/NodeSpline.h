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
#ifndef UNIVERSE_NODESPLINE_H_
#define UNIVERSE_NODESPLINE_H_

#include <Universe/Node.h>
#include <Universe/RessourcePool.h>
#include <Renderer/ISpline.h>

namespace Universe
{
class LM_API_UNI NodeSpline : public Node
{
public:
    NodeSpline(const Ptr<Renderer::IAABox> & pBox,
               const Ptr<Renderer::ISpline> & pSpline,
               const Ptr<RessourcePool> & pRessourcesPool,
               World * pWorld);

    virtual ENodeType getNodeType() const { return NODE_SPLINE; };

    virtual void setSpline(const Core::CRSpline & spline);
    virtual const Core::CRSpline & getSpline() const;

    virtual void setResolution(int32 res);
    virtual int32 getResolution() const;

    virtual void setColor(const Core::Vector4f & color);
    virtual const Core::Vector4f & getColor() const;

    virtual void importXML(const Ptr<Core::XMLNode> & pNode);
    virtual void exportXML(const Ptr<Core::XMLNode> & pNode);

    virtual void update(double elapsed);

protected:
    virtual void onWorldMatrixUpdate();
    virtual void onKill();
    virtual void onVisibilityChange(bool flag);
    virtual Core::AABoxf internalGetBoundingBox() const;

    Ptr<Renderer::ISpline> _pSpline;
    Ptr<RessourcePool>     _pRessourcesPool;
};
}

#endif /* NODEDECAL_H_ */
