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
#include "ScreenMouseKeyboardSettings.h"

namespace ChronoRage
{

ScreenMouseKeyboardSettings::ScreenMouseKeyboardSettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButton = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    static const int32 Y_PADDING = 53;
    static const int32 X_COL_1 = 24;
    static const int32 X_COL_2 = 245;

    _pLabelMovement = _appContext.pWidgetFactory->createMediumLabel(L"movement", 0, 20);
    Core::Vector2f panelSize = _pPanelButton->getSize();
    panelSize.y = 110.0f;
    _pLabelMovement->setSize(panelSize);
    _pLabelMovement->setHLayout(GUI::HL_CENTER);

    int padding = 2;
    _pLabelUp       = _appContext.pWidgetFactory->createMediumLabel(L"up", X_COL_1, padding * Y_PADDING);
    _pButtonUp      = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelDown     = _appContext.pWidgetFactory->createMediumLabel(L"down", X_COL_1, padding * Y_PADDING);
    _pButtonDown    = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelLeft     = _appContext.pWidgetFactory->createMediumLabel(L"left", X_COL_1, padding * Y_PADDING);
    _pButtonLeft    = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelRight    = _appContext.pWidgetFactory->createMediumLabel(L"right", X_COL_1, padding * Y_PADDING);
    _pButtonRight   = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);

    _pButtonUp->setDictionary(null);
    _pButtonDown->setDictionary(null);
    _pButtonLeft->setDictionary(null);
    _pButtonRight->setDictionary(null);

    _pButtonUp->addEventHandler(this);
    _pButtonDown->addEventHandler(this);
    _pButtonLeft->addEventHandler(this);
    _pButtonRight->addEventHandler(this);

    _pLabelAim = _appContext.pWidgetFactory->createMediumLabel(L"fire", 0,  padding++ * Y_PADDING);
    panelSize.y = 40;
    _pLabelAim->setSize(panelSize);
    _pLabelAim->setHLayout(GUI::HL_CENTER);

    _pLabelShoot    = _appContext.pWidgetFactory->createMediumLabel(L"fire", X_COL_1, padding * Y_PADDING);
    _pButtonShoot   = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelTimeWarp = _appContext.pWidgetFactory->createMediumLabel(L"time-warp", X_COL_1, padding * Y_PADDING);
    _pButtonTimeWarp= _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pAutoShoot     = _appContext.pWidgetFactory->createCheckBox(L"autofire", X_COL_1, padding++ * Y_PADDING);
    _pAutoShoot->setChecked(false);

    _pButtonShoot->addEventHandler(this);
    _pButtonTimeWarp->addEventHandler(this);
    _pAutoShoot->addEventHandler(this);

    _pButtonShoot->setDictionary(null);
    _pButtonShoot->setText(L"Left Button");
    _pButtonTimeWarp->setDictionary(null);
    _pButtonTimeWarp->setText(L"Right Button");

    _pPanelButton->addChild(_pLabelMovement);

    _pPanelButton->addChild(_pLabelUp);
    _pPanelButton->addChild(_pButtonUp);
    _pPanelButton->addChild(_pLabelDown);
    _pPanelButton->addChild(_pButtonDown);
    _pPanelButton->addChild(_pLabelLeft);
    _pPanelButton->addChild(_pButtonLeft);
    _pPanelButton->addChild(_pLabelRight);
    _pPanelButton->addChild(_pButtonRight);

    _pPanelButton->addChild(_pLabelAim);
    _pPanelButton->addChild(_pLabelShoot);
    _pPanelButton->addChild(_pButtonShoot);
    _pPanelButton->addChild(_pLabelTimeWarp);
    _pPanelButton->addChild(_pButtonTimeWarp);
    _pPanelButton->addChild(_pAutoShoot);

    _pButtonDefault = _appContext.pWidgetFactory->createSmallButton(L"default", X_COL_1, padding * Y_PADDING);
    _pButtonDefault->addEventHandler(this);

    _pButtonBack = _appContext.pWidgetFactory->createSmallButton(L"back", X_COL_2, padding * Y_PADDING);
    _pButtonBack->setHotKey(VK_ESCAPE);
    _pButtonBack->addEventHandler(this);

    _pPanelButton->addChild(_pButtonBack);
    _pPanelButton->addChild(_pButtonDefault);
    _pPanelButton->pack();

    _appContext.pWidgetFactory->bottomScreen(_pPanelButton);

    _pDialog->addChild(_pPanelButton);

    // Key nav
    _pLastKeyFocus = _pButtonBack.get();
    // end key nav
}

void ScreenMouseKeyboardSettings::resetLabels()
{
    _pLabelMovement->setDictionary(_appContext.pDico);
    _pLabelUp->setDictionary(_appContext.pDico);
    _pLabelDown->setDictionary(_appContext.pDico);
    _pLabelLeft->setDictionary(_appContext.pDico);
    _pLabelRight->setDictionary(_appContext.pDico);
    _pLabelAim->setDictionary(_appContext.pDico);
    _pLabelShoot->setDictionary(_appContext.pDico);
    _pLabelTimeWarp->setDictionary(_appContext.pDico);
    _pAutoShoot->setDictionary(_appContext.pDico);
    _pButtonDefault->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelMovement->setText(L"movement");
    _pLabelUp->setText(L"up");
    _pLabelDown->setText(L"down");
    _pLabelLeft->setText(L"left");
    _pLabelRight->setText(L"right");
    _pLabelAim->setText(L"fire");
    _pLabelShoot->setText(L"fire");
    _pLabelTimeWarp->setText(L"time-warp");
    _pAutoShoot->setText(L"autofire");
    _pButtonDefault->setText(L"default");
    _pButtonBack->setText(L"back");

    _pButtonShoot->setText(L"Left Button");
    _pButtonTimeWarp->setText(L"Right Button");
}

