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
#include "ScreenSettings.h"

namespace ChronoRage
{

ScreenSettings::ScreenSettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    _pLabelSettings = _appContext.pWidgetFactory->createMediumLabel(L"settings", 0, 20);
    Core::Vector2f panelSize = _pPanelButtons->getSize();
    panelSize.y = 110.0f;
    _pLabelSettings->setSize(panelSize);
    _pLabelSettings->setHLayout(GUI::HL_CENTER);    

    Core::List<String> languageList;
    languageList.push_back((*_appContext.pDico)[L"english"]);
    languageList.push_back((*_appContext.pDico)[L"french"]);

    static const int32 Y_PADDING = 53;
    int padding = 3;
    _pLabelLanguage         = _appContext.pWidgetFactory->createMediumLabel(L"language", 49, padding * Y_PADDING);
    _pComboSelectedLanguage = _appContext.pWidgetFactory->createLargeComboBox(languageList, 230, (padding++ * Y_PADDING)+5);
    padding++;
    _pButtonAudioSettings   = _appContext.pWidgetFactory->createMediumButton(L"audio", 49, padding++ * Y_PADDING);
    _pButtonVideoSettings   = _appContext.pWidgetFactory->createMediumButton(L"video", 49, padding++ * Y_PADDING);
    _pButtonControlsSettings= _appContext.pWidgetFactory->createMediumButton(L"controls", 49, padding++ * Y_PADDING);
    padding+=2;
    _pButtonBack            = _appContext.pWidgetFactory->createMediumButton(L"back", 49, padding++ * Y_PADDING);
     
    _pPanelButtons->addChild(_pLabelLanguage);
    _pPanelButtons->addChild(_pComboSelectedLanguage);
    _pPanelButtons->addChild(_pLabelSettings);
    _pPanelButtons->addChild(_pButtonAudioSettings);
    _pPanelButtons->addChild(_pButtonVideoSettings);
    _pPanelButtons->addChild(_pButtonControlsSettings);
    _pPanelButtons->addChild(_pButtonBack);

    _pPanelButtons->pack();

    _pLastKeyFocus = _pButtonBack.get();

    _appContext.pWidgetFactory->bottomScreen(_pPanelButtons);

    _pPanelButtons->addEventHandler(this);
    _pComboSelectedLanguage->addEventHandler(this);
    _pButtonAudioSettings->addEventHandler(this);
    _pButtonVideoSettings->addEventHandler(this);
    _pButtonControlsSettings->addEventHandler(this);
    _pButtonBack->addEventHandler(this);
    _pButtonBack->setHotKey(VK_ESCAPE);
    
    _pDialog->addChild(_pPanelButtons);
}

void ScreenSettings::resetLabels()
{
    Core::List<String> languageList;
    languageList.push_back((*_appContext.pDico)[L"english"]);
    languageList.push_back((*_appContext.pDico)[L"french"]);

    _pComboSelectedLanguage->setItems(languageList);

    _pLabelSettings->setDictionary(_appContext.pDico);
    _pLabelLanguage->setDictionary(_appContext.pDico);
    _pButtonAudioSettings->setDictionary(_appContext.pDico);
    _pButtonVideoSettings->setDictionary(_appContext.pDico);
    _pButtonControlsSettings->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelSettings->setText(L"settings");
    _pLabelLanguage->setText(L"language");
    _pButtonAudioSettings->setText(L"audio");
    _pButtonVideoSettings->setText(L"video");
    _pButtonControlsSettings->setText(L"controls");
    _pButtonBack->setText(L"back");
}

void ScreenSettings::startScreen()
{
    fillComponentsFromSettings();
    // Key nav
    _pComboSelectedLanguage->setKeyWidgetU(_pButtonBack.get());
    _pComboSelectedLanguage->setKeyWidgetD(_pButtonAudioSettings.get());

    _pButtonAudioSettings->setKeyWidgetU(_pComboSelectedLanguage.get());
    _pButtonAudioSettings->setKeyWidgetD(_pButtonVideoSettings.get());

    _pButtonVideoSettings->setKeyWidgetU(_pButtonAudioSettings.get());
    _pButtonVideoSettings->setKeyWidgetD(_pButtonControlsSettings.get());

    _pButtonControlsSettings->setKeyWidgetU(_pButtonVideoSettings.get());
    _pButtonControlsSettings->setKeyWidgetD(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pButtonControlsSettings.get());
    _pButtonBack->setKeyWidgetD(_pComboSelectedLanguage.get());
    // end key nav

    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _pDialog->pack();
}
    
void ScreenSettings::endScreen()
{
}

void ScreenSettings::update(double elapsed)
{
}

void ScreenSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenMainMenu != null);
            _pManager->setScreen(_pScreenMainMenu.get());
            _pLastKeyFocus = _pButtonBack.get();
        }
        else if(pWidget == _pButtonVideoSettings.get())
        {
            LM_ASSERT(_pScreenVideoSettings != null);
            _pManager->setScreen(_pScreenVideoSettings.get());
            _pLastKeyFocus = _pButtonVideoSettings.get();
        }
        else if(pWidget == _pButtonAudioSettings.get())
        {
            LM_ASSERT(_pScreenAudioSettings != null);
            _pManager->setScreen(_pScreenAudioSettings.get());
            _pLastKeyFocus = _pButtonAudioSettings.get();
        }
        else if(pWidget == _pButtonControlsSettings.get())
        {
            LM_ASSERT(_pScreenControlsSettings != null);
            _pManager->setScreen(_pScreenControlsSettings.get());
            _pLastKeyFocus = _pButtonControlsSettings.get();
        }
        else if(pWidget == _pComboSelectedLanguage.get())
        {
            int32 selectedControlsIndex  = _pComboSelectedLanguage->getIndex();
            switch(selectedControlsIndex)
            {
            case 0:
                _appContext.pGameSettings->setLanguage(Core::ENGLISH);
                break;
            case 1:
                _appContext.pGameSettings->setLanguage(Core::FRENCH);
                break;
            }

            _appContext.reloadDictionary = true;

            _pLastKeyFocus = _pComboSelectedLanguage.get();
        }
    }
}

void ScreenSettings::fillComponentsFromSettings()
{
    Core::ELanguage language = _appContext.pGameSettings->getLanguage();

    switch(language)
    {
    case Core::ENGLISH:
        _pComboSelectedLanguage->setIndex(0);
        break;
    case Core::FRENCH:
        _pComboSelectedLanguage->setIndex(1);
        break;
    }
}

void ScreenSettings::setLabels()
{
    
}

}//namespace ChronoRage
