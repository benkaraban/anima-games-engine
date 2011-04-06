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
#ifndef IPROPERTYNODEEDITABLECHILDREN_H_
#define IPROPERTYNODEEDITABLECHILDREN_H_


#include <EPI/Document/PropertyNode.moc.h>

namespace EPI
{
class StateRecorder;

//-----------------------------------------------------------------------------
class LM_API_EPI PropertyNodeEditableChildren : public PropertyNode
{
    Q_OBJECT

public:
    PropertyNodeEditableChildren(const Ptr<StateRecorder>& pStateRecorder, const Core::String& name, bool isSelectable = false, bool isDeletable = true, const EPropertyType type = CONTENT);
    virtual ~PropertyNodeEditableChildren();

    virtual bool isEditable() const {return true;}

    const Ptr<Property>& getPtySelection() const;
    Ptr<Property>& getPtySelection();

    virtual void internalCopy(const Ptr<Property>& pSrc);

    virtual bool findPtyWithId(Ptr<Property>& pResult, int32 id);

private Q_SLOTS:
    virtual void editingSelectionChanged() = 0;

private:
    Ptr<Property>   _pPtySelection;
};
//-----------------------------------------------------------------------------
} // namespace EPI
#endif IPROPERTYNODEEDITABLECHILDREN_H_
