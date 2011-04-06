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
#include "ScreenVideoSettings.h"

namespace ChronoRage
{

ScreenVideoSettings::ScreenVideoSettings(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pPanelButtons = _appContext.pWidgetFactory->createMediumPanel(0, 0);

    _pLabelSettings = _appContext.pWidgetFactory->createMediumLabel(L"video", 0, 20);
    Core::Vector2f panelSize = _pPanelButtons->getSize();
    panelSize.y = 110.0f;
    _pLabelSettings->setSize(panelSize);
    _pLabelSettings->setHLayout(GUI::HL_CENTER);

    Renderer::EMSAALevel msaaMaxLevel = _appContext.pRenderer->getMaxRenderSettings().msaaLevel;
    
    Core::List<String> antiAliasList;

    switch(msaaMaxLevel)
    {
        case Renderer::MSAA_NONE:
            antiAliasList.push_back(L"Off");
            break;
        case Renderer::MSAA_2X:
            antiAliasList.push_back(L"Off");
            antiAliasList.push_back(L"x2");
            break;
        case Renderer::MSAA_4X:
            antiAliasList.push_back(L"Off");
            antiAliasList.push_back(L"x2");
            antiAliasList.push_back(L"x4");
            break;
        default:
            antiAliasList.push_back(L"Off");
            antiAliasList.push_back(L"x2");
            antiAliasList.push_back(L"x4");
            antiAliasList.push_back(L"x8");
            break;
    }

    static const int32 Y_PADDING = 53;
    int padding = 3;
    _pCheckBoxFullScreen            = _appContext.pWidgetFactory->createCheckBox(L"fullscreen", 49, padding++ * Y_PADDING);
    _pCheckBoxKeepAspectRatio       = _appContext.pWidgetFactory->createCheckBox(L"keep-aspect-ratio", 49, padding++ * Y_PADDING);
    _pLabelResolution               = _appContext.pWidgetFactory->createMediumLabel(L"resolution", 49, padding * Y_PADDING);
    _pComboResolution               = _appContext.pWidgetFactory->createComboBox(Core::List<String>(), 300, (padding++ * Y_PADDING)+5);
    _pCheckBoxVSync                 = _appContext.pWidgetFactory->createCheckBox(L"v-sync", 49, padding++ * Y_PADDING);
    _pCheckBoxSpecialEffects        = _appContext.pWidgetFactory->createCheckBox(L"special-effects", 49, padding++ * Y_PADDING);
    _pLabelAntiAlias                = _appContext.pWidgetFactory->createMediumLabel(L"anti-alias", 49, padding * Y_PADDING);
    _pComboAntiAlias                = _appContext.pWidgetFactory->createComboBox(antiAliasList, 300, padding++ * Y_PADDING);
    padding++;
    _pButtonApply                   = _appContext.pWidgetFactory->createSmallButton(L"apply", 24, padding * Y_PADDING);
    _pButtonBack                    = _appContext.pWidgetFactory->createSmallButton(L"back", 245, padding++ * Y_PADDING);

    _pPanelButtons->addChild(_pLabelSettings);
    _pPanelButtons->addChild(_pCheckBoxFullScreen);
    _pPanelButtons->addChild(_pCheckBoxVSync);
    _pPanelButtons->addChild(_pCheckBoxKeepAspectRatio);
    _pPanelButtons->addChild(_pLabelResolution);
    _pPanelButtons->addChild(_pComboResolution);
    _pPanelButtons->addChild(_pCheckBoxSpecialEffects);
    _pPanelButtons->addChild(_pLabelAntiAlias);
    _pPanelButtons->addChild(_pComboAntiAlias);
    _pPanelButtons->addChild(_pButtonApply);
    _pPanelButtons->addChild(_pButtonBack);

    _pPanelButtons->pack();

    _appContext.pWidgetFactory->bottomScreen(_pPanelButtons);

    _pCheckBoxFullScreen->addEventHandler(this);
    _pCheckBoxVSync->addEventHandler(this);
    _pCheckBoxKeepAspectRatio->addEventHandler(this);
    _pComboResolution->addEventHandler(this);
    _pCheckBoxSpecialEffects->addEventHandler(this);
    _pComboAntiAlias->addEventHandler(this);
    _pButtonApply->addEventHandler(this);
    _pButtonBack->addEventHandler(this);
    _pButtonBack->setHotKey(VK_ESCAPE);
    
    _pDialog->addChild(_pPanelButtons);
}

void ScreenVideoSettings::resetLabels()
{
    _pLabelSettings->setDictionary(_appContext.pDico);
    _pCheckBoxFullScreen->setDictionary(_appContext.pDico);
    _pCheckBoxKeepAspectRatio->setDictionary(_appContext.pDico);
    _pLabelResolution->setDictionary(_appContext.pDico);
    _pCheckBoxVSync->setDictionary(_appContext.pDico);
    _pCheckBoxSpecialEffects->setDictionary(_appContext.pDico);
    _pLabelAntiAlias->setDictionary(_appContext.pDico);
    _pButtonApply->setDictionary(_appContext.pDico);
    _pButtonBack->setDictionary(_appContext.pDico);

    _pLabelSettings->setText(L"video");
    _pCheckBoxFullScreen->setText(L"fullscreen");
    _pCheckBoxKeepAspectRatio->setText(L"keep-aspect-ratio");
    _pLabelResolution->setText(L"resolution");
    _pCheckBoxVSync->setText(L"v-sync");
    _pCheckBoxSpecialEffects->setText(L"special-effects");
    _pLabelAntiAlias->setText(L"anti-alias");
    _pButtonApply->setText(L"apply");
    _pButtonBack->setText(L"back");
}

void ScreenVideoSettings::startScreen()
{
    fillComponentsFromSettings();
    
    // Key nav
    _pCheckBoxFullScreen->setKeyWidgetU(_pButtonBack.get());
    _pCheckBoxFullScreen->setKeyWidgetD(_pCheckBoxKeepAspectRatio.get());

    _pCheckBoxKeepAspectRatio->setKeyWidgetU(_pCheckBoxFullScreen.get());
    _pCheckBoxKeepAspectRatio->setKeyWidgetD(_pComboResolution.get());

    _pComboResolution->setKeyWidgetU(_pCheckBoxKeepAspectRatio.get());
    _pComboResolution->setKeyWidgetD(_pCheckBoxVSync.get());

    _pCheckBoxVSync->setKeyWidgetU(_pComboResolution.get());
    _pCheckBoxVSync->setKeyWidgetD(_pCheckBoxSpecialEffects.get());

    _pCheckBoxSpecialEffects->setKeyWidgetU(_pCheckBoxVSync.get());
    _pCheckBoxSpecialEffects->setKeyWidgetD(_pComboAntiAlias.get());

    _pComboAntiAlias->setKeyWidgetU(_pCheckBoxSpecialEffects.get());
    _pComboAntiAlias->setKeyWidgetD(_pButtonApply.get());

    _pButtonApply->setKeyWidgetU(_pComboAntiAlias.get());
    _pButtonApply->setKeyWidgetR(_pButtonBack.get());

    _pButtonBack->setKeyWidgetU(_pComboAntiAlias.get());
    _pButtonBack->setKeyWidgetL(_pButtonApply.get());
    _pButtonBack->setKeyWidgetD(_pCheckBoxFullScreen.get());
    // end key nav

    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pButtonBack.get();
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _pDialog->pack();
}
    
void ScreenVideoSettings::endScreen()
{
}

void ScreenVideoSettings::update(double elapsed)
{
}

void ScreenVideoSettings::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenSettings != null);
            _pManager->setScreen(_pScreenSettings.get());
        }
        else if(pWidget == _pButtonApply.get())
        {
            applySettings();
        }
        else
        {
            if(pWidget == _pCheckBoxKeepAspectRatio.get())
            {
                switchResolutionsInCombo();
            }
        }
    }
}

