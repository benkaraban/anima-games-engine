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
#include <QtToolbox/SpinBox.moc.h>


#include <QApplication>
#include <QDesktopWidget>
#include <QMouseEvent>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
SpinBox::SpinBox(QWidget *parent) :
    QDoubleSpinBox(parent),
    _screenHeight(QApplication::desktop()->screenGeometry().height()),
    _isPressed(false)
{
    setContextMenuPolicy(Qt::NoContextMenu);

    this->installEventFilter( this );

    setDecimals(3);
    setRange(-10000,10000);
    setSingleStep(0.1);

    connect (this, SIGNAL(editingFinished()),
            this, SIGNAL(signalWidgetUpdated()));
}
//-----------------------------------------------------------------------------
bool SpinBox::eventFilter( QObject *dist, QEvent *event )
{
    if( event->type() == QEvent::KeyPress )
    {
      QKeyEvent *keyEvent = static_cast<QKeyEvent*>( event );

      if (keyEvent == QKeySequence::Undo || keyEvent == QKeySequence::Redo)
      {
          QWidget* p = LM_DEBUG_CAST<QWidget*>(parent());
          LM_ASSERT(p!=NULL);
          p->setFocus();

          return QWidget::event (event);
      }
    }

    return false;
}
//-----------------------------------------------------------------------------
SpinBox::~SpinBox()
{
}
//-----------------------------------------------------------------------------
void SpinBox::mousePressEvent(QMouseEvent * event)
{
    if (event->button() & Qt::LeftButton || event->button() & Qt::MidButton)
    {
        _isPressed = true;
        _mouseHeight = event->globalPos().y();

        emit signalWidgetHeld();
    }

    QDoubleSpinBox::mousePressEvent(event);
}
//-----------------------------------------------------------------------------
void SpinBox::mouseMoveEvent(QMouseEvent * event)
{
    if (_isPressed == true)
    {
        if(_mouseHeight != event->globalPos().y())
            stepBy(_mouseHeight - event->globalPos().y());

        if(event->globalPos().y() == 0)
        {
            _mouseHeight = _screenHeight-1;
            QCursor::setPos( event->globalPos().x(), _screenHeight-1);
        }
        else if(event->globalPos().y() == _screenHeight-1)
        {
            _mouseHeight = 0;
            QCursor::setPos( event->globalPos().x(), 0);
        }
        else
            _mouseHeight = event->globalPos().y();
    }
        
    
    QDoubleSpinBox::mouseMoveEvent(event);

    emit signalWidgetUpdated();
}
//-----------------------------------------------------------------------------
void SpinBox::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
    if (mouseEvent->button() & Qt::LeftButton || mouseEvent->button() & Qt::MidButton)
    {
        if (_isPressed == true)
        {
            _isPressed = false;
            emit signalWidgetReleased();
        }
    }

    QWidget::mouseReleaseEvent(mouseEvent);
}

//-----------------------------------------------------------------------------
}//namespace QtToolbox
