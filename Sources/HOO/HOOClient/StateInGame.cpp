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

#include <Window/Tools.h>

#include <Universe/Tasks/LookAt.h>
#include <Universe/Tasks/TurnAround.h>

#include <HOOClient/StateInGame.h>

using namespace HOOUserProtocol;

#ifdef LM_DEVMODE
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void StateInGame::startStateDebug()
{
    Universe::World & world = *_game.getWorld();
    _pDebugCamera = world.createPerspectiveCamera(f_PI_DIV_4, 1.0, 0.2f, 1000.0f);
    _useDebugCamera = false;

    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pLabelGameplayTags = pWidgets->createMediumLabel(L"", 300, 0);
    _pLabelGameplayTags->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pLabelGameplayTags->setFontSize(10);
    _pLabelGameplayTags->setTextColor(Core::Vector4f(1.0,0.5,0.5,1.0));
    _pLabelGameplayTags->setVisible(false);
    //_pLabelInfos->setSize(Core::Vector2f(800.0f,600.0f));
    _pLabelGameplayTags->setDictionary(null);
    _pDialogUp->addChild(_pLabelGameplayTags);
}

void StateInGame::updateDebug(double elapsed)
{
    const Window::InputManager & input = *_appContext.pInput;

    if(input.isKeyTyped(VK_F1))
    {
        _pLabelGameplayTags->setVisible(!_pLabelGameplayTags->isVisible());
    }

    if(input.isKeyTyped('C'))
    {
        _useDebugCamera = !_useDebugCamera;
        _quakeCamera = Core::CameraQuake(_game.getCamera()->getWorldPosition());
    }

    if(_useDebugCamera)
    {
        Window::updateQuakeCamera(_quakeCamera, input, input.isKeyDown(VK_SPACE) ? 20.0 : 5.0, 0.003, elapsed);
        _pDebugCamera->setLocalMatrix(_quakeCamera.getWorldMatrix());
    }

    _pLabelGameplayTags->setText(Core::toString(_game.getGameplayTags()));
}

