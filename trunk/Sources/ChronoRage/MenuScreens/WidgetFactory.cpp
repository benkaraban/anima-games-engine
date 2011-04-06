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
#include <Core/TGA.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

class AutoCenteredPanel : public GUI::Widget
{
public:
    AutoCenteredPanel(const Ptr<Renderer::IRenderer> & pRenderer,
                      int32 width, int32 height) : _pRenderer(pRenderer), _width(width), _height(height)
    {
        pack();
    }

    virtual void update(double elapsed)
    {
        pack();
        GUI::Widget::update(elapsed);
    }

    virtual void pack()
    {
        Ptr<Renderer::IRenderView> pView = _pRenderer->getDefaultView();
        setSize(Core::Vector2f(float(_width), float(_height)));

        int32 xpos = (pView->getWidth() - _width) / 2;
        int32 ypos = (pView->getHeight() - _height) / 2;

        setPosition(Core::Vector2f(float(xpos), float(ypos)));
    }

    virtual void startAsDialog()
    {
        pack();
    }

protected:
    Ptr<Renderer::IRenderer> _pRenderer;
    int32                    _width;
    int32                    _height;
};

WidgetFactory::WidgetFactory(   const Ptr<GUI::Manager> & pGUI, 
                                const Ptr<Window::InputManager> & pInput, 
                                const Ptr<Core::Dictionary> & pDico, 
                                const Ptr<SoundLibrary> & pSoundLibrary) 
:   _pGUI(pGUI),
    _pInput(pInput),
    _pDico(pDico),
    _pSoundLibrary(pSoundLibrary)
{
    _defaultFontName = L"Lucida Console";

    _pKeyNav = Ptr<KeyNav>(new KeyNav(pInput));
    pGUI->getKeyNavigation()->pCtrl = _pKeyNav;

    {
        const Renderer::PictureInfos & pic = _pGUI->getResPool()->getPicture(L"/ChronoRageData/GUI/menu-button.tga");
        _mediumLabelSize.x = float(pic.width);
        _mediumLabelSize.y = float(pic.height);
    }
    {
        const Renderer::PictureInfos & pic = _pGUI->getResPool()->getPicture(L"/ChronoRageData/GUI/menu-little-button.tga");
        _smallLabelSize.x = float(pic.width);
        _smallLabelSize.y = float(pic.height);
    }

    _screenFixedWidth = 1024;
    _screenFixedHeight = 768;

    _mediumTextSize = 18;
    _smallTextSize = 10;

    _smallHintSize = Core::Vector2f(256, 48);
    _smallHintTextSize = 10;
}

void WidgetFactory::setCursor(ECursor cursor)
{
    switch(cursor)
    {
    case CURSOR_ARROW:
        _pGUI->setCursor(_pCursorArrow, 2, 2);
        break;
    }
}


void WidgetFactory::setStyle(const Ptr<GUI::Button> & pButton, const String & text, int32 size, const String & picName)
{
    GUI::ButtonStyle bs;
    bs.text     = text;
    bs.fontName = _defaultFontName;
    bs.fontSize = size;
    bs.bold     = true;
    bs.italic   = false;
    bs.shadow   = true;

    GUI::ButtonState bss;
    bss.picName   = picName;
    bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bss.textColor = Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f);
    bss.posDelta  = Core::Vector2f(1.0f, 1.0f);
    bs.state[GUI::BS_PUSHED] = bss;

    bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
    bss.textColor = Core::Vector4f(0.4f, 0.9f, 1.0f, 1.0f);
    bs.state[GUI::BS_MOUSE_OVER] = bss;

    bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bss.picColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    bs.state[GUI::BS_DEFAULT] = bss;

    bss.picColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
    bss.textColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
    bs.state[GUI::BS_DISABLED] = bss;

    bss.picColor = Core::Vector4f(0.6f, 0.6f, 0.6f, 1.0f);
    bss.textColor = Core::Vector4f(0.6f, 0.6f, 0.6f, 1.0f);
    bs.state[GUI::BS_DISABLED_MOUSE_OVER] = bss;

    pButton->setDictionary(_pDico);
    pButton->setStyle(bs);
}

Ptr<GUI::Label> WidgetFactory::createMediumLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setText(text);
    pLabel->setSize(_mediumLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_mediumTextSize);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::RichTextLabel> WidgetFactory::createRichTextLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::RichTextLabel> pLabel(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setSize(_mediumLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_mediumTextSize);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    pLabel->setText(text);

    return pLabel;
}

Ptr<GUI::RichTextLabel> WidgetFactory::createRichTextHint(const String & text, int32 x, int32 y)
{
    Ptr<GUI::RichTextLabel> pLabel(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setSize(_smallHintSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_smallHintTextSize);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));
    pLabel->setOwnColor(Core::Vector4f(0.7f, 0.7f, 0.7f, 1.0f));

    pLabel->setText(text);

    return pLabel;
}

