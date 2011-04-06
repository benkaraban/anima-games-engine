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
#include "PtyPartEffect.h"

namespace EPI
{
//-----------------------------------------------------------------------------
PtyPartEffect::PtyPartEffect(const Ptr<Universe::NodeEmitter>& pNodeE, Ptr<Universe::IPartEffect> pEffect, const Core::String& title) :
    PropertyNode(title, false, false, CONTENT),
    _pNodeE(pNodeE),
    _pEffect(pEffect)
{
}
//-----------------------------------------------------------------------------
PtyPartEffect::PtyPartEffect(const PtyPartEffect& other)
{
    _pEffect = null;
    setUniverseNodeEmitter(other.getUniverseNodeEmitter());
}
//-----------------------------------------------------------------------------
PtyPartEffect::~PtyPartEffect()
{
}
//-----------------------------------------------------------------------------
void PtyPartEffect::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyPartEffect::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Universe::IPartEffect> PtyPartEffect::getEffect() const
{
    return _pEffect;
}
//-----------------------------------------------------------------------------
void PtyPartEffect::setEffect(const Ptr<Universe::IPartEffect> pEffect)
{
    _pEffect = pEffect;
}
//-----------------------------------------------------------------------------
void PtyPartEffect::internalCopy(const Ptr<Property>& pSrc)
{
    //Ptr<Universe::NodeEmitter> tmpPNode = getUniverseNodeEmitter();
    Ptr<Universe::IPartEffect> tmpPE    = getEffect();

    Ptr<PtyPartEffect> pPty = LM_DEBUG_PTR_CAST<PtyPartEffect>(pSrc);

    //setUniverseNodeEmitter(tmpPNode);
    setUniverseNodeEmitter(pPty->getUniverseNodeEmitter());
    setEffect(tmpPE);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyPartEffect::setUniverseNodeEmitter(const Ptr<Universe::NodeEmitter>& pNode)
{
    _pNodeE = pNode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeEmitter>& PtyPartEffect::getUniverseNodeEmitter() const
{
    return _pNodeE;
}
//-----------------------------------------------------------------------------
} //namespace EPI