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
#include "HOOAdminTool.moc.h"


#include <QGridLayout>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QTableWidget>
#include <QDockWidget>
#include <QHeaderView>
#include <QTextEdit>
#include <QLabel>
#include <QTimer>
#include <QCheckbox>
#include <QLineEdit>
#include <QtToolbox/ComboBox.moc.h>
#include <QDialogButtonBox>
#include <QInputDialog>

#include <Core/RT/Thread.h>
#include <Core/Logger.h>
#include <Core/Strings/Tools.h>
#include <Network/Message.h>

#include <QtToolBox/OutputWidgetLoggerStream.moc.h>

#include <Network/Message.h>
#include <Network/NetworkException.h>

#include <HOOAdminProtocol/AdminConnect.h>
#include <HOOAdminProtocol/AdminLogin.h>
#include <HOOAdminProtocol/AdminLogout.h>
#include <HOOAdminProtocol/ShutdownServer.h>
#include <HOOAdminProtocol/ServerStats.h>
#include <HOOAdminProtocol/BanUsers.h>
#include <HOOAdminProtocol/LockUsers.h>
#include <HOOAdminProtocol/SendMsgToUsers.h>

#include <HOOAdminTool/AdminRequestBar.moc.h>
#include <HOOAdminTool/AdminMessage.h>


#include <HOOAdminTool/Dialog/LoginDialog.moc.h>

