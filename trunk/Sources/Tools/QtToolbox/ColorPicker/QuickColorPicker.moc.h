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
#ifndef QUICK_COLOR_PICKER_H_
#define QUICK_COLOR_PICKER_H_

#include <QWidget>

#include <Core/Math/Vector.h>

class QGridLayout;
class QVBoxLayout;
class QGroupBox;
class QLabel;
class QPushButton;



namespace QtToolbox
{

class HueSaturationWidget;
class GradientWidget;

class LM_API_QTB QuickColorPicker : public QWidget
{
    Q_OBJECT
public:
    QuickColorPicker(   QWidget *parent = 0,
                        const QString & name = "",
                        const QColor & color = Qt::white,
                        bool showAlpha = true);
    virtual ~QuickColorPicker();

    void setupUi();
    void setColor(const QColor & color);

    void getColorLinear(float & r, float & g, float & b, float & a);
    void setColorLinear(float r, float g, float b, float a);
    void getColorLinear(Core::Vector4f& c) const;
    void setColorLinear(const Core::Vector4f& c);

    void getColorSRGB(float & r, float & g, float & b, float & a);
    void setColorSRGB(float r, float g, float b, float a);
    void getColorSRGB(Core::Vector4f& c) const;
    void setColorSRGB(const Core::Vector4f& c);

private Q_SLOTS:
    void valueChanged(float value);
    void alphaChanged(float value);
    void hueSatChanged(float hue, float sat);
    void openQColorDialog();
    void widgetHolding();
    void widgetReleasing();

Q_SIGNALS:
    void colorUpdated(float r, float g, float b, float a);
    void signalWidgetHeld();
    void signalWidgetReleased();

private:
    void setTextFromColor();

protected:
    QString            _name;

    QLabel              *   _r;
    QLabel              *   _g;
    QLabel              *   _b;
    QLabel              *   _a;
    QPushButton         *   _preview;
    HueSaturationWidget *   _hueSaturation;
    GradientWidget      *   _value;
    GradientWidget      *   _alpha;
    QGridLayout         *   _gridLayout;
    QGroupBox           *   _groupBox;
    QVBoxLayout         *   _layout;
    QColor                  _color;

    long            _width;
    long            _height;
    bool            _showAlpha;
};

}//namespace QtToolbox

#endif/*QUICK_COLOR_PICKER_H_*/
