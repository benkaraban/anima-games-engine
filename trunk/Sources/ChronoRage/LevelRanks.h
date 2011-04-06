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
#ifndef CHRONORAGE_LEVELRANKS_H_
#define CHRONORAGE_LEVELRANKS_H_

#include <Core/Standard.h>
#include <Core/XML/XMLNode.h>

namespace ChronoRage
{

LM_ENUM_4(ELevelRank,   RANK_ACE,
                        RANK_EXPERT,
                        RANK_VETERAN,
                        RANK_ROOKIE);

String rankToString(ELevelRank rank);

bool isAtLeast(ELevelRank expectedRank, ELevelRank currentRank);

struct LM_API_CHR Ranks : public Core::Object
{
    int32 aceTime;
    int32 expertTime;
    int32 veteranTime;
};

class LM_API_CHR LevelRanks : public Core::Object
{
public:
    LevelRanks(){}
    virtual ~LevelRanks(){}

    ELevelRank getRank(int32 level, int32 time);
    int32 getTime(int32 level, ELevelRank rank);
    
    void importXML(const Ptr<Core::XMLNode> & pXMLNode);

protected:
    Ranks   _levelRanks[20];
};

}//namespace ChronoRage

#endif/*CHRONORAGE_LEVELRANKS_H_*/