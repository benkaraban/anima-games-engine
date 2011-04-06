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
#ifndef HOOADMINTOOL_LOGINDIALOG_H_
#define HOOADMINTOOL_LOGINDIALOG_H_

#include <QDialog>

class QGridLayout;
class QDialogButtonBox;

namespace QtToolbox
{
class SmartGroupBox;
class LineEdit;
}

namespace HOOAdminTool
{

class LM_API_HAT LoginDialog : public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QWidget * parent = 0);
    virtual ~LoginDialog();

    void setupUi();

    QString getAddress() const;
    int getPort() const;
    QString getLogin() const;
    QString getPassword() const;

private:
    QGridLayout *               _layout;
    QtToolbox::SmartGroupBox *  _groupBox;
    QtToolbox::LineEdit *       _address;
    QtToolbox::LineEdit *       _port;
    QtToolbox::LineEdit *       _login;
    QtToolbox::LineEdit *       _password;

    QDialogButtonBox *          _buttonBox;
};

}//namespace HOOAdminTool

#endif /*HOOADMINTOOL_LOGINDIALOG_H_*/