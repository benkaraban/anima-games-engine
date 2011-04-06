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
#ifndef HOOCLIENT_PANELSTATUS_H_
#define HOOCLIENT_PANELSTATUS_H_

#include <Core/Dictionary.h>

#include <GUI/Widget.h>
#include <GUI/Label.h>
#include <GUI/Manager.h>

#include <HOOUserProtocol/GameMessage/GameData.h>

#include <HOOClient/GUI/WidgetFactory.h>

class PanelSphereStatus : public GUI::Widget
{
public:
    PanelSphereStatus(const Ptr<WidgetFactory> & pFactory, const String & picName);

    void initStatus(const HOOUserProtocol::PlayerSphereStatus & status);
    void setStatus(const HOOUserProtocol::PlayerSphereStatus & status);

    virtual void update(double elapsed);

protected:
    HOOUserProtocol::PlayerSphereStatus     _status;

    Ptr<WidgetFactory>              _pFactory;

    Ptr<GUI::Picture>               _pPicture;
    Ptr<GUI::Picture>               _pChaining;
    Ptr<GUI::BitmapLabel>           _pLabelMana;
    Ptr<GUI::BitmapLabel>           _pLabelChaining;
    Ptr<GUI::BitmapLabel>           _pLabelDelta;

    float                           _elapsed;
    bool                            _deltaHidden;
};

class PanelStatus : public GUI::Widget
{
public:
    PanelStatus(const Ptr<WidgetFactory> & pFactory);

    void initStatus(const HOOUserProtocol::GameStatus & status, const String & p1, const String & p2);
    void setStatus(const HOOUserProtocol::GameStatus & status);

    virtual void update(double elapsed);

protected:
    Ptr<WidgetFactory>              _pFactory;
    HOOUserProtocol::GameStatus     _status;

    Ptr<PanelSphereStatus>          _pSphere[3];
    Ptr<PanelSphereStatus>          _pSphere2[3];

    int32                           _maxHealth1;
    int32                           _maxHealth2;

    Ptr<GUI::Picture>               _pSphereMonad;
    Ptr<GUI::Picture>               _pSphereMonad2;

    Ptr<GUI::Label>                 _pName1;
    Ptr<GUI::Label>                 _pName2;

    Ptr<GUI::Picture>               _pTimerPanel;
    Ptr<GUI::BitmapLabel>           _pTimerLabel;

    Ptr<GUI::Picture>               _pVantage1;
    Ptr<GUI::Picture>               _pVantage2;

    Ptr<GUI::ProgressBar>           _pHealthBar1;
    Ptr<GUI::BitmapLabel>           _pHealthLabel1;
    Ptr<GUI::BitmapLabel>           _pHealthDelta1;

    Ptr<GUI::ProgressBar>           _pHealthBar2;
    Ptr<GUI::BitmapLabel>           _pHealthLabel2;
    Ptr<GUI::BitmapLabel>           _pHealthDelta2;

    Ptr<GUI::Picture>               _pAbsorbIcon;
    Ptr<GUI::BitmapLabel>           _pAbsorbLabel;
    Ptr<GUI::Picture>               _pImpairIcon;
    Ptr<GUI::BitmapLabel>           _pImpairLabel;
    Ptr<GUI::Picture>               _pAnticipateIcon;
    Ptr<GUI::BitmapLabel>           _pAnticipateLabel;

    float                           _elapsed;
    bool                            _deltaHidden1;
    bool                            _deltaHidden2;
    bool                            _deltaHiddenWill;
};

#endif
