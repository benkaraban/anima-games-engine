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
#ifndef CHRONORAGE_GUI_SCREENKEYBOARDONLYSETTINGS_H_
#define CHRONORAGE_GUI_SCREENKEYBOARDONLYSETTINGS_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>
#include <MenuScreens/ScreenSelectInput.h>

namespace ChronoRage
{

class ScreenKeyboardOnlySettings : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenKeyboardOnlySettings(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setControlsSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenControlsSettings = pScreen; }
    void setSelectInputScreen(const Ptr<ScreenSelectInput> & pScreen) { _pScreenSelectInput = pScreen; }

    void fillComponentsFromSettings();

protected:
    AppContext &            _appContext;
    
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pGlobalPanel;

    //D�placement
    Ptr<GUI::Widget>        _pPanelButton;
    Ptr<GUI::Label>         _pLabelMovement;

    Ptr<GUI::Label>         _pLabelUp;
    Ptr<GUI::Button>        _pButtonUp;
    Ptr<GUI::Label>         _pLabelDown;
    Ptr<GUI::Button>        _pButtonDown;
    Ptr<GUI::Label>         _pLabelLeft;
    Ptr<GUI::Button>        _pButtonLeft;
    Ptr<GUI::Label>         _pLabelRight;
    Ptr<GUI::Button>        _pButtonRight;
    
    Ptr<GUI::Label>         _pLabelTimeWarp;
    Ptr<GUI::Button>        _pButtonTimeWarp;

    Ptr<GUI::Widget>        _pPanelAim;
    Ptr<GUI::Label>         _pLabelAim;

    Ptr<GUI::Label>         _pLabelShootUp;
    Ptr<GUI::Button>        _pButtonShootUp;
    Ptr<GUI::Label>         _pLabelShootDown;
    Ptr<GUI::Button>        _pButtonShootDown;
    Ptr<GUI::Label>         _pLabelShootLeft;
    Ptr<GUI::Button>        _pButtonShootLeft;
    Ptr<GUI::Label>         _pLabelShootRight;
    Ptr<GUI::Button>        _pButtonShootRight;

    Ptr<GUI::Label>         _pLabelFire;
    Ptr<GUI::Button>        _pButtonFire;
    Ptr<GUI::CheckBox>      _pAutoShoot;
    
    Ptr<GUI::Button>        _pButtonDefault;
    Ptr<GUI::Button>        _pButtonBack;
    
    Ptr<IScreen>            _pScreenControlsSettings;
    Ptr<ScreenSelectInput>  _pScreenSelectInput;
    GUI::Widget *           _pLastKeyFocus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_SCREENKEYBOARDONLYSETTINGS_H_*/
