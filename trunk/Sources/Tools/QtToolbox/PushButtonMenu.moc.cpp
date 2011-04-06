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
#include "PushButtonMenu.moc.h"

#include <QMouseEvent>
#include <QMenu>
#include <QTimer>
#include <QAction>
#include <QPainter>

namespace QtToolbox
{
//-----------------------------------------------------------------------------
PushButtonMenu::PushButtonMenu(const QList <QAction*>&    actions, QWidget *parent, 
                               const Core::TimeDuration&  timeForShowMenu, 
                               const QString & name, const QString & defaultValue) :
    QPushButton(parent),
    _timeForShowMenu(timeForShowMenu)
{
    
    _timer = new QTimer(this);

    setActions(actions);

    connect(_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));
    
    setupUi();
}
//-----------------------------------------------------------------------------
PushButtonMenu::PushButtonMenu( QWidget *parent,
                const Core::TimeDuration&  timeForShowMenu,
                const QString & name,
                const QString & defaultValue):
    QPushButton(parent),
    _timeForShowMenu(timeForShowMenu)
{
    _timer = new QTimer(this);

    connect(_timer, SIGNAL(timeout()), this, SLOT(timerTimeout()));

    setupUi();
}
//-----------------------------------------------------------------------------
PushButtonMenu::~PushButtonMenu()
{
    delete _timer;

    for (int32 iAction=0; iAction<_actions.size(); ++iAction)
    {
        QAction* pAct = _actions[iAction];
        delete pAct;
    }
}
//-----------------------------------------------------------------------------
void PushButtonMenu::setActions(const QList <QAction*>& actions)
{
    for (int32 iAction=0; iAction<_actions.size(); ++iAction)
    {
        QAction* pAct = _actions[iAction];
        delete pAct;
    }

    _actions.clear();

    //on recopie les actions
    QList <QAction*>::const_iterator itAction = actions.begin();
    QList <QAction*>::const_iterator itActionEnd = actions.end();
    while(itAction!=itActionEnd)
    {
        QAction* pAct = new QAction(QIcon((*itAction)->icon()), (*itAction)->text(), this);
        pAct->setShortcut((*itAction)->shortcut());
        pAct->setShortcutContext((*itAction)->shortcutContext());

        _actions.push_back(pAct);
        ++itAction;
    }

    _indexActionActivated = 0;

    if (_actions.size()>1)
    {
        setIcon(_actions[0]->icon());
    }

    //addActions(_actions);
    for (int32 ii=0; ii<_actions.size(); ++ii)
    {
        connect (_actions[ii], SIGNAL(triggered()), this, SLOT(actionTriggered()));
    }
}
//-----------------------------------------------------------------------------
void PushButtonMenu::actionTriggered()
{
    int32 newAction = _indexActionActivated;
    for (int32 iAction=0; iAction<_actions.size(); ++iAction)
    {
        if (sender () == _actions[iAction])
        {
            newAction =  iAction;
            break;
        }
    }

    if (newAction == _indexActionActivated)
    {
        if (isDown() == true)
            setDown(false);
        else
            setDown(true);

        emit actionChanged(_indexActionActivated, isDown());
    }
    else
    {
        setIcon(_actions[newAction]->icon());
        _indexActionActivated = newAction;
        //this->setChecked(true);
        this->setDown(true);
        emit actionChanged(_indexActionActivated, isDown());
    }
}
//-----------------------------------------------------------------------------
void PushButtonMenu::setupUi()
{
    _indexActionActivated = 0;

    setContentsMargins(0,0,0,0);
    
    int32 val = std::min(width(),  height());
    setIconSize(QSize(val-6, val-6));
}
//-----------------------------------------------------------------------------
void PushButtonMenu::mousePressEvent(QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        QPoint p(event->globalPos().x()-5, event->globalPos().y()-5);
        _posMenu = p;
        _dateStartTimer = Core::Clock::universalTime();

        _timer->start();
    }
    else
        QPushButton::mousePressEvent(event);
}
//-----------------------------------------------------------------------------
void PushButtonMenu::actualizeIHM(int32 index, bool isDown)
{
    LM_ASSERT(index>=0 && index<_actions.size());

     setIcon(_actions[index]->icon());
    setDown(isDown);
}
//-----------------------------------------------------------------------------
void PushButtonMenu::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() & Qt::LeftButton)
    {
        _timer->stop();
        

       // setDown(!isDown());
        //emit actionChanged(_indexActionActivated, isDown());
        setDown(!isDown());
        emit actionChanged(_indexActionActivated, isDown());
//        QPushButton::mouseReleaseEvent(event);
        //_actions[_indexActionActivated]->trigger();
    }
    else
    {
        QPushButton::mouseReleaseEvent(event);
    }

}
//-----------------------------------------------------------------------------
void PushButtonMenu::timerTimeout()
{
    Core::TimeValue dateTimeout = Core::Clock::universalTime();

    Core::TimeDuration td = dateTimeout-_dateStartTimer;
    
    if (td > _timeForShowMenu)
    {
        _timer->stop();

        QMenu menu;

        menu.addActions(_actions);
        
        menu.setActiveAction(_actions[_indexActionActivated]);
        QAction * ret = menu.exec(_posMenu, _actions[_indexActionActivated]);
    }
}
//-----------------------------------------------------------------------------
void PushButtonMenu::setIcon(const QIcon & icon)
{
    QIcon myIcon (icon);
    
    QPixmap pixmap = myIcon.pixmap (QSize(256,256), QIcon::Normal, QIcon::Off);

    QPainter painter(&pixmap);
    painter.setPen(Qt::black);
    painter.setBrush(QBrush(Qt::black));
    
    static const QPointF points[3] = {
     QPointF(0.7*pixmap.width(), pixmap.height()),
     QPointF(pixmap.width(), pixmap.height()),
     QPointF(pixmap.width(), 0.7*pixmap.height())};


     painter.drawPolygon(points, 3);


    myIcon.addPixmap  (pixmap, QIcon::Normal, QIcon::Off);

    QPushButton::setIcon(myIcon);

    int32 val = std::min(width(),  height());
    setIconSize(QSize(val-6, val-6));
}
//-----------------------------------------------------------------------------
}//namespace QtToolbox
