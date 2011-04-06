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
#include "SpinBoxVector.moc.h"

#include <QGridLayout>
#include <QLabel>

#include <QtToolbox/SpinBox.moc.h>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
SpinBoxVector3::SpinBoxVector3( QWidget *parent, const QString & name, Core::String textX, Core::String textY, Core::String textZ):
    QWidget(parent),
    _pGridLayout(NULL),
    _pSpinBoxX(NULL),
    _pSpinBoxY(NULL),
    _pSpinBoxZ(NULL),
    _name(name),
    _textX(textX),
    _textY(textY),
    _textZ(textZ)
{
    setupUi();
}
//-----------------------------------------------------------------------------
SpinBoxVector3::~SpinBoxVector3()
{
    delete _pGridLayout;
    delete _pSpinBoxX;
    delete _pSpinBoxY;
    delete _pSpinBoxZ;
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setupUi()
{
    _pGridLayout = new QGridLayout();
    _pGridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _pGridLayout->setContentsMargins(0, 0, 0, 0);
    _pGridLayout->setSpacing(0);

    _pSpinBoxX = new SpinBox(this);
    _pSpinBoxY = new SpinBox(this);
    _pSpinBoxZ = new SpinBox(this);

    _pSpinBoxX->setPrefix(Core::String8(_textX).c_str());
    _pSpinBoxY->setPrefix(Core::String8(_textY).c_str());
    _pSpinBoxZ->setPrefix(Core::String8(_textZ).c_str());

    int height = 60;

    if(_name != "")
    {
        _labelTitle = new QLabel(this);
        _labelTitle->setText(_name);
        
        _pGridLayout->addWidget(_labelTitle);
        height = 80;
    }

    _pGridLayout->addWidget(_pSpinBoxX);
    _pGridLayout->addWidget(_pSpinBoxY);
    _pGridLayout->addWidget(_pSpinBoxZ);

    setFixedSize(120, height);

    setLayout(_pGridLayout);

    connect (_pSpinBoxX, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_pSpinBoxX, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));
    connect (_pSpinBoxY, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_pSpinBoxY, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));
    connect (_pSpinBoxZ, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_pSpinBoxZ, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));
    connect (_pSpinBoxX, SIGNAL(signalWidgetUpdated()),
            this, SLOT(spinBoxXChanged()));
    connect (_pSpinBoxY, SIGNAL(signalWidgetUpdated()),
            this, SLOT(spinBoxYChanged()));
    connect (_pSpinBoxZ, SIGNAL(signalWidgetUpdated()),
            this, SLOT(spinBoxZChanged()));
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setValue(const Core::Vector3f& value)
{
    _pSpinBoxX->setValue(value.x);
    _pSpinBoxY->setValue(value.y); 
    _pSpinBoxZ->setValue(value.z);

    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::getValue(Core::Vector3f& value) const
{
    value = Core::Vector3f( (float)_pSpinBoxX->value(),
                            (float)_pSpinBoxY->value(), 
                            (float)_pSpinBoxZ->value());
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::spinBoxXChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::spinBoxYChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::spinBoxZChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setSuffix(const Core::String& suffix)
{
    if (_pSpinBoxX!= NULL) _pSpinBoxX->setSuffix(Core::String8(suffix).c_str());
    if (_pSpinBoxY!= NULL) _pSpinBoxY->setSuffix(Core::String8(suffix).c_str());
    if (_pSpinBoxZ!= NULL) _pSpinBoxZ->setSuffix(Core::String8(suffix).c_str());
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setSuffix(const char* suffix)
{
    if (_pSpinBoxX!= NULL) _pSpinBoxX->setSuffix(suffix);
    if (_pSpinBoxY!= NULL) _pSpinBoxY->setSuffix(suffix);
    if (_pSpinBoxZ!= NULL) _pSpinBoxZ->setSuffix(suffix);
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setRange(double min, double max)
{
    if (_pSpinBoxX!= NULL) _pSpinBoxX->setRange(min, max);
    if (_pSpinBoxY!= NULL) _pSpinBoxY->setRange(min, max);
    if (_pSpinBoxZ!= NULL) _pSpinBoxZ->setRange(min, max);
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::setSingleStep(double step)
{
    if (_pSpinBoxX!= NULL) _pSpinBoxX->setSingleStep(step);
    if (_pSpinBoxY!= NULL) _pSpinBoxY->setSingleStep(step);
    if (_pSpinBoxZ!= NULL) _pSpinBoxZ->setSingleStep(step);
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::widgetHolding()
{
    emit signalWidgetHeld();
}
//-----------------------------------------------------------------------------
void SpinBoxVector3::widgetReleasing()
{
    emit signalWidgetReleased();
}
//-----------------------------------------------------------------------------
} //namespace QtToolbox