void StateInGame::endStateDebug()
{
    _pDebugCamera = null;
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#endif

static const float OPPONENT_SPELL_PANEL_DISPLAY_TIME = 4.0f;

StateInGame::StateInGame(AppContext & appContext) :
    _appContext(appContext),
    _game(appContext.game)
{}

EAppState StateInGame::getState() const
{
    return APPSTATE_INGAME;
}

void StateInGame::update(double elapsed)
{
    EConnexionState state = _appContext.pConnexion->getState();

    if(state == CONNEXION_LOST || state == CONNEXION_ERROR)
    {
        _appContext.nextState = APPSTATE_OUTGAME;
        return;
    }

    const Window::InputManager & input = *_appContext.pInput;

    if(_opponentSpellTime > 0.0f)
    {
        _opponentSpellTime -= float(elapsed);
        if(_opponentSpellTime <= 0.0f)
            hideOpponentSpellPanel();
    }

    if(input.isKeyDown(VK_SPACE))
        this->_pPanelSpellsBar->setSpellFocus(0);
    else if(input.isKeyDown('1'))
        this->_pPanelSpellsBar->setSpellFocus(1);
    else if(input.isKeyDown('2'))
        this->_pPanelSpellsBar->setSpellFocus(2);
    else if(input.isKeyDown('3'))
        this->_pPanelSpellsBar->setSpellFocus(3);
    else if(input.isKeyDown('4'))
        this->_pPanelSpellsBar->setSpellFocus(4);
    else if(input.isKeyDown('5'))
        this->_pPanelSpellsBar->setSpellFocus(5);
    else if(input.isKeyDown('6'))
        this->_pPanelSpellsBar->setSpellFocus(6);
    else if(input.isKeyDown('7'))
        this->_pPanelSpellsBar->setSpellFocus(7);
    else if(input.isKeyDown('8'))
        this->_pPanelSpellsBar->setSpellFocus(8);
    else if(input.isKeyDown('9'))
        this->_pPanelSpellsBar->setSpellFocus(9);
    else if(input.isKeyDown('0'))
        this->_pPanelSpellsBar->setSpellFocus(10);
    else if(input.isKeyReleased(VK_SPACE))
        this->_pPanelSpellsBar->quickCast(0);
    else if(input.isKeyReleased('1'))
        this->_pPanelSpellsBar->quickCast(1);
    else if(input.isKeyReleased('2'))
        this->_pPanelSpellsBar->quickCast(2);
    else if(input.isKeyReleased('3'))
        this->_pPanelSpellsBar->quickCast(3);
    else if(input.isKeyReleased('4'))
        this->_pPanelSpellsBar->quickCast(4);
    else if(input.isKeyReleased('5'))
        this->_pPanelSpellsBar->quickCast(5);
    else if(input.isKeyReleased('6'))
        this->_pPanelSpellsBar->quickCast(6);
    else if(input.isKeyReleased('7'))
        this->_pPanelSpellsBar->quickCast(7);
    else if(input.isKeyReleased('8'))
        this->_pPanelSpellsBar->quickCast(8);
    else if(input.isKeyReleased('9'))
        this->_pPanelSpellsBar->quickCast(9);
    else if(input.isKeyReleased('0'))
        this->_pPanelSpellsBar->quickCast(10);
    /*else
        this->_pPanelSpellsBar->setSpellFocus(-1);*/


    if(_lastCommand == GAME_START && _pSynchroTask != null && !_pSynchroTask->isFinished() && _appContext.pInput->isKeyTyped(VK_ESCAPE))
        _pSynchroTask->kill();

    gameUpdate(elapsed);
    _game.getWorld()->update(elapsed);
    _appContext.pGUI->update(elapsed);

#ifdef LM_DEVMODE
    updateDebug(elapsed);
#endif
}

void StateInGame::render()
{
    Ptr<Universe::NodeCamera> pCamera = _game.getCamera();
#ifdef LM_DEVMODE
    if(_useDebugCamera)
        pCamera = _pDebugCamera;
#endif

    Ptr<Renderer::IRenderView> pView = _appContext.pRenderer->getDefaultView();
    pCamera->setAspectRatio(float(pView->getWidth())/float(std::max(1, pView->getHeight())));
    _game.getWorld()->render(pView, pCamera);
    _appContext.pGUI->render(pView);
}

void StateInGame::startState()
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _game.setCamPosStart();
    

    Ptr<GUI::Widget> pPriorityBooster1(new GUI::Widget());
    _pDialog = Ptr<GUI::Widget>(new GUI::Widget());

    _pDialogUp = pWidgets->createDialogBaseTop(768);
    _pDialogCenter = pWidgets->createDialogBase();
    _pDialogDown = pWidgets->createDialogBaseBottom(200);

    _pDialog->addChild(_pDialogUp);
    _pDialog->addChild(_pDialogCenter);
    _pDialog->addChild(_pDialogDown);

    pPriorityBooster1->setDeltaPriority(10000);
    _pDialogCenter->addChild(pPriorityBooster1);

    _pPanelWillContest = Ptr<PanelWillContest>(new PanelWillContest(pWidgets));
    _pPanelStatus = Ptr<PanelStatus>(new PanelStatus(_appContext.pWidgets));
    
    _pPanelSpellsBar = Ptr<PanelSpellsBar>(new PanelSpellsBar(_appContext.pWidgets, _appContext.pGameLib, pPriorityBooster1, _appContext.account.equipedSpells));
    _pPanelSpellsBar->setPosition(Core::Vector2f(_pDialogDown->getSize().x * 0.5f - _pPanelSpellsBar->getSize().x * 0.5f, 130.0f));

    _pPanelWillContest->addEventHandler(this);
    _pPanelSpellsBar->addEventHandler(this);

    _pDialogUp->addChild(_pPanelStatus);
    _pDialogCenter->addChild(_pPanelWillContest);
    _pDialogDown->addChild(_pPanelSpellsBar);

    _pPanelStatus->setOwnColor(Core::Vector4f(0.0), 0.0f);
    _pPanelSpellsBar->setOwnColor(Core::Vector4f(0.0), 0.0f);
    _pPanelSpellsBar->setEnabled(false);

    // Waiting for {
    _pPicInfos = pWidgets->createPicture(L"/HOO/GUI/Widgets/button-large.tga", 1024 / 2 - 357 / 2, 300);
    _pLabelInfos = pWidgets->createMediumLabel(L"", 0, 0);
    _pLabelInfos->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pLabelInfos->setFontSize(10);
    _pLabelInfos->setDictionary(null);
    _pLabelInfos->setSize(_pPicInfos->getSize());

    _pPicInfos->addChild(_pLabelInfos);
    _pPicInfos->setOwnColor(Core::Vector4f(0.0f));

    _pDialogCenter->addChild(_pPicInfos);
    // } Waiting for

    _appContext.pGUI->setDialog(_pDialog);

    _pConnexion = _appContext.pConnexion;
    _pSynchroTask = null;
    _lastCommand = (HOOUserProtocol::EGameCommandAnswer)-1;
    _firstStatus = true;
    _mustUpdateStatusAfterSynchro = false;
    _mustUpdateStatusAfterTag1 = false;
    _mustUpdateStatusAfterTag2 = false;
    _mustShowOpponentSpellAfterTag = false;

    _playerId = _appContext.playerId;
    _opponentSpellTime = 0.0f;


#ifdef LM_DEVMODE
    startStateDebug();
#endif
}

