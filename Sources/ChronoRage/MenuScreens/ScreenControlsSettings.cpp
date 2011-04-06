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
#include "ScreenControlsSettings.h"

namespace ChronoRage
{

ScreenControlsSettings::ScreenControlsSettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    _pLabelSettings = _appContext.pWidgetFactory->createMediumLabel(L"controls", 0, 20);
    Core::Vector2f panelSize = _pPanelButtons->getSize();
    panelSize.y = 110.0f;
    _pLabelSettings->setSize(panelSize);
    _pLabelSettings->setHLayout(GUI::HL_CENTER);

    Core::List<String> controllerList;
    controllerList.push_back((*_appContext.pDico)[L"mouse-keyboard"]);
    controllerList.push_back((*_appContext.pDico)[L"keyboard-only"]);

    Window::InputManager & input = *appContext.pInputManager;
    int32 joyCount = std::min(5, input.getJoysticks().size());

    for(int32 ii=0; ii < joyCount; ii++)
    {
        String name(input.getJoysticks()[ii]->getName());
        name.resize(std::min(name.size(), 20));
        controllerList.push_back(name);
    }

    static const int32 Y_PADDING = 53;
    int padding = 3;
    _pLabelSelectedControls = _appContext.pWidgetFactory->createMediumLabel(L"selected-controls", 49, padding * Y_PADDING);
    _pComboSelectedControls = _appContext.pWidgetFactory->createLargeComboBox(controllerList, 230, (padding++ * Y_PADDING)+5);

    _pButtonMouseKeyboard   = _appContext.pWidgetFactory->createMediumButton(L"mouse-keyboard", 49, padding++ * Y_PADDING);
    _pButtonKeyboardOnly    = _appContext.pWidgetFactory->createMediumButton(L"keyboard-only", 49, padding++ * Y_PADDING);

    Core::List<Ptr<GUI::Button> > joyButtons;

    for(int32 ii=0; ii < joyCount; ii++)
    {
        String name(input.getJoysticks()[ii]->getName());
        name.resize(std::min(name.size(), 20));

        Ptr<GUI::Button> pButton = _appContext.pWidgetFactory->createMediumButton(L"", 49, padding++ * Y_PADDING);
        pButton->setDictionary(null);
        pButton->setText(name);
        pButton->setId(ii);
        pButton->addEventHandler(this);
        _pPanelButtons->addChild(pButton);

        controllerList.push_back(name);
        joyButtons.push_back(pButton);
    }

    padding = 10;
    _pButtonBack                    = _appContext.pWidgetFactory->createMediumButton(L"back", 49, padding++ * Y_PADDING);

    _pPanelButtons->addChild(_pLabelSettings);
    _pPanelButtons->addChild(_pLabelSelectedControls);
    _pPanelButtons->addChild(_pComboSelectedControls);
    _pPanelButtons->addChild(_pButtonMouseKeyboard);
    _pPanelButtons->addChild(_pButtonKeyboardOnly);
    _pPanelButtons->addChild(_pButtonBack);
    
    _pPanelButtons->pack();

    // Key nav
    _pComboSelectedControls->setKeyWidgetU(_pButtonBack.get());
    _pComboSelectedControls->setKeyWidgetD(_pButtonMouseKeyboard.get());

    _pButtonMouseKeyboard->setKeyWidgetU(_pComboSelectedControls.get());
    _pButtonMouseKeyboard->setKeyWidgetD(_pButtonKeyboardOnly.get());

    _pButtonKeyboardOnly->setKeyWidgetU(_pButtonMouseKeyboard.get());
    _pButtonKeyboardOnly->setKeyWidgetD(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pButtonKeyboardOnly.get());
    _pButtonBack->setKeyWidgetD(_pComboSelectedControls.get());

    if(!joyButtons.empty())
    {
        for(int32 ii=0; ii < joyButtons.size(); ii++)
        {
            if(ii - 1 >= 0)
                joyButtons[ii]->setKeyWidgetU(joyButtons[ii - 1].get());
            if(ii + 1 < joyButtons.size())
                joyButtons[ii]->setKeyWidgetD(joyButtons[ii + 1].get());
        }

        _pButtonKeyboardOnly->setKeyWidgetD(joyButtons[0].get());
        joyButtons[0]->setKeyWidgetU(_pButtonKeyboardOnly.get());

        joyButtons[joyButtons.size() - 1]->setKeyWidgetD(_pButtonBack.get());
        _pButtonBack->setKeyWidgetU(joyButtons[joyButtons.size() - 1].get());
    }

