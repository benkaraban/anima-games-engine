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
#include "PtyEmitterShape.h"

namespace EPI
{
//-----------------------------------------------------------------------------
PtyEmitterShape::PtyEmitterShape(const Ptr<Universe::NodeEmitter>& pNodeE, Ptr<Universe::IEmitterShape> pShape, const Core::String& title) :
    PropertyNode(title, false, false, CONTENT),
    _pNodeE(pNodeE),
    _pShape(pShape)
{
}
//-----------------------------------------------------------------------------
PtyEmitterShape::PtyEmitterShape(const PtyEmitterShape& other)
{
    _pShape = null;
    setUniverseNodeEmitter(other.getUniverseNodeEmitter());
}
//-----------------------------------------------------------------------------
PtyEmitterShape::~PtyEmitterShape()
{
}
//-----------------------------------------------------------------------------
void PtyEmitterShape::updateData()
{
}
//-----------------------------------------------------------------------------
void PtyEmitterShape::updateProperty()
{
}
//-----------------------------------------------------------------------------
Ptr<Universe::IEmitterShape> PtyEmitterShape::getShape() const
{
    return _pShape;
}
//-----------------------------------------------------------------------------
void PtyEmitterShape::setShape(const Ptr<Universe::IEmitterShape> pShape)
{
    _pShape = pShape;
}
//-----------------------------------------------------------------------------
void PtyEmitterShape::internalCopy(const Ptr<Property>& pSrc)
{
    //Ptr<Universe::NodeEmitter> tmpPNode = getUniverseNodeEmitter();
    Ptr<Universe::IEmitterShape> tmpPS    = getShape();

    Ptr<PtyEmitterShape> pPty = LM_DEBUG_PTR_CAST<PtyEmitterShape>(pSrc);

    //setUniverseNodeEmitter(tmpPNode);
    setUniverseNodeEmitter(pPty->getUniverseNodeEmitter());
    setShape(tmpPS);

    updateData();
}
//-----------------------------------------------------------------------------
void PtyEmitterShape::setUniverseNodeEmitter(const Ptr<Universe::NodeEmitter>& pNode)
{
    _pNodeE = pNode;
}
//-----------------------------------------------------------------------------
const Ptr<Universe::NodeEmitter>& PtyEmitterShape::getUniverseNodeEmitter() const
{
    return _pNodeE;
}
//-----------------------------------------------------------------------------
} //namespace EPI