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
#include <HOOClient/InGame/PanelSpellsBar.h>
#include <HOOClient/SpellIcon.h>
#include <Core/Math/Interpolate.h>

static const float APPEAR_TIME  = 0.5f;
static const float DELTA_X      = 270.0f;
static const float GREY         = 0.5f;
static const float POSITION_X2  = 700.0f;

struct SpellSorter
{
    bool operator () (const GameEngine::Spell * & pSpell1, const GameEngine::Spell * & pSpell2)
    {
        if(pSpell1->getType() != pSpell2->getType())
            return pSpell1->getType() < pSpell2->getType();
        else if(pSpell1->getSphere() != pSpell2->getSphere())
            return pSpell1->getSphere() < pSpell2->getSphere();
        //else if(pSpell1->getCost() != pSpell2->getCost())
        //    return pSpell1->getCost() < pSpell2->getCost();
        else
            return pSpell1->getId() < pSpell2->getId();
    }
};


PanelSpellsBar::PanelSpellsBar(
        const Ptr<WidgetFactory> & pFactory,
        const Ptr<GameEngine::GameLibrary> & pGameLib,
        const Ptr<GUI::Widget> & pDetailPanels,
        const Core::List<int32> & deck) :
    _pFactory(pFactory), 
    _pGameLib(pGameLib),
    _pDetailPanels(pDetailPanels),
    _status(SPELLSBAR_CHOOSING),
    _showSpellAfterCast(false),
    _spellStarted(false),
    _curIndex(-1),
    _internalCurIndex(-2),
    _lastValidIndex(0),
    _x1(0.0f),
    _x2(0.0f),
    _elapsed(0.0f)
{
    for(int32 ii=0; ii < deck.size(); ii++)
        _deck.push_back(pGameLib->getSpell(deck[ii]));

    std::sort(_deck.begin(), _deck.end(), SpellSorter());

    Ptr<GUI::Manager> pGUI(pFactory->getManager());
    _pStretchBar = Ptr<GUI::StretchBar>(new GUI::StretchBar(pGUI->getResPool(), pGUI->getHUD(), pGUI->getInput()));

    // Pass icon/panel
    {
        Ptr<SpellIcon> pPassIcon(new SpellIcon(*pFactory));
        Ptr<SpellPanel> pPassPanel(new SpellPanel(*pFactory));
        pPassIcon->addEventHandler(this);

        _pStretchBar->addChild(pPassIcon);
        _pDetailPanels->addChild(pPassPanel);

        _icons.push_back(pPassIcon);
        _panels.push_back(pPassPanel);

        _spellIdToIconIndex[0] = 0;
    }

    // Deck

    for(int32 ii=0; ii < _deck.size(); ii++)
    {
        Ptr<SpellIcon> pIcon(new SpellIcon(*_deck[ii], *pFactory));
        Ptr<SpellPanel> pPanel(new SpellPanel(*_deck[ii], *pFactory));
        pIcon->addEventHandler(this);

        _pStretchBar->addChild(pIcon);
        _pDetailPanels->addChild(pPanel);

        _icons.push_back(pIcon);
        _panels.push_back(pPanel);

        _spellIdToIconIndex[_deck[ii]->getId()] = ii + 1;
    }

    _pStretchBar->setStandardSize(Core::Vector2f(58.0f, 58.0f));
    _pStretchBar->setHighlightSize(Core::Vector2f(116.0f, 116.0f));

    addChild(_pStretchBar);
    setSize(_pStretchBar->getSize());

    for(int32 ii=0; ii < _panels.size(); ii++)
        _panels[ii]->setOwnColor(Core::Vector4f(0.0f));

    internalUpdate(0.0);
}

