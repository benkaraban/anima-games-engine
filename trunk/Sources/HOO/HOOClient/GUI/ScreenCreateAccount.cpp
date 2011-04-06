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
#include <Core/Strings/Tools.h>
#include <HOOClient/GUI/ScreenCreateAccount.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>

using namespace HOOUserProtocol;

ScreenCreateAccount::ScreenCreateAccount(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pWidgets = pWidgets;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMediumPanel();

    _pLabelLogin = pWidgets->createMediumLabel(L"login", 40, 50);
    _pEditLogin  = pWidgets->createMediumTextEdit(170, 50);

    _pLabelCheckLogin = pWidgets->createMediumLabel(L"4", 195, 90);
    _pLabelCheckLogin->setHLayout(GUI::HL_CENTER);

    _pLabelPassword1 = pWidgets->createMediumLabel(L"password", 40, 140);
    _pEditPassword1  = pWidgets->createMediumTextEdit(170, 140);
    _pEditPassword1->getEdit()->setPassword(true);

    _pLabelPassword2 = pWidgets->createMediumRTLabel(L"retype-password", 40, 185, 45);
    _pLabelPassword2->setAutoCenterVContent(true);
    _pEditPassword2  = pWidgets->createMediumTextEdit(170, 185);
    _pEditPassword2->getEdit()->setPassword(true);

    _pLabelMail = pWidgets->createMediumLabel(L"mail", 40, 260);
    _pEditMail  = pWidgets->createMediumTextEdit(170, 260);

    _pCheckMail = pWidgets->createCheckBox(L"receive-newsletter", 80, 310);

    _pButtonOK = pWidgets->createMediumButton(L"ok", 30, 370);
    _pButtonExit = pWidgets->createMediumButton(L"cancel", 230, 370);

    _pEditLogin->setPrevWidget(_pEditMail);
    _pEditLogin->setNextWidget(_pEditPassword1);

    _pEditPassword1->setPrevWidget(_pEditLogin);
    _pEditPassword1->setNextWidget(_pEditPassword2);

    _pEditPassword2->setPrevWidget(_pEditPassword1);
    _pEditPassword2->setNextWidget(_pEditMail);

    _pEditMail->setPrevWidget(_pEditPassword2);
    _pEditMail->setNextWidget(_pEditLogin);

    _pEditLogin->addEventHandler(this);
    _pEditPassword1->addEventHandler(this);
    _pEditPassword2->addEventHandler(this);
    _pEditMail->addEventHandler(this);
    _pButtonOK->addEventHandler(this);
    _pButtonExit->addEventHandler(this);

    _pDialog->addChild(_pPanel);
 
    _pPanel->addChild(_pLabelLogin);
    _pPanel->addChild(_pEditLogin);
    _pPanel->addChild(_pLabelCheckLogin);
    _pPanel->addChild(_pLabelPassword1);
    _pPanel->addChild(_pEditPassword1);
    _pPanel->addChild(_pLabelPassword2);
    _pPanel->addChild(_pLabelMail);
    _pPanel->addChild(_pEditPassword2);
    _pPanel->addChild(_pEditMail);
    _pPanel->addChild(_pCheckMail);
    _pPanel->addChild(_pButtonOK);
    _pPanel->addChild(_pButtonExit);

    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK, false));
    _pScreenProgress = Ptr<ScreenCreateAccountInProgress>(new ScreenCreateAccountInProgress(_appContext));
}

void ScreenCreateAccount::startScreen()
{
    _lastLogin = L"";
    _sentLogin = L"";
    _waitingForAnswer = false;
    _elapsedSinceLogin = 0.0;
    
    _pButtonOK->setEnabled(false);
    _pWidgets->getManager()->setDialog(_pDialog);
    _pEditLogin->focus();
    _pDialog->pack();
}
    
void ScreenCreateAccount::endScreen()
{
}

