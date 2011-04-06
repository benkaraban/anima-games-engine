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
#include "HighScores.h"

#include <GameTools.h>

namespace ChronoRage
{
//-----------------------------------------------------------------------------
//                  LevelHighScores
//-----------------------------------------------------------------------------
LevelHighScores::LevelHighScores()
{
    for(int32 iLevel = 0; iLevel < 20; ++iLevel)
    {
        addHighScore(iLevel, -1, L"ANIMA GAMES");
        addHighScore(iLevel, -1, L"ANIMA GAMES");
        addHighScore(iLevel, -1, L"ANIMA GAMES");
        addHighScore(iLevel, -1, L"ANIMA GAMES");
        addHighScore(iLevel, -1, L"ANIMA GAMES");
    }
}

bool LevelHighScores::isHighScore(int32 level, int32 msec) const
{
    if(_levelScores[level].size() < 5)
        return true;

    bool returnCode = false;

    int32 iScore = 0;
    while(returnCode == false && iScore < _levelScores[level].size())
    {
        if(_levelScores[level][iScore].score == -1 || msec < _levelScores[level][iScore].score)
        {
            returnCode = true;
        }

        ++iScore;
    }

    return returnCode;
}

void LevelHighScores::addHighScore(int32 level, int32 msec, Core::String name)
{
    if(!isHighScore(level, msec))
        return;

    bool scoreInserted = false;
    int32 iScore = 0;
    while(!scoreInserted && iScore < _levelScores[level].size())
    {
        if(msec != -1 && (_levelScores[level][iScore].score == -1 || msec < _levelScores[level][iScore].score))
        {
            Score score;
            score.name = name;
            score.score = msec;
            _levelScores[level].insert(_levelScores[level].begin() + iScore, score);
            scoreInserted = true;
        }

        ++iScore;
    }

    if(!scoreInserted)
    {
        Score score;
        score.name = name;
        score.score = msec;
        _levelScores[level].insert(_levelScores[level].begin() + iScore, score);
    }

    while(_levelScores[level].size()>5)
    {
        _levelScores[level].erase(_levelScores[level].begin() + 5);
    }    
}

const Core::List<Score> & LevelHighScores::getLevelHighScores(int32 level) const
{
    LM_ASSERT(level >= 0 && level < 20);
    return _levelScores[level];
}


void LevelHighScores::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    for(int32 iLevel=0; iLevel<20; ++iLevel)
    {
        Core::List<Ptr<Core::XMLNode> > pNodes;
        String xmlNodeLabel;
        xmlNodeLabel << L"high-scores.levels.level-"<<Core::toString(iLevel);
        pXMLNode->getNodes(xmlNodeLabel, L"score", pNodes);
        
        for(int32 iScore = 0; iScore < pNodes.size(); ++iScore)
        {
            Core::String name =L"ANIMA GAMES";
            int32 msec = -1;

            pNodes[iScore]->getValue(L"name", name);
            pNodes[iScore]->getValue(L"time", msec);

            addHighScore(iLevel, msec, name);
        }
    }
}

void LevelHighScores::exportXML(const Ptr<Core::XMLNode> & pXMLNode) const
{
    for(int32 iLevel=0; iLevel<20; ++iLevel)
    {
        LM_ASSERT(_levelScores[iLevel].size() == 5);
        for(int32 iScore=0; iScore<_levelScores[iLevel].size(); ++iScore)
        {
            Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"score"));
        
            pNode->setValue(L"name", _levelScores[iLevel][iScore].name);
            pNode->setValue(L"time", _levelScores[iLevel][iScore].score);

            String xmlNodeLabel;
            xmlNodeLabel << L"high-scores.levels.level-"<<Core::toString(iLevel);
            pXMLNode->addNode(xmlNodeLabel, pNode);
        }
    }
}
//-----------------------------------------------------------------------------
//                  SurvivalHighScores
//-----------------------------------------------------------------------------
SurvivalHighScores::SurvivalHighScores()
{
    addHighScore(HSST_NORMAL, -1, L"ANIMA GAMES");
    addHighScore(HSST_NORMAL, -1, L"ANIMA GAMES");
    addHighScore(HSST_NORMAL, -1, L"ANIMA GAMES");
    addHighScore(HSST_NORMAL, -1, L"ANIMA GAMES");
    addHighScore(HSST_NORMAL, -1, L"ANIMA GAMES");

    addHighScore(HSST_HARD, -1, L"ANIMA GAMES");
    addHighScore(HSST_HARD, -1, L"ANIMA GAMES");
    addHighScore(HSST_HARD, -1, L"ANIMA GAMES");
    addHighScore(HSST_HARD, -1, L"ANIMA GAMES");
    addHighScore(HSST_HARD, -1, L"ANIMA GAMES");

    addHighScore(HSST_DODGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_DODGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_DODGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_DODGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_DODGE, -1, L"ANIMA GAMES");

    addHighScore(HSST_REVENGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_REVENGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_REVENGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_REVENGE, -1, L"ANIMA GAMES");
    addHighScore(HSST_REVENGE, -1, L"ANIMA GAMES");
}

bool SurvivalHighScores::isHighScore(EHighScoreSurvivalType type, int32 msec) const
{
    const Core::List<Score> * scores = NULL;

    LM_ASSERT(isValid(type));

    if(type == HSST_NORMAL)
        scores = &_normalScores;
    else if(type == HSST_HARD)
        scores = &_hardScores;
    else if(type == HSST_DODGE)
        scores = &_dodgeScores;
    else if(type == HSST_REVENGE)
        scores = &_revengeScores;

    LM_ASSERT(scores != NULL);

    if(scores->size() < 5)
        return true;

    bool returnCode = false;

    int32 iScore = 0;
    while(returnCode == false && iScore < scores->size())
    {
        if((*scores)[iScore].score == -1 || msec > (*scores)[iScore].score)
        {
            returnCode = true;
        }

        ++iScore;
    }

    return returnCode;
}

void SurvivalHighScores::addHighScore(EHighScoreSurvivalType type, int32 msec, Core::String name)
{
    LM_ASSERT(isValid(type));

    if(!isHighScore(type, msec))
        return;

    Core::List<Score> * scores = NULL;
    if(type == HSST_NORMAL)
        scores = &_normalScores;
    else if(type == HSST_HARD)
        scores = &_hardScores;
    else if(type == HSST_DODGE)
        scores = &_dodgeScores;
    else if(type == HSST_REVENGE)
        scores = &_revengeScores;


    LM_ASSERT(scores != NULL);

    bool scoreInserted = false;
    int32 iScore = 0;
    while(!scoreInserted && iScore < scores->size())
    {
        if(msec != -1 && ((*scores)[iScore].score == -1 || msec > (*scores)[iScore].score))
        {
            Score score;
            score.name = name;
            score.score = msec;
            scores->insert(scores->begin() + iScore, score);
            scoreInserted = true;
        }

        ++iScore;
    }

    if(!scoreInserted)
    {
        Score score;
        score.name = name;
        score.score = msec;
        scores->insert((*scores).begin() + iScore, score);
    }

    while(scores->size()>5)
    {
        scores->erase(scores->begin() + 5);
    }
}

const Core::List<Score> & SurvivalHighScores::getSurvivalHighScores(EHighScoreSurvivalType type) const
{
    LM_ASSERT(isValid(type));

    if(type == HSST_NORMAL)
        return _normalScores;
    else if(type == HSST_HARD)
        return _hardScores;
    else if(type == HSST_DODGE)
        return _dodgeScores;
    else/* if(type == HSST_REVENGE)*/
        return _revengeScores;
}

