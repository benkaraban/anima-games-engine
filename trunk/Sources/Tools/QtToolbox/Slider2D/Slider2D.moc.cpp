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
#include "Slider2D.moc.h"

#include <QGridLayout>
#include <QGroupBox>
#include <QVBoxLayout>

#include <QtToolbox/LineEdit.moc.h>
#include <QtToolbox/Slider2D/Pad2D.moc.h>

namespace QtToolbox
{

Slider2D::Slider2D(   QWidget * parent,
                const QString & name,
                const QString & xName,
                const QString & yName,
                long padWidth, long padHeight,
                double xMin,double xMax,
                double yMin,double yMax,
                double x,   double y)
:   _name(name),
    _xName(xName),
    _yName(yName),
    _xLineEdit(NULL),
    _yLineEdit(NULL),
    _pad2D(NULL),
    _gridLayout(NULL),
    _groupBox(NULL),
    _layout(NULL),
    _padWidth(padWidth),
    _padHeight(padHeight),
    _xMin(xMin),
    _xMax(xMax),
    _yMin(yMin),
    _yMax(yMax),
    _x(x),
    _y(y)
{
    setupUi();
}

Slider2D::~Slider2D()
{
    if(_xLineEdit != NULL)
        delete _xLineEdit;
    if(_yLineEdit != NULL)
        delete _yLineEdit;
    if(_pad2D != NULL)
        delete _pad2D;
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_groupBox != NULL)
        delete _groupBox;
    if(_layout != NULL)
        delete _layout;

    _xLineEdit = NULL;
    _yLineEdit = NULL;
    _pad2D = NULL;
    _gridLayout = NULL;
    _groupBox = NULL;
    _layout = NULL;

}

void Slider2D::setupUi()
{
    _pad2D = new Pad2D(this, _padWidth, _padHeight, _xMin, _xMax, _yMin, _yMax, _x, _y);
    _xLineEdit = new LineEdit(this, _xName, false, QString::number(_x), 70);
    _xLineEdit->setLabelColor(Qt::red);
    _yLineEdit = new LineEdit(this, _yName, false, QString::number(_y), 70);
    _yLineEdit->setLabelColor(Qt::blue);
    _xLineEdit->setValidator(_xMin, _xMax, 5);
    _yLineEdit->setValidator(_yMin, _yMax, 5);

    _gridLayout = new QGridLayout();
    _gridLayout->setContentsMargins(0, 0, 0, 0);
    _gridLayout->setSpacing(1);

    _gridLayout->addWidget(_pad2D, 0, 0, 2, 1);
    _gridLayout->addWidget(_xLineEdit, 0, 1, 1, 1, Qt::AlignBottom);
    _gridLayout->addWidget(_yLineEdit, 1, 1, 1, 1, Qt::AlignTop);

    _groupBox = new QGroupBox(_name, this);
    _groupBox->setLayout(_gridLayout);

    _layout = new QVBoxLayout();
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->addWidget(_groupBox);
    this->setLayout(_layout);

    this->setFixedWidth(_pad2D->width() + _xLineEdit->width() + 5);
    this->setFixedHeight(std::max(_pad2D->height(), 2*_xLineEdit->height()) + 17);

    connect (_pad2D, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_pad2D, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));
    connect (_pad2D, SIGNAL(valueUpdated(double, double)), 
            this, SLOT(valueChanged(double, double)));
    connect (_xLineEdit, SIGNAL(valueUpdated()), 
            this, SLOT(lineEdited()));
    connect (_yLineEdit, SIGNAL(valueUpdated()), 
            this, SLOT(lineEdited()));
}

void Slider2D::valueChanged(double x, double y)
{
    _x = x;
    _y = y;

    _xLineEdit->setText(QString::number(_x));
    _yLineEdit->setText(QString::number(_y));

    emit valueUpdated(_x, _y);
}

void Slider2D::lineEdited()
{
    _x = _xLineEdit->text().toDouble();
    _y = _yLineEdit->text().toDouble();

    _pad2D->updateValues(_x, _y);

    emit valueUpdated(_x, _y);
}

void Slider2D::setValue(double x, double y)
{
    _x = x;
    _y = y;

    _xLineEdit->setText(QString::number(_x));
    _yLineEdit->setText(QString::number(_y));

    _pad2D->updateValues(_x, _y);
}

void Slider2D::getValue(double & x, double & y)
{
    x = _x;
    y = _y;
}


void Slider2D::widgetHolding()
{
    emit signalWidgetHeld();
}

void Slider2D::widgetReleasing()
{
    emit signalWidgetReleased();
}

}//namespace QtToolbox