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
#include "ScreenHelp.h"

namespace ChronoRage
{

ScreenHelp::ScreenHelp(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();
    _pPanel = _appContext.pWidgetFactory->createLargePanel();

    _pPicKey1 = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-dirs.tga", 80, 220);
    _pPicKey2 = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-dirs2.tga", 80, 340);
    _pPicMouse = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-mouse.tga", 70, 380);
    _pPicScreen = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-screen.tga", 555, 140);
    _pPicCarrier = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-carrier.tga", 385, 200);
    _pPicBonus = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-bonus.tga", 400, 370);
    _pPicPad = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/GUI/instructions-pad.tga", 50, 240);
    _pButtonNext   = _appContext.pWidgetFactory->createMediumButton(L"continue", 500 - 368 / 2, 540);

    _pLabelUp = createKeyLabel(L"", 50, 5);
    _pLabelDown = createKeyLabel(L"", 50, 30);
    _pLabelLeft = createKeyLabel(L"", 50, 55);
    _pLabelRight = createKeyLabel(L"", 50, 80);

    _pLabelAimUp = createKeyLabel(L"", 50, 5);
    _pLabelAimDown = createKeyLabel(L"", 50, 30);
    _pLabelAimLeft = createKeyLabel(L"", 50, 55);
    _pLabelAimRight = createKeyLabel(L"", 50, 80);
    _pLabelKeyTimeWarp = createKeyLabel(L"", 0, 115);
    _pLabelKeyFire = createKeyLabel(L"", 0, 140);

    _pLabelPadMove = createKeyLabel(L"", 55, 35);
    _pLabelPadAim = createKeyLabel(L"", 145, 70);
    _pLabelPadTimeWarp = createKeyLabel(L"", 50, 185);
    _pLabelPadFire = createKeyLabel(L"", 50, 210);

    _pLabelPadMove->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    _pLabelPadAim->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);

    _pLabelLMB = createKeyLabel(L"", 60, -5);
    _pLabelRMB = createKeyLabel(L"", 60, 20);

    _pLabelUp->setDictionary(null);
    _pLabelDown->setDictionary(null);
    _pLabelLeft->setDictionary(null);
    _pLabelRight->setDictionary(null);
    _pLabelAimUp->setDictionary(null);
    _pLabelAimDown->setDictionary(null);
    _pLabelAimLeft->setDictionary(null);
    _pLabelAimRight->setDictionary(null);
    _pLabelKeyTimeWarp->setDictionary(null);
    _pLabelKeyFire->setDictionary(null);
    _pLabelPadTimeWarp->setDictionary(null);
    _pLabelPadFire->setDictionary(null);

    _pPicKey1->addChild(_pLabelUp);
    _pPicKey1->addChild(_pLabelDown);
    _pPicKey1->addChild(_pLabelLeft);
    _pPicKey1->addChild(_pLabelRight);

    _pPicKey2->addChild(_pLabelAimUp);
    _pPicKey2->addChild(_pLabelAimDown);
    _pPicKey2->addChild(_pLabelAimLeft);
    _pPicKey2->addChild(_pLabelAimRight);
    _pPicKey2->addChild(_pLabelKeyTimeWarp);
    _pPicKey2->addChild(_pLabelKeyFire);

    _pPicPad->addChild(_pLabelPadMove);
    _pPicPad->addChild(_pLabelPadAim);
    _pPicPad->addChild(_pLabelPadTimeWarp);
    _pPicPad->addChild(_pLabelPadFire);

    _pPicMouse->addChild(_pLabelLMB);
    _pPicMouse->addChild(_pLabelRMB);
#ifndef CHRONORAGE_DEMO
    _pLabelDestroy = createLabel(L"objectives", 0, 60);
    _pLabelDestroyHint = createHint(L"objectives-hint", 400, 80);
#else
    _pLabelDestroy = createLabel(L"objectives-demo", 0, 60);
    _pLabelDestroyHint = createHint(L"", 400, 80);
#endif

    _pLabelDestroy->setFontSize(18);
    _pLabelDestroy->setSize(Core::Vector2f(1000.0f, 0.0f));
    _pLabelDestroyHint->setSize(Core::Vector2f(200.0f, 0.0f));

    _pLabelCarrier = createLabel(L"carrier", int32(0.5f * _pPicCarrier->getSize().x), 100);
    _pLabelCarrierHint = createHint(L"carrier-hint", int32(0.5f * _pPicCarrier->getSize().x), 120);

    _pLabelBonus = createLabel(L"bonus", int32(0.5f * _pPicBonus->getSize().x), 70);
    _pLabelBonusHint1 = createHint(L"bonus-hint", int32(0.5f * _pPicBonus->getSize().x), 90);

    _pLabelRemaining = createLabel(L"remaining-time", 190, 5);
    _pLabelRemainingHint = createHint(L"remaining-time-hint", 190, 20);

    _pLabelFirepower = createLabel(L"firepower", 190, 310);
    _pLabelFirepowerHint = createHint(L"firepower-hint", 190, 325);