void ScreenMouseKeyboardSettings::startScreen()
{
    // Key nav
    _pButtonUp->setKeyWidgetU(_pButtonBack.get());
    _pButtonUp->setKeyWidgetD(_pButtonDown.get());

    _pButtonDown->setKeyWidgetU(_pButtonUp.get());
    _pButtonDown->setKeyWidgetD(_pButtonLeft.get());

    _pButtonLeft->setKeyWidgetU(_pButtonDown.get());
    _pButtonLeft->setKeyWidgetD(_pButtonRight.get());

    _pButtonRight->setKeyWidgetU(_pButtonLeft.get());
    _pButtonRight->setKeyWidgetD(_pButtonShoot.get());

    _pButtonShoot->setKeyWidgetU(_pButtonRight.get());
    _pButtonShoot->setKeyWidgetD(_pButtonTimeWarp.get());

    _pButtonTimeWarp->setKeyWidgetU(_pButtonShoot.get());
    _pButtonTimeWarp->setKeyWidgetD(_pAutoShoot.get());

    _pAutoShoot->setKeyWidgetU(_pButtonTimeWarp.get());
    _pAutoShoot->setKeyWidgetD(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pAutoShoot.get());
    _pButtonBack->setKeyWidgetL(_pButtonDefault.get());
    _pButtonBack->setKeyWidgetD(_pButtonUp.get());

    _pButtonDefault->setKeyWidgetU(_pAutoShoot.get());
    _pButtonDefault->setKeyWidgetR(_pButtonBack.get());
    // end key nav

    fillComponentsFromSettings();
    _pScreenSelectInput->setControlsSettingsScreen(this);
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenMouseKeyboardSettings::endScreen()
{
}

void ScreenMouseKeyboardSettings::update(double elapsed)
{
}

void ScreenMouseKeyboardSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenControlsSettings != null);
            _pManager->setScreen(_pScreenControlsSettings.get());
        }
        else if(pWidget == _pButtonUp.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"up"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_UP);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonUp.get();
        }
        else if(pWidget == _pButtonDown.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"down"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_DOWN);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonDown.get();
        }
        else if(pWidget == _pButtonLeft.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"left"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_LEFT);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonLeft.get();
        }
        else if(pWidget == _pButtonRight.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"right"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_RIGHT);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonRight.get();
        }
        else if(pWidget == _pButtonShoot.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"press-mouse-button"] << L"\n" << (*_appContext.pDico)[L"fire"];
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForMouseControl(MC_SHOOT);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonShoot.get();
        }
        else if(pWidget == _pButtonTimeWarp.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"press-mouse-button"] << L"\n" << (*_appContext.pDico)[L"time-warp"];
            _pScreenSelectInput->initialize(label, SC_MOUSE_KEYBOARD);
            _pScreenSelectInput->waitForMouseControl(MC_TIME_WARP);
            _pManager->setScreen(_pScreenSelectInput.get());
            _pLastKeyFocus = _pButtonTimeWarp.get();
        }
        else if(pWidget == _pAutoShoot.get())
        {
            _appContext.pGameSettings->setMouseAutoFire(_pAutoShoot->isChecked());
        }
        else if(pWidget == _pButtonDefault.get())
        {
            _appContext.pGameSettings->setDefaultKeyboardMovementConfig();
            _appContext.pGameSettings->setDefaultKeyboardShootingConfig();
            _appContext.pGameSettings->setDefaultMouseSettings();
            fillComponentsFromSettings();
        }
    }
}

void ScreenMouseKeyboardSettings::fillComponentsFromSettings()
{
    int32 up;
    int32 down;
    int32 left;
    int32 right;
    _appContext.pGameSettings->getMovementKeyboardControls( up,
                                                            down, 
                                                            left, 
                                                            right);

    String upKeyName = Core::System::getKeyCodeName(up);
    _pButtonUp->setText(upKeyName);
    String downKeyName = Core::System::getKeyCodeName(down);
    _pButtonDown->setText(downKeyName);
    String leftKeyName = Core::System::getKeyCodeName(left);
    _pButtonLeft->setText(leftKeyName);
    String rightKeyName = Core::System::getKeyCodeName(right);
    _pButtonRight->setText(rightKeyName);


    EMouseButton shoot; 
    EMouseButton timeWarp;
    _appContext.pGameSettings->getShootingMouseControls(shoot, timeWarp);

    String shootLabel = mouseButtonToString(shoot);
    String timeWarpLabel = mouseButtonToString(timeWarp);

    _pButtonShoot->setText(shootLabel);
    _pButtonTimeWarp->setText(timeWarpLabel);

    _pAutoShoot->setChecked(_appContext.pGameSettings->getMouseAutoFire());
}

String ScreenMouseKeyboardSettings::mouseButtonToString(EMouseButton mouseButton)
{
    String label;
    switch(mouseButton)
    {
    case MOUSE_LEFT_B:
        label =  (*_appContext.pDico)[L"left-mouse-button"];
        break;
    case MOUSE_RIGHT_B:
        label = (*_appContext.pDico)[L"right-mouse-button"];
        break;
    case MOUSE_MIDDLE_B:
        label = (*_appContext.pDico)[L"middle-mouse-button"];
        break;
    }
    return label;
}

}//namespace ChronoRage