void ScreenVideoSettings::fillComponentsFromSettings()
{
    //FULLSCREEN
    _pCheckBoxFullScreen->setChecked(_appContext.pGameSettings->fullscreen());
    //VSYNC
    _pCheckBoxVSync->setChecked(_appContext.pGameSettings->getRendererSettings().vsync);

    //RESOLUTIONS
    int32 screenWidth = _appContext.pGameSettings->getScreenWidth();
    int32 screenHeight = _appContext.pGameSettings->getScreenHeight();

    const Core::List<Renderer::Resolution> & allResolutions = _appContext.pRenderer->getAllResolutions();
    const Core::List<Renderer::Resolution> & prefResolutions = _appContext.pRenderer->getPreferredResolutions();

    int32 indexRes = 0;
    bool prefResolution = false;

    Core::List<Renderer::Resolution>::const_iterator iPrefResolution = prefResolutions.begin();
    while(iPrefResolution != prefResolutions.end() && !prefResolution)
    {
        if(iPrefResolution->width >= 1024)
        {
            if(screenWidth == iPrefResolution->width && screenHeight == iPrefResolution->height)
            {
                prefResolution = true;
            }
            else
                ++indexRes;
        }

        ++iPrefResolution;
    }

    if(!prefResolution)
    {
        indexRes = 0;
        bool resoFound = false;
        Core::List<Renderer::Resolution>::const_iterator iResolution = allResolutions.begin();
        while(iResolution != allResolutions.end() && !resoFound)
        {
            if(iResolution->width >= 1024)
            {
                if(screenWidth == iResolution->width && screenHeight == iResolution->height)
                {
                    resoFound = true;
                }
                else
                    ++indexRes;
            }

            ++iResolution;
        }
    }

    Core::List<String> comboItems;
    if(prefResolution)
    {
        Core::List<Renderer::Resolution>::const_iterator iPrefResolution = prefResolutions.begin();
        while(iPrefResolution != prefResolutions.end())
        {
            if(iPrefResolution->width >= 1024)
            {
                Core::String resItem;
                resItem << iPrefResolution->width << L"x" << iPrefResolution->height;
                comboItems.push_back(resItem);
            }
            ++iPrefResolution;
        }
    }
    else
    {
        Core::List<Renderer::Resolution>::const_iterator iResolution = allResolutions.begin();
        while(iResolution != allResolutions.end())
        {
            if(iResolution->width >= 1024)
            {
                Core::String resItem;
                resItem << iResolution->width << L"x" << iResolution->height;
                comboItems.push_back(resItem);
            }
            ++iResolution;
        }
    }
    _pComboResolution->setItems(comboItems);
    _pComboResolution->setIndex(indexRes);

    //KEEP ASPECT RATIO
    _pCheckBoxKeepAspectRatio->setChecked(prefResolution);

    //ANTI-ALIAS
    int32 indexAA = 0;
    switch(_appContext.pGameSettings->getRendererSettings().msaaLevel)
    {
    case Renderer::MSAA_NONE:
        indexAA = 0;
        break;
    case Renderer::MSAA_2X:
        indexAA = 1;
        break;
    case Renderer::MSAA_4X:
        indexAA = 2;
        break;
    case Renderer::MSAA_8X:
        indexAA = 3;
        break;
    }
    
    if(indexAA < _pComboAntiAlias->getItems().size())
        _pComboAntiAlias->setIndex(indexAA);
    else
        _pComboAntiAlias->setIndex(_pComboAntiAlias->getItems().size()-1);

    //SPECIAL EFFECTS
    _pCheckBoxSpecialEffects->setChecked(_appContext.pGameSettings->specialEffectsEnabled());
}