void StateInGame::endState()
{
#ifdef LM_DEVMODE
    endStateDebug();
#endif

    _pPanelOpponentSpell = null;

    _pDialog = null;
    _pPanelStatus = null;
    _pPanelSpellsBar = null;

    _game.freeLevel();
}

void StateInGame::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pPanelWillContest.get())
        {
            _pConnexion->send(HOOUserProtocol::GameCommand(_pPanelWillContest->getCommand()));
            setCursor(false);
        }
        else if(pWidget == _pPanelSpellsBar.get())
        {
            _pConnexion->send(HOOUserProtocol::GameCommand(_pPanelSpellsBar->getCommand()));
            setCursor(false);
        }
        else if(pWidget == _pButtonVictoryOK.get())
        {
            _pConnexion->send(HOOUserProtocol::GameCommand(HOOUserProtocol::LEAVE_GAME));
            _appContext.nextState = APPSTATE_OUTGAME;
        }
    }
}

void StateInGame::updateStatus(const HOOUserProtocol::GameStatusUpdate & status)
{
    status.applyUpdate(_status);

    if(_firstStatus)
    {
        _pPanelStatus->initStatus(_status, _appContext.login, _game.getOpponentName());;
        _firstStatus = false;
    }
    else
    {
        _pPanelStatus->setStatus(_status);
    }

    if(_pPanelSpellsBar->getState() != SPELLSBAR_SPELL_IN_PROGRESS)
        _pPanelSpellsBar->setStatus(_status.player);

    _game.statutsUpdate(_status);
}

void StateInGame::updateStatusAfterSynchro(const HOOUserProtocol::GameStatusUpdate & status)
{
    _statusAfterSynchro = status;
    _mustUpdateStatusAfterSynchro = true;
}

void StateInGame::updateStatusAfterTag1(const HOOUserProtocol::GameStatusUpdate & status, const String & tag)
{
    _statusAfterTag1 = status;
    _statusUpdateTag1 = tag;
    _mustUpdateStatusAfterTag1 = true;
}

void StateInGame::updateStatusAfterTag2(const HOOUserProtocol::GameStatusUpdate & status, const String & tag)
{
    _statusAfterTag2 = status;
    _statusUpdateTag2 = tag;
    _mustUpdateStatusAfterTag2 = true;
}

void StateInGame::showOpponentSpellAfterTag(int32 spellId, const String & tag)
{
    _opponentSpellId = spellId;
    _opponentSpellTag = tag;
    _mustShowOpponentSpellAfterTag = true;
}


