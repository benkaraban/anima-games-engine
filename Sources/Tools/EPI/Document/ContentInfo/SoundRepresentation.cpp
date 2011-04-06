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
#include "SoundRepresentation.h"

#include <Universe/World.h>
#include <EPI/Document/Properties/PtyTrajectory/PtyCheckPoint.moc.h>
#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <Core/List.h>

#include <Assets/GeometricPrimitive.h>
#include <Assets/Tool.h>

#include <EPI/Constants.h>

namespace EPI
{
const String tagNodeForSelection       = L"SoundRepresentationSelection";


//-----------------------------------------------------------------------------
Ptr<Universe::NodeGroup> createSoundUniverseRepresentation(const Ptr<Universe::World>& pWorld)
{
    Ptr<Universe::NodeGroup> pRet = pWorld->createGroup();

    Ptr<Universe::NodeDecal> pNodeDecal = pWorld->createDecal();
        pNodeDecal->setTexture(L"/EpiData/iconSound.tex");
        pNodeDecal->addTag(tagNodeForSelection);
        pNodeDecal->setBillboard(true);

    pRet->addChild(pNodeDecal);

    return pRet;
}
//-----------------------------------------------------------------------------
SoundRepresentation::SoundRepresentation(const Ptr<Universe::World>& pWorld, const Ptr<Universe::Node>& pNodeToRepresent):
    IContentRepresentation(),
    _mode(SELECTION_REPRESENTATION),
    _pWorld(pWorld)
{
    _pNodeRepresentation = createSoundUniverseRepresentation(pWorld);

    pNodeToRepresent->addGhostChild(_pNodeRepresentation);
}
//-----------------------------------------------------------------------------
SoundRepresentation::SoundRepresentation(const SoundRepresentation& other):
    IContentRepresentation(other),
    _pWorld(other._pWorld)
{
    _pNodeRepresentation = createSoundUniverseRepresentation(_pWorld);
        
}
//-----------------------------------------------------------------------------
SoundRepresentation::~SoundRepresentation()
{
    _pNodeRepresentation->kill();
}
//-----------------------------------------------------------------------------
void SoundRepresentation::updateObjectRepresentation(const Property& pPty)
{

}
//-----------------------------------------------------------------------------
void SoundRepresentation::setRepresentationMode(ECRMode mode)
{
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeGroup>& SoundRepresentation::getNodeRepresentation()
{
    return _pNodeRepresentation;
}
//-----------------------------------------------------------------------------
bool SoundRepresentation::isItMe(const Ptr<Universe::Node>& pNode)
{
    if (pNode==_pNodeRepresentation)    return true;
    return false;
}
//-----------------------------------------------------------------------------
Ptr<IContentRepresentation> SoundRepresentation::clone()
{
    Ptr<SoundRepresentation>   pRet = Ptr<SoundRepresentation>(new SoundRepresentation (*this));

    return pRet;
}
//-----------------------------------------------------------------------------
void SoundRepresentation::setNodeToRepresent(const Ptr<Universe::Node>& pNode)
{
    pNode->addGhostChild(pNode);
}
//-----------------------------------------------------------------------------
bool SoundRepresentation::isIntersecting (const Core::Rayf& ray, float& distance)
{
    if (isVisible==false)   return false;

    Ptr<Universe::Node> pNode = LM_DEBUG_PTR_CAST<Universe::Node>(_pNodeRepresentation->getChildWithTag(tagNodeForSelection));
    return pNode->isIntersecting(ray, distance);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI