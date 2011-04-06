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
#include "RangedColorValue.h"
#include <Images/Color.h>

#include <iostream>

namespace QtToolbox
{

RangedColorValue::RangedColorValue( QWidget *parent,
                                    const QString & name,
                                    float r1,
                                    float g1,
                                    float b1,
                                    float a1,
                                    float r2,
                                    float g2,
                                    float b2,
                                    float a2)
:   QWidget(parent),
    _name(name),
    _r1(r1),
    _g1(g1),
    _b1(b1),
    _a1(a1),
    _r2(r2),
    _g2(g2),
    _b2(b2),
    _a2(a2),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _color1(NULL),
    _color2(NULL)
{
    setupUi();
}

RangedColorValue::~RangedColorValue()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_color1 != NULL)
        delete _color1;
    if(_color2 != NULL)
        delete _color2;

    _gridLayout = NULL;
    _nameLabel = NULL;
    _color1 = NULL;
    _color2 = NULL;
}


void RangedColorValue::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("RangedColorValue"));
    this->setFixedWidth(170);
    this->setFixedHeight(24);

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);
    _nameLabel->setText(_name);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    QColor color;
    color.setRgbF ( _r1, _g1, _b1, _a1);
    _color1 = new AdvColorPicker(this, color);
    _color1->setObjectName(QString::fromUtf8("color1"));

    _gridLayout->addWidget(_color1, 0, 1, 1, 1);

    color.setRgbF ( _r2, _g2, _b2, _a2);
    _color2 = new AdvColorPicker(this, color);
    _color2->setObjectName(QString::fromUtf8("color2"));

    _gridLayout->addWidget(_color2, 0, 2, 1, 1);

    setLayout(_gridLayout);

    connect(_color1,    SIGNAL(colorChanged(float, float, float, float)),
            this,   SLOT(color1Changed(float, float, float, float)));
    connect(_color2,    SIGNAL(colorChanged(float, float, float, float)),
            this,   SLOT(color2Changed(float, float, float, float)));
}

void RangedColorValue::getColorsLinear(float & r1, float & g1, float & b1, float & a1,
                                       float & r2, float & g2, float & b2, float & a2)
{
    getColor1Linear(r1, g1, b1, a1);
    getColor2Linear(r2, g2, b2, a2);
}

void RangedColorValue::getColor1Linear(float & r, float & g, float & b, float & a)
{
    r = Images::SRGB2Linear(_r1);
    g = Images::SRGB2Linear(_g1);
    b = Images::SRGB2Linear(_b1);
    a = _a1;
}

void RangedColorValue::getColor2Linear(float & r, float & g, float & b, float & a)
{
    r = Images::SRGB2Linear(_r2);
    g = Images::SRGB2Linear(_g2);
    b = Images::SRGB2Linear(_b2);
    a = _a2;
}

void RangedColorValue::setColor1Linear(float r1, float g1, float b1, float a1)
{
    setColor1SRGB(Images::linear2SRGB(r1),
                  Images::linear2SRGB(g1),
                  Images::linear2SRGB(b1),
                  a1);
}

void RangedColorValue::setColor2Linear(float r2, float g2, float b2, float a2)
{
    setColor2SRGB(Images::linear2SRGB(r2),
                  Images::linear2SRGB(g2),
                  Images::linear2SRGB(b2),
                  a2);
}

void RangedColorValue::getColorsSRGB(float & r1, float & g1, float & b1, float & a1,
                                     float & r2, float & g2, float & b2, float & a2)
{
    getColor1SRGB(r1, g1, b1, a1);
    getColor2SRGB(r2, g2, b2, a2);
}

void RangedColorValue::getColor1SRGB(float & r, float & g, float & b, float & a)
{
    r = _r1;
    g = _g1;
    b = _b1;
    a = _a1;
}

void RangedColorValue::getColor2SRGB(float & r, float & g, float & b, float & a)
{
    r = _r2;
    g = _g2;
    b = _b2;
    a = _a2;
}

void RangedColorValue::color1Changed(float r, float g, float b, float a)
{
    setColor1SRGB(r, g, b, a);
}

void RangedColorValue::color2Changed(float r, float g, float b, float a)
{
    setColor2SRGB(r, g, b, a);
}

void RangedColorValue::setColor1SRGB(float r1, float g1, float b1, float a1)
{
    _r1 = r1; _g1 = g1; _b1 = b1; _a1 = a1;

    QColor color;
    color.setRgbF ( _r1, _g1, _b1, _a1);
    _color1->setColor(color);

    emit colorsUpdated();
}

void RangedColorValue::setColor2SRGB(float r2, float g2, float b2, float a2)
{
    _r2 = r2; _g2 = g2; _b2 = b2; _a2 = a2;

    QColor color;
    color.setRgbF ( _r2, _g2, _b2, _a2);
    _color2->setColor(color);

    emit colorsUpdated();
}

}//namespace QtToolbox
