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
#ifndef SINGLE_SLIDING_VALUE_H_
#define SINGLE_SLIDING_VALUE_H_

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSlider>
#include <QKeyEvent>

#include <Core/Standard.h>
#include <Core/Math/MathTools.h>

namespace QtToolbox
{


class LM_API_QTB AbstractSingleSlidingValue : public QWidget
{
    Q_OBJECT
public:
    AbstractSingleSlidingValue(QWidget *parent = 0);
    virtual ~AbstractSingleSlidingValue();

    virtual void setupUi();
    bool eventFilter( QObject *dist, QEvent *event );

protected Q_SLOTS:
    virtual void valueChanged(int value) = 0;
    virtual void editingFinished() = 0;

private Q_SLOTS:
    void sliderHolding();
    void sliderReleasing();

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
};

class LM_API_QTB SingleSlidingInt : public AbstractSingleSlidingValue
{
    Q_OBJECT

public:
    SingleSlidingInt(   QWidget *parent = 0,
                        const QString & name = "",
                        int minValue = 0,
                        int maxValue = 100);
    virtual ~SingleSlidingInt();

    virtual void setupUi();

    virtual void getSingleValue(int & value);
    virtual void setSingleValue(const int & value);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void valueChanged(int value);
    virtual void editingFinished();

private Q_SLOTS:
    void textChanged(const QString & text);

protected:
    QString _name;
    int _minValue;
    int _maxValue;
    bool _isValueChanged;
};

class LM_API_QTB SingleSlidingDouble : public AbstractSingleSlidingValue
{
    Q_OBJECT

public:
    SingleSlidingDouble(QWidget *parent = 0,
                        const QString & name = "",
                        double minValue = 0,
                        double maxValue = 100);
    virtual ~SingleSlidingDouble();

    virtual void setupUi();
    virtual void getSingleValue(double & value);
    virtual void getSingleValue(float & value);
    virtual void setSingleValue(const double & value);

protected:
    virtual void connectSignals();
    virtual void disconnectSignals();
    virtual void valueChanged(int value);
    virtual void editingFinished();

private Q_SLOTS:
    void textChanged(const QString & text);

protected:
    QString _name;
    double _minValue;
    double _maxValue;
    bool _isValueChanged;
};

}//namespace QtToolbox

#endif /*SINGLE_SLIDING_VALUE_H_*/