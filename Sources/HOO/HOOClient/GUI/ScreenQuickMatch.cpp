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

#include <HOOClient/GUI/ScreenQuickMatch.h>

#include <HOOUserProtocol/LaunchMatchMessage/QuickMatch.h>
#include <HOOUserProtocol/LaunchMatchMessage/LaunchMatch.h>

using namespace HOOUserProtocol;

ScreenQuickMatch::ScreenQuickMatch(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMessageBoxMedium();

    _pLabel = pWidgets->createMediumLabel(L"looking-for-opponent", 5, 5);
    _pLabel->setSize(Core::Vector2f(535.0f, 100.0f));
    _pLabel->setHLayout(GUI::HL_CENTER);
    _pLabel->setVLayout(GUI::VL_CENTER);
 
    _pButtonCancel = pWidgets->createMediumButton(L"cancel", 0, 0);
    _pButtonCancel->setHotKey(VK_ESCAPE);
    _pButtonCancel->setPosition(Core::Vector2f(0.5f * (_pPanel->getSize().x - _pButtonCancel->getSize().x), 110.0f));
    _pButtonCancel->addEventHandler(this);

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabel);
    _pPanel->addChild(_pButtonCancel);
}

void ScreenQuickMatch::startScreen()
{
    askQuickMatch();
    _cancelled = false;
    _appContext.pGUI->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenQuickMatch::endScreen()
{
}

void ScreenQuickMatch::askQuickMatch()
{
    INF << L"Sending quickmatch request\n";
    QuickMatch qm;
    _appContext.pConnexion->send(qm);
    _pButtonCancel->setEnabled(true);
}

void ScreenQuickMatch::update(double elapsed)
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_QUICK_MATCH_ANSWER,
        HOOUserProtocol::USER_CANCEL_QUICK_MATCH_ANSWER,
        HOOUserProtocol::USER_LAUNCH_MATCH_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_QUICK_MATCH_ANSWER)
        {
            Ptr<QuickMatchAnswer> pQuick = LM_DEBUG_PTR_CAST<QuickMatchAnswer>(pMessage);
            
            INF << L"Received quick match answer " << HOOUserProtocol::toString(pQuick->getQuickMatchAnswerType()) << L"\n";

            switch(pQuick->getQuickMatchAnswerType())
            {
            case OPPONENT_FOUND:
                if(!_cancelled)
                {
                    _pButtonCancel->setEnabled(false);

                    INF << L"Sending launch match request\n";

                    LaunchMatch launch;
                    _appContext.pConnexion->send(launch);
                }
                // else on attend le QuickMatchCancelAnswer pour quitter
                break;

            case LOOKING_FOR_OPPONENT:
                // Rien � faire
                break;

            case ALREADY_LOOKING_FOR_OPPONENT:
            default:
                // TODO deconnexion
                break;
            }
        }
        else if(pMessage->getType() == USER_CANCEL_QUICK_MATCH_ANSWER)
        {
            Ptr<CancelQuickMatchAnswer> pQuick = LM_DEBUG_PTR_CAST<CancelQuickMatchAnswer>(pMessage);
            
            INF << L"Received quick match cancel answer " << HOOUserProtocol::toString(pQuick->getCancelQuickMatchAnswerType()) << L"\n";
            _pManager->setScreen(_pScreenCancel.get());
        }
        else if(pMessage->getType() == USER_LAUNCH_MATCH_ANSWER)
        {
            Ptr<LaunchMatchAnswer> pLaunch = LM_DEBUG_PTR_CAST<LaunchMatchAnswer>(pMessage);
            
            INF << L"Received launch match answer " << HOOUserProtocol::toString(pLaunch->getLaunchMatchAnswerType()) << L"\n";

            if(pLaunch->getLaunchMatchAnswerType() == OPPONENT_CANCELLED)
            {
                askQuickMatch();
            }
            else
            {
                INF << L"LAUNCH GAME\n";
                _appContext.playerId = pLaunch->getPlayerId();
                _pScreenLoading->setLoadingInfos(pLaunch->getLevelId(), pLaunch->getOpponentInfos());
                _pManager->setScreen(_pScreenLoading.get());
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for quickmatch.\n";
        }
    }
}

void ScreenQuickMatch::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonCancel.get())
        {
            INF << L"Sending quickmatch cancel\n";
            
            CancelQuickMatch cancel;
            _appContext.pConnexion->send(cancel);

            _cancelled = true;
            _pButtonCancel->setEnabled(false);
        }
    }
}
