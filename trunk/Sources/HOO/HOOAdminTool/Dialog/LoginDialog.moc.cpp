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
#include "LoginDialog.moc.h"

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRegExpValidator>

#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/LineEdit.moc.h>

namespace HOOAdminTool
{

LoginDialog::LoginDialog(QWidget * parent)
:   QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint)
{
    setupUi();
}

LoginDialog::~LoginDialog()
{}

void LoginDialog::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QtToolbox::SmartGroupBox(this, "Login information");

    _address    = new QtToolbox::LineEdit(this, "Address");
    _port       = new QtToolbox::LineEdit(this, "Port");
    _port->setValidator(1, 65535);
    _login      = new QtToolbox::LineEdit(this, "Login");
    _login->setValidator("[A-Za-z0-9_]+");
    _password   = new QtToolbox::LineEdit(this, "Password");

    _buttonBox = new QDialogButtonBox(this);
    _buttonBox->setOrientation(Qt::Horizontal);
    _buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

    _groupBox->addWidget(_address);
    _groupBox->addWidget(_port);
    _groupBox->addWidget(_login);
    _groupBox->addWidget(_password);
    _groupBox->addWidget(_buttonBox, Qt::AlignRight);

    _layout->addWidget(_groupBox);

    connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));

    //TODO a effacer
    _address->setText("127.0.0.1");
    _port->setText("32674");
    _login->setText("bill");
    _password->setText("joe");
}

QString LoginDialog::getAddress() const
{
    return _address->text();
}

int LoginDialog::getPort() const
{
    int port = _port->text().toInt();
    return port;
}

QString LoginDialog::getLogin() const
{
    return _login->text();
}

QString LoginDialog::getPassword() const
{
    return _password->text();
}

}//namespace HOOAdminTool