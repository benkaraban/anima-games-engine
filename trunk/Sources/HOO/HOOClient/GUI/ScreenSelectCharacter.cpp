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

#include <Universe/Tasks/Move.h>
#include <Universe/Tasks/Turn.h>

#include <HOOClient/Avatar.h>
#include <HOOClient/GUI/ScreenSelectCharacter.h>
#include <HOOUserProtocol/CheckLoginAvailable.h>

using namespace HOOUserProtocol;

ScreenSelectCharacter::ScreenSelectCharacter(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pWidgets = pWidgets;

    _pDialog = pWidgets->createDialogBase();

    _pPanel = pWidgets->createMediumPanel(false);
    _pPanel->setPosition(Core::Vector2f(560.0f, 130.0f));

    _charId = 1;

    _pLabelName = pWidgets->createTitleLabel(L"CHARACTER_0001_NAME", 30, 40);
    _pLabelDetails = pWidgets->createLargeRTLabel(L"CHARACTER_0001_DETAILED", 30, 120, 120);
    _pLabelNotice = pWidgets->createLargeRTLabel(L"all-chars-equal", 30, 260, 120);
    _pButtonOK = pWidgets->createLargeButton(L"select-character", 30, 390);

    _pButtonPrev = pWidgets->createMediumButton(L"previous", 30, 340);
    _pButtonNext = pWidgets->createMediumButton(L"next", 230, 340);

    _pButtonPrev->addEventHandler(this);
    _pButtonNext->addEventHandler(this);
    _pButtonOK->addEventHandler(this);

    _pDialog->addChild(_pPanel);
 
    _pPanel->addChild(_pButtonPrev);
    _pPanel->addChild(_pButtonNext);
    _pPanel->addChild(_pLabelName);
    _pPanel->addChild(_pLabelDetails);
    _pPanel->addChild(_pLabelNotice);
    _pPanel->addChild(_pButtonOK);

    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK, false));
    _pScreenProgress = Ptr<ScreenSelectCharacterInProgress>(new ScreenSelectCharacterInProgress(_appContext));
}

void ScreenSelectCharacter::startScreen()
{
    _pWorld = _appContext.pWorldGUI;
    _pCamera = _appContext.pCameraGUI;

    Ptr<Universe::Node> pNodeCam = _appContext.pWorldGUI->getFirstNodeWithTag(L"CAM_CLOSE");
    static const float TIME = 0.4f;

    Ptr<Universe::ITask> pTaskMove(new Universe::TaskMove(Universe::DURATION_DEPENDANT, _pCamera, pNodeCam->getWorldPosition(), TIME, Core::INTER_FADE_OUT));
    Ptr<Universe::ITask> pTaskTurn(new Universe::TaskTurn(_pCamera, pNodeCam->getLocalOrient(), TIME, Core::INTER_FADE_OUT));

    _pWorld->registerTask(pTaskMove);
    _pWorld->registerTask(pTaskTurn);

    onCharChange();

    _pWidgets->getManager()->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenSelectCharacter::endScreen()
{
}

void ScreenSelectCharacter::onCharChange()
{
    String base(L"CHARACTER_" + Core::toString(_charId, 4));
    _pLabelName->setText(base + L"_NAME");
    _pLabelDetails->setText(base + L"_DETAILED");

    Ptr<Universe::Node> pNode = _appContext.pWorldGUI->getFirstNodeWithTag(L"POS_CHAR");

    if(_appContext.pAvatarGUI != null)
    {
        Universe::ScriptContext ctx;
        ctx.pNode = _appContext.pAvatarGUI;
        _appContext.pWorldGUI->runScript(L"/HOO/GUI/Scripts/CharacterDisparition.lua", ctx);
        _appContext.pAvatarGUI = null;
    }

    _appContext.pAvatarGUI = createBaseAvatar(*_appContext.pWorldGUI, _charId);
    _appContext.pAvatarGUI->setLocalPosition(pNode->getWorldPosition());
    _appContext.pAvatarGUI->setLocalOrientation(pNode->getLocalOrient());
    _appContext.pAvatarGUI->animate(L"idle", 0.0, 0.0f, 1.0f, true);
    _appContext.pAvatarGUI->setLocalScale(Core::Vector3f(0.0f, -2.0f, 0.0f));

    Universe::ScriptContext ctx;
    ctx.pNode = _appContext.pAvatarGUI;
    _appContext.pWorldGUI->runScript(L"/HOO/GUI/Scripts/CharacterApparition.lua", ctx);
}

void ScreenSelectCharacter::update(double elapsed)
{
}

void ScreenSelectCharacter::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonPrev.get())
        {
            if(_charId == 1)
                _charId = CHARACTER_COUNT;
            else
                _charId --;
            onCharChange();
        }
        else if(pWidget == _pButtonNext.get())
        {
            _charId = 1 + (((_charId - 1) + 1) % CHARACTER_COUNT);
            onCharChange();
        }
        else if(pWidget == _pButtonOK.get())
        {
            _pScreenProgress->selectChar = HOOUserProtocol::SelectCharacter(_charId);
            _pScreenProgress->setScreenSuccess(_pScreenOK.get());
            _pScreenProgress->setScreenFailed(_pScreenFailed.get());

            _pManager->setScreen(_pScreenProgress.get());
        }
    }
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

ScreenSelectCharacterInProgress::ScreenSelectCharacterInProgress(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();
    _pMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(pWidgets, L"", MSGBOX_OK));
}

void ScreenSelectCharacterInProgress::startScreen()
{
    INF << L"Sending character selection request\n";

    _appContext.pConnexion->send(selectChar);
    _appContext.pGUI->pushDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenSelectCharacterInProgress::endScreen()
{
}

void ScreenSelectCharacterInProgress::createError(const String & error)
{
    _pMessageBox->setMessage(error);
    _pMessageBox->setNextScreenOK(_pScreenFailed);
    _pManager->setScreen(_pMessageBox.get());
}

void ScreenSelectCharacterInProgress::update(double elapsed)
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_SELECT_CHARACTER_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_SELECT_CHARACTER_ANSWER)
        {
            Ptr<SelectCharacterAnswer> pSelect = LM_DEBUG_PTR_CAST<SelectCharacterAnswer>(pMessage);

            INF << L"Received character selection answer " << HOOUserProtocol::toString(pSelect->getSelectCharacterAnswerType()) << L"\n";
            
            switch(pSelect->getSelectCharacterAnswerType())
            {
            case CHARACTER_SELECTED_FAILED_NO_TOKEN:
                createError(L"invalid-mail"); 
                break;
            case CHARACTER_SELECTED_OK:
                _appContext.account = pSelect->getAccountInfos();
                _pManager->setScreen(_pScreenSuccess);
                break;
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for character selection answer.\n";
        }
    }
}

void ScreenSelectCharacterInProgress::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
}
