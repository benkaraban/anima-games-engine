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
#ifndef HOOCLIENT_STATEINGAME_H_
#define HOOCLIENT_STATEINGAME_H_

#include <Core/Standard.h>
#include <Core/Math/CameraController.h>

#include <HOOUserProtocol/GameMessage/GameCommand.h>

#include <HOOCLient/IAppState.h>
#include <HOOClient/InGame/PanelStatus.h>
#include <HOOClient/InGame/PanelSpellsBar.h>
#include <HOOClient/InGame/PanelWillContest.h>

class StateInGame : public IAppState, public GUI::IEventHandler
{
public:
    StateInGame(AppContext & appContext);

    virtual EAppState getState() const;

    virtual void update(double elapsed);
    virtual void render();

    virtual void startState();
    virtual void endState();

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

protected:
#ifdef LM_DEVMODE
    virtual void startStateDebug();
    virtual void updateDebug(double elapsed);
    virtual void endStateDebug();

    Core::CameraQuake           _quakeCamera;
    Ptr<Universe::NodeCamera>   _pDebugCamera;
    bool                        _useDebugCamera;
#endif

    virtual void setCursor(bool active);
    virtual void showWaitingFor(bool show);
    virtual void StateInGame::displayVictoryPanel(int32 winnerId);
    virtual void displayOpponentSpellPanel(int32 spellId, float time = INFINITE);
    virtual void hideOpponentSpellPanel();
    virtual void gameUpdate(double elapsed);
    virtual void checkCommand();
    virtual void checkSynchro();
    virtual void handleCommand(const HOOUserProtocol::GameCommandAnswer & command);
    virtual void updateStatus(const HOOUserProtocol::GameStatusUpdate & status);
    virtual void updateStatusAfterSynchro(const HOOUserProtocol::GameStatusUpdate & status);
    virtual void updateStatusAfterTag1(const HOOUserProtocol::GameStatusUpdate & status, const String & tag);
    virtual void updateStatusAfterTag2(const HOOUserProtocol::GameStatusUpdate & status, const String & tag);
    virtual void showOpponentSpellAfterTag(int32 spellId, const String & tag);

    AppContext &                _appContext;
    ClientGame &                _game;
    Ptr<Connexion>              _pConnexion;
    HOOUserProtocol::GameStatus _status;
    bool                        _firstStatus;
    int32                       _playerId;

    Ptr<GUI::Widget>            _pDialog;
    Ptr<GUI::Widget>            _pDialogUp;
    Ptr<GUI::Widget>            _pDialogCenter;
    Ptr<GUI::Widget>            _pDialogDown;
    Ptr<PanelStatus>            _pPanelStatus;
    Ptr<PanelSpellsBar>         _pPanelSpellsBar;
    Ptr<PanelWillContest>       _pPanelWillContest;
    Ptr<SpellPanel>             _pPanelOpponentSpell;
    float                       _opponentSpellTime;

    #ifdef LM_DEVMODE
        Ptr<GUI::Label>            _pLabelGameplayTags;
    #endif

    Ptr<GUI::Picture>               _pPicInfos;
    Ptr<GUI::Label>                 _pLabelInfos;

    Ptr<Universe::ITask>            _pSynchroTask;
    HOOUserProtocol::EGameCommandAnswer   _lastCommand;

    HOOUserProtocol::GameStatusUpdate   _statusAfterSynchro;
    bool                                _mustUpdateStatusAfterSynchro;

    HOOUserProtocol::GameStatusUpdate   _statusAfterTag1;
    String                              _statusUpdateTag1;
    bool                                _mustUpdateStatusAfterTag1;

    HOOUserProtocol::GameStatusUpdate   _statusAfterTag2;
    String                              _statusUpdateTag2;
    bool                                _mustUpdateStatusAfterTag2;

    int32                               _opponentSpellId;
    String                              _opponentSpellTag;
    bool                                _mustShowOpponentSpellAfterTag;

    // Victory / defeat
    Ptr<GUI::Widget>            _pDialogVictory;
    Ptr<GUI::Button>            _pButtonVictoryOK;

};

#endif
