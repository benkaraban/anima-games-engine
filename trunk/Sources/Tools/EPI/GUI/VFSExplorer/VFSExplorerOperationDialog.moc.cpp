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
#include "VFSExplorerOperationDialog.moc.h"

#include <QVBoxLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QRegExpValidator>

namespace EPI
{

VFSExplorerOperationDialog::VFSExplorerOperationDialog( const Core::List<Core::String> & others, 
                                                        EVFSE_OPERATION_TYPE type, 
                                                        const Core::String & oldName, 
                                                        QWidget *parent)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint),
    _others(others),
    _type(type),
    _oldName(oldName),
    _layout(NULL),
    _lineEdit(NULL),
    _buttonBox(NULL),
    _newName(L"")
{
    setupUi();
}

VFSExplorerOperationDialog::~VFSExplorerOperationDialog()
{
    if(_layout != NULL)
        delete _layout;
    if(_lineEdit != NULL)
        delete _lineEdit;
    if(_buttonBox != NULL)
        delete _buttonBox;

    _layout         = NULL;
    _lineEdit       = NULL;
    _buttonBox      = NULL;
}

void VFSExplorerOperationDialog::setupUi()
{
    setFixedWidth(265);
    setFixedHeight(76);

    _layout         = new QVBoxLayout();

    _lineEdit       = new QLineEdit();

    if(_oldName.length() > 0)
    {
        _lineEdit->setText(String8(_oldName).c_str());
        _lineEdit->selectAll();
    }

    switch(_type)
    {
    case EVFSE_NEW_DIR:
        this->setWindowTitle(QString::fromUtf8("VFSExplorer : Create new directory"));
        _lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]+"), _lineEdit));
        break;
    case EVFSE_RENAME_FILE:
        this->setWindowTitle(QString::fromUtf8("VFSExplorer : Rename file"));
        _lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_.]+"), _lineEdit));
        break;
    case EVFSE_RENAME_DIR:
        this->setWindowTitle(QString::fromUtf8("VFSExplorer : Rename directory"));
        _lineEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]+"), _lineEdit));
        break;
    }

    _buttonBox = new QDialogButtonBox();
    _buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
    _buttonBox->setOrientation(Qt::Horizontal);
    _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    textChanged();

    _layout->addWidget(_lineEdit);
    _layout->addWidget(_buttonBox);

    setLayout(_layout);

    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    connect(_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(textChanged()));
}

void VFSExplorerOperationDialog::textChanged()
{
    _newName = String(_lineEdit->text().toStdString().c_str());
    
    bool acceptName = true;
    if(_newName.length() > 0)
    {
        for(int32 ii=0; ii<_others.size(); ++ii)
        {
            if(_others[ii] == _newName)
                acceptName = false;
        }
    }
    else 
        acceptName = false;

    _buttonBox->button( QDialogButtonBox::Ok )->setEnabled(acceptName);
}

}//namespace EPI
