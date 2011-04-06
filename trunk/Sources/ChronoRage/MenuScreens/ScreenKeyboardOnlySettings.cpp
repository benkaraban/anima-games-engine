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
#include "ScreenKeyboardOnlySettings.h"

namespace ChronoRage
{

ScreenKeyboardOnlySettings::ScreenKeyboardOnlySettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pGlobalPanel = Ptr<GUI::Widget>(new GUI::Widget());
    _pGlobalPanel->setPosition(Core::Vector2f(0, 0));
    
    _pPanelButton = _appContext.pWidgetFactory->createMediumPanel(0, 0);
    _pPanelAim = _appContext.pWidgetFactory->createMediumPanel(500, 0);

    static const int32 Y_PADDING = 53;
    static const int32 X_COL_1 = 24;
    static const int32 X_COL_2 = 245;

    _pLabelMovement = _appContext.pWidgetFactory->createMediumLabel(L"movement", 0, 20);
    Core::Vector2f panelSize = _pPanelButton->getSize();
    panelSize.y = 110.0f;
    _pLabelMovement->setSize(panelSize);
    _pLabelMovement->setHLayout(GUI::HL_CENTER);

    int padding = 3;
    _pLabelUp       = _appContext.pWidgetFactory->createMediumLabel(L"up", X_COL_1, padding * Y_PADDING);
    _pButtonUp      = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelDown     = _appContext.pWidgetFactory->createMediumLabel(L"down", X_COL_1, padding * Y_PADDING);
    _pButtonDown    = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelLeft     = _appContext.pWidgetFactory->createMediumLabel(L"left", X_COL_1, padding * Y_PADDING);
    _pButtonLeft    = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelRight    = _appContext.pWidgetFactory->createMediumLabel(L"right", X_COL_1, padding * Y_PADDING);
    _pButtonRight   = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    padding++;
    _pLabelTimeWarp   = _appContext.pWidgetFactory->createMediumLabel(L"time-warp", X_COL_1, padding * Y_PADDING);
    _pButtonTimeWarp  = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    padding++;
    _pButtonDefault = _appContext.pWidgetFactory->createSmallButton(L"default", X_COL_1, padding * Y_PADDING);
    _pButtonDefault->addEventHandler(this);

    _pButtonUp->setDictionary(null);
    _pButtonDown->setDictionary(null);
    _pButtonLeft->setDictionary(null);
    _pButtonRight->setDictionary(null);
    _pButtonTimeWarp->setDictionary(null);

    _pButtonUp->addEventHandler(this);
    _pButtonDown->addEventHandler(this);
    _pButtonLeft->addEventHandler(this);
    _pButtonRight->addEventHandler(this);
    _pButtonTimeWarp->addEventHandler(this);

    _pPanelButton->addChild(_pLabelMovement);

    _pPanelButton->addChild(_pLabelUp);
    _pPanelButton->addChild(_pButtonUp);
    _pPanelButton->addChild(_pLabelDown);
    _pPanelButton->addChild(_pButtonDown);
    _pPanelButton->addChild(_pLabelLeft);
    _pPanelButton->addChild(_pButtonLeft);
    _pPanelButton->addChild(_pLabelRight);
    _pPanelButton->addChild(_pButtonRight);
    _pPanelButton->addChild(_pLabelTimeWarp);
    _pPanelButton->addChild(_pButtonTimeWarp);

    _pPanelButton->addChild(_pButtonDefault);
    _pPanelButton->pack();


    _pLabelAim = _appContext.pWidgetFactory->createMediumLabel(L"fire", 0,  20);
    panelSize.y = 110.0f;
    _pLabelAim->setSize(panelSize);
    _pLabelAim->setHLayout(GUI::HL_CENTER);

    padding = 3;
    _pLabelShootUp      = _appContext.pWidgetFactory->createMediumLabel(L"up", X_COL_1, padding * Y_PADDING);
    _pButtonShootUp     = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelShootDown    = _appContext.pWidgetFactory->createMediumLabel(L"down", X_COL_1, padding * Y_PADDING);
    _pButtonShootDown   = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelShootLeft    = _appContext.pWidgetFactory->createMediumLabel(L"left", X_COL_1, padding * Y_PADDING);
    _pButtonShootLeft   = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelShootRight   = _appContext.pWidgetFactory->createMediumLabel(L"right", X_COL_1, padding * Y_PADDING);
    _pButtonShootRight  = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);

    padding++;
    _pLabelFire         = _appContext.pWidgetFactory->createMediumLabel(L"fire", X_COL_1, padding * Y_PADDING);
    _pButtonFire        = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pAutoShoot         = _appContext.pWidgetFactory->createCheckBox(L"autofire", X_COL_1, padding++ * Y_PADDING);
    _pAutoShoot->setChecked(true);
    
    _pButtonShootUp->setDictionary(null);
    _pButtonShootDown->setDictionary(null);
    _pButtonShootLeft->setDictionary(null);
    _pButtonShootRight->setDictionary(null);
    _pButtonFire->setDictionary(null);

    _pButtonShootUp->addEventHandler(this);
    _pButtonShootDown->addEventHandler(this);
    _pButtonShootLeft->addEventHandler(this);
    _pButtonShootRight->addEventHandler(this);
    _pButtonFire->addEventHandler(this);
    _pAutoShoot->addEventHandler(this);

    _pPanelAim->addChild(_pLabelAim);
    _pPanelAim->addChild(_pLabelShootUp);
    _pPanelAim->addChild(_pButtonShootUp);
    _pPanelAim->addChild(_pLabelShootDown);
    _pPanelAim->addChild(_pButtonShootDown);
    _pPanelAim->addChild(_pLabelShootLeft);
    _pPanelAim->addChild(_pButtonShootLeft);
    _pPanelAim->addChild(_pLabelShootRight);
    _pPanelAim->addChild(_pButtonShootRight);
    _pPanelAim->addChild(_pLabelFire);
    _pPanelAim->addChild(_pButtonFire);
    _pPanelAim->addChild(_pAutoShoot);

    _pButtonBack = _appContext.pWidgetFactory->createSmallButton(L"back", X_COL_2, padding * Y_PADDING);
    _pButtonBack->setHotKey(VK_ESCAPE);
    _pButtonBack->addEventHandler(this);

    _pPanelAim->addChild(_pButtonBack);
    _pPanelAim->pack();

    _pGlobalPanel->addChild(_pPanelButton);
    _pGlobalPanel->addChild(_pPanelAim);
    _pGlobalPanel->pack();

    _appContext.pWidgetFactory->bottomScreen(_pGlobalPanel);

    _pLastKeyFocus = _pButtonBack.get();
    _pDialog->addChild(_pGlobalPanel);
}

void ScreenKeyboardOnlySettings::resetLabels()
{
    _pLabelUp->setDictionary(_appContext.pDico);
    _pLabelDown->setDictionary(_appContext.pDico);
    _pLabelLeft->setDictionary(_appContext.pDico);
    _pLabelRight->setDictionary(_appContext.pDico);
    _pLabelTimeWarp->setDictionary(_appContext.pDico);
    _pButtonDefault->setDictionary(_appContext.pDico);
    _pLabelAim->setDictionary(_appContext.pDico);
    _pLabelShootUp->setDictionary(_appContext.pDico);
    _pLabelShootDown->setDictionary(_appContext.pDico);
    _pLabelShootLeft->setDictionary(_appContext.pDico);
    _pLabelShootRight->setDictionary(_appContext.pDico);
    _pLabelFire->setDictionary(_appContext.pDico);
    _pAutoShoot->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelUp->setText(L"up");
    _pLabelDown->setText(L"down");
    _pLabelLeft->setText(L"left");
    _pLabelRight->setText(L"right");
    _pLabelTimeWarp->setText(L"time-warp");
    _pButtonDefault->setText(L"default");
    _pLabelAim->setText(L"fire");
    _pLabelShootUp->setText(L"up");
    _pLabelShootDown->setText(L"down");
    _pLabelShootLeft->setText(L"left");
    _pLabelShootRight->setText(L"right");
    _pLabelFire->setText(L"fire");
    _pAutoShoot->setText(L"autofire");
    _pButtonBack->setText(L"back");
}

