/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * JÃ©rÃ©mie Comarmond, Didier Colin.
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
#ifndef CHRONORAGE_WIDGETFACTORY_H_
#define CHRONORAGE_WIDGETFACTORY_H_

#include <MenuScreens/GUI/CRButton.h>
#include <MenuScreens/GUI/CRCheckBox.h>
#include <MenuScreens/GUI/CRComboBox.h>
#include <GUI/TextEdit.h>
#include <GUI/Manager.h>
#include <GUI/ScrollBar.h>
#include <GUI/RichTextLabel.h>

namespace ChronoRage
{

class TextEdit : public GUI::Widget, public GUI::IEventHandler
{
public:
    TextEdit(const Ptr<GUI::TextEdit> & pEdit, const Ptr<GUI::Picture> & pBackground) : _pEdit(pEdit), _pBackGround(pBackground)
    {
        addChild(pBackground);
        pEdit->addEventHandler(this);
    }

    Ptr<GUI::Picture>   getBackground() const { return _pBackGround; };
    Ptr<GUI::TextEdit>  getEdit() const { return _pEdit; };

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
    {
        notify(ev);
    }

    virtual void focus()
    {
        _pEdit->focus();
    }
    virtual void        setPrevWidget(const Ptr<TextEdit> & pWidget)
    { 
        _pEdit->setPrevWidget((Widget *)pWidget->_pEdit.get()); 
    }
    virtual void        setNextWidget(const Ptr<TextEdit> & pWidget)
    { 
        _pEdit->setNextWidget((Widget *)pWidget->_pEdit.get()); 
    }

protected:
    Ptr<GUI::TextEdit>  _pEdit;
    Ptr<GUI::Picture>   _pBackGround;
};

class KeyNav : public Core::Object, public GUI::IKeyController
{
public:
    KeyNav(const Ptr<Window::InputManager> & pInput) : _pInput(pInput) {};

    virtual bool up() { return _pInput->isKeyTyped(VK_UP); };
    virtual bool down() { return _pInput->isKeyTyped(VK_DOWN); };
    virtual bool left() { return _pInput->isKeyTyped(VK_LEFT); };
    virtual bool right() { return _pInput->isKeyTyped(VK_RIGHT); };

    virtual bool enter() { return _pInput->isKeyDown(VK_RETURN); };
    virtual bool escape() { return _pInput->isKeyDown(VK_ESCAPE); };

    Ptr<Window::InputManager>   _pInput;
};

LM_ENUM_1(ECursor, CURSOR_ARROW);

class WidgetFactory : public Core::Object
{
public:
    WidgetFactory(  const Ptr<GUI::Manager> & pGUI, 
                    const Ptr<Window::InputManager> & pInput, 
                    const Ptr<Core::Dictionary> & pDico, 
                    const Ptr<SoundLibrary> & pSoundLibrary);

    Ptr<GUI::Manager> getManager() { return _pGUI; };

    void bottomScreen(const Ptr<GUI::Widget> & pWidget);
    void centerScreen(const Ptr<GUI::Widget> & pWidget);

    void setCursor(ECursor cursor);

    /**
     * Crée un panel invisible centré pour travailler "virtuellement" en 1024*768
     */
    Ptr<GUI::Widget>  createDialogBase();

    Ptr<CRCheckBox> createCheckBox(const String & text, int32 x, int32 y);

    Ptr<CRComboBox> createComboBox(const Core::List<String> & items, int32 x, int32 y);
    Ptr<CRComboBox> createLargeComboBox(const Core::List<String> & items, int32 x, int32 y);

    Ptr<GUI::HScrollBar> createHScrollBar(int32 x, int32 y);

    Ptr<GUI::Picture> createPicture(const String & name, int32 x, int32 y);

    Ptr<GUI::Picture> createThinPanel(bool centered = true);
    Ptr<GUI::Picture> createThinPanel(int32 x, int32 y);

    Ptr<GUI::Picture> createMediumPanel(bool centered = true);
    Ptr<GUI::Picture> createMediumPanel(int32 x, int32 y);

    Ptr<GUI::Picture> createLargePanel(bool centered = true);
    Ptr<GUI::Picture> createLargePanel(int32 x, int32 y);

    Ptr<GUI::Picture> createThinBand(int32 x, int32 y);
    Ptr<GUI::Picture> createMediumBand(int32 x, int32 y);
    Ptr<GUI::Picture> createLargeBand(int32 x, int32 y);

    Ptr<GUI::Picture> createMessageBoxMedium();

    Ptr<GUI::Label>   createMediumLabel(const String & text, int32 x, int32 y);

    Ptr<GUI::RichTextLabel> createRichTextLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::RichTextLabel> createRichTextHint(const String & text, int32 x, int32 y);

    Ptr<TextEdit>     createMediumTextEdit(int32 x, int32 y);

    Ptr<CRButton>  createSmallButton(const String & text, int32 x, int32 y);
    Ptr<CRButton>  createMediumButton(const String & text, int32 x, int32 y);
    //Ptr<GUI::Button>  createLargeButton(const String & text, int32 x, int32 y);

protected:
    void setStyle(const Ptr<GUI::Button> & pButton, const String & text, int32 size, const String & picName);

    Ptr<GUI::Manager>           _pGUI;
    Ptr<Window::InputManager>   _pInput;
    Ptr<Core::Dictionary>       _pDico;
    Ptr<SoundLibrary>           _pSoundLibrary;

    String                      _defaultFontName;
    Core::Vector2f              _mediumLabelSize;
    int32                       _mediumTextSize;
    Core::Vector2f              _smallLabelSize;
    int32                       _smallTextSize;

    Core::Vector2f              _smallHintSize;
    int32                       _smallHintTextSize;

    int32                       _screenFixedWidth;
    int32                       _screenFixedHeight;

    Ptr<KeyNav>                 _pKeyNav;

    Ptr<Core::Bitmap>           _pCursorArrow;
};

}//namespace ChronoRage

#endif /*CHRONORAGE_WIDGETFACTORY_H_*/
