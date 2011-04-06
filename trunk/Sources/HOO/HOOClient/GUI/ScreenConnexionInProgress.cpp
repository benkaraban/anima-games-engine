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

#include <HOOClient/GUI/ScreenConnexionInProgress.h>

#include <HOOUserProtocol/UserConnect.h>

using namespace HOOUserProtocol;

ScreenConnexionInProgress::ScreenConnexionInProgress(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _state = CONNECT_LOOKING_FOR_SERVER;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMessageBoxMedium();

    _pLabel = pWidgets->createMediumLabel(L"connection-in-progress", 5, 5);
    _pLabel->setSize(Core::Vector2f(535.0f, 100.0f));
    _pLabel->setHLayout(GUI::HL_CENTER);
    _pLabel->setVLayout(GUI::VL_CENTER);
 
    _pButtonCancel = pWidgets->createMediumButton(L"cancel", 0, 0);
    _pButtonCancel->setHotKey(VK_ESCAPE);
    _pButtonCancel->setPosition(Core::Vector2f(0.5f * (_pPanel->getSize().x - _pButtonCancel->getSize().x), 110.0f));
    _pButtonCancel->addEventHandler(this);

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabel);
    //_pPanel->addChild(_pButtonCancel);

    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_RETRY_CANCEL));
}

void ScreenConnexionInProgress::connectToServer()
{
    _appContext.pConnexion->open(_appContext.serverAddress, 31337);
    _state = CONNECT_LOOKING_FOR_SERVER;
}

void ScreenConnexionInProgress::sendConnect()
{
    INF << L"Sending  connect infos\n";

    UserConnect userConnect;
    _appContext.pConnexion->send(userConnect);
    _state = CONNECT_WAITING_FOR_CONNECT;
}

void ScreenConnexionInProgress::startScreen()
{
    switch(_appContext.pConnexion->getState())
    {
    default:
    case CONNEXION_FAILED:
    case CONNEXION_FAILED_SERVER_FULL:
    case CONNEXION_ERROR:
    case CONNEXION_LOST:
        _appContext.pConnexion->resetState();
        // no break
    case CONNEXION_NOT_OPENED:
        connectToServer();
        break;
    case CONNEXION_OK:
        sendConnect();
        break;
    case CONNEXION_IN_PROGRESS:
        _state = CONNECT_LOOKING_FOR_SERVER;
        break;
    }

    _appContext.pGUI->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenConnexionInProgress::endScreen()
{
}

void ScreenConnexionInProgress::connectionError(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(this);
    _pMessageBox->setNextScreenCancel(_pScreenFailed.get());
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenConnexionInProgress::waitConnectAnswer()
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_CONNECT_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_CONNECT_ANSWER)
        {
            Ptr<UserConnectAnswer> pConnect = LM_DEBUG_PTR_CAST<UserConnectAnswer>(pMessage);
            
            INF << L"Received connect answer " << HOOUserProtocol::toString(pConnect->getConnectAnswerType()) << L"\n";

            switch(pConnect->getConnectAnswerType())
            {
            case USER_CONNECT_VERSION_ERROR:    connectionError(L"connection-failed-bad-version"); break;
            default:                            connectionError(L"connection-failed"); break;
            case USER_CONNECT_OK:               _state = CONNECT_OK; break;
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for connect answer.\n";
        }
    }
}

void ScreenConnexionInProgress::update(double elapsed)
{
    switch(_state)
    {
    case CONNECT_LOOKING_FOR_SERVER:
    {
        switch(_appContext.pConnexion->getState())
        {
        case CONNEXION_FAILED:
        case CONNEXION_ERROR:
        case CONNEXION_LOST:
        case CONNEXION_NOT_OPENED:          connectionError(L"connection-failed"); break;
        case CONNEXION_FAILED_SERVER_FULL:  connectionError(L"connection-failed-server-full"); break;
        case CONNEXION_IN_PROGRESS:         /* Toujours en cours, on attend */ break;
        case CONNEXION_OK:                  sendConnect(); break;
        }
        break;
    }
    case CONNECT_WAITING_FOR_CONNECT:   waitConnectAnswer(); break;
    } // switch(_state)

    
    if(_state == CONNECT_OK)
        _pManager->setScreen(_pScreenSuccess.get());
}

void ScreenConnexionInProgress::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        _state = CONNECT_CANCELED;
        _pManager->setScreen(_pScreenFailed.get());
    }
}