namespace HOOAdminTool
{
const String LOGIN_SEPARATOR = L";";

void stringLoginToListLogins(String strLogins, Core::List<String>& logins)
{
    String temp;
    logins.clear();

    trimLeft(strLogins, temp, L" ");
    trimRight(temp, strLogins, L" ");

    Core::List<String> listUsers;
    Core::tokenize(strLogins, logins, LOGIN_SEPARATOR);
    
    for (int32 ii=0; ii<logins.size(); ++ii)
    {
        trimLeft(logins[ii], temp, L" ");
        trimRight(temp, logins[ii], L" ");
    }
}

HOOAdminToolConfig::HOOAdminToolConfig()
{
    serverAddress               = L"";
    serverPort                  = 31338;

    openSocketRetryCount        = 3;
    openSocketRetryWaitTime     = 5;
}

HOOAdminTool::HOOAdminTool(QWidget * parent)
:   QMainWindow(parent),
    _networkEngine(&_conditionVar, &_conditionMutex),
    _sessionId(-1)
{
    setupUi();

    _timer = new QTimer (this);
    _timer->setInterval(100);
    connect(_timer, SIGNAL(timeout()), this, SLOT(checkReceiveMsg()));

    resize(800, 800);
    initialize();
}

void HOOAdminTool::initialize()
{
    _usersInformations.clear();
    _sessionId = -1;
    _config = HOOAdminToolConfig();
    _adminRequestBar->initialize();
    updateUserList();
    _timer->stop();
}

HOOAdminTool::~HOOAdminTool()
{
    if(_adminRequestBar != NULL)
    {
        delete _adminRequestBar;
        _adminRequestBar = NULL;
    }
}

void HOOAdminTool::setupUi()
{
    QWidget* centralWidget = new QWidget(this);
    QGridLayout * layout = new QGridLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    
    _adminRequestBar = new AdminRequestBar(this);

    _pLoggerStream   = Ptr<QtToolbox::OutputWidgetLoggerStream> (new QtToolbox::OutputWidgetLoggerStream());
    Ptr<Core::TextOutputStream> tos ( new Core::TextOutputStream(_pLoggerStream, true));
    Core::LoggerManager::getInstance().getDefaultLogger()->addStream(tos, L"loggerOutputWidget");

    _pTextLog        = new QPlainTextEdit(NULL);
        _pTextLog->setReadOnly(true);
        _pTextLog->setLineWrapMode(QPlainTextEdit::NoWrap);
        QBrush brush (QColor(0,0,0));
            QTextCharFormat format;
            format.setFont(QFont("Consolas", 8));
            format.setForeground(brush);
        _pTextLog->setCurrentCharFormat(format);
        _pTextLog->setBackgroundVisible(false);
        
    _userList       = new QTableWidget (this);
    _userList->setSortingEnabled (true);
    
    layout->addWidget(_adminRequestBar, 0, 0);
    layout->addWidget(_userList,        0, 1);

    centralWidget->setLayout(layout);

    _dockLog = new QDockWidget(tr("Log"), this);
    _dockLog->setAllowedAreas(Qt::BottomDockWidgetArea);
    _dockLog->setFeatures(QDockWidget::NoDockWidgetFeatures);
    _dockLog->setWidget(_pTextLog);

    setCentralWidget(centralWidget);
    addDockWidget(Qt::BottomDockWidgetArea, _dockLog);


    connect(_adminRequestBar,     SIGNAL(doLogin()),                    this, SLOT(performLogin()));
    connect(_adminRequestBar,     SIGNAL(doLogout()),                   this, SLOT(performLogout()));
    connect(_adminRequestBar,     SIGNAL(doShutDownServer()),           this, SLOT(performShutDownServer()));
    connect(_adminRequestBar,     SIGNAL(doRefreshUsersInfo()),         this, SLOT(performRedreshUsersInfo()));
    connect(_adminRequestBar,     SIGNAL(doServerStats()),              this, SLOT(performServerStats()));
    connect(_adminRequestBar,     SIGNAL(doBanUsers()),                 this, SLOT(performBanUsers()));
    connect(_adminRequestBar,     SIGNAL(doUnbanUsers()),               this, SLOT(performUnbanUsers()));
    connect(_adminRequestBar,     SIGNAL(doLockUsers()),                this, SLOT(performLockUsers()));
    connect(_adminRequestBar,     SIGNAL(doUnlockUsers()),              this, SLOT(performUnlockUsers()));
    connect(_adminRequestBar,     SIGNAL(doSendMessageToUsers()),       this, SLOT(performSendMessageToUsers()));

    connect(_pLoggerStream.get(), SIGNAL(newText(const Core::String&)), this, SLOT(newLogMsg(const Core::String&)));
}

Core::List<HOOAdminProtocol::UserInfos> HOOAdminTool::getUsersChecked()
{
    Core::List<HOOAdminProtocol::UserInfos> usersChecked;

    int32 nbRow = _userList->rowCount();

    LM_ASSERT(nbRow == _usersInformations.size());

    for (int32 ii=0; ii<nbRow; ++ii)
    {
        QCheckBox* cb = ( QCheckBox*) _userList->cellWidget(ii, 0);
        if (cb->isChecked())
        {
            String login (_userList->item(ii, 1)->text().toStdString().c_str());

            //TODO : actuellement la liste n'est pas trie donc on recherche de maniere bien naive
            for (int32 iInfo = 0; iInfo<_usersInformations.size(); iInfo++)
            {
                if (_usersInformations[iInfo].login == login)
                {
                    usersChecked.push_back(_usersInformations[iInfo]);
                    break;
                }
            }
        }
    }

    return usersChecked;
}

String HOOAdminTool::getLoginsCheckedUsers()
{
    String loginsChecked;

    Core::List<HOOAdminProtocol::UserInfos> usersChecked = getUsersChecked();

    for (int32 ii=0; ii<usersChecked.size(); ++ii)
        loginsChecked += usersChecked[ii].login + LOGIN_SEPARATOR + L" ";

    return loginsChecked;
}

void HOOAdminTool::updateUserList()
{
    _userList->clear();
    _userList->setRowCount(0);
    
    _userList->setColumnCount(3);
    QStringList columnName;
        columnName.push_back("");
        columnName.push_back("Login");
        columnName.push_back("MatchStatus");
    _userList->setHorizontalHeaderLabels(columnName);
    _userList->verticalHeader()->show();

    _userList->setColumnWidth(0, 30);
    _userList->setColumnWidth(1, 400);
    _userList->setColumnWidth(2, 75);

    _userList->sortItems (1, Qt::AscendingOrder );

    for (int32 ii=0; ii<_usersInformations.size(); ++ii)
    {
        if(_usersInformations[ii].loggedIn)
        {
            int32 row = _userList->rowCount();
            _userList->insertRow(row);

            _userList->setCellWidget(row, 0, new QCheckBox);
            _userList->setItem(row, 1, new QTableWidgetItem(Core::String8(_usersInformations[ii].login).c_str()));
            _userList->setItem(row, 2, new QTableWidgetItem(Core::String8(Core::toString(_usersInformations[ii].qMatchStatus)).c_str()));
        }
    }
}

void HOOAdminTool::newLogMsg(const Core::String& str)
{
    _pTextLog->insertPlainText(String8(str).c_str());
    _pTextLog->repaint();
}

void HOOAdminTool::performLogin()
{
    LoginDialog loginDialog(this);
    QString     addressQStr;;
    int32       port;
    QString     loginQStr;
    QString     passwordQStr;;

    if(loginDialog.exec())
    {
        addressQStr  = loginDialog.getAddress();
        port         = loginDialog.getPort();
        loginQStr    = loginDialog.getLogin();
        passwordQStr = loginDialog.getPassword();
    

        Core::String addressStr     (addressQStr.toStdString().c_str());
        Core::String loginStr       (loginQStr.toStdString().c_str());
        Core::String passwordStr    (passwordQStr.toStdString().c_str());

        _config.serverAddress = addressStr;
        _config.serverPort    = port;

        if(!openSocket())                   {initialize(); return;}
        if(!connectToServer())              {initialize(); return;}
        if(!login(loginStr, passwordStr))   {initialize(); return;}

        performRedreshUsersInfo();
        _adminRequestBar->enabledAdminAction(true);
        _timer->start();
    }
    else
    {
        initialize();
    }
}

void HOOAdminTool::performLogout()
{
    logout();
    disconnectToServer();
    initialize();
}

void HOOAdminTool::performShutDownServer()
{
    bool ok;
    int32 nbMin = QInputDialog::getInt(this, tr("ShutDown"),
                                  tr("Minutes before ShutDown :"), 30, 0, 1440, 1, &ok);

    if (ok == false)
        return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("ShutDown"),
                                     "ShutDown server, really ?",
                                     QMessageBox::No | QMessageBox::Yes);

