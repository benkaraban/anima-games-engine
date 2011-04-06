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
#include "ScreenCredits.h"

namespace ChronoRage
{

ScreenCredits::ScreenCredits(AppContext & appContext) 
:   _appContext(appContext)
{
    _pDialog = _appContext.pWidgetFactory->createDialogBase();

    _pGlobalPanel = Ptr<GUI::Widget>(new GUI::Widget());
    _pGlobalPanel->setPosition(Core::Vector2f(0, 0));
    
    _pPanelLeft = _appContext.pWidgetFactory->createMediumPanel(0, 0);
    _pPanelRight = _appContext.pWidgetFactory->createMediumPanel(500, 0);

    static const int32 Y_PADDING = 30;
    static const int32 X_COL_1 = 24;
    static const int32 X_COL_2 = 100;

    _pAnimaGames = _appContext.pWidgetFactory->createMediumLabel(L"", 0, 20);
    Core::Vector2f panelLeftSize = _pPanelLeft->getSize();
    panelLeftSize.y = 110.0f;
    _pAnimaGames->setSize(panelLeftSize);
    _pAnimaGames->setHLayout(GUI::HL_CENTER);
    _pAnimaGames->setDictionary(null);
    _pAnimaGames->setText(L"ANIMA GAMES Team");

    _pPanelLeft->addChild(_pAnimaGames);

    int padding = 4;    

    _pProgramming   = _appContext.pWidgetFactory->createMediumLabel(L"programming", X_COL_1, padding++ * Y_PADDING);
    _pProgrammingJC = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pProgrammingBK = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    
    _pProgrammingJC->setDictionary(null);
    _pProgrammingJC->setText(L"Jérémie COMARMOND");
    _pProgrammingBK->setDictionary(null);
    _pProgrammingBK->setText(L"Benjamin KARABAN");

    _pPanelLeft->addChild(_pProgramming);
    _pPanelLeft->addChild(_pProgrammingJC);
    _pPanelLeft->addChild(_pProgrammingBK);
    
    padding++;
    _pToolsProgramming  = _appContext.pWidgetFactory->createMediumLabel(L"tools-programming", X_COL_1, padding++ * Y_PADDING);
    _pToolsProgrammingLS= _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pToolsProgrammingLS->setDictionary(null);
    _pToolsProgrammingLS->setText(L"Laurent SCHNEIDER");

    _pPanelLeft->addChild(_pToolsProgramming);
    _pPanelLeft->addChild(_pToolsProgrammingLS);

    padding++;
    _pArtworks      = _appContext.pWidgetFactory->createMediumLabel(L"artworks", X_COL_1, padding++ * Y_PADDING);
    _pArtworksJC    = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pArtworksBK    = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    
    _pArtworksJC->setDictionary(null);
    _pArtworksJC->setText(L"Jérémie COMARMOND");
    _pArtworksBK->setDictionary(null);
    _pArtworksBK->setText(L"Benjamin KARABAN");

    _pPanelLeft->addChild(_pArtworks);
    _pPanelLeft->addChild(_pArtworksJC);
    _pPanelLeft->addChild(_pArtworksBK);

    padding++;
    _pGameDesign    = _appContext.pWidgetFactory->createMediumLabel(L"game-design", X_COL_1, padding++ * Y_PADDING);
    _pGameDesignDC  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pGameDesignJC  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pGameDesignBK  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    
    _pGameDesignDC->setDictionary(null);
    _pGameDesignDC->setText(L"Didier COLIN");
    _pGameDesignJC->setDictionary(null);
    _pGameDesignJC->setText(L"Jérémie COMARMOND");
    _pGameDesignBK->setDictionary(null);
    _pGameDesignBK->setText(L"Benjamin KARABAN");

    _pPanelLeft->addChild(_pGameDesign);
    _pPanelLeft->addChild(_pGameDesignDC);
    _pPanelLeft->addChild(_pGameDesignJC);
    _pPanelLeft->addChild(_pGameDesignBK);

    _pPanelLeft->pack();

    //RIGHT
    padding = 2;

    _pSpecialThanks = _appContext.pWidgetFactory->createMediumLabel(L"special-thanks", X_COL_1, padding++ * Y_PADDING);
    _pPanelRight->addChild(_pSpecialThanks);

    _pSpecialThanksCB  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksAC  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksMC  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksAD  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);

    _pSpecialThanksCB->setDictionary(null);
    _pSpecialThanksAC->setDictionary(null);
    _pSpecialThanksMC->setDictionary(null);
    _pSpecialThanksAD->setDictionary(null);

