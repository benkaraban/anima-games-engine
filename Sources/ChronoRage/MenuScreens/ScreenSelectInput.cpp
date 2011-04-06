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
#include "ScreenSelectInput.h"

namespace ChronoRage
{

ScreenSelectInput::ScreenSelectInput(AppContext & appContext)
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButtons = _appContext.pWidgetFactory->createThinPanel(0, 0);
    
    _pLabelSettings = _appContext.pWidgetFactory->createMediumLabel(L"", 0, 20);
    _pLabelSettings->setDictionary(null);
    _pLabelSettings->setHLayout(GUI::HL_CENTER);
    Core::Vector2f panelSize = _pPanelButtons->getSize();
    panelSize.y = 110.0f;
    _pLabelSettings->setSize(panelSize);
    
    _pButtonBack  = _appContext.pWidgetFactory->createSmallButton(L"cancel", 226, 110);
    _pButtonBack->setHotKey(VK_ESCAPE);
    
    _pPanelButtons->addChild(_pLabelSettings);
    _pPanelButtons->addChild(_pButtonBack);
    _pPanelButtons->pack();

    _appContext.pWidgetFactory->centerScreen(_pPanelButtons);

    _pDialog->addChild(_pPanelButtons);
    _pDialog->pack();

    _pButtonBack->addEventHandler(this);
}

void ScreenSelectInput::resetLabels()
{
    _pButtonBack->setDictionary(_appContext.pDico);
    _pButtonBack->setText(L"cancel");
}

void ScreenSelectInput::initialize(String label, ESelectedControls selectedControls)
{
    _pLabelSettings->setText(label);
    _selectedControls = selectedControls;

    _controlDirection = (EControlDirection)-1;
    _mouseControl = (EMouseControl)-1;
}

void ScreenSelectInput::waitForControlDirection(EControlDirection controlDirection)
{
    _controlDirection = controlDirection;
    _mouseControl = (EMouseControl)-1;
}

void ScreenSelectInput::waitForMouseControl(EMouseControl mouseControl)
{
    _controlDirection = (EControlDirection)-1;
    _mouseControl = mouseControl;
}

void ScreenSelectInput::waitForJoystickButton(EJoystickControl joystickControl, int32 index)
{
    _controlDirection = (EControlDirection)-1;
    _joystickControl = joystickControl;
    _joystickIndex = index;
}

void ScreenSelectInput::startScreen()
{
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _pDialog->pack();
}

void ScreenSelectInput::endScreen()
{}

void ScreenSelectInput::update(double elapsed)
{
    switch(_selectedControls)
    {
    case SC_MOUSE_KEYBOARD:
        {
            if(isValid(_controlDirection))
            {
                int32 keyType = _appContext.pInputManager->getKeyTyped();
                if(keyType != -1 && Core::System::getKeyCodeName(keyType) != String::EMPTY && keyType != VK_ESCAPE)
                {
                    _appContext.pGameSettings->setKeyboardValue(_controlDirection, keyType);
                    
                    LM_ASSERT(_pScreenControlsSettings != NULL);
                    _pManager->setScreen(_pScreenControlsSettings);
                }
            }
            else if(isValid(_mouseControl))
            {
                if(_appContext.pInputManager->isLMBClicked())
                {
                    _appContext.pGameSettings->setMouseValue(_mouseControl, MOUSE_LEFT_B);
                    LM_ASSERT(_pScreenControlsSettings != NULL);
                    _pManager->setScreen(_pScreenControlsSettings);
                }
                else if(_appContext.pInputManager->isMMBClicked())
                {
                    _appContext.pGameSettings->setMouseValue(_mouseControl, MOUSE_MIDDLE_B);
                    LM_ASSERT(_pScreenControlsSettings != NULL);
                    _pManager->setScreen(_pScreenControlsSettings);
                }
                else if(_appContext.pInputManager->isRMBClicked())
                {
                    _appContext.pGameSettings->setMouseValue(_mouseControl, MOUSE_RIGHT_B);
                    LM_ASSERT(_pScreenControlsSettings != NULL);
                    _pManager->setScreen(_pScreenControlsSettings);
                }
            }
        }
        break;
    case SC_KEYBOARD_ONLY:
        {
            if(isValid(_controlDirection))
            {
                int32 keyType = _appContext.pInputManager->getKeyTyped();
                if(keyType != -1 && Core::System::getKeyCodeName(keyType) != String::EMPTY && keyType != VK_ESCAPE)
                {
                    _appContext.pGameSettings->setKeyboardValue(_controlDirection, keyType);
                    
                    LM_ASSERT(_pScreenControlsSettings != NULL);
                    _pManager->setScreen(_pScreenControlsSettings);
                }
            }
        }
        break;
    case SC_JOYSTICK:
        {
            const Window::IJoystick & joy = *_appContext.pInputManager->getJoysticks()[_joystickIndex];

            int32 buttonTyped = joy.getButtonTyped();

            if(buttonTyped != -1)
            {
                if(isValid(_joystickControl))
                {
                    JoySettings settings = _appContext.pGameSettings->getJoySettings(joy.getName());
                    if(_joystickControl == JC_TIME_WARP)
                        settings.timeWarpButton = buttonTyped;
                    else if(_joystickControl == JC_FIRE)
                        settings.fireButton = buttonTyped;

                    _appContext.pGameSettings->setJoySettings(joy.getName(), settings);

                    _pManager->setScreen(_pScreenControlsSettings);
                }
            }
        }
        break;
    }
}

void ScreenSelectInput::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenControlsSettings != NULL);
            _pManager->setScreen(_pScreenControlsSettings);
        }
    }
}


}//namespace ChronoRage