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
#ifndef HOOADMINTOOL_HOOADMINTOOL_H_
#define HOOADMINTOOL_HOOADMINTOOL_H_

#include <QMainWindow>
#include <Network/NetworkEngine.h>

#include <HOOAdminProtocol/IAdminMessage.h>
#include <HOOAdminProtocol/ConnectedUsersInfos.h>

class QGridLayout;
class QPlainTextEdit;
class QTableWidget;
class QDockWidget;
namespace QtToolbox
{
    class OutputWidgetLoggerStream;
}

namespace HOOAdminTool
{

class AdminRequestBar;
class DisplayTool;
class AdminMessage;

struct HOOAdminToolConfig
{
    HOOAdminToolConfig();

    String          serverAddress;
    int32           serverPort;

    int32           openSocketRetryCount;
    int32           openSocketRetryWaitTime; // Attente entre les essais en SECONDES
};

class LM_API_HAT HOOAdminTool : public QMainWindow
{
    Q_OBJECT

public:
    HOOAdminTool(QWidget * parent = 0);

    virtual ~HOOAdminTool();

    AdminMessage * getAdminMessage() { return _adminMessage; }

protected:
    bool openSocket();
    bool connectToServer();
    void disconnectToServer();
    bool login(const String& loginStr, const String& passwordStr);
    void logout();
    void shutDownServer(int32 nbSecond);
    void send(HOOAdminProtocol::IAdminRequest & message);
    bool receive(HOOAdminProtocol::IAdminAnswer & message, bool blocking = true);

protected Q_SLOTS:
    void performLogin();
    void performLogout();
    void performShutDownServer();
    void performRedreshUsersInfo();
    void performServerStats();
    void performBanUsers();
    void performUnbanUsers();
    void performLockUsers();
    void performUnlockUsers();
    void performSendMessageToUsers();
    void newLogMsg(const Core::String&);
    void checkReceiveMsg();

private:
    void setupUi();
    Core::String toStringCount(int32 c);
    void updateUserList();
    void initialize();
    String getLoginsCheckedUsers();
    Core::List<HOOAdminProtocol::UserInfos> getUsersChecked();

private:
    HOOAdminToolConfig                          _config;
    AdminRequestBar *                           _adminRequestBar;
    AdminMessage *                              _adminMessage;

    Core::ConditionMutex                        _conditionMutex;
    Core::ConditionVar                          _conditionVar;
    Network::NetworkEngine                      _networkEngine;

    int32                                       _sessionId;

    QDockWidget *                               _dockLog;
    QPlainTextEdit *                            _pTextLog;
    Ptr<QtToolbox::OutputWidgetLoggerStream>    _pLoggerStream;
    QTableWidget *                              _userList;

    Core::List<HOOAdminProtocol::UserInfos>     _usersInformations;

    QTimer *                                    _timer;
};

}//namespace HOOAdminTool

#endif/*HOOADMINTOOL_HOOADMINTOOL_H_*/