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
#include "ScreenAudioSettings.h"

namespace ChronoRage
{

ScreenAudioSettings::ScreenAudioSettings(AppContext & appContext) 
:   _appContext(appContext),
    _pSoundSource(NULL)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    _pLabelSettings = _appContext.pWidgetFactory->createMediumLabel(L"audio", 0, 20);
    Core::Vector2f panelSize = _pPanelButtons->getSize();
    panelSize.y = 110.0f;
    _pLabelSettings->setSize(panelSize);
    _pLabelSettings->setHLayout(GUI::HL_CENTER);

    static const int32 Y_PADDING = 53;
    int padding = 4;
    _pLabelMusic    = _appContext.pWidgetFactory->createMediumLabel(L"music-volume", 49, padding++ * Y_PADDING);
    _pLabelMusicVolume = _appContext.pWidgetFactory->createMediumLabel(L"", 49, padding * Y_PADDING);
    _pScrollMusic   = _appContext.pWidgetFactory->createHScrollBar(200, (padding++ * Y_PADDING)+5);
    _pLabelSound    = _appContext.pWidgetFactory->createMediumLabel(L"sound-volume", 49, padding++ * Y_PADDING);
    _pLabelSoundVolume = _appContext.pWidgetFactory->createMediumLabel(L"", 49, padding * Y_PADDING);
    _pScrollSound   = _appContext.pWidgetFactory->createHScrollBar(200, (padding++ * Y_PADDING)+5);

    _pLabelMusicVolume->setDictionary(null);
    _pLabelMusicVolume->setText(L"100%");
    _pLabelSoundVolume->setDictionary(null);
    _pLabelSoundVolume->setText(L"100%");

    padding+=2;
    _pButtonBack    = _appContext.pWidgetFactory->createMediumButton(L"back", 49, padding++ * Y_PADDING);

    _pPanelButtons->addChild(_pLabelSettings);
    _pPanelButtons->addChild(_pLabelMusic);
    _pPanelButtons->addChild(_pLabelMusicVolume);
    _pPanelButtons->addChild(_pScrollMusic);
    _pPanelButtons->addChild(_pLabelSound);
    _pPanelButtons->addChild(_pLabelSoundVolume);
    _pPanelButtons->addChild(_pScrollSound);
    _pPanelButtons->addChild(_pButtonBack);

    _pPanelButtons->pack();

    _appContext.pWidgetFactory->bottomScreen(_pPanelButtons);

    _pScrollMusic->addEventHandler(this);
    _pScrollSound->addEventHandler(this);
    _pButtonBack->addEventHandler(this);
    _pButtonBack->setHotKey(VK_ESCAPE);
    
    _pDialog->addChild(_pPanelButtons);

}

ScreenAudioSettings::~ScreenAudioSettings()
{
    stopSoundTest();
}

void ScreenAudioSettings::resetLabels()
{
    _pLabelSettings->setDictionary(_appContext.pDico);
    _pLabelMusic->setDictionary(_appContext.pDico);
    _pLabelSound->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelSettings->setText(L"audio");
    _pLabelMusic->setText(L"music-volume");
    _pLabelSound->setText(L"sound-volume");
    _pButtonBack->setText(L"back");
}

void ScreenAudioSettings::startScreen()
{
    // Key nav
    _pScrollMusic->setKeyWidgetU(_pButtonBack.get());
    _pScrollMusic->setKeyWidgetD(_pScrollSound.get());

    _pScrollSound->setKeyWidgetU(_pScrollMusic.get());
    _pScrollSound->setKeyWidgetD(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pScrollSound.get());
    _pButtonBack->setKeyWidgetD(_pScrollMusic.get());
    // end key nav

    fillComponentsFromSettings();
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pButtonBack.get();
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _pDialog->pack();
}
    
void ScreenAudioSettings::endScreen()
{
}

void ScreenAudioSettings::update(double elapsed)
{
}

void ScreenAudioSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenSettings != null);
            _pManager->setScreen(_pScreenSettings.get());
        }
        stopSoundTest();
    }
    else if(ev == GUI::EV_SCROLLBAR)
    {
        if(pWidget == _pScrollMusic.get())
        {
            updateLabels();
            _appContext.pGameSettings->setMusicVolume(_pScrollMusic->getValuePercent());
            //On applique le gain � la musique en cours
            if(_appContext.pMenuMusic != null)
                _appContext.pMenuMusic->setGain(_appContext.pGameSettings->getMusicVolume() * _appContext.pSoundLibrary->getMusicGain());
            stopSoundTest();
        }
        else if(pWidget == _pScrollSound.get())
        {
            updateLabels();
            _appContext.pGameSettings->setSoundVolume(_pScrollSound->getValuePercent());
            playSoundTest();
        }
        else
        {
            stopSoundTest();
        }
    }
}

void ScreenAudioSettings::playSoundTest()
{
    if(_appContext.pSoundWorld != null)
    {
        if(_pSoundSource == null)
        {
            String soundName = _appContext.pSoundLibrary->getSound(SOUND_PLAYER_SHOT);
            if(soundName != String::EMPTY)
            {
                _pSoundSource = _appContext.pSoundWorld->createSoundSource();
                if(_pSoundSource != null)
                {
                    _pSoundSource->enqueue(soundName, true);
                    _pSoundSource->killAtEnd(true);
                }
            }
        }

        if(_pSoundSource != null)
            _pSoundSource->setGain(_appContext.pGameSettings->getSoundVolume() * _appContext.pSoundLibrary->getDefaultGain(SOUND_PLAYER_SHOT));
    }
}

void ScreenAudioSettings::stopSoundTest()
{
    if(_pSoundSource != null)
    {
        _pSoundSource->stopCurrentLoop();
        _pSoundSource = null;
    }
}

void ScreenAudioSettings::fillComponentsFromSettings()
{
    _pScrollMusic->setValuePercent(_appContext.pGameSettings->getMusicVolume());
    _pScrollSound->setValuePercent(_appContext.pGameSettings->getSoundVolume());
    updateLabels();
}

void ScreenAudioSettings::updateLabels()
{
    int32 musicValue = int32(_pScrollMusic->getValuePercent() * 100);
    Core::String musicValueLabel;
    if(musicValue < 100)
        musicValueLabel << L" ";
    if(musicValue < 10)
        musicValueLabel << L" ";
    musicValueLabel << musicValue << L"%";
    _pLabelMusicVolume->setText(musicValueLabel);

    int32 soundValue = int32(_pScrollSound->getValuePercent() * 100);
    Core::String soundValueLabel;
    if(soundValue < 100)
        soundValueLabel << L" ";
    if(soundValue < 10)
        soundValueLabel << L" ";
    soundValueLabel << soundValue << L"%";
    _pLabelSoundVolume->setText(soundValueLabel);
}

}//namespace ChronoRage
