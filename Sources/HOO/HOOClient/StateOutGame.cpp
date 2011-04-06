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
#include <Universe/Tasks/LookAt.h>
#include <Universe/Tasks/TurnAround.h>

#include <HOOClient/StateOutGame.h>



    
static const bool TEST_MODE = false;

StateOutGame::StateOutGame(AppContext & appContext) :
    _appContext(appContext),
    _pCurrentScreen(NULL)
{
    // Creation

    _pScreenLogin = Ptr<ScreenLogin>(new ScreenLogin(appContext));
    _pScreenMainMenu = Ptr<ScreenMainMenu>(new ScreenMainMenu(appContext));
    _pScreenConnexionInProgress = Ptr<ScreenConnexionInProgress>(new ScreenConnexionInProgress(appContext));
    _pScreenCreateAccount = Ptr<ScreenCreateAccount>(new ScreenCreateAccount(appContext));
    _pScreenCanNotAccess = Ptr<ScreenCanNotAccess>(new ScreenCanNotAccess(appContext));
    _pScreenSelectCharacter = Ptr<ScreenSelectCharacter>(new ScreenSelectCharacter(appContext));
    _pScreenLoginInProgress = Ptr<ScreenLoginInProgress>(new ScreenLoginInProgress(appContext));
    _pScreenQuickMatch = Ptr<ScreenQuickMatch>(new ScreenQuickMatch(appContext));
    _pScreenLoading = Ptr<ScreenLoading>(new ScreenLoading(appContext));
    _pScreenExit = Ptr<ScreenExit>(new ScreenExit(appContext));
    _pScreenTest = Ptr<ScreenTest>(new ScreenTest(appContext));
    _pScreenMessageBox = Ptr<ScreenMessageBox>(new ScreenMessageBox(appContext.pWidgets, L"", MSGBOX_RETRY_CANCEL));

    // Links

    _pScreenConnexionInProgress->setScreenSuccess(_pScreenLogin);
    _pScreenConnexionInProgress->setScreenFailed(_pScreenExit);

    _pScreenLogin->setScreenLoginInProgress(_pScreenLoginInProgress);
    _pScreenLogin->setScreenCreateAccount(_pScreenCreateAccount);
    _pScreenLogin->setScreenCanNotAccess(_pScreenCanNotAccess);

    _pScreenLoginInProgress->setScreenFailed(_pScreenLogin);
    _pScreenLoginInProgress->setScreenSelectCharacter(_pScreenSelectCharacter);
    _pScreenLoginInProgress->setScreenSuccess(_pScreenMainMenu);

    _pScreenCreateAccount->setScreenCancel(_pScreenLogin);
    _pScreenCreateAccount->setScreenOK(_pScreenSelectCharacter);

    _pScreenCanNotAccess->setScreenCancel(_pScreenLogin);
    _pScreenCanNotAccess->setScreenOK(_pScreenMainMenu);

    _pScreenSelectCharacter->setScreenOK(_pScreenMainMenu);
    _pScreenSelectCharacter->setScreenFailed(_pScreenExit);

    _pScreenMainMenu->setQuickMatchScreen(_pScreenQuickMatch);
    _pScreenMainMenu->setExitScreen(_pScreenLogin);

    _pScreenQuickMatch->setScreenCancel(_pScreenMainMenu);
    _pScreenQuickMatch->setScreenLoading(_pScreenLoading);
}

StateOutGame::~StateOutGame()
{
    // Links

    _pScreenConnexionInProgress->setScreenSuccess(null);
    _pScreenConnexionInProgress->setScreenFailed(null);

    _pScreenLogin->setScreenLoginInProgress(null);
    _pScreenLogin->setScreenCreateAccount(null);

    _pScreenLoginInProgress->setScreenFailed(null);
    _pScreenLoginInProgress->setScreenSelectCharacter(null);
    _pScreenLoginInProgress->setScreenSuccess(null);

    _pScreenCreateAccount->setScreenCancel(null);
    _pScreenCreateAccount->setScreenOK(null);

    _pScreenCanNotAccess->setScreenCancel(null);
    _pScreenCanNotAccess->setScreenOK(null);


    _pScreenSelectCharacter->setScreenOK(null);
    _pScreenSelectCharacter->setScreenFailed(null);

    _pScreenMainMenu->setQuickMatchScreen(null);
    _pScreenMainMenu->setExitScreen(null);

    _pScreenQuickMatch->setScreenCancel(null);
    _pScreenQuickMatch->setScreenLoading(null);

    _pScreenMessageBox->setNextScreenOK(NULL);
    _pScreenMessageBox->setNextScreenCancel(NULL);

    // Free

    LM_PTR_DELETE(_pScreenLogin);
    LM_PTR_DELETE(_pScreenMainMenu);
    LM_PTR_DELETE(_pScreenConnexionInProgress);
    LM_PTR_DELETE(_pScreenCreateAccount);
    LM_PTR_DELETE(_pScreenCanNotAccess);
    LM_PTR_DELETE(_pScreenSelectCharacter);
    LM_PTR_DELETE(_pScreenLoginInProgress);
    LM_PTR_DELETE(_pScreenQuickMatch);
    LM_PTR_DELETE(_pScreenLoading);
    LM_PTR_DELETE(_pScreenExit);
    LM_PTR_DELETE(_pScreenTest);
    LM_PTR_DELETE(_pScreenMessageBox);
}

