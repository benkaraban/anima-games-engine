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
#ifndef CHRONORAGE_GUI_SCREENSETTINGS_H_
#define CHRONORAGE_GUI_SCREENSETTINGS_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

class ScreenSettings : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenSettings(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setMainMenuScreen(const Ptr<IScreen> & pScreen) { _pScreenMainMenu = pScreen; }
    void setVideoSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenVideoSettings = pScreen; }
    void setAudioSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenAudioSettings = pScreen; }
    void setControlsSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenControlsSettings = pScreen; }

    void fillComponentsFromSettings();

    void setLabels();

protected:
    AppContext &            _appContext;
    
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pPanelButtons;
    
    Ptr<GUI::Label>         _pLabelSettings;

    Ptr<GUI::Label>         _pLabelLanguage;
    Ptr<GUI::ComboBox>      _pComboSelectedLanguage;

    Ptr<GUI::Button>        _pButtonAudioSettings;
    Ptr<GUI::Button>        _pButtonVideoSettings;
    Ptr<GUI::Button>        _pButtonControlsSettings;
    Ptr<GUI::Button>        _pButtonBack;
    
    Ptr<IScreen>            _pScreenMainMenu;
    Ptr<IScreen>            _pScreenVideoSettings;
    Ptr<IScreen>            _pScreenAudioSettings;
    Ptr<IScreen>            _pScreenControlsSettings;

    GUI::Widget *           _pLastKeyFocus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_SCREENSETTINGS_H_*/