Ptr<GUI::Widget> WidgetFactory::createDialogBase()
{
    return Ptr<GUI::Widget>(new AutoCenteredPanel(_pGUI->getRenderer(), _screenFixedWidth, _screenFixedHeight));
}

void WidgetFactory::bottomScreen(const Ptr<GUI::Widget> & pWidget)
{
    int32 xpos = (_screenFixedWidth - int32(pWidget->getSize().x)) / 2;
    int32 ypos = 3*(_screenFixedHeight - int32(pWidget->getSize().y)) / 4;

    pWidget->setPosition(Core::Vector2f(float(xpos), float(ypos)));
}

void WidgetFactory::centerScreen(const Ptr<GUI::Widget> & pWidget)
{
    int32 xpos = (_screenFixedWidth - int32(pWidget->getSize().x)) / 2;
    int32 ypos = (_screenFixedHeight - int32(pWidget->getSize().y)) / 2;

    pWidget->setPosition(Core::Vector2f(float(xpos), float(ypos)));
}

Ptr<GUI::Picture> WidgetFactory::createMediumPanel(bool centered)
{
    Ptr<GUI::Picture> pResult(createMediumPanel(0, 0));
    if(centered)
        centerScreen(pResult);
    return pResult;
}

Ptr<GUI::Picture> WidgetFactory::createThinPanel(bool centered)
{
    Ptr<GUI::Picture> pResult(createThinPanel(0, 0));
    if(centered)
        centerScreen(pResult);
    return pResult;
}

Ptr<GUI::Picture> WidgetFactory::createLargePanel(bool centered)
{
    Ptr<GUI::Picture> pResult(createLargePanel(0, 0));
    if(centered)
        centerScreen(pResult);
    return pResult;
}

