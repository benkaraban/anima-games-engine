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
#ifndef HOO_WIDGETFACTORY_H_
#define HOO_WIDGETFACTORY_H_

#include <GUI/Button.h>
#include <GUI/RichTextLabel.h>
#include <GUI/TextEdit.h>
#include <GUI/Manager.h>
#include <GUI/CheckBox.h>
#include <GUI/ProgressBar.h>
#include <GUI/BitmapLabel.h>

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

LM_ENUM_2(ECursor, CURSOR_ARROW, CURSOR_WAIT);

class WidgetFactory : public Core::Object
{
public:
    WidgetFactory(const Ptr<GUI::Manager> & pGUI, const Ptr<Window::InputManager> & pInput, const Ptr<Core::Dictionary> & pDico);

    Ptr<GUI::Manager> getManager() { return _pGUI; };

    void centerScreen(const Ptr<GUI::Widget> & pWidget);

    void setCursor(ECursor cursor);

    Ptr<Core::Dictionary> getDictionary() const { return _pDico; }

    /**
     * Crée un panel invisible centré pour travailler "virtuellement" en 1024*768
     */
    Ptr<GUI::Widget>  createDialogBase();
    Ptr<GUI::Widget>  createDialogBaseTop(int32 h);
    Ptr<GUI::Widget>  createDialogBaseBottom(int32 h);

    Ptr<GUI::CheckBox> createCheckBox(const String & text, int32 x, int32 y);

    Ptr<GUI::Picture> createThinPanel(bool centered = true);
    Ptr<GUI::Picture> createThinPanel(int32 x, int32 y);

    Ptr<GUI::Picture> createMediumPanel(bool centered = true);
    Ptr<GUI::Picture> createMediumPanel(int32 x, int32 y);

    Ptr<GUI::Picture> createPicture(const String & pic, int32 x, int32 y);

    Ptr<GUI::Picture> createMessageBoxMedium();

    Ptr<GUI::BitmapLabel> createSmallDigitsLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::BitmapLabel> createMediumDigitsLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::BitmapLabel> createBigDigitsLabel(const String & text, int32 x, int32 y);

    Ptr<GUI::Label>   createInGameSmallLabel(const String & text, int32 x, int32 y, bool withDico = false);
    Ptr<GUI::Label>   createInGameMediumLabel(const String & text, int32 x, int32 y, bool withDico = false);

    Ptr<GUI::Label>   createSmallLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::Label>   createMediumLabel(const String & text, int32 x, int32 y);

    Ptr<GUI::Label>   createCenteredSmallLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::Label>   createCenteredMediumLabel(const String & text, int32 x, int32 y);

    Ptr<GUI::Label>   createTitleLabel(const String & text, int32 x, int32 y);

    Ptr<GUI::RichTextLabel> createMediumRTLabel(const String & text, int32 x, int32 y, int32 height);
    Ptr<GUI::RichTextLabel> createLargeRTLabel(const String & text, int32 x, int32 y, int32 height);

    Ptr<TextEdit>     createMediumTextEdit(int32 x, int32 y);

    Ptr<GUI::Button>  createSmallButton(const String & text, int32 x, int32 y, bool withDico = true);
    Ptr<GUI::Button>  createCompactButton(const String & text, int32 x, int32 y, bool withDico = true);
    Ptr<GUI::Button>  createMediumButton(const String & text, int32 x, int32 y, bool withDico = true);
    Ptr<GUI::Button>  createLargeButton(const String & text, int32 x, int32 y, bool withDico = true);

    Ptr<GUI::ProgressBar> createProgressBar(int32 x, int32 y);

protected:
    Ptr<Core::Bitmap> loadCursor(const String & name);
    void setStyle(const Ptr<GUI::Button> & pButton, const String & text, int32 size, const String & picName, bool withDico);

    Ptr<GUI::Manager>           _pGUI;
    Ptr<Window::InputManager>   _pInput;
    Ptr<Core::Dictionary>       _pDico;

    String                      _defaultFontName;
    Core::Vector2f              _largeLabelSize;

    Core::Vector2f              _mediumLabelSize;
    int32                       _mediumTextSize;

    Core::Vector2f              _titleLabelSize;
    int32                       _titleTextSize;

    Core::Vector2f              _smallLabelSize;
    int32                       _smallTextSize;

    int32                       _screenFixedWidth;
    int32                       _screenFixedHeight;

    Ptr<Core::Bitmap>           _pCursorArrow;
    Ptr<Core::Bitmap>           _pCursorWait;
};

#endif