    _pLastKeyFocus = _pButtonBack.get();
    // end key nav

    _appContext.pWidgetFactory->bottomScreen(_pPanelButtons);

    _pComboSelectedControls->addEventHandler(this);
    _pButtonMouseKeyboard->addEventHandler(this);
    _pButtonKeyboardOnly->addEventHandler(this);

    _pButtonBack->addEventHandler(this);
    _pButtonBack->setHotKey(VK_ESCAPE);
    
    _pDialog->addChild(_pPanelButtons);
}

void ScreenControlsSettings::resetLabels()
{
    Core::List<String> controllerList;
    controllerList.push_back((*_appContext.pDico)[L"mouse-keyboard"]);
    controllerList.push_back((*_appContext.pDico)[L"keyboard-only"]);

    Window::InputManager & input = *_appContext.pInputManager;
    int32 joyCount = std::min(5, input.getJoysticks().size());

    for(int32 ii=0; ii < joyCount; ii++)
    {
        String name(input.getJoysticks()[ii]->getName());
        name.resize(std::min(name.size(), 20));
        controllerList.push_back(name);
    }
    
    _pComboSelectedControls->setItems(controllerList);

    _pLabelSettings->setDictionary(_appContext.pDico);
    _pLabelSelectedControls->setDictionary(_appContext.pDico);
    _pButtonMouseKeyboard->setDictionary(_appContext.pDico);
    _pButtonKeyboardOnly->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelSettings->setText(L"controls");
    _pLabelSelectedControls->setText(L"selected-controls");
    _pButtonMouseKeyboard->setText(L"mouse-keyboard");
    _pButtonKeyboardOnly->setText(L"keyboard-only");
    _pButtonBack->setText(L"back");
}

void ScreenControlsSettings::startScreen()
{
    fillComponentsFromSettings();
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenControlsSettings::endScreen()
{
}

void ScreenControlsSettings::update(double elapsed)
{
}

void ScreenControlsSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenSettings != null);
            _pManager->setScreen(_pScreenSettings.get());
        }
        else if(pWidget == _pComboSelectedControls.get())
        {
            int32 selectedControlsIndex  = _pComboSelectedControls->getIndex();
            switch(selectedControlsIndex)
            {
            case 0:
                _appContext.pGameSettings->setSelectedControls(SC_MOUSE_KEYBOARD);
                break;
            case 1:
                _appContext.pGameSettings->setSelectedControls(SC_KEYBOARD_ONLY);
                break;
            default:
                _appContext.pGameSettings->setSelectedControls(SC_JOYSTICK);
                _appContext.pGameSettings->setSelectedJoystick(selectedControlsIndex - 2);
                break;
            }
        }
        else if(pWidget == _pButtonMouseKeyboard.get())
        {
            LM_ASSERT(_pScreenMouseKeyboardSettings != null);
            _appContext.pGameSettings->setSelectedControls(SC_MOUSE_KEYBOARD);
            _pManager->setScreen(_pScreenMouseKeyboardSettings.get());
            _pLastKeyFocus = _pButtonMouseKeyboard.get();
        }
        else if(pWidget == _pButtonKeyboardOnly.get())
        {
            LM_ASSERT(_pScreenKeyboardOnlySettings != null);
            _appContext.pGameSettings->setSelectedControls(SC_KEYBOARD_ONLY);
            _pManager->setScreen(_pScreenKeyboardOnlySettings.get());
            _pLastKeyFocus = _pButtonKeyboardOnly.get();
        }
        else
        {
            int32 selectedJoystick  = pWidget->getId();
            _appContext.pGameSettings->setSelectedControls(SC_JOYSTICK);
            _appContext.pGameSettings->setSelectedJoystick(selectedJoystick);

            _pScreenJoystickSettings->setJoyIndex(pWidget->getId());
            _pManager->setScreen(_pScreenJoystickSettings.get());
            _pLastKeyFocus = pWidget;
        }
    }
}

void ScreenControlsSettings::fillComponentsFromSettings()
{
    ESelectedControls selectedControls = _appContext.pGameSettings->getSelectedControls();
    switch(selectedControls)
    {
    case SC_MOUSE_KEYBOARD:
        _pComboSelectedControls->setIndex(0);
        break;
    case SC_KEYBOARD_ONLY:
        _pComboSelectedControls->setIndex(1);
        break;
    case SC_JOYSTICK:
        _pComboSelectedControls->setIndex(2 + _appContext.pGameSettings->getSelectedJoystick());
        break;
    }
}

}//namespace ChronoRage
