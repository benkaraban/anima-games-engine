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
#include "GradientWidget.moc.h"

#include <QPainter>
#include <QMouseEvent>

namespace QtToolbox
{

GradientWidget::GradientWidget( QWidget *parent,
                                long width,
                                long height,
                                bool alphaGradient,
                                const QColor & color)
:   QWidget(parent),
    _selection(0),
    _color(color),
    _width(width),
    _height(height),
    _alphaGradient(alphaGradient),
    _background(width, height, QImage::Format_RGB32),
    _mustGenerateBackground(true),
    _isPressed(false)
{
    setSelectionFromColor(color);
    setupUi();
}

GradientWidget::~GradientWidget()
{
}

void GradientWidget::setupUi()
{
    this->setFixedWidth(_width);
    this->setFixedHeight(_height);
}

void GradientWidget::generateBackground()
{    
    QPainter bgPainter(&_background);
    
    QColor tempColor;
    if(_width > _height)//horizontal gradient
    {
        double valueStep = 1.0 / _width;
        for(long val = 0; val <= _width; ++val)
        {
            tempColor.setHsvF(_color.hueF(), _color.saturationF(), val*valueStep);
            bgPainter.setPen(tempColor);
            bgPainter.drawLine(val, 0, val, _height);
        }
    }
    else//vertical gradient
    {
        double valueStep = 1.0 / _height;
        for(long val = 0; val <= _height; ++val)
        {
            tempColor.setHsvF(_color.hueF(), _color.saturationF(), val*valueStep);
            bgPainter.setPen(tempColor);
            bgPainter.drawLine(0, _height-val, _width, _height-val);
        } 
    }
}

void GradientWidget::paintEvent(QPaintEvent * paintEvent)
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
    
    if(_width > _height)//horizontal gradient
    {
        pixPainter.drawLine(_selection -1, 0, _selection -1, _height-1);
        pixPainter.drawLine(_selection +1, 0, _selection +1, _height-1);
    }
    else
    {
        pixPainter.drawLine(0, _height-(_selection -1), _width-1, _height-(_selection -1));
        pixPainter.drawLine(0, _height-(_selection +1), _width-1, _height-(_selection +1));
    }

    //On recopie la pixmap dans le widget
    QPainter hueSatPainter(this);
    hueSatPainter.drawPixmap(contentsRect().topLeft(), pix);
}

void GradientWidget::handleMouseEvent(int mouse)
{
    float valueF = 0.0;

    if(_width > _height)//horizontal gradient
    {
        if(mouse <= 0)
            _selection = 0;
        else if(mouse >= _width)
            _selection = _width;
        else
            _selection = mouse;
            
        update();

        int value = _selection * 255 / _width;
        valueF = value / 255.0;
    }
    else//vertical gradient
    {
        if(mouse <= 0)
            _selection = 0;
        else if(mouse >= _height)
            _selection = _height;
        else
            _selection = mouse;
            
        update();

        int value = _selection * 255 / _height;
        valueF = value / 255.0;
    }

    emit valueUpdated(valueF);
}

void GradientWidget::mousePressEvent(QMouseEvent * mouseEvent)
{
    if (mouseEvent->button() & Qt::LeftButton)
    {
        _isPressed = true;
        emit signalWidgetHeld();

        if(_width > _height)//horizontal gradient
            handleMouseEvent(mouseEvent->pos().x());
        else//vertical gradient
            handleMouseEvent(_height-mouseEvent->pos().y());
    }
 
    QWidget::mousePressEvent(mouseEvent);
}

void GradientWidget::mouseReleaseEvent(QMouseEvent * mouseEvent)
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

void GradientWidget::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() & Qt::LeftButton && _isPressed == true)
    {
        if(_width > _height)//horizontal gradient
            handleMouseEvent(mouseEvent->pos().x());
        else//vertical gradient
            handleMouseEvent(_height-mouseEvent->pos().y());
    }

    QWidget::mouseMoveEvent(mouseEvent);
}

void GradientWidget::updateHueSaturation(float hue, float saturation)
{
    if(_color.hueF() != hue || _color.saturationF() != saturation)
    {
        _color.setHsvF(hue, saturation, 1.0);
        _mustGenerateBackground = true;

        update();
    }
}

void GradientWidget::updateColor(const QColor & color)
{
    if(!_alphaGradient)
    {
        _mustGenerateBackground = true;
        _color = color;
    }

    setSelectionFromColor(color);

    update();
}

void GradientWidget::setSelectionFromColor(const QColor & color)
{
    if(_alphaGradient)
    {
        if(_width > _height)//horizontal gradient
            //Note : l'alpha est dans l'interval 0-255
            _selection = ((color.alpha() * _width)/255);
        else//vertical gradient
            //Note : l'alpha est dans l'interval 0-255
            _selection = ((color.alpha() * _height)/255);
    }
    else
    {
        if(_width > _height)//horizontal gradient
            //Note : l'alpha est dans l'interval 0-255
            _selection = ((color.value() * _width)/255);
        else//vertical gradient
            //Note : l'alpha est dans l'interval 0-255
            _selection = ((color.value() * _height)/255);
    }
}

}//namespace QtToolbox
