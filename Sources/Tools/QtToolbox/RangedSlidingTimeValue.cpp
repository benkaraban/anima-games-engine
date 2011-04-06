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
#include "RangedSlidingTimeValue.h"

#ifdef _WIN32
#include <winsafe.h>
#endif

#include <Core/Math/MathTools.h>

namespace QtToolbox
{

RangedSlidingTimeValue::RangedSlidingTimeValue( QWidget *parent,
                                                const QString & name,
                                                double minValue,
                                                double maxValue)
:   RangedSlidingDouble(parent, name, minValue, maxValue)
{
    _maxValueLabel->setText("Infinite");
}

void RangedSlidingTimeValue::spanChanged(int lower, int upper)
{
    disconnectSignals();
    double difference = _maxValue - _minValue;
    double specialisedLowerValue = _minValue + ((difference * lower)/100);
    double specialisedUpperValue = _minValue + ((difference * upper)/100);

    QString stringLowerValue;
    stringLowerValue = QString::number(specialisedLowerValue);
    QString stringUpperValue;
    stringUpperValue = QString::number(specialisedUpperValue);

    if(lower == 100)
        _lowerValue->setText("INF");
    else 
        _lowerValue->setText(stringLowerValue);

    if(upper == 100)
        _upperValue->setText("INF");
    else
        _upperValue->setText(stringUpperValue);
    
    _isValueChanged = false;
    connectSignals();
    emit rangeUpdated();
}

void RangedSlidingTimeValue::getRangedValue(double & lower, double & upper)
{
    if(_lowerValue->text() == "INF")
        lower = INFINITE;
    else
        lower = _lowerValue->text().toDouble();

    if(_upperValue->text() == "INF")
        upper = INFINITE;
    else
        upper = _upperValue->text().toDouble();
}

void RangedSlidingTimeValue::setRangedValue(const double & lower, const double & upper)
{
    QString stringLowerValue;
    stringLowerValue = QString::number( lower );
    if(lower >= _maxValue)
        _lowerValue->setText("INF");
    else
        _lowerValue->setText(stringLowerValue);
    
    QString stringUpperValue;
    stringUpperValue = QString::number( upper );
    if(upper >= _maxValue)
        _upperValue->setText("INF");
    else
        _upperValue->setText(stringUpperValue);

    _isValueChanged = true;

    editingFinished();
}

void RangedSlidingTimeValue::editingFinished()
{
    if (_isValueChanged == true)
    {
        disconnectSignals();

        int sliderLowerValue = 0;
        int sliderUpperValue = 0;

        double textLowerValue = _lowerValue->text().toDouble();
        double textUpperValue = _upperValue->text().toDouble();
        
        double difference = _maxValue - _minValue;
        double newLowerValue = Core::L_round(((textLowerValue - _minValue) * 100)/difference);
        double newUpperValue = Core::L_round(((textUpperValue - _minValue) * 100)/difference);

        if(!Core::isValid(textLowerValue))
            sliderLowerValue = 100;
        else
            sliderLowerValue = static_cast<int>(newLowerValue);

        if(!Core::isValid(textUpperValue))
            sliderUpperValue = 100;
        else
            sliderUpperValue = static_cast<int>(newUpperValue);

        _slider->setSpan(sliderLowerValue, sliderUpperValue);
        _isValueChanged = false;
        connectSignals();

        emit rangeUpdated();
    }
}

}
