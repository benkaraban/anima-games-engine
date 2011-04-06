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
#ifndef HOOCLIENT_CLIENTGAME_H_
#define HOOCLIENT_CLIENTGAME_H_

#include <Universe/World.h>
#include <GameEngine/GameLibrary.h>
#include <HOOUserProtocol/GameMessage/GameData.h>
#include <HOOUserProtocol/UserAccountInfos.h>
#include <HOOClient/InGame/InGameMusic.h>

struct ClientGame : public Core::Object
{
    ClientGame();

    void setPlayerId(int32 playerId) { _playerId = playerId; };
    void setLevel(const Ptr<Universe::World> & pWorld);
    void setLevelMusic(const Ptr<InGameMusic> & pGameMusic);
    void setIntroSpline(const String & introSpline);
    void setGameLibrary(const Ptr<GameEngine::GameLibrary> & pLibrary) { _pLibrary = pLibrary; }

    void createPlayerAvatar(const HOOUserProtocol::UserAccountInfos & account);
    void createOpponentAvatar(const HOOUserProtocol::OpponentInfos & account);

    const Core::Set<String>& getGameplayTags() const {return _pGamePlay->getTags();}

    void setCamPosStart();
    void setCamPosIdle();
    const String & getOpponentName() const { return _opponentName; }

    bool hasTag(const String & tag);

    void freeLevel();

    const Ptr<Universe::World> & getWorld() const { return _pWorld; }
    const Ptr<Universe::NodeCamera> & getCamera() const { return _pCamera; }

    ///
    Ptr<Universe::ITask> introScript();
    Ptr<Universe::ITask> gameFinished(int32 winnerId); // Task finished à la fin de l'animation

    Ptr<Universe::ITask> willContest(int32 manaCount, int32 manaSphere); // Task finished immédiatement
    Ptr<Universe::ITask> willResolve(int32 bidWinnerId); // Task finished quand animation terminée
    Ptr<Universe::ITask> playerTurn(int32 playerId); // Task finished quand caméra positionnée
    
    Ptr<Universe::ITask> playerCastSpell(int32 playerId, int32 spellId); // Task finished immédiatement
    Ptr<Universe::ITask> playerSpellHit(); // Task finished à la fin de l'animation
    Ptr<Universe::ITask> playerSpellHitAfterTrick(int32 trickId); // Task finished à la fin de l'animation
    Ptr<Universe::ITask> playerSpellCountered(int32 counterId); // Task finished à la fin de l'animation

    Ptr<Universe::ITask> playerCastTrick(int32 playerId, int32 trickId); // Task finished immédiatement
    Ptr<Universe::ITask> playerTrickHit(); // Task finished à la fin de l'animation
    Ptr<Universe::ITask> playerTrickHitAfterTrick(int32 trickId); // Task finished à la fin de l'animation

    Ptr<Universe::ITask> playerCastRetribution(int32 playerId, int32 retribId); // Task finished immédiatement
    Ptr<Universe::ITask> playerRetributionHit(); // Task finished à la fin de l'animation
    Ptr<Universe::ITask> playerRetributionHitAfterTrick(int32 trickId); // Task finished à la fin de l'animation

    Ptr<Universe::ITask> playerPass(int32 playerId); // Task finished immédiatement
    Ptr<Universe::ITask> playerPassFinish(); // Task finished à la fin de l'animation
    Ptr<Universe::ITask> playerPassFinishAfterTrick(int32 trickId); // Task finished à la fin de l'animation

    void statutsUpdate(const HOOUserProtocol::GameStatus &status);
    void startMusic()           {_pGameMusic->start();}


protected:
    void cleanup(Ptr<Universe::ITask> & pTask);
    void cleanupWillScripts();
    void cleanupSpellScripts();
    void cleanTags();
    void setCasterTags(int32 id);
    void invertCasterTags();
    Ptr<Universe::ITask> registerTask(Universe::ITask * pTask);
    String getSpellScript(int32 spellId);
    String getCounterScript(int32 spellId);
    String getTrickScript(int32 spellId);
    String getRetributionScript(int32 spellId);

    String                          _opponentName;

    Ptr<Universe::World>            _pWorld;
    Ptr<InGameMusic>                _pGameMusic;
    Ptr<Universe::NodeCamera>       _pCamera;
    Ptr<GameEngine::GameLibrary>    _pLibrary;
    String                          _introSpline;
    int32                           _playerId;
    
    Ptr<Universe::NodeSkeleton>     _pAvatarP1;
    Ptr<Universe::NodeSkeleton>     _pAvatarP2;
    
    Ptr<Universe::Node>             _pNodeCamIdle;
    Ptr<Universe::Node>             _pGamePlay;

    int32                           _casterId;
    Ptr<Universe::ITask>            _pTaskWillContest;
    Ptr<Universe::ITask>            _pTaskSpell;
    Ptr<Universe::ITask>            _pTaskCounter;
    Ptr<Universe::ITask>            _pTaskTrick;
};

#endif