void StateInGame::setCursor(bool active)
{
    if(active)
        _appContext.pWidgets->setCursor(CURSOR_ARROW);
    else
        _appContext.pWidgets->setCursor(CURSOR_WAIT);
}

void StateInGame::showWaitingFor(bool show)
{
    if(show)
    {
        String base((*_appContext.pDico)[L"waiting-for"]);
        _pLabelInfos->setText(Core::strFormat(base, _game.getOpponentName()));
        _pPicInfos->setOwnColor(Core::Vector4f(1.0f), 0.25f);
    }
    else
    {
        _pPicInfos->setOwnColor(Core::Vector4f(0.0f), 0.25f);
    }
}

void StateInGame::handleCommand(const HOOUserProtocol::GameCommandAnswer & command)
{
    bool isPlayerTurn = command.getCasterId() == _playerId;

    switch(command.getCommand())
    {
    case GAME_START:
        updateStatus(command.getStatusUpdate());
        setCursor(false);
        _pSynchroTask = _game.introScript();
        _game.startMusic();
        break;

    case WILL_CONTEST:
        updateStatus(command.getStatusUpdate());
        setCursor(true);
        _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), true);
        _pPanelWillContest->setTotalWill(_status.player.willpower);
        _pPanelWillContest->setContest(command.getManaCount(), command.getManaSphere(), command.getAvailableActions());
        _game.willContest(command.getManaCount(), command.getManaSphere());
        break;

    case WILL_RESOLVE:
        updateStatus(command.getStatusUpdate());
        setCursor(false);
        _pPanelWillContest->resolve(command.getBidWinner() == _playerId);
        _pSynchroTask = _game.willResolve(command.getBidWinner());
        break;

    case PLAYER_TURN:
        updateStatus(command.getStatusUpdate());
        setCursor(isPlayerTurn);
        showWaitingFor(!isPlayerTurn);
        _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), true);
        _game.playerTurn(command.getCasterId());
        break;

    case PLAYER_CAST_SPELL:
        updateStatus(command.getStatusUpdate());
        setCursor(!isPlayerTurn);
        showWaitingFor(isPlayerTurn);
        if(!isPlayerTurn)
        {
            displayOpponentSpellPanel(command.getSpellId());
            _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), false);
        }
        _game.playerCastSpell(command.getCasterId(), command.getSpellId());
        break;

    case PLAYER_SPELL_HIT:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        setCursor(false);
        showWaitingFor(false);
        hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerSpellHit();
        break;

    case PLAYER_SPELL_HIT_AFTER_TRICK:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        updateStatusAfterTag2(command.getStatusUpdateAfterOpponentTrick(), L"trick_status_update");
        setCursor(false);
        showWaitingFor(false);
        if(!isPlayerTurn)
            displayOpponentSpellPanel(command.getSpellId(), OPPONENT_SPELL_PANEL_DISPLAY_TIME);
        else
            hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerSpellHitAfterTrick(command.getSpellId());
        break;

    case PLAYER_SPELL_COUNTERED:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        updateStatusAfterTag2(command.getStatusUpdateAfterOpponentTrick(), L"counter_status_update");
        setCursor(false);
        showWaitingFor(false);
        if(!isPlayerTurn)
            showOpponentSpellAfterTag(command.getSpellId(), L"spell_ready_for_counter");
        else
            hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerSpellCountered(command.getSpellId());
        break;
    
    case PLAYER_CAST_TRICK:
        updateStatus(command.getStatusUpdate());
        setCursor(!isPlayerTurn);
        showWaitingFor(isPlayerTurn);
        if(!isPlayerTurn)
        {
            displayOpponentSpellPanel(command.getSpellId());
            _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), false);
        }
        _game.playerCastTrick(command.getCasterId(), command.getSpellId());
        break;

    case PLAYER_TRICK_HIT:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        setCursor(false);
        showWaitingFor(false);
        hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerTrickHit();
        break;

    case PLAYER_TRICK_HIT_AFTER_TRICK:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        updateStatusAfterTag2(command.getStatusUpdateAfterOpponentTrick(), L"trick_status_update");
        setCursor(false);
        showWaitingFor(false);
        if(!isPlayerTurn)
            displayOpponentSpellPanel(command.getSpellId(), OPPONENT_SPELL_PANEL_DISPLAY_TIME);
        else
            hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerTrickHitAfterTrick(command.getSpellId());
        break;

    case PLAYER_START_PASS:
        updateStatus(command.getStatusUpdate());
        setCursor(!isPlayerTurn);
        showWaitingFor(isPlayerTurn);
        if(!isPlayerTurn)
        {
            displayOpponentSpellPanel(0);
            _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), false);
        }
        _game.playerPass(command.getCasterId());
        break;

    case PLAYER_PASS_FINISH:
        updateStatusAfterSynchro(command.getStatusUpdate());
        setCursor(false);
        showWaitingFor(false);
        hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerPassFinish();
        break;

    case PLAYER_PASS_FINISH_AFTER_TRICK:
        updateStatusAfterSynchro(command.getStatusUpdate());
        updateStatusAfterTag1(command.getStatusUpdateAfterOpponentTrick(), L"trick_status_update");
        setCursor(false);
        showWaitingFor(false);
        if(!isPlayerTurn)
            displayOpponentSpellPanel(command.getSpellId(), OPPONENT_SPELL_PANEL_DISPLAY_TIME);
        else
            hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerPassFinishAfterTrick(command.getSpellId());
        break;

    case PLAYER_CAST_RETRIBUTION:
        updateStatus(command.getStatusUpdate());
        setCursor(!isPlayerTurn);
        showWaitingFor(isPlayerTurn);
        if(!isPlayerTurn)
        {
            displayOpponentSpellPanel(command.getSpellId());
            _pPanelSpellsBar->setAvailableActions(command.getAvailableActions(), false);
        }
        _game.playerCastRetribution(command.getCasterId(), command.getSpellId());
        break;

    case PLAYER_RETRIBUTION_HIT:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        setCursor(false);
        showWaitingFor(false);
        hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerRetributionHit();
        break;

    case PLAYER_RETRIBUTION_HIT_AFTER_TRICK:
        updateStatusAfterTag1(command.getStatusUpdate(), L"spell_impact");
        updateStatusAfterTag2(command.getStatusUpdateAfterOpponentTrick(), L"trick_status_update");
        setCursor(false);
        showWaitingFor(false);
        if(!isPlayerTurn)
            displayOpponentSpellPanel(command.getSpellId(), OPPONENT_SPELL_PANEL_DISPLAY_TIME);
        else
            hideOpponentSpellPanel();
        _pPanelSpellsBar->disable();
        _pSynchroTask = _game.playerRetributionHitAfterTrick(command.getSpellId());
        break;

    case GAME_FINISHED:
        updateStatus(command.getStatusUpdate());
        showWaitingFor(false);
        _pPanelSpellsBar->disable();
        _game.gameFinished(command.getCasterId());
        displayVictoryPanel(command.getCasterId());
        setCursor(true);
        break;
    }

    _lastCommand = command.getCommand();
}

