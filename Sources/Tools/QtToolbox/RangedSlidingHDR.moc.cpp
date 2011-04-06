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
#include "RangedSlidingHDR.moc.h"

#include <QDoubleValidator>
#include <QComboBox>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
RangedSlidingHDR::RangedSlidingHDR(QWidget *parent,
                                   const QString & name,
                                   bool allowNegative)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _lowerValue(NULL),
    _upperValue(NULL),
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
    _lowerValue->setValidator(doubleValidator);
    _upperValue->setValidator(doubleValidator);
}
//-----------------------------------------------------------------------------
RangedSlidingHDR::~RangedSlidingHDR()
{
    disconnect();

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
bool RangedSlidingHDR::eventFilter( QObject *dist, QEvent *event )
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
void RangedSlidingHDR::setupUi()
{
    if(this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("RangedSlidingValue"));
    this->setFixedWidth(507);
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

    _gridLayout->addWidget(_combo, 0, 6, 1, 1);

    setLayout(_gridLayout);

    connect (_slider, SIGNAL(sliderPressed()),
            this, SLOT(rangeHolding()));

    connect (_slider, SIGNAL(sliderReleased()),
            this, SLOT(rangeReleasing()));

    _slider->setMinimum(0);
    _slider->setMaximum(100);

    _minValueLabel->setText(QString::number(_min));
    _maxValueLabel->setText(QString::number(_max));
    _nameLabel->setText(_name);

    double v = (_max + _min) / 2;
    setRangedValue(v, v);
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::rangeHolding()
{
    emit signalRangeHeld();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::rangeReleasing()
{
    emit signalRangeReleased();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::editingFinished()
{
    if(_isValueChanged)
    {
        disconnectSignals();

        double textLower = _lowerValue->text().toDouble();
        double textUpper = _upperValue->text().toDouble();

        _isValueChanged = false;

        if(_isAutoScaleNeeded)
        {
            internalUpdateScaleIfNeeded(textLower, textUpper);
            _isAutoScaleNeeded = false;
        }

        internalSetSlider(textLower, textUpper);
        connectSignals();

        emit rangeUpdated();
    }
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::internalSetSlider(double lower, double upper)
{
    double delta = _max - _min;
    int lowerVal = int(Core::L_round(((lower - _min) * 100) / delta));
    int upperVal = int(Core::L_round(((upper - _min) * 100) / delta));

    _slider->setSpan(lowerVal, upperVal);
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::getRangedValue(float  & lower, float  & upper)
{
    lower = float(_lowerValue->text().toDouble());
    upper = float(_upperValue->text().toDouble());
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::getRangedValue(double & lower, double & upper)
{
    lower = _lowerValue->text().toDouble();
    upper = _upperValue->text().toDouble();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::connectSignals()
{
    connect(_slider,    SIGNAL(spanChanged(int, int)),
            this,       SLOT(spanChanged(int, int)));

    connect(_lowerValue, SIGNAL(textChanged(const QString &)),
            this,        SLOT(lowerTextChanged(const QString &)));

    connect(_upperValue, SIGNAL(textChanged(const QString &)),
            this,        SLOT(upperTextChanged(const QString &)));

    connect(_lowerValue, SIGNAL(editingFinished()),
            this,        SLOT(editingFinished()));
    
    connect(_upperValue, SIGNAL(editingFinished()),
            this,        SLOT(editingFinished()));
    
    connect(_combo, SIGNAL(currentIndexChanged(int)),
            this,   SLOT(scaleChanged(int)));
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::disconnectSignals()
{
    disconnect(_slider,    SIGNAL(spanChanged(int, int)),
               this,       SLOT(spanChanged(int, int)));

    disconnect(_lowerValue, SIGNAL(textChanged(const QString &)),
               this,        SLOT(lowerTextChanged(const QString &)));

    disconnect(_upperValue, SIGNAL(textChanged(const QString &)),
               this,        SLOT(upperTextChanged(const QString &)));

    disconnect(_lowerValue, SIGNAL(editingFinished()),
               this,        SLOT(editingFinished()));
    
    disconnect(_upperValue, SIGNAL(editingFinished()),
               this,        SLOT(editingFinished()));

    disconnect(_combo, SIGNAL(currentIndexChanged(int)),
               this,   SLOT(scaleChanged(int)));
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::scaleChanged(int index)
{
    disconnectSignals();
    internalSetScale(index);
    connectSignals();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::internalSetScale(int32 index)
{
    double l, u;
    getRangedValue(l, u);

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

    l = Core::clamp(_min, _max, l);
    u = Core::clamp(_min, _max, u);

    internalSetText(l, u);
    internalSetSlider(l, u);

    _minValueLabel->setText(QString::number(_min));
    _maxValueLabel->setText(QString::number(_max));
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::internalUpdateScaleIfNeeded(double l, double u)
{
    double value = std::max(abs(l), abs(u));
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
void RangedSlidingHDR::internalSetText(double l, double u)
{
    _lowerValue->setText(QString::number(l));
    _upperValue->setText(QString::number(u));
    _isValueChanged = true;
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::setRangedValue(double l, double u)
{
    disconnectSignals();
    internalUpdateScaleIfNeeded(l, u);
    connectSignals();

    internalSetText(l, u);

    _isValueChanged = true;
    editingFinished();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::spanChanged(int lower, int upper)
{
    disconnectSignals();
    double delta = _max - _min;
    double lowerValue = _min + ((delta * lower) / 100);
    double upperValue = _min + ((delta * upper) / 100);

    _lowerValue->setText(QString::number(lowerValue));
    _upperValue->setText(QString::number(upperValue));

    _isValueChanged = false;
    connectSignals();

    emit rangeUpdated();
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::upperTextChanged(const QString & text)
{
    _isValueChanged = true;
    _isAutoScaleNeeded = true;
}
//-----------------------------------------------------------------------------
void RangedSlidingHDR::lowerTextChanged(const QString & text)
{
    _isValueChanged = true;
    _isAutoScaleNeeded = true;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
}//namespace QtToolbox