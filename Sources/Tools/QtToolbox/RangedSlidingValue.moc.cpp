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
#include "RangedSlidingValue.moc.h"

#include <QIntValidator>
#include <QDoubleValidator>
#include <QKeyEvent>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AbstractRangedSlidingValue::AbstractRangedSlidingValue(QWidget *parent)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _lowerValue(NULL),
    _upperValue(NULL),
    _minValueLabel(NULL),
    _maxValueLabel(NULL),
    _slider(NULL)
{
}
//-----------------------------------------------------------------------------
AbstractRangedSlidingValue::~AbstractRangedSlidingValue()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_lowerValue != NULL)
        delete _lowerValue;
    if(_upperValue != NULL)
        delete _upperValue;
    if(_minValueLabel != NULL)
        delete _minValueLabel;
    if(_maxValueLabel != NULL)
        delete _maxValueLabel;
    if(_slider != NULL)
        delete _slider;

    _gridLayout     = NULL;
    _nameLabel      = NULL;
    _lowerValue     = NULL;
    _upperValue     = NULL;
    _minValueLabel  = NULL;
    _maxValueLabel  = NULL;
    _slider         = NULL;
}
//-----------------------------------------------------------------------------
bool AbstractRangedSlidingValue::eventFilter( QObject *dist, QEvent *event )
{
    if( event->type() == QEvent::KeyPress )
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );

      if (keyEvent == QKeySequence::Undo || keyEvent == QKeySequence::Redo)
      {
          QWidget* p = LM_DEBUG_CAST<QWidget*>(parent());
          LM_ASSERT(p!=NULL);
          setFocus();

          return  QWidget::event (event);
      }
    }

    return false;
}
//-----------------------------------------------------------------------------
void AbstractRangedSlidingValue::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("RangedSlidingValue"));
    this->setFixedWidth(447);
    this->setFixedHeight(16);
    
    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    _lowerValue = new QLineEdit(this);
    _lowerValue->setObjectName(QString::fromUtf8("lowerValue"));
    _lowerValue->setFixedWidth(60);
    _lowerValue->installEventFilter(this);

    _gridLayout->addWidget(_lowerValue, 0, 1, 1, 1);

    _upperValue = new QLineEdit(this);
    _upperValue->setObjectName(QString::fromUtf8("upperValue"));
    _upperValue->setFixedWidth(60);
    _upperValue->installEventFilter(this);

    _gridLayout->addWidget(_upperValue, 0, 2, 1, 1);

    _minValueLabel = new QLabel(this);
    _minValueLabel->setObjectName(QString::fromUtf8("minValue"));
    _minValueLabel->setFixedWidth(40);
    _minValueLabel->setAlignment(Qt::AlignRight);

    _gridLayout->addWidget(_minValueLabel, 0, 3, 1, 1);

    _slider = new QxtSpanSlider(this);
    _slider->setObjectName(QString::fromUtf8("slider"));
    _slider->setOrientation(Qt::Horizontal);
    _slider->setFixedWidth(130);

    _gridLayout->addWidget(_slider, 0, 4, 1, 1);

    _maxValueLabel = new QLabel(this);
    _maxValueLabel->setObjectName(QString::fromUtf8("maxValue"));
    _maxValueLabel->setFixedWidth(40);

    _gridLayout->addWidget(_maxValueLabel, 0, 5, 1, 1);

    setLayout(_gridLayout);

    connect (_slider, SIGNAL(sliderPressed()),
            this, SLOT(rangeHolding()));

    connect (_slider, SIGNAL(sliderReleased()),
            this, SLOT(rangeReleasing()));
}
//-----------------------------------------------------------------------------
void AbstractRangedSlidingValue::rangeHolding()
{
    emit signalRangeHeld();
}
//-----------------------------------------------------------------------------
void AbstractRangedSlidingValue::rangeReleasing()
{
    emit signalRangeReleased();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RangedSlidingInt::RangedSlidingInt( QWidget *parent,
                                    const QString & name,
                                    int minValue,
                                    int maxValue)
:   AbstractRangedSlidingValue(parent),
    _name(name),
    _minValue(minValue),
    _maxValue(maxValue),
    _isValueChanged(false)
{
    setupUi();
    QIntValidator * intValidator = new QIntValidator(this);
    intValidator->setRange(_minValue, _maxValue);
    _lowerValue->setValidator(intValidator);
    _upperValue->setValidator(intValidator);
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::editingFinished()
{
    if (_isValueChanged == true)
    {
        disconnectSignals();

        double textLowerValue = _lowerValue->text().toDouble();
        double textUpperValue = _upperValue->text().toDouble();
        int difference = _maxValue - _minValue;
        double newLowerValue = Core::L_round(((textLowerValue - _minValue) * 100)/difference);
        double newUpperValue = Core::L_round(((textUpperValue - _minValue) * 100)/difference);
        int sliderLowerValue = static_cast<int>(newLowerValue);
        int sliderUpperValue = static_cast<int>(newUpperValue);

        _slider->setSpan(sliderLowerValue, sliderUpperValue);
        _isValueChanged = false;
        connectSignals();

        emit rangeUpdated();
    }
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::getRangedValue(int & lower, int & upper)
{
    lower = _lowerValue->text().toInt();
    upper = _upperValue->text().toInt();
}
//-----------------------------------------------------------------------------
RangedSlidingInt::~RangedSlidingInt()
{
    disconnect();
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::connectSignals()
{
    connect(_slider,    SIGNAL(spanChanged(int, int)),
            this,       SLOT(spanChanged(int, int)));

    connect(_upperValue,SIGNAL(textChanged (const QString &)),
            this,       SLOT(upperTextChanged(const QString &)));

    connect(_lowerValue,SIGNAL(textChanged (const QString &)),
            this,       SLOT(lowerTextChanged(const QString &)));

    connect(_lowerValue,SIGNAL(editingFinished()),
            this,       SLOT(editingFinished()));

    connect(_upperValue,SIGNAL(editingFinished()),
            this,       SLOT(editingFinished()));
}

void RangedSlidingInt::disconnectSignals()
{
    disconnect( _slider,    SIGNAL(spanChanged(int, int)),
                this,       SLOT(spanChanged(int, int)));

    disconnect( _upperValue,SIGNAL(textChanged (const QString &)),
                this,       SLOT(upperTextChanged(const QString &)));

    disconnect( _lowerValue,SIGNAL(textChanged (const QString &)),
                this,       SLOT(lowerTextChanged(const QString &)));

    disconnect( _lowerValue,SIGNAL(editingFinished()),
                this,       SLOT(editingFinished()));

    disconnect( _upperValue,SIGNAL(editingFinished()),
                this,       SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::setupUi()
{
    AbstractRangedSlidingValue::setupUi();
    _slider->setMinimum(0);
    _slider->setMaximum(100);
    
    _minValueLabel->setText(QString::number(_minValue));
    _maxValueLabel->setText(QString::number(_maxValue));
    _nameLabel->setText(_name);
    
    setRangedValue( _minValue + ((_maxValue - _minValue)/4), 
                    _minValue + ((_maxValue - _minValue)*3/4) );
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::setRangedValue(const int & lower, const int & upper)
{
    QString stringLowerValue;
    stringLowerValue = QString::number( lower );
    _lowerValue->setText(stringLowerValue);
    
    QString stringUpperValue;
    stringUpperValue = QString::number( upper );
    _upperValue->setText(stringUpperValue);

    _isValueChanged = true;

    editingFinished();
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::spanChanged(int lower, int upper)
{
    disconnectSignals();
    int difference = _maxValue - _minValue;
    int specialisedLowerValue = _minValue + ((difference * lower)/100);
    int specialisedUpperValue = _minValue + ((difference * upper)/100);

    QString stringLowerValue;
    stringLowerValue = QString::number(specialisedLowerValue);
    QString stringUpperValue;
    stringUpperValue = QString::number(specialisedUpperValue);

    _lowerValue->setText(stringLowerValue);
    _upperValue->setText(stringUpperValue);
    _isValueChanged = false;
    connectSignals();
    
    emit rangeUpdated();
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::lowerTextChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
void RangedSlidingInt::upperTextChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
RangedSlidingDouble::RangedSlidingDouble(   QWidget *parent,
                                            const QString & name,
                                            double minValue,
                                            double maxValue)
:   AbstractRangedSlidingValue(parent),
    _name(name),
    _minValue(minValue),
    _maxValue(maxValue),
    _isValueChanged(false)
{
    setupUi();
    QDoubleValidator * doubleValidator = new QDoubleValidator(_minValue, _maxValue, 10, this);
    _lowerValue->setValidator(doubleValidator);
    _upperValue->setValidator(doubleValidator);
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::editingFinished()
{
    if (_isValueChanged == true)
    {
        disconnectSignals();

        double textLowerValue = _lowerValue->text().toDouble();
        double textUpperValue = _upperValue->text().toDouble();
        double difference = _maxValue - _minValue;
        double newLowerValue = Core::L_round(((textLowerValue - _minValue) * 100)/difference);
        double newUpperValue = Core::L_round(((textUpperValue - _minValue) * 100)/difference);
        int sliderLowerValue = static_cast<int>(newLowerValue);
        int sliderUpperValue = static_cast<int>(newUpperValue);

        _slider->setSpan(sliderLowerValue, sliderUpperValue);
        _isValueChanged = false;
        connectSignals();

        emit rangeUpdated();
    }
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::getRangedValue(double & lower, double & upper)
{
    lower = _lowerValue->text().toDouble();
    upper = _upperValue->text().toDouble();
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::getRangedValue(float & lower, float & upper)
{
    lower = float(_lowerValue->text().toDouble());
    upper = float(_upperValue->text().toDouble());
}
//-----------------------------------------------------------------------------
RangedSlidingDouble::~RangedSlidingDouble()
{
    disconnect();
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::connectSignals()
{
    connect(_slider,    SIGNAL(spanChanged(int, int)),
            this,       SLOT(spanChanged(int, int)));

    connect( _upperValue,SIGNAL(textChanged (const QString &)),
            this,       SLOT(upperTextChanged(const QString &)));

    connect( _lowerValue,SIGNAL(textChanged (const QString &)),
             this,       SLOT(lowerTextChanged(const QString &)));

    connect(_lowerValue,SIGNAL(editingFinished()),
            this,       SLOT(editingFinished()));

    connect(_upperValue,SIGNAL(editingFinished()),
            this,       SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::disconnectSignals()
{
    disconnect( _slider,    SIGNAL(spanChanged(int, int)),
                this,       SLOT(spanChanged(int, int)));

    disconnect( _upperValue,SIGNAL(textChanged (const QString &)),
                this,       SLOT(upperTextChanged(const QString &)));

    disconnect( _lowerValue,SIGNAL(textChanged (const QString &)),
                this,       SLOT(lowerTextChanged(const QString &)));

    disconnect( _lowerValue,SIGNAL(editingFinished()),
                this,       SLOT(editingFinished()));

    disconnect( _upperValue,SIGNAL(editingFinished()),
                this,       SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::setupUi()
{
    AbstractRangedSlidingValue::setupUi();
    _slider->setMinimum(0);
    _slider->setMaximum(100);
    
    _minValueLabel->setText(QString::number(_minValue));
    _maxValueLabel->setText(QString::number(_maxValue));
    _nameLabel->setText(_name);
    
    setRangedValue( _minValue + ((_maxValue - _minValue)/4), 
                    _minValue + ((_maxValue - _minValue)*3/4) );
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::setRangedValue(const double & lower, const double & upper)
{
    QString stringLowerValue;
    stringLowerValue = QString::number( lower );
    _lowerValue->setText(stringLowerValue);
    
    QString stringUpperValue;
    stringUpperValue = QString::number( upper );
    _upperValue->setText(stringUpperValue);

    _isValueChanged = true;

    editingFinished();
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::spanChanged(int lower, int upper)
{
    disconnectSignals();
    double difference = _maxValue - _minValue;
    double specialisedLowerValue = _minValue + ((difference * lower)/100);
    double specialisedUpperValue = _minValue + ((difference * upper)/100);

    QString stringLowerValue;
    stringLowerValue = QString::number(specialisedLowerValue);
    QString stringUpperValue;
    stringUpperValue = QString::number(specialisedUpperValue);

    _lowerValue->setText(stringLowerValue);
    _upperValue->setText(stringUpperValue);
    _isValueChanged = false;
    connectSignals();
    
    emit rangeUpdated();
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::lowerTextChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
void RangedSlidingDouble::upperTextChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
} //namespace QtToolbox