void SurvivalHighScores::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    Core::List<Ptr<Core::XMLNode> > pNodesNormal;
    String xmlNodeLabel = L"high-scores.survival.normal";
    pXMLNode->getNodes(xmlNodeLabel, L"score", pNodesNormal);
    
    for(int32 iScore = 0; iScore < pNodesNormal.size(); ++iScore)
    {
        Core::String name =L"ANIMA GAMES";
        int32 msec = -1;

        pNodesNormal[iScore]->getValue(L"name", name);
        pNodesNormal[iScore]->getValue(L"time", msec);

        addHighScore(HSST_NORMAL, msec, name);
    }

    Core::List<Ptr<Core::XMLNode> > pNodesHard;
    xmlNodeLabel = L"high-scores.survival.hard";
    pXMLNode->getNodes(xmlNodeLabel, L"score", pNodesHard);
    
    for(int32 iScore = 0; iScore < pNodesHard.size(); ++iScore)
    {
        Core::String name =L"ANIMA GAMES";
        int32 msec = -1;

        pNodesHard[iScore]->getValue(L"name", name);
        pNodesHard[iScore]->getValue(L"time", msec);

        addHighScore(HSST_HARD, msec, name);
    }

    Core::List<Ptr<Core::XMLNode> > pNodesDodge;
    xmlNodeLabel = L"high-scores.survival.dodge";
    pXMLNode->getNodes(xmlNodeLabel, L"score", pNodesDodge);
    
    for(int32 iScore = 0; iScore < pNodesDodge.size(); ++iScore)
    {
        Core::String name =L"ANIMA GAMES";
        int32 msec = -1;

        pNodesDodge[iScore]->getValue(L"name", name);
        pNodesDodge[iScore]->getValue(L"time", msec);

        addHighScore(HSST_DODGE, msec, name);
    }

    Core::List<Ptr<Core::XMLNode> > pNodesRevenge;
    xmlNodeLabel = L"high-scores.survival.revenge";
    pXMLNode->getNodes(xmlNodeLabel, L"score", pNodesRevenge);
    
    for(int32 iScore = 0; iScore < pNodesRevenge.size(); ++iScore)
    {
        Core::String name =L"ANIMA GAMES";
        int32 msec = -1;

        pNodesRevenge[iScore]->getValue(L"name", name);
        pNodesRevenge[iScore]->getValue(L"time", msec);

        addHighScore(HSST_REVENGE, msec, name);
    }
    
}

void SurvivalHighScores::exportXML(const Ptr<Core::XMLNode> & pXMLNode) const
{
    LM_ASSERT(_normalScores.size() == 5);
    for(int32 iScore=0; iScore<_normalScores.size(); ++iScore)
    {
        Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"score"));
    
        pNode->setValue(L"name", _normalScores[iScore].name);
        pNode->setValue(L"time", _normalScores[iScore].score);

        String xmlNodeLabel;
        xmlNodeLabel << L"high-scores.survival.normal";
        pXMLNode->addNode(xmlNodeLabel, pNode);
        
    }

    LM_ASSERT(_hardScores.size() == 5);
    for(int32 iScore=0; iScore<_hardScores.size(); ++iScore)
    {
        Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"score"));
    
        pNode->setValue(L"name", _hardScores[iScore].name);
        pNode->setValue(L"time", _hardScores[iScore].score);

        String xmlNodeLabel;
        xmlNodeLabel << L"high-scores.survival.hard";
        pXMLNode->addNode(xmlNodeLabel, pNode);
     }

    LM_ASSERT(_dodgeScores.size() == 5);
    for(int32 iScore=0; iScore<_dodgeScores.size(); ++iScore)
    {
        Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"score"));
    
        pNode->setValue(L"name", _dodgeScores[iScore].name);
        pNode->setValue(L"time", _dodgeScores[iScore].score);

        String xmlNodeLabel;
        xmlNodeLabel << L"high-scores.survival.dodge";
        pXMLNode->addNode(xmlNodeLabel, pNode);
     }

    LM_ASSERT(_revengeScores.size() == 5);
    for(int32 iScore=0; iScore<_revengeScores.size(); ++iScore)
    {
        Ptr<Core::XMLNode> pNode(new Core::XMLNode(L"score"));
    
        pNode->setValue(L"name", _revengeScores[iScore].name);
        pNode->setValue(L"time", _revengeScores[iScore].score);

        String xmlNodeLabel;
        xmlNodeLabel << L"high-scores.survival.revenge";
        pXMLNode->addNode(xmlNodeLabel, pNode);
     }
}

//-----------------------------------------------------------------------------
//                  HighScores
//-----------------------------------------------------------------------------
HighScores::HighScores(Ptr<LevelRanks> pLevelRanks)
:   _pLevelRanks(pLevelRanks),
    _pixelKilledCount(0)
{
    for(int32 ii=0; ii<EAchievementType_COUNT; ++ii)
        _achievements[ii] = false;
}

bool HighScores::isLevelHighScore(int32 level, int32 msec) const
{
    return _levelHighScore.isHighScore(level, msec);
}

void HighScores::addLevelHighScore(int32 level, int32 msec, Core::String name)
{
    _levelHighScore.addHighScore(level, msec, name);
}

const Core::List<Score> & HighScores::getLevelHighScores(int32 level) const
{
    return _levelHighScore.getLevelHighScores(level);
}

bool HighScores::isLevelAvailable(int32 level) const
{
    bool atLeastOneScore = false;
    const Core::List<Score> & scores = getLevelHighScores(level);
    if(scores[0].score != -1)
        atLeastOneScore = true;
    
    return atLeastOneScore;
}

