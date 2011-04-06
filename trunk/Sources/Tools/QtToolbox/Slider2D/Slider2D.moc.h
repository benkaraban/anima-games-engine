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
#ifndef QTTOOLBOX_SLIDER2D_H_
#define QTTOOLBOX_SLIDER2D_H_

#include <QWidget>
#include <QString>

class QGridLayout;
class QGroupBox;
class QVBoxLayout;

namespace QtToolbox
{

class Pad2D;
class LineEdit;

class LM_API_QTB Slider2D : public QWidget
{
    Q_OBJECT
public:
    Slider2D(   QWidget * parent,
                const QString & name,
                const QString & xName,
                const QString & yName,
                long padWidth, long padHeight,
                double xMin,double xMax,
                double yMin,double yMax,
                double x,   double y);

    virtual ~Slider2D();

    void setupUi();

    virtual void setValue(double x, double y);
    virtual void getValue(double & x, double & y);

private Q_SLOTS:
    void valueChanged(double x, double y);
    void lineEdited();
    void widgetHolding();
    void widgetReleasing();

signals:
    void valueUpdated(double x, double y);
    void signalWidgetHeld();
    void signalWidgetReleased();

private:
    QString         _name;
    QString         _xName;
    QString         _yName;

    LineEdit    *   _xLineEdit;
    LineEdit    *   _yLineEdit;
    Pad2D       *   _pad2D;
    QGridLayout *   _gridLayout;
    QGroupBox   *   _groupBox;
    QVBoxLayout *   _layout;

    long            _padWidth;
    long            _padHeight;
    double          _xMin;
    double          _xMax;
    double          _yMin;
    double          _yMax;
    double          _x;
    double          _y;
};

}//namespace QtToolbox

#endif/*QTTOOLBOX_SLIDER2D_H_*/