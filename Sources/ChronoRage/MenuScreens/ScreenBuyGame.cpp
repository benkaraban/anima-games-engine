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
#include "ScreenBuyGame.h"

namespace ChronoRage
{

const float ENABLE_CONTINUE_TIMER = 20.0f;
const float SWITCH_PICTURE_TIMER = 7.0f;
const float SWITCH_PICTURE_FADE_OUT = 0.2f;
const float SWITCH_PICTURE_FADE_IN = 0.2f;

ScreenBuyGame::ScreenBuyGame(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();
    _pPanel = _appContext.pWidgetFactory->createLargePanel();

    _pButtonBuyGame = _appContext.pWidgetFactory->createMediumButton(L"buy-game", 250 - (368 / 2), 610);
    _pButtonContinue = _appContext.pWidgetFactory->createMediumButton(L"", 750 - (368 / 2), 610);
    _pButtonContinue->setDictionary(null);
    _pButtonContinue->setText((*_appContext.pDico)[L"continue"]);

    _pButtonBuyGame->addEventHandler(this);
    _pButtonContinue->addEventHandler(this);

    _pPanel->addChild(_pButtonBuyGame);
    _pPanel->addChild(_pButtonContinue);

    _pPicUpLeft     = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/DemoGUI/Screen-01.tga", 20, 40);
    _pPicDownLeft   = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/DemoGUI/Screen-02.tga", 20, 310);
    _pPicDownRight  = _appContext.pWidgetFactory->createPicture(L"/ChronoRageData/DemoGUI/Screen-03.tga", 510, 310);
    _pPanel->addChild(_pPicUpLeft);
    _pPanel->addChild(_pPicDownLeft);
    _pPanel->addChild(_pPicDownRight);

    _pLabelTitle = createLabel(L"gameplay", 750, 70);
    _pLabelReason1 = createKeyLabel(L"reason-01", 560, 110);
    _pLabelReason2 = createKeyLabel(L"reason-02", 560, 140);
    _pLabelReason3 = createKeyLabel(L"reason-03", 560, 170);
    _pLabelReason4 = createKeyLabel(L"reason-04", 560, 200);
    _pLabelReason5 = createKeyLabel(L"reason-05", 560, 230);
    _pLabelReason6 = createKeyLabel(L"reason-06", 560, 260);

    _pPanel->addChild(_pLabelTitle);
    _pPanel->addChild(_pLabelReason1);
    _pPanel->addChild(_pLabelReason2);
    _pPanel->addChild(_pLabelReason3);
    _pPanel->addChild(_pLabelReason4);
    _pPanel->addChild(_pLabelReason5);
    _pPanel->addChild(_pLabelReason6);

#ifdef CHRONORAGE_DEMO
    _pLabelScore = createLabel(L"", 500, 15);
    _pLabelScore->setDictionary(null);
    String shipDestroyed;
    shipDestroyed << (*_appContext.pDico)[L"score"] << L" : " << _appContext.shipDestroyed;
    _pLabelScore->setText(shipDestroyed);
    _pPanel->addChild(_pLabelScore);
#endif

    
    _pPanel->pack();

    _pDialog->addChild(_pPanel);
    _pDialog->pack();


}

Ptr<GUI::Label> ScreenBuyGame::createKeyLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_LEFT, GUI::VL_UP);
    pResult->setBold(false);
    pResult->setFontSize(14);
    return pResult;
}

Ptr<GUI::Label> ScreenBuyGame::createLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    pResult->setBold(false);
    pResult->setFontSize(16);
    return pResult;
}

Ptr<GUI::Label> ScreenBuyGame::createHint(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pResult = _appContext.pWidgetFactory->createMediumLabel(text, x, y);
    pResult->setSize(Core::Vector2f(0.0f, 0.0f));
    pResult->setLayouts(GUI::HL_CENTER, GUI::VL_CENTER);
    pResult->setBold(false);
    pResult->setFontSize(8);
    return pResult;
}

