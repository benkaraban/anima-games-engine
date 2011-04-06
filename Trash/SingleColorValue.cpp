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
#include "SingleColorValue.h"
#include <Images/Color.h>

namespace QtToolbox
{

SingleColorValue::SingleColorValue( QWidget *parent,
                                    const QString & name,
                                    float r,
                                    float g,
                                    float b,
                                    float a)
:   QWidget(parent),
    _name(name),
    _r(r),
    _g(g),
    _b(b),
    _a(a),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _color(NULL)
{
    setupUi();
}

SingleColorValue::~SingleColorValue()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_color != NULL)
        delete _color;

    _gridLayout = NULL;
    _nameLabel = NULL;
    _color = NULL;
}


void SingleColorValue::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("SingleColorValue"));

	_gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);
    _nameLabel->setText(_name);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    QColor color;
    color.setRgbF ( _r, _g, _b, _a);
    _color = new AdvColorPicker(this, color);
    _color->setObjectName(QString::fromUtf8("color"));

    _gridLayout->addWidget(_color, 0, 1, 1, 1);

	this->setFixedWidth(120);
    this->setFixedHeight(16);

    setLayout(_gridLayout);

    connect(_color, SIGNAL(colorChanged(float, float, float, float)),
            this,   SLOT(colorChanged(float, float, float, float)));
}

void SingleColorValue::colorChanged(float r, float g, float b, float a)
{
    setColorSRGB(r, g, b, a);
}

void SingleColorValue::getColorLinear(float & r, float & g, float & b, float & a)
{
    r = Images::SRGB2Linear(_r);
    g = Images::SRGB2Linear(_g);
    b = Images::SRGB2Linear(_b);
    a = Images::SRGB2Linear(_a);
}

void SingleColorValue::getColorSRGB(float & r, float & g, float & b, float & a)
{
    r = _r;
    g = _g;
    b = _b;
    a = _a;
}

void SingleColorValue::setColorLinear(float r, float g, float b, float a)
{
    setColorSRGB(Images::linear2SRGB(r), Images::linear2SRGB(g), Images::linear2SRGB(b), a);
}

void SingleColorValue::setColorSRGB(float r, float g, float b, float a)
{
    _r = r;
    _g = g;
    _b = b;
    _a = a;

    QColor color;
    color.setRgbF ( _r, _g, _b, _a);
    _color->setColor(color);

    emit colorUpdated();
}

}//namespace QtToolbox
