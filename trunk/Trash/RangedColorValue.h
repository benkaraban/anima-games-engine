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
#ifndef RANGED_COLOR_VALUE_H_
#define RANGED_COLOR_VALUE_H_

#include <QWidget>

#include <QGridLayout>
#include <QLabel>
#include <QtToolbox/ColorPicker/AdvColorPicker.h>

#include <QResizeEvent>

namespace QtToolbox
{

class LM_API_QTB RangedColorValue : public QWidget
{
    Q_OBJECT
public:
    RangedColorValue(   QWidget *parent = 0,
                        const QString & name = "",
                        float r1 = 1.0,
                        float g1 = 1.0,
                        float b1 = 1.0,
                        float a1 = 1.0,
                        float r2 = 1.0,
                        float g2 = 1.0,
                        float b2 = 1.0,
                        float a2 = 1.0);

    virtual ~RangedColorValue();

    virtual void setupUi();

    virtual void getColorsLinear( float & r1, float & g1, float & b1, float & a1,
                                  float & r2, float & g2, float & b2, float & a2);

    virtual void getColor1Linear( float & r, float & g, float & b, float & a);
    virtual void getColor2Linear( float & r, float & g, float & b, float & a);

    virtual void setColor1Linear( float r1, float g1, float b1, float a1);
    virtual void setColor2Linear( float r2, float g2, float b2, float a2);


    virtual void getColorsSRGB( float & r1, float & g1, float & b1, float & a1,
                                float & r2, float & g2, float & b2, float & a2);

    virtual void getColor1SRGB( float & r, float & g, float & b, float & a);
    virtual void getColor2SRGB( float & r, float & g, float & b, float & a);

    virtual void setColor1SRGB( float r1, float g1, float b1, float a1);
    virtual void setColor2SRGB( float r2, float g2, float b2, float a2);

protected slots:
    virtual void color1Changed(float r, float g, float b, float a);
    virtual void color2Changed(float r, float g, float b, float a);
signals:
    void colorsUpdated();

protected:
    QString _name;
    //first color
    float   _r1;
    float   _g1;
    float   _b1;
    float   _a1;
    //second color
    float   _r2;
    float   _g2;
    float   _b2;
    float   _a2;
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    AdvColorPicker *   _color1;
    AdvColorPicker *   _color2;
};

}//namespace QtToolbox

#endif/*SINGLE_COLOR_VALUE_H_*/
