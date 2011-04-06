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
#ifndef I_CONTENT_REPRESENTATION_H_
#define I_CONTENT_REPRESENTATION_H_

#include <Core/Standard.h>
#include <Universe/Tools.h>
namespace Universe
{
    class World;
    class Node;
}


namespace EPI
{
class Property;

LM_ENUM_2 (ECRMode,
           SELECTION_REPRESENTATION,
           OBJECT_REPRESENTATION)

//-----------------------------------------------------------------------------
class LM_API_EPI IContentRepresentation
{
protected:
    IContentRepresentation() : isVisible(true) {}

public:
    virtual ~IContentRepresentation() {}
    virtual Ptr<IContentRepresentation> clone() = 0;
    virtual void updateObjectRepresentation(const Property& pPty) = 0;
    virtual void setRepresentationMode(ECRMode mode)              = 0;
    virtual const Ptr<Universe::NodeGroup>& getNodeRepresentation()   = 0;
    virtual bool isItMe(const Ptr<Universe::Node>& pNode) = 0;
    virtual void setNodeToRepresent(const Ptr<Universe::Node>& pNode) = 0;
    virtual bool isIntersecting (const Core::Rayf& ray, float& distance) {return false;}
    Core::AABoxf getBoundingBox()
    {
        Core::AABoxf boxNode;
        getHierarchyBox(boxNode, *(getNodeRepresentation()));

        return boxNode;
    }
    virtual void setVisible(bool flag) {isVisible = flag; getNodeRepresentation()->setVisible(flag, true);}

protected:
    bool isVisible;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //I_CONTENT_REPRESENTATION_H_
