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
#include "SingleSlidingValue.moc.h"

#include <QIntValidator>
#include <QDoubleValidator>


namespace QtToolbox
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
AbstractSingleSlidingValue::AbstractSingleSlidingValue(QWidget *parent)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _value(NULL),
    _minValueLabel(NULL),
    _maxValueLabel(NULL),
    _slider(NULL)
{
    
}
//-----------------------------------------------------------------------------
AbstractSingleSlidingValue::~AbstractSingleSlidingValue()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_value != NULL)
        delete _value;
    if(_minValueLabel != NULL)
        delete _minValueLabel;
    if(_maxValueLabel != NULL)
        delete _maxValueLabel;
    if(_slider != NULL)
        delete _slider;

    _gridLayout     = NULL;
    _nameLabel      = NULL;
    _value          = NULL;
    _minValueLabel  = NULL;
    _maxValueLabel  = NULL;
    _slider         = NULL;
}
//-----------------------------------------------------------------------------
bool AbstractSingleSlidingValue::eventFilter( QObject *dist, QEvent *event )
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
void AbstractSingleSlidingValue::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("SingleSlidingValue"));
    this->setFixedWidth(382);
    this->setFixedHeight(16);

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    _value = new QLineEdit(this);
    _value->setObjectName(QString::fromUtf8("value"));
    _value->setFixedWidth(60);
    _value->installEventFilter(this);

    _gridLayout->addWidget(_value, 0, 1, 1, 1);

    _minValueLabel = new QLabel(this);
    _minValueLabel->setObjectName(QString::fromUtf8("minValue"));
    _minValueLabel->setFixedWidth(40);
    _minValueLabel->setAlignment(Qt::AlignRight);

    _gridLayout->addWidget(_minValueLabel, 0, 2, 1, 1);

    _slider = new QSlider(this);
    _slider->setObjectName(QString::fromUtf8("slider"));
    _slider->setOrientation(Qt::Horizontal);
    _slider->setFixedWidth(130);

    _gridLayout->addWidget(_slider, 0, 3, 1, 1);

    _maxValueLabel = new QLabel(this);
    _maxValueLabel->setObjectName(QString::fromUtf8("maxValue"));
    _maxValueLabel->setFixedWidth(40);

    _gridLayout->addWidget(_maxValueLabel, 0, 4, 1, 1);

    setLayout(_gridLayout);

    connect (_slider, SIGNAL(sliderPressed()),
            this, SLOT(sliderHolding()));

    connect (_slider, SIGNAL(sliderReleased()),
            this, SLOT(sliderReleasing()));
}
//-----------------------------------------------------------------------------
void AbstractSingleSlidingValue::sliderHolding()
{
    emit signalSliderHeld();
}
//-----------------------------------------------------------------------------
void AbstractSingleSlidingValue::sliderReleasing()
{
    emit signalSliderReleased();
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SingleSlidingInt::SingleSlidingInt( QWidget *parent,
                                    const QString & name,
                                    int minValue,
                                    int maxValue)
:   AbstractSingleSlidingValue(parent),
    _name(name),
    _minValue(minValue),
    _maxValue(maxValue),
    _isValueChanged(false)
{
    setupUi();
    QIntValidator * intValidator = new QIntValidator(this);
    intValidator->setRange(_minValue, _maxValue);
    _value->setValidator(intValidator);
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::editingFinished()
{
    if (_isValueChanged == true)
    {
        disconnectSignals();

        int textValue = _value->text().toInt();
        int difference = _maxValue - _minValue;
        double newValue = Core::L_round(((textValue - _minValue) * 100)/difference);
        int sliderValue = static_cast<int>(newValue);
        _slider->setValue(sliderValue);
        _isValueChanged = false;

        connectSignals();

        emit valueUpdated();
    }
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::getSingleValue(int & value)
{
    value = _value->text().toInt();
}
//-----------------------------------------------------------------------------
SingleSlidingInt::~SingleSlidingInt()
{
    disconnect();
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::connectSignals()
{
    connect(_slider,    SIGNAL(valueChanged(int)),
            this,       SLOT(valueChanged(int)));

    connect(_value,     SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged(const QString &)));

    connect(_value,    SIGNAL(editingFinished()),
            this,      SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::disconnectSignals()
{
    disconnect( _slider,    SIGNAL(valueChanged(int)),
                this,       SLOT(valueChanged(int)));

    disconnect( _value,     SIGNAL(textChanged (const QString &)),
                this,       SLOT(textChanged(const QString &)));

    disconnect( _value,     SIGNAL(editingFinished()),
                this,       SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::setupUi()
{
    AbstractSingleSlidingValue::setupUi();
    _slider->setMinimum(0);
    _slider->setMaximum(100);

    _minValueLabel->setText(QString::number(_minValue));
    _maxValueLabel->setText(QString::number(_maxValue));
    _nameLabel->setText(_name);

    setSingleValue((_maxValue + _minValue)/2);
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::setSingleValue(const int & value)
{
    QString stringValue;
    stringValue = QString::number(value);
    _value->setText(stringValue);
    _isValueChanged = true;

    editingFinished();
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::valueChanged(int newValue)
{
    disconnectSignals();
    int difference = _maxValue - _minValue;
    int specialisedValue = _minValue + ((difference * newValue)/100);

    QString stringValue;
    stringValue = QString::number(specialisedValue);

    _value->setText(stringValue);
    _isValueChanged = false;
    connectSignals();

    emit valueUpdated();
}
//-----------------------------------------------------------------------------
void SingleSlidingInt::textChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SingleSlidingDouble::SingleSlidingDouble(   QWidget *parent,
                                            const QString & name,
                                            double minValue,
                                            double maxValue)
:   AbstractSingleSlidingValue(parent),
    _name(name),
    _minValue(minValue),
    _maxValue(maxValue),
    _isValueChanged(false)
{
    setupUi();
    QDoubleValidator * doubleValidator = new QDoubleValidator(_minValue, _maxValue, 10, this);
    _value->setValidator(doubleValidator);
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::editingFinished()
{
    if (_isValueChanged == true)
    {
        disconnectSignals();

        double textValue = _value->text().toDouble();
        double difference = _maxValue - _minValue;
        double newValue = Core::L_round(((textValue - _minValue) * 100)/difference);
        int sliderValue = static_cast<int>(newValue);
        _slider->setValue(sliderValue);
        _isValueChanged = false;

        connectSignals();

        emit valueUpdated();
    }
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::getSingleValue(double & value)
{
    value = _value->text().toDouble();
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::getSingleValue(float & value)
{
    value = float(_value->text().toDouble());
}
//-----------------------------------------------------------------------------
SingleSlidingDouble::~SingleSlidingDouble()
{
    disconnect();
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::connectSignals()
{
 /*   connect(_slider,    SIGNAL(sliderPressed()),
            this,       SLOT(sliderHolding()));

    connect(_slider,    SIGNAL(sliderReleased()),
            this,       SLOT(sliderReleasing()));*/

    connect(_slider,    SIGNAL(valueChanged(int)),
            this,       SLOT(valueChanged(int)));

    connect(_value,    SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged(const QString &)));

    connect(_value, SIGNAL(editingFinished()),
            this,   SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::disconnectSignals()
{
 /*   disconnect(_slider,    SIGNAL(sliderPressed()),
            this,       SLOT(sliderHolding()));

    disconnect(_slider,    SIGNAL(sliderReleased()),
            this,       SLOT(sliderReleasing()));*/

    disconnect(_slider,    SIGNAL(valueChanged(int)),
            this,       SLOT(valueChanged(int)));

    disconnect(_value,    SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged(const QString &)));

    disconnect( _value, SIGNAL(editingFinished()),
                this,   SLOT(editingFinished()));
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::setupUi()
{
    AbstractSingleSlidingValue::setupUi();
    _slider->setMinimum(0);
    _slider->setMaximum(100);

    _minValueLabel->setText(QString::number(_minValue));
    _maxValueLabel->setText(QString::number(_maxValue));
    _nameLabel->setText(_name);

    setSingleValue((_maxValue + _minValue)/2);
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::setSingleValue(const double & value)
{
    QString stringValue;
    stringValue = QString::number(value);
    _value->setText(stringValue);
    _isValueChanged = true;

    editingFinished();
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::valueChanged(int newValue)
{
    disconnectSignals();
    double difference = _maxValue - _minValue;
    double specialisedValue = _minValue + ((difference * newValue)/100);

    QString stringValue;
    stringValue = QString::number(specialisedValue);

    _value->setText(stringValue);
    _isValueChanged = false;
    connectSignals();

    emit valueUpdated();
}
//-----------------------------------------------------------------------------
void SingleSlidingDouble::textChanged(const QString & text)
{
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace QtToolbox