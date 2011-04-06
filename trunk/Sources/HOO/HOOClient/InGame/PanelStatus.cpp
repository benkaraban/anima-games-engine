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
#include <HOOClient/InGame/PanelStatus.h>
#include <Core/Strings/Tools.h>

static const float APPEAR_TIME = 0.25f;
static const float DELTA_TIME = 3.0f;

PanelSphereStatus::PanelSphereStatus(const Ptr<WidgetFactory> & pFactory, const String & picName) : 
    _pFactory(pFactory),
    _elapsed(0.0f),
    _deltaHidden(true)
{
    Ptr<GUI::Widget> pPriorityBooster1(new GUI::Widget());

    _pLabelMana = pFactory->createMediumDigitsLabel(L"0", 40, 0);
    _pLabelMana->setHLayout(GUI::HL_CENTER);

    _pChaining = pFactory->createPicture(L"/HOO/GUI/Widgets/icon-chaining.tga", 15, 23);
    _pLabelChaining = pFactory->createSmallDigitsLabel(L"0", 25, 31);
    _pLabelChaining->setSize(Core::Vector2f(0.0f, 0.0f));
    _pLabelChaining->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 0.0f));
    _pLabelChaining->setHLayout(GUI::HL_CENTER);

    _pLabelDelta = pFactory->createSmallDigitsLabel(L"0", 40, -10);
    _pLabelDelta->setSize(Core::Vector2f(0.0f, 0.0f));
    _pLabelDelta->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pLabelDelta->setHLayout(GUI::HL_CENTER);

    addChild(_pLabelMana);
    addChild(_pChaining);
    addChild(_pLabelDelta);
    addChild(pPriorityBooster1);

    pPriorityBooster1->addChild(_pLabelChaining);
}

void PanelSphereStatus::initStatus(const HOOUserProtocol::PlayerSphereStatus & status)
{
    _status = status;
    setStatus(status);
}

void PanelSphereStatus::setStatus(const HOOUserProtocol::PlayerSphereStatus & status)
{
    // Mana

    _pLabelMana->setText(Core::toString(status.mana));

    if(status.mana > _status.mana)
    {
        _pLabelMana->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);

        _pLabelDelta->setText(L"+" + Core::toString(status.mana - _status.mana));
        _pLabelDelta->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        _pLabelDelta->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden = false;
    }
    else if(status.mana < _status.mana)
    {
        _pLabelMana->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);

        _pLabelDelta->setText(L"-" + Core::toString(_status.mana - status.mana));
        _pLabelDelta->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
        _pLabelDelta->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden = false;
    }

    // Chaining

    if(status.chaining > 0)
    {
        _pLabelChaining->setText(Core::toString(status.chaining));
        _pChaining->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
        _pLabelChaining->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
    }
    else
    {
        _pChaining->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
        _pLabelChaining->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
    }

    _status = status;
}

void PanelSphereStatus::update(double elapsed)
{
    Widget::update(elapsed);

    if(!_deltaHidden)
    {
        _elapsed += float(elapsed);

        if(_elapsed > DELTA_TIME)
        {
            _pLabelMana->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
            _pLabelDelta->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
            _deltaHidden = true;
        }
    }
}

