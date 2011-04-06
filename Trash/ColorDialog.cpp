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
#include "ColorDialog.h"

#include <QtToolbox/ColorPicker/ColorPicker.h>
#include <QtToolbox/ColorPicker/qtcolortriangle.h>
#include <Core/String.h>

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QLabel>

namespace QtToolbox
{

ColorDialog::ColorDialog(QWidget * parent, const QColor & color)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _color(color),
    _layout(NULL),
    _buttonBox(NULL),
    _colorPicker(NULL),
    _colorTriangle(NULL),
    _r(NULL),
    _g(NULL),
    _b(NULL),
    _a(NULL)
{
    setupUi();
}

ColorDialog::~ColorDialog()
{
    if(_layout != NULL)
        delete _layout;
    if(_buttonBox != NULL)
        delete _buttonBox;
    if(_colorPicker !=NULL)
        delete _colorPicker;
    if(_colorTriangle != NULL)
        delete _colorTriangle;
    if(_r != NULL)
        delete _r;
    if(_g != NULL)
        delete _g;
    if(_b != NULL)
        delete _b;
    if(_a != NULL)
        delete _a;

    _layout = NULL;
    _buttonBox = NULL;
    _colorTriangle = NULL;
    _r = NULL;
    _g = NULL;
    _b = NULL;
    _a = NULL;
}

void ColorDialog::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("ColorDialog"));
    this->setWindowTitle("Color chooser");
    this->setFixedWidth(392);
    this->setFixedHeight(278);

    _layout = new QGridLayout();
    _layout->setObjectName(QString::fromUtf8("gridLayout"));
    _layout->setContentsMargins(5, 5, 5, 5);

    _colorPicker = new ColorPicker(this, _color);
    _layout->addWidget(_colorPicker, 0, 0, 3, 1);

    int r, g, b, a;
    _color.getRgb(&r, &g, &b, &a);

    _colorTriangle = new QtColorTriangle(this);
    _layout->addWidget(_colorTriangle, 0, 1, 3, 1);
    _colorTriangle->setColor(_color);
    _colorTriangle->setFixedSize(200, 200);

    Core::String component;
    component = L"Red :\t";
    component << r;
    _r = new QLabel(Core::String8(component).c_str(), this);
    component = L"Green :\t";
    component << g;
    _g = new QLabel(Core::String8(component).c_str(), this);
    component = L"Blue :\t";
    component << g;
    _b = new QLabel(Core::String8(component).c_str(), this);

    _layout->addWidget(_r, 0, 2, 1, 1);
    _layout->addWidget(_g, 1, 2, 1, 1);
    _layout->addWidget(_b, 2, 2, 1, 1);

    _a = new SingleSlidingInt(this, "Alpha", 0, 255);
    _a->setSingleValue(a);
    _layout->addWidget(_a, 3, 0, 1, 3);


    _buttonBox = new QDialogButtonBox ( QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                        Qt::Horizontal,
                                        this );
    _layout->addWidget(_buttonBox, 4, 1, 1, 2);

    setLayout(_layout);

    connect(_colorTriangle, SIGNAL(colorChanged(const QColor &)), this, SLOT(colorUpdated()));
    connect(_a, SIGNAL(valueUpdated()), this, SLOT(colorUpdated()));

    connect(_colorPicker,   SIGNAL(colorChanged(float, float, float, float)),
            this,           SLOT(changeColor(float, float, float, float)));

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void ColorDialog::colorUpdated()
{
    int alpha;
    _a->getSingleValue(alpha);

    QColor color = _colorTriangle->color();

    color.setAlpha(alpha);

    int r, g, b;
    color.getRgb(&r, &g, &b);
    Core::String component;
    component = L"Red :\t";
    component << r;
    _r->setText(Core::String8(component).c_str());
    component = L"Green :\t";
    component << g;
    _g->setText(Core::String8(component).c_str());
    component = L"Blue :\t";
    component << g;
    _b->setText(Core::String8(component).c_str());

    _colorPicker->setColor(color);
    double rf, gf, bf, af;
    color.getRgbF(&rf, &gf, &bf, &af);

    emit colorChanged(rf, gf, bf, af);
}

void ColorDialog::changeColor(float rf, float gf, float bf, float af)
{
    QColor color;
    color.setRgbF(rf, gf, bf, af);
    int r, g, b, a;
    color.getRgb(&r, &g, &b, &a);

    Core::String component;
    component = L"Red :\t";
    component << r;
    _r->setText(Core::String8(component).c_str());
    component = L"Green :\t";
    component << g;
    _g->setText(Core::String8(component).c_str());
    component = L"Blue :\t";
    component << g;
    _b->setText(Core::String8(component).c_str());

    _colorTriangle->setColor(color);
    _a->setSingleValue(a);
}

}//namespace QtToolbox
