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
#ifndef CAMERA_REPRESENTATION_H_
#define CAMERA_REPRESENTATION_H_

#include <Core/Standard.h>
#include <EPI/Document/ContentInfo/IContentRepresentation.h>
#include <Universe/World.h>

namespace Universe
{
    class World;
    class Node;
}

namespace EPI
{
//-----------------------------------------------------------------------------
class LM_API_EPI CameraRepresentation : public IContentRepresentation
{
public:
    CameraRepresentation(const Ptr<Universe::World>& pWorldForRepresentation, const Ptr<Universe::Node>& pNodeToRepresent);
    CameraRepresentation(const CameraRepresentation& other);
    virtual ~CameraRepresentation();
    virtual void updateObjectRepresentation(const Property& pPty);
    virtual void setRepresentationMode(ECRMode mode);
    virtual const Ptr<Universe::NodeGroup>& getNodeRepresentation();
    virtual bool isItMe(const Ptr<Universe::Node>& pNode);
    virtual bool isIntersecting (const Core::Rayf& ray, float& distance);
    virtual Ptr<IContentRepresentation> clone();
    virtual void setNodeToRepresent(const Ptr<Universe::Node>& pNode);
    virtual void setVisible(bool flag);
private:
    Ptr<Universe::World>        _pWorld;
    Ptr<Universe::NodeGroup>    _pNodeRepresentation;
    ECRMode                     _mode;
};
//-----------------------------------------------------------------------------
} // namespace EPI

#endif //CAMERA_REPRESENTATION_H_
