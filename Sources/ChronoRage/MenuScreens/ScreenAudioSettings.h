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
#ifndef CHRONORAGE_GUI_SCREENAUDIOSETTINGS_H_
#define CHRONORAGE_GUI_SCREENAUDIOSETTINGS_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

class ScreenAudioSettings : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenAudioSettings(AppContext & appContext);

    virtual ~ScreenAudioSettings();

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void playSoundTest();
    void stopSoundTest();

    void setSettingsScreen(const Ptr<IScreen> & pScreen) { _pScreenSettings = pScreen; }

    void fillComponentsFromSettings();
    void updateLabels();

protected:
    AppContext &            _appContext;

    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pPanelButtons;
    
    Ptr<GUI::Label>         _pLabelSettings;

    Ptr<GUI::Label>         _pLabelMusic;
    Ptr<GUI::Label>         _pLabelMusicVolume;
    Ptr<GUI::HScrollBar>    _pScrollMusic;
    Ptr<GUI::Label>         _pLabelSound;
    Ptr<GUI::Label>         _pLabelSoundVolume;
    Ptr<GUI::HScrollBar>    _pScrollSound;

    Ptr<GUI::Button>        _pButtonBack;

    Ptr<IScreen>            _pScreenSettings;

    Ptr<Universe::NodeSoundSource>  _pSoundSource;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_SCREENAUDIOSETTINGS_H_*/