void ScreenVideoSettings::switchResolutionsInCombo()
{
    int32 indexRes = 0;
    Core::List<String> resolutions;
    bool resolutionFound = false;

    int32 screenWidth = _appContext.pGameSettings->getScreenWidth();
    int32 screenHeight = _appContext.pGameSettings->getScreenHeight();

    if(_pCheckBoxKeepAspectRatio->isChecked())
    {
        const Core::List<Renderer::Resolution> & prefResolutions = _appContext.pRenderer->getPreferredResolutions();
        Core::List<Renderer::Resolution>::const_iterator iResolution = prefResolutions.begin();
        while(iResolution != prefResolutions.end())
        {
            if(iResolution->width >= 1024)
            {
                Core::String resItem;
                resItem << iResolution->width << L"x" << iResolution->height;
                resolutions.push_back(resItem);

                if( !resolutionFound )
                {
                    if(screenWidth == iResolution->width && screenHeight == iResolution->height)
                    {
                        resolutionFound = true;
                    }
                    else
                        ++indexRes;
                }
            }

            ++iResolution;
        }
    }
    else
    {
        const Core::List<Renderer::Resolution> & allResolutions = _appContext.pRenderer->getAllResolutions();
        Core::List<Renderer::Resolution>::const_iterator iResolution = allResolutions.begin();
        while(iResolution != allResolutions.end())
        {
            if(iResolution->width >= 1024)
            {
                Core::String resItem;
                resItem << iResolution->width << L"x" << iResolution->height;
                resolutions.push_back(resItem);

                if( !resolutionFound )
                {
                    if(screenWidth == iResolution->width && screenHeight == iResolution->height)
                    {
                        resolutionFound = true;
                    }
                    else
                        ++indexRes;
                }
            }

            ++iResolution;
        }
    }

    _pComboResolution->setItems(resolutions);
    if(resolutionFound)
        _pComboResolution->setIndex(indexRes);
    else
        _pComboResolution->setIndex(0);
}

