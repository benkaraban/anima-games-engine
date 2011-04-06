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
#include "QuickColorPicker.moc.h"

#include <QLineEdit>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

#include <QtToolbox/ColorPicker/HueSaturationWidget.moc.h>
#include <QtToolbox/ColorPicker/GradientWidget.moc.h>

#include <Core/String.h>
#include <Core/Math/Color.h>

namespace QtToolbox
{

QuickColorPicker::QuickColorPicker(QWidget *parent, const QString & name, const QColor & color, bool showAlpha)
:   QWidget(parent),
    _name(name),
    _r(NULL),
    _g(NULL),
    _b(NULL),
    _a(NULL),
    _preview(NULL),
    _hueSaturation(NULL),
    _value(NULL),
    _alpha(NULL),
    _gridLayout(NULL),
    _groupBox(NULL),
    _layout(NULL),
    _color(color),
    _width(314),
    _height(95),
    _showAlpha(showAlpha)
{
    setupUi();
}

QuickColorPicker::~QuickColorPicker()
{
    delete _r;
    _r = NULL;
    delete _g;
    _g = NULL;
    delete _b;
    _b = NULL;
    delete _a;
    _a = NULL;
    delete _preview;
    _preview = NULL;
    delete _hueSaturation;
    _hueSaturation = NULL;
    delete _value;
    _value = NULL;
    delete _alpha;
    _alpha = NULL;
    delete _gridLayout;
    _gridLayout = NULL;
    delete _groupBox;
    _groupBox = NULL;
    delete _layout;
    _layout = NULL;
}

void QuickColorPicker::setupUi()
{
    this->setContentsMargins(0, 0, 0, 0);

    _r = new QLabel();
    _g = new QLabel();
    _b = new QLabel();
    _a = new QLabel();

    _preview = new QPushButton(this);
    _preview->setFixedWidth(40);
    _preview->setFixedHeight(16);

    setTextFromColor();

    _alpha = new GradientWidget(this, 10, 77, true, Qt::white);
    _alpha->updateColor(_color);

    _hueSaturation = new HueSaturationWidget(this, 256, 64, _color);

    _value = new GradientWidget(this, 256, 10, false, _color);

    _gridLayout = new QGridLayout();
    _gridLayout->setContentsMargins(0, 0, 0, 0);
    _gridLayout->setSpacing(1);

    _gridLayout->addWidget(_preview, 0, 0, 1, 1);
    _gridLayout->addWidget(_r, 1, 0, 1, 1);
    _gridLayout->addWidget(_g, 2, 0, 1, 1);
    _gridLayout->addWidget(_b, 3, 0, 1, 1);
    _gridLayout->addWidget(_a, 4, 0, 1, 1);
    _gridLayout->addWidget(_hueSaturation, 0, 1, 4, 1);
    _gridLayout->addWidget(_value, 4, 1, 1, 1);
    if(_showAlpha)
        _gridLayout->addWidget(_alpha, 0, 2, 5, 1);
    else
        _alpha->hide();


    _groupBox = new QGroupBox(_name, this);
    _groupBox->setLayout(_gridLayout);

    _layout = new QVBoxLayout();
    _layout->setContentsMargins(0, 0, 0, 0);
    _layout->addWidget(_groupBox);
    this->setLayout(_layout);

    if(! _showAlpha)
        _width = 304;

    this->setFixedWidth(_width);
    this->setFixedHeight(_height);

    connect(_value, SIGNAL(valueUpdated(float)),
            this, SLOT(valueChanged(float)));
    connect(_alpha, SIGNAL(valueUpdated(float)),
            this, SLOT(alphaChanged(float)));
    connect(_hueSaturation, SIGNAL(hueSaturationUpdated(float, float)),
            this, SLOT(hueSatChanged(float, float)));
    connect(_preview, SIGNAL(clicked()),
            this, SLOT(openQColorDialog()));

    connect (_alpha, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_alpha, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));

    connect (_value, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_value, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));

    connect (_hueSaturation, SIGNAL(signalWidgetHeld()),
            this, SLOT(widgetHolding()));
    connect (_hueSaturation, SIGNAL(signalWidgetReleased()),
            this, SLOT(widgetReleasing()));
}

