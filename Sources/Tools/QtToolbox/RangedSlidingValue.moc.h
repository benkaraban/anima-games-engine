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
#ifndef RANGED_SLIDING_VALUE_H_
#define RANGED_SLIDING_VALUE_H_
#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QtToolbox/SpanSlider/qxtspanslider.moc.h>

#include <Core/Standard.h>
#include <Core/Math/MathTools.h>


namespace QtToolbox
{
//-----------------------------------------------------------------------------
class LM_API_QTB AbstractRangedSlidingValue : public QWidget
{
    Q_OBJECT
public:
    AbstractRangedSlidingValue(QWidget *parent = 0);
    virtual ~AbstractRangedSlidingValue();

    virtual void setupUi();
    bool eventFilter( QObject *dist, QEvent *event );

protected Q_SLOTS:
    virtual void spanChanged(int lower, int upper) = 0;
    virtual void editingFinished() = 0;

private Q_SLOTS:
    void rangeHolding();
    void rangeReleasing();

Q_SIGNALS:
    void rangeUpdated();
    void signalRangeHeld();
    void signalRangeReleased();

protected:
    QGridLayout *   _gridLayout;
    QLabel *        _nameLabel;
    QLineEdit *     _lowerValue;
    QLineEdit *     _upperValue;
    QLabel *        _minValueLabel;
    QLabel *        _maxValueLabel;
    QxtSpanSlider * _slider;
};
//-----------------------------------------------------------------------------
class LM_API_QTB RangedSlidingInt : public AbstractRangedSlidingValue
{
    Q_OBJECT

public:
    RangedSlidingInt(   QWidget *parent = 0,
                        const QString & name = "",
                        int minValue = 0,
                        int maxValue = 100);
    virtual ~RangedSlidingInt();

    virtual void setupUi();
    virtual void getRangedValue(int & lower, int & upper);
    virtual void setRangedValue(const int & lower, const int & upper);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void spanChanged(int lower, int upper);
    virtual void editingFinished();

private Q_SLOTS:
    void upperTextChanged(const QString & text);
    void lowerTextChanged(const QString & text);

protected:
    QString _name;
    int       _minValue;
    int       _maxValue;
    bool _isValueChanged;
};
//-----------------------------------------------------------------------------
class LM_API_QTB RangedSlidingDouble : public AbstractRangedSlidingValue
{
    Q_OBJECT

public:
    RangedSlidingDouble(QWidget *parent = 0,
                        const QString & name = "",
                        double minValue = 0,
                        double maxValue = 100);
    virtual ~RangedSlidingDouble();

    virtual void setupUi();
    virtual void getRangedValue(double & lower, double & upper);
    virtual void getRangedValue(float & lower, float & upper);
    virtual void setRangedValue(const double & lower, const double & upper);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void spanChanged(int lower, int upper);
    virtual void editingFinished();

private Q_SLOTS:
    void upperTextChanged(const QString & text);
    void lowerTextChanged(const QString & text);

protected:
    QString _name;
    double  _minValue;
    double  _maxValue;
    bool _isValueChanged;
};
//-----------------------------------------------------------------------------
}//namespace QtToolbox

#endif /*RANGED_SLIDING_VALUE_H_*/