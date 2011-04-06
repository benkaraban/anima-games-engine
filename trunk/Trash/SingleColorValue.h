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
#ifndef SINGLE_COLOR_VALUE_H_
#define SINGLE_COLOR_VALUE_H_

#include <QWidget>

#include <QGridLayout>
#include <QLabel>

#include <QtToolbox/ColorPicker/AdvColorPicker.h>

namespace QtToolbox
{

class LM_API_QTB SingleColorValue : public QWidget
{
    Q_OBJECT
public:
    SingleColorValue(   QWidget *parent = 0,
                        const QString & name = "",
                        float r = 1.0,
                        float g = 1.0,
                        float b = 1.0,
                        float a = 1.0);

    virtual ~SingleColorValue();

    virtual void setupUi();

    virtual void getColorLinear(float & r, float & g, float & b, float & a);
    virtual void setColorLinear(float r, float g, float b, float a);

    virtual void getColorSRGB(float & r, float & g, float & b, float & a);
    virtual void setColorSRGB(float r, float g, float b, float a);

protected slots:
    virtual void colorChanged(float r, float g, float b, float a);
signals:
    void colorUpdated();

protected:
    QString _name;
    float   _r;
    float   _g;
    float   _b;
    float   _a;
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    AdvColorPicker* _color;
};

}//namespace QtToolbox

#endif/*SINGLE_COLOR_VALUE_H_*/
