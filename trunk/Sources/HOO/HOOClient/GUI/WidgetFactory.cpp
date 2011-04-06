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
#include <HOOClient/GUI/WidgetFactory.h>

LM_ENUM_3(EAutoCenterMode, PANEL_CENTER, PANEL_TOP, PANEL_BOTTOM);

class AutoCenteredPanel : public GUI::Widget
{
public:
    AutoCenteredPanel(const Ptr<Renderer::IRenderer> & pRenderer,
                      EAutoCenterMode mode,
                      int32 width, int32 height) : _pRenderer(pRenderer), _mode(mode), _width(width), _height(height)
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
        int32 ypos = 0;

        switch(_mode)
        {
        case PANEL_TOP:    ypos = 0; break;
        case PANEL_CENTER: ypos = (pView->getHeight() - _height) / 2; break;
        case PANEL_BOTTOM: ypos = (pView->getHeight() - _height); break;
        }

        setPosition(Core::Vector2f(float(xpos), float(ypos)));
    }

protected:
    Ptr<Renderer::IRenderer> _pRenderer;
    EAutoCenterMode          _mode;
    int32                    _width;
    int32                    _height;
};

WidgetFactory::WidgetFactory(const Ptr<GUI::Manager> & pGUI, const Ptr<Window::InputManager> & pInput, const Ptr<Core::Dictionary> & pDico) :
    _pGUI(pGUI),
    _pInput(pInput),
    _pDico(pDico)
{
    _pGUI->getResPool()->mountPictureBank(L"/HOO/GUI/Widgets/", L"/HOO/GUI/Widgets.pbk");
    _pGUI->getResPool()->mountPictureBank(L"/HOO/GUI/LevelsPreview/", L"/HOO/GUI/LevelsPreview.pbk");
    _pGUI->getResPool()->mountPictureBank(L"/HOO/GUI/Spells/", L"/HOO/GUI/Spells.pbk");

    _defaultFontName = L"Segoe UI"; // TODO => Arial si pas install�e

    {
        const Renderer::PictureInfos & pic = _pGUI->getResPool()->getPicture(L"/HOO/GUI/Widgets/button-large.tga");
        _largeLabelSize.x = float(pic.width);
        _largeLabelSize.y = float(pic.height);
        _titleLabelSize = _largeLabelSize;
    }
    {
        const Renderer::PictureInfos & pic = _pGUI->getResPool()->getPicture(L"/HOO/GUI/Widgets/button-medium.tga");
        _mediumLabelSize.x = float(pic.width);
        _mediumLabelSize.y = float(pic.height);
    }
    {
        const Renderer::PictureInfos & pic = _pGUI->getResPool()->getPicture(L"/HOO/GUI/Widgets/button-small.tga");
        _smallLabelSize.x = float(pic.width);
        _smallLabelSize.y = float(pic.height);
    }

    _screenFixedWidth = 1024;
    _screenFixedHeight = 768;

    _titleTextSize = 16;
    _mediumTextSize = 12;
    _smallTextSize = 10;

    _pCursorArrow = loadCursor(L"/HOO/GUI/Cursors/arrow.tga");
    _pCursorWait = loadCursor(L"/HOO/GUI/Cursors/wait.tga");

    setCursor(CURSOR_ARROW);
}

Ptr<Core::Bitmap> WidgetFactory::loadCursor(const String & name)
{
    Ptr<Core::InputStream> pInputCursor(_pGUI->getResPool()->getVFS()->readFile(name));

    if(pInputCursor == null)
        throw Core::Exception(L"Unable to open cursor file " + name);

    return Core::loadTGA(*pInputCursor);
}

void WidgetFactory::setCursor(ECursor cursor)
{
    switch(cursor)
    {
    case CURSOR_ARROW:
        _pGUI->setCursor(_pCursorArrow, 2, 2);
        break;
    case CURSOR_WAIT:
        _pGUI->setCursor(_pCursorWait, 2, 2);
        break;
    }
}


