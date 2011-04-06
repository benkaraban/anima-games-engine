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

#include <GUI/CheckBox.h>
#include <GUI/Label.h>
#include <GUI/BitmapLabel.h>
#include <GUI/Picture.h>
#include <GUI/TextEdit.h>
#include <GUI/Button.h>

#include "ScrollBarTest.h"
#include <math.h>

using namespace Core;
using namespace Universe;

static const int32 ID_BUTTON_UP     = 1;
static const int32 ID_BUTTON_DOWN   = 2;
static const int32 ID_VSCROLL       = 3;

static const wchar_t RICH_TEXT [] = L""
L"{font Arial}So much {b}amazingness{/b}, so {i}little{/i} {color 1.0 0.0 0.0 1.0}time{/color} to see it {size 8}all{/size}.{/font}{n}{n}"
L"{font Consolas}{size 10}Oh my {img /TUDataRaw/GUI/img-plus.tga} god {img /TUDataRaw/GUI/img-plus.tga} this is really {color 00FFFF}amazing{/color} I just can't {img /TUDataRaw/GUI/img-plus.tga} stand it!{/size}{/font}{n}{n}"
L"{align right}{size 10}Really {size 8}this{/size} kicks some {size 12}serious{/size} asses, I bet you could charge millions of dollars for it.{/size}{/align}{n}"
L"{font Arial}So much {b}amazingness{/b}, so {i}little{/i} {color 1.0 0.0 0.0 1.0}time{/color} to see it {size 8}all{/size}.{/font}{n}{n}"
L"{font Consolas}{size 10}Oh my {img /TUDataRaw/GUI/img-plus.tga} god {img /TUDataRaw/GUI/img-plus.tga} this is really {color 00FFFF}amazing{/color} I just can't {img /TUDataRaw/GUI/img-plus.tga} stand it!{/size}{/font}{n}{n}"
L"{align right}{size 10}Really {size 8}this{/size} kicks some {size 12}serious{/size} asses, I bet you could charge millions of dollars for it.{/size}{/align}{n}"
L"{align center}Carmack should {b}better{/b} stay quiet because we're gonna rock hard with it!{/align}";

