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
#ifndef APPLICATIONWINDOWBASE_H_
#define APPLICATIONWINDOWBASE_H_

#include <QMainWindow>

#include <Core/Standard.h>

#include <EPI/ControllerMain.moc.h>

class QAction;
namespace EPI
{
/**
* La classe implemente la fenetre principale de l'application qui sera fermee automatiquement lorsque l'application est quittee
*/
class LM_API_EPI ApplicationWindowBase : public QMainWindow
{
    
public:
    virtual ~ApplicationWindowBase();

public:
    ApplicationWindowBase(const Ptr<ControllerMain>& pCtrl, QWidget * parent = 0, Qt::WindowFlags flags = 0);

    const Ptr<ControllerMain>& getControllerMain() const {return _pCtrl;}
    Ptr<ControllerMain>& getControllerMain() {return _pCtrl;}
    virtual void keyPressEvent(QKeyEvent* event);
    virtual void keyReleaseEvent(QKeyEvent* event);
private:
    Ptr<ControllerMain> _pCtrl;

    QAction* actionKeyPad0;
    QAction* actionKeyPad1;
    QAction* actionKeyPad2;
    QAction* actionKeyPad3;
    QAction* actionKeyPad4;
    QAction* actionKeyPad5;
    QAction* actionKeyPad6;
    QAction* actionKeyPad7;
    QAction* actionKeyPad8;
    QAction* actionKeyPad9;

    QAction* actionCtrlKeyPad0;
    QAction* actionCtrlKeyPad1;
    QAction* actionCtrlKeyPad2;
    QAction* actionCtrlKeyPad3;
    QAction* actionCtrlKeyPad4;
    QAction* actionCtrlKeyPad5;
    QAction* actionCtrlKeyPad6;
    QAction* actionCtrlKeyPad7;
    QAction* actionCtrlKeyPad8;
    QAction* actionCtrlKeyPad9;

    QAction* actionKey0;
    QAction* actionKey1;
    QAction* actionKey2;
    QAction* actionKey3;
    QAction* actionKey4;
    QAction* actionKey5;
    QAction* actionKey6;
    QAction* actionKey7;
    QAction* actionKey8;
    QAction* actionKey9;

    QAction* actionCtrlKey0;
    QAction* actionCtrlKey1;
    QAction* actionCtrlKey2;
    QAction* actionCtrlKey3;
    QAction* actionCtrlKey4;
    QAction* actionCtrlKey5;
    QAction* actionCtrlKey6;
    QAction* actionCtrlKey7;
    QAction* actionCtrlKey8;
    QAction* actionCtrlKey9;

    bool _shiftPressed;
};

} //EPI
#endif APPLICATIONWINDOWBASE_H_
