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
#include <EPI/GUI/Widget/DocumentToolbar.moc.h>

#include <EPI/GUI/Widget/ToolsTransform.moc.h>
#include <QGridLayout>
#include <EPI/ControllerMain.moc.h>
#include <EPI/GUI/GuiDocument.moc.h>
#include <EPI/GUI/Widget/ToolsTransform.moc.h>
#include <EPI/GUI/Widget/GuiDocumentAnimate.moc.h>
#include <EPI/GUI/Widget/GuiDocumentRunStopScript.moc.h>
#include <EPI/GUI/Widget/PickingFilter.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
DocumentToolBar::DocumentToolBar(const Ptr<ControllerMain>& pCtrl, QWidget* parent):
    QWidget(parent)
{
    _layout = NULL;
    //connection au signal aucun guiDocument dans la liste
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalNoGuiDocument()),
            this, SLOT(resetDocumentToolBar()));

    //signal un guiDocument est actif
    connect(const_cast<ControllerMain*>(pCtrl.get()), SIGNAL(signalActivatedGuiDocument(const Ptr<EPI::GuiDocument>&)),
            this, SLOT(selectActiveGuiDocument(const Ptr<EPI::GuiDocument>&)));
}
//-----------------------------------------------------------------------------
DocumentToolBar::~DocumentToolBar()
{
}
//-----------------------------------------------------------------------------
void DocumentToolBar::resetDocumentToolBar()
{
    _pCurrentGDoc = null;
    setupUi();
}
//-----------------------------------------------------------------------------
void DocumentToolBar::selectActiveGuiDocument(const Ptr<EPI::GuiDocument>& pGDoc)
{
    connectGuiDocument(false);
    
    resetDocumentToolBar();

    _pCurrentGDoc = pGDoc;
    setupUi();

    connectGuiDocument(true);
}
//-----------------------------------------------------------------------------
void DocumentToolBar::connectGuiDocument(bool makeConnection)
{
    if (_pCurrentGDoc!=null)
    {
        if (_pCurrentGDoc->getDocument()->getType() == DOC_PARTICLES || _pCurrentGDoc->getDocument()->getType() == DOC_TEST  || _pCurrentGDoc->getDocument()->getType() == DOC_WORLD)
        {
            connectDocumentParticles(makeConnection);
        }
        else
        {
            connectDocumentDefault(makeConnection);
        }

    }
}
//-----------------------------------------------------------------------------
void DocumentToolBar::connectDocumentDefault(bool makeConnection)
{

}
//-----------------------------------------------------------------------------
void DocumentToolBar::connectDocumentParticles(bool makeConnection)
{
    if (makeConnection==true)
    {
        connect(_pToolsTransform.get(), SIGNAL(signalToolChanged(const Ptr<IViewportTool>&)), 
            _pCurrentGDoc.get(), SLOT(setViewportTool(const Ptr<IViewportTool>&)));
    }
    else
    {
        disconnect(_pToolsTransform.get(), SIGNAL(signalToolChanged(const Ptr<IViewportTool>&)), 
            _pCurrentGDoc.get(), SLOT(setViewportTool(const Ptr<IViewportTool>&)));
    }
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUi()
{
    delete _layout;
    _layout = new QGridLayout(this);

    if (_pCurrentGDoc!=null)
    {
        if (_pCurrentGDoc->getDocument()->getType() == DOC_SPLINE)
        {
            setupUiDocumentSpline();
        }
        else if(_pCurrentGDoc->getDocument()->getType() == DOC_SCRIPT)
        {
            setupUiDocumentScript();
        }
        else if (_pCurrentGDoc->getDocument()->getType() == DOC_WORLD)
        {
            setupUiDocumentWorld();
        }
        else if (_pCurrentGDoc->getDocument()->getType() == DOC_GROUP)
        {
            setupUiDocumentGroup();
        }
        else
        {
            setupUiDocumentDefault();
        }
    }
    else
    {
        setupUiDocumentDefault();
    }

    _layout->setContentsMargins(0,0,0,0);
    _layout->setMargin(0);
    _layout->setAlignment(Qt::AlignLeft);

    setFixedHeight(30);
    update();
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUiDocumentDefault()
{
    _pToolsTransform        = null;
    _pGDocAnimate           = null;
    _pGDocRunStopScript     = null;
    _pRestart               = null;
    _pPickingFilter         = null;
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUiDocumentScript()
{
    _pToolsTransform = null;
    _pRestart = null;

    _pGDocAnimate = Ptr<GuiDocumentAnimate>(new GuiDocumentAnimate(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocAnimate.get(), 0, 1);

    _pGDocRunStopScript = Ptr<GuiDocumentRunStopScript>(new GuiDocumentRunStopScript(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocRunStopScript.get(), 0, 2);
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUiDocumentSpline()
{
    _pGDocAnimate = Ptr<GuiDocumentAnimate>(new GuiDocumentAnimate(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocAnimate.get(), 0, 1, Qt::AlignLeft);

    _pToolsTransform = Ptr<ToolsTransform>(new ToolsTransform(_pCurrentGDoc, this));
    _layout->addWidget(_pToolsTransform.get(), 0, 1, Qt::AlignLeft);
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUiDocumentGroup()
{
    _pGDocAnimate = Ptr<GuiDocumentAnimate>(new GuiDocumentAnimate(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocAnimate.get(), 0, 0, Qt::AlignLeft);

    _pRestart = Ptr<GuiDocumentRestart>(new GuiDocumentRestart(_pCurrentGDoc, this));
    _layout->addWidget(_pRestart.get(), 0, 1, Qt::AlignLeft);

    _pGDocRunStopScript = Ptr<GuiDocumentRunStopScript>(new GuiDocumentRunStopScript(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocRunStopScript.get(), 0, 2, Qt::AlignLeft);

    _pPickingFilter = Ptr<PickingFilterWidget>(new PickingFilterWidget(_pCurrentGDoc, this));
    _layout->addWidget(_pPickingFilter.get(), 0, 3, Qt::AlignLeft);

    _pToolsTransform = Ptr<ToolsTransform>(new ToolsTransform(_pCurrentGDoc, this));
    _layout->addWidget(_pToolsTransform.get(), 0, 4, Qt::AlignLeft);
}
//-----------------------------------------------------------------------------
void DocumentToolBar::setupUiDocumentWorld()
{
    _pGDocAnimate = Ptr<GuiDocumentAnimate>(new GuiDocumentAnimate(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocAnimate.get(), 0, 0, Qt::AlignLeft);

    _pRestart = Ptr<GuiDocumentRestart>(new GuiDocumentRestart(_pCurrentGDoc, this));
    _layout->addWidget(_pRestart.get(), 0, 1, Qt::AlignLeft);

    _pGDocRunStopScript = Ptr<GuiDocumentRunStopScript>(new GuiDocumentRunStopScript(_pCurrentGDoc, this));
    _layout->addWidget(_pGDocRunStopScript.get(), 0, 2, Qt::AlignLeft);

    _pPickingFilter = Ptr<PickingFilterWidget>(new PickingFilterWidget(_pCurrentGDoc, this));
    _layout->addWidget(_pPickingFilter.get(), 0, 3, Qt::AlignLeft);

    _pToolsTransform = Ptr<ToolsTransform>(new ToolsTransform(_pCurrentGDoc, this));
    _layout->addWidget(_pToolsTransform.get(), 0, 4, Qt::AlignLeft);
}
//-----------------------------------------------------------------------------
} //namespace EPI