EAppState StateOutGame::getState() const
{
    return APPSTATE_OUTGAME;
}

void StateOutGame::update(double elapsed)
{
    EConnexionState state = _appContext.pConnexion->getState();

    if(state == CONNEXION_LOST || state == CONNEXION_ERROR)
        manageLostConnexion();

    _appContext.pWorldGUI->update(elapsed);
    _appContext.pGUI->update(elapsed);

    if(_pCurrentScreen != NULL)
        _pCurrentScreen ->update(elapsed);
}

void StateOutGame::render()
{
    Ptr<Renderer::IRenderView> pView = _appContext.pRenderer->getDefaultView();
    _appContext.pCameraGUI->setAspectRatio(float(pView->getWidth())/float(std::max(1, pView->getHeight())));
    _appContext.pWorldGUI->render(pView, _appContext.pCameraGUI);
    _appContext.pGUI->render(pView);
}

void StateOutGame::startState()
{
    Universe::World & world = *_appContext.pWorldGUI;

    Ptr<Universe::Node> pNodeCamTitle = world.getFirstNodeWithTag(L"CAM_TITLE");
    _appContext.pCameraGUI = world.createPerspectiveCamera();
    _appContext.pCameraGUI->addChild(world.getNodeListener());

    if(pNodeCamTitle != null)
    {
        _appContext.pCameraGUI->setWorldPosition(pNodeCamTitle->getWorldPosition());
        _appContext.pCameraGUI->setLocalOrientation(pNodeCamTitle->getLocalOrient());
    }

    if(TEST_MODE)
        setScreen(_pScreenTest.get());
    else
    {
        switch(_appContext.pConnexion->getState())
        {
        default:
        case CONNEXION_IN_PROGRESS:
        case CONNEXION_FAILED:
        case CONNEXION_FAILED_SERVER_FULL:
        case CONNEXION_ERROR:
        case CONNEXION_LOST:
            manageLostConnexion();
            break;
        case CONNEXION_NOT_OPENED:
            setScreen(_pScreenConnexionInProgress.get());
            break;
        case CONNEXION_OK:
            setScreen(_pScreenMainMenu.get());
            break;
        }
    }
    //setScreen(_pScreenSelectCharacter.get());
    //setScreen(_pScreenCreateAccount.get());
    //setScreen(_pScreenMainMenu.get());
    _appContext.pOutGameMusic->start();
}

void StateOutGame::endState()
{
    _appContext.pCameraGUI->kill();
}

void StateOutGame::setScreen(IScreen * pScreen)
{
    if(_pCurrentScreen != NULL)
        _pCurrentScreen->endScreen();

    pScreen->setScreenManager(this);
    pScreen->startScreen();
    _pCurrentScreen = pScreen;
}

void StateOutGame::manageLostConnexion()
{
    switch(_appContext.pConnexion->getState())
    {
    case CONNEXION_ERROR:
        _pScreenMessageBox->setMessage(L"connection-error");
        break;
    case CONNEXION_LOST:
        _pScreenMessageBox->setMessage(L"connection-lost");
        break;
    default:
        LM_ASSERT(false);
        ERR << L"Error in manageLostConnexion => state is " << ::toString(_appContext.pConnexion->getState()) << L"\n";
        _pScreenMessageBox->setMessage(L"connection-lost");
        break;
    }

    if(_appContext.pAvatarGUI != null)
    {
        Universe::ScriptContext ctx;
        ctx.pNode = _appContext.pAvatarGUI;
        _appContext.pWorldGUI->runScript(L"/HOO/GUI/Scripts/CharacterDisparition.lua", ctx);
        _appContext.pAvatarGUI = null;
    }

    _appContext.pConnexion->resetState();

    _pScreenMessageBox->setNextScreenOK(_pScreenConnexionInProgress.get());
    _pScreenMessageBox->setNextScreenCancel(_pScreenExit.get());

    setScreen(_pScreenMessageBox.get());
}


