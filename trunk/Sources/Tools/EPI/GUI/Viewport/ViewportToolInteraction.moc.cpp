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
#include "ViewportToolInteraction.moc.h"

#include <QMouseEvent>

#include <EPI/Gui/Viewport/Tools/ViewportTool.moc.h>
#include <EPI/Gui/GuiDocument.moc.h>
namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ViewportToolInteraction::ViewportToolInteraction(const Ptr<GuiDocument>& pGDoc)
{
    _pVTool = pGDoc->getViewportTool();
    connect (pGDoc.get(), SIGNAL(signalActivatedViewportTool(const Ptr<IViewportTool>&)),
        this, SLOT(VToolChanged(const Ptr<IViewportTool>&)));
}
//-----------------------------------------------------------------------------
ViewportToolInteraction::~ViewportToolInteraction()
{
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::VToolChanged(const Ptr<IViewportTool>& pVTool)
{
    _pVTool = pVTool;
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::mouseMoveEvent        (QMouseEvent* event, const DrawContext& dc)
{
    if (_pVTool!=null)
    {
        _pVTool->mouseMoveEvent(event, dc);
    }
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::mousePressEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (_pVTool!=null)
    {
        _pVTool->mousePressEvent(event, dc);
    }
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::mouseReleaseEvent(QMouseEvent* event, const DrawContext& dc)
{
    if (_pVTool!=null)
    {
        _pVTool->mouseReleaseEvent(event, dc);
    }
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::mouseMiddleSingleClick (QMouseEvent* event, const DrawContext& dc)
{
    if (_pVTool!=null)
    {
        _pVTool->mouseMiddleSingleClick(event, dc);
    }
}
//-----------------------------------------------------------------------------
void ViewportToolInteraction::updateCameraMove (Ptr<Universe::NodeCamera> pNCam)
{
    if (_pVTool!=null)
    {
        _pVTool->updateCameraMove(pNCam);
    }
}
//-----------------------------------------------------------------------------
bool ViewportToolInteraction::isHighlighted()
{
    bool ret = false;

    if (_pVTool!=null)
    {
        ret = _pVTool->isHighlighted();
    }

    return ret;
}
//-----------------------------------------------------------------------------
bool ViewportToolInteraction::isSelected()
{
    bool ret = false;

    if (_pVTool!=null)
    {
        ret = _pVTool->isSelected();
    }

    return ret;
}
//-----------------------------------------------------------------------------
} // namespace EPI
