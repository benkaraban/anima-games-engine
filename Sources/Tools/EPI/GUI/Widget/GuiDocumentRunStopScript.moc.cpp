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
#include "GuiDocumentRunStopScript.moc.h"

#include <EPI/Gui/GuiDocument.moc.h>
#include <EPI/Document/DocumentBase.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GuiDocumentRestart::GuiDocumentRestart(const Ptr<GuiDocument>& pGDoc, QWidget * parent) :
    QWidget(parent),
    _pRestart(NULL),
    _pGDoc(pGDoc),
    _init(false)
{
    setupUi();      
}
//-----------------------------------------------------------------------------
GuiDocumentRestart::~GuiDocumentRestart()
{
}
//-----------------------------------------------------------------------------
void GuiDocumentRestart::setupUi()
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->setSpacing(0);
    _pRestart = new QPushButton(QIcon(":/icons/restart.png"), "", this);
    
    _pRestart->setFlat(true);

    connect(_pRestart, SIGNAL(clicked()), this, SLOT(restart()));

    int32 val = Core::min(width(),  height());
    _pRestart->setIconSize(QSize(val-6, val-6));

    _layout->addWidget(_pRestart);
    setLayout(_layout);
}
//-----------------------------------------------------------------------------
void GuiDocumentRestart::restart()
{
    if (_pGDoc!=null)
    {
        _pGDoc->restart();
    }
}
//-----------------------------------------------------------------------------
void GuiDocumentRestart::resizeEvent ( QResizeEvent * event )
{/*
    if (_init==false)
    {
        int32 val = Core::min(width(),  height());
        _pRestart->setIconSize(QSize(val-4, val-4));
        _init = true;
    }*/
    QWidget::resizeEvent(event);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GuiDocumentRunStopScript::GuiDocumentRunStopScript(const Ptr<GuiDocument>& pGDoc, QWidget * parent):
    QWidget(parent),
    _pRunStop(NULL),
    _pGDoc(pGDoc)
{
    setupUi();                       
    connect(_pGDoc->getDocument().get(), SIGNAL(runStopScriptChanged()),
            this, SLOT(runStopScriptChanged()));
                       

}
//-----------------------------------------------------------------------------
GuiDocumentRunStopScript::~GuiDocumentRunStopScript()
{
    delete _pRunStop;
}
//-----------------------------------------------------------------------------
void GuiDocumentRunStopScript::setupUi()
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->setSpacing(0);

    _pRunStop = new QPushButton(QIcon(":/icons/runScript.png"), "", this);
    _pRunStop->setCheckable(true);
    _pRunStop->setFlat(true);

    int32 val = Core::min(width(),  height());
    _pRunStop->setIconSize(QSize(val-6, val-6));
    
    connect(_pRunStop, SIGNAL(toggled(bool)), this, SLOT(runStop(bool)));
    _layout->addWidget(_pRunStop);
    setLayout(_layout);

    _pRunStop->setChecked(!_pGDoc->getDocument()->isEditing());
}
//-----------------------------------------------------------------------------
void GuiDocumentRunStopScript::runStopScriptChanged()
{
    disconnect(_pRunStop, SIGNAL(toggled(bool)), this, SLOT(runStop(bool)));
    if (_pGDoc->getDocument()->isEditing() == true)
    {
        _pRunStop->setChecked(false);
    }
    else
    {
        _pRunStop->setChecked(true);
    }
    connect(_pRunStop, SIGNAL(toggled(bool)), this, SLOT(runStop(bool)));
}
//-----------------------------------------------------------------------------
void GuiDocumentRunStopScript::runStop(bool toggled)
{
    disconnect(_pGDoc->getDocument().get(), SIGNAL(runStopScriptChanged()),
            this, SLOT(runStopScriptChanged()));

    _pGDoc->getDocument()->runStopScript();
    runStopScriptChanged();

    connect(_pGDoc->getDocument().get(), SIGNAL(runStopScriptChanged()),
            this, SLOT(runStopScriptChanged()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