     if (reply == QMessageBox::No)
         return;

    int32 nbSecond = nbMin*60;
    shutDownServer(nbSecond);
}

void HOOAdminTool::performRedreshUsersInfo()
{
    HOOAdminProtocol::ConnectedUsersInfos req;
    send(req);

    HOOAdminProtocol::ConnectedUsersInfosAnswer answer;
    receive(answer);

    _usersInformations = answer.getUsersInfos();
    INF << L"users informations received, nb users connected : " << Core::toString(_usersInformations.size());

    updateUserList();
}

void HOOAdminTool::performServerStats()
{
    HOOAdminProtocol::ServerStats req;
    send(req);

    HOOAdminProtocol::ServerStatsAnswer answer;
    receive(answer);

    QDialog dlg (this);
        QGridLayout layoutDlg(&dlg);
        QLabel      statsLabel("Server Stats");   
        QTextEdit   text(&dlg);
        text.setReadOnly(true);
        layoutDlg.addWidget(&statsLabel);
        layoutDlg.addWidget(&text);
        dlg.setLayout(&layoutDlg);

    text.setText(Core::String8(answer.getStats()).c_str());
    dlg.exec();
}

void HOOAdminTool::performBanUsers()
{
    QDialog dlg (this);
        dlg.setFixedSize(800, 150);
        QGridLayout             layoutDlg(&dlg);
        QLabel                  loginsLabel("logins:");
        QLineEdit               logins;
        QtToolbox::ComboBox     timeDuration(&dlg, "time");
        QDialogButtonBox buttonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        layoutDlg.addWidget(&loginsLabel);
        layoutDlg.addWidget(&logins);
        layoutDlg.addWidget(&timeDuration);
        layoutDlg.addWidget(&buttonBox);
        layoutDlg.setAlignment(Qt::AlignTop);
        dlg.setLayout(&layoutDlg);
        timeDuration.addItem(L"ban for ever");
        for (int32 iDay=1; iDay<=31; iDay++)
            timeDuration.addItem(L"ban for "+ Core::toString(iDay) + L"day");
        

    String loginsChecked = getLoginsCheckedUsers();
    logins.setText(Core::String8(loginsChecked).c_str());


    if (dlg.exec() == QDialog::Accepted)
    {
        String loginsStr = String(logins.text().toStdString().c_str());
        Core::List<String> loginsList;
        stringLoginToListLogins(loginsStr, loginsList);

        int32 nbDay = timeDuration.selectedIndex();
        HOOAdminProtocol::BanUsers req (loginsList, nbDay);
        send(req);

        String strInfo = L"Ban Users send with logins : \n";
        for (int32 ii=0; ii<loginsList.size(); ++ii)
            strInfo += loginsList[ii] + L"\n";
        INF << strInfo;

        HOOAdminProtocol::BanUsersAnswer answer;
        receive(answer);

        const Core::List<String> & logins = answer.getLogins();
        const Core::List<bool> & banned = answer.getBanned();
        LM_ASSERT(logins.size() == banned.size());
        String results = L"Ban Users results : \n";
        for (int32 ii=0; ii<logins.size(); ++ii)
        {            
            results << logins[ii] << L"\t";
            if(banned[ii])
                results << L"banned" <<L"\n";
            else
                results << L"user unknown" <<L"\n";
        }
        INF << results;

        performRedreshUsersInfo();  //on actualise la liste des utilisateurs connecte
    }
}

