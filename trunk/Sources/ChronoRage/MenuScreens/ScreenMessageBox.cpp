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
#include "ScreenMessageBox.h"

namespace ChronoRage
{

ScreenMessageBox::ScreenMessageBox(AppContext & appContext) 
:   _appContext(appContext), 
    _result(MSGBOX_RESULT_RESUME)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();
    _pPanel = _appContext.pWidgetFactory->createMessageBoxMedium();

    _pLabel = _appContext.pWidgetFactory->createMediumLabel(L"game-paused", 5, 5);
    _pLabel->setSize(Core::Vector2f(535.0f, 100.0f));
    _pLabel->setHLayout(GUI::HL_CENTER);
    _pLabel->setVLayout(GUI::VL_CENTER);

    _pPanelButtons = Ptr<GUI::Widget>(new GUI::Widget());

    _pButtonResume = _appContext.pWidgetFactory->createMediumButton(L"resume", 0, 0);
    _pButtonResume->setHotKey(VK_ESCAPE);
    _pButtonResume->addEventHandler(this);

    _pButtonRetry = _appContext.pWidgetFactory->createMediumButton(L"retry", 100, 0);
    _pButtonRetry->addEventHandler(this);

    _pButtonQuit = _appContext.pWidgetFactory->createMediumButton(L"quit", 200, 0);
    _pButtonQuit->addEventHandler(this);

    _pPanelButtons->addChild(_pButtonResume);
    _pPanelButtons->addChild(_pButtonRetry);
    _pPanelButtons->addChild(_pButtonQuit);
        
    // hcenter buttons

    _pPanelButtons->pack();
    float xsize = _pPanelButtons->getSize().x;
    float x = 0.5f * (_pPanel->getSize().x - xsize);
    _pPanelButtons->setPosition(Core::Vector2f(x, 110.0f));

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabel);
    _pPanel->addChild(_pPanelButtons);
}

void ScreenMessageBox::resetLabels()
{
    _pLabel->setDictionary(_appContext.pDico);
    _pButtonResume->setDictionary(_appContext.pDico);
    _pButtonRetry->setDictionary(_appContext.pDico);
    _pButtonQuit->setDictionary(_appContext.pDico);

    _pLabel->setText(L"game-paused");
    _pButtonResume->setText(L"resume");
    _pButtonRetry->setText(L"retry");
    _pButtonQuit->setText(L"quit");
}

void ScreenMessageBox::startScreen()
{
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);

    _pDialog->pack();
}

void ScreenMessageBox::endScreen()
{
}

void ScreenMessageBox::update(double elapsed)
{
}

void ScreenMessageBox::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonResume.get())
        {
            _result = MSGBOX_RESULT_RESUME;
        }
        else if(pWidget == _pButtonRetry.get())
        {
            _result = MSGBOX_RESULT_RETRY;
        }
        else if(pWidget == _pButtonQuit.get())
        {
            _result = MSGBOX_RESULT_QUIT;
        }
    }
}

}//namespace ChronoRage