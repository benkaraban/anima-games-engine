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
#ifndef QTTOOLBOX_PAD2D_H_
#define QTTOOLBOX_PAD2D_H_

#include <QWidget>

namespace QtToolbox
{

class LM_API_QTB Pad2D : public QWidget
{
    Q_OBJECT
public:
    Pad2D(  QWidget * parent,
            long width, long height,
            double xMin,double xMax,
            double yMin,double yMax,
            double x,   double y);
    virtual ~Pad2D();

    void setupUi(long width, long height);

    void getValuesFromSelection(double & X, double & Y);
    void updateValues(double X, double Y);

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);
    virtual void mousePressEvent(QMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent * mouseEvent);

signals:
    void valueUpdated(double x, double y);
    void signalWidgetHeld();
    void signalWidgetReleased();


private:
    void setSelectionFromValues(double x, double y);
    void handleMouseEvent(int x, int y);

private:
    long    _width;
    long    _height;
    double  _xMin;
    double  _xMax;
    double  _yMin;
    double  _yMax;
    QPoint  _selection;
    QImage  _background;

    bool    _isPressed;
};

}//namespace QtToolbox

#endif/*QTTOOLBOX_PAD2D_H_*/