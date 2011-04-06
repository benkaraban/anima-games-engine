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
#include "GameTools.h"

namespace ChronoRage
{

bool GameTools::isLevelValid(int32 levelNumber)
{
    bool returnCode = false;

    if((levelNumber > 0 && levelNumber <=20) || levelNumber == -1)
        returnCode = true;

    return returnCode;
}


Core::String GameTools::getWaveFile(int32 levelNumber)
{
    if(!isLevelValid(levelNumber))
    {
        Core::String message;
        message << L"Invalid level number to get wave file : " << levelNumber;
        throw Core::Exception(message);
    }

    Core::String levelWaveFile = L"/ChronoRageLevels/Waves/waves-level01.xml";

    if(levelNumber == -1)
        levelWaveFile = L"/ChronoRageLevels/Waves/waves-credits.xml";
    else
        levelWaveFile = L"/ChronoRageLevels/Waves/waves-level" + Core::toString(levelNumber, 2) + L".xml";


    return levelWaveFile;
}

Core::String GameTools::getLevelFile(int32 levelNumber)
{
    if(!isLevelValid(levelNumber))
    {
        Core::String message;
        message << L"Invalid level number to get level file : " << levelNumber;
        throw Core::Exception(message);
    }

    Core::String levelWaveFile = L"/ChronoRageLevels/level01.xml";

    if(levelNumber == -1)
        levelWaveFile = L"/ChronoRageLevels/credits.xml";
    else
        levelWaveFile = L"/ChronoRageLevels/level" + Core::toString(levelNumber, 2) + L".xml";

    return levelWaveFile;
}

int32 GameTools::getSecondsToClearLevel(int32 levelNumber)
{
    switch(levelNumber)
    {
    default: return 420;
    case 17: return 480;
    case 20: return 840;
    }
}

Core::String GameTools::msecTimerToString(int32 msecTimer, bool allowNegative)
{
    bool negativeValue = false;

    if(msecTimer < 0)
    {
        if(!allowNegative)
        {
            return L"--:--.---";
        }
        else
        {
            negativeValue = true;
            msecTimer *= -1;
        }
    }

    int32 min = 0;
    int32 sec = 0;
    int32 msec = 0;

    msec = msecTimer % 1000;
    sec = msecTimer / 1000;
    min = sec / 60;
    sec = sec % 60;

    Core::String label;
    Core::String negLabel;
    Core::String minLabel;
    Core::String secLabel;
    Core::String msecLabel;
    
    if(min < 10)
        minLabel << L"0";
    minLabel << Core::toString(min);

    if(sec < 10)
        secLabel << L"0";
    secLabel << Core::toString(sec);

    if(msec < 100)
        msecLabel << L"0";
    if(msec < 10)
        msecLabel << L"0";
    msecLabel << Core::toString(msec);

    if(negativeValue)
        negLabel = L"-";

    label << negLabel << minLabel << L":" << secLabel << L"." << msecLabel;

    return label;
}

Core::String GameTools::getAchievementDictKey(EAchievementType type)
{
    Core::String returnCode = L"achievement-just-in-time";
    switch(type)
    {
    case FIRST_STEP:
        returnCode = L"achievement-first-step";
        break;
    case JUST_IN_TIME:
        returnCode = L"achievement-just-in-time";
        break;
    case MISSION_COMPLETE:
        returnCode = L"achievement-mission-complete";
        break;
    case VETERAN:
        returnCode = L"achievement-veteran";
        break;
    case EXPERT:
        returnCode = L"achievement-expert";
        break;
    case ELITE:
        returnCode = L"achievement-elite";
        break;
    case PERFECT:
        returnCode = L"achievement-perfect";
        break;
    case MARATHON:
        returnCode = L"achievement-marathon";
        break;
    case KILLING_SPREE:
        returnCode = L"achievement-killing-spree";
        break;
    case PIXEL_KILLER:
        returnCode = L"achievement-pixel-killer";
        break;
    case QUICK_DRAW:
        returnCode = L"achievement-quick-draw";
        break;
    case MOTHERSHIP_SLAYER:
        returnCode = L"achievement-mothership-slayer";
        break;
    case BULLET_TIME:
        returnCode = L"achievement-bullet-time";
        break;
    case TIME_MACHINE:
        returnCode = L"achievement-time-machine";
        break;
    case BARE_HANDED:
        returnCode = L"achievement-bare-handed";
        break;
    case OBSTINATE:
        returnCode = L"achievement-obstinate";
        break;
    case CREDITS_HUNTER:
        returnCode = L"achievement-credits-hunter";
        break;
    case DIE_HARD:
        returnCode = L"achievement-die-hard";
        break;
    case IMMORTAL:
        returnCode = L"achievement-immortal";
        break;
    case BULLET_DANCER:
        returnCode = L"achievement-bullet-dancer";
        break;
    case SPITEFUL:
        returnCode = L"achievement-spiteful";
        break;
    }

    return returnCode;
}

Core::String GameTools::getAchievementHintDictKey(EAchievementType type)
{
    Core::String returnCode = L"achievement-just-in-time-hint";
    switch(type)
    {
    case FIRST_STEP:
        returnCode = L"achievement-first-step-hint";
        break;
    case JUST_IN_TIME:
        returnCode = L"achievement-just-in-time-hint";
        break;
    case MISSION_COMPLETE:
        returnCode = L"achievement-mission-complete-hint";
        break;
    case VETERAN:
        returnCode = L"achievement-veteran-hint";
        break;
    case EXPERT:
        returnCode = L"achievement-expert-hint";
        break;
    case ELITE:
        returnCode = L"achievement-elite-hint";
        break;
    case PERFECT:
        returnCode = L"achievement-perfect-hint";
        break;
    case MARATHON:
        returnCode = L"achievement-marathon-hint";
        break;
    case KILLING_SPREE:
        returnCode = L"achievement-killing-spree-hint";
        break;
    case PIXEL_KILLER:
        returnCode = L"achievement-pixel-killer-hint";
        break;
    case QUICK_DRAW:
        returnCode = L"achievement-quick-draw-hint";
        break;
    case MOTHERSHIP_SLAYER:
        returnCode = L"achievement-mothership-slayer-hint";
        break;
    case BULLET_TIME:
        returnCode = L"achievement-bullet-time-hint";
        break;
    case TIME_MACHINE:
        returnCode = L"achievement-time-machine-hint";
        break;
    case BARE_HANDED:
        returnCode = L"achievement-bare-handed-hint";
        break;
    case OBSTINATE:
        returnCode = L"achievement-obstinate-hint";
        break;
    case CREDITS_HUNTER:
        returnCode = L"achievement-credits-hunter-hint";
        break;
    case DIE_HARD:
        returnCode = L"achievement-die-hard-hint";
        break;
    case IMMORTAL:
        returnCode = L"achievement-immortal-hint";
        break;
    case BULLET_DANCER:
        returnCode = L"achievement-bullet-dancer-hint";
        break;
    case SPITEFUL:
        returnCode = L"achievement-spiteful-hint";
        break;
    }

    return returnCode;
}

Core::String GameTools::getAchievementPicture(EAchievementType type)
{
    //return L"/ChronoRageData/GUI/life-icon.tga";
    Core::String returnCode = L"/ChronoRageData/GUI/achievement-first-step.tga";

    switch(type)
    {
    case FIRST_STEP:
        returnCode = L"/ChronoRageData/GUI/achievement-first-step.tga";
        break;
    case JUST_IN_TIME:
        returnCode = L"/ChronoRageData/GUI/achievement-just-in-time.tga";
        break;
    case MISSION_COMPLETE:
        returnCode = L"/ChronoRageData/GUI/achievement-mission-complete.tga";
        break;
    case VETERAN:
        returnCode = L"/ChronoRageData/GUI/achievement-veteran.tga";
        break;
    case EXPERT:
        returnCode = L"/ChronoRageData/GUI/achievement-expert.tga";
        break;
    case ELITE:
        returnCode = L"/ChronoRageData/GUI/achievement-elite.tga";
        break;
    case PERFECT:
        returnCode = L"/ChronoRageData/GUI/achievement-perfect.tga";
        break;
    case MARATHON:
        returnCode = L"/ChronoRageData/GUI/achievement-marathon.tga";
        break;
    case KILLING_SPREE:
        returnCode = L"/ChronoRageData/GUI/achievement-killing-spree.tga";
        break;
    case PIXEL_KILLER:
        returnCode = L"/ChronoRageData/GUI/achievement-pixel-killer.tga";
        break;
    case QUICK_DRAW:
        returnCode = L"/ChronoRageData/GUI/achievement-quick-draw.tga";
        break;
    case MOTHERSHIP_SLAYER:
        returnCode = L"/ChronoRageData/GUI/achievement-mothership-slayer.tga";
        break;
    case BULLET_TIME:
        returnCode = L"/ChronoRageData/GUI/achievement-bullet-time.tga";
        break;
    case TIME_MACHINE:
        returnCode = L"/ChronoRageData/GUI/achievement-time-machine.tga";
        break;
    case BARE_HANDED:
        returnCode = L"/ChronoRageData/GUI/achievement-bare-handed.tga";
        break;
    case OBSTINATE:
        returnCode = L"/ChronoRageData/GUI/achievement-obstinate.tga";
        break;
    case CREDITS_HUNTER:
        returnCode = L"/ChronoRageData/GUI/achievement-credits-hunter.tga";
        break;
    case DIE_HARD:
        returnCode = L"/ChronoRageData/GUI/achievement-die-hard.tga";
        break;
    case IMMORTAL:
        returnCode = L"/ChronoRageData/GUI/achievement-immortal.tga";
        break;
    case BULLET_DANCER:
        returnCode = L"/ChronoRageData/GUI/achievement-bullet-dancer.tga";
        break;
    case SPITEFUL:
        returnCode = L"/ChronoRageData/GUI/achievement-spiteful.tga";
        break;
    }

    return returnCode;
}

}//namespace ChronoRage