void WidgetFactory::setStyle(const Ptr<GUI::Button> & pButton, const String & text, int32 size, const String & picName, bool withDico)
{
    GUI::ButtonStyle bs;
    bs.text     = text;
    bs.fontName = _defaultFontName;
    bs.fontSize = size;
    bs.bold     = false;
    bs.italic   = false;
    bs.shadow   = true;

    GUI::ButtonState bss;
    bss.picName   = picName;
    bss.picColor  = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bss.textColor = Core::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
    bss.posDelta  = Core::Vector2f(1.0f, 1.0f);

    bs.state[GUI::BS_PUSHED] = bss;

    bss.posDelta  = Core::Vector2f(0.0f, 0.0f);
    bs.state[GUI::BS_MOUSE_OVER] = bss;

    bss.picColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    bs.state[GUI::BS_DEFAULT] = bss;

    bss.picColor = Core::Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
    bss.textColor = Core::Vector4f(0.8f, 0.8f, 0.8f, 1.0f);
    bs.state[GUI::BS_DISABLED] = bss;
    bs.state[GUI::BS_DISABLED_MOUSE_OVER] = bss;

    if(withDico)
        pButton->setDictionary(_pDico);
    pButton->setStyle(bs);
}

Ptr<GUI::BitmapLabel> WidgetFactory::createSmallDigitsLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::BitmapLabel> pLabel(new GUI::BitmapLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setFont(L"/HOO/GUI/BitmapFonts/small-digits.font.xml");
    pLabel->setText(text);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::BitmapLabel> WidgetFactory::createMediumDigitsLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::BitmapLabel> pLabel(new GUI::BitmapLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setFont(L"/HOO/GUI/BitmapFonts/medium-digits.font.xml");
    pLabel->setText(text);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::BitmapLabel> WidgetFactory::createBigDigitsLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::BitmapLabel> pLabel(new GUI::BitmapLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setFont(L"/HOO/GUI/BitmapFonts/large-digits.font.xml");
    pLabel->setText(text);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::Label> WidgetFactory::createTitleLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setText(text);
    pLabel->setSize(_largeLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_titleTextSize);
    pLabel->setBold(true);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::Label>   WidgetFactory::createInGameSmallLabel(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));

    if(withDico)
        pLabel->setDictionary(_pDico);

    pLabel->setText(text);
    pLabel->setSize(_smallLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(11);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::Label>   WidgetFactory::createInGameMediumLabel(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));

    if(withDico)
        pLabel->setDictionary(_pDico);

    pLabel->setText(text);
    pLabel->setSize(_smallLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(14);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}


Ptr<GUI::Label> WidgetFactory::createSmallLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setText(text);
    pLabel->setSize(_smallLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_smallTextSize);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
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

Ptr<GUI::Label> WidgetFactory::createCenteredMediumLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setText(text);
    pLabel->setSize(_largeLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_mediumTextSize);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::Label> WidgetFactory::createCenteredSmallLabel(const String & text, int32 x, int32 y)
{
    Ptr<GUI::Label> pLabel(new GUI::Label(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setText(text);
    pLabel->setSize(_largeLabelSize);
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_smallTextSize);
    pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setHLayout(GUI::HL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::RichTextLabel> WidgetFactory::createMediumRTLabel(const String & text, int32 x, int32 y, int32 height)
{
    Ptr<GUI::RichTextLabel> pLabel(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setSize(Core::Vector2f(_mediumLabelSize.x, float(height)));
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_mediumTextSize);
    pLabel->setText(text);
    //pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::RichTextLabel> WidgetFactory::createLargeRTLabel(const String & text, int32 x, int32 y, int32 height)
{
    Ptr<GUI::RichTextLabel> pLabel(new GUI::RichTextLabel(_pGUI->getResPool(), _pGUI->getHUD()));
    pLabel->setDictionary(_pDico);
    pLabel->setSize(Core::Vector2f(_largeLabelSize.x, float(height)));
    pLabel->setFontName(_defaultFontName);
    pLabel->setFontSize(_mediumTextSize);
    pLabel->setText(text);
    //pLabel->setVLayout(GUI::VL_CENTER);
    pLabel->setPosition(Core::Vector2f(float(x), float(y)));

    return pLabel;
}

Ptr<GUI::Widget> WidgetFactory::createDialogBase()
{
    return Ptr<GUI::Widget>(new AutoCenteredPanel(_pGUI->getRenderer(), PANEL_CENTER, _screenFixedWidth, _screenFixedHeight));
}

Ptr<GUI::Widget> WidgetFactory::createDialogBaseTop(int32 h)
{
    return Ptr<GUI::Widget>(new AutoCenteredPanel(_pGUI->getRenderer(), PANEL_TOP, _screenFixedWidth, h));
}

Ptr<GUI::Widget> WidgetFactory::createDialogBaseBottom(int32 h)
{
    return Ptr<GUI::Widget>(new AutoCenteredPanel(_pGUI->getRenderer(), PANEL_BOTTOM, _screenFixedWidth, h));
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

Ptr<GUI::Picture> WidgetFactory::createThinPanel(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/HOO/GUI/Widgets/panel-v-thin.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createMediumPanel(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/HOO/GUI/Widgets/panel-v-medium.tga");
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<TextEdit> WidgetFactory::createMediumTextEdit(int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/HOO/GUI/Widgets/textedit-medium.tga");

    Ptr<GUI::TextEdit> pTextEdit(new GUI::TextEdit(_pGUI->getResPool(), _pGUI->getHUD(), _pInput, _pGUI->getFocus()));

    static const float padding = 16.0f;

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

Ptr<GUI::Button> WidgetFactory::createSmallButton(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Button> pButton(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    setStyle(pButton, text, _smallTextSize, L"/HOO/GUI/Widgets/button-small.tga", withDico);
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    return pButton;
}

Ptr<GUI::Button> WidgetFactory::createCompactButton(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Button> pButton(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    setStyle(pButton, text, _mediumTextSize, L"/HOO/GUI/Widgets/button-compact.tga", withDico);
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    return pButton;
}

Ptr<GUI::Button> WidgetFactory::createMediumButton(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Button> pButton(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    setStyle(pButton, text, _mediumTextSize, L"/HOO/GUI/Widgets/button-medium.tga", withDico);
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    return pButton;
}

Ptr<GUI::Button> WidgetFactory::createLargeButton(const String & text, int32 x, int32 y, bool withDico)
{
    Ptr<GUI::Button> pButton(new GUI::Button(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));
    setStyle(pButton, text, _mediumTextSize, L"/HOO/GUI/Widgets/button-large.tga", withDico);
    pButton->setPosition(Core::Vector2f(float(x), float(y)));
    return pButton;
}

Ptr<GUI::Picture> WidgetFactory::createMessageBoxMedium()
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(L"/HOO/GUI/Widgets/messagebox-medium.tga");
    centerScreen(pPicture);
    return pPicture;
}

Ptr<GUI::Picture> WidgetFactory::createPicture(const String & pic, int32 x, int32 y)
{
    Ptr<GUI::Picture> pPicture(new GUI::Picture(_pGUI->getResPool(), _pGUI->getHUD()));
    pPicture->setPicture(pic);
    pPicture->setPosition(Core::Vector2f(float(x), float(y)));
    return pPicture;
}

Ptr<GUI::ProgressBar> WidgetFactory::createProgressBar(int32 x, int32 y)
{
    Ptr<GUI::ProgressBar> pProgress(new GUI::ProgressBar(_pGUI->getResPool(), _pGUI->getHUD()));
    pProgress->setBarInterior(L"/HOO/GUI/Widgets/bar-inside.tga");
    pProgress->setBarBorder(L"/HOO/GUI/Widgets/bar-border.tga");
    pProgress->setPosition(Core::Vector2f(float(x), float(y)));
    pProgress->setBarInteriorPosition(Core::Vector2f(23.0f, 7.0f));
    return pProgress;
}

Ptr<GUI::CheckBox> WidgetFactory::createCheckBox(const String & text, int32 x, int32 y)
{
    Ptr<GUI::CheckBox> pCheckBox(new GUI::CheckBox(_pGUI->getResPool(), _pGUI->getHUD(), _pInput));

    GUI::CheckBoxStyle style;
    style.text          = text;
    style.picChecked    = L"/HOO/GUI/Widgets/checkbox-on.tga";
    style.picUnchecked  = L"/HOO/GUI/Widgets/checkbox-off.tga";
    style.fontName      = _defaultFontName;
    style.fontSize      = _mediumTextSize;
    style.padding       = 5.0f;
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

    return pCheckBox;
}

