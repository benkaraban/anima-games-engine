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
#include "HueSaturationWidget.moc.h"

#include <QPainter>
#include <QMouseEvent>

namespace QtToolbox
{

HueSaturationWidget::HueSaturationWidget(   QWidget *parent,
                                            long hueWidth,
                                            long satHeight,
                                            const QColor & color)
:   QWidget(parent),
    _selection(0, 0),
    _value(color.valueF()),
    _hueWidth(hueWidth),
    _satHeight(satHeight),
    _background(hueWidth, _satHeight, QImage::Format_RGB32),
    _mustGenerateBackground(true),
    _isPressed(false)
{
    setSelectionFromColor(color);
    setupUi();
}

HueSaturationWidget::~HueSaturationWidget()
{
}

void HueSaturationWidget::setupUi()
{
    this->setFixedWidth(_hueWidth);
    this->setFixedHeight(_satHeight);
}

void HueSaturationWidget::generateBackground()
{    
    double hueStep = 1.0 / _hueWidth;
    double satStep = 1.0 / _satHeight;

    QPainter bgPainter(&_background);
    bgPainter.fillRect(_background.rect(), palette().mid());
    
    QColor tempColor;

    for(long sat = 0; sat < _satHeight; ++sat)
    {
        QRgb * rgb = (QRgb *)_background.scanLine(sat);
        
        for(long hue = 0; hue<_hueWidth; ++hue, ++rgb)
        {
            tempColor.setHsvF(hue*hueStep, (_satHeight - sat)*satStep, _value);
            *rgb = tempColor.rgb();
        }
    }
}

void HueSaturationWidget::paintEvent(QPaintEvent * paintEvent)
{
    if(_mustGenerateBackground)
    {
        _mustGenerateBackground = false;
        generateBackground();
    }
    
    //On convertit l'image en pixmap
    QPixmap pix = QPixmap::fromImage(_background);
    //On dessine le rectangle dedans
    QPainter pixPainter(&pix);
    pixPainter.setPen(Qt::black);
    
    pixPainter.drawRect( _selection.x() -1, _selection.y() - 1, 2, 2);

    //On recopie la pixmap dans le widget
    QPainter hueSatPainter(this);
    hueSatPainter.drawPixmap(contentsRect().topLeft(), pix);
}

void HueSaturationWidget::handleMouseEvent(int mouseX, int mouseY)
{
    if(mouseX <= 0)
        _selection.setX(0);
    else if(mouseX >= _hueWidth)
        _selection.setX(_hueWidth);
    else
        _selection.setX(mouseX);

    if(mouseY <= 0)
        _selection.setY(0);
    else if(mouseY >= _satHeight)
        _selection.setY(_satHeight);
    else
        _selection.setY(mouseY);

    update();

    float hue = 0.0;
    float saturation = 0.0;

    getHueSat(hue, saturation);

    emit hueSaturationUpdated(hue, saturation);
}


void HueSaturationWidget::mousePressEvent(QMouseEvent * mouseEvent)
{
    if (mouseEvent->button() & Qt::LeftButton)
    {
        _isPressed = true;
        emit signalWidgetHeld();

        handleMouseEvent(mouseEvent->pos().x(), mouseEvent->pos().y());
    }
  
    QWidget::mousePressEvent(mouseEvent);
}

void HueSaturationWidget::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::RightButton)    //l'evenement est declenche par le bouton droit
    {
        if (mouseEvent->buttons() & Qt::LeftButton)     //lors de l'evenement le bouton gauche est enfonce
        {
            _isPressed = false;
        }
    }

    if (mouseEvent->button() & Qt::LeftButton)
    {
        if (_isPressed == true)
        {
            _isPressed = false;
            emit signalWidgetReleased();
        }
    }

    QWidget::mouseReleaseEvent(mouseEvent);
}

void HueSaturationWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() && Qt::LeftButton && _isPressed == true)
    {
        handleMouseEvent(mouseEvent->pos().x(), mouseEvent->pos().y());
    }

    QWidget::mouseMoveEvent(mouseEvent);
}

void HueSaturationWidget::updateColor(const QColor & color)
{
    if(color.valueF() != _value)
    {
        _value = color.valueF();
        _mustGenerateBackground = true;
    }
    
    setSelectionFromColor(color);

    update();
}

void HueSaturationWidget::updateValue(float value)
{
    if(value != _value)
    {
        _value = value;
        _mustGenerateBackground = true;
        
        update();
    }
}

void HueSaturationWidget::setSelectionFromColor(const QColor & color)
{
    //Note : la teinte (hue) est dans l'interval 0-360 et la saturation dans 0-255.
    int X = ((color.hue() * _hueWidth)/360);
    int Y = _satHeight - ((color.saturation() * _satHeight)/255);
    _selection = QPoint(X, Y);
}

void HueSaturationWidget::getHueSat(float & hueF, float & satF)
{
    //Note : la teinte (hue) est dans l'interval 0-360 et la saturation dans 0-255.
    int hue = (_selection.x() * 360)/_hueWidth;
    int sat = ((_selection.y() - _satHeight)*255/_satHeight) *-1;
    QColor color;
    color.setHsv(hue, sat, _value*255);

    hueF = color.hueF();
    satF = color.saturationF();
}

}//namespace QtToolbox
