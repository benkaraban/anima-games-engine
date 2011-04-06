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
#ifndef HOO_MESSAGEBOX_H_
#define HOO_MESSAGEBOX_H_

#include <GUI/Widget.h>

#include <HOOClient/GUI/IScreen.h>
#include <HOOClient/GUI/WidgetFactory.h>

LM_ENUM_2(EMessageBoxType,
          MSGBOX_OK,
          MSGBOX_RETRY_CANCEL
          );

LM_ENUM_2(EMessageBoxResult,
          MSGBOX_RESULT_OK, // RETRY == OK
          MSGBOX_RESULT_CANCEL
          );

class ScreenMessageBox : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenMessageBox(const Ptr<WidgetFactory> & pWidgets, const String & message, EMessageBoxType boxType, bool pushDialog = false);

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setMessage(const String & message) { _pLabel->setText(message); }

    void setNextScreenOK(IScreen * pNextScreen) { _pNextScreenOK = pNextScreen; }
    void setNextScreenCancel(IScreen * pNextScreen) { _pNextScreenCancel = pNextScreen; }

    EMessageBoxResult getResult() const { return _result; }

protected:
    Ptr<WidgetFactory>      _pWidgets;
    Ptr<GUI::Widget>        _pDialog;
    EMessageBoxType         _boxType;

    Ptr<GUI::Widget>        _pPanel;
    Ptr<GUI::RichTextLabel> _pLabel;

    Ptr<GUI::Widget>        _pPanelButtons;
    Ptr<GUI::Button>        _pButtonOK;
    Ptr<GUI::Button>        _pButtonCancel;
    
    EMessageBoxResult       _result;
    IScreen *               _pNextScreenOK;
    IScreen *               _pNextScreenCancel;
    bool                    _pushDialog;
};

#endif