    _pLabelTimeWarps = createLabel(L"time-warp", 190, 150);
    _pLabelTimeWarpsHint = createHint(L"time-warp-hint", 190, 165);

    _pLabelControls = createLabel(L"controls", 190, 150);
    _pLabelControlsHint = createHint(L"controls-hint", 190, 165);

    _pPicCarrier->addChild(_pLabelCarrier);
    _pPicCarrier->addChild(_pLabelCarrierHint);

    _pPicBonus->addChild(_pLabelBonus);
    _pPicBonus->addChild(_pLabelBonusHint1);

    _pPicScreen->addChild(_pLabelRemaining);
    _pPicScreen->addChild(_pLabelRemainingHint);
    _pPicScreen->addChild(_pLabelFirepower);
    _pPicScreen->addChild(_pLabelFirepowerHint);
    _pPicScreen->addChild(_pLabelTimeWarps);
    _pPicScreen->addChild(_pLabelTimeWarpsHint);

    _pPanel->addChild(_pLabelDestroy);
    _pPanel->addChild(_pLabelDestroyHint);
    _pPanel->addChild(_pPicKey1);
    _pPanel->addChild(_pPicKey2);
    _pPanel->addChild(_pPicMouse);
    _pPanel->addChild(_pPicPad);
    _pPanel->addChild(_pPicScreen);
    _pPanel->addChild(_pPicCarrier);
    _pPanel->addChild(_pPicBonus);
    _pPanel->addChild(_pLabelControls);
    _pPanel->addChild(_pLabelControlsHint);
    _pPanel->addChild(_pButtonNext);

    _pLastKeyFocus = _pButtonNext.get();

    _pButtonNext->addEventHandler(this);
    _pButtonNext->setHotKey(VK_ESCAPE);
    
    _pDialog->addChild(_pPanel);
}

Ptr<GUI::Label> ScreenHelp::createKeyLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_LEFT, GUI::VL_UP);
    pResult->setBold(false);
    pResult->setFontSize(12);
    return pResult;
}

Ptr<GUI::Label> ScreenHelp::createLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    pResult->setBold(false);
    pResult->setFontSize(14);
    return pResult;
}

Ptr<GUI::Label> ScreenHelp::createHint(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    pResult->setBold(false);
    pResult->setFontSize(8);
    return pResult;
}

void ScreenHelp::resetLabels()
{
    _pButtonNext->setDictionary(_appContext.pDico);
    _pLabelDestroy->setDictionary(_appContext.pDico);
    _pLabelDestroyHint->setDictionary(_appContext.pDico);
    _pLabelCarrier->setDictionary(_appContext.pDico);
    _pLabelCarrierHint->setDictionary(_appContext.pDico);
    _pLabelBonus->setDictionary(_appContext.pDico);
    _pLabelBonusHint1->setDictionary(_appContext.pDico);
    _pLabelRemaining->setDictionary(_appContext.pDico);
    _pLabelRemainingHint->setDictionary(_appContext.pDico);
    _pLabelFirepower->setDictionary(_appContext.pDico);
    _pLabelFirepowerHint->setDictionary(_appContext.pDico);
    _pLabelTimeWarps->setDictionary(_appContext.pDico);
    _pLabelTimeWarpsHint->setDictionary(_appContext.pDico);
    _pLabelControls->setDictionary(_appContext.pDico);
    _pLabelControlsHint->setDictionary(_appContext.pDico);

    if(_pScreenNext != null)
        _pButtonNext->setText(L"back");
    else
        _pButtonNext->setText(L"continue");
#ifndef CHRONORAGE_DEMO
    _pLabelDestroy->setText(L"objectives");
    _pLabelDestroyHint->setText(L"objectives-hint");
#else
    _pLabelDestroy->setText(L"objectives-demo");
    _pLabelDestroyHint->setText(L"");
#endif
    _pLabelCarrier->setText(L"carrier");
    _pLabelCarrierHint->setText(L"carrier-hint");
    _pLabelBonus->setText(L"bonus");
    _pLabelBonusHint1->setText(L"bonus-hint");
    _pLabelRemaining->setText(L"remaining-time");
    _pLabelRemainingHint->setText(L"remaining-time-hint");
    _pLabelFirepower->setText(L"firepower");
    _pLabelFirepowerHint->setText(L"firepower-hint");
    _pLabelTimeWarps->setText(L"time-warp");
    _pLabelTimeWarpsHint->setText(L"time-warp-hint");
    _pLabelControls->setText(L"controls");
    _pLabelControlsHint->setText(L"controls-hint");
}


