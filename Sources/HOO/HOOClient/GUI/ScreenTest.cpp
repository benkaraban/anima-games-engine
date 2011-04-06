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

#include <HOOClient/GUI/ScreenTest.h>
#include <HOOClient/SpellPanel.h>

ScreenTest::ScreenTest(AppContext & appContext) : _appContext(appContext)
{
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = Ptr<GUI::Widget>(new GUI::Widget());
    _pDialogCenter = pWidgets->createDialogBase();
    _pDialogUp = pWidgets->createDialogBaseTop(768);
    _pDialogDown = pWidgets->createDialogBaseBottom(200);

    {
        HOOUserProtocol::ActionCommand command;
        command.actionType = 1;
        
        Core::List<HOOUserProtocol::ActionCommand> commands;
        commands.push_back(command);

        command.actionValue = 0; commands.push_back(command);
        command.actionValue = 1; commands.push_back(command);
        command.actionValue = 2; commands.push_back(command);
        command.actionValue = 3; commands.push_back(command);
        command.actionValue = 4; commands.push_back(command);

        _pWill = Ptr<PanelWillContest>(new PanelWillContest(pWidgets));
        _pWill->pack();
        _pWill->setOwnColor(Core::Vector4f(1.0f));
        _pWill->setTotalWill(20);
        _pWill->setContest(4, 2, commands);
    }

    _pDialogCenter->addChild(_pWill);
    _pDialog->addChild(_pDialogCenter);
    _pDialog->addChild(_pDialogUp);
    _pDialog->addChild(_pDialogDown);   

    HOOUserProtocol::ActionCommand command;
    command.actionType = 2;

    Core::List<HOOUserProtocol::ActionCommand> commands;
    commands.push_back(command);

    command.actionType = 0;
    command.actionValue = 1001; commands.push_back(command);
    command.actionValue = 1002; commands.push_back(command);
    command.actionValue = 1004; commands.push_back(command);

    HOOUserProtocol::SpellCooldown cooldown;
    cooldown.spellId = 1003;
    cooldown.cooldown = 4;

    Core::List<HOOUserProtocol::SpellCooldown> cooldowns;
    cooldowns.push_back(cooldown);

    //_pPanel1 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1001), *pWidgets));
    //_pPanel2 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1002), *pWidgets));
    //_pPanel3 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1003), *pWidgets));
    //_pPanel4 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1004), *pWidgets));
    //_pPanel5 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1005), *pWidgets));
    //_pPanel6 = Ptr<GUI::Widget>(new SpellPanel(*_appContext.pGameLib->getSpell(1006), *pWidgets));

    Core::List<int32> deck;
    deck.push_back(1001);
    deck.push_back(1002);
    deck.push_back(1003);
    deck.push_back(1004);
    deck.push_back(1005);
    deck.push_back(1006);

    _pPanel1 = Ptr<PanelSpellsBar>(new PanelSpellsBar(pWidgets, _appContext.pGameLib, _pDialogCenter, deck));
    _pPanel1->setPosition(Core::Vector2f(500.0f, 130.0f));
    _pPanel1->setAvailableActions(commands, true);

    _pStatus = Ptr<PanelStatus>(new PanelStatus(pWidgets));

    //_pSphere1 = Ptr<PanelSphereStatus>(new PanelSphereStatus(pWidgets, L"/HOO/GUI/Widgets/icon-mana-b.tga"));
    //_pSphere1->setPosition(Core::Vector2f(100.0f, 100.0f));

    HOOUserProtocol::GameStatus s;
    s.player.willpower = 20;
    s.player.health = 100;
    s.opponent.health = 100;
    s.player.hasVantage = false;
    s.player.hasVantage = false;
    s.player.nextTurnsToSee = 0;
    s.player.cooldowns = cooldowns;
    s.player.spheres[0].mana = 5;
    s.player.spheres[0].chaining = 0;
    s.player.spheres[0].impair = 0;
    s.player.spheres[1].mana = 5;
    s.player.spheres[1].chaining = 0;
    s.player.spheres[1].impair = 0;
    s.player.spheres[2].mana = 5;
    s.player.spheres[2].chaining = 0;
    s.player.spheres[2].impair = 0;

    _pStatus->initStatus(s, L"ProsperLOADED", L"[LOADED] KarenCheryl");
    _pPanel1->setStatus(s.player);

    //HOOUserProtocol::PlayerSphereStatus s;
    //s.mana = 5;
    //s.chaining = 0;
    //s.impair = 0;

    //_pSphere1->initStatus(s);

    _pDialogDown->addChild(_pPanel1);
    _pDialogUp->addChild(_pStatus);
}