void ScreenBuyGame::resetLabels()
{
    _pButtonBuyGame->setDictionary(_appContext.pDico);
    _pButtonContinue->setDictionary(null);

    _pButtonBuyGame->setText(L"buy-game");
    _pButtonContinue->setText((*_appContext.pDico)[L"continue"]);

    _pLabelTitle->setText(L"gameplay");
    _pLabelReason1->setText(L"reason-01");
    _pLabelReason2->setText(L"reason-02");
    _pLabelReason3->setText(L"reason-03");
    _pLabelReason4->setText(L"reason-04");
    _pLabelReason5->setText(L"reason-05");
    _pLabelReason6->setText(L"reason-06");
}


void ScreenBuyGame::startScreen()
{
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _totalElapsed = 0.0f;
    _switchPictures = 0.0f;
    _picturesSet = 0;
    _doFadeOut = true;

    _pButtonContinue->setEnabled(false);
}
    
void ScreenBuyGame::endScreen()
{
}

void ScreenBuyGame::update(double elapsed)
{
    _totalElapsed += (float)elapsed;

    if(_totalElapsed < ENABLE_CONTINUE_TIMER)
    {
        String continueText = (*_appContext.pDico)[L"continue"];
        continueText << L" (" << int32(ENABLE_CONTINUE_TIMER+1 - _totalElapsed) << L")";
        _pButtonContinue->setText(continueText);
    }
    else
    {
        if(!_pButtonContinue->isEnabled())
        {
            _pButtonContinue->setText((*_appContext.pDico)[L"continue"]);
            _pButtonContinue->setEnabled(true);
        }
    }


    
    
    _switchPictures += (float)elapsed;
    if(_switchPictures > SWITCH_PICTURE_TIMER)
    {
        if(_doFadeOut && _switchPictures < SWITCH_PICTURE_TIMER + SWITCH_PICTURE_FADE_OUT)
        {
            _pPicUpLeft->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), SWITCH_PICTURE_FADE_OUT);
            _pPicDownLeft->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), SWITCH_PICTURE_FADE_OUT);
            _pPicDownRight->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.0f), SWITCH_PICTURE_FADE_OUT);
            _doFadeOut = false;
        }

        if(_switchPictures > SWITCH_PICTURE_TIMER + SWITCH_PICTURE_FADE_OUT)
        {
            _picturesSet = (_picturesSet+1)%3;
            String picture1;
            picture1 << L"/ChronoRageData/DemoGUI/Screen-0" << (_picturesSet*3)+1<<L".tga";
            String picture2;
            picture2 << L"/ChronoRageData/DemoGUI/Screen-0" << (_picturesSet*3)+2<<L".tga";
            String picture3;
            picture3 << L"/ChronoRageData/DemoGUI/Screen-0" << (_picturesSet*3)+3<<L".tga";

            _pPicUpLeft->setPicture(picture1);
            _pPicDownLeft->setPicture(picture2);
            _pPicDownRight->setPicture(picture3);

            _pPicUpLeft->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SWITCH_PICTURE_FADE_IN);
            _pPicDownLeft->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SWITCH_PICTURE_FADE_IN);
            _pPicDownRight->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f), SWITCH_PICTURE_FADE_IN);

            _doFadeOut = true;
            _switchPictures = 0.0f;
        }
        
    }
}

void ScreenBuyGame::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonContinue.get())
        {
            _pLastKeyFocus = _pButtonContinue.get();

            if(_pScreenNext != null)
            {
                _pManager->setScreen(_pScreenNext.get());
            }
        }
        else if(pWidget == _pButtonBuyGame.get())
        {
            _pLastKeyFocus = _pButtonBuyGame.get();

            Core::System::launchDefaultInternetNavigator(L"http://www.chronorage.com/");
        }
    }
}

}//namespace ChronoRage
