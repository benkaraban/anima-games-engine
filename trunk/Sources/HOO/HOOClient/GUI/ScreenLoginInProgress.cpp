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

#include <HOOClient/GUI/ScreenLoginInProgress.h>

#include <HOOUserProtocol/UserConnect.h>
#include <HOOUserProtocol/UserLogin.h>

using namespace HOOUserProtocol;

ScreenLoginInProgress::ScreenLoginInProgress(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();
    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK));
}

void ScreenLoginInProgress::setLogin(const String & login, const String & password)
{
    _login = login;
    _password = password;
}

void ScreenLoginInProgress::startScreen()
{
    INF << L"Sending  login infos\n";

    UserLogin login(_login, _password);
    _appContext.pConnexion->send(login);
    
    _appContext.pGUI->pushDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenLoginInProgress::endScreen()
{
    setLogin(L"", L"");
}

void ScreenLoginInProgress::connectionError(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(_pScreenFailed.get());
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenLoginInProgress::update(double elapsed)
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_LOGIN_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_LOGIN_ANSWER)
        {
            Ptr<UserLoginAnswer> pLogin = LM_DEBUG_PTR_CAST<UserLoginAnswer>(pMessage);

            INF << L"Received login answer " << HOOUserProtocol::toString(pLogin->getLoginAnswerType()) << L"\n";
            
            switch(pLogin->getLoginAnswerType())
            {
            default:
            case USER_ACCOUNT_LOCKED: connectionError(L"account-locked"); break;
            case USER_ACCOUNT_BANNED: connectionError(L"account-banned"); break;
            case USER_ALREADY_LOGGED: connectionError(L"already-logged"); break;
            case USER_LOGIN_FAILED:  connectionError(L"login-failed"); break;
            case USER_PASSWD_FAILED: connectionError(L"password-failed"); break;
            case USER_LOGIN_OK:      
                {
                    _appContext.login = _login;
                    _appContext.account = pLogin->getAccountInfos();
                    if(_appContext.account.character == 0)
                        _pManager->setScreen(_pScreenSelectChar.get());
                    else
                        _pManager->setScreen(_pScreenSuccess.get()); 
                    break;
                }
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for connect answer.\n";
        }
    }
}

void ScreenLoginInProgress::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        _pManager->setScreen(_pScreenFailed.get());
    }
}
