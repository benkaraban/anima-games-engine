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
#include <Core/Logger.h>
#include <HOOClient/GUI/ScreenLogin.h>

ScreenLogin::ScreenLogin(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pWidgets = pWidgets;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMediumPanel();

    _pLabelLogin = pWidgets->createMediumLabel(L"login", 40, 70);
    _pEditLogin  = pWidgets->createMediumTextEdit(170, 70);

    _pLabelPassword = pWidgets->createMediumLabel(L"password", 40, 120);
    _pEditPassword  = pWidgets->createMediumTextEdit(170, 120);
    _pEditPassword->getEdit()->setPassword(true);

     

    _pCheckRemember = pWidgets->createCheckBox(L"remember-password", 175, 170);

    

    _pButtonOK = pWidgets->createMediumButton(L"ok", 30, 270);
    _pButtonExit = pWidgets->createMediumButton(L"exit", 230, 270);
    _pButtonCreateAccount = pWidgets->createLargeButton(L"create-account", 30, 320);
    _pButtonCanNotAccess = pWidgets->createLargeButton(L"can-not-login", 30, 370);

    _pEditLogin->setPrevWidget(_pEditPassword);
    _pEditLogin->setNextWidget(_pEditPassword);

    _pEditPassword->setPrevWidget(_pEditLogin);
    _pEditPassword->setNextWidget(_pEditLogin);

    _pEditLogin->getEdit()->setText(_appContext.config.login);
    _pEditPassword->getEdit()->setText(_appContext.config.password);
    _pCheckRemember->setChecked(_appContext.config.savePassword);
    
    _pEditLogin->addEventHandler(this);
    _pEditPassword->addEventHandler(this);
    _pButtonCanNotAccess->addEventHandler(this);
    _pButtonOK->addEventHandler(this);
    _pButtonExit->addEventHandler(this);
    _pButtonCreateAccount->addEventHandler(this);

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabelLogin);
    _pPanel->addChild(_pEditLogin);
    _pPanel->addChild(_pLabelPassword);
    _pPanel->addChild(_pEditPassword);
    _pPanel->addChild(_pButtonCanNotAccess);
    _pPanel->addChild(_pCheckRemember);
    _pPanel->addChild(_pButtonOK);
    _pPanel->addChild(_pButtonExit);
    _pPanel->addChild(_pButtonCreateAccount);
}

void ScreenLogin::startScreen()
{
    _pWidgets->getManager()->setDialog(_pDialog);

    if(_pEditLogin->getEdit()->getText().empty())
        _pEditLogin->focus();
    else
        _pEditPassword->focus();
    _pDialog->pack();
}
    
void ScreenLogin::endScreen()
{
}

void ScreenLogin::update(double elapsed)
{
#ifdef LM_DEVMODE
    if(_appContext.debugAutoConnect)
        onEvent(GUI::EV_COMMAND, _pButtonOK.get());            
#endif
}

void ScreenLogin::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pEditLogin.get() || 
            pWidget == _pEditPassword.get() || 
            pWidget == _pButtonOK.get())
        {
            _appContext.config.login = _pEditLogin->getEdit()->getText();
            _appContext.config.savePassword = _pCheckRemember->isChecked();

            if(_appContext.config.savePassword)
                _appContext.config.password = _pEditPassword->getEdit()->getText();
            else
                _appContext.config.password.clear();

            _appContext.config.save(_appContext.pVFS);

            _pScreenLoginInProgress->setLogin(_pEditLogin->getEdit()->getText(), _pEditPassword->getEdit()->getText());
            _pManager->setScreen(_pScreenLoginInProgress.get());
        }
        else if(pWidget == _pButtonCreateAccount.get())
        {
            _pManager->setScreen(_pScreenCreateAccount.get());
        }
        else if(pWidget == _pButtonExit.get())
        {
            _appContext.mustExit = true;
        }
        else if(pWidget == _pButtonCanNotAccess.get())
        {
            _pManager->setScreen(_pScreenCanNotAccess.get());
        }
    }
}