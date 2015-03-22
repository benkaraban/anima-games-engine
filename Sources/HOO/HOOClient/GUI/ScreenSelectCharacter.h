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
#ifndef HOO_GUI_SCREENSELECTCHARACTER_H_
#define HOO_GUI_SCREENSELECTCHARACTER_H_

#include <HOOClient/IAppState.h>

#include <HOOClient/GUI/IScreen.h>
#include <HOOClient/GUI/WidgetFactory.h>
#include <HOOClient/GUI/ScreenMessageBox.h>
#include <HOOClient/GUI/ScreenLoginInProgress.h>

#include <HOOUserProtocol/SelectCharacter.h>

class ScreenSelectCharacterInProgress;

class ScreenSelectCharacter : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenSelectCharacter(AppContext & appContext);

    void setScreenOK(const Ptr<IScreen> & pScreen) { _pScreenOK = pScreen; }
    void setScreenFailed(const Ptr<IScreen> & pScreen) { _pScreenFailed = pScreen; }

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

protected:
    void onCharChange();

    AppContext &            _appContext;
    Ptr<WidgetFactory>      _pWidgets;

    Ptr<ScreenMessageBox>   _pMessageBox;

    Ptr<GUI::Widget>        _pDialog;
    Ptr<GUI::Widget>        _pPanel;

    Ptr<GUI::Label>         _pLabelName;
    Ptr<GUI::RichTextLabel> _pLabelDetails;
    Ptr<GUI::RichTextLabel> _pLabelNotice;

    Ptr<GUI::Button>        _pButtonPrev;
    Ptr<GUI::Button>        _pButtonNext;
    Ptr<GUI::Button>        _pButtonOK;

    Ptr<IScreen>            _pScreenOK;
    Ptr<IScreen>            _pScreenFailed;

    Ptr<ScreenSelectCharacterInProgress>  _pScreenProgress;

    Ptr<Universe::World>            _pWorld;
    Ptr<Universe::NodeCamera>       _pCamera;

    int32                           _charId;
};

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class ScreenSelectCharacterInProgress : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenSelectCharacterInProgress(AppContext & appContext);

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setScreenSuccess(IScreen * pScreen) { _pScreenSuccess = pScreen; }
    void setScreenFailed(IScreen * pScreen) { _pScreenFailed = pScreen; }

    HOOUserProtocol::SelectCharacter selectChar;

protected:
    void createError(const String & error);

    AppContext &                _appContext;

    Ptr<ScreenMessageBox>       _pMessageBox;
    IScreen *                   _pScreenSuccess;
    IScreen *                   _pScreenFailed;

    Ptr<GUI::Widget>            _pDialog;
};



#endif