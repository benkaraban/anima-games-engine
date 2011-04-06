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
#include "ColorPicker.h"
#include <QColorDialog>

namespace QtToolbox
{

ColorPicker::ColorPicker(QWidget *parent, const QColor & color)
:   QPushButton(parent),
    _color(color)
{
    setupUi();
}

ColorPicker::~ColorPicker()
{}

void ColorPicker::setupUi()
{
    this->setFixedWidth(40);
    this->setFixedHeight(16);
    QPalette palette;
    palette.setColor(QPalette::Button,_color);
    this->setPalette(palette);

    this->setFlat(false);
    this->setAutoFillBackground(false);

    connect(this, SIGNAL(clicked()),
            this, SLOT(showColorDialog()));
}

void ColorPicker::setColor(const QColor & color)
{
    _color = color;
    QPalette palette;
    palette.setColor(QPalette::Button, _color);
    this->setPalette(palette);
}

void ColorPicker::showColorDialog()
{
    QColor color = QColorDialog::getColor(_color, this, "Color chooser", QColorDialog::ShowAlphaChannel);

    if(color.isValid())
    {
        _color = color;
        double rf = 0.0f;
        double gf = 0.0f;
        double bf = 0.0f;
        double af = 0.0f;
        _color.getRgbF(&rf, &gf, &bf, &af);
        QPalette palette;
        palette.setColor(QPalette::Button,_color);
        this->setPalette(palette);

        emit colorChanged(rf, gf, bf, af);
    }
}

void ColorPicker::updateColor(float r, float g, float b, float a)
{
    QColor color;
    color.setRgbF(r, g, b, a);
    setColor(color);
    double rf = 0.0f;
    double gf = 0.0f;
    double bf = 0.0f;
    double af = 0.0f;
    _color.getRgbF(&rf, &gf, &bf, &af);
    emit colorChanged(rf, gf, bf, af);
}

}//namespace QtToolbox