PanelStatus::PanelStatus(const Ptr<WidgetFactory> & pFactory) :
    _pFactory(pFactory),
    _maxHealth1(1),
    _maxHealth2(1),
    _deltaHidden1(true),
    _deltaHidden2(true)
{
    Ptr<GUI::Widget> pPriorityBooster1(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster2(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster3(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster4(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster5(new GUI::Widget());
    Ptr<GUI::Widget> pPriorityBooster6(new GUI::Widget());

    //------------------------------------------------------------
    // Construction des monades
    //------------------------------------------------------------
    _pSphereMonad = _pFactory->createPicture(L"/HOO/GUI/Widgets/icon-spheres-circle-large.tga",20,20);
    _pSphereMonad2 = _pFactory->createPicture(L"/HOO/GUI/Widgets/icon-spheres-circle-large.tga",900,20);

    _pSphere[0] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-r.tga"));
    _pSphere[1] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-b.tga"));
    _pSphere[2] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-g.tga"));

    _pSphere2[0] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-r.tga"));
    _pSphere2[1] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-b.tga"));
    _pSphere2[2] = Ptr<PanelSphereStatus>(new PanelSphereStatus(pFactory, L"/HOO/GUI/Widgets/icon-mana-g.tga"));

    _pSphere[0]->setPosition(Core::Vector2f(33.0f, 60.0f));
    _pSphere[1]->setPosition(Core::Vector2f(52.0f, 95.0f));
    _pSphere[2]->setPosition(Core::Vector2f(15.0f, 95.0f));

    _pSphere2[0]->setPosition(Core::Vector2f(13.0f, 40.0f));
    _pSphere2[1]->setPosition(Core::Vector2f(32.0f, 75.0f));
    _pSphere2[2]->setPosition(Core::Vector2f(-5.0f, 75.0f));

    _pName1 = _pFactory->createInGameMediumLabel(L"Funny_ProsperLOADED", 168, 38);
    _pName2 = _pFactory->createInGameMediumLabel(L"Laocoon_Da_Boss", 662, 38);

    _pName1->setSize(Core::Vector2f(200.0f, 20.0f));
    _pName2->setSize(Core::Vector2f(200.0f, 20.0f));

    _pName1->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pName2->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);

    _pVantage1 = _pFactory->createPicture(L"/HOO/GUI/Widgets/vantage-left.tga", 398, 83);
    _pVantage2 = _pFactory->createPicture(L"/HOO/GUI/Widgets/vantage-right.tga", 554, 83);

    _pTimerPanel = _pFactory->createPicture(L"/HOO/GUI/Widgets/timebox.tga", 479, 32);
    _pTimerLabel = _pFactory->createBigDigitsLabel(L"", 0, 0);
    _pTimerLabel->setSize(_pTimerPanel->getSize());
    _pTimerLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);

    _pTimerPanel->addChild(_pTimerLabel);

    _pAbsorbIcon = _pFactory->createPicture(L"/HOO/GUI/Widgets/icon-status-absorb.tga", 240, 90);
    _pImpairIcon = _pFactory->createPicture(L"/HOO/GUI/Widgets/icon-status-impair.tga", 280, 90);
    _pAnticipateIcon = _pFactory->createPicture(L"/HOO/GUI/Widgets/icon-status-anticipation.tga", 320, 90);

    _pAbsorbLabel = _pFactory->createSmallDigitsLabel(L"0", 240, 90);
    _pImpairLabel = _pFactory->createSmallDigitsLabel(L"0", 280, 90);
    _pAnticipateLabel = _pFactory->createSmallDigitsLabel(L"0", 320, 90);

    _pAbsorbLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pImpairLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pAnticipateLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

    _pAbsorbIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pImpairIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pAnticipateIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

    _pAbsorbLabel->setSize(Core::Vector2f(32.0f, 32.0f));
    _pImpairLabel->setSize(Core::Vector2f(32.0f, 32.0f));
    _pAnticipateLabel->setSize(Core::Vector2f(32.0f, 32.0f));

    _pAbsorbLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pImpairLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pAnticipateLabel->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);

    _pHealthBar1 = _pFactory->createProgressBar(131, 54);
    _pHealthBar1->setOrient(GUI::PROGRESSBAR_RIGHT_TO_LEFT);

    _pHealthBar2 = _pFactory->createProgressBar(538, 54);
    _pHealthBar2->setOrient(GUI::PROGRESSBAR_LEFT_TO_RIGHT);

    _pHealthLabel1 = _pFactory->createSmallDigitsLabel(L"", 168, 88);
    _pHealthLabel2 = _pFactory->createSmallDigitsLabel(L"", 814, 88);

    _pHealthLabel1->setSize(Core::Vector2f(45.0f, 13.0f));
    _pHealthLabel2->setSize(Core::Vector2f(45.0f, 13.0f));

    _pHealthLabel1->setHLayout(GUI::HL_CENTER);
    _pHealthLabel2->setHLayout(GUI::HL_CENTER);
    _pHealthLabel1->setVLayout(GUI::VL_CENTER);
    _pHealthLabel2->setVLayout(GUI::VL_CENTER);

    _pHealthDelta1 = _pFactory->createMediumDigitsLabel(L"", 168, 108);
    _pHealthDelta2 = _pFactory->createMediumDigitsLabel(L"", 814, 108);

    _pHealthDelta1->setSize(Core::Vector2f(45.0f, 13.0f));
    _pHealthDelta2->setSize(Core::Vector2f(45.0f, 13.0f));

    _pHealthDelta1->setHLayout(GUI::HL_CENTER);
    _pHealthDelta2->setHLayout(GUI::HL_CENTER);
    _pHealthDelta1->setVLayout(GUI::VL_CENTER);
    _pHealthDelta2->setVLayout(GUI::VL_CENTER);

    _pVantage1->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
    _pVantage2->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));

    addChild(_pSphereMonad);
    addChild(_pVantage1);
    addChild(_pVantage2);
    addChild(pPriorityBooster1);

    pPriorityBooster1->addChild(pPriorityBooster2);
    pPriorityBooster2->addChild(pPriorityBooster3);

    pPriorityBooster3->setDeltaPriority(1000);
    pPriorityBooster3->addChild(_pHealthBar1);
    pPriorityBooster3->addChild(_pHealthBar2);
    pPriorityBooster3->addChild(pPriorityBooster4);

    pPriorityBooster4->setDeltaPriority(1000);
    pPriorityBooster4->addChild(_pTimerPanel);
    pPriorityBooster4->addChild(pPriorityBooster5);

    pPriorityBooster5->addChild(_pSphere[0]);
    pPriorityBooster5->addChild(_pSphere[1]);
    pPriorityBooster5->addChild(_pSphere[2]);

    pPriorityBooster5->addChild(_pName1);
    pPriorityBooster5->addChild(_pName2);
    pPriorityBooster5->addChild(_pHealthLabel1);
    pPriorityBooster5->addChild(_pHealthLabel2);
    pPriorityBooster5->addChild(_pHealthDelta1);
    pPriorityBooster5->addChild(_pHealthDelta2);
    pPriorityBooster4->addChild(_pAbsorbIcon);
    pPriorityBooster4->addChild(_pImpairIcon);
    pPriorityBooster4->addChild(_pAnticipateIcon);
    pPriorityBooster5->addChild(pPriorityBooster6);

    pPriorityBooster6->addChild(_pAbsorbLabel);
    pPriorityBooster6->addChild(_pImpairLabel);
    pPriorityBooster6->addChild(_pAnticipateLabel);

    /* opponent status */
    addChild(_pSphereMonad2);
    _pSphereMonad2->addChild(_pSphere2[0]);
    _pSphereMonad2->addChild(_pSphere2[1]);
    _pSphereMonad2->addChild(_pSphere2[2]);

    setOwnColor(Core::Vector4f(0.0f));
}

