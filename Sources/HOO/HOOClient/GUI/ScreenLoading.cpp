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
#include <Core/Logger.h>

#include <HOOClient/GUI/ScreenLoading.h>
#include <HOOClient/InGame/InGameMusic.h>

using namespace HOOUserProtocol;

ScreenLoading::ScreenLoading(AppContext & appContext) : _appContext(appContext)
{
    _levelId = -1;
    const Ptr<WidgetFactory> & pWidgets = _appContext.pWidgets;

    _pDialog = pWidgets->createDialogBase();
    _pPanel = pWidgets->createMediumPanel();

    _pLabelLoading = pWidgets->createTitleLabel(L"loading", 30, 15);
    _pLabelLevelName = pWidgets->createTitleLabel(L"", 30, 50);
    _pPicPreview = pWidgets->createPicture(L"", 115, 100);
 
    _pLabelNameP1 = pWidgets->createCenteredMediumLabel(L"", 10, 245);
    _pLabelNameP2 = pWidgets->createCenteredMediumLabel(L"", 10, 345);

    _pLabelNameP1->setDictionary(null);
    _pLabelNameP2->setDictionary(null);

    _pLabelNameP1->setText(L"");
    _pLabelNameP2->setText(L"");

    _pLabelRankP1 = pWidgets->createCenteredSmallLabel(L"RANK_00", 30, 265);
    _pLabelRankP2 = pWidgets->createCenteredSmallLabel(L"RANK_00", 30, 365);

    _pProgressP1 = pWidgets->createProgressBar(22, 295);
    _pProgressP2 = pWidgets->createProgressBar(22, 395);

    _pDialog->addChild(_pPanel);
    _pPanel->addChild(_pLabelLoading);
    _pPanel->addChild(_pLabelLevelName);
    _pPanel->addChild(_pPicPreview);
    _pPanel->addChild(_pLabelNameP1);
    _pPanel->addChild(_pLabelNameP2);
    _pPanel->addChild(_pLabelRankP1);
    _pPanel->addChild(_pLabelRankP2);
    _pPanel->addChild(_pProgressP1);
    _pPanel->addChild(_pProgressP2);
}

void ScreenLoading::startScreen()
{
    _pPicPreview->setPicture(L"/HOO/GUI/LevelsPreview/" + Core::toString(_levelId, 2) + L".tga");
    _pLabelLevelName->setText(L"LEVEL_" + Core::toString(_levelId, 2));
    _pLabelNameP1->setText(_appContext.login);
    _pLabelNameP2->setText(_opponent.name);

    _pProgressP1->setProgress(0.0f);
    _pProgressP2->setProgress(0.0f);

    _appContext.pGUI->setDialog(_pDialog);

    setProgression(USER_LOADING_0_PERCENT);
    _pDialog->pack();
}

void ScreenLoading::setProgression(HOOUserProtocol::EUserLoadingType p)
{
    _loading = p;
    HOOUserProtocol::UserLoading loading(_loading);
    _appContext.pConnexion->send(loading);
}

void ScreenLoading::setLoadingInfos(int32 levelId, const HOOUserProtocol::OpponentInfos & opponent)
{
    _levelId = levelId;
    _opponent = opponent;
}

void ScreenLoading::endScreen()
{
}

void ScreenLoading::getLevelFile(String & fileName, String & splineName) const
{
    switch(_levelId)
    {
    case 0: 
        fileName = L"/HOO/Levels/TrainingGround/TrainingGround.world";
        splineName = L"";
        break;
    case 1: 
        fileName = L"/HOO/Levels/ViaOlympia/ViaOlympia4-om-pak.world";
        splineName = L"/HOO/Levels/ViaOlympia/intro.spline";
        break;
        //return L"/HOO/Levels/StairwayToAtlantis/StairwayToAtlantisDay3.world";
        //return L"/HOO/Levels/AnankeShore/AnankeShore.world";
    default:
        throw Core::Exception(L"Unknown level id " + Core::toString(_levelId));
        break;
    };
}