Ptr<GUI::Picture> WidgetFactory::createPicture(const String & name, int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(name);
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createThinPanel(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/little-menu-border.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createMediumPanel(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/menu-border.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createLargePanel(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/menu-border-big.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createThinBand(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/little-band.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createMediumBand(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/band.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createLargeBand(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/large-band.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<TextEdit> WidgetFactory::createMediumTextEdit(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/ChronoRageData/GUI/textedit-medium.tga");

    Ptr<GUI::TextEdit> pTextEdit(new GUI::TextEdit(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI->getFocus()));

    static const float padding = 5.0f;

    float xsize = pPicture->getSize().x - 2.0f * padding;
    float ysize = pPicture->getSize().y;

    pTextEdit->setSize(Core::Vector2f(xsize, ysize));
    pTextEdit->setVLayout(GUI::VL_CENTER);
    pTextEdit->setPosition(Core::Vector2f(padding, 0.0f));
    pTextEdit->setFontName(_defaultFontName);
    pTextEdit->setFontSize(_mediumTextSize);

    pPicture->addChild(pTextEdit);

    Ptr<TextEdit> pResult(new TextEdit(pTextEdit, pPicture));
    pResult->setPosition(Core::Vector2f(float(x), float(y)));

    return pResult;
}

Ptr<CRButton> WidgetFactory::createSmallButton(const String & text, int32 x, int32 y)
{
    Ptr<CRButton> pButton(new CRButton(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pSoundLibrary));
    pButton->setSize(Core::Vector2f(100.0f, 40.0f));
    setStyle(pButton, text, _mediumTextSize, L"/ChronoRageData/GUI/menu-little-button.tga");
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    pButton->setKeyNavigation(_pGUI->getKeyNavigation());
    return pButton;
}

Ptr<CRButton> WidgetFactory::createMediumButton(const String & text, int32 x, int32 y)
{
    Ptr<CRButton> pButton(new CRButton(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pSoundLibrary));
    pButton->setSize(Core::Vector2f(400.0f, 40.0f));
    setStyle(pButton, text, _mediumTextSize, L"/ChronoRageData/GUI/menu-button.tga");
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    pButton->setKeyNavigation(_pGUI->getKeyNavigation());
    return pButton;
}

Ptr<GUI::Picture> WidgetFactory::createMessageBoxMedium()
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    //pPicture->setPicture(L"/HOO/GUI/Widgets/messagebox-medium.tga");
    centerScreen(pPicture);
    return pPicture;
}

Ptr<CRCheckBox> WidgetFactory::createCheckBox(const String & text, int32 x, int32 y)
{
    Ptr<CRCheckBox> pCheckBox(new CRCheckBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pSoundLibrary));

    GUI::CheckBoxStyle style;
    style.text          = text;
    style.picChecked    = L"/ChronoRageData/GUI/checkbox-on.tga";
    style.picUnchecked  = L"/ChronoRageData/GUI/checkbox-off.tga";
    style.fontName      = _defaultFontName;
    style.fontSize      = _mediumTextSize;
    style.padding       = 10.0f;
    style.bold          = false;
    style.italic        = false;
    style.shadow        = true;

    GUI::CheckBoxState cbs;
    cbs.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    cbs.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    cbs.posDelta  = Core::Vector2f(1.0f, 1.0f);

    style.state[GUI::CBS_PUSHED] = cbs;

    cbs.posDelta  = Core::Vector2f(0.0f, 0.0f);
    style.state[GUI::CBS_MOUSE_OVER] = cbs;

    cbs.picColor  = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    style.state[GUI::CBS_DEFAULT] = cbs;

    cbs.picColor  = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
    style.state[GUI::CBS_DISABLED] = cbs;

    pCheckBox->setDictionary(_pDico);
    pCheckBox->setStyle(style);
    pCheckBox->setPosition(Core::Vector2f(float(x), float(y)));
    pCheckBox->setKeyNavigation(_pGUI->getKeyNavigation());

    return pCheckBox;
}

Ptr<CRComboBox> WidgetFactory::createComboBox(const Core::List<String> & items, int32 x, int32 y)
{
    Ptr<CRComboBox> pCombo = Ptr<CRComboBox>(new CRComboBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI, _pSoundLibrary));

    GUI::ButtonStyle bs;
    bs.text = L"";
    bs.fontName = _defaultFontName;
    bs.fontSize = _smallTextSize;
    bs.bold = false;
    bs.italic = false;
    bs.shadow = true;

    GUI::ButtonState bss;
    bss.picName   = L"/ChronoRageData/GUI/combo-button.tga";
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

    pCombo->setStyle(bs);
    pCombo->setItems(items);
    pCombo->setPosition(Core::Vector2f(float(x), float(y)));
    pCombo->setPadding(2.0f);
    pCombo->setMaxLines(5);

    bs.state[GUI::BS_DISABLED].picName = bs.state[GUI::BS_PUSHED].picName = bs.state[GUI::BS_MOUSE_OVER].picName = bs.state[GUI::BS_DEFAULT].picName = L"/ChronoRageData/GUI/combo-item.tga";
    pCombo->setItemsStyle(bs);
    pCombo->setKeyNavigation(_pGUI->getKeyNavigation());

    return pCombo;
}

Ptr<CRComboBox> WidgetFactory::createLargeComboBox(const Core::List<String> & items, int32 x, int32 y)
{
    Ptr<CRComboBox> pCombo = Ptr<CRComboBox>(new CRComboBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI, _pSoundLibrary));

    GUI::ButtonStyle bs;
    bs.text = L"";
    bs.fontName = _defaultFontName;
    bs.fontSize = _smallTextSize;
    bs.bold = false;
    bs.italic = false;
    bs.shadow = true;

    GUI::ButtonState bss;
    bss.picName   = L"/ChronoRageData/GUI/large-combo-button.tga";
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

    pCombo->setStyle(bs);
    pCombo->setItems(items);
    pCombo->setPosition(Core::Vector2f(float(x), float(y)));
    pCombo->setPadding(2.0f);
    pCombo->setMaxLines(5);

    bs.state[GUI::BS_DISABLED].picName = bs.state[GUI::BS_PUSHED].picName = bs.state[GUI::BS_MOUSE_OVER].picName = bs.state[GUI::BS_DEFAULT].picName = L"/ChronoRageData/GUI/large-combo-item.tga";
    pCombo->setItemsStyle(bs);
    pCombo->setKeyNavigation(_pGUI->getKeyNavigation());

    return pCombo;
}

Ptr<GUI::HScrollBar> WidgetFactory::createHScrollBar(int32 x, int32 y)
{
    Ptr<GUI::HScrollBar> pHScroll = Ptr<GUI::HScrollBar>(new GUI::HScrollBar(_pGUI->getResPool(), _pGUI->getHUD(), _pGUI->getInput()));
    pHScroll->setBarPicture(L"/ChronoRageData/GUI/hscroll-cursor.tga");
    pHScroll->setBackgroundPicture(L"/ChronoRageData/GUI/hscroll-bar.tga");
    pHScroll->setSize(Core::Vector2f(200.0f, 21.0f));
    pHScroll->setPosition(Core::Vector2f(float(x), float(y)));
    pHScroll->setBackgroundColor(Core::Vector4f(0.0f, 0.0f, 0.0f, 0.5f));
    pHScroll->setBarColor(Core::Vector4f(1.0f, 1.0f, 1.0f, 0.5f));
    pHScroll->setRange(0, 200, 12); // 200 = size bar, 21 = size cursor
    pHScroll->setKeyNavigation(_pGUI->getKeyNavigation());

    return pHScroll;
}

}//namespace ChronoRage