void ScreenCreateAccount::update(double elapsed)
{
    String login = _pEditLogin->getEdit()->getText();
    
    if(login == _lastLogin)
    {
        _elapsedSinceLogin += elapsed;
    }
    else
    {
        _lastLogin = login;
        _elapsedSinceLogin = 0.0;
    }

    if(login != String::EMPTY && _elapsedSinceLogin > 1.0 && !_waitingForAnswer && _sentLogin != login)
    {
        _sentLogin = login;
        _waitingForAnswer = true;
        _answer = false;

        HOOUserProtocol::CheckLoginAvailable message(login);
        _appContext.pConnexion->send(message);
    }

    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    while(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<HOOUserProtocol::IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == HOOUserProtocol::USER_CHECK_LOGIN_AVAILABLE_ANSWER)
        {
            Ptr<HOOUserProtocol::CheckLoginAvailableAnswer> pCheck = LM_DEBUG_PTR_CAST<HOOUserProtocol::CheckLoginAvailableAnswer>(pMessage);
            _waitingForAnswer = false;
            _answerReceived = true;
            _answer = (pCheck->getLoginAvailability() == HOOUserProtocol::USER_LOGIN_AVAILABLE);
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for connect answer.\n";
        }
    }

    if(_sentLogin == login && login != String::EMPTY)
    {
        if(_waitingForAnswer)
        {
            if(_elapsedSinceLogin > 1.5)
            {
                _pLabelCheckLogin->setText(L"checking-if-available");
                _pLabelCheckLogin->setOwnColor(Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f));
            }
            
            _loginIsOK = false;
        }
        else if(_answer)
        {
            _loginIsOK = true;
            _pLabelCheckLogin->setText(L"ok");
            _pLabelCheckLogin->setOwnColor(Core::Vector4f(0.4f, 1.0f, 0.4f, 1.0f));
        }
        else
        {
            _loginIsOK = false;
            _pLabelCheckLogin->setText(L"checking-if-available-taken");
            _pLabelCheckLogin->setOwnColor(Core::Vector4f(1.0f, 0.4f, 0.4f, 1.0f));
        }
    }
    else
    {
        _loginIsOK = false;
        _pLabelCheckLogin->setText(L"");
    }

    if(_loginIsOK && 
        _pEditPassword1->getEdit()->getText() != String::EMPTY && 
        _pEditPassword2->getEdit()->getText() != String::EMPTY &&
        _pEditMail->getEdit()->getText() != String::EMPTY)
    {
        _pButtonOK->setEnabled(true);
    }
    else
    {
        _pButtonOK->setEnabled(false);
    }
}

void ScreenCreateAccount::errorBox(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(this);
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenCreateAccount::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(_pButtonOK->isEnabled())
        {
            if(pWidget == _pEditLogin.get() || 
                pWidget == _pEditPassword1.get() || 
                pWidget == _pEditPassword2.get() || 
                pWidget == _pEditMail.get() || 
                pWidget == _pButtonOK.get())
            {
                if(_pEditPassword1->getEdit()->getText() != _pEditPassword2->getEdit()->getText())
                    errorBox(L"password-mismatch");
                else if(!Core::isValidMail(_pEditMail->getEdit()->getText()))
                    errorBox(L"invalid-mail");
                else
                {
                    _pScreenProgress->createAccount = CreateUserAccount(_pEditLogin->getEdit()->getText(),
                                      _pEditPassword1->getEdit()->getText(),
                                      _pEditMail->getEdit()->getText(),
                                      _pCheckMail->isChecked());

                    _pScreenProgress->setScreenFailed(this);
                    _pScreenProgress->setScreenSuccess(_pScreenOK.get());
                    _pManager->setScreen(_pScreenProgress.get());
                }
            }
        }

        if(pWidget == _pButtonExit.get())
        {
            _pManager->setScreen(_pScreenCancel.get());
        }
    }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ScreenCreateAccountInProgress::ScreenCreateAccountInProgress(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();
    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK));
}

void ScreenCreateAccountInProgress::startScreen()
{
    INF << L"Sending  account creation request\n";

    _appContext.pConnexion->send(createAccount);
    _appContext.pGUI->pushDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenCreateAccountInProgress::endScreen()
{
}

void ScreenCreateAccountInProgress::createError(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(_pScreenFailed);
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenCreateAccountInProgress::update(double elapsed)
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_CREATE_ACCOUNT_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_CREATE_ACCOUNT_ANSWER)
        {
            Ptr<CreateUserAccountAnswer> pCreate = LM_DEBUG_PTR_CAST<CreateUserAccountAnswer>(pMessage);

            INF << L"Received account creation answer " << HOOUserProtocol::toString(pCreate->getCreateAccountAnswerType()) << L"\n";
            
            switch(pCreate->getCreateAccountAnswerType())
            {
            case LOGIN_FORBIDDEN:
            case LOGIN_INCORRECT:       createError(L"invalid-login"); break;
            case LOGIN_ALREADY_USED:    createError(L"checking-if-available-taken"); break;
            case PWD_INCORRECT:         createError(L"invalid-password"); break;
            case MAIL_INCORRECT:        createError(L"invalid-mail"); break;
            case USER_ACCOUNT_CREATED:
                _appContext.login = createAccount.getLogin();
                _pManager->setScreen(_pScreenSuccess);
                break;
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for connect answer.\n";
        }
    }
}

void ScreenCreateAccountInProgress::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
}