void ScreenVideoSettings::applySettings()
{
    _appContext.pGameSettings->enableSpecialEffects(_pCheckBoxSpecialEffects->isChecked());
    _appContext.pGameSettings->setFullscreen(_pCheckBoxFullScreen->isChecked());
    _appContext.pGameSettings->setVSync(_pCheckBoxVSync->isChecked());

    int32 indexAA = _pComboAntiAlias->getIndex();
    Renderer::EMSAALevel msaaLevel = Renderer::MSAA_NONE;
    switch(indexAA)
    {
    case 0:
        msaaLevel = Renderer::MSAA_NONE;
        break;
    case 1:
        msaaLevel = Renderer::MSAA_2X;
        break;
    case 2:
        msaaLevel = Renderer::MSAA_4X;
        break;
    case 3:
        msaaLevel = Renderer::MSAA_8X;
        break;
    }
    _appContext.pGameSettings->setAntiAlias(msaaLevel);

    int32 indexRes = _pComboResolution->getIndex();
    bool itemFound = false;
    int32 width = 1024;
    int32 height = 768;
    if(_pCheckBoxKeepAspectRatio->isChecked())
    {
        const Core::List<Renderer::Resolution> & prefResolutions = _appContext.pRenderer->getPreferredResolutions();

        Core::List<Renderer::Resolution>::const_iterator iResolutions = prefResolutions.begin();
        while(iResolutions != prefResolutions.end() && !itemFound)
        {
            if(iResolutions->width >= 1024)
            {
                if(indexRes == 0)
                {
                    width = iResolutions->width;
                    height= iResolutions->height;
                    itemFound = true;
                }
                else
                    indexRes--;
            }

            ++iResolutions;
        }

    }
    else
    {
        const Core::List<Renderer::Resolution> & allResolutions = _appContext.pRenderer->getAllResolutions();

        Core::List<Renderer::Resolution>::const_iterator iResolutions = allResolutions.begin();
        while(iResolutions != allResolutions.end() && !itemFound)
        {
            if(iResolutions->width >= 1024)
            {
                if(indexRes == 0)
                {
                    width = iResolutions->width;
                    height= iResolutions->height;
                    itemFound = true;
                }
                else
                    indexRes--;
            }

            ++iResolutions;
        }
    }

    _appContext.pGameSettings->setResolution(width, height);
    
    _appContext.pRenderer->setRenderSettings(_appContext.pGameSettings->getRendererSettings());
    _appContext.pRenderWindow->toggleFullscreen(_appContext.pGameSettings->getRendererSettings().fullscreen, width, height);
}

}//namespace ChronoRage