void StateInGame::displayVictoryPanel(int32 winnerId)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;
    _pDialog = pWidgets->createDialogBase();
    _pDialogVictory = pWidgets->createMessageBoxMedium();

    Ptr<GUI::Label> pLabel = pWidgets->createMediumLabel(L"draw", 5, 5);
    pLabel->setSize(Core::Vector2f(535.0f, 100.0f));
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setVLayout(GUI::VL_CENTER);

    if(winnerId != 0)
    {
        String winnerName((winnerId == _playerId) ? _appContext.login : _game.getOpponentName());

        pLabel->setDictionary(null);
        pLabel->setText(Core::strFormat((*_appContext.pDico)[L"x-win-the-duel"], winnerName));
    }
 
    _pButtonVictoryOK = pWidgets->createMediumButton(L"ok", 0, 0);
    _pButtonVictoryOK->setHotKey(VK_RETURN);
    _pButtonVictoryOK->setPosition(Core::Vector2f(0.5f * (_pDialogVictory->getSize().x - _pButtonVictoryOK->getSize().x), 110.0f));
    _pButtonVictoryOK->addEventHandler(this);

    _pDialogVictory->addChild(pLabel);
    _pDialogVictory->addChild(_pButtonVictoryOK);

    _pDialog->addChild(_pDialogVictory);

    _appContext.pGUI->pushDialog(_pDialog);
}

