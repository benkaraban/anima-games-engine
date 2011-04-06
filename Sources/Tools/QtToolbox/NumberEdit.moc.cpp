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
#include "NumberEdit.moc.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QPainter>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>

namespace QtToolbox
{
class ValidatorNumberEdit : public QValidator
{
public:
    ValidatorNumberEdit(const QString& prefix, const QString& suffix, QObject  * parent = 0) : QValidator(parent), validator(NULL), _suffix(suffix), _prefix(prefix) {}
    virtual ~ValidatorNumberEdit() {}

    QValidator::State  validate ( QString & input, int & pos ) const
    {
        QString val = input.remove(0, _prefix.size());
        val.chop(_suffix.size());

        if (pos < _prefix.size()) pos = _prefix.size();

        int posV = pos - _prefix.size();
        QValidator::State ret = validator.validate(val, posV);
        pos = posV + _prefix.size();


        input = _prefix + val + _suffix;

        return ret;
    }

private:
    QString           _suffix;
    QString           _prefix;
    QDoubleValidator validator;
};
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class Gliss : public QWidget
{
public:
    Gliss(QWidget* parent, int32 width, int32 height);
    virtual ~Gliss();

protected:
    virtual void paintEvent(QPaintEvent * paintEvent);

private:
    void setupUi();

private:
    QImage  _background;
};
//-----------------------------------------------------------------------------
Gliss::Gliss(QWidget* parent, int32 width, int32 height) :
    _background(width, height, QImage::Format_RGB32)
{
    setupUi();
    setFixedSize(width, height);
}
//-----------------------------------------------------------------------------
Gliss::~Gliss()
{
}
//-----------------------------------------------------------------------------
void Gliss::setupUi()
{
    QPainter bgPainter(&_background);
    bgPainter.fillRect(_background.rect(), Qt::transparent);
}
//-----------------------------------------------------------------------------
void Gliss::paintEvent(QPaintEvent * paintEvent)
{
    QPixmap pix = QPixmap::fromImage(_background);
    
    QPainter pixPainter(&pix);
        pixPainter.setRenderHint(QPainter::HighQualityAntialiasing, true);

    pixPainter.fillRect(0,0,width(), height(), /*Qt::transparent*/QColor(240, 240, 240));

    int32 marge = 4;
    QPainterPath path;
        path.moveTo(width()-1-marge, height()-1);
        path.lineTo(0, height()-1);
        path.lineTo(0, 0);
        path.lineTo(width()-1-marge, 0);
        path.cubicTo(width()-1-marge, 0, 
                     width()-1+marge, (height()-1)/2,
                     width()-1-marge, height()-1);

    pixPainter.setPen(QColor(186, 186, 186));
    pixPainter.setBrush(QColor(190, 190, 190));
    pixPainter.drawPath(path);

    pixPainter.setPen(Qt::white);
    pixPainter.setBrush(Qt::white);
        int32 separatorX = (width()-marge+1)/2;
        int32  separatorMargeY = 3;
        int32 test = 1;
    pixPainter.drawLine(separatorX, separatorMargeY, separatorX, height()-separatorMargeY-test);
    QPolygon leftAr;
        leftAr << QPoint(separatorX-2, separatorMargeY+2) << QPoint(1, (height())/2-1)  << QPoint(1, (height())/2) << QPoint(separatorX-2, height()-separatorMargeY-2-test);
        pixPainter.drawConvexPolygon(leftAr);

    QPolygon rightAr;
        rightAr << QPoint(separatorX+2, separatorMargeY+2) << QPoint(width()-marge, (height())/2-1)  << QPoint(width()-marge, (height())/2) << QPoint(separatorX+2, height()-separatorMargeY-2-test);
        pixPainter.drawConvexPolygon(rightAr);

    //On recopie la pixmap dans le widget
    QPainter pad2DPainter(this);
    pad2DPainter.drawPixmap(contentsRect().topLeft(), pix);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
NumberEdit::NumberEdit(const QString& prefix, const QString& suffix, QWidget* parent):
    QWidget(parent),
    _prefix(prefix),
    _suffix(suffix)
{
    _singleStep = 0.1;
    _precision = 3;
    setupUi();
    _isPressed = false;
     updateEdit(0);

     memo = getValue();
}  
//-----------------------------------------------------------------------------
NumberEdit::~NumberEdit()
{
}
//-----------------------------------------------------------------------------
void NumberEdit::emitValueUpdated()
{
    double val = getValue();

    if (val != memo)
    {
        memo = val;
        emit signalWidgetUpdated();
    }
}
//-----------------------------------------------------------------------------
void NumberEdit::emitValueHeld()
{
    if (_isPressed == false)
    {
        _isPressed = true;
        emit signalWidgetHeld();
       /* QEvent e (QEvent::FocusIn);
        QApplication::sendEvent(_pValue, &e);*/
        _pValue->setFocus();
    }
}
//-----------------------------------------------------------------------------
void NumberEdit::emitValueReleased()
{
    if (_isPressed == true)
    {
        _isPressed = false;
        memo = getValue();
        emit signalWidgetReleased();
    }
}
//-----------------------------------------------------------------------------
bool NumberEdit::eventFilter( QObject *dist, QEvent *event )
{
    if (dist == _pValue)
    {

        if( event->type() == QEvent::KeyPress )
        {
          QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );

          if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter)
          {
              this->setFocus(); 
          }

          if (keyEvent == QKeySequence::SelectAll)
          {
                int32 length = _pValue->text().size() - _suffix.size() - _prefix.size();
                _pValue->setSelection (_prefix.size(), length);

                return  true;
          }
        }

        if( event->type() == QEvent::FocusIn )
        {
            _pValue->setStyleSheet(
                "QLineEdit {border: 1px solid rgb(186, 186, 186); border-top-left-radius: 4px; border-bottom-left-radius: 4px; border-top-right-radius: 0px; border-top-right-radius: 0px; padding: 0 0px; background: lightgray; selection-background-color: darkgray;}"
                );
            QFocusEvent * eventFocus = (QFocusEvent *) event;
            if (eventFocus->reason() == Qt::TabFocus)
            {
                int32 length = _pValue->text().size() - _suffix.size() - _prefix.size();
                _pValue->setSelection (_prefix.size(), length);
            }
/*
            int32 length = _pValue->text().size() - _suffix.size() - _prefix.size();
            _pValue->setSelection (_prefix.size(), length);
*/
            return false;
        }

        if( event->type() == QEvent::FocusOut )
        {
            _pValue->setStyleSheet(
                "QLineEdit {border: 1px solid rgb(186, 186, 186); border-top-left-radius: 4px; border-bottom-left-radius: 4px; border-top-right-radius: 0px; border-top-right-radius: 0px; padding: 0 0px; background: white; selection-background-color: darkgray;}"
                );

            emitValueReleased();

            
            return false;
        }
      

        if ( event->type() == QEvent::MouseButtonDblClick )
        {
            int32 length = _pValue->text().size() - _suffix.size() - _prefix.size();
            _pValue->setSelection (_prefix.size(), length);
            //event->accept();
            return  true;
        }

        if ( event->type() == QEvent::ContextMenu)
        {
            event->accept();
            return true;
        }

        if (event->type() == QEvent::Wheel)
        {
            emitValueHeld();

            QWheelEvent *wEvent = static_cast<QWheelEvent*>(event);

            int32 numDegrees = wEvent->delta() / 8;
            int32 numSteps = numDegrees / 15;

            stepBy(numSteps);

            event->accept();
            return  true;
        }
    }
    else if (dist == _pGliss)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);

            if (mEvent->button() == Qt::LeftButton && _isPressed == false)
            {

                _mouseHeight  = mEvent->globalPos().y();
                _mouseWidth   = mEvent->globalPos().x();

                _screenHeight = QApplication::desktop()->screenGeometry().height();
                _screenWidth  = QApplication::desktop()->screenGeometry().width();
                _curPos.x = mEvent->globalPos().x();
                _curPos.y = mEvent->globalPos().y();
               // QApplication::setOverrideCursor(Qt::BlankCursor);

                emitValueHeld();

                event->accept();
                return  true;
            }
            else
            {
                if (_isPressed == false)
                    this->setFocus();
            }
        }

        if (event->type() == QEvent::MouseButtonRelease)
        {
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);

            if (mEvent->button() == Qt::LeftButton && _isPressed == true)
            {
                
                QApplication::restoreOverrideCursor();
               // QApplication::overrideCursor()->setPos(_curPos.x, _curPos.y);
                this->setFocus();
                emitValueReleased();
                event->accept();
                return  true;
            }
            else
            {
                QApplication::restoreOverrideCursor();
            /*    if (_isPressed==true)
                {
                    QApplication::overrideCursor()->setPos(_curPos.x, _curPos.y);
                }*/
                //this->setFocus();
            }
            
        }

        if (event->type() == QEvent::MouseMove)
        {
            QMouseEvent *mEvent = static_cast<QMouseEvent*>(event);

            if (_isPressed == true)
            {
                float stepX = mEvent->globalPos().x()-_mouseWidth;
                float stepY = _mouseHeight - mEvent->globalPos().y();
                stepBy((stepX+stepY)/2.f);

                if(mEvent->globalPos().y() <= 1)
                {
                    _mouseHeight = _screenHeight-1;
                    QCursor::setPos( mEvent->globalPos().x(), _mouseHeight);
                }
                else if(mEvent->globalPos().y() >= _screenHeight-1)
                {
                    _mouseHeight = 0;
                    QCursor::setPos( mEvent->globalPos().x(), _mouseHeight);
                }
                else
                    _mouseHeight = mEvent->globalPos().y();

                if(mEvent->globalPos().x() <= 1)
                {
                    _mouseWidth = _screenWidth-1;
                    QCursor::setPos( _mouseWidth, mEvent->globalPos().y());
                }
                else if(mEvent->globalPos().x() >= _screenWidth-1)
                {
                    _mouseWidth = 0;
                    QCursor::setPos( _mouseWidth, mEvent->globalPos().y());
                }
                else
                    _mouseWidth = mEvent->globalPos().x();
            }

            event->accept();
            return  true;
        }
    }

    return false;
}
//-----------------------------------------------------------------------------
bool NumberEdit::event (QEvent * e)
{
    return  QWidget::event (e);
}
//-----------------------------------------------------------------------------
void NumberEdit::mousePressEvent(QMouseEvent* event)
{
    return QWidget::mousePressEvent(event);
}
//-----------------------------------------------------------------------------
void NumberEdit::mouseMoveEvent(QMouseEvent* event)
{
    return QWidget::mouseMoveEvent(event);
}
//-----------------------------------------------------------------------------
void NumberEdit::mouseReleaseEvent(QMouseEvent* event)
{
    return QWidget::mouseReleaseEvent(event);
}
//-----------------------------------------------------------------------------
void NumberEdit::mouseDoubleClickEvent(QMouseEvent * event)
{
    return QWidget::mouseDoubleClickEvent(event);
}
//-----------------------------------------------------------------------------
void NumberEdit::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Up)
    {
        emitValueHeld();

        stepBy(1);
        event->accept();
    }

    if (event->key() == Qt::Key_Down)
    {
        emitValueHeld();

        stepBy(-1);
        event->accept();
    }
}
//-----------------------------------------------------------------------------
void NumberEdit::keyReleaseEvent(QKeyEvent* event)
{
}
//-----------------------------------------------------------------------------
void NumberEdit::focusInEvent ( QFocusEvent * event )
{
    QWidget::focusInEvent(event);

    if (event->reason() == Qt::TabFocusReason)
    {
        _pValue->setFocus(Qt::TabFocusReason);
    }
    
}
//-----------------------------------------------------------------------------
void NumberEdit::setupUi()
{
    _playout = new QHBoxLayout(this);
        _playout->setMargin(0);
        _playout->setContentsMargins(0,0,0,0);
        _playout->setSpacing(0);

    _pValue = new QLineEdit (this);
    //_pValue->setButtonSymbols(QAbstractSpinBox::NoButtons);
        //_pValue->setFixedWidth(120);
        _pValue->setStyleSheet(
            "QLineEdit {border: 1px solid rgb(186, 186, 186); border-top-left-radius: 4px; border-bottom-left-radius: 4px; border-top-right-radius: 0px; border-top-right-radius: 0px; padding: 0 0px; background: white; selection-background-color: darkgray;}"
            );
        _pValue->installEventFilter(this);
        ValidatorNumberEdit * doubleValidator = new ValidatorNumberEdit(_prefix, _suffix, this);
        _pValue->setValidator(doubleValidator);
        _pValue->setFocusPolicy(Qt::WheelFocus);

    _pGliss = new Gliss(this, 15, int32(_pValue->height()/1.6));
        _pGliss->installEventFilter(this);
        _pGliss->setFocusPolicy(Qt::StrongFocus);
        _pGliss->setFocusProxy(_pValue);

    _playout->addWidget(_pValue);
    _playout->addWidget(_pGliss);

    connect (_pValue, SIGNAL(cursorPositionChanged(int, int)),  this, SLOT(cursorPositionChanged(int, int)));
    connect (_pValue, SIGNAL(returnPressed()),                  this, SLOT(lineEditreturnPressed()));
    connect (_pValue, SIGNAL( 	editingFinished  ( )),          this, SLOT(lineEditFinished()));

   
    setLayout(_playout);
    setFocusPolicy(Qt::WheelFocus);
}
//-----------------------------------------------------------------------------
void NumberEdit::lineEditreturnPressed()
{

    emitValueUpdated();
    emitValueReleased();
}
//-----------------------------------------------------------------------------
void NumberEdit::lineEditChanged()
{
  //  emitValueUpdated();
}
//-----------------------------------------------------------------------------
void NumberEdit::lineEditFinished()
{
    emitValueUpdated();              
}
//-----------------------------------------------------------------------------
double NumberEdit::getValue() const
{
    double ret = 0;

    QString text = _pValue->text();

    text.remove(0, _prefix.size());
    text.chop(_suffix.size());

    bool res = false;
    ret = text.toDouble(&res);

    if (res==false)
        ret = 0;

    return ret;
}
//-----------------------------------------------------------------------------
void NumberEdit::setValue(double val)
{
    updateEdit(val);
}
//-----------------------------------------------------------------------------
void NumberEdit::updateEdit(double val)
{
    const QString newQText = _prefix + QString::number (val, 'f', _precision) + _suffix;


    if (newQText == _pValue->displayText())
        return;

    const bool empty = _pValue->text().isEmpty();

    int32 cursor = _pValue->cursorPosition();
    int32 selsize = _pValue->selectedText().size();

    const bool sb = _pValue->blockSignals(true);
    _pValue->setText(newQText);


    _pValue->setCursorPosition(_prefix.size()+Core::toString(val).size());
/*
    if (!specialValue()) {
        cursor = qBound(prefix.size(), cursor, edit->displayText().size() - suffix.size());

        if (selsize > 0) {
            edit->setSelection(cursor, selsize);
        } else {
            edit->setCursorPosition(empty ? prefix.size() : cursor);
        }
    }*/

    _pValue->blockSignals(sb); 

    emitValueUpdated();
}
//-----------------------------------------------------------------------------
void NumberEdit::cursorPositionChanged(int old, int newP)
{
    if (newP < _prefix.size())
    {
        const bool sb = _pValue->blockSignals(true);
        _pValue->setCursorPosition(_prefix.size());
        _pValue->blockSignals(sb); 
    }
    else if(newP > _pValue->text().size() - _suffix.size())
    {
        const bool sb = _pValue->blockSignals(true);
        _pValue->setCursorPosition(_pValue->text().size() - _suffix.size());
        _pValue->blockSignals(sb);
    }
}
//-----------------------------------------------------------------------------
void NumberEdit::stepBy(int steps)
{/*
    const QVariant old = d->value;
    QString tmp = d->edit->displayText();
    int cursorPos = d->edit->cursorPosition();
    bool dontstep = false;
    EmitPolicy e = EmitIfChanged;
    if (d->pendingEmit) {
        dontstep = validate(tmp, cursorPos) != QValidator::Acceptable;
        d->cleared = false;
        d->interpret(NeverEmit);
        if (d->value != old)
            e = AlwaysEmit;
    }
    if (!dontstep) {
        d->setValue(d->bound(d->value + (d->singleStep * steps), old, steps), e);
    } else if (e == AlwaysEmit) {
        d->emitSignals(e, old);
    }

    selectAll();*/
    double val = getValue();
    val += steps*_singleStep;
    updateEdit(val);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
} // namespace QtToolBox