void PanelStatus::initStatus(const HOOUserProtocol::GameStatus & status, const String & p1, const String & p2)
{
    _pName1->setText(p1);
    _pName2->setText(p2);
    _status = status;

    for(int32 ii=0; ii < 3; ii++)
        _pSphere[ii]->initStatus(status.player.spheres[ii]);

    setStatus(status);
}

void PanelStatus::setStatus(const HOOUserProtocol::GameStatus & status)
{
    _maxHealth1 = std::max(_maxHealth1, status.player.health);
    _maxHealth2 = std::max(_maxHealth2, status.opponent.health);

    _pHealthLabel1->setText(Core::toString(status.player.health) + L"/" + Core::toString(_maxHealth1));
    _pHealthLabel2->setText(Core::toString(status.opponent.health) + L"/" + Core::toString(_maxHealth2));

    _pHealthBar1->setProgress(float(status.player.health) / float(_maxHealth1));
    _pHealthBar2->setProgress(float(status.opponent.health) / float(_maxHealth2));

    for(int32 ii=0; ii < 3; ii++)
    {
        _pSphere[ii]->setStatus(status.player.spheres[ii]);
        _pSphere2[ii]->setStatus(status.opponent.spheres[ii]);
    }

    int32 deltaHealth1 = status.player.health - _status.player.health;
    int32 deltaHealth2 = status.opponent.health - _status.opponent.health;

    if(deltaHealth1 > 0)
    {
        _pHealthDelta1->setText(L"+" + Core::toString(deltaHealth1));
        _pHealthDelta1->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        _pHealthDelta1->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
        _pHealthLabel1->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden1 = false;
    }
    else if(deltaHealth1 < 0)
    {
        _pHealthDelta1->setText(L"-" + Core::toString(-deltaHealth1));
        _pHealthDelta1->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.0f));
        _pHealthDelta1->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _pHealthLabel1->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden1 = false;
    }

    if(deltaHealth2 > 0)
    {
        _pHealthDelta2->setText(L"+" + Core::toString(deltaHealth2));
        _pHealthDelta2->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        _pHealthDelta2->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
        _pHealthLabel2->setOwnColor(Core::Vector4f(0.0f, 1.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden2 = false;
    }
    else if(deltaHealth2 < 0)
    {
        _pHealthDelta2->setText(L"-" + Core::toString(-deltaHealth2));
        _pHealthDelta2->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f));
        _pHealthDelta2->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _pHealthLabel2->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _elapsed = 0.0f;
        _deltaHidden2 = false;
    }

    if(status.player.hasVantage)
        _pVantage1->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
    else
        _pVantage1->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);

    if(status.opponent.hasVantage)
        _pVantage2->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
    else
        _pVantage2->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);

    _pAbsorbLabel->setText(Core::toString(status.player.spheres[0].absorb));
    _pImpairLabel->setText(Core::toString(status.player.spheres[0].impair));
    _pAnticipateLabel->setText(Core::toString(status.player.nextTurnsToSee));

    if(status.player.spheres[0].absorb > 0)
    {
        _pAbsorbLabel->setOwnColor(Core::Vector4f(0.7f, 1.0f, 0.7f, 1.0f), APPEAR_TIME);
        _pAbsorbIcon->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
    }
    else
    {
        _pAbsorbLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
        _pAbsorbIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
    }

    if(status.player.spheres[0].impair > 0)
    {
        _pImpairLabel->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 1.0f), APPEAR_TIME);
        _pImpairIcon->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
    }
    else
    {
        _pImpairLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
        _pImpairIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
    }

    if(status.player.nextTurnsToSee > 0)
    {
        _pAnticipateLabel->setOwnColor(Core::Vector4f(0.7f, 0.7f, 1.0f, 1.0f), APPEAR_TIME);
        _pAnticipateIcon->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
    }
    else
    {
        _pAnticipateLabel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
        _pAnticipateIcon->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
    }


    _status = status;
}

void PanelStatus::update(double elapsed)
{
    Widget::update(elapsed);

    if(!_deltaHidden1 || !_deltaHidden2)
    {
        _elapsed += float(elapsed);

        if(_elapsed > DELTA_TIME)
        {
            if(!_deltaHidden1)
            {
                _pHealthLabel1->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
                _pHealthDelta1->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
                _deltaHidden1 = true;
            }

            if(!_deltaHidden2)
            {
                _pHealthLabel2->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), APPEAR_TIME);
                _pHealthDelta2->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), APPEAR_TIME);
                _deltaHidden2 = true;
            }
        }
    }
}