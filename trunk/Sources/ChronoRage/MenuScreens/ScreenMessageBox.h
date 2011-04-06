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
#ifndef CHRONORAGE_MESSAGEBOX_H_
#define CHRONORAGE_MESSAGEBOX_H_

#include <GUI/Widget.h>

#include <Modes/IGameState.h>
#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

LM_ENUM_3(EMessageBoxResult,
          MSGBOX_RESULT_RESUME,
          MSGBOX_RESULT_RETRY,
          MSGBOX_RESULT_QUIT
          );

class ScreenMessageBox : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenMessageBox(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setMessage(const String & message) { _pLabel->setText(message); }

    EMessageBoxResult getResult() const { return _result; }

protected:
    AppContext &            _appContext;
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pPanel;
    Ptr<GUI::Label>         _pLabel;

    Ptr<GUI::Widget>        _pPanelButtons;
    Ptr<GUI::Button>        _pButtonResume;
    Ptr<GUI::Button>        _pButtonRetry;
    Ptr<GUI::Button>        _pButtonQuit;
    
    EMessageBoxResult       _result;
};

}//namespace ChronoRage
#endif /*CHRONORAGE_MESSAGEBOX_H_*/
