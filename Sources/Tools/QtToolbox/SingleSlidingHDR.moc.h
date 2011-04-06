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
#ifndef SINGLE_SLIDING_VALUE_HDR_H_
#define SINGLE_SLIDING_VALUE_HDR_H_

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QKeyEvent>

#include <Core/Standard.h>
#include <Core/Math/MathTools.h>

class QComboBox;

namespace QtToolbox
{
class LM_API_QTB SingleSlidingHDR : public QWidget
{
    Q_OBJECT
public:
    SingleSlidingHDR(QWidget *parent = 0,
                     const QString & name = "",
                     bool allowNegative = true);

    virtual ~SingleSlidingHDR();

    virtual void setupUi();
    bool eventFilter( QObject *dist, QEvent *event );

    virtual void getSingleValue(float  & value);
    virtual void getSingleValue(double & value);
    virtual void setSingleValue(double   value);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    
    void internalUpdateScaleIfNeeded(double value);
    void internalSetScale(int32 index);
    void internalSetText(double value);
    void internalSetSlider(double value);

protected Q_SLOTS:
    virtual void valueChanged(int value);
    virtual void editingFinished();

private Q_SLOTS:
    void sliderHolding();
    void sliderReleasing();
    void textChanged(const QString & text);
    void scaleChanged(int index);

Q_SIGNALS:
    void valueUpdated();
    void signalSliderHeld();
    void signalSliderReleased();
   
protected:
    QGridLayout *	_gridLayout;
    QLabel *		_nameLabel;
    QLineEdit *		_value;
    QLabel *		_minValueLabel;
    QLabel *		_maxValueLabel;
    QSlider *       _slider;
    QComboBox *     _combo;

    QString         _name;
    double          _min;
    double          _max;
    bool            _isValueChanged;
    bool            _isAutoScaleNeeded;
    bool            _allowNegative;
};

}//namespace QtToolbox

#endif /*SINGLE_SLIDING_VALUE_H_*/