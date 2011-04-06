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
#ifndef CHRONORAGE_GUI_HELP_H_
#define CHRONORAGE_GUI_HELP_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>

namespace ChronoRage
{

class ScreenHelp : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenHelp(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setNextScreen(const Ptr<IScreen> & pScreen) { _pScreenNext = pScreen; }

protected:
    Ptr<GUI::Label> createKeyLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::Label> createLabel(const String & text, int32 x, int32 y);
    Ptr<GUI::Label> createHint(const String & text, int32 x, int32 y);

    AppContext &            _appContext;
    
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pPanel;

    Ptr<GUI::Picture>       _pPicKey1;
    Ptr<GUI::Label>         _pLabelUp;
    Ptr<GUI::Label>         _pLabelDown;
    Ptr<GUI::Label>         _pLabelLeft;
    Ptr<GUI::Label>         _pLabelRight;

    Ptr<GUI::Picture>       _pPicKey2;
    Ptr<GUI::Label>         _pLabelAimUp;
    Ptr<GUI::Label>         _pLabelAimDown;
    Ptr<GUI::Label>         _pLabelAimLeft;
    Ptr<GUI::Label>         _pLabelAimRight;
    Ptr<GUI::Label>         _pLabelKeyTimeWarp;
    Ptr<GUI::Label>         _pLabelKeyFire;

    Ptr<GUI::Picture>       _pPicMouse;
    Ptr<GUI::Label>         _pLabelLMB;
    Ptr<GUI::Label>         _pLabelRMB;

    Ptr<GUI::Picture>       _pPicPad;
    Ptr<GUI::Label>         _pLabelPadMove;
    Ptr<GUI::Label>         _pLabelPadAim;
    Ptr<GUI::Label>         _pLabelPadTimeWarp;
    Ptr<GUI::Label>         _pLabelPadFire;

    Ptr<GUI::Picture>       _pPicScreen;

    Ptr<GUI::Label>         _pLabelDestroy;
    Ptr<GUI::Label>         _pLabelDestroyHint;

    Ptr<GUI::Picture>       _pPicCarrier;
    Ptr<GUI::Label>         _pLabelCarrier;
    Ptr<GUI::Label>         _pLabelCarrierHint;

    Ptr<GUI::Picture>       _pPicBonus;
    Ptr<GUI::Label>         _pLabelBonus;
    Ptr<GUI::Label>         _pLabelBonusHint1;

    Ptr<GUI::Label>         _pLabelRemaining;
    Ptr<GUI::Label>         _pLabelRemainingHint;

    Ptr<GUI::Label>         _pLabelFirepower;
    Ptr<GUI::Label>         _pLabelFirepowerHint;

    Ptr<GUI::Label>         _pLabelTimeWarps;
    Ptr<GUI::Label>         _pLabelTimeWarpsHint;

    Ptr<GUI::Label>         _pLabelControls;
    Ptr<GUI::Label>         _pLabelControlsHint;

    Ptr<GUI::Button>        _pButtonNext;
    
    Ptr<IScreen>            _pScreenNext;

    GUI::Widget *           _pLastKeyFocus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_HELP_H_*/
