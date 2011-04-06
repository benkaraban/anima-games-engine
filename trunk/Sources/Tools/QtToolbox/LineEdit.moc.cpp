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
#include "LineEdit.moc.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QIntValidator>
#include <QDoubleValidator>
#include <QRegExpValidator>
#include <QDragEnterEvent>

#include <QPainter>


namespace QtToolbox
{

LineEdit::LineEdit( QWidget *parent,
                    const QString & name,
                    bool showClearButton,
                    const QString & defaultValue,
                    int lineEditWidth)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _value(NULL),
    _name(name),
    _showClearButton(showClearButton),
    _defaultValue(defaultValue),
    _isValueChanged(false),
    _lineEditWidth(lineEditWidth),
    _pixClear(12, 12)
{
    setupUi();
}

LineEdit::~LineEdit()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_value != NULL)
        delete _value;

    _gridLayout     = NULL;
    _nameLabel      = NULL;
    _value          = NULL;
}

void LineEdit::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("LineEdit"));

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(70);
    _nameLabel->setText(_name);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    _value = new QLineEdit(this);
    _value->setObjectName(QString::fromUtf8("value"));
    _value->setFixedWidth(_lineEditWidth);
    _value->setText(_defaultValue);

    _gridLayout->addWidget(_value, 0, 1, 1, 1, Qt::AlignLeft);

    createPixMaps();

    setLayout(_gridLayout);

    int offset = 5;
    if(_showClearButton)
        offset += _pixClear.width() + 2;

    this->setFixedWidth(_value->width() + _nameLabel->width() + offset);
    this->setFixedHeight(16);
    
    connect(_value,    SIGNAL(editingFinished()),
            this,      SLOT(editingFinished()));

    connect(_value,     SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged()));
}

void LineEdit::textChanged()
{
    _isValueChanged = true;
}

void LineEdit::editingFinished()
{
    if (_isValueChanged == true)
    {
        _isValueChanged = false;

        emit valueUpdated();
    }
}

QString LineEdit::text() const
{
    return _value->text();
}

void LineEdit::setText(const QString & text)
{
    _value->setText(text);
}

void LineEdit::setReadOnly(bool readOnly)
{
    _value->setReadOnly(readOnly);
}

void LineEdit::setValidator(int min, int max)
{
    _value->setValidator(new QIntValidator(min, max, _value));
}

void LineEdit::setValidator(double min, double max, int decimals)
{
    _value->setValidator(new QDoubleValidator(min, max, decimals, _value));
}

void LineEdit::setValidator(QString regExp)
{
    _value->setValidator(new QRegExpValidator(QRegExp(regExp), _value));
}

void LineEdit::setLabelColor(const QColor & color)
{
    QPalette palette = _nameLabel->palette();
    palette.setColor(QPalette::WindowText, color);
    _nameLabel->setPalette(palette);
}

void LineEdit::pushAuthorizedDropMimeData(const QString & mimeData)
{
    setAcceptDrops(true);
    _authorizedMimeData << mimeData;
}

void LineEdit::dragEnterEvent(QDragEnterEvent * dragEnterEvent)
{
    if(_authorizedMimeData.size() > 0)
    {
        const QMimeData * mimeData = dragEnterEvent->mimeData();
        QString format;
        if(isAuthorizedMimeData(mimeData, format))
        {
            dragEnterEvent->accept();
        }
        else
            dragEnterEvent->ignore();
        
    }
    else
        QWidget::dragEnterEvent(dragEnterEvent);
}

void LineEdit::dragMoveEvent(QDragMoveEvent * dragMoveEvent)
{
    if(_authorizedMimeData.size() > 0)
    {
        const QMimeData * mimeData = dragMoveEvent->mimeData();
        QString format;
        if(isAuthorizedMimeData(mimeData, format))
        {
            dragMoveEvent->accept();
        }
        else
            dragMoveEvent->ignore();
    }
    else
        QWidget::dragMoveEvent(dragMoveEvent);
}

void LineEdit::dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent)
{
    QWidget::dragLeaveEvent(dragLeaveEvent);
}

void LineEdit::dropEvent(QDropEvent * dropEvent)
{
    if(_authorizedMimeData.size() > 0)
    {
        const QMimeData * mimeData = dropEvent->mimeData();
        QString format;
        if(isAuthorizedMimeData(mimeData, format))
        {
            dropEvent->accept();

            QString text = mimeData->data(format);
            setText(text);
            emit valueUpdated();
        }
        else
            dropEvent->ignore();
    }
    else
        QWidget::dropEvent(dropEvent);
}

bool LineEdit::isAuthorizedMimeData(const QMimeData * mimeData, QString & format)
{
    bool isMimeDataAuthorized = false;

    int iMimeData = 0;
    while(iMimeData < _authorizedMimeData.size() && isMimeDataAuthorized == false)
    {
        if(mimeData->hasFormat(_authorizedMimeData[iMimeData]))
        {
            isMimeDataAuthorized = true;
            format = _authorizedMimeData[iMimeData];
        }

        ++iMimeData;
    }

    return isMimeDataAuthorized;
}

void LineEdit::paintEvent(QPaintEvent * paintEvent)
{
    QWidget::paintEvent(paintEvent);
    
    if(_showClearButton)
    {
        QPainter lineEditPainter(this);
        QRect rectangle;
        int left = rect().right() - _pixClear.width();
        int top = 2;
        rectangle.setRect(left, top, _pixClear.width(), _pixClear.height());
        
        lineEditPainter.drawPixmap(rectangle, _pixClear);
    }
}

void LineEdit::mousePressEvent(QMouseEvent * mouseEvent )
{
    if(_showClearButton)
    {
        if( mouseEvent->button() == Qt::LeftButton )
        {
            QRect buttonArea;
            int left = rect().right() - _pixClear.width();
            int top = 2;
            buttonArea.setRect(left, top, _pixClear.width(), _pixClear.height());
            if( buttonArea.contains( mouseEvent->pos() ) )
            {
                _pointClicked = mouseEvent->pos();
            }
        }
    }
    QWidget::mousePressEvent(mouseEvent);
}

void LineEdit::mouseReleaseEvent(QMouseEvent * mouseEvent)
{
    if(_showClearButton)
    {
        if( mouseEvent->button() == Qt::LeftButton && _pointClicked == mouseEvent->pos() )
        {    
            _value->clear();

            emit valueUpdated();
        }
    }
    QWidget::mouseReleaseEvent(mouseEvent);
}

void LineEdit::createPixMaps()
{
    QPainter clearPainter(&_pixClear);
    clearPainter.fillRect(_pixClear.rect(), palette().color(QPalette::Window));

    clearPainter.setPen(Qt::darkGray);
    clearPainter.setBrush(QBrush(Qt::darkGray));
    clearPainter.drawEllipse(0, 0, _pixClear.width()-1, _pixClear.height()-1);

    clearPainter.setPen(Qt::white);
    clearPainter.drawLine(_pixClear.width()/4, _pixClear.width()/4, 3*_pixClear.width()/4 - 1, 3*_pixClear.height()/4 - 1);
    clearPainter.drawLine(3 * _pixClear.width()/4 -1, _pixClear.width()/4, _pixClear.width()/4, 3*_pixClear.height()/4 - 1);
    
}

}//namespace QtToolbox