void ScreenLoading::update(double elapsed)
{
    switch(_loading)
    {
    case USER_LOADING_0_PERCENT:
        _pProgressP1->setProgress(0.0f);
        break;
    case USER_LOADING_25_PERCENT:
        _pProgressP1->setProgress(0.25f);
        break;
    case USER_LOADING_50_PERCENT:
        _pProgressP1->setProgress(0.50f);
        break;
    case USER_LOADING_75_PERCENT:
        _pProgressP1->setProgress(0.75f);
        break;
    case USER_LOADING_FINISHED:
        _pProgressP1->setProgress(1.0f);
        break;
    }

    switch(_loading)
    {
    case USER_LOADING_0_PERCENT:
        setProgression(USER_LOADING_25_PERCENT);
        break;

    case USER_LOADING_25_PERCENT:
    {
        Ptr<Universe::World> pLevelWorld(new Universe::World(_appContext.pRenderer, 
                                                             _appContext.pVFS, 
                                                             _appContext.pWorldGUI->getRessourcesPool(), 
                                                             _appContext.pAudioDevice));

        String levelFile;
        String splineFile;
        getLevelFile(levelFile, splineFile);
        Ptr<Core::InputStream> pInput = _appContext.pVFS->readFile(levelFile);

        if(pInput == null)
            throw Core::Exception(L"level file not found");

        Core::XMLDocument xmlDocument;
        xmlDocument.loadDocument(*pInput);
        pLevelWorld->importWorldXML(xmlDocument);
        pLevelWorld->startWorldScripts();

        Ptr<InGameMusic> pGameMusic (new InGameMusic (pLevelWorld,
            L"/HOO/Levels/ViaOlympia/Musics/ViaOlympia_A.ogg",
            L"/HOO/Levels/ViaOlympia/Musics/ViaOlympia_B.ogg",
            L"/HOO/Levels/ViaOlympia/Musics/ViaOlympia_C.ogg",  
            L"/HOO/Levels/ViaOlympia/Musics/ViaOlympia_Outro.ogg"));

        pGameMusic->setGain(0.2f);

        _appContext.game.setGameLibrary(_appContext.pGameLib);
        _appContext.game.setLevel(pLevelWorld);
        _appContext.game.setLevelMusic(pGameMusic);
        _appContext.game.setPlayerId(_appContext.playerId);
        _appContext.game.setIntroSpline(splineFile);
        _appContext.pOutGameMusic->stop();

        setProgression(USER_LOADING_50_PERCENT);
        break;
    }

    case USER_LOADING_50_PERCENT:
        _appContext.game.createPlayerAvatar(_appContext.account);
        setProgression(USER_LOADING_75_PERCENT);
        break;

    case USER_LOADING_75_PERCENT:
        _appContext.game.createOpponentAvatar(_opponent);
        setProgression(USER_LOADING_FINISHED);
        break;

    case USER_LOADING_FINISHED:
        break;
    }

    static const HOOUserProtocol::EUserAnswerType FILTER [] = { 
        HOOUserProtocol::USER_LOADING_ANSWER,
        HOOUserProtocol::NULL_ANSWER
    };

    if(_appContext.pConnexion->hasPendingMessage(FILTER))
    {
        Ptr<IUserAnswer> pMessage = _appContext.pConnexion->popMessage(FILTER);

        if(pMessage->getType() == USER_LOADING_ANSWER)
        {
            Ptr<UserLoadingAnswer> pLoading = LM_DEBUG_PTR_CAST<UserLoadingAnswer>(pMessage);

            INF << L"Received loading answer " << HOOUserProtocol::toString(pLoading->getUserLoadingAnswerType()) << L"\n";

            _opponentLoading = pLoading->getUserLoadingAnswerType();

            switch(_opponentLoading)
            {
            case OPPONENT_LOADING_0_PERCENT:
                _pProgressP2->setProgress(0.0f);
                break;
            case OPPONENT_LOADING_25_PERCENT:
                _pProgressP2->setProgress(0.25f);
                break;
            case OPPONENT_LOADING_50_PERCENT:
                _pProgressP2->setProgress(0.50f);
                break;
            case OPPONENT_LOADING_75_PERCENT:
                _pProgressP2->setProgress(0.75f);
                break;
            case OPPONENT_LOADING_FINISHED:
                _pProgressP2->setProgress(1.0f);
                break;
            }
        }
        else
        {
            WAR << L"Unexpected message " << HOOUserProtocol::toString(pMessage->getType()) << L" when loading.\n";
        }
    }

    if(_opponentLoading == OPPONENT_LOADING_FINISHED && _loading == USER_LOADING_FINISHED)
        _appContext.nextState = APPSTATE_INGAME;
}

void ScreenLoading::onEvent(GUI::EEvent ev, GUI::Widget * pWidget)
{
}
