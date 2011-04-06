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
#ifndef CHRONORAGE_HIGHSCORES_H_
#define CHRONORAGE_HIGHSCORES_H_

#include <Core/Standard.h>
#include <Core/List.h>
#include <Core/XML/XMLNode.h>

#include <LevelRanks.h>
#include <GameTools.h>

namespace ChronoRage
{

struct LM_API_CHR Score : public Core::Object
{
    String  name;
    int32   score;
};

class LM_API_CHR LevelHighScores : public Core::Object
{
public:
    LevelHighScores();
    virtual ~LevelHighScores(){}

    bool isHighScore(int32 level, int32 msec) const;
    void addHighScore(int32 level, int32 msec, Core::String name);

    const Core::List<Score> & getLevelHighScores(int32 level) const;

    void importXML(const Ptr<Core::XMLNode> & pXMLNode);
    void exportXML(const Ptr<Core::XMLNode> & pXMLNode) const;

protected:
    Core::List<Score> _levelScores[20];
};

LM_ENUM_4(EHighScoreSurvivalType,   HSST_NORMAL,
                                    HSST_HARD,
                                    HSST_DODGE,
                                    HSST_REVENGE);

class LM_API_CHR SurvivalHighScores : public Core::Object
{
public:
    SurvivalHighScores();
    virtual ~SurvivalHighScores(){}

    bool isHighScore(EHighScoreSurvivalType type, int32 msec) const;
    void addHighScore(EHighScoreSurvivalType type, int32 msec, Core::String name);

    const Core::List<Score> & getSurvivalHighScores(EHighScoreSurvivalType type) const;

    void importXML(const Ptr<Core::XMLNode> & pXMLNode);
    void exportXML(const Ptr<Core::XMLNode> & pXMLNode) const;

protected:
    Core::List<Score> _normalScores;
    Core::List<Score> _hardScores;
    Core::List<Score> _dodgeScores;
    Core::List<Score> _revengeScores;
};

class LM_API_CHR HighScores : public Core::Object
{
public:
    HighScores(Ptr<LevelRanks> pLevelRanks);
    virtual ~HighScores(){}

    bool isLevelHighScore(int32 level, int32 msec) const;
    void addLevelHighScore(int32 level, int32 msec, Core::String name);

    const Core::List<Score> & getLevelHighScores(int32 level) const;

    bool isLevelAvailable(int32 level) const;

    bool isSurvivalHighScore(EHighScoreSurvivalType type, int32 msec) const;
    void addSurvivalHighScore(EHighScoreSurvivalType type, int32 msec, Core::String name);

    const Core::List<Score> & getSurvivalHighScores(EHighScoreSurvivalType type) const;

    bool isSurvivalModeAvailable() const;
    bool isSurvivalHardModeAvailable() const;
    bool isDodgeModeAvailable() const;
    bool isRevengeModeAvailable() const;

    bool isAchievementUnlocked(EAchievementType type);
    void unlockAchievement(EAchievementType type);

    void setPixelKilledCount(int32 count) { _pixelKilledCount = count; }
    int32 getPixelKilledCount() const { return _pixelKilledCount; }

    void importXML(const Ptr<Core::XMLNode> & pXMLNode);
    void exportXML(const Ptr<Core::XMLNode> & pXMLNode) const;

private:
    Ptr<LevelRanks>     _pLevelRanks;
    LevelHighScores     _levelHighScore;
    SurvivalHighScores  _survivalHighScore;
    bool                _achievements[EAchievementType_COUNT];
    int32               _pixelKilledCount;
};

}//namespace ChronoRage

#endif/*CHRONORAGE_HIGHSCORES_H_*/