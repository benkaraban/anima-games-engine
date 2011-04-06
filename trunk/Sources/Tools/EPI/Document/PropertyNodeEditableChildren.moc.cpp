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
#include <EPI/Document/PropertyNodeEditableChildren.moc.h>

#include <EPI/Document/PropertySelection.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PropertyNodeEditableChildren::PropertyNodeEditableChildren(const Ptr<StateRecorder>& pStateRecorder, const Core::String& name, bool isSelectable, bool isDeletable, const EPropertyType type) :
    PropertyNode(name, isSelectable, isDeletable, type)
{
    _pPtySelection = Ptr<PropertySelectionDefault>( new PropertySelectionDefault(L"Selection_"+name, this, pStateRecorder) );

    connect (_pPtySelection.get(), SIGNAL(selectionChanged()),
        this, SLOT(editingSelectionChanged()));
}
//-----------------------------------------------------------------------------
PropertyNodeEditableChildren::~PropertyNodeEditableChildren()
{
}
//-----------------------------------------------------------------------------
void PropertyNodeEditableChildren::internalCopy(const Ptr<Property>& pSrc)
{
    Ptr<PropertyNodeEditableChildren> pPty = LM_DEBUG_PTR_CAST<PropertyNodeEditableChildren>(pSrc);

    _pPtySelection->copy(pPty->_pPtySelection);
}
//-----------------------------------------------------------------------------
const Ptr<Property>& PropertyNodeEditableChildren::getPtySelection() const
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
Ptr<Property>& PropertyNodeEditableChildren::getPtySelection()
{
    return _pPtySelection;
}
//-----------------------------------------------------------------------------
bool PropertyNodeEditableChildren::findPtyWithId(Ptr<Property>& pResult, int32 id)
{
    if (id == _pPtySelection->getId())
    {
        pResult = _pPtySelection;
        return true;
    }
    
    return PropertyNode::findPtyWithId(pResult, id);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