void ScreenTest::startScreen()
{
    _appContext.pGUI->setDialog(_pDialog);
    _pDialog->pack();
}
    
void ScreenTest::endScreen()
{
}

void ScreenTest::update(double elapsed)
{
    HOOUserProtocol::SpellCooldown cooldown;
    cooldown.spellId = 1003;
    cooldown.cooldown = 4;

    Core::List<HOOUserProtocol::SpellCooldown> cooldowns;
    cooldowns.push_back(cooldown);

    HOOUserProtocol::GameStatus s;
    s.player.willpower = 20;
    s.player.health = 100;
    s.opponent.health = 100;
    s.player.hasVantage = false;
    s.player.hasVantage = false;
    s.player.nextTurnsToSee = 0;
    s.player.cooldowns = cooldowns;
    s.player.spheres[0].mana = 5;
    s.player.spheres[0].chaining = 0;
    s.player.spheres[0].impair = 0;
    s.player.spheres[1].mana = 5;
    s.player.spheres[1].chaining = 0;
    s.player.spheres[1].impair = 0;
    s.player.spheres[2].mana = 5;
    s.player.spheres[2].chaining = 0;
    s.player.spheres[2].impair = 0;

    if(_appContext.pInput->isKeyTyped(VK_F1))
    {
        _pStatus->setStatus(s);
        _pPanel1->setStatus(s.player);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F2))
    {
        s.player.willpower = 18;
        s.player.spheres[0].mana = 12;
        s.player.spheres[0].chaining = 2;

        _pStatus->setStatus(s);
        _pPanel1->setStatus(s.player);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F3))
    {
        s.player.hasVantage = true;
        s.opponent.health = 80;
        s.player.spheres[0].mana = 1;
        s.player.spheres[0].chaining = 1;

        _pStatus->setStatus(s);
        _pPanel1->setStatus(s.player);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F4))
    {
        s.player.health = 70;
        s.opponent.health = 80;
        s.player.spheres[1].mana = 7;
        s.player.spheres[0].impair = 2;

        _pStatus->setStatus(s);
        _pPanel1->setStatus(s.player);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F5))
    {
        s.player.health = 70;
        s.opponent.health = 80;
        s.player.spheres[0].mana = 0;
        s.player.spheres[0].chaining = 2;
        s.player.spheres[0].absorb = 3;
        s.player.nextTurnsToSee = 2;

        _pStatus->setStatus(s);
        _pPanel1->setStatus(s.player);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F6))
    {
        HOOUserProtocol::ActionCommand command;
        command.actionType = 2;

        Core::List<HOOUserProtocol::ActionCommand> commands;
        commands.push_back(command);

        command.actionType = 0;
        command.actionValue = 1001; commands.push_back(command);
        command.actionValue = 1002; commands.push_back(command);
        command.actionValue = 1004; commands.push_back(command);

        _pPanel1->setAvailableActions(commands, true);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F7))
    {
        HOOUserProtocol::ActionCommand command;
        command.actionType = 2;

        Core::List<HOOUserProtocol::ActionCommand> commands;
        commands.push_back(command);

        command.actionType = 0;
        command.actionValue = 1001; commands.push_back(command);
        command.actionValue = 1002; commands.push_back(command);
        command.actionValue = 1004; commands.push_back(command);

        _pPanel1->setAvailableActions(commands, false);
        {
            HOOUserProtocol::ActionCommand command;
            command.actionType = 1;
            
            Core::List<HOOUserProtocol::ActionCommand> commands;
            commands.push_back(command);

            command.actionValue = 0; commands.push_back(command);
            command.actionValue = 1; commands.push_back(command);
            command.actionValue = 2; commands.push_back(command);
            command.actionValue = 3; commands.push_back(command);

            _pWill->setTotalWill(3);
            _pWill->setContest(4, 2, commands);
        }
    }
    else if(_appContext.pInput->isKeyTyped(VK_F8))
    {
        _pWill->resolve(true);
    }
    else if(_appContext.pInput->isKeyTyped(VK_F9))
    {
        _pWill->resolve(false);
    }
}

void ScreenTest::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
}