void HOOAdminTool::performUnbanUsers()
{
    QDialog dlg (this);
        dlg.setFixedSize(800, 150);
        QGridLayout             layoutDlg(&dlg);
        QLabel                  loginsLabel("logins:");
        QLineEdit               logins;
        QDialogButtonBox buttonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        layoutDlg.addWidget(&loginsLabel);
        layoutDlg.addWidget(&logins);
        layoutDlg.addWidget(&buttonBox);
        layoutDlg.setAlignment(Qt::AlignTop);
        dlg.setLayout(&layoutDlg);
        

    String loginsChecked = getLoginsCheckedUsers();
    logins.setText(Core::String8(loginsChecked).c_str());

    if (dlg.exec() == QDialog::Accepted)
    {
        String loginsStr = String(logins.text().toStdString().c_str());
        Core::List<String> loginsList;
        stringLoginToListLogins(loginsStr, loginsList);

        HOOAdminProtocol::UnbanUsers req (loginsList);
        send(req);

        String strInfo = L"Unban Users send with logins : \n";
        for (int32 ii=0; ii<loginsList.size(); ++ii)
            strInfo += loginsList[ii] + L"\n";
        INF << strInfo;

        HOOAdminProtocol::UnbanUsersAnswer answer;
        receive(answer);

        const Core::List<String> & logins = answer.getLogins();
        const Core::List<bool> & unbanned = answer.getUnbanned();
        LM_ASSERT(logins.size() == unbanned.size());
        String results = L"Unban Users results : \n";
        for (int32 ii=0; ii<logins.size(); ++ii)
        {            
            results << logins[ii] << L"\t";
            if(unbanned[ii])
                results << L"unbanned" <<L"\n";
            else
                results << L"user unknown" <<L"\n";
        }
        INF << results;
    }
}

void HOOAdminTool::performLockUsers()
{
    QDialog dlg (this);
        dlg.setFixedSize(800, 150);
        QGridLayout             layoutDlg(&dlg);
        QLabel                  loginsLabel("logins:");
        QLineEdit               logins;
        QDialogButtonBox buttonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        layoutDlg.addWidget(&loginsLabel);
        layoutDlg.addWidget(&logins);
        layoutDlg.addWidget(&buttonBox);
        layoutDlg.setAlignment(Qt::AlignTop);
        dlg.setLayout(&layoutDlg);
        

    String loginsChecked = getLoginsCheckedUsers();
    logins.setText(Core::String8(loginsChecked).c_str());


    if (dlg.exec() == QDialog::Accepted)
    {
        String loginsStr = String(logins.text().toStdString().c_str());
        Core::List<String> loginsList;
        stringLoginToListLogins(loginsStr, loginsList);

        HOOAdminProtocol::LockUsers req (loginsList);
        send(req);

        String strInfo = L"Lock Users send with logins : \n";
        for (int32 ii=0; ii<loginsList.size(); ++ii)
            strInfo += loginsList[ii] + L"\n";
        INF << strInfo;

        HOOAdminProtocol::LockUsersAnswer answer;
        receive(answer);

        const Core::List<String> & logins = answer.getLogins();
        const Core::List<bool> & locked = answer.getLocked();
        LM_ASSERT(logins.size() == locked.size());
        String results = L"Lock Users results : \n";
        for (int32 ii=0; ii<logins.size(); ++ii)
        {            
            results << logins[ii] << L"\t";
            if(locked[ii])
                results << L"locked" <<L"\n";
            else
                results << L"user unknown" <<L"\n";
        }
        INF << results;

        performRedreshUsersInfo();  //on actualise la liste des utilisateurs connecte
    }
    
}

