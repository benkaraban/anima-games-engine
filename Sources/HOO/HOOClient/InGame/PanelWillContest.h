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
#ifndef HOOCLIENT_PANEWILLCONTEST_H_
#define HOOCLIENT_PANEWILLCONTEST_H_

#include <Core/Dictionary.h>

#include <GUI/Widget.h>
#include <GUI/Label.h>
#include <GUI/Manager.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

#include <HOOClient/GUI/WidgetFactory.h>

static const int32 BID_BUTTONS_COUNT = 5;

LM_ENUM_6(EWillState,
          WILL_PANEL_WAIT_CHOICE,
          WILL_PANEL_WAIT_CHOICE_MOUSE_OVER,
          WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN_OVER,
          WILL_PANEL_WAIT_CHOICE_MOUSE_DOWN,
          WILL_PANEL_CHOICE_DONE,
          WILL_PANEL_RESOLVE);

class PanelWillContest : public GUI::Widget
{
public:
    PanelWillContest(const Ptr<WidgetFactory> & pFactory);

    virtual void update(double elapsed);

    void setTotalWill(int32 w) { _totalWill = w; }
    void setContest(int32 manaCount, int32 manaSphere, const Core::List<HOOUserProtocol::ActionCommand> & commands);
    void resolve(bool playerWin);

    const HOOUserProtocol::ActionCommand & getCommand() const { return _command; }

protected:
    Ptr<WidgetFactory>              _pFactory;

    Ptr<GUI::Widget>                _pPanel;
    EWillState                      _state;
    HOOUserProtocol::ActionCommand  _command;
    int32                           _totalWill;
    int32                           _maxBid;
    int32                           _step;
    float                           _elapsed;
    float                           _w1;
    float                           _w2;
    float                           _x1;
    float                           _x2;

    Ptr<GUI::Picture>               _pAvailablePic;
    Ptr<GUI::Label>                 _pAvailableLabel;

    Ptr<GUI::BitmapLabel>           _pManaLabel;
    Ptr<GUI::Picture>               _pManaPic;

    Ptr<GUI::Picture>               _pWillBack;
    Ptr<GUI::ProgressBar>           _pWillBar;
    Ptr<GUI::BitmapLabel>           _pWillLabel;
    Ptr<GUI::Picture>               _pWillPic;
    Ptr<GUI::BitmapLabel>           _pBidLabel;
};

#endif
