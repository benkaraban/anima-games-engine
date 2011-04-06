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
#include "LevelRanks.h"

namespace ChronoRage
{

String rankToString(ELevelRank rank)
{
    String returnCode = L"rookie";

    switch(rank)
    {
    case RANK_ACE:
        returnCode = L"ace";
        break;
    case RANK_EXPERT:
        returnCode = L"expert";
        break;
    case RANK_VETERAN:
        returnCode = L"veteran";
        break;
    case RANK_ROOKIE:
        returnCode = L"rookie";
        break;
    }

    return returnCode;
}

bool isAtLeast(ELevelRank expectedRank, ELevelRank currentRank)
{
    bool returnCode;
    switch(expectedRank)
    {
    case RANK_ACE:
        returnCode = (currentRank == RANK_ACE);
        break;
    case RANK_EXPERT:
        returnCode = (currentRank == RANK_ACE || currentRank == RANK_EXPERT);
        break;
    case RANK_VETERAN:
        returnCode = (currentRank == RANK_ACE || currentRank == RANK_EXPERT || currentRank == RANK_VETERAN);
        break;
    case RANK_ROOKIE:
        returnCode = (currentRank == RANK_ACE || currentRank == RANK_EXPERT || currentRank == RANK_VETERAN || currentRank == RANK_ROOKIE);
        break;
    }

    return returnCode;
}

ELevelRank LevelRanks::getRank(int32 level, int32 time)
{
    ELevelRank rank = RANK_ROOKIE;

    if(time <= 0)
    {
        return RANK_ROOKIE;
    }

    LM_ASSERT(level >= 1 && level <= 20);
    
    if(time <= _levelRanks[level-1].aceTime)
    {
        rank = RANK_ACE;
    }
    else if(time <= _levelRanks[level-1].expertTime)
    {
        rank = RANK_EXPERT;
    }
    else if(time <= _levelRanks[level-1].veteranTime)
    {
        rank = RANK_VETERAN;
    }

    return rank;
}

int32 LevelRanks::getTime(int32 level, ELevelRank rank)
{
    int32 time = -1;
    LM_ASSERT(level >= 1 && level <= 20);
    switch(rank)
    {
    case RANK_ACE:
        time = _levelRanks[level-1].aceTime;
        break;
    case RANK_EXPERT:
        time = _levelRanks[level-1].expertTime;
        break;
    case RANK_VETERAN:
        time = _levelRanks[level-1].veteranTime;
        break;
    }

    return time;
}

void LevelRanks::importXML(const Ptr<Core::XMLNode> & pXMLNode)
{
    Core::List<Ptr<Core::XMLNode> > pRankNodes;
    String xmlNodeLabel = L"levels";
    pXMLNode->getNodes(xmlNodeLabel, L"level", pRankNodes);
    
    for(int32 iScore = 0; iScore < pRankNodes.size(); ++iScore)
    {
        int32 aceTime = 300000;
        int32 expertTime = 300000;
        int32 veteranTime = 300000;

        int32 id = -1;

        pRankNodes[iScore]->getAttribute(L"", L"id", id);
        pRankNodes[iScore]->getValue(L"ace", aceTime);
        pRankNodes[iScore]->getValue(L"expert", expertTime);
        pRankNodes[iScore]->getValue(L"veteran", veteranTime);

        LM_ASSERT(id >= 1 && id <= 20);
        _levelRanks[id-1].aceTime = aceTime;
        _levelRanks[id-1].expertTime = expertTime;
        _levelRanks[id-1].veteranTime = veteranTime;
    }
}

}//namespace ChronoRage