void HOOAdminTool::performUnlockUsers()
{
    QDialog dlg (this);
        dlg.setFixedSize(800, 150);
        QGridLayout             layoutDlg(&dlg);
        QLabel                  loginsLabel("logins:");
        QLineEdit               logins;
        QDialogButtonBox buttonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        layoutDlg.addWidget(&loginsLabel);
        layoutDlg.addWidget(&logins);
        layoutDlg.addWidget(&buttonBox);
        layoutDlg.setAlignment(Qt::AlignTop);
        dlg.setLayout(&layoutDlg);
        

    String loginsChecked = getLoginsCheckedUsers();
    logins.setText(Core::String8(loginsChecked).c_str());

    if (dlg.exec() == QDialog::Accepted)
    {
        String loginsStr = String(logins.text().toStdString().c_str());
        Core::List<String> loginsList;
        stringLoginToListLogins(loginsStr, loginsList);

        HOOAdminProtocol::UnlockUsers req (loginsList);
        send(req);

        String strInfo = L"Unlock Users send with logins : \n";
        for (int32 ii=0; ii<loginsList.size(); ++ii)
            strInfo += loginsList[ii] + L"\n";
        INF << strInfo;

        HOOAdminProtocol::UnlockUsersAnswer answer;
        receive(answer);

        const Core::List<String> & logins = answer.getLogins();
        const Core::List<bool> & unlocked = answer.getUnlocked();
        LM_ASSERT(logins.size() == unlocked.size());
        String results = L"Unlock Users results : \n";
        for (int32 ii=0; ii<logins.size(); ++ii)
        {            
            results << logins[ii] << L"\t";
            if(unlocked[ii])
                results << L"unlocked" <<L"\n";
            else
                results << L"user unknown" <<L"\n";
        }
        INF << results;
    }
}

