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
#include "AdminRequestBar.moc.h"

#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QMessageBox>



#include <QtToolbox/SmartGroupBox.moc.h>
#include <QtToolbox/NumberEdit.moc.h>


namespace HOOAdminTool
{

AdminRequestBar::AdminRequestBar(QWidget * parent)
:   QWidget(parent)
{
    setupUi();
}

AdminRequestBar::~AdminRequestBar()
{}

void AdminRequestBar::initialize()
{
    enabledAdminAction(false);

    disconnect(_login_logout,       SIGNAL(toggled(bool)), this, SLOT(logInOut(bool)));
    _login_logout->setChecked(false);
    connect(_login_logout,       SIGNAL(toggled(bool)), this, SLOT(logInOut(bool)));
}

void AdminRequestBar::setupUi()
{
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);



    _login_logout = new QPushButton("Login", this);
    _login_logout->setFixedSize(120, 20);
    _login_logout->setCheckable(true);

    _refreshUsersInfo = new QPushButton("Refresh users info", this);
    _refreshUsersInfo->setFixedSize(120, 20);
    
    _shutDownServer = new QPushButton("Shutdown server", this);
    _shutDownServer->setFixedSize(120, 20);

    _serverStats = new QPushButton("Server stats", this);
    _serverStats->setFixedSize(120, 20);

    _banUsers = new QPushButton("Ban users", this);
    _banUsers->setFixedSize(120, 20);

    _unbanUsers = new QPushButton("Unban users", this);
    _unbanUsers->setFixedSize(120, 20);

    _lockUsers = new QPushButton("Lock users", this);
    _lockUsers->setFixedSize(120, 20);

    _unlockUsers = new QPushButton("Unlock users", this);
    _unlockUsers->setFixedSize(120, 20);

    _sendMessageToUsers = new QPushButton("Send message to users", this);
    _sendMessageToUsers->setFixedSize(120, 20);

    _layout->addWidget(_login_logout);
    _layout->addWidget(_refreshUsersInfo);
    _layout->addWidget(_serverStats);
    _layout->addWidget(_banUsers);
    _layout->addWidget(_unbanUsers);
    _layout->addWidget(_lockUsers);
    _layout->addWidget(_unlockUsers);
    _layout->addWidget(_sendMessageToUsers);
    _layout->addWidget(_shutDownServer);
    _layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    setLayout(_layout);

    connect(_login_logout,       SIGNAL(toggled(bool)), this, SLOT(logInOut(bool)));
    connect(_refreshUsersInfo,   SIGNAL(clicked()),     this, SIGNAL(doRefreshUsersInfo()));
    connect(_shutDownServer,     SIGNAL(clicked()),     this, SIGNAL(doShutDownServer()));
    connect(_serverStats,        SIGNAL(clicked()),     this, SIGNAL(doServerStats()));
    connect(_banUsers,           SIGNAL(clicked()),     this, SIGNAL(doBanUsers()));
    connect(_unbanUsers,         SIGNAL(clicked()),     this, SIGNAL(doUnbanUsers()));
    connect(_lockUsers,          SIGNAL(clicked()),     this, SIGNAL(doLockUsers()));
    connect(_unlockUsers,        SIGNAL(clicked()),     this, SIGNAL(doUnlockUsers()));
    connect(_sendMessageToUsers, SIGNAL(clicked()),     this, SIGNAL(doSendMessageToUsers()));

    enabledAdminAction(false);
}

void AdminRequestBar::enabledAdminAction(bool flag)
{
    _refreshUsersInfo->setEnabled(flag);
    _shutDownServer->setEnabled(flag);
    _serverStats->setEnabled(flag);
    _banUsers->setEnabled(flag);
    _unbanUsers->setEnabled(flag);
    _lockUsers->setEnabled(flag);
    _unlockUsers->setEnabled(flag);
    _sendMessageToUsers->setEnabled(flag);
}

void AdminRequestBar::logInOut(bool flag)
{
    if (flag == true)
    {
        emit doLogin();
    }
    else
    {
        emit doLogout();
    }
}

}//namespace HOOAdminTool