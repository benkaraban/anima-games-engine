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
#ifndef COLOR_DIALOG_H_
#define COLOR_DIALOG_H_

#include <QDialog>

#include <QtToolbox/SingleSlidingValue.h>

class QGridLayout;
class QDialogButtonBox;
class QLabel;
class QtColorTriangle;

namespace QtToolbox
{

class ColorPicker;

class LM_API_QTB ColorDialog : public QDialog
{
    Q_OBJECT
public:
    ColorDialog(QWidget * parent = 0, const QColor & color = Qt::white);
    virtual ~ColorDialog();

    virtual void setupUi();

signals:
    void colorChanged(float r, float g, float b, float a);

protected slots:
    void colorUpdated();
    void changeColor(float r, float g, float b, float a);

private:
    QColor _color;
    QGridLayout *       _layout;
    QDialogButtonBox *  _buttonBox;

    ColorPicker *       _colorPicker;
    QtColorTriangle *   _colorTriangle;
    QLabel *            _r;
    QLabel *            _g;
    QLabel *            _b;

    SingleSlidingInt *  _a;
};

}//namespace QtToolbox

#endif/*COLOR_DIALOG_H_*/