void HOOAdminTool::performSendMessageToUsers()
{
    QDialog dlg (this);
        dlg.setFixedSize(800, 800);
        QGridLayout             layoutDlg(&dlg);
        QCheckBox               forAllUsers ("Send To All Users");
        QLabel                  loginsLabel("logins:");
        QLineEdit               logins;
        QLabel                  msgLabel("message:");
        QTextEdit               messageForUser;
        QDialogButtonBox buttonBox (QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        connect(&buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        connect(&buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));
        layoutDlg.addWidget(&forAllUsers);
        layoutDlg.addWidget(&loginsLabel);
        layoutDlg.addWidget(&logins);
        layoutDlg.addWidget(&msgLabel);
        layoutDlg.addWidget(&messageForUser);
        layoutDlg.addWidget(&buttonBox);
        layoutDlg.setAlignment(Qt::AlignTop | Qt::AlignLeft);
        dlg.setLayout(&layoutDlg);
        

    String loginsChecked = getLoginsCheckedUsers();
    logins.setText(Core::String8(loginsChecked).c_str());


    if (dlg.exec() == QDialog::Accepted)
    {
        String loginsStr = String(logins.text().toStdString().c_str());
        Core::List<String> loginsList;
        stringLoginToListLogins(loginsStr, loginsList);

        String textMsg = String(messageForUser.toPlainText().toStdString().c_str());
        HOOAdminProtocol::SendMessageToUsers req (textMsg, forAllUsers.isChecked(), loginsList);
        send(req);

        INF << L"Message send to Users : " + textMsg;
    }
}

String HOOAdminTool::toStringCount(int32 c)
{
    if(c == -1)
        return L"INFINITE";
    else
        return Core::toString(c);
}

bool HOOAdminTool::openSocket()
{
    bool result = false;
    int32 tryCount = 0;

    do
    {
        if(tryCount != 0)
        {
            INF << L"Socket open failed, will retry in " << Core::toString(_config.openSocketRetryWaitTime) << L" s";
            Core::Thread::sleep(_config.openSocketRetryWaitTime * 1000);
        }

        tryCount ++;
        INF << L"Opening socket " << _config.serverAddress << L":" << Core::toString(_config.serverPort)
            << L" try " << Core::toString(tryCount) << L"/" << toStringCount(_config.openSocketRetryCount) << L"...";
        _sessionId = _networkEngine.connect(_config.serverAddress, _config.serverPort);

    } while(tryCount < _config.openSocketRetryCount && _sessionId == -1);


    if(_sessionId != -1)
    {
        INF << L"Socket opened with sessionId " << Core::toString(_sessionId);
        _networkEngine.launch();
        result = true;
    }
    else
    {
        ERR << L"Socket open failed";
        QMessageBox::critical(this, "error", "Socket open failed");
    }

    return result;
}

void HOOAdminTool::send(HOOAdminProtocol::IAdminRequest & message)
{
    Network::Message messageToSend(Network::APPLICATION_MESSAGE, message, _sessionId);
    _networkEngine.sendMessage(messageToSend);
}

bool HOOAdminTool::receive(HOOAdminProtocol::IAdminAnswer & message, bool blocking)
{
    bool messageToProcess = false;
    
    do
    {
        Network::Message * incomingMessage = NULL;

        if(blocking)
        {
            Core::ScopedCondLock lock(_conditionMutex);
            while(!_networkEngine.getMessage(incomingMessage))
                _conditionVar.wait(lock);

            messageToProcess = true;
        }
        else
        {
            Core::ScopedCondLock lock(_conditionMutex);
            messageToProcess = _networkEngine.getMessage(incomingMessage);
        }

        if(incomingMessage != NULL)
        {
            Network::EMessageType messageType = incomingMessage->getMessageType();

            switch(messageType)
            {
            case Network::NETWORK_CLOSE_SESSION_MESSAGE:
                ERR << L"The network connection was remotely closed!";
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            case Network::APPLICATION_MESSAGE:
                {
                    if(incomingMessage->getDataSize() >= sizeof(HOOAdminProtocol::EAdminAnswerType))
                    {
                        HOOAdminProtocol::EAdminAnswerType answerType;
                        incomingMessage->readData(&answerType, sizeof(HOOAdminProtocol::EAdminAnswerType));

                        if(answerType == message.getType())
                            incomingMessage->getMessage(message);
                        else
                        {
                            INF << L"Ignoring message " << HOOAdminProtocol::toString(answerType) << L"\n";
                            messageToProcess = false;
                        }
                    }
                    else
                    {
                        ERR << L"Received invalid message!";
                        throw Network::NetworkException(L"Received invalid message!");
                    }
                }
                break;
            default:
                ERR << L"The network connection was remotely closed!";
                throw Network::NetworkException(L"The network connection was remotely closed!");
                break;
            }

            delete incomingMessage;
            incomingMessage = NULL;
        }
    } while(blocking && !messageToProcess);

    return messageToProcess;
}

bool HOOAdminTool::connectToServer()
{
    bool result = false;

    {
        INF << L"Sending connection request\n";
        HOOAdminProtocol::AdminConnect connectMsg;
        send(connectMsg);
    }
    {
        HOOAdminProtocol::AdminConnectAnswer connectAnswer;
        receive(connectAnswer);

        INF << L"Connect answer is " << HOOAdminProtocol::toString(connectAnswer.getConnectAnswerType()) << L"\n";

        result = (connectAnswer.getConnectAnswerType() == HOOAdminProtocol::ADMIN_CONNECT_OK);
        if (result==false)
        {
            ERR << L"connect to server failed";
            QMessageBox::critical(this, "error", "connect to server failed");
        }
    }

    return result;
}

void HOOAdminTool::disconnectToServer()
{
    INF << L"Sending disconnection request\n";
    Network::Message msg (Network::NETWORK_CLOSE_SESSION_MESSAGE, _sessionId);
    _networkEngine.sendMessage(msg);
}

bool HOOAdminTool::login(const String& loginStr, const String& passwordStr)
{
    bool result = false;

    {
        HOOAdminProtocol::AdminLogin adminLogin (loginStr, passwordStr);
        send(adminLogin);
    }

    {
        HOOAdminProtocol::AdminLoginAnswer answer;
        receive(answer);

        result = (answer.getLoginAnswerType() == HOOAdminProtocol::ADMIN_LOGIN_OK);
        if (result==false)
        {
            ERR << L"login to server failed";
            QMessageBox::critical(this, "error", "login to server failed");
        }
    }

    return result;
}

void HOOAdminTool::logout()
{
    HOOAdminProtocol::AdminLogout adminLogout;
    send(adminLogout);
    INF << L"logout to server send";
    _usersInformations.clear();
    updateUserList();
}

void HOOAdminTool::shutDownServer(int32 nbSecond)
{
    HOOAdminProtocol::ShutdownServer msg (nbSecond);
    send(msg);
    INF << L"shutdown to server send";
}

void HOOAdminTool::checkReceiveMsg()
{
    bool messageToProcess = false;
    
    Network::Message * incomingMessage = NULL;

    Core::ScopedCondLock lock(_conditionMutex);
    messageToProcess = _networkEngine.getMessage(incomingMessage);

    if (messageToProcess == true && incomingMessage != NULL)
    {
        Network::EMessageType messageType = incomingMessage->getMessageType();
        if (messageType == Network::NETWORK_CLOSE_SESSION_MESSAGE)
        {
            initialize();
            INF << L"receive " + Network::toString(messageType);
        }
        else
        {
            Core::String message;
            message << L"Timer expected NETWORK_CLOSE_SESSION_MESSAGE and received : " << Network::toString(messageType);
            ERR << message;
            throw Network::NetworkException(message);
        }
    }
}

}//namespace HOOAdminTool