//-----------------------------------------------------------------------------
void GUIScrollBar::initalise(const Ptr<Renderer::IRenderer> & pRenderer, const Ptr<Window::Window> & pRenderWindow, const Ptr<Core::VirtualFileSystem> & pVFS, const Ptr<Window::InputManager> & pInput)
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

    _pButtonUp = Ptr<GUI::Button>(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    _pButtonDown = Ptr<GUI::Button>(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    {
        {
            GUI::ButtonStyle bs;
            GUI::ButtonState bss;
            bss.picName   = L"/TUDataRaw/GUI/scroll-up.tga";
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

            _pButtonUp->setStyle(bs);
        }
        {
            GUI::ButtonStyle bs;
            GUI::ButtonState bss;
            bss.picName   = L"/TUDataRaw/GUI/scroll-down.tga";
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

            _pButtonDown->setStyle(bs);
        }

        _pButtonUp->setPosition(Core::Vector2f(150.0f, 0.0f));
        _pButtonDown->setPosition(Core::Vector2f(150.0f, 170.0f));

        _pButtonUp->addEventHandler(this);
        _pButtonDown->addEventHandler(this);

        _pButtonUp->setId(ID_BUTTON_UP);
        _pButtonDown->setId(ID_BUTTON_DOWN);
    }

    _pRichText = Ptr<GUI::RichTextLabel>(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    _pRichText->setPosition(Core::Vector2f(100.0f, 100.0f));
    _pRichText->setSize(Core::Vector2f(150.0f, 200.0f));
    _pRichText->setText(RICH_TEXT);

    Ptr<GUI::Picture> pPicRichText(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicRichText->setSize(_pRichText->getSize());
    pPicRichText->setOwnColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));

    _pScroll = Ptr<GUI::VScrollBar>(new GUI::VScrollBar(_pGUI->getResPool(), _pGUI->getHUD(), _pGUI->getInput()));
    _pScroll->setBarPicture(L"/TUDataRaw/GUI/vscroll.tga");
    _pScroll->setBackgroundPicture(L"/TUDataRaw/GUI/vscroll.tga");
    _pScroll->setPosition(Core::Vector2f(150.0f, 32.0f));
    _pScroll->setSize(Core::Vector2f(32.0f, 140.0f));
    _pScroll->setBackgroundColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));
    _pScroll->setBarColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
    _pScroll->setRange(0, _pRichText->getLineCount(), _pRichText->getVisibleLineCount());
    _pScroll->addEventHandler(this);
    _pScroll->setId(ID_VSCROLL);

    _pHScroll = Ptr<GUI::HScrollBar>(new GUI::HScrollBar(_pGUI->getResPool(), _pGUI->getHUD(), _pGUI->getInput()));
    _pHScroll->setBarPicture(L"/TUDataRaw/GUI/hscroll-cursor.tga");
    _pHScroll->setBackgroundPicture(L"/TUDataRaw/GUI/hscroll-bar.tga");
    _pHScroll->setPosition(Core::Vector2f(250.0f, 32.0f));
    _pHScroll->setSize(Core::Vector2f(200.0f, 21.0f));
    _pHScroll->setBackgroundColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));
    _pHScroll->setBarColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
    _pHScroll->setRange(0, 200, 21); // 200 = size bar, 21 = size cursor

    _pRichText->addChild(pPicRichText);
    _pRichText->addChild(_pButtonUp);
    _pRichText->addChild(_pButtonDown);
    _pRichText->addChild(_pScroll);
    _pRichText->addChild(_pHScroll);

    _pStretch = Ptr<GUI::StretchBar>(new GUI::StretchBar(_pGUI->getResPool(), _pGUI->getHUD(), _pGUI->getInput()));
    {
        for(int32 ii=0; ii < 8; ii++)
        {
            Ptr<GUI::Picture> pPic(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
            pPic->setPicture(L"/TUDataRaw/GUI/button-00.png");
            _pStretch->addChild(pPic);
        }
    }
    _pStretch->setPosition(Core::Vector2f(350.0f, 400.0f));
    _pStretch->setSize(Core::Vector2f(600.0f, 100.0f));

    // Checkbox

    Ptr<GUI::CheckBox> pCheckBox(new GUI::CheckBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    {
        GUI::CheckBoxStyle style;
        style.text          = L"Amazingness On";
        style.picChecked    = L"/TUDataRaw/GUI/checkbox-on.tga";
        style.picUnchecked  = L"/TUDataRaw/GUI/checkbox-off.tga";

        GUI::CheckBoxState cbs;
        cbs.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        cbs.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
        cbs.posDelta  = Core::Vector2f(1.0f, 1.0f);

        style.state[GUI::CBS_PUSHED] = cbs;

        cbs.posDelta  = Core::Vector2f(0.0f, 0.0f);
        style.state[GUI::CBS_MOUSE_OVER] = cbs;

        cbs.picColor  = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
        style.state[GUI::CBS_DEFAULT] = cbs;

        cbs.picColor  = Core::Vector4f(0.8f, 0.4f, 0.4f, 1.0f);
        style.state[GUI::CBS_DISABLED] = cbs;

        pCheckBox->setStyle(style);
        pCheckBox->setPosition(Core::Vector2f(500.0f, 200.0f));
    }

    {
        _pProgress = Ptr<GUI::ProgressBar>(new GUI::ProgressBar(_pGUI->getResPool(), _pGUI->getHUD()));
        _pProgress->setBarInterior(L"/TUDataRaw/GUI/progressbar-inside.tga");
        _pProgress->setBarBorder(L"/TUDataRaw/GUI/progressbar-border.tga");
        _pProgress->setPosition(Core::Vector2f(500.0f, 230.0f));
        //_pProgress->setOrient(GUI::PROGRESSBAR_RIGHT_TO_LEFT);
    }

    //_pStretch->setId(ID_VSCROLL);

    _pDialog1 = Ptr<GUI::Widget>(new GUI::Widget());
    _pDialog1->addChild(_pRichText);
    _pDialog1->addChild(_pStretch);
    _pDialog1->addChild(pCheckBox);
    _pDialog1->addChild(_pProgress);

    _pGUI->pushDialog(_pDialog1);
    _pGUI->popDialog();
    _pGUI->pushDialog(_pDialog1);


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
void GUIScrollBar::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    INF << L"Received event " << GUI::toString(ev) << L" from widget ID " << Core::toString(pWidget) << L"\n";

    switch(pWidget->getId())
    {
    case ID_BUTTON_UP:
    {
        if(ev == GUI::EV_COMMAND)
        {
            _pRichText->setFirstLine(_pRichText->getFirstLine() - 1);
            _pScroll->setValue(_pRichText->getFirstLine());
        }
        break;
    }
    case ID_BUTTON_DOWN:
    {
        if(ev == GUI::EV_COMMAND)
        {
            _pRichText->setFirstLine(_pRichText->getFirstLine() + 1);
            _pScroll->setValue(_pRichText->getFirstLine());
        }
        break;
    }
    case ID_VSCROLL:
    {
        _pRichText->setFirstLine(_pScroll->getValue());
        break;
    }
    }
}
//------------------------------------------------------------------------------
void GUIScrollBar::update(double elapsed)
{
    ITestGUI::update(elapsed);

    _progress = fmod(_progress + float(elapsed) * 0.2f, 1.0f);

    Core::Vector4f col1(1.0f, 0.2f, 0.2f, 0.8f);
    Core::Vector4f col2(0.0f, 1.0f, 0.0f, 0.8f);
    Core::Vector4f col3(col1, col2, _progress);

    Core::Vector4f colA(0.5f, 0.5f, 0.5f, 0.8f);
    Core::Vector4f colB(1.0f, 1.0f, 1.0f, 0.8f);
    Core::Vector4f colC(colA, colB, _progress);

    _pProgress->setProgress(_progress);
    _pProgress->setOwnColor(col3);
    _pProgress->setBorderColor(colC);

    _pGUI->update(elapsed);
}
//-----------------------------------------------------------------------------
void GUIScrollBar::render(const Ptr<Renderer::IRenderView> & pView)
{
    ITestGUI::render(pView);

    _pGUI->render(pView);
}
//-----------------------------------------------------------------------------
