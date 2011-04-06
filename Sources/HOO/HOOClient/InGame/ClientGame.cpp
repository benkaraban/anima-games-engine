/*
 * Copyright (c) 2010, Anima Games, Benjamin Karaban, Laurent Schneider,
 * J√©r√©mie Comarmond, Didier Colin.
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

#include <Universe/Tools.h>
#include <Universe/Tasks/Wait.h>
#include <Universe/Tasks/SynchroPoint.h>
#include <Universe/Tasks/MoveTrajectory.h>

#include <HOOClient/InGame/ClientGame.h>
#include <HOOClient/Avatar.h>

static const float INTRO_DURATION       = 10.0f;

ClientGame::ClientGame()
{
}

String ClientGame::getSpellScript(int32 spellId)
{
    String script(_pLibrary->getSpell(spellId)->getScript());

    if(script.empty())
        script = L"/HOO/Spells/DefaultSpell/DefaultSpell.lua";

    return script;
}

String ClientGame::getCounterScript(int32 spellId)
{
    String script(_pLibrary->getSpell(spellId)->getScript());

    if(script.empty())
        script = L"/HOO/Spells/DefaultCounter/DefaultCounter.lua";

    return script;
}

String ClientGame::getTrickScript(int32 spellId)
{
    String script(_pLibrary->getSpell(spellId)->getScript());

    if(script.empty())
        script = L"/HOO/Spells/DefaultTrick/DefaultTrick.lua";

    return script;
}

String ClientGame::getRetributionScript(int32 spellId)
{
    String script(_pLibrary->getSpell(spellId)->getScript());

    if(script.empty())
        script = L"/HOO/Spells/DefaultRetribution/DefaultRetribution.lua";

    return script;
}

void ClientGame::setLevel(const Ptr<Universe::World> & pWorld)
{
    _pWorld = pWorld;

    _pCamera = _pWorld->createPerspectiveCamera();
    _pCamera->addChild(_pWorld->getNodeListener());
    //_pCamera->setFOV(f_PI_DIV_2);
    _pCamera->setNearPlane(0.2f);
    _pCamera->setFarPlane(1000.0f);
    _pCamera->addTag(L"CAMERA_SCRIPT");

    _pNodeCamIdle = _pWorld->getFirstNodeWithTag(L"CAM_IDLE");
    _pGamePlay = _pWorld->createNode();
    _pGamePlay->addTag(L"gameplay");
    _pGamePlay->addTag(L"in_game");

    _pWorld->restart();
}

void ClientGame::setLevelMusic(const Ptr<InGameMusic> & pGameMusic)
{
    _pGameMusic = pGameMusic;
}

void ClientGame::setIntroSpline(const String & introSpline)
{
    _introSpline = introSpline;
}

void ClientGame::createPlayerAvatar(const HOOUserProtocol::UserAccountInfos & account)
{
    AvatarDesc avatar;
    avatar.characterId = account.character;
    avatar.equipment = account.equipedStuff;

    _pAvatarP1 = createAvatar(*_pWorld, avatar);

    Ptr<Universe::Node> pNodeStart = _pWorld->getFirstNodeWithTag(L"PlayersPosition");

    Ptr<Universe::Node> pPivot = _pWorld->createNode();
    pPivot->setLocalScale(Core::Vector3f(1.0f, 1.0f, 1.0f));
    pPivot->addTag(L"pivot");

    _pAvatarP1->setLocalPosition(Core::Vector3f(0.0f, 0.0f, -12.0f));
    _pAvatarP1->setLocalDirection(Core::Vector3f(0.0f, 0.0f,  1.0f));
    _pAvatarP1->addTag(L"player");
    _pAvatarP1->addChild(pPivot);

    pNodeStart->addChild(_pAvatarP1);
}

void ClientGame::createOpponentAvatar(const HOOUserProtocol::OpponentInfos & opponent)
{
    AvatarDesc avatar;
    avatar.characterId = opponent.character;
    avatar.equipment = opponent.equipedStuff;

    _pAvatarP2 = createAvatar(*_pWorld, avatar);

    Ptr<Universe::Node> pNodeStart = _pWorld->getFirstNodeWithTag(L"PlayersPosition");

    Ptr<Universe::Node> pPivot = _pWorld->createNode();
    pPivot->setLocalScale(Core::Vector3f(-1.0f, 1.0f, 1.0f));
    pPivot->addTag(L"pivot");

    _pAvatarP2->setLocalPosition(Core::Vector3f(0.0f, 0.0f,  12.0f));
    _pAvatarP2->setLocalDirection(Core::Vector3f(0.0f, 0.0f, -1.0f));
    _pAvatarP2->addTag(L"opponent");
    _pAvatarP2->addChild(pPivot);

    pNodeStart->addChild(_pAvatarP2);

    _opponentName = opponent.name;
}

void ClientGame::setCamPosStart()
{
    if(_introSpline == String::EMPTY)
        setCamPosIdle();
    else
    {
        Ptr<Universe::Node> pNodeStart = _pWorld->getFirstNodeWithTag(L"PlayersPosition");
        Ptr<Core::Trajectory> pSpline(_pWorld->getRessourcesPool()->getSpline(_introSpline));

        Core::Matrix4f mat;
        Universe::getCleanMatrix(mat, *pNodeStart, *pSpline, 0.0f);
        _pCamera->setLocalMatrix(mat);
    }
}

void ClientGame::setCamPosIdle()
{
    _pCamera->setWorldPosition(_pNodeCamIdle->getWorldPosition());
    _pCamera->setWorldDirection(_pNodeCamIdle->getWorldDirection());
}

void ClientGame::cleanup(Ptr<Universe::ITask> & pTask)
{
    if(pTask != null)
    {
        pTask->kill();
        pTask = null;
    }
}

void ClientGame::cleanupWillScripts()
{
    cleanup(_pTaskWillContest);
}

void ClientGame::cleanupSpellScripts()
{
    cleanup(_pTaskSpell);
    cleanup(_pTaskCounter);
    cleanup(_pTaskTrick);
}

void ClientGame::freeLevel()
{
    cleanupWillScripts();
    cleanupSpellScripts();

    _pCamera = null;
    
    _pAvatarP1 = null;
    _pAvatarP2 = null;
    
    _pNodeCamIdle = null;
    _pGamePlay = null;

    _pWorld = null;
    _pGameMusic = null;
}

Ptr<Universe::ITask> ClientGame::introScript()
{
    INF << L"Playing intro script...\n";

    if(_introSpline == String::EMPTY)
        return registerTask(new Universe::TaskWait(0.0f));
    else
    {
        Ptr<Universe::Node> pNodeStart = _pWorld->getFirstNodeWithTag(L"PlayersPosition");
        const Core::Trajectory & traj = *_pWorld->getRessourcesPool()->getSpline(_introSpline);
        return registerTask(new Universe::TaskMoveTrajectory(_pCamera, _pAvatarP1, traj, INTRO_DURATION / traj.getDuration()));
    }
}

Ptr<Universe::ITask> ClientGame::gameFinished(int32 winnerId)
{
    INF << L"Playing victory script...\n";

    _casterId = winnerId;
    cleanTags();
    setCasterTags(_casterId);

    _pGameMusic->engageOutro();
    return _pWorld->runScript(L"/HOO/Spells/Common/Victory.lua");
}

Ptr<Universe::ITask> ClientGame::willContest(int32 manaCount, int32 manaSphere)
{
    INF << L"Playing will contest script...\n";
    _pGamePlay->removeTag(L"will_contest_finished");
    cleanupWillScripts();
    switch(manaSphere)
    {
    case 0: _pGamePlay->addTag(L"mana_energy"); break;
    case 1: _pGamePlay->addTag(L"mana_spirit"); break;
    case 2: _pGamePlay->addTag(L"mana_life"); break;
    }
    return _pTaskWillContest = _pWorld->runScript(L"/HOO/Spells/Common/WillContest.lua");
}

Ptr<Universe::ITask> ClientGame::willResolve(int32 bidWinnerId)
{
    INF << L"Playing will resolve script...\n";

    if(bidWinnerId == _playerId || bidWinnerId == 0) // =0 => ÈgalitÈ
        _pGamePlay->addTag(L"player_gain_mana");
    else
        _pGamePlay->removeTag(L"player_gain_mana");

    _pGamePlay->addTag(L"will_contest_finished");

    return _pTaskWillContest;
}

Ptr<Universe::ITask> ClientGame::playerTurn(int32 playerId)
{
    INF << L"Playing player turn script...\n";
    return _pWorld->runScript(L"/HOO/Spells/Common/PlayerTurn.lua");
}

Ptr<Universe::ITask> ClientGame::playerCastSpell(int32 playerId, int32 spellId)
{
    INF << L"Playing player cast spell script...\n";

    cleanupSpellScripts();

    _casterId = playerId;
    cleanTags();
    setCasterTags(_casterId);

    return _pTaskSpell = _pWorld->runScript(getSpellScript(spellId));
}

Ptr<Universe::ITask> ClientGame::playerSpellHit()
{
    INF << L"Playing player spell hit script...\n";
    _pGamePlay->addTag(L"incant_finished");
    return _pTaskSpell;
}

Ptr<Universe::ITask> ClientGame::playerSpellHitAfterTrick(int32 trickId)
{
    INF << L"Playing player spell hit after trick script...\n";

    _pGamePlay->addTag(L"spell_interrupted");

    invertCasterTags();
    _pTaskTrick = _pWorld->runScript(getTrickScript(trickId));

    return Ptr<Universe::ITask>(new Universe::TaskSynchroPoint(_pTaskSpell, _pTaskTrick));
}

Ptr<Universe::ITask> ClientGame::playerSpellCountered(int32 counterId)
{
    INF << L"Playing player spell countered script...\n";

    _pGamePlay->addTag(L"spell_countered");
    _pGamePlay->addTag(L"incant_finished");

    invertCasterTags();
    _pTaskCounter = _pWorld->runScript(getCounterScript(counterId));

    return Ptr<Universe::ITask>(new Universe::TaskSynchroPoint(_pTaskSpell, _pTaskCounter));
}

Ptr<Universe::ITask> ClientGame::playerCastTrick(int32 playerId, int32 trickId)
{
    INF << L"Playing player cast trick...\n";

    _casterId = playerId;
    cleanTags();
    setCasterTags(_casterId);

    return _pTaskSpell = _pWorld->runScript(getTrickScript(trickId));
}

Ptr<Universe::ITask> ClientGame::playerTrickHit()
{
    INF << L"Playing player trick hit...\n";
    _pGamePlay->addTag(L"incant_finished");
    return _pTaskSpell;
}

Ptr<Universe::ITask> ClientGame::playerTrickHitAfterTrick(int32 trickId)
{
    INF << L"Playing player trick hit after trick...\n";

    invertCasterTags();
    _pTaskTrick = _pWorld->runScript(getTrickScript(trickId));

    return Ptr<Universe::ITask>(new Universe::TaskSynchroPoint(_pTaskSpell, _pTaskTrick));
}

Ptr<Universe::ITask> ClientGame::playerCastRetribution(int32 playerId, int32 retribId)
{
    INF << L"Playing player cast retribution...\n";

    _casterId = playerId;
    cleanTags();
    setCasterTags(_casterId);

    return _pTaskSpell = _pWorld->runScript(getRetributionScript(retribId));
}

Ptr<Universe::ITask> ClientGame::playerRetributionHit()
{
    INF << L"Playing player retribution hit...\n";
    _pGamePlay->addTag(L"incant_finished");
    return _pTaskSpell;
}

Ptr<Universe::ITask> ClientGame::playerRetributionHitAfterTrick(int32 trickId)
{
    INF << L"Playing player retribution hit after trick...\n";

    invertCasterTags();
    _pTaskTrick = _pWorld->runScript(getTrickScript(trickId));

    return Ptr<Universe::ITask>(new Universe::TaskSynchroPoint(_pTaskSpell, _pTaskTrick));
}

Ptr<Universe::ITask> ClientGame::playerPass(int32 playerId)
{
    INF << L"Playing player pass...\n";

    _casterId = playerId;
    cleanTags();
    setCasterTags(_casterId);

    return _pTaskSpell = _pWorld->runScript(L"/HOO/Spells/Common/Pass.lua");
}

Ptr<Universe::ITask> ClientGame::playerPassFinish()
{
    INF << L"Playing player pass finish...\n";
    _pGamePlay->addTag(L"incant_finished");
    return _pTaskSpell;
}

Ptr<Universe::ITask> ClientGame::playerPassFinishAfterTrick(int32 trickId)
{
    INF << L"Playing player pass finish after trick...\n";

    invertCasterTags();
    _pTaskTrick = _pWorld->runScript(getTrickScript(trickId));

    return Ptr<Universe::ITask>(new Universe::TaskSynchroPoint(_pTaskSpell, _pTaskTrick));
}

Ptr<Universe::ITask> ClientGame::registerTask(Universe::ITask * pTask)
{
    Ptr<Universe::ITask> pTask2(pTask);
    _pWorld->registerTask(pTask2);
    return pTask2;
}

void ClientGame::cleanTags()
{
    _pGamePlay->removeTag(L"first_incant");
    _pGamePlay->removeTag(L"incant_finished");
    _pGamePlay->removeTag(L"spell_countered");
    _pGamePlay->removeTag(L"spell_ready_for_counter");
    _pGamePlay->removeTag(L"counter_ready_for_impact");
    _pGamePlay->removeTag(L"spell_impact");
    _pGamePlay->removeTag(L"spell_interrupted");
    _pGamePlay->removeTag(L"counter_status_update");
    _pGamePlay->removeTag(L"trick_status_update");
}

void ClientGame::setCasterTags(int32 id)
{
    _pAvatarP1->removeTag(L"caster");
    _pAvatarP2->removeTag(L"caster");

    _pAvatarP1->removeTag(L"target");
    _pAvatarP2->removeTag(L"target");

    if(id == _playerId)
    {
        _pAvatarP1->addTag(L"caster");
        _pAvatarP2->addTag(L"target");
    }
    else
    {
        _pAvatarP1->addTag(L"target");
        _pAvatarP2->addTag(L"caster");
    }
}

bool ClientGame::hasTag(const String & tag)
{
    return _pGamePlay->hasTag(tag);
}

void ClientGame::invertCasterTags()
{
    if(_casterId == 1)
        setCasterTags(2);
    else
        setCasterTags(1);
}

void ClientGame::statutsUpdate(const HOOUserProtocol::GameStatus &status)
{
    if (_pGameMusic->getMusicState() == IN_GAME_MUSIC_BEGIN)
    {
        if (status.player.health<50 || status.opponent.health<50)
        {
            _pGameMusic->engageFinalState();
        }
    }
}