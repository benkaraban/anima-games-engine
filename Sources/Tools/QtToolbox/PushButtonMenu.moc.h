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
#ifndef PUSH_BUTTON_MENU_H_
#define PUSH_BUTTON_MENU_H_

#include <QPushButton>

#include <Core/Standard.h>
#include <Core/DateAndTime.h>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
class LM_API_QTB PushButtonMenu : public QPushButton
{
    Q_OBJECT

public:
    PushButtonMenu(
                const QList <QAction*>&    actions,
                QWidget *parent = 0,
                const Core::TimeDuration&  timeForShowMenu = Core::microseconds(0.5*1000000),
                const QString & name = "",
                const QString & defaultValue = "");

    PushButtonMenu(
                QWidget *parent = 0,
                const Core::TimeDuration&  timeForShowMenu = Core::microseconds(0.5*1000000),
                const QString & name = "",
                const QString & defaultValue = "");

    void setActions(const QList <QAction*>& actions);
    virtual ~PushButtonMenu();

    virtual void setupUi();

    virtual void setIcon  (const QIcon & icon);
   
    virtual void mousePressEvent   (QMouseEvent* event);
    virtual void mouseReleaseEvent (QMouseEvent* event);

protected:
    void actualizeIHM(int32 index, bool isDown);

private Q_SLOTS:
    void timerTimeout();
    void actionTriggered();

Q_SIGNALS:
    void actionChanged(int32 _index, bool isChecked);

protected:
    Core::TimeDuration  _timeForShowMenu;   //le temp a attendre avant de montrer le menu
    QPoint              _posMenu;
    Core::TimeValue     _dateStartTimer;
    QList <QAction*>    _actions;
    int32               _indexActionActivated;
    QTimer*             _timer;
};
//-----------------------------------------------------------------------------
}//namespace QtToolbox

#endif/*PUSH_BUTTON_MENU_H_*/