    _pSpecialThanksCB->setText(L"Cécile BAUDONCOURT");
    _pSpecialThanksAC->setText(L"Aurélie COLIN");
    _pSpecialThanksMC->setText(L"Mialy COMARMOND");
    _pSpecialThanksAD->setText(L"Aurélie DURAND");

    _pPanelRight->addChild(_pSpecialThanksCB);
    _pPanelRight->addChild(_pSpecialThanksAC);
    _pPanelRight->addChild(_pSpecialThanksMC);
    _pPanelRight->addChild(_pSpecialThanksAD);

    padding++;

    _pSpecialThanksSB  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksPG  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksJK  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksMK  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksMBL = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksTL  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksGS  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksMS  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    _pSpecialThanksKS  = _appContext.pWidgetFactory->createMediumLabel(L"", X_COL_2, padding++ * Y_PADDING);
    
    _pSpecialThanksSB->setDictionary(null);
    _pSpecialThanksPG->setDictionary(null);
    _pSpecialThanksJK->setDictionary(null);
    _pSpecialThanksMK->setDictionary(null);
    _pSpecialThanksMBL->setDictionary(null);
    _pSpecialThanksTL->setDictionary(null);
    _pSpecialThanksGS->setDictionary(null);
    _pSpecialThanksMS->setDictionary(null);
    _pSpecialThanksKS->setDictionary(null);

    _pSpecialThanksSB->setText(L"Sylvain BUGAT");
    _pSpecialThanksPG->setText(L"Pierre GAGNEUR");
    _pSpecialThanksJK->setText(L"Jérôme KARABAN");
    _pSpecialThanksMK->setText(L"Michèle KARABAN");
    _pSpecialThanksMBL->setText(L"Marie-Bernard LASCOMBE");
    _pSpecialThanksTL->setText(L"Thérèse LASCOMBE");
    _pSpecialThanksGS->setText(L"Gilles SCHNEIDER");
    _pSpecialThanksMS->setText(L"Matthieu SCHNEIDER");
    _pSpecialThanksKS->setText(L"Karl SCHWERDT");

    _pPanelRight->addChild(_pSpecialThanksSB);
    _pPanelRight->addChild(_pSpecialThanksPG);
    _pPanelRight->addChild(_pSpecialThanksJK);
    _pPanelRight->addChild(_pSpecialThanksMK);
    _pPanelRight->addChild(_pSpecialThanksMBL);
    _pPanelRight->addChild(_pSpecialThanksTL);
    _pPanelRight->addChild(_pSpecialThanksGS);
    _pPanelRight->addChild(_pSpecialThanksMS);
    _pPanelRight->addChild(_pSpecialThanksKS);
    
    _pPanelRight->pack();

    _pGlobalPanel->addChild(_pPanelLeft);
    _pGlobalPanel->addChild(_pPanelRight);

    _pButtonBack = _appContext.pWidgetFactory->createSmallButton(L"back", 769, 620);
    _pButtonBack->setHotKey(VK_ESCAPE);
    _pButtonBack->addEventHandler(this);

    _pGlobalPanel->addChild(_pButtonBack);
    
    _pGlobalPanel->pack();

    _appContext.pWidgetFactory->centerScreen(_pGlobalPanel);

    _pLastKeyFocus = _pButtonBack.get();
    _pDialog->addChild(_pGlobalPanel);
}

void ScreenCredits::resetLabels()
{
    _pProgramming->setDictionary(_appContext.pDico);
    _pToolsProgramming->setDictionary(_appContext.pDico);
    _pArtworks->setDictionary(_appContext.pDico);
    _pGameDesign->setDictionary(_appContext.pDico);
    _pSpecialThanks->setDictionary(_appContext.pDico);

    _pProgramming->setText(L"programming");
    _pToolsProgramming->setText(L"tools-programming");
    _pArtworks->setText(L"artworks");
    _pGameDesign->setText(L"game-design");
    _pSpecialThanks->setText(L"special-thanks");
}

void ScreenCredits::startScreen()
{
    _appContext.pWidgetFactory->getManager()->setDialog(_pDialog);
    _appContext.pWidgetFactory->getManager()->getKeyNavigation()->pKeyFocus = _pLastKeyFocus;

    _pDialog->pack();
}
    
void ScreenCredits::endScreen()
{
}

void ScreenCredits::update(double elapsed)
{
}

void ScreenCredits::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
    if(ev == GUI::EV_COMMAND)
    {
        if(pWidget == _pButtonBack.get())
        {
            LM_ASSERT(_pScreenMainMenu != null);
            _pManager->setScreen(_pScreenMainMenu.get());
        }

        _pLastKeyFocus = pWidget;
    }
}

}//namespace ChronoRage
