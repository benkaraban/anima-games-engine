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
#include "ToolsPanel.moc.h"

#include <QVBoxLayout>

#include <EPI/ControllerMain.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>

#include <EPI/Document/Properties/PtyTransform.moc.h>
#include <EPI/GUI/Viewport/Tools/ViewportTool.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
ToolsPanel::ToolsPanel(const Ptr<ControllerMain>& pCtrl, QWidget * parent) :
    QWidget(parent)
{
    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(cleanPanel()));

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));

    setupUi();
}
//-----------------------------------------------------------------------------
ToolsPanel::~ToolsPanel()
{
}
//-----------------------------------------------------------------------------
void ToolsPanel::setupUi()
{
    _layout = new QVBoxLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    setLayout(_layout);
}
//-----------------------------------------------------------------------------
void ToolsPanel::cleanPanel()
{
    _pVToolW = null;
    _pGDoc = null;
    update();
}
//-----------------------------------------------------------------------------
void ToolsPanel::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    if (_pGDoc!=null)
    {
        disconnectToGuiDocument();
    }

    cleanPanel();
    _pGDoc = pGDoc;

    if (_pGDoc!=null)
    {
        connectToGuiDocument();
    }
}
//-----------------------------------------------------------------------------
void ToolsPanel::connectToGuiDocument()
{
    connect(_pGDoc.get(), SIGNAL(signalActivatedViewportTool(const Ptr<IViewportTool>&)),
            this, SLOT(selectActiveViewportTool(const Ptr<IViewportTool>&)));
}
//-----------------------------------------------------------------------------
void ToolsPanel::disconnectToGuiDocument()
{
}
//-----------------------------------------------------------------------------
void ToolsPanel::selectActiveViewportTool(const Ptr<IViewportTool>& pVTool)
{
    if (_pGDoc!=null)
    {
        if (pVTool!=null)
        {
            _pVToolW = pVTool->createViewportToolWidget(this);
            _layout->addWidget(_pVToolW.get(), 0, Qt::AlignCenter | Qt::AlignTop);
            //setMinimumSize(_pVToolW->width(), _pVToolW->height());
        }
        else
        {
            _pVToolW = null;
        }
        update();
    }
}
//-----------------------------------------------------------------------------
} //namespace EPI