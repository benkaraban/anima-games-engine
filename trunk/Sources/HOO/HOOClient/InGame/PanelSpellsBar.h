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
#ifndef HOOCLIENT_PANELSPELLSBAR_H_
#define HOOCLIENT_PANELSPELLSBAR_H_

#include <Core/Dictionary.h>

#include <GUI/Widget.h>
#include <GUI/Button.h>
#include <GUI/Manager.h>
#include <GUI/StretchBar.h>

#include <GameEngine/GameLibrary.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

#include <HOOClient/GUI/WidgetFactory.h>
#include <HOOClient/SpellIcon.h>
#include <HOOClient/SpellPanel.h>

LM_ENUM_4(ESpellsBarStatus,
          SPELLSBAR_CHOOSING,
          SPELLSBAR_SPELL_IN_PROGRESS,
          SPELLSBAR_CHOICE_DONE,
          SPELLSBAR_DISABLED);

class PanelSpellsBar : public GUI::Widget, public GUI::IEventHandler
{
public:
    PanelSpellsBar(
        const Ptr<WidgetFactory> & pFactory,
        const Ptr<GameEngine::GameLibrary> & pGameLib,
        const Ptr<GUI::Widget> & pDetailPanels,
        const Core::List<int32> & deck);

    ESpellsBarStatus getState() const { return _status; }
    void disable();
    void setAvailableActions(const Core::List<HOOUserProtocol::ActionCommand> & commands, bool showSpellAfterCast);
    void setStatus(const HOOUserProtocol::PlayerStatus & status);

    const HOOUserProtocol::ActionCommand & getCommand() const { return _command; }

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    virtual void update(double elapsed);

    void quickCast(int32 id);
    void setSpellFocus(int32 id);

protected:
    void setXPanel(int32 index, float x);
    void internalUpdate(double elapsed);
    void internalUpdateChoosing(double elapsed);
    void internalUpdateInProgress(double elapsed);
    void internalUpdateChoiceDone(double elapsed);
    void internalUpdateDisabled(double elapsed);

    Ptr<WidgetFactory>                      _pFactory;
    Ptr<GameEngine::GameLibrary>            _pGameLib;
    Ptr<GUI::Widget>                        _pDetailPanels;
    HOOUserProtocol::ActionCommand          _command;
    ESpellsBarStatus                        _status;
    bool                                    _showSpellAfterCast;
    bool                                    _spellStarted;

    Ptr<GUI::StretchBar>                    _pStretchBar;
    int32                                   _curIndex;
    int32                                   _choiceIndex;
    int32                                   _internalCurIndex;
    int32                                   _lastValidIndex;

    float                                   _x1;
    float                                   _x2;
    float                                   _elapsed;

    Core::List<const GameEngine::Spell *>   _deck;
    Core::List<Ptr<SpellIcon> >             _icons;
    Core::List<Ptr<SpellPanel> >            _panels;
    Core::Map<int32, int32>                 _spellIdToIconIndex;
};

#endif
