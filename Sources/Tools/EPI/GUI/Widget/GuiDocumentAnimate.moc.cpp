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
#include "GuiDocumentAnimate.moc.h"

#include <EPI/Gui/GuiDocument.moc.h>

namespace EPI
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
GuiDocumentAnimate::GuiDocumentAnimate(const Ptr<GuiDocument>& pGDoc, QWidget * parent):
    QWidget(parent),
    _pPlayStopAnimate(NULL),
    _pGDoc(pGDoc)
{
    setupUi();                       
    connect(_pGDoc.get(), SIGNAL(playStopChanged()),
            this, SLOT(playStopGuiDocChanged()));



    //playStopGuiDocChanged();                       
}
//-----------------------------------------------------------------------------
GuiDocumentAnimate::~GuiDocumentAnimate()
{
    delete _pPlayStopAnimate;
}
//-----------------------------------------------------------------------------
void GuiDocumentAnimate::setupUi()
{
    _layout = new QHBoxLayout(this);
    _layout->setContentsMargins(0,0,0,0);
    _layout->setSpacing(0);

    _pPlayStopAnimate = new QPushButton(QIcon(":/icons/pause.png"), "", this);
    _pPlayStopAnimate->setCheckable(true);
    _pPlayStopAnimate->setFlat(true);
    int32 val = Core::min(width(),  height());
    _pPlayStopAnimate->setIconSize(QSize(val-6, val-6));

    if (_pGDoc->isAnimated() == true)
    {
        _pPlayStopAnimate->setChecked(false);
    }
    else
    {
        _pPlayStopAnimate->setChecked(true);
    }

    connect(_pPlayStopAnimate, SIGNAL(toggled(bool)), this, SLOT(playStop2(bool)));
    _layout->addWidget(_pPlayStopAnimate);
    setLayout(_layout);

    _pPlayStopAnimate->setChecked(!_pGDoc->isAnimated());
}
//-----------------------------------------------------------------------------
void GuiDocumentAnimate::playStopGuiDocChanged()
{
    disconnect(_pGDoc.get(), SIGNAL(playStopChanged()),
            this, SLOT(playStopGuiDocChanged()));

    disconnect(_pPlayStopAnimate, SIGNAL(toggled(bool)), this, SLOT(playStop2(bool)));

    if (_pGDoc->isAnimated() == true)
    {
        _pPlayStopAnimate->setChecked(false);
    }
    else
    {
        _pPlayStopAnimate->setChecked(true);
    }

    connect(_pPlayStopAnimate, SIGNAL(toggled(bool)), this, SLOT(playStop2(bool)));

    connect(_pGDoc.get(), SIGNAL(playStopChanged()),
            this, SLOT(playStopGuiDocChanged()));
}
//-----------------------------------------------------------------------------
void GuiDocumentAnimate::playStop2(bool toggled)
{
    disconnect(_pGDoc.get(), SIGNAL(playStopChanged()),
            this, SLOT(playStopGuiDocChanged()));

    disconnect(_pPlayStopAnimate, SIGNAL(toggled(bool)), this, SLOT(playStop2(bool)));

    if (toggled == true)
    {
        _pGDoc->stopAnimate();
    }
    else
    {
        _pGDoc->startAnimate();
    }

    connect(_pPlayStopAnimate, SIGNAL(toggled(bool)), this, SLOT(playStop2(bool)));
    connect(_pGDoc.get(), SIGNAL(playStopChanged()),
            this, SLOT(playStopGuiDocChanged()));
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace EPI

