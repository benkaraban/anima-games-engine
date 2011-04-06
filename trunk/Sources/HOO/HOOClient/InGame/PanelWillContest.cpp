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
#include <HOOClient/InGame/PanelWillContest.h>
#include <GameEngine/Enums.h>
#include <Core/Strings/Tools.h>
#include <Core/Math/Interpolate.h>

static const float APPEAR_TIME = 0.25f;
static const float DELTA_TIME = 3.0f;
static const Core::Vector2f MANA_PIC_SIZE(44.0f, 44.0f);
const int32 MANA_PANEL = 170;

PanelWillContest::PanelWillContest(const Ptr<WidgetFactory> & pFactory) : _pFactory(pFactory)
{
    _pPanel = Ptr<GUI::Widget>(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster1(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster2(new GUI::Widget());

    pPriorityBooster1->setDeltaPriority(1000);
    pPriorityBooster2->setDeltaPriority(2000);

    _pAvailablePic = pFactory->createPicture(L"/HOO/GUI/Widgets/button-large.tga", 0,  0);
    _pAvailableLabel = pFactory->createInGameMediumLabel(L"mana-stake", 0, 0, true);
    _pAvailableLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pAvailableLabel->setSize(_pAvailablePic->getSize());
    _pAvailablePic->addChild(_pAvailableLabel);

    _pManaPic = pFactory->createPicture(L"/HOO/GUI/Widgets/icon-mana-b.tga", MANA_PANEL, 50);
    _pManaPic->setSize(MANA_PIC_SIZE);

    _pManaLabel = pFactory->createMediumDigitsLabel(L"0", -40, 0);
    _pManaLabel->setSize(_pManaPic->getSize());
    _pManaLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pManaPic->addChild(_pManaLabel);

    _pWillPic = pFactory->createPicture(L"/HOO/GUI/Widgets/icon-willpower.tga", -30, -30);
    _pWillLabel = pFactory->createMediumDigitsLabel(L"0", -1, 0);
    _pWillLabel->setSize(_pWillPic->getSize());
    _pWillLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pWillPic->addChild(_pWillLabel);

    _pBidLabel = pFactory->createMediumDigitsLabel(L"0", 0, 0);

    _pWillBack = pFactory->createPicture(L"/HOO/GUI/Widgets/bar-willpower-border.tga", 20, 110);
    _pWillBar = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(pFactory->getManager()->getResPool(), pFactory->getManager()->getHUD()));
    _pWillBar->setBarInterior(L"/HOO/GUI/Widgets/bar-willpower-inside.tga");
    _pWillBar->setBarBorder(L"/HOO/GUI/Widgets/bar-willpower-border.tga");

    _pWillBack->addChild(_pWillBar);
    _pWillBack->addChild(pPriorityBooster1);
    _pWillBack->addChild(pPriorityBooster2);

    pPriorityBooster1->addChild(_pWillPic);
    pPriorityBooster2->addChild(_pBidLabel);

    addChild(_pPanel);

    _pPanel->addChild(_pAvailablePic);
    _pPanel->addChild(_pManaPic);
    _pPanel->addChild(_pWillBack);
    _pPanel->pack();

    setPosition(Core::Vector2f(512.0f, 320.0f) - 0.5f * _pPanel->getSize());

    setOwnColor(Core::Vector4f(0.0f));
    _maxBid = 0;
}

void PanelWillContest::setContest(int32 manaCount, int32 manaSphere, const Core::List<HOOUserProtocol::ActionCommand> & commands)
{
    _state = WILL_PANEL_WAIT_CHOICE;
    _pManaLabel->setText(Core::toString(manaCount));

    switch((GameEngine::ESphere)manaSphere)
    {
    case GameEngine::SPHERE_ENERGY:
        _pManaPic->setPicture(L"/HOO/GUI/Widgets/icon-mana-r.tga");
        break;
    case GameEngine::SPHERE_SPIRIT:
        _pManaPic->setPicture(L"/HOO/GUI/Widgets/icon-mana-b.tga");
        break;
    case GameEngine::SPHERE_LIFE:
        _pManaPic->setPicture(L"/HOO/GUI/Widgets/icon-mana-g.tga");
        break;
    }

    _maxBid = 0;

    for(int32 ii=0; ii < commands.size(); ii++)
    {
        if(commands[ii].actionType == 1)
            _maxBid = std::max(_maxBid, commands[ii].actionValue);
    }

    _pManaPic->setPosition(Core::Vector2f(float(MANA_PANEL), 50.0f));
    _pManaPic->setSize(MANA_PIC_SIZE);
    setOwnColor(Core::Vector4f(1.0f), APPEAR_TIME);
    _pWillBar->setOwnColor(Core::Vector4f(0.5f), APPEAR_TIME);
}

void PanelWillContest::resolve(bool playerWin)
{
    _state = WILL_PANEL_RESOLVE;
    _elapsed = 0.0f;
    _x1 = float(MANA_PANEL);
    _x2 = playerWin ? (MANA_PANEL - 200.0f) : (MANA_PANEL + 200.0f);
    setOwnColor(Core::Vector4f(0.0f), 1.0f);
}

void PanelWillContest::update(double elapsed)
{
    Widget::update(elapsed);

    Core::Vector2f lm = _pFactory->getManager()->getInput()->getMousePos() - _pWillBack->getScreenPos();
    bool mouseOver = (lm.x >= 0.0f) && (lm.y >= 0.0f) && (lm.x < _pWillBack->getSize().x) && (lm.y < _pWillBack->getSize().y);
    bool lmbDown =  _pFactory->getManager()->getInput()->isLMBDown();

    switch(_state)
    {
    case WILL_PANEL_WAIT_CHOICE:
        if(mouseOver)
        {
            _state = WILL_PANEL_WAIT_CHOICE_MOUSE_OVER;
            _pWillBar->setOwnColor(Core::Vector4f(1.0f), APPEAR_TIME);
        }
        break;

    case WILL_PANEL_WAIT_CHOICE_MOUSE_OVER:
        if(!mouseOver)
        {
            _state = WILL_PANEL_WAIT_CHOICE;
            _pWillBar->setOwnColor(Core::Vector4f(0.5f), APPEAR_TIME);
        }
        else if(lmbDown)
        {
            _state = WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN_OVER;
        }
        break;

    case WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN_OVER:
        if(!mouseOver)
        {
            _state = WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN;
        }
        else if(!lmbDown)
        {
            _state = WILL_PANEL_CHOICE_DONE;
            _command.actionType = 1; // bet
            _command.actionValue = _step - 1;
            notify(GUI::EV_COMMAND);
            _pWillBar->setOwnColor(Core::Vector4f(0.5f, 0.5f, 0.5f, 1.0f), APPEAR_TIME);
        }
        break;

    case WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN:
        if(!lmbDown)
        {
            _state = WILL_PANEL_WAIT_CHOICE;
            _pWillBar->setOwnColor(Core::Vector4f(0.5f), APPEAR_TIME);
        }
        break;

    case WILL_PANEL_CHOICE_DONE:
    case WILL_PANEL_RESOLVE:
        break;
    }

    if(_state == WILL_PANEL_RESOLVE)
    {
        _elapsed += float(elapsed);

        float t = std::min(1.0f, _elapsed / 1.0f);
        float x = Core::lerp(_x1, _x2, Core::interpolate(t, Core::INTER_FADE_IN));
        _pManaPic->setPosition(Core::Vector2f(x, 50.0f));
    }
    else if(_state != WILL_PANEL_CHOICE_DONE)
    {
        float alpha = (lm.x / _pWillBack->getSize().x);
        int32 step = Core::clamp(1, _maxBid + 1, Core::L_round(alpha * 5.0f));

        _elapsed += float(elapsed);

        if(step != _step)
        {
            _w1 = _pWillBar->getProgress();
            _w2 = step / 5.0f;
            _elapsed = 0.0f;
            _step = step;
        }

        float t = std::min(1.0f, _elapsed / 0.25f);
        float w = Core::lerp(_w1, _w2, Core::interpolate(t, Core::INTER_FADE_OUT));
        _pWillBar->setProgress(w);

        int32 bid = _step - 1;

        _pWillLabel->setText(Core::toString(_totalWill - bid));

        _pBidLabel->setText(Core::toString(bid));
        _pBidLabel->setPosition(Core::Vector2f(w * _pWillBack->getSize().x - 15.0f, 13.0f));
    }
}