void QuickColorPicker::setColor(const QColor & color)
{
    if(color.isValid())
    {
        _color = color;
        setTextFromColor();
        _hueSaturation->updateColor(_color);
        _value->updateColor(_color);
        _alpha->updateColor(_color);

        emit colorUpdated(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
    }
}

void QuickColorPicker::valueChanged(float value)
{
    _color.setHsvF(_color.hueF(), _color.saturationF(), value, _color.alphaF());
    _hueSaturation->updateValue(value);
    setTextFromColor();

    emit colorUpdated(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
}

void QuickColorPicker::alphaChanged(float alpha)
{
    _color.setHsvF(_color.hueF(), _color.saturationF(), _color.valueF(), alpha);
    setTextFromColor();

    emit colorUpdated(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
}

void QuickColorPicker::hueSatChanged(float hue, float sat)
{
    _color.setHsvF(hue, sat, _color.valueF(), _color.alphaF());
    _value->updateHueSaturation(hue, sat);
    setTextFromColor();

    emit colorUpdated(_color.redF(), _color.greenF(), _color.blueF(), _color.alphaF());
}

void QuickColorPicker::setTextFromColor()
{
    Core::String r = L"R : ";
    Core::String g = L"G : ";
    Core::String b = L"B : ";
    Core::String a = L"A : ";

    r += Core::toString(_color.redF(),   2, 2);
    g += Core::toString(_color.greenF(), 2, 2);
    b += Core::toString(_color.blueF(),  2, 2);
    a += Core::toString(_color.alphaF(), 2, 2);

    _r->setText(Core::String8(r).c_str());
    _g->setText(Core::String8(g).c_str());
    _b->setText(Core::String8(b).c_str());
    _a->setText(Core::String8(a).c_str());

    //Pour la couleur de fond on ne prend pas en compte l'alpha
    QColor backgroundColor = _color;
    backgroundColor.setAlpha(255);
    QPalette palette = _preview->palette();
    palette.setColor(QPalette::Button, backgroundColor);
    _preview->setPalette(palette);
}

void QuickColorPicker::openQColorDialog()
{
    QColor color;
    if(_showAlpha)
        color = QColorDialog::getColor(_color, this, "Color chooser", QColorDialog::ShowAlphaChannel);
    else
        color = QColorDialog::getColor(_color, this, "Color chooser");

    setColor(color);
}

void QuickColorPicker::getColorLinear(float & r, float & g, float & b, float & a)
{
    r = Core::SRGB2Linear((float)_color.redF());
    g = Core::SRGB2Linear((float)_color.greenF());
    b = Core::SRGB2Linear((float)_color.blueF());
    a = _color.alphaF();
}

void QuickColorPicker::getColorSRGB(float & r, float & g, float & b, float & a)
{
    r = _color.redF();
    g = _color.greenF();
    b = _color.blueF();
    a = _color.alphaF();
}

void QuickColorPicker::setColorLinear(float r, float g, float b, float a)
{
    setColorSRGB(Core::linear2SRGB(r), Core::linear2SRGB(g), Core::linear2SRGB(b), a);
}

void QuickColorPicker::getColorLinear(Core::Vector4f& c) const
{
    c.r = Core::SRGB2Linear((float)_color.redF());
    c.g = Core::SRGB2Linear((float)_color.greenF());
    c.b = Core::SRGB2Linear((float)_color.blueF());
    c.a = _color.alphaF();
}

void QuickColorPicker::getColorSRGB(Core::Vector4f& c) const
{
    c.r = _color.redF();
    c.g = _color.greenF();
    c.b = _color.blueF();
    c.a = _color.alphaF();
}

void QuickColorPicker::setColorSRGB(const Core::Vector4f& c)
{
    setColorSRGB(c.r, c.g, c.b, c.a);
}

void QuickColorPicker::setColorLinear(const Core::Vector4f& c)
{
    setColorSRGB(Core::linear2SRGB(c.r), Core::linear2SRGB(c.g), Core::linear2SRGB(c.b), c.a);
}

void QuickColorPicker::setColorSRGB(float r, float g, float b, float a)
{
    QColor color;
    color.setRgbF( r, g, b, a);
    _color = color;

    setTextFromColor();
    _hueSaturation->updateColor(_color);
    _value->updateColor(_color);
    _alpha->updateColor(_color);
}

void QuickColorPicker::widgetHolding()
{
    emit signalWidgetHeld();
}

void QuickColorPicker::widgetReleasing()
{
    emit signalWidgetReleased();
}
}//namespace QtToolbox
