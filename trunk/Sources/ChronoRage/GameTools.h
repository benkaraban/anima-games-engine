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
#ifndef CHRONORAGE_GAMETOOLS_H_
#define CHRONORAGE_GAMETOOLS_H_

#include <Core/Standard.h>

namespace ChronoRage
{

LM_ENUM_21(EAchievementType,    FIRST_STEP,
                                JUST_IN_TIME,
                                MISSION_COMPLETE,
                                VETERAN,
                                EXPERT,
                                ELITE,
                                PERFECT,
                                MARATHON,
                                KILLING_SPREE,
                                PIXEL_KILLER,
                                QUICK_DRAW,
                                MOTHERSHIP_SLAYER,
                                BULLET_TIME,
                                TIME_MACHINE,
                                BARE_HANDED,
                                OBSTINATE,
                                CREDITS_HUNTER,
                                DIE_HARD,
                                IMMORTAL,
                                BULLET_DANCER,
                                SPITEFUL);

const int32 KILLING_SPREE_NUMBER = 500;
const int32 PIXEL_KILLER_NUMBER = 1000000;//1 million
const float BULLET_TIME_DIST = 3.0f;
const int32 TIME_MACHINE_NUMBER = 50;
const int32 MOTHERSHIP_SLAYER_NUMBER = 6;
const int32 DIE_HARD_TIME = 6 * 60 * 1000;
const int32 IMMORTAL_TIME = 6 * 60 * 1000;
const int32 BULLET_DANCER_TIME = 3 * 60 * 1000;
const int32 SPITEFUL_TIME = 10 * 60 * 1000;

class LM_API_CHR GameTools
{
public:
    static bool isLevelValid(int32 levelNumber);
    static Core::String getWaveFile(int32 levelNumber);
    static Core::String getLevelFile(int32 levelNumber);
    static int32 getSecondsToClearLevel(int32 levelNumber);

    static Core::String msecTimerToString(int32 msecTimer, bool allowNegative = false);

    static Core::String getAchievementDictKey(EAchievementType type);
    static Core::String getAchievementHintDictKey(EAchievementType type);
    static Core::String getAchievementPicture(EAchievementType type);
};
}//namespace ChronoRage

#endif/*CHRONORAGE_LEVELSSEQUENCE_H_*/