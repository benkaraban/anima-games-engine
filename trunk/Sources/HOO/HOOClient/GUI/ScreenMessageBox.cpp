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
#include <HOOClient/GUI/ScreenMessageBox.h>

ScreenMessageBox::ScreenMessageBox(const Ptr<WidgetFactory> & pWidgets, const String & message, EMessageBoxType boxType, bool pushDialog) :
    _boxType(boxType),
    _result(MSGBOX_RESULT_OK),
    _pNextScreenOK(NULL),
    _pushDialog(pushDialog)
{
    _pWidgets = pWidgets;
    _pDialog = pWidgets->createDialogBase();
    _pPanel = pWidgets->createMessageBoxMedium();

    _pLabel = pWidgets->createLargeRTLabel(L"connection-in-progress", 5, 5, 35);
    _pLabel->setSize(Core::Vector2f(535.0f, 100.0f));
    _pLabel->setHLayout(GUI::HL_CENTER);
    _pLabel->setVLayout(GUI::VL_CENTER);
    _pLabel->setAutoCenterVContent(true);

    _pPanelButtons = Ptr<GUI::Widget>(new GUI::Widget());

    switch(boxType)
    {
    case MSGBOX_OK:
    {
        _pButtonOK = pWidgets->createMediumButton(L"ok", 0, 0);
        _pButtonOK->setHotKey(VK_RETURN);
        _pButtonOK->addEventHandler(this);

        _pPanelButtons->addChild(_pButtonOK);
        break;
    }
    case MSGBOX_RETRY_CANCEL:
    {
        _pButtonOK = pWidgets->createMediumButton(L"retry", 0, 0);
        _pButtonOK->setHotKey(VK_RETURN);
        _pButtonOK->addEventHandler(this);

        _pButtonCancel = pWidgets->createMediumButton(L"cancel", 200, 0);
        _pButtonCancel->setHotKey(VK_ESCAPE);
        _pButtonCancel->addEventHandler(this);

        _pPanelButtons->addChild(_pButtonOK);
        _pPanelButtons->addChild(_pButtonCancel);
        break;
    }
    }

    // hcenter buttons

    _pPanelButtons->pack();
    float xsize = _pPanelButtons->getSize().x;
    float x = 0.5f * (_pPanel->getSize().x - xsize);
    _pPanelButtons->setPosition(Core::Vector2f(x, 110.0f));

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabel);
    _pPanel->addChild(_pPanelButtons);
}

void ScreenMessageBox::startScreen()
{
    if(_pushDialog)
        _pWidgets->getManager()->pushDialog(_pDialog);
    else
        _pWidgets->getManager()->setDialog(_pDialog);
    _pDialog->pack();
}

void ScreenMessageBox::endScreen()
{
}

void ScreenMessageBox::update(double elapsed)
{
    if(_boxType == MSGBOX_RESULT_OK)
    {
        if(_pWidgets->getManager()->getInput()->isKeyTyped(VK_ESCAPE))
            onEvent(GUI::EV_COMMAND, _pButtonOK.get());
    }
}

void ScreenMessageBox::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonOK.get())
        {
            _result = MSGBOX_RESULT_OK;
            _pManager->setScreen(_pNextScreenOK);
        }
        else if(pWidget == _pButtonCancel.get())
        {
            _result = MSGBOX_RESULT_CANCEL;
            _pManager->setScreen(_pNextScreenCancel);
        }
    }
}