void PanelSpellsBar::internalUpdateChoosing(double elapsed)
{
    if(_curIndex != -1)
    {
        float xcenter = getPosition().x + _icons[_curIndex]->getPosition().x + 0.5f * _icons[_curIndex]->getSize().x;

        if(_showSpellAfterCast)
            _x2 = xcenter - 0.5f * _panels[_curIndex]->getSize().x;
        else
            _x2 = POSITION_X2;

        if(_internalCurIndex != _curIndex)
        {
            _internalCurIndex = _curIndex;
            _lastValidIndex = _curIndex;

            for(int32 ii=0; ii < _panels.size(); ii++)
            {
                if(ii == _curIndex)
                    _panels[ii]->setOwnColor(Core::Vector4f(1.0f), APPEAR_TIME);
                else
                    if(_showSpellAfterCast)
                        _panels[ii]->setOwnColor(Core::Vector4f(GREY, GREY, GREY, 1.0f), APPEAR_TIME);
                    else
                        _panels[ii]->setOwnColor(Core::Vector4f(0.0f), APPEAR_TIME);
            }

            for(int32 ii=0; ii < _panels.size(); ii++)
                _panels[ii]->setVisible(true);

            _x1 = _panels[_curIndex]->getPosition().x;
            _elapsed = 0.0f;
        }
    }
    else
    {
        if(_internalCurIndex != _curIndex)
        {
            _internalCurIndex = _curIndex;

            for(int32 ii=0; ii < _panels.size(); ii++)
                _panels[ii]->setOwnColor(Core::Vector4f(0.0f), APPEAR_TIME);
        }
    }

    static const float MOVE_DURATION = 0.3f;

    float a = _elapsed / MOVE_DURATION;
    float xpanel = Core::lerp(_x1, _x2, Core::interpolate(a, Core::INTER_FADE_OUT));
    _elapsed = std::min(_elapsed + float(elapsed), MOVE_DURATION);

    setXPanel(_lastValidIndex, xpanel);
}

void PanelSpellsBar::setXPanel(int32 index, float xpanel)
{
    Core::Vector2f basePos(xpanel, 200.0f);
    Core::Vector2f rpos(basePos);
    Core::Vector2f lpos(basePos);

    _panels[index]->setPosition(basePos);

    for(int32 ii=index + 1; ii < _panels.size(); ii++)
    {
        rpos.x += DELTA_X;
        _panels[ii]->setPosition(rpos);
    }

    for(int32 ii=index - 1; ii >= 0; ii--)
    {
        lpos.x -= DELTA_X;
        _panels[ii]->setPosition(lpos);
    }
}

void PanelSpellsBar::internalUpdateInProgress(double elapsed)
{
    if(!_spellStarted)
    {
        for(int32 ii=0; ii < _panels.size(); ii++)
        {
            if(ii == _choiceIndex)
                _panels[ii]->setOwnColor(Core::Vector4f(1.0f), APPEAR_TIME);
            else
                _panels[ii]->setOwnColor(Core::Vector4f(0.0f), APPEAR_TIME);
        }

        for(int32 ii=0; ii < _panels.size(); ii++)
            _panels[ii]->setVisible(true);

        _x1 = _panels[_choiceIndex]->getPosition().x;
        _elapsed = 0.0f;
        _spellStarted = true;
        _x2 = POSITION_X2;
    }

    static const float MOVE_DURATION = 0.5f;

    float a = _elapsed / MOVE_DURATION;
    float xpanel = Core::lerp(_x1, _x2, Core::interpolate(a, Core::INTER_FADE_OUT));
    _elapsed = std::min(_elapsed + float(elapsed), MOVE_DURATION);

    setXPanel(_choiceIndex, xpanel);
}

void PanelSpellsBar::internalUpdate(double elapsed)
{
    switch(_status)
    {
    case SPELLSBAR_CHOOSING:
        internalUpdateChoosing(elapsed);
        break;
    case SPELLSBAR_SPELL_IN_PROGRESS:
        internalUpdateInProgress(elapsed);
        break;
    case SPELLSBAR_CHOICE_DONE:
        internalUpdateChoiceDone(elapsed);
        break;
    case SPELLSBAR_DISABLED:
        internalUpdateDisabled(elapsed);
        break;
    }
}

void PanelSpellsBar::update(double elapsed)
{
    GUI::Widget::update(elapsed);

    if(isEnabled())
    {
        internalUpdate(elapsed);
    }
}

void PanelSpellsBar::disable()
{
    setOwnColor(Core::Vector4f(0.0f), 0.25f);
    _status = SPELLSBAR_DISABLED;
    _spellStarted = false;
}

