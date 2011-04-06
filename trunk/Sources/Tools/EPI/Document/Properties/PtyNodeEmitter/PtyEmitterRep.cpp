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
#include "PtyEmitterRep.h"

namespace EPI
{
//-----------------------------------------------------------------------------
PtyEmitterRep::PtyEmitterRep(const Ptr<Universe::NodeEmitter>& pNodeE, Ptr<Universe::IPartRepresentation> pRep, const Core::String& title) :
    PropertyNode(title, false, false, CONTENT),
    _pNodeE(pNodeE),
    _pRep(pRep)
{
}
//-----------------------------------------------------------------------------
PtyEmitterRep::PtyEmitterRep(const PtyEmitterRep& other)
{
    _pRep = null;
    setUniverseNodeEmitter(other.getUniverseNodeEmitter());
}
//-----------------------------------------------------------------------------
PtyEmitterRep::~PtyEmitterRep()
{
}
//-----------------------------------------------------------------------------
void PtyEmitterRep::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyEmitterRep::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Universe::IPartRepresentation> PtyEmitterRep::getRepresentation() const
{
    return _pRep;
}
//-----------------------------------------------------------------------------
void PtyEmitterRep::setRepresentation(const Ptr<Universe::IPartRepresentation> & pRep)
{
    _pRep = pRep;
}
//-----------------------------------------------------------------------------
void PtyEmitterRep::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<Universe::IPartRepresentation> tmpPS    = getRepresentation();

    Ptr<PtyEmitterRep> pPty = LM_DEBUG_PTR_CAST<PtyEmitterRep>(pSrc);

    setUniverseNodeEmitter(pPty->getUniverseNodeEmitter());
    setRepresentation(tmpPS);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterRep::setUniverseNodeEmitter(const Ptr<Universe::NodeEmitter>& pNode)
{
    _pNodeE = pNode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeEmitter>& PtyEmitterRep::getUniverseNodeEmitter() const
{
    return _pNodeE;
}
//-----------------------------------------------------------------------------
} //namespace EPI