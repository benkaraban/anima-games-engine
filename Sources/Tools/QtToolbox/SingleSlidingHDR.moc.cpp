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
#include <Core/Logger.h>
#include "SingleSlidingHDR.moc.h"

#include <QDoubleValidator>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
SingleSlidingHDR::SingleSlidingHDR(QWidget *parent,
                                   const QString & name,
                                   bool allowNegative)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _value(NULL),
    _minValueLabel(NULL),
    _maxValueLabel(NULL),
    _slider(NULL),
    _name(name),
    _isValueChanged(false),
    _isAutoScaleNeeded(false),
    _allowNegative(allowNegative)
{
    if(_allowNegative)
    {
        _min = -1.0;
        _max = 1.0;
    }
    else
    {
        _min = 0.0;
        _max = 1.0;
    }

    setupUi();
    QDoubleValidator * doubleValidator = new QDoubleValidator(this);
    _value->setValidator(doubleValidator);
}
//-----------------------------------------------------------------------------
SingleSlidingHDR::~SingleSlidingHDR()
{
    disconnect();

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
bool SingleSlidingHDR::eventFilter( QObject *dist, QEvent *event )
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
void SingleSlidingHDR::setupUi()
{
    if(this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("SingleSlidingValue"));
    this->setFixedWidth(442);
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

    _combo = new QComboBox(this);
    _combo->setObjectName(QString::fromUtf8("scale"));
    _combo->setFixedWidth(50);
    _combo->addItem(QString::fromUtf8("0.01"));
    _combo->addItem(QString::fromUtf8("0.1"));
    _combo->addItem(QString::fromUtf8("1"));
    _combo->addItem(QString::fromUtf8("10"));
    _combo->addItem(QString::fromUtf8("100"));
    _combo->addItem(QString::fromUtf8("1 k"));
    _combo->addItem(QString::fromUtf8("10 k"));

    _gridLayout->addWidget(_combo, 0, 5, 1, 1);

    setLayout(_gridLayout);

    connect (_slider, SIGNAL(sliderPressed()),
            this, SLOT(sliderHolding()));

    connect (_slider, SIGNAL(sliderReleased()),
            this, SLOT(sliderReleasing()));

    _slider->setMinimum(0);
    _slider->setMaximum(100);

    _combo->setCurrentIndex(1);

    _minValueLabel->setText(QString::number(_min));
    _maxValueLabel->setText(QString::number(_max));
    _nameLabel->setText(_name);

    setSingleValue((_max + _min)/2);
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::sliderHolding()
{
    emit signalSliderHeld();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::sliderReleasing()
{
    emit signalSliderReleased();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::editingFinished()
{
    if(_isValueChanged)
    {
        disconnectSignals();

        double textValue = _value->text().toDouble();
        _isValueChanged = false;

        if(_isAutoScaleNeeded)
        {
            internalUpdateScaleIfNeeded(textValue);
            _isAutoScaleNeeded = false;
        }

        internalSetSlider(textValue);
        connectSignals();

        emit valueUpdated();
    }
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::internalSetSlider(double value)
{
    double difference = _max - _min;
    double newValue = Core::L_round(((value - _min) * 100)/difference);
    int sliderValue = static_cast<int>(newValue);
    _slider->setValue(sliderValue);
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::getSingleValue(double & value)
{
    value = _value->text().toDouble();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::getSingleValue(float & value)
{
    value = float(_value->text().toDouble());
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::connectSignals()
{
    connect(_slider,    SIGNAL(valueChanged(int)),
            this,       SLOT(valueChanged(int)));

    connect(_value,    SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged(const QString &)));

    connect(_value, SIGNAL(editingFinished()),
            this,   SLOT(editingFinished()));
    
    connect(_combo, SIGNAL(currentIndexChanged(int)),
            this,   SLOT(scaleChanged(int)));
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::disconnectSignals()
{
    disconnect(_slider,    SIGNAL(valueChanged(int)),
            this,       SLOT(valueChanged(int)));

    disconnect(_value,    SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged(const QString &)));

    disconnect( _value, SIGNAL(editingFinished()),
                this,   SLOT(editingFinished()));

    disconnect(_combo, SIGNAL(currentIndexChanged(int)),
               this,   SLOT(scaleChanged(int)));
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::scaleChanged(int index)
{
    disconnectSignals();
    internalSetScale(index);
    connectSignals();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::internalSetScale(int32 index)
{
    double v;
    getSingleValue(v);

    switch(index)
    {
    case 0: _max = 0.01;    break;
    case 1: _max = 0.1;     break;
    case 2: _max = 1.0;     break;
    case 3: _max = 10.0;    break;
    case 4: _max = 100.0;   break;
    case 5: _max = 1000.0;  break;
    case 6: _max = 10000.0;  break;
    }

    if(_allowNegative)
        _min = -_max;
    else
        _min = 0.0;

    v = Core::clamp(_min, _max, v);

    internalSetText(v);
    internalSetSlider(v);

    _minValueLabel->setText(QString::number(_min));
    _maxValueLabel->setText(QString::number(_max));
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::internalUpdateScaleIfNeeded(double value)
{
    value = abs(value);
    int32 newIndex = 6;

    if(value <= 0.01 && value != 0.0)
        newIndex = 0;
    if(value <= 0.1 && value != 0.0)
        newIndex = 1;
    else if(value <= 1.0)
        newIndex = 2;
    else if(value <= 10.0)
        newIndex = 3;
    else if(value <= 100.0)
        newIndex = 4;
    else if(value <= 1000.0)
        newIndex = 5;

    if(newIndex != _combo->currentIndex())
    {
        _combo->setCurrentIndex(newIndex);
        internalSetScale(newIndex);
    }
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::internalSetText(double value)
{
    QString stringValue;
    stringValue = QString::number(value);
    _value->setText(stringValue);
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::setSingleValue(double value)
{
    disconnectSignals();
    internalUpdateScaleIfNeeded(value);
    connectSignals();

    internalSetText(value);
    editingFinished();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::valueChanged(int newValue)
{
    disconnectSignals();
    double difference = _max - _min;
    double specialisedValue = _min + ((difference * newValue)/100);

    QString stringValue;
    stringValue = QString::number(specialisedValue);

    _value->setText(stringValue);

    _isValueChanged = false;
    connectSignals();

    emit valueUpdated();
}
//-----------------------------------------------------------------------------
void SingleSlidingHDR::textChanged(const QString & text)
{
    _isValueChanged = true;
    _isAutoScaleNeeded = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace QtToolbox