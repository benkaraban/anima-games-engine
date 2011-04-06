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
#include "PtyDocEdition.moc.h"

#include <algorithm>
#include <EPI/Document/State.h>
#include <EPI/Document/StateRecorder.h>

#include <EPI/Document/PropertySelection.moc.h>
namespace EPI
{
//TODO virer les constantes
const int32 TEMP_SELECTION = 0;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyDocEdition::PtyDocEdition(const String& name, const Ptr<Property>& pRootPtySelectable, const Ptr<StateRecorder>& pStateRecorder) :
    PropertyNode(name, false, DOCUMENT_EDITION)
{
    addChild(Ptr<Property> (new PropertySelectionDefault(L"CurrentSelection", pRootPtySelectable.get(), pStateRecorder)));
}
//-----------------------------------------------------------------------------
PtyDocEdition::~PtyDocEdition()
{
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PtyDocEdition::getPtySelection() const
{
    return getChild(TEMP_SELECTION);
}
//-----------------------------------------------------------------------------
Ptr<Property>& PtyDocEdition::getPtySelection()
{
    return getChild(TEMP_SELECTION);
}
//-----------------------------------------------------------------------------
void PtyDocEdition::changePropertySelection(const Ptr<Property>& pPtySelection)
{
    removeChild(TEMP_SELECTION);
    addChild(pPtySelection);
}
//-----------------------------------------------------------------------------
} // namespace EPI
