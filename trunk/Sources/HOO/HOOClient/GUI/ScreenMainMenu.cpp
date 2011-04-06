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
#include <HOOClient/GUI/ScreenMainMenu.h>
#include <HOOClient/Avatar.h>
#include <HOOUserProtocol/UserLogout.h>

using namespace HOOUserProtocol;

ScreenMainMenu::ScreenMainMenu(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pWidgets = pWidgets;

    _pDialog = pWidgets->createDialogBase();

    static const int32 Y_PADDING = 40;

    _pButtonProfile = pWidgets->createSmallButton(L"profile", 30, 0);
    _pButtonSettings = pWidgets->createSmallButton(L"settings", 170, 0);
    _pPanelNews = pWidgets->createThinPanel(0, 30);

    _pPanelRight = Ptr<GUI::Widget>(new GUI::Widget());
    _pPanelRight->addChild(_pButtonProfile);
    _pPanelRight->addChild(_pButtonSettings);
    _pPanelRight->addChild(_pPanelNews);
    _pPanelRight->setPosition(Core::Vector2f(600.0f, 140.0f));

    _pButtonTutorial = pWidgets->createMediumButton(L"tutorial", 0, 0 * Y_PADDING);
    _pButtonPlay = pWidgets->createMediumButton(L"play", 0, 1 * Y_PADDING);
    _pButtonCharacter = pWidgets->createMediumButton(L"character", 0, 2 * Y_PADDING);
    _pButtonShop = pWidgets->createMediumButton(L"shop", 0, 3 * Y_PADDING);
    _pButtonExit = pWidgets->createMediumButton(L"logout", 0, 4 * Y_PADDING);

    _pPanelButtons = Ptr<GUI::Widget>(new GUI::Widget());
    _pPanelButtons->addChild(_pButtonTutorial);
    _pPanelButtons->addChild(_pButtonPlay);
    _pPanelButtons->addChild(_pButtonCharacter);
    _pPanelButtons->addChild(_pButtonShop);
    _pPanelButtons->addChild(_pButtonExit);
    _pPanelButtons->pack();

    _pWidgets->centerScreen(_pPanelButtons);

    _pButtonTutorial->addEventHandler(this);
    _pButtonPlay->addEventHandler(this);
    _pButtonCharacter->addEventHandler(this);
    _pButtonShop->addEventHandler(this);
    _pButtonExit->addEventHandler(this);
    _pButtonProfile->addEventHandler(this);
    _pButtonSettings->addEventHandler(this);

    _pButtonTutorial->setEnabled(false);
    _pButtonCharacter->setEnabled(false);
    _pButtonShop->setEnabled(false);
    _pButtonProfile->setEnabled(false);
    _pButtonSettings->setEnabled(false);

    _pDialog->addChild(_pPanelButtons);
    _pDialog->addChild(_pPanelRight);
}

void ScreenMainMenu::startScreen()
{
    //createAvatar(*_pWorld, avatar);

    if(_appContext.pAvatarGUI == null)
    {
        Ptr<Universe::Node> pNode = _appContext.pWorldGUI->getFirstNodeWithTag(L"POS_CHAR");

        if(pNode != null)
        {
            AvatarDesc ad;
            ad.characterId = _appContext.account.character;
            ad.equipment = _appContext.account.equipedStuff;

            _appContext.pAvatarGUI = createAvatar(*_appContext.pWorldGUI, ad);
            _appContext.pAvatarGUI->setLocalPosition(pNode->getWorldPosition());
            _appContext.pAvatarGUI->setLocalOrientation(pNode->getLocalOrient());
            _appContext.pAvatarGUI->animate(L"idle", 0.0, 0.0f, 1.0f, true);
            _appContext.pAvatarGUI->setLocalScale(Core::Vector3f(0.0f, -2.0f, 0.0f));

            Universe::ScriptContext ctx;
            ctx.pNode = _appContext.pAvatarGUI;
            _appContext.pWorldGUI->runScript(L"/HOO/GUI/Scripts/CharacterApparition.lua", ctx);
        }
    }

    Ptr<Universe::Node> pNodeCam = _appContext.pWorldGUI->getFirstNodeWithTag(L"CAM_TITLE");
    static const float TIME = 0.4f;

    Ptr<Universe::ITask> pTaskMove(new Universe::TaskMove(Universe::DURATION_DEPENDANT, _appContext.pCameraGUI, pNodeCam->getWorldPosition(), TIME, Core::INTER_FADE_OUT));
    Ptr<Universe::ITask> pTaskTurn(new Universe::TaskTurn(_appContext.pCameraGUI, pNodeCam->getLocalOrient(), TIME, Core::INTER_FADE_OUT));

    _appContext.pWorldGUI->registerTask(pTaskMove);
    _appContext.pWorldGUI->registerTask(pTaskTurn);

    _pWidgets->getManager()->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenMainMenu::endScreen()
{
}

void ScreenMainMenu::update(double elapsed)
{
#ifdef LM_DEVMODE
    if(_appContext.debugAutoMatch)
        onEvent(GUI::EV_COMMAND, _pButtonPlay.get());
#endif
}

void ScreenMainMenu::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonPlay.get())
        {
            _pManager->setScreen(_pScreenQM.get());
        }
        else if(pWidget == _pButtonExit.get())
        {
            if(_appContext.pAvatarGUI != null)
            {
                Universe::ScriptContext ctx;
                ctx.pNode = _appContext.pAvatarGUI;
                _appContext.pWorldGUI->runScript(L"/HOO/GUI/Scripts/CharacterDisparition.lua", ctx);
                _appContext.pAvatarGUI = null;
            }
            UserLogout logout;
            _appContext.pConnexion->send(logout);

            _pManager->setScreen(_pScreenExit.get());
        }
    }
}
