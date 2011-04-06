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
#ifndef HUE_SATURATION_WIDGET_H_
#define HUE_SATURATION_WIDGET_H_

#include <QWidget>

namespace QtToolbox
{

class LM_API_QTB HueSaturationWidget : public QWidget
{
    Q_OBJECT
public:
    HueSaturationWidget(QWidget *parent,
                        long hueWidth,
                        long satHeight,
                        const QColor & color);
    virtual ~HueSaturationWidget();

    void setupUi();

    void getHueSat(float & hue, float & sat);
    void updateColor(const QColor & color);

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);
    virtual void mousePressEvent(QMouseEvent *mouseEvent);
    virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);
    virtual void mouseMoveEvent(QMouseEvent *mouseEvent);

public slots:
    void updateValue(float value);

signals:
    void hueSaturationUpdated(float hue, float saturation);
    void signalWidgetHeld();
    void signalWidgetReleased();

private:
    void generateBackground();
    void handleMouseEvent(int x, int y);
    void setSelectionFromColor(const QColor & color);

protected:
    QPoint          _selection;
    double          _value;

    long            _hueWidth;
    long            _satHeight;
    QImage          _background;
    bool            _mustGenerateBackground;
    bool            _isPressed;

};

}//namespace QtToolbox

#endif/*HUE_SATURATION_WIDGET_H_*/