void ScreenHelp::startScreen()
{
    GameSettings & settings = *_appContext.pGameSettings;

    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    EMouseButton mbShoot, mbTimeWarp;

    settings.cleanupSettings(*_appContext.pInputManager);
    settings.getShootingMouseControls(mbShoot, mbTimeWarp);

    {
        int32 up, down, left, right;
        settings.getMovementKeyboardControls(up, down, left, right);

        _pLabelUp->setText(Core::System::getKeyCodeName(up));
        _pLabelDown->setText(Core::System::getKeyCodeName(down));
        _pLabelLeft->setText(Core::System::getKeyCodeName(left));
        _pLabelRight->setText(Core::System::getKeyCodeName(right));
    }
    {
        int32 up, down, left, right, timeWarp, fire;
        settings.getShootingKeyboardControls(up, down, left, right, timeWarp, fire);
        String strFire;

        if(settings.getKeyboardAutoFire())
            strFire << (*_appContext.pDico)[L"hold-fire"];
        else
            strFire << (*_appContext.pDico)[L"fire"];

        _pLabelAimUp->setText(Core::System::getKeyCodeName(up));
        _pLabelAimDown->setText(Core::System::getKeyCodeName(down));
        _pLabelAimLeft->setText(Core::System::getKeyCodeName(left));
        _pLabelAimRight->setText(Core::System::getKeyCodeName(right));
        _pLabelKeyTimeWarp->setText((*_appContext.pDico)[L"time-warp"] + L" : " + Core::System::getKeyCodeName(timeWarp));
        _pLabelKeyFire->setText(strFire + L" : " + Core::System::getKeyCodeName(fire));
    }

    if(settings.getSelectedControls() == SC_JOYSTICK)
    {
        int32 iJoy = settings.getSelectedJoystick();
        Ptr<Window::IJoystick> pJoy = _appContext.pInputManager->getJoysticks()[iJoy];
        JoySettings joy = settings.getJoySettings(pJoy->getName());

        String strFire;

        if(joy.autofire)
            strFire << (*_appContext.pDico)[L"hold-fire"];
        else
            strFire << (*_appContext.pDico)[L"fire"];

        if(joy.inverseSticks)
        {
            _pLabelPadAim->setText(L"movement");
            _pLabelPadMove->setText(L"aim");
        }
        else
        {
            _pLabelPadMove->setText(L"movement");
            _pLabelPadAim->setText(L"aim");
        }

        _pLabelPadTimeWarp->setText((*_appContext.pDico)[L"time-warp"] + L" : " + pJoy->getButtonName(joy.timeWarpButton));
        _pLabelPadFire->setText(strFire + L" : " + pJoy->getButtonName(joy.fireButton));
    }

    if(MOUSE_LEFT_B == mbShoot)
        if(settings.getMouseAutoFire())
            _pLabelLMB->setText(L"hold-fire");
        else
            _pLabelLMB->setText(L"fire");
    else if(MOUSE_LEFT_B == mbTimeWarp)
        _pLabelLMB->setText(L"time-warp");
    else
        _pLabelLMB->setText(L"");

    if(MOUSE_RIGHT_B == mbShoot)
        if(settings.getMouseAutoFire())
            _pLabelRMB->setText(L"hold-fire");
        else
            _pLabelRMB->setText(L"fire");
    else if(MOUSE_RIGHT_B == mbTimeWarp)
        _pLabelRMB->setText(L"time-warp");
    else
        _pLabelRMB->setText(L"");

    switch(settings.getSelectedControls())
    {
    case SC_MOUSE_KEYBOARD:
        _pPicKey1->setOwnColor(Core::Vector4f(1.0f));
        _pPicKey2->setOwnColor(Core::Vector4f(0.0f));
        _pPicMouse->setOwnColor(Core::Vector4f(1.0f));
        _pPicPad->setOwnColor(Core::Vector4f(0.0f));
        break;
    case SC_KEYBOARD_ONLY:
        _pPicKey1->setOwnColor(Core::Vector4f(1.0f));
        _pPicKey2->setOwnColor(Core::Vector4f(1.0f));
        _pPicMouse->setOwnColor(Core::Vector4f(0.0f));
        _pPicPad->setOwnColor(Core::Vector4f(0.0f));
        break;
    case SC_JOYSTICK:
        _pPicKey1->setOwnColor(Core::Vector4f(0.0f));
        _pPicKey2->setOwnColor(Core::Vector4f(0.0f));
        _pPicMouse->setOwnColor(Core::Vector4f(0.0f));
        _pPicPad->setOwnColor(Core::Vector4f(1.0f));
        break;
    }

    if(_pScreenNext != null)
        _pButtonNext->setText(L"back");
    else
        _pButtonNext->setText(L"continue");

    _pDialog->pack();
}
    
void ScreenHelp::endScreen()
{
}

void ScreenHelp::update(double elapsed)
{
}

void ScreenHelp::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonNext.get())
        {
            _pLastKeyFocus = _pButtonNext.get();

            if(_pScreenNext != null)
            {
                _pManager->setScreen(_pScreenNext.get());
            }
            else
            {
#ifndef CHRONORAGE_DEMO
                _appContext.wantedMode = NEW_GAME_MODE;
                _appContext.wantedLevel = 1;
#else
                _appContext.wantedMode = DEMO_MODE;
                _appContext.wantedLevel = 1;
#endif
            }
        }
    }
}

}//namespace ChronoRage
