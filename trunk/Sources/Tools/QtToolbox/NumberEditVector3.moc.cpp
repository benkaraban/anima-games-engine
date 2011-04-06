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
#include "NumberEditVector3.moc.h"

#include <QtToolbox/NumberEdit.moc.h>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
NumberEditVector3::NumberEditVector3(QWidget* parent, const QString & name, const QString& unit, const QString& xLabel, const QString& yLabel, const QString& zLabel) :
    QWidget(parent),
    _xLabel(xLabel),
    _yLabel(yLabel),
    _zLabel(zLabel),
    _name(name),
    _unit(unit)
{
    setupUi();
}
//-----------------------------------------------------------------------------
NumberEditVector3::~NumberEditVector3()
{
}
//-----------------------------------------------------------------------------
void NumberEditVector3::setupUi()
{
    _pGridLayout = new QGridLayout();
    _pGridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _pGridLayout->setContentsMargins(0, 0, 0, 0);
    _pGridLayout->setSpacing(0);

    int height = 60;

    if(_name != "")
    {
        _labelTitle = new QLabel(this);
        _labelTitle->setText(_name);
        
        _pGridLayout->addWidget(_labelTitle);
        height = 80;
    }

    _pSpinBoxX = new NumberEdit(_xLabel, _unit, this);
    _pSpinBoxY = new NumberEdit(_yLabel, _unit, this);
    _pSpinBoxZ = new NumberEdit(_zLabel, _unit, this);

    _pGridLayout->addWidget(_pSpinBoxX);
    _pGridLayout->addWidget(_pSpinBoxY);
    _pGridLayout->addWidget(_pSpinBoxZ);

   // setFixedSize(120, height);

   /* setTabOrder(_pSpinBoxX, _pSpinBoxY);
    setTabOrder(_pSpinBoxY, _pSpinBoxZ);*/
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
void NumberEditVector3::spinBoxXChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::spinBoxYChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::spinBoxZChanged()
{
    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::widgetHolding()
{
    emit signalWidgetHeld();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::widgetReleasing()
{
    emit signalWidgetReleased();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::setValue(const Core::Vector3f& value)
{
    _pSpinBoxX->setValue(value.x);
    _pSpinBoxY->setValue(value.y); 
    _pSpinBoxZ->setValue(value.z);

    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void NumberEditVector3::getValue(Core::Vector3f& value) const
{
    value = Core::Vector3f( (float)_pSpinBoxX->getValue(),
                            (float)_pSpinBoxY->getValue(), 
                            (float)_pSpinBoxZ->getValue());
}
//-----------------------------------------------------------------------------
} //namespace EPI

