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
#ifndef CHRONORAGE_GUI_SCREENCREDITS_H_
#define CHRONORAGE_GUI_SCREENCREDITS_H_

#include <Modes/IGameState.h>

#include <MenuScreens/IScreen.h>
#include <MenuScreens/WidgetFactory.h>
#include <MenuScreens/ScreenSelectInput.h>

namespace ChronoRage
{

class ScreenCredits : public Core::Object, public GUI::IEventHandler, public IScreen
{
public:
    ScreenCredits(AppContext & appContext);

    virtual void resetLabels();

    virtual void startScreen();
    virtual void endScreen();

    virtual void update(double elapsed);

    virtual void onEvent(GUI::EEvent ev, GUI::Widget * pWidget);

    void setMainMenuScreen(const Ptr<IScreen> & pScreen) { _pScreenMainMenu = pScreen; }

protected:
    AppContext &            _appContext;
    
    Ptr<GUI::Widget>        _pDialog;

    Ptr<GUI::Widget>        _pGlobalPanel;

    //Gauche
    Ptr<GUI::Widget>        _pPanelLeft;

    Ptr<GUI::Label>         _pAnimaGames;
    //Programming
    Ptr<GUI::Label>         _pProgramming;
    Ptr<GUI::Label>         _pProgrammingJC;
    Ptr<GUI::Label>         _pProgrammingBK;

    //Tools Programming
    Ptr<GUI::Label>         _pToolsProgramming;
    Ptr<GUI::Label>         _pToolsProgrammingLS;

    //Artworks
    Ptr<GUI::Label>         _pArtworks;
    Ptr<GUI::Label>         _pArtworksJC;
    Ptr<GUI::Label>         _pArtworksBK;

    //Game Design
    Ptr<GUI::Label>         _pGameDesign;
    Ptr<GUI::Label>         _pGameDesignDC;
    Ptr<GUI::Label>         _pGameDesignJC;
    Ptr<GUI::Label>         _pGameDesignBK;

    //Droite
    Ptr<GUI::Widget>        _pPanelRight;
    //Special Thanks
    Ptr<GUI::Label>         _pSpecialThanks;
    Ptr<GUI::Label>         _pSpecialThanksCB;
    Ptr<GUI::Label>         _pSpecialThanksAC;
    Ptr<GUI::Label>         _pSpecialThanksMC;
    Ptr<GUI::Label>         _pSpecialThanksAD;

    Ptr<GUI::Label>         _pSpecialThanksSB;
    Ptr<GUI::Label>         _pSpecialThanksPG;
    Ptr<GUI::Label>         _pSpecialThanksJK;
    Ptr<GUI::Label>         _pSpecialThanksMK;
    Ptr<GUI::Label>         _pSpecialThanksMBL;
    Ptr<GUI::Label>         _pSpecialThanksTL;
    Ptr<GUI::Label>         _pSpecialThanksGS;
    Ptr<GUI::Label>         _pSpecialThanksMS;
    Ptr<GUI::Label>         _pSpecialThanksKS;

    Ptr<GUI::Button>        _pButtonBack;
    
    Ptr<IScreen>            _pScreenMainMenu;

    GUI::Widget *           _pLastKeyFocus;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_GUI_SCREENCREDITS_H_*/