bool HighScores::isSurvivalHighScore(EHighScoreSurvivalType type, int32 msec) const
{
    return _survivalHighScore.isHighScore(type, msec);
}
void HighScores::addSurvivalHighScore(EHighScoreSurvivalType type, int32 msec, Core::String name)
{
    _survivalHighScore.addHighScore(type, msec, name);
}

const Core::List<Score> & HighScores::getSurvivalHighScores(EHighScoreSurvivalType type) const
{
    return _survivalHighScore.getSurvivalHighScores(type);
}

bool HighScores::isSurvivalModeAvailable() const
{
    bool atLeastOneScore = true;

    int32 iLevel = 0;
    while(atLeastOneScore && iLevel < 10)
    {
        const Core::List<Score> & scores = getLevelHighScores(iLevel);
        if(scores[0].score == -1)
            atLeastOneScore = false;

        ++iLevel;
    }

    return atLeastOneScore;
}

bool HighScores::isSurvivalHardModeAvailable() const
{
    bool survivalHardAvailable = true;

    int32 iLevel = 0;

    while(survivalHardAvailable && GameTools::isLevelValid(iLevel+1))
    {
        const Core::List<Score> & scores = getLevelHighScores(iLevel);
        if(scores[0].score != -1)
        {
            ELevelRank currentRank = _pLevelRanks->getRank(iLevel+1, scores[0].score);
            survivalHardAvailable = isAtLeast(RANK_EXPERT, currentRank);
        }
        else
            survivalHardAvailable = false;

        ++iLevel;
    }

    return survivalHardAvailable;
}

bool HighScores::isDodgeModeAvailable() const
{
    bool dodgeAvailable = true;

    int32 iLevel = 0;

    while(dodgeAvailable && GameTools::isLevelValid(iLevel+1))
    {
        const Core::List<Score> & scores = getLevelHighScores(iLevel);
        if(scores[0].score != -1)
        {
            ELevelRank currentRank = _pLevelRanks->getRank(iLevel+1, scores[0].score);
            dodgeAvailable = isAtLeast(RANK_VETERAN, currentRank);
        }
        else
            dodgeAvailable = false;

        ++iLevel;
    }

    return dodgeAvailable;
}

bool HighScores::isRevengeModeAvailable() const
{
    bool revengeAvailable = true;

    int32 iLevel = 1;

    while(revengeAvailable && GameTools::isLevelValid(iLevel+1))
    {
        const Core::List<Score> & scores = getLevelHighScores(iLevel);
        if(scores[0].score != -1)
        {
            ELevelRank currentRank = _pLevelRanks->getRank(iLevel+1, scores[0].score);
            revengeAvailable = isAtLeast(RANK_ACE, currentRank);
        }
        else
            revengeAvailable = false;

        ++iLevel;
    }

    return revengeAvailable;
}

bool HighScores::isAchievementUnlocked(EAchievementType type)
{
    LM_ASSERT(type >= 0 && type < EAchievementType_COUNT);

    return _achievements[type];
}

void HighScores::unlockAchievement(EAchievementType type)
{
    LM_ASSERT(type >= 0 && type < EAchievementType_COUNT);

    _achievements[type] = true;
}

void HighScores::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    _pixelKilledCount = 0;

    _levelHighScore.importXML(pXMLNode);
    _survivalHighScore.importXML(pXMLNode);
    pXMLNode->getValue(L"high-scores.achievements.pixel-killed-count", _pixelKilledCount);

    for(int32 ii=0; ii<EAchievementType_COUNT; ++ii)
    {
        String nodeName;
        nodeName << L"high-scores.achievements." << ChronoRage::toString((EAchievementType)ii);

        pXMLNode->getValue(nodeName, _achievements[ii]);
    }
}

void HighScores::exportXML(const Ptr<Core::XMLNode> & pXMLNode) const
{
    _levelHighScore.exportXML(pXMLNode);
    _survivalHighScore.exportXML(pXMLNode);
    pXMLNode->setValue(L"high-scores.achievements.pixel-killed-count", _pixelKilledCount);

    for(int32 ii=0; ii<EAchievementType_COUNT; ++ii)
    {
        String nodeName;
        nodeName << L"high-scores.achievements." << ChronoRage::toString((EAchievementType)ii);

        pXMLNode->setValue(nodeName, _achievements[ii]);
    }
}

}