void ScreenKeyboardOnlySettings::startScreen()
{
    // Key nav
    _pButtonUp->setKeyWidgetU(_pButtonBack.get());
    _pButtonUp->setKeyWidgetD(_pButtonDown.get());
    _pButtonUp->setKeyWidgetR(_pButtonShootUp.get());

    _pButtonDown->setKeyWidgetU(_pButtonUp.get());
    _pButtonDown->setKeyWidgetD(_pButtonLeft.get());
    _pButtonDown->setKeyWidgetR(_pButtonShootDown.get());

    _pButtonLeft->setKeyWidgetU(_pButtonDown.get());
    _pButtonLeft->setKeyWidgetD(_pButtonRight.get());
    _pButtonLeft->setKeyWidgetR(_pButtonShootLeft.get());

    _pButtonRight->setKeyWidgetU(_pButtonLeft.get());
    _pButtonRight->setKeyWidgetD(_pButtonTimeWarp.get());
    _pButtonRight->setKeyWidgetR(_pButtonShootRight.get());

    _pButtonShootUp->setKeyWidgetU(_pButtonBack.get());
    _pButtonShootUp->setKeyWidgetD(_pButtonShootDown.get());
    _pButtonShootUp->setKeyWidgetL(_pButtonUp.get());

    _pButtonShootDown->setKeyWidgetU(_pButtonShootUp.get());
    _pButtonShootDown->setKeyWidgetD(_pButtonShootLeft.get());
    _pButtonShootDown->setKeyWidgetL(_pButtonDown.get());

    _pButtonShootLeft->setKeyWidgetU(_pButtonShootDown.get());
    _pButtonShootLeft->setKeyWidgetD(_pButtonShootRight.get());
    _pButtonShootLeft->setKeyWidgetL(_pButtonLeft.get());

    _pButtonShootRight->setKeyWidgetU(_pButtonShootLeft.get());
    _pButtonShootRight->setKeyWidgetD(_pButtonFire.get());
    _pButtonShootRight->setKeyWidgetL(_pButtonRight.get());

    _pButtonFire->setKeyWidgetU(_pButtonShootRight.get());
    _pButtonFire->setKeyWidgetD(_pAutoShoot.get());
    _pButtonFire->setKeyWidgetL(_pButtonTimeWarp.get());
    
    _pAutoShoot->setKeyWidgetU(_pButtonFire.get());
    _pAutoShoot->setKeyWidgetD(_pButtonBack.get());
    _pAutoShoot->setKeyWidgetL(_pButtonTimeWarp.get());

    _pButtonTimeWarp->setKeyWidgetU(_pButtonShootRight.get());
    _pButtonTimeWarp->setKeyWidgetD(_pButtonBack.get());
    _pButtonTimeWarp->setKeyWidgetR(_pButtonFire.get());

    _pButtonBack->setKeyWidgetU(_pAutoShoot.get());
    _pButtonBack->setKeyWidgetL(_pButtonDefault.get());
    _pButtonBack->setKeyWidgetD(_pButtonUp.get());

    _pButtonDefault->setKeyWidgetU(_pButtonTimeWarp.get());
    _pButtonDefault->setKeyWidgetR(_pButtonBack.get());
    // end key nav

    fillComponentsFromSettings();
    _pScreenSelectInput->setControlsSettingsScreen(this);
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenKeyboardOnlySettings::endScreen()
{
}

void ScreenKeyboardOnlySettings::update(double elapsed)
{
}

void ScreenKeyboardOnlySettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
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
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_UP);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonDown.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"down"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_DOWN);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonLeft.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"left"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_LEFT);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonRight.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"right"] << L" ( " << (*_appContext.pDico)[L"movement"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_MOVE_RIGHT);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonShootUp.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"up"] << L" ( " << (*_appContext.pDico)[L"fire"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_SHOOT_UP);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonShootDown.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"down"] << L" ( " << (*_appContext.pDico)[L"fire"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_SHOOT_DOWN);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonShootLeft.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"left"] << L" ( " << (*_appContext.pDico)[L"fire"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_SHOOT_LEFT);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonShootRight.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"right"] << L" ( " << (*_appContext.pDico)[L"fire"] << L" )";
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_SHOOT_RIGHT);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonTimeWarp.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"time-warp"];
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_FIRE);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonFire.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"push-key"] << L"\n" << (*_appContext.pDico)[L"fire"];
            _pScreenSelectInput->initialize(label, SC_KEYBOARD_ONLY);
            _pScreenSelectInput->waitForControlDirection(CD_FIRE);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pAutoShoot.get())
        {
            _appContext.pGameSettings->setKeyboardAutoFire(_pAutoShoot->isChecked());
        }
        else if(pWidget == _pButtonDefault.get())
        {
            _appContext.pGameSettings->setDefaultKeyboardMovementConfig();
            _appContext.pGameSettings->setDefaultKeyboardShootingConfig();
            fillComponentsFromSettings();
        }

        _pLastKeyFocus = pWidget;
    }
}

void ScreenKeyboardOnlySettings::fillComponentsFromSettings()
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

    int32 timeWarp;
    int32 fire;
    _appContext.pGameSettings->getShootingKeyboardControls( up,
                                                            down, 
                                                            left, 
                                                            right,
                                                            timeWarp,
                                                            fire);

    upKeyName = Core::System::getKeyCodeName(up);
    _pButtonShootUp->setText(upKeyName);
    downKeyName = Core::System::getKeyCodeName(down);
    _pButtonShootDown->setText(downKeyName);
    leftKeyName = Core::System::getKeyCodeName(left);
    _pButtonShootLeft->setText(leftKeyName);
    rightKeyName = Core::System::getKeyCodeName(right);
    _pButtonShootRight->setText(rightKeyName);

    String timeWarpKeyName = Core::System::getKeyCodeName(timeWarp);
    _pButtonTimeWarp->setText(timeWarpKeyName);

    String fireKeyName = Core::System::getKeyCodeName(fire);
    _pButtonFire->setText(fireKeyName);

    _pAutoShoot->setChecked(_appContext.pGameSettings->getKeyboardAutoFire());
}

}//namespace ChronoRage