void StateInGame::checkCommand()
{
    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_GAME_COMMAND_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_GAME_COMMAND_ANSWER)
        {
            Ptr<GameCommandAnswer> pCommand = LM_DEBUG_PTR_CAST<GameCommandAnswer>(pMessage);
            handleCommand(*pCommand);
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when waiting for game command answer.\n";
        }
    }
}

void StateInGame::displayOpponentSpellPanel(int32 spellId, float time)
{
    if(_pPanelOpponentSpell != null)
        _pDialogCenter->removeChild(_pPanelOpponentSpell);

    if(spellId == 0)
        _pPanelOpponentSpell = Ptr<SpellPanel>(new SpellPanel(*_appContext.pWidgets));
    else
        _pPanelOpponentSpell = Ptr<SpellPanel>(new SpellPanel(*_appContext.pGameLib->getSpell(spellId), *_appContext.pWidgets));
    _pPanelOpponentSpell->setPosition(Core::Vector2f(100.0f, 200.0f));
    _pPanelOpponentSpell->setOwnColor(Core::Vector4f(0.0f));
    _pPanelOpponentSpell->setOwnColor(Core::Vector4f(1.0f), 0.25f);
    _pDialogCenter->addChild(_pPanelOpponentSpell);
    _opponentSpellTime = time;
}

void StateInGame::hideOpponentSpellPanel()
{
    if(_pPanelOpponentSpell != null)
    {
        _pPanelOpponentSpell->setOwnColor(Core::Vector4f(0.0f), 0.25f);
        _opponentSpellTime = 0.0f;
    }
}

void StateInGame::checkSynchro()
{
    if(_pSynchroTask != null && _pSynchroTask->isFinished())
    {
        HOOUserProtocol::GameCommand command(HOOUserProtocol::SYNCHRONISATION);
        _pConnexion->send(command);
        INF << L"Sending synchro\n";
        _pSynchroTask = null;

        if(_mustUpdateStatusAfterSynchro)
        {
            updateStatus(_statusAfterSynchro);
            _mustUpdateStatusAfterSynchro = false;
        }

        if(_lastCommand == GAME_START)
        {
            _pPanelStatus->setOwnColor(Core::Vector4f(1.0), 0.25f);
            _pPanelSpellsBar->setOwnColor(Core::Vector4f(1.0), 0.0f);
            _pPanelSpellsBar->setEnabled(true);
        }
    }
}

void StateInGame::gameUpdate(double elapsed)
{
    if(_mustUpdateStatusAfterTag1)
    {
        if(_appContext.game.hasTag(_statusUpdateTag1))
        {
            updateStatus(_statusAfterTag1);
            _mustUpdateStatusAfterTag1 = false;
        }
    }

    if(_mustUpdateStatusAfterTag2)
    {
        if(_appContext.game.hasTag(_statusUpdateTag2))
        {
            updateStatus(_statusAfterTag2);
            _mustUpdateStatusAfterTag2 = false;
        }
    }

    if(_mustShowOpponentSpellAfterTag)
    {
        if(_appContext.game.hasTag(_opponentSpellTag))
        {
            displayOpponentSpellPanel(_opponentSpellId, 3.0f);
            _mustShowOpponentSpellAfterTag = false;
        }
    }

    if(_pSynchroTask != null)
        checkSynchro();
    else
        checkCommand();
}