void PanelSpellsBar::setAvailableActions(const Core::List<HOOUserProtocol::ActionCommand> & commands, bool showSpellAfterCast)
{
    setOwnColor(Core::Vector4f(1.0f), 0.25f);
    _showSpellAfterCast = showSpellAfterCast;
    _status = SPELLSBAR_CHOOSING;

    for(int32 ii=0; ii < _icons.size(); ii++)
    {
        _icons[ii]->setSpellEnabled(false);
        _panels[ii]->setSpellEnabled(false);
    }

    for(int32 ii=0; ii < commands.size(); ii++)
    {
        if(commands[ii].actionType == 0) // cast
        {
            Core::Map<int32, int32>::const_iterator iIndex = _spellIdToIconIndex.find(commands[ii].actionValue);

            if(iIndex != _spellIdToIconIndex.end())
            {
                _icons[iIndex->second]->setSpellEnabled(true);
                _panels[iIndex->second]->setSpellEnabled(true);
            }
        }
        else if(commands[ii].actionType == 2) // pass
        {
            _icons[0]->setSpellEnabled(true);
        }
    }
}

void PanelSpellsBar::setStatus(const HOOUserProtocol::PlayerStatus & status)
{
    for(int32 ii=0; ii < _icons.size(); ii++)
    {
        _icons[ii]->setStatus(status);
        _panels[ii]->setStatus(status);
    }
}

void PanelSpellsBar::internalUpdateChoiceDone(double elapsed)
{
    if(!_spellStarted)
    {
        for(int32 ii=0; ii < _panels.size(); ii++)
            _panels[ii]->setOwnColor(Core::Vector4f(0.0f), APPEAR_TIME);

        _spellStarted = true;
    }
}

void PanelSpellsBar::internalUpdateDisabled(double elapsed)
{
    if(!_spellStarted)
    {
        for(int32 ii=0; ii < _panels.size(); ii++)
            _panels[ii]->setOwnColor(Core::Vector4f(0.0f), APPEAR_TIME);

        _spellStarted = true;
    }
}

void PanelSpellsBar::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(isEnabled())
    {
        if(ev == GUI::EV_OVER_BEGIN)
        {
            _curIndex = _spellIdToIconIndex[pWidget->getId()];
        }
        else if(ev == GUI::EV_OVER_END)
        {
            if(_spellIdToIconIndex[pWidget->getId()] == _curIndex)
                _curIndex = -1;
        }
        else if(ev == GUI::EV_COMMAND)
        {
            if(_showSpellAfterCast)
                _status = SPELLSBAR_SPELL_IN_PROGRESS;
            else
                _status = SPELLSBAR_CHOICE_DONE;

            _spellStarted = false;
            _choiceIndex = _spellIdToIconIndex[pWidget->getId()];
            LM_ASSERT(_curIndex == _choiceIndex);

            if(pWidget->getId() == 0)
            {
                _command.actionType = 2; // pass
            }
            else
            {
                _command.actionType = 0; // cast
                _command.actionValue = pWidget->getId();
            }

            for(int32 ii=0; ii < _icons.size(); ii++)
                _icons[ii]->setSpellEnabled(false);

            notify(GUI::EV_COMMAND);
        }

        internalUpdate(0.0);
    }
}


void PanelSpellsBar::quickCast(int32 ithSpell)
{
    if(_status == SPELLSBAR_CHOOSING)
    {
        if(_showSpellAfterCast)
            _status = SPELLSBAR_SPELL_IN_PROGRESS;
        else
            _status = SPELLSBAR_CHOICE_DONE;

        _spellStarted = false;
        //_choiceIndex = _spellIdToIconIndex[_deck[ithSpell-1]->getId()];

        
        if(ithSpell == 0)
        {
            _curIndex = _spellIdToIconIndex[0];    
            _choiceIndex = _spellIdToIconIndex[0];
            _command.actionType = 2; // pass
        }
        else
        {
            _curIndex = _spellIdToIconIndex[_deck[ithSpell-1]->getId()];
            _choiceIndex = _spellIdToIconIndex[_deck[ithSpell-1]->getId()];
            _command.actionType = 0; // cast
            _command.actionValue = _deck[ithSpell-1]->getId();
        }

        for(int32 ii=0; ii < _icons.size(); ii++)
            _icons[ii]->setSpellEnabled(false);

        notify(GUI::EV_COMMAND);

        internalUpdate(0.0);
    }
}
void PanelSpellsBar::setSpellFocus(int32 ithSpell)
{
    if(ithSpell != -1)
        if(ithSpell == 0)
            _curIndex = 0;
        else
            _curIndex = _spellIdToIconIndex[_deck[ithSpell-1]->getId()];
    else
        _curIndex = -1;
}