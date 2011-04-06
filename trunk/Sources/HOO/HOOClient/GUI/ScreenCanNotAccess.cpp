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

#include <HOOClient/GUI/ScreenCanNotAccess.h>

using namespace HOOUserProtocol;

ScreenCanNotAccess::ScreenCanNotAccess(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pWidgets = pWidgets;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMediumPanel();
    
    _pDialog->addChild(_pPanel);

    _pStep1Description  = pWidgets->createLargeRTLabel(L"can-not-login-step1", 30, 5, 120);
    _pLabelMail         = pWidgets->createMediumLabel(L"mail", 40, 105);
    _pEditMail          = pWidgets->createMediumTextEdit(170, 105);
    _pButtonSend        = pWidgets->createMediumButton(L"send", 170, 145);
    _pStep2Description  = pWidgets->createLargeRTLabel(L"can-not-login-step2", 30, 200, 120);
    _pLabelLogin        = pWidgets->createMediumLabel(L"login", 40, 300);
    _pEditLogin         = pWidgets->createMediumTextEdit(170, 300);
    _pLabelAcCode       = pWidgets->createMediumLabel(L"activation-code", 40, 340);
    _pEditAcCode        = pWidgets->createMediumTextEdit(170, 340);
    _pButtonOK          = pWidgets->createMediumButton(L"ok", 30, 400);
    _pButtonExit        = pWidgets->createMediumButton(L"cancel", 230, 400);

    _pPanel->addChild(_pStep1Description);
    _pPanel->addChild(_pLabelMail);
    _pPanel->addChild(_pEditMail);
    _pPanel->addChild(_pButtonSend);
    _pPanel->addChild(_pStep2Description);
    _pPanel->addChild(_pLabelLogin);
    _pPanel->addChild(_pEditLogin);
    _pPanel->addChild(_pLabelAcCode);
    _pPanel->addChild(_pEditAcCode);
    _pPanel->addChild(_pButtonOK);
    _pPanel->addChild(_pButtonExit);

    _pEditMail->addEventHandler(this);
    _pButtonSend->addEventHandler(this);
    _pButtonOK->addEventHandler(this);
    _pButtonExit->addEventHandler(this);

    _pMessageBox        = Ptr<ScreenMessageBox>              (new ScreenMessageBox(pWidgets, L"", MSGBOX_OK, false));
    _pScreenProgress    = Ptr<ScreenCanNotAccessInProgress>  (new ScreenCanNotAccessInProgress(_appContext));
}

void ScreenCanNotAccess::startScreen()
{
    _pButtonOK->setEnabled(false);
    _pButtonSend->setEnabled(false);
    _pWidgets->getManager()->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenCanNotAccess::endScreen()
{
}

void ScreenCanNotAccess::update(double elapsed)
{
    bool validMail = Core::isValidMail(_pEditMail->getEdit()->getText());
    _pButtonSend->setEnabled(validMail);

    if (_pEditLogin->getEdit()->getText() != L"" && _pEditAcCode->getEdit()->getText() != L"")  _pButtonOK->setEnabled(true);
    else _pButtonOK->setEnabled(false);
}

void ScreenCanNotAccess::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if (pWidget == _pButtonSend.get() && _pButtonSend->isEnabled())
        {
            if(!Core::isValidMail(_pEditMail->getEdit()->getText()))
                    errorBox(L"invalid-mail");
            else
            {
                ESACLangType lang = SACL_ENGLISH;
                if ((*_appContext.pDico).getLoadedLanguage()==Core::FRENCH)   lang = SACL_FRENCH;
                
                _pScreenProgress->sendActivationCode = SendActivationCode(_pEditMail->getEdit()->getText(), lang);
                _pScreenProgress->request = &_pScreenProgress->sendActivationCode;
                _pScreenProgress->setScreenFailed(this);
                _pScreenProgress->setScreenSuccess(this);
                _pManager->setScreen(_pScreenProgress.get());
            }
        }

        if (pWidget == _pButtonOK.get() && _pButtonOK->isEnabled())
        {
            _pScreenProgress->userLoginWithAcCode = UserLoginWithAcCode(_pEditLogin->getEdit()->getText(), _pEditAcCode->getEdit()->getText());
            _pScreenProgress->request = &_pScreenProgress->userLoginWithAcCode;
            _pScreenProgress->setScreenFailed(this);
            _pScreenProgress->setScreenSuccess(_pScreenOK.get());
            _pManager->setScreen(_pScreenProgress.get());
        }

        if(pWidget == _pButtonExit.get())
        {
            _pManager->setScreen(_pScreenCancel.get());
        }
    }
}

void ScreenCanNotAccess::errorBox(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(this);
    _pManager->setScreen(_pMessageBox.get());
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
ScreenCanNotAccessInProgress::ScreenCanNotAccessInProgress(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();
    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK));
}

void ScreenCanNotAccessInProgress::startScreen()
{
    INF << L"Sending  activation code request\n";

    _appContext.pConnexion->send(*request);
    _appContext.pGUI->pushDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenCanNotAccessInProgress::endScreen()
{
}

void ScreenCanNotAccessInProgress::createError(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(_pScreenFailed);
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenCanNotAccessInProgress::update(double elapsed)
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_SEND_ACTIVATION_CODE_ANSWER,
        HOOUserProtocol::USER_LOGIN_WITH_ACTIVATION_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_SEND_ACTIVATION_CODE_ANSWER)
        {
            Ptr<SendActivationCodeAnswer> pCreate = LM_DEBUG_PTR_CAST<SendActivationCodeAnswer>(pMessage);

            INF << L"Received activation code answer " << HOOUserProtocol::toString(pCreate->getSACAnswerType()) << L"\n";
            
            switch(pCreate->getSACAnswerType())
            {
            case SAC_CAN_NOT_SEND_MAIL : createError(L"can-not-send-mail"); break;
            case SAC_MAIL_SENDED:
                _pMessageBox->setMessage(L"mail-sended");
                _pMessageBox->setNextScreenOK(_pScreenSuccess);
                _pManager->setScreen(_pMessageBox.get());
                break;
            }
        }
        else if(pMessage->getType() == USER_LOGIN_WITH_ACTIVATION_ANSWER)
        {
            Ptr<UserLoginWithAcCodeAnswer> pMsg = LM_DEBUG_PTR_CAST<UserLoginWithAcCodeAnswer>(pMessage);

            INF << L"Received user login with activation code answer " << HOOUserProtocol::toString(pMsg->getUACAnswerType()) << L"\n";

            switch(pMsg->getUACAnswerType())
            {
            case USER_ALREADY_LOGGED: createError(L"already-logged"); break;
            case USER_LOGIN_FAILED:  createError(L"login-failed"); break;
            case USER_PASSWD_FAILED: createError(L"password-failed"); break;
            case USER_LOGIN_OK:                  
                _appContext.login = pMsg->getLogin();
                _appContext.account = pMsg->getAccountInfos();
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

void ScreenCanNotAccessInProgress::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
