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
#ifndef RANGED_SLIDING_VALUE_HDR_H_
#define RANGED_SLIDING_VALUE_HDR_H_

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QKeyEvent>

#include <QtToolbox/SpanSlider/qxtspanslider.moc.h>

#include <Core/Standard.h>
#include <Core/Math/MathTools.h>

class QComboBox;

namespace QtToolbox
{
class LM_API_QTB RangedSlidingHDR : public QWidget
{
    Q_OBJECT
public:
    RangedSlidingHDR(QWidget *parent = 0,
                     const QString & name = "",
                     bool allowNegative = true);

    virtual ~RangedSlidingHDR();

    virtual void setupUi();
    bool eventFilter( QObject *dist, QEvent *event );

    virtual void getRangedValue(float  & lower, float  & upper);
    virtual void getRangedValue(double & lower, double & upper);
    virtual void setRangedValue(double   lower, double   upper);

    //virtual void getRangedValue(float & value);
    //virtual void setRangedValue(float   value);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    
    void internalUpdateScaleIfNeeded(double l, double u);
    void internalSetScale(int32 index);
    void internalSetText(double l, double u);
    void internalSetSlider(double l, double u);

protected Q_SLOTS:
    virtual void spanChanged(int lower, int upper);
    virtual void editingFinished();

private Q_SLOTS:
    void rangeHolding();
    void rangeReleasing();
    void upperTextChanged(const QString & text);
    void lowerTextChanged(const QString & text);
    void scaleChanged(int index);

Q_SIGNALS:
    void rangeUpdated();
    void signalRangeHeld();
    void signalRangeReleased();
   
protected:
    QGridLayout *	_gridLayout;
    QLabel *		_nameLabel;
    QLineEdit *     _lowerValue;
    QLineEdit *     _upperValue;
    QLabel *		_minValueLabel;
    QLabel *		_maxValueLabel;
    QxtSpanSlider * _slider;
    QComboBox *     _combo;

    QString         _name;
    double          _min;
    double          _max;
    bool            _isValueChanged;
    bool            _isAutoScaleNeeded;
    bool            _allowNegative;
};

}//namespace QtToolbox

#endif /*Ranged_SLIDING_VALUE_H_*/