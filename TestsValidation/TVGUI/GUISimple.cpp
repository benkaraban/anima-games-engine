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
#include <Core/Logger.h>

#include <Packing/PicLoaderOnTheFly.h>
#include <Packing/TexCompressionOnTheFly.h>

#include <GUI/RichTextLabel.h>
#include <GUI/Label.h>
#include <GUI/BitmapLabel.h>
#include <GUI/Picture.h>
#include <GUI/TextEdit.h>
#include <GUI/Button.h>
#include <GUI/ComboBox.h>

#include "GUISimple.h"
#include <math.h>

using namespace Core;
using namespace Universe;

static const int32 ID_BUTTON_1      = 1;
static const int32 ID_BUTTON_2      = 2;
static const int32 ID_BUTTON_OK     = 3;
static const int32 ID_BUTTON_CANCEL = 4;
static const int32 ID_BUTTON_START  = 5;
static const int32 ID_COMBO         = 6;

static const wchar_t RICH_TEXT [] = L""
L"{font Arial}So much {b}amazingness{/b}, so {i}little{/i} {color 1.0 0.0 0.0 1.0}time{/color} to see it {size 8}all{/size}.{/font}{n}{n}"
L"{font Consolas}{size 10}Oh my {img /TUDataRaw/GUI/img-plus.tga} god {img /TUDataRaw/GUI/img-plus.tga} this is really {color 00FFFF}amazing{/color} I just can't {img /TUDataRaw/GUI/img-plus.tga} stand it!{/size}{/font}{n}{n}"
L"{align right}{size 10}Really {size 8}this{/size} kicks some {size 12}serious{/size} asses, I bet you could charge millions of dollars for it.{/size}{/align}{n}"
L"{align center}Carmack should {b}better{/b} stay quiet because we're gonna rock hard with it!{/align}";

//-----------------------------------------------------------------------------
void GUISimple::initalise(const Ptr<Renderer::IRenderer> & pRenderer, const Ptr<Window::Window> & pRenderWindow, const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::InputManager> & pInput)
{
    _pRenderer = pRenderer;
    _pVFS = pVFS;
    _pWorld = Ptr<Universe::World>(new Universe::World(pRenderer, pVFS));
    _pInput = pInput;

    Ptr<Universe::IOnTheFlyTextureGenerator> pTexGen(new Packing::OnTheFlyTextureGenerator());
    _pWorld ->getRessourcesPool()->setOnTheFlyTextureGenerator(pTexGen);

    Ptr<GUI::IOnTheFlyPictureLoader> pLoader(new Packing::OnTheFlyPictureGenerator(pVFS, pRenderer));

    _pGUI = Ptr<GUI::Manager>(new GUI::Manager(pVFS, pRenderWindow, pInput, pRenderer, _pWorld->getRessourcesPool()));
    _pGUI->getResPool()->setOnTheFlyPictureLoader(pLoader);

    Ptr<GUI::Picture> pPanel1(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPanel1->setPicture(L"/TUDataRaw/GUI/panel-00.tga");
    pPanel1->setPosition(Core::Vector2f(100.0f, 100.0f));

    Ptr<GUI::Button> pTextButton1(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    Ptr<GUI::Button> pTextButton2(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    {
        pTextButton1->setPosition(Core::Vector2f(90.0f, 55.0f));
        pTextButton1->setSize(Core::Vector2f(110.0, 20.0f));

        pTextButton2->setPosition(Core::Vector2f(90.0f, 75.0f));
        pTextButton2->setSize(pTextButton1->getSize());

        GUI::ButtonStyle bs;
        bs.text = L"Start game";
        bs.fontName = L"Calibri";
        bs.fontSize = 12;
        bs.bold = false;
        bs.italic = false;
        bs.shadow = true;

        GUI::ButtonState bss;
        bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 0.0f);
        bss.textColor = Core::Vector4f(1.0f, 0.5f, 0.5f, 1.0f);
        bss.posDelta  = Core::Vector2f(1.0f, 1.0f);

        bs.state[GUI::BS_PUSHED] = bss;

        bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
        bs.state[GUI::BS_MOUSE_OVER] = bss;

        bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        bs.state[GUI::BS_DEFAULT] = bss;

        bss.textColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        bs.state[GUI::BS_DISABLED] = bss;

        pTextButton1->setStyle(bs);
        pTextButton1->addEventHandler(this);
        pTextButton1->setId(ID_BUTTON_START);

        bs.text = L"Load game";
        //pTextButton2->setEnabled(false);
        pTextButton2->setStyle(bs);
    }

    Ptr<GUI::Button> pButton1(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    Ptr<GUI::Button> pButton2(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    {
        GUI::ButtonStyle bs;
        bs.text = L"Push me";
        bs.fontName = L"Calibri";
        bs.fontSize = 12;
        bs.bold = false;
        bs.italic = false;
        bs.shadow = true;

        GUI::ButtonState bss;
        bss.picName   = L"/TUDataRaw/GUI/button-00.png";
        bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        bss.posDelta  = Core::Vector2f(2.0f, 2.0f);

        bs.state[GUI::BS_PUSHED] = bss;

        bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
        bs.state[GUI::BS_MOUSE_OVER] = bss;

        bss.picColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        bs.state[GUI::BS_DEFAULT] = bss;

        bss.picColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
        bss.textColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        bs.state[GUI::BS_DISABLED] = bss;

        pButton1->setStyle(bs);

        bs.text = L"I'm disabled";
        pButton2->setEnabled(false);
        pButton2->setStyle(bs);

        pButton1->setPosition(Core::Vector2f(30.0f, 120.0f));
        pButton2->setPosition(Core::Vector2f(30.0f, 170.0f));

        pButton1->setSize(Core::Vector2f(230.0, 40.0f));
        pButton2->setSize(Core::Vector2f(230.0, 40.0f));
        
        pButton1->addEventHandler(this);
        pButton2->addEventHandler(this);

        _pButton1 = pButton1;
        _pButton2 = pButton2;

        _pButton1->setId(ID_BUTTON_1);
        _pButton2->setId(ID_BUTTON_2);
    }

    Ptr<GUI::Picture> pPicTextArea(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicTextArea->setSize(Core::Vector2f(100.0f, 20.0f));
    pPicTextArea->setPosition(Core::Vector2f(330.0f, 30.0f));
    pPicTextArea->setPictureColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));

    Ptr<GUI::TextEdit> pTextEdit(new GUI::TextEdit(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI->getFocus()));
    pTextEdit->setSize(Core::Vector2f(90.0f, 20.0f));
    pTextEdit->setPosition(Core::Vector2f(5.0f, 0.0f));
    pTextEdit->setVLayout(GUI::VL_CENTER);
    pTextEdit->setText(L"This is a test!");
    pTextEdit->setMaxLength(40);
    pTextEdit->setPassword(false);

    pPicTextArea->addChild(pTextEdit);

    Ptr<GUI::BitmapLabel> pBitmapLabel1(new GUI::BitmapLabel(_pGUI->getResPool(), _pGUI->getHUD()));

    pBitmapLabel1->setFont(L"/TUDataRaw/GUI/font-counter-00.font.xml");
    pBitmapLabel1->setText(L"0123456789");
    pBitmapLabel1->setPosition(Core::Vector2f(30.0f, -30.0f));
    pBitmapLabel1->setSize(Core::Vector2f(230.0f, 16.0f));

    Ptr<GUI::RichTextLabel> pRichText(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pRichText->setPosition(Core::Vector2f(400.0f, 100.0f));
    pRichText->setSize(Core::Vector2f(150.0f, 200.0f));
    pRichText->setText(RICH_TEXT);

    Ptr<GUI::Picture> pPicRichText(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicRichText->setSize(pRichText->getSize());
    pPicRichText->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));

    pRichText->addChild(pPicRichText);

    // Combo box
    _pCombo = Ptr<GUI::ComboBox>(new GUI::ComboBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI));
    {
        GUI::ButtonStyle bs;
        bs.text = L"";
        bs.fontName = L"Calibri";
        bs.fontSize = 12;
        bs.bold = false;
        bs.italic = false;
        bs.shadow = true;

        GUI::ButtonState bss;
        bss.picName   = L"/TUDataRaw/GUI/combo-button.tga";
        bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        bss.posDelta  = Core::Vector2f(2.0f, 2.0f);

        bs.state[GUI::BS_PUSHED] = bss;

        bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
        bs.state[GUI::BS_MOUSE_OVER] = bss;

        bss.picColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        bs.state[GUI::BS_DEFAULT] = bss;

        bss.picColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
        bss.textColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        bs.state[GUI::BS_DISABLED] = bss;

        Core::List<String> items;
        items.push_back(L"Very low");
        items.push_back(L"Low");
        items.push_back(L"Medium");
        items.push_back(L"High");
        items.push_back(L"Very high");

        _pCombo->setStyle(bs);
        _pCombo->setItems(items);
        _pCombo->setPosition(Core::Vector2f(400.0f, 250.0f));
        _pCombo->setIndex(2);
        _pCombo->setId(ID_COMBO);
        _pCombo->addEventHandler(this);
        _pCombo->setPadding(2.0f);
        _pCombo->setMaxLines(3);

        bs.state[GUI::BS_DISABLED].picName = bs.state[GUI::BS_PUSHED].picName = bs.state[GUI::BS_MOUSE_OVER].picName = bs.state[GUI::BS_DEFAULT].picName = L"/TUDataRaw/GUI/combo-item.tga";
        _pCombo->setItemsStyle(bs);
    }

    Ptr<GUI::Label> pLabel1(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));

    pLabel1->setText(L"This GUI kicks ass");
    pLabel1->setPosition(Core::Vector2f(30.0f, 30.0f));
    pLabel1->setSize(Core::Vector2f(230.0, 40.0f));
    pLabel1->setHLayout(GUI::HL_RIGHT);

    //pPanel1->setOwnColor(Core::Vector4f(1.0f, 1.0f, 0.0f, 0.25f));

    pPanel1->addChild(pBitmapLabel1);
    pPanel1->addChild(pLabel1);
    pPanel1->addChild(pTextButton1);
    pPanel1->addChild(pTextButton2);
    pPanel1->addChild(pButton1);
    pPanel1->addChild(pButton2);
    pPanel1->addChild(_pCombo);

    _pDialog1 = Ptr<GUI::Widget>(new GUI::Widget());
    _pDialog1->addChild(pPanel1);
    _pDialog1->addChild(pPicTextArea);
    _pDialog1->addChild(pRichText);

    _pGUI->pushDialog(_pDialog1);
    _pGUI->popDialog();
    _pGUI->pushDialog(_pDialog1);


    // Modal
    {
        _pModalDialog1 = Ptr<GUI::Widget>(new GUI::Widget());

        Ptr<GUI::Picture> pPanel(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
        pPanel->setPicture(L"/TUDataRaw/GUI/panel.tga");
        pPanel->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

        Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
        pLabel->setHLayout(GUI::HL_CENTER);
        pLabel->setSize(pPanel->getSize());
        pLabel->setPosition(Core::Vector2f(0.0f, 20.0f));
        pLabel->setText(L"Actually you may have to wait a few months before starting a game.");

        _pButtonOK = Ptr<GUI::Button>(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
        _pButtonCancel = Ptr<GUI::Button>(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
        {
            GUI::ButtonStyle bs;
            bs.text = L"OK";
            bs.fontName = L"Calibri";
            bs.fontSize = 12;
            bs.bold = false;
            bs.italic = false;
            bs.shadow = true;

            GUI::ButtonState bss;
            bss.picName   = L"/TUDataRaw/GUI/button-00.png";
            bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
            bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
            bss.posDelta  = Core::Vector2f(2.0f, 2.0f);

            bs.state[GUI::BS_PUSHED] = bss;

            bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
            bs.state[GUI::BS_MOUSE_OVER] = bss;

            bss.picColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
            bs.state[GUI::BS_DEFAULT] = bss;

            bss.picColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
            bss.textColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
            bs.state[GUI::BS_DISABLED] = bss;

            _pButtonOK->setStyle(bs);

            bs.text = L"Cancel";
            _pButtonCancel->setStyle(bs);

            _pButtonOK->setPosition(Core::Vector2f(90.0f, 60.0f));
            _pButtonCancel->setPosition(Core::Vector2f(260.0f, 60.0f));

            _pButtonOK->setSize(Core::Vector2f(230.0, 40.0f));
            _pButtonCancel->setSize(Core::Vector2f(230.0, 40.0f));
            
            _pButtonOK->addEventHandler(this);
            _pButtonCancel->addEventHandler(this);

            _pButtonOK->setId(ID_BUTTON_OK);
            _pButtonCancel->setId(ID_BUTTON_CANCEL);
        }

        pPanel->addChild(pLabel);
        _pModalDialog1->addChild(_pButtonOK);
        _pModalDialog1->addChild(_pButtonCancel);
        _pModalDialog1->addChild(pPanel);
    }

    // Ciel

    _pWorld->setSkyTexture(L"/TUData/Textures/FullskiesSunset0024_1_S.tex");
    _pWorld->setSkyColor(Core::Vector4f(1.0, 1.0, 1.0, 0.0));
    _pWorld->setSkyHorizon(-0.25);

    // Fog

    _pWorld->setFogColor(Core::Vector4f(0.15f, 0.1f, 0.1f, 0.0f));
    _pWorld->setFogRange(75.0, 500.0);
    _pWorld->setFogSkyOpacity(0.0);

    // Teapot

    _pMesh1 = _pWorld->createMesh(L"/TUData/Models/teapot.mdl", L"Teapot01");

    // Caméra

    _camQuake.setPosition(Core::Vector3f(0.0, 0.0, -600.0));
    _pCamera = _pWorld->createPerspectiveCamera(f_PI * 0.25f, 1.0, 10.0, 10000.0);
}
//------------------------------------------------------------------------------
void GUISimple::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    INF << L"Received event " << GUI::toString(ev) << L" from widget ID " << Core::toString(pWidget) << L"\n";

    if(ev == GUI::EV_COMMAND)
    {
        switch(pWidget->getId())
        {
        case ID_BUTTON_1:
        {
            _pDialog1->setOwnColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
            _pButton2->setText(L"I'm ready!");
            _pButton2->setEnabled(true);
            break;
        }
        case ID_BUTTON_2:
        {
            _pDialog1->setOwnColor(Core::Vector4f(1.0f, 0.0f, 0.0f, 0.25f), 4.0f);
            _pButton2->setText(L"Disabled again...");
            _pButton2->setEnabled(false);
            break;
        }
        case ID_BUTTON_START:
        {
            _pModalDialog1->pack();
            _pGUI->pushDialog(_pModalDialog1);
            _pGUI->centerScreen(_pModalDialog1);
            break;
        }
        case ID_BUTTON_OK:
        case ID_BUTTON_CANCEL:
        {
            _pGUI->popDialog();
            break;
        }
        case ID_COMBO:
        {
            INF << L"Combo selection " << Core::toString(_pCombo->getItems()[_pCombo->getIndex()]) << L"\n";
            break;
        }
        }
    }
}
//------------------------------------------------------------------------------
void GUISimple::update(double elapsed)
{
    ITestGUI::update(elapsed);

    _pGUI->update(elapsed);
}
//-----------------------------------------------------------------------------
void GUISimple::render(const Ptr<Renderer::IRenderView> & pView)
{
    ITestGUI::render(pView);

    _pGUI->render(pView);
}
//-----------------------------------------------------------------------------
