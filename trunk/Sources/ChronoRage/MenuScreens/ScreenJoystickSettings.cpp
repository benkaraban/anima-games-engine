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
#include "ScreenJoystickSettings.h"

namespace ChronoRage
{

ScreenJoystickSettings::ScreenJoystickSettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButton = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    static const int32 Y_PADDING = 53;
    static const int32 X_COL_1 = 24;
    static const int32 X_COL_2 = 245;

    _pLabelJoyName = _appContext.pWidgetFactory->createMediumLabel(L"", 0, 20);
    Core::Vector2f panelSize = _pPanelButton->getSize();
    panelSize.y = 110.0f;
    _pLabelJoyName->setDictionary(null);
    _pLabelJoyName->setSize(panelSize);
    _pLabelJoyName->setHLayout(GUI::HL_CENTER);

    int padding = 4;

    _pLabelTimeWarp  = _appContext.pWidgetFactory->createMediumLabel(L"time-warp", X_COL_1, padding * Y_PADDING);
    _pButtonTimeWarp = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pLabelFire         = _appContext.pWidgetFactory->createMediumLabel(L"fire", X_COL_1, padding * Y_PADDING);
    _pButtonFire        = _appContext.pWidgetFactory->createSmallButton(L"", X_COL_2, padding++ * Y_PADDING);
    _pAutoShoot         = _appContext.pWidgetFactory->createCheckBox(L"autofire", X_COL_1, padding++ * Y_PADDING);
    _pAutoShoot->setChecked(true);
    _pAutoShoot->addEventHandler(this);
    _pInvertSticks   = _appContext.pWidgetFactory->createCheckBox(L"invert-sticks", X_COL_1, padding++ * Y_PADDING);
    _pInvertSticks->addEventHandler(this);

    _pButtonTimeWarp->setDictionary(null);
    _pButtonTimeWarp->setText(L"---");
    _pButtonTimeWarp->addEventHandler(this);

    _pButtonFire->setDictionary(null);
    _pButtonFire->setText(L"---");
    _pButtonFire->addEventHandler(this);

    _pPanelButton->addChild(_pLabelJoyName);
    _pPanelButton->addChild(_pLabelTimeWarp);
    _pPanelButton->addChild(_pButtonTimeWarp);
    _pPanelButton->addChild(_pLabelFire);
    _pPanelButton->addChild(_pButtonFire);
    _pPanelButton->addChild(_pAutoShoot);
    _pPanelButton->addChild(_pInvertSticks);

    padding = 10;

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
    _pLastKeyFocus = _pButtonBack.get();
}

void ScreenJoystickSettings::resetLabels()
{
    _pLabelTimeWarp->setDictionary(_appContext.pDico);
    _pLabelFire->setDictionary(_appContext.pDico);
    _pAutoShoot->setDictionary(_appContext.pDico);
    _pInvertSticks->setDictionary(_appContext.pDico);
    _pButtonDefault->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelTimeWarp->setText(L"time-warp");
    _pLabelFire->setText(L"fire");
    _pAutoShoot->setText(L"autofire");
    _pInvertSticks->setText(L"invert-sticks");
    _pButtonDefault->setText(L"default");
    _pButtonBack->setText(L"back");
}

void ScreenJoystickSettings::startScreen()
{
    // Key nav
    _pButtonTimeWarp->setKeyWidgetU(_pButtonBack.get());
    _pButtonTimeWarp->setKeyWidgetD(_pButtonFire.get());

    _pButtonFire->setKeyWidgetU(_pButtonTimeWarp.get());
    _pButtonFire->setKeyWidgetD(_pAutoShoot.get());

    _pAutoShoot->setKeyWidgetU(_pButtonFire.get());
    _pAutoShoot->setKeyWidgetD(_pInvertSticks.get());

    _pInvertSticks->setKeyWidgetU(_pAutoShoot.get());
    _pInvertSticks->setKeyWidgetD(_pButtonBack.get());

    _pButtonDefault->setKeyWidgetU(_pInvertSticks.get());
    _pButtonDefault->setKeyWidgetR(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pInvertSticks.get());
    _pButtonBack->setKeyWidgetL(_pButtonDefault.get());
    _pButtonBack->setKeyWidgetD(_pButtonTimeWarp.get());
    // end key nav

    fillComponentsFromSettings();
    _pScreenSelectInput->setControlsSettingsScreen(this);
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenJoystickSettings::endScreen()
{
    _pJoystick = null;
}

void ScreenJoystickSettings::update(double elapsed)
{
}

void ScreenJoystickSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenControlsSettings != null);
            _pManager->setScreen(_pScreenControlsSettings.get());
        }
        else if(pWidget == _pButtonTimeWarp.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"press-joy-button"] << L"\n" << (*_appContext.pDico)[L"time-warp"];
            _pScreenSelectInput->initialize(label, SC_JOYSTICK);
            _pScreenSelectInput->waitForJoystickButton(JC_TIME_WARP, _index);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pButtonFire.get())
        {
            LM_ASSERT(_pScreenSelectInput != null);
            Core::String label;
            label << (*_appContext.pDico)[L"press-joy-button"] << L"\n" << (*_appContext.pDico)[L"fire"];
            _pScreenSelectInput->initialize(label, SC_JOYSTICK);
            _pScreenSelectInput->waitForJoystickButton(JC_FIRE, _index);
            _pManager->setScreen(_pScreenSelectInput.get());
        }
        else if(pWidget == _pAutoShoot.get())
        {
            JoySettings settings = _appContext.pGameSettings->getJoySettings(_pJoystick->getName());
            settings.autofire = _pAutoShoot->isChecked();
            _appContext.pGameSettings->setJoySettings(_pJoystick->getName(), settings);
        }
        else if(pWidget == _pButtonDefault.get())
        {
            _appContext.pGameSettings->setJoySettings(_pJoystick->getName(), JoySettings());
            fillComponentsFromSettings();
        }
        else if(pWidget == _pInvertSticks.get())
        {
            JoySettings settings = _appContext.pGameSettings->getJoySettings(_pJoystick->getName());
            settings.inverseSticks = _pInvertSticks->isChecked();
            _appContext.pGameSettings->setJoySettings(_pJoystick->getName(), settings);
        }

        _pLastKeyFocus = pWidget;
    }
}

void ScreenJoystickSettings::fillComponentsFromSettings()
{
    _pJoystick = _appContext.pInputManager->getJoysticks()[_index];
    
    String joyName = _pJoystick->getName();
    const JoySettings & settings = _appContext.pGameSettings->getJoySettings(joyName);

    _pLabelJoyName->setText(joyName);
    _pButtonTimeWarp->setText(_pJoystick->getButtonName(settings.timeWarpButton));
    _pButtonFire->setText(_pJoystick->getButtonName(settings.fireButton));
    _pAutoShoot->setChecked(settings.autofire);
    _pInvertSticks->setChecked(settings.inverseSticks);
}

}//namespace ChronoRage
