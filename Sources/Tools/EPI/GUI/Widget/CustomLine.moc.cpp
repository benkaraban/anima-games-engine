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
#include "CustomLine.moc.h"

#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QDragEnterEvent>

#include <QFileDialog>

#include <EPI/Constants.h>
#include <EPI/Gui/Widget/FileDialog.moc.h>
namespace EPI
{

CustomLine::CustomLine( QWidget *parent,
                        const QString & name)
:   QWidget(parent),
    _gridLayout(NULL),
    _nameLabel(NULL),
    _value(NULL),
    _clear(NULL),
    _localOpen(NULL),
    _refresh(NULL),
    _name(name),
    _isValueChanged(false),
    _path("")
{
    setupUi();
}

CustomLine::~CustomLine()
{
    if(_gridLayout != NULL)
        delete _gridLayout;
    if(_nameLabel != NULL)
        delete _nameLabel;
    if(_value != NULL)
        delete _value;
    if(_clear != NULL)
        delete _clear;
    if(_localOpen != NULL)
        delete _localOpen;
    if(_refresh != NULL)
        delete _refresh;

    _gridLayout = NULL;
    _nameLabel  = NULL;
    _value      = NULL;
    _clear      = NULL;
    _localOpen  = NULL;
    _refresh    = NULL;
}

void CustomLine::setupUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("CustomLine"));

    _gridLayout = new QGridLayout();
    _gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    _gridLayout->setContentsMargins(0, 0, 0, 0);
    _gridLayout->setSpacing(0);

    _nameLabel = new QLabel(this);
    _nameLabel->setObjectName(QString::fromUtf8("label"));
    _nameLabel->setFixedWidth(100);
    _nameLabel->setText(_name);

    _gridLayout->addWidget(_nameLabel, 0, 0, 1, 1);

    _value = new QLineEdit(this);
    _value->setFixedWidth(140);

    _gridLayout->addWidget(_value, 0, 1, 1, 1);

    _clear = new QPushButton(QIcon(":/icons/clear.png"), "", this);
    _localOpen = new QPushButton(QIcon(":/icons/folder.png"), "", this);
    _refresh = new QPushButton(QIcon(":/icons/refresh.png"), "", this);
    _refresh->setDisabled(true);

    _clear->setFixedWidth(20);
    _localOpen->setFixedWidth(20);
    _refresh->setFixedWidth(20);
    
    _gridLayout->addWidget(_clear, 0, 2, 1, 1);
    _gridLayout->addWidget(_localOpen, 0, 3, 1, 1);
    _gridLayout->addWidget(_refresh, 0, 4, 1, 1);

    setLayout(_gridLayout);

    this->setFixedWidth(_value->width() + _nameLabel->width() + _clear->width() + _localOpen->width() + _refresh->width());
    this->setFixedHeight(16);
    
    connect(_value,    SIGNAL(editingFinished()),
            this,      SLOT(editingFinished()));

    connect(_value,     SIGNAL(textChanged (const QString &)),
            this,       SLOT(textChanged()));

    connect(_clear, SIGNAL(clicked()), this, SLOT(clearValue()));
    connect(_localOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(_refresh, SIGNAL(clicked()), this, SLOT(refresh()));
}

void CustomLine::textChanged()
{
    _isValueChanged = true;
}

void CustomLine::editingFinished()
{
    if (_isValueChanged == true)
    {
        _isValueChanged = false;

        emit valueUpdated();
    }
}

QString CustomLine::text() const
{
    return _value->text();
}

void CustomLine::setText(const QString & text)
{
    _value->setText(text);
}

void CustomLine::getText(Core::String& text) const
{
    text =  Core::String(Core::String8(_value->text().toStdString().c_str()));
}

void CustomLine::setText(const Core::String & text)
{
    _value->setText(String8(text).c_str());
}

void CustomLine::setReadOnly(bool readOnly)
{
    _value->setReadOnly(readOnly);
}

void CustomLine::pushAuthorizedDropMimeData(const QString & mimeData)
{
    setAcceptDrops(true);
    _authorizedMimeData << mimeData;
}

void CustomLine::dragEnterEvent(QDragEnterEvent * dragEnterEvent)
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

void CustomLine::dragMoveEvent(QDragMoveEvent * dragMoveEvent)
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

void CustomLine::dragLeaveEvent(QDragLeaveEvent * dragLeaveEvent)
{
    QWidget::dragLeaveEvent(dragLeaveEvent);
}

void CustomLine::dropEvent(QDropEvent * dropEvent)
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
            _refresh->setDisabled(true);
            emit valueUpdated();
        }
        else
            dropEvent->ignore();
    }
    else
        QWidget::dropEvent(dropEvent);
}

bool CustomLine::isAuthorizedMimeData(const QMimeData * mimeData, QString & format)
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

void CustomLine::clearValue()
{
    _value->clear();

    emit valueUpdated();
}

void CustomLine::openFile()
{
   // QString file = QFileDialog::getOpenFileName(this, tr("Open File"), "", String8(AUTHORISED_IMAGE_EXT).c_str());
    
    String file = FileDialog::getOpenFileName(MiscConst::memoPathVfs, L"",  MiscConst::vfsFolderShortcuts, NULL, L"Open File", AUTHORISED_IMAGE_EXT);


    if(file.length() > 0)
    {
        String path = Core::System::getPath(file);
        MiscConst::addPathToMemoPath(MiscConst::memoPathVfs, path);

        _refresh->setDisabled(false);
        _value->setText(StrToQStr(file));
        emit valueUpdated();
    }
}

void CustomLine::refresh()
{
    emit refreshValue();
}

}//namespace EPI
