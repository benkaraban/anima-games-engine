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
#include "Pad2D.moc.h"

#include <QPainter>
#include <QMouseEvent>

namespace QtToolbox
{

Pad2D::Pad2D(  QWidget * parent,
                long width, long height,
                double xMin,double xMax,
                double yMin,double yMax,
                double x,   double y)
:   _width(width),
    _height(height),
    _xMin(xMin),
    _xMax(xMax),
    _yMin(yMin),
    _yMax(yMax),
    _background(_width, _height, QImage::Format_RGB32),
    _isPressed(false)
{
    setSelectionFromValues(x, y);
    setupUi(width, height);
}

Pad2D::~Pad2D()
{}

void Pad2D::setupUi(long width, long height)
{
    setFixedWidth(width);
    setFixedHeight(height);

    QPainter bgPainter(&_background);
    bgPainter.fillRect(_background.rect(), Qt::lightGray);
}

void Pad2D::paintEvent(QPaintEvent * paintEvent)
{
    //On convertit l'image en pixmap
    QPixmap pix = QPixmap::fromImage(_background);

    //On dessine les deux lignes et les 4 pixels dedans
    QPainter pixPainter(&pix);
    pixPainter.setPen(Qt::red);
    pixPainter.drawLine(_selection.x(), 0, _selection.x(), height());
    pixPainter.setPen(Qt::blue);
    pixPainter.drawLine(0, _selection.y(), width(), _selection.y());

    pixPainter.setPen(Qt::black);

    pixPainter.drawPoint(_selection.x()-1, _selection.y());
    pixPainter.drawPoint(_selection.x()+1, _selection.y());
    pixPainter.drawPoint(_selection.x(), _selection.y()-1);
    pixPainter.drawPoint(_selection.x(), _selection.y()+1);

    //On recopie la pixmap dans le widget
    QPainter pad2DPainter(this);
    pad2DPainter.drawPixmap(contentsRect().topLeft(), pix);

}

void Pad2D::handleMouseEvent(int mouseX, int mouseY)
{
    if(mouseX <= 0)
        _selection.setX(0);
    else if(mouseX >= _width-1)
        _selection.setX(_width-1);
    else
        _selection.setX(mouseX);

    if(mouseY <= 0)
        _selection.setY(0);
    else if(mouseY >= _height-1)
        _selection.setY(_height-1);
    else
        _selection.setY(mouseY);

    update();

    double x = 0.0;
    double y = 0.0;

    getValuesFromSelection(x, y);

    emit valueUpdated(x, y);
}

void Pad2D::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() & Qt::LeftButton)
    {
        _isPressed = true;
        emit signalWidgetHeld();

        handleMouseEvent(mouseEvent->pos().x(), mouseEvent->pos().y());
    }
  
    QWidget::mousePressEvent(mouseEvent);
}


void Pad2D::mouseReleaseEvent(QMouseEvent *mouseEvent)
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

void Pad2D::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->buttons() && Qt::LeftButton && _isPressed == true)
    {
        if(mouseEvent->modifiers() == Qt::ShiftModifier)
            handleMouseEvent(mouseEvent->pos().x(), _selection.y());
        else if(mouseEvent->modifiers() == Qt::AltModifier)
            handleMouseEvent(_selection.x(), mouseEvent->pos().y());
        else
            handleMouseEvent(mouseEvent->pos().x(), mouseEvent->pos().y());
    }

    QWidget::mouseMoveEvent(mouseEvent);
}

void Pad2D::setSelectionFromValues(double x, double y)
{
    int X = ((x - _xMin) * (_width - 1))/(_xMax - _xMin);
    int Y = ((y - _yMin) * (_height - 1))/(_yMax - _yMin);

    _selection = QPoint(X, Y);
}

void Pad2D::updateValues(double X, double Y)
{
    setSelectionFromValues(X, Y);
    update();
}

void Pad2D::getValuesFromSelection(double & X, double & Y)
{
    X = _xMin + ((_selection.x() * (_xMax - _xMin)) / (_width - 1));
    Y = _yMin + ((_selection.y() * (_yMax - _yMin)) / (_height - 1));
}

}//namespace QtToolbox