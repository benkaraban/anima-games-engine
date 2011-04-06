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
#include "SelectionManager.h"

#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Document/DocumentBase.moc.h>

#include <Core/Logger.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
PtyNodeSelectionManager::PtyNodeSelectionManager(const Ptr<PropertySelection>& pPtySelection) :
    ISelectionManager(),
    _pPtySelection(pPtySelection),
    _useAddToSelection(false)
{
    connect(_pPtySelection.get(), SIGNAL(selectionChanged()), this, SLOT(selectChanged()));
}
//-----------------------------------------------------------------------------
PtyNodeSelectionManager::~PtyNodeSelectionManager()
{
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::select(const Ptr<Property>& pPty)
{
    if (_useAddToSelection == false)
    {
        _pPtySelection->select(pPty);
    }
    else
    {
        _pPtySelection->addToSelection(pPty);
    }
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::addToSelection(const Ptr<Property>& pPty)
{
    _pPtySelection->addToSelection(pPty);
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::removeFromSelection(const Ptr<Property>& pPty)
{
    _pPtySelection->removeFromSelection(pPty);
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::deselectAll()
{
    _pPtySelection->deselectAll();
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::selectAll()
{
    ERR << L"DocumentSelectionManager::selectAll() not implemented";
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::invertSelection()
{
    ERR << L"DocumentSelectionManager::invertSelection() not implemented";
}
//-----------------------------------------------------------------------------
void PtyNodeSelectionManager::configureAddToSelection(bool flag)
{
    _useAddToSelection = flag;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
DocumentSelectionManager::DocumentSelectionManager(const Ptr<PropertySelection>& pPtySelection) :
    ISelectionManager(),
    _pPtySelection(pPtySelection),
    _useAddToSelection(false)
{
    connect(_pPtySelection.get(), SIGNAL(selectionChanged()), this, SLOT(selectChanged()));
}
//-----------------------------------------------------------------------------
DocumentSelectionManager::~DocumentSelectionManager()
{
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::select(const Ptr<Property>& pPty)
{
    if (_useAddToSelection == false)
    {
        _pPtySelection->select(pPty);
    }
    else
    {
        _pPtySelection->addToSelection(pPty);
    }
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::addToSelection(const Ptr<Property>& pPty)
{
    _pPtySelection->addToSelection(pPty);
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::removeFromSelection(const Ptr<Property>& pPty)
{
    _pPtySelection->removeFromSelection(pPty);
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::deselectAll()
{
    _pPtySelection->deselectAll();
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::selectAll()
{
    ERR << L"DocumentSelectionManager::selectAll() not implemented";
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::invertSelection()
{
    ERR << L"DocumentSelectionManager::invertSelection() not implemented";
}
//-----------------------------------------------------------------------------
void DocumentSelectionManager::configureAddToSelection(bool flag)
{
    _useAddToSelection = flag;
}
//-----------------------------------------------------------------------------
} //namespace EPI
