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
#ifndef GRADIENT_WIDGET_H_
#define GRADIENT_WIDGET_H_

#include <QWidget>

namespace QtToolbox
{

class LM_API_QTB GradientWidget : public QWidget
{
    Q_OBJECT
public:
    GradientWidget( QWidget *parent,
                    long width,
                    long height,
                    bool alphaGradient,
                    const QColor & color);
    virtual ~GradientWidget();

    void setupUi();

    void updateColor(const QColor & color);

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);
    virtual void mousePressEvent(QMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent);

public slots:
    void updateHueSaturation(float hue, float saturation);

signals:
    void valueUpdated(float value);
    void signalWidgetHeld();
    void signalWidgetReleased();

private:
    void generateBackground();
    void handleMouseEvent(int x);
    void setSelectionFromColor(const QColor & color);

protected:
    int             _selection;
    QColor          _color;

    long            _width;
    long            _height;
    bool            _alphaGradient;
    QImage          _background;
    bool            _mustGenerateBackground;
    bool            _isPressed;
};

}//namespace QtToolbox

#endif/*GRADIENT_WIDGET_